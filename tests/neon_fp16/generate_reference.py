# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Exact binary16 oracle for native AArch64 arithmetic, using Fraction only.

The eight configurations are [RNE, RUP, RDN, RTZ] times [FZ16 off, on].
FZ16 flushes input subnormals and tiny exact results before rounding. AH=0.
NaN sign/payload and exception flags are outside this value oracle.

References: Arm DDI0600A.d shared FPRoundBase and FPUnpackBase, pp509-510/518,
https://kib.kiev.ua/x86docs/ARM/ARMARMv8/DDI0600A_d_armv8_r64_supplement.pdf
and Arm Cortex-A55 FPCR register description,
https://documentation-service.arm.com/static/649ac6f938511951cb799083
This is an independent mathematical implementation, not copied pseudocode.
"""
from collections import Counter
from fractions import Fraction
from math import isqrt
from pathlib import Path
import argparse
import hashlib
import json
import random
import struct

SIGN, INF, QNAN = 0x8000, 0x7c00, 0x7e00
MODES = ('RNE', 'RUP', 'RDN', 'RTZ')
OPS = ('add', 'sub', 'mul', 'fma', 'div', 'sqrt')


def power(exponent):
    return Fraction(1 << exponent) if exponent >= 0 else Fraction(1, 1 << -exponent)


def kind(word):
    magnitude = word & 0x7fff
    return 'nan' if magnitude > INF else 'inf' if magnitude == INF else 'zero' if magnitude == 0 else 'finite'


def decode(word):
    exponent, fraction = (word >> 10) & 31, word & 1023
    if exponent == 31:
        raise ValueError('Cannot decode an infinity or NaN as a rational')
    value = fraction * power(-24) if exponent == 0 else (1024 + fraction) * power(exponent - 25)
    return -value if word & SIGN else value


def normalize(word, flush):
    return word & SIGN if flush and (word & 0x7fff) < 0x400 else word


def pack(value, mode, flush=False, zero_sign=0):
    if mode not in MODES:
        raise ValueError(mode)
    if not value:
        return zero_sign
    sign = SIGN if value < 0 else 0
    magnitude = abs(value)
    if flush and magnitude < power(-14):
        return sign
    exponent = magnitude.numerator.bit_length() - magnitude.denominator.bit_length()
    if magnitude < power(exponent):
        exponent -= 1
    step = max(exponent, -14) - 10
    numerator, denominator = magnitude.numerator, magnitude.denominator
    if step < 0:
        numerator <<= -step
    else:
        denominator <<= step
    mantissa, remainder = divmod(numerator, denominator)
    increment = (mode == 'RNE' and (2 * remainder > denominator or
                                  (2 * remainder == denominator and mantissa & 1)))
    increment |= bool(remainder and ((mode == 'RUP' and not sign) or
                                     (mode == 'RDN' and sign)))
    mantissa += int(increment)
    if exponent >= -14 and mantissa == 2048:
        mantissa >>= 1
        exponent += 1
    if exponent > 15:
        to_infinity = mode == 'RNE' or (mode == 'RUP' and not sign) or (mode == 'RDN' and sign)
        return sign | (INF if to_infinity else 0x7bff)
    word = mantissa if exponent < -14 else ((exponent + 15) << 10) | (mantissa - 1024)
    return sign | word


def square_root(value, mode):
    """Round sqrt(nonnegative rational) with integer squares, never host FP."""
    if not value:
        return 0
    if value < 0 or mode not in MODES:
        raise ValueError((value, mode))
    exponent = value.numerator.bit_length() - value.denominator.bit_length()
    if value < power(exponent):
        exponent -= 1
    # sqrt's binary exponent is floor(exponent/2), including negative exponents.
    step = max(exponent // 2, -14) - 10
    scaled = value / power(2 * step)
    numerator, denominator = scaled.numerator, scaled.denominator
    mantissa = isqrt(numerator // denominator)
    exact = mantissa * mantissa * denominator == numerator
    midpoint_square = (2 * mantissa + 1) ** 2 * denominator
    increment = mode == 'RUP' and not exact
    increment |= mode == 'RNE' and (4 * numerator > midpoint_square or
                                  (4 * numerator == midpoint_square and mantissa & 1))
    return pack((mantissa + int(increment)) * power(step), 'RNE')


def expected(operation, words, mode, flush):
    a, b, c = [normalize(word, flush) for word in words]
    if operation == 'sub':
        b ^= SIGN
        operation = 'add'
    used = (a,) if operation == 'sqrt' else (a, b, c) if operation == 'fma' else (a, b)
    if any(kind(word) == 'nan' for word in used):
        return QNAN
    ai, bi, ci = [kind(word) == 'inf' for word in (a, b, c)]
    if operation == 'sqrt':
        if kind(a) == 'zero':
            return a  # Includes -0 after optional input flushing.
        if a & SIGN:
            return QNAN
        return INF if ai else square_root(decode(a), mode)
    if operation == 'div':
        sign = (a ^ b) & SIGN
        az, bz = kind(a) == 'zero', kind(b) == 'zero'
        if (ai and bi) or (az and bz):
            return QNAN
        if ai or bz:
            return sign | INF
        if az or bi:
            return sign
        return pack(decode(a) / decode(b), mode, flush, sign)
    negative_cancel = SIGN if mode == 'RDN' else 0
    if operation == 'add':
        if ai and bi and (a ^ b) & SIGN:
            return QNAN
        if ai or bi:
            return a if ai else b
        left, right = decode(a), decode(b)
        zero_sign = a & SIGN if left == right == 0 and (a ^ b) & SIGN == 0 else negative_cancel
        return pack(left + right, mode, flush, zero_sign)
    product_sign = (a ^ b) & SIGN
    if (ai and kind(b) == 'zero') or (bi and kind(a) == 'zero'):
        return QNAN
    if ai or bi:
        if operation == 'fma' and ci and (product_sign ^ c) & SIGN:
            return QNAN
        return product_sign | INF
    if operation == 'fma' and ci:
        return c
    product = decode(a) * decode(b)
    if operation == 'mul':
        return pack(product, mode, flush, product_sign)
    if operation != 'fma':
        raise ValueError(operation)
    addend = decode(c)
    zero_sign = product_sign if product == addend == 0 and (product_sign ^ c) & SIGN == 0 else negative_cancel
    return pack(product + addend, mode, flush, zero_sign)


def self_check():
    checks = 0
    for word in range(65536):
        if kind(word) in ('nan', 'inf'):
            continue
        value = decode(word)
        for mode in MODES:
            assert pack(value, mode, zero_sign=word & SIGN) == word
            checks += 1
    # Every adjacent finite nonnegative pair: midpoint and points to each side.
    for low in range(0x7bff):
        high = low + 1
        left, right = decode(low), decode(high)
        midpoint = (left + right) / 2
        even = low if low % 2 == 0 else high
        for sign in (0, SIGN):
            middle = -midpoint if sign else midpoint
            assert pack(middle, 'RNE') == (even | sign)
            assert pack(middle, 'RTZ') == (low | sign)
            assert pack(middle, 'RUP') == ((low if sign else high) | sign)
            assert pack(middle, 'RDN') == ((high if sign else low) | sign)
            checks += 4
        assert pack((3 * left + right) / 4, 'RNE') == low
        assert pack((left + 3 * right) / 4, 'RNE') == high
        checks += 2
    # For every relevant sqrt output boundary, square-root rounding must select
    # the exact endpoint, ties-to-even midpoint, and correct side of the midpoint.
    # This checks the integer-root algorithm independently of input representability.
    for low in range(0x0c00, 0x5c00):  # sqrt(min subnormal) through sqrt(max finite).
        left, right = decode(low), decode(low + 1)
        middle = (left + right) / 2
        assert square_root(left * left, 'RNE') == low
        assert square_root(middle * middle, 'RNE') == (low + (low & 1))
        assert square_root(middle * middle, 'RUP') == low + 1
        assert square_root(middle * middle, 'RDN') == low
        assert square_root(middle * middle, 'RTZ') == low
        assert square_root(((3 * left + right) / 4) ** 2, 'RNE') == low
        assert square_root(((left + 3 * right) / 4) ** 2, 'RNE') == low + 1
        checks += 7
    witnesses = [
        ('div', (0, 0, 0), 'RNE', False, QNAN),
        ('div', (INF, INF, 0), 'RNE', False, QNAN),
        ('div', (0x3c00, SIGN, 0), 'RNE', False, SIGN | INF),
        ('div', (SIGN, 0x3c00, 0), 'RNE', False, SIGN),
        ('div', (0x3c00, 0x4200, 0), 'RNE', False, 0x3555),
        ('div', (0x3c00, 0x4200, 0), 'RUP', False, 0x3556),
        ('div', (1, 0x4000, 0), 'RNE', False, 0),
        ('div', (1, 0x4000, 0), 'RUP', False, 1),
        ('div', (0x0400, 0x4000, 0), 'RNE', True, 0),
        ('sqrt', (SIGN, 0, 0), 'RNE', False, SIGN),
        ('sqrt', (0x8001, 0, 0), 'RNE', False, QNAN),
        ('sqrt', (0x8001, 0, 0), 'RNE', True, SIGN),
        ('sqrt', (1, 0, 0), 'RNE', False, 0x0c00),
        ('sqrt', (0x4000, 0, 0), 'RNE', False, 0x3da8),
        ('sqrt', (0x4000, 0, 0), 'RUP', False, 0x3da9),
        ('sqrt', (INF, QNAN, 0), 'RNE', False, INF),
        ('fma', (0x3c02, 0x3d00, 0x0001), 'RNE', False, 0x3d03),
        ('fma', (0x3c02, 0x3d00, 0x0001), 'RNE', True, 0x3d02),
        ('mul', (0x0400, 0x3bff, 0), 'RNE', False, 0x0400),
        ('mul', (0x0400, 0x3bff, 0), 'RNE', True, 0x0000),
        ('mul', (0x8400, 0x3bff, 0), 'RDN', True, 0x8000),
        ('add', (0x3c00, 0xbc00, 0), 'RDN', False, 0x8000),
        ('add', (0x3c00, 0xbc00, 0), 'RUP', False, 0),
        ('add', (SIGN, SIGN, 0), 'RUP', False, SIGN),
        ('sub', (SIGN, 0, 0), 'RNE', False, SIGN),
        ('fma', (SIGN, 0x3c00, 0), 'RNE', False, 0),
        ('fma', (SIGN, 0x3c00, 0), 'RDN', False, SIGN),
        ('mul', (0x7bff, 0x4000, 0), 'RTZ', False, 0x7bff),
        ('mul', (0xfbff, 0x4000, 0), 'RUP', False, 0xfbff),
        ('mul', (0xfbff, 0x4000, 0), 'RDN', False, 0xfc00),
        ('fma', (0x7c00, 0, 0), 'RNE', False, QNAN),
        ('fma', (0x7c00, 0x3c00, 0xfc00), 'RNE', False, QNAN),
    ]
    for operation, words, mode, flush, wanted in witnesses:
        actual = expected(operation, words, mode, flush)
        assert actual == wanted, (operation, words, mode, flush, hex(actual), hex(wanted))
        checks += 1
    return checks


def generate(output):
    checks = self_check()
    rng = random.Random(0x4e46503136)
    cases, categories = [], Counter()

    def add(category, words):
        assert all(0 <= word <= 65535 for word in words)
        cases.append(tuple(words))
        categories[category] += 1

    positives = [0, 1, 0x3ff, 0x400, 0x401, 0x3800, 0x3bff, 0x3c00,
                 0x3c01, 0x3c02, 0x7bff, INF, QNAN, 0x7c01]
    special = positives + [word | SIGN for word in positives]
    for a in special:
        for b in special:
            add('special_pairs', (a, b, rng.choice(special)))
    for a, b, c in [(0x3c02, 0x3d00, 1), (0x400, 0x3bff, 0),
                    (0x400, 0x3bff, 1), (0x3c01, 0x3c01, 0xbc02),
                    (0x7bff, 0x4000, 0xfbff), (1, 0x3800, 0),
                    (0x0000, 0x3c00, 0x0000)]:
        for signs in range(8):
            add('signed_witnesses', (a ^ (SIGN if signs & 1 else 0),
                                     b ^ (SIGN if signs & 2 else 0),
                                     c ^ (SIGN if signs & 4 else 0)))
    for _ in range(384):
        exponents = (0, 1, 2, 3, 13, 14, 15, 16, 17, 28, 29, 30)
        add('stratified_finite', [rng.randrange(2) << 15 | rng.choice(exponents) << 10 |
                                  rng.getrandbits(10) for _ in range(3)])
    for _ in range(256):
        a, b = [rng.randrange(1, 31) << 10 | rng.getrandbits(10) |
                rng.randrange(2) << 15 for _ in range(2)]
        c = pack(-decode(a) * decode(b), 'RNE')
        add('fma_cancellation', (a, b, c))
    while len(cases) < 2048:
        add('uniform_bits', [rng.getrandbits(16) for _ in range(3)])
    # Keep the original 2048 inputs intact, then add division-specific boundaries.
    for a, b in [(0x400, 0x3c01), (0x3ff, 0x3bff), (1, 0x4000),
                 (3, 0x4000), (0x7bff, 0x3bff), (0x3c00, 0x4200),
                 (0x401, 0x4001), (0x7bff, 1)]:
        for signs in range(4):
            add('division_signed_boundaries', (a ^ (SIGN if signs & 1 else 0),
                                              b ^ (SIGN if signs & 2 else 0), 0))
    while len(cases) < 2304:
        add('division_boundary_neighbours', (rng.choice((1, 2, 3, 0x3ff, 0x400, 0x401, 0x7bff)) |
                                             rng.randrange(2) << 15,
                                             rng.choice((0x37ff, 0x3800, 0x3801, 0x3bff, 0x3c00,
                                                         0x3c01, 0x3fff, 0x4000, 0x4001)) |
                                             rng.randrange(2) << 15, 0))
    rows = []
    class_counts = Counter()
    expected_data = bytearray()
    for words in cases:
        configurations = []
        for mode in MODES:
            for flush in (False, True):
                answers = [expected(op, words, mode, flush) for op in OPS]
                for op, word in zip(OPS, answers):
                    class_counts[f'{mode}:FZ16={int(flush)}:{op}:{kind(word)}'] += 1
                expected_data.extend(struct.pack('<6H', *answers))
                configurations.append('{' + ','.join(f'0x{word:04x}' for word in answers) + '}')
        rows.append('  {' + ','.join(f'0x{word:04x}' for word in words) + ',{' + ','.join(configurations) + '}},')
    inputs = b''.join(struct.pack('<3H', *words) for words in cases)
    manifest = {'cases': len(cases), 'comparisons': len(cases) * 8 * len(OPS),
                'seed': '0x4e46503136', 'categories': dict(categories),
                'oracle_self_checks': checks, 'input_sha256': hashlib.sha256(inputs).hexdigest(),
                'expected_sha256': hashlib.sha256(expected_data).hexdigest(),
                'result_classes': dict(class_counts)}
    header = '''// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <cstddef>
#include <cstdint>
// Generated by generate_reference.py using exact rational arithmetic.
// Configuration: rounding [RNE,RUP,RDN,RTZ] * 2 + FZ16 [off,on].
// Operations: add, sub, mul, fma, div, sqrt. Expected NaNs compare by classification.
namespace neon_fp16_reference {
struct case_data { std::uint16_t a, b, c; std::uint16_t expected[8][6]; };
inline constexpr case_data cases[] = {
'''
    header += '\n'.join(rows) + '\n};\ninline constexpr std::size_t case_count = sizeof(cases) / sizeof(cases[0]);\n}\n'
    # Only positive finite inputs need a table: special encodings and FZ16
    # normalization are exact bit classification in sqrt_expected(). RDN == RTZ.
    sqrt_rows, sqrt_data = [], bytearray()
    for word in range(INF):
        answers = [square_root(decode(word), mode) for mode in MODES[:3]]
        sqrt_data.extend(struct.pack('<3H', *answers))
        sqrt_rows.append('  {' + ','.join(f'0x{answer:04x}' for answer in answers) + '},')
    sqrt_header = """// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <cstdint>
// Generated using integer square roots and exact midpoint-square comparisons.
namespace neon_fp16_reference {
inline constexpr std::uint16_t sqrt_positive[][3] = {
""" + '\n'.join(sqrt_rows) + """
};
constexpr std::uint16_t sqrt_expected(std::uint16_t word, unsigned config) {
  auto magnitude = word & 0x7fff;
  if (magnitude > 0x7c00) return 0x7e00;
  if (magnitude == 0 || ((config & 1) && magnitude < 0x400)) return word & 0x8000;
  if (word & 0x8000) return 0x7e00;
  if (magnitude == 0x7c00) return 0x7c00;
  auto rounding = config / 2;
  return sqrt_positive[word][rounding == 3 ? 2 : rounding];
}
}
"""
    manifest['sqrt_encodings'] = 65536
    manifest['sqrt_positive_sha256'] = hashlib.sha256(sqrt_data).hexdigest()
    output.mkdir(parents=True, exist_ok=True)
    (output / 'sqrt_reference.h').write_text(sqrt_header, encoding='utf-8')
    (output / 'reference_cases.h').write_text(header, encoding='utf-8')
    (output / 'reference_inputs.bin').write_bytes(inputs)
    (output / 'reference_expected.bin').write_bytes(expected_data)
    (output / 'reference_manifest.json').write_text(json.dumps(manifest, indent=2) + '\n', encoding='utf-8')
    print(json.dumps({key: value for key, value in manifest.items() if key != 'result_classes'}, indent=2))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', type=Path, default=Path(__file__).resolve().parent)
    args = parser.parse_args()
    generate(args.output)
