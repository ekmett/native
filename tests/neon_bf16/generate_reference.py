# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Independent exact-rational Arm BFDOT value oracle; AH=FIZ=0.

Arm DDI0616 A.a B3.1.2 and E2.2 specify separate baseline products/adds,
or EBF16 fused pair followed by separately rounded accumulation. No host
floating point, native instructions, copied implementation, or production use.
"""
from fractions import Fraction as Q
from pathlib import Path
import argparse
import hashlib
import json
import random

SIGN, INF, NAN = 0x80000000, 0x7f800000, 0x7fc00000
MODES = ('RNE', 'RUP', 'RDN', 'RTZ')


def power(e):
    return Q(1 << e) if e >= 0 else Q(1, 1 << -e)


def unpack(w, flush=False):
    e, f, s = (w >> 23) & 255, w & 0x7fffff, bool(w & SIGN)
    if e == 255:
        return ('nan' if f else 'inf', s, Q(0))
    x = Q(0) if not e and flush else f * power(-149) if not e else (0x800000 + f) * power(e-150)
    return ('num', s, -x if s else x)


def pack(v, mode, flush=False):
    k, negative, x = v
    s = SIGN if negative else 0
    if k == 'nan':
        return NAN
    if k == 'inf':
        return s | INF
    if not x:
        return s
    s = SIGN if x < 0 else 0
    x = abs(x)
    if flush and x < power(-126):
        return s
    e = x.numerator.bit_length() - x.denominator.bit_length()
    if x < power(e):
        e -= 1
    unit = max(e, -126) - 23
    scaled = x / power(unit)
    m, r = divmod(scaled.numerator, scaled.denominator)
    d = scaled.denominator
    if mode == 'RTO':
        if r:
            m |= 1
    else:
        m += int(bool((mode == 'RNE' and (2*r > d or (2*r == d and m & 1))) or
                      (r and ((mode == 'RUP' and not s) or (mode == 'RDN' and s)))))
    if e >= -126 and m == 0x1000000:
        m >>= 1
        e += 1
    if e > 127:
        infinity = mode in ('RNE', 'RTO') or (mode == 'RUP' and not s) or (mode == 'RDN' and s)
        return s | (INF if infinity else 0x7f7fffff)
    return s | (m if e < -126 else ((e + 127) << 23) | (m - 0x800000))


def product(a, b):
    ka, sa, xa = a
    kb, sb, xb = b
    if ka == 'nan' or kb == 'nan' or (ka == 'inf' and kb == 'num' and not xb) or (kb == 'inf' and ka == 'num' and not xa):
        return ('nan', False, Q(0))
    if ka == 'inf' or kb == 'inf':
        return ('inf', sa ^ sb, Q(0))
    return ('num', sa ^ sb, xa * xb)


def addition(a, b, mode):
    ka, sa, xa = a
    kb, sb, xb = b
    if ka == 'nan' or kb == 'nan' or (ka == kb == 'inf' and sa != sb):
        return ('nan', False, Q(0))
    if ka == 'inf' or kb == 'inf':
        return a if ka == 'inf' else b
    x = xa + xb
    negative = x < 0 if x else sa if xa == xb == 0 and sa == sb else mode == 'RDN'
    return ('num', negative, x)


def dot(case, enhanced=False, mode='RNE', flush=False):
    a0, a1, b0, b1, c = case
    if not enhanced:
        mode, flush = 'RTO', True
    a0, a1, b0, b1 = (unpack(w << 16, flush) for w in (a0, a1, b0, b1))
    p0, p1 = product(a0, b0), product(a1, b1)
    if not enhanced:
        p0, p1 = unpack(pack(p0, mode, flush), flush), unpack(pack(p1, mode, flush), flush)
    pair = pack(addition(p0, p1, mode), mode, flush)
    return pack(addition(unpack(c, flush), unpack(pair, flush), mode), mode, flush)


def self_check():
    n = 0
    for h in range(65536):
        w = h << 16
        if (w & 0x7fffffff) >= INF:
            continue
        for mode in MODES + ('RTO',):
            assert pack(unpack(w), mode) == w
            n += 1
    # Independently stated representable midpoint and direction rules.
    for low in (0, 1, 0x7ffffe, 0x7fffff, 0x800000, 0x3f7fffff, 0x3f800000, 0x3f800001, 0x7f7ffffe):
        a, b = unpack(low)[2], unpack(low+1)[2]
        mid = (a+b)/2
        for s in (0, SIGN):
            v = ('num', bool(s), -mid if s else mid)
            assert pack(v, 'RNE') == ((low if not low & 1 else low+1) | s)
            assert pack(v, 'RTO') == ((low if low & 1 else low+1) | s)
            assert pack(v, 'RTZ') == (low | s)
            assert pack(v, 'RUP') == ((low if s else low+1) | s)
            assert pack(v, 'RDN') == ((low+1 if s else low) | s)
            n += 5
    assert dot((0x0080, 0x0080, 0x3f00, 0x3f00, 0)) == 0
    assert dot((0x0080, 0x0080, 0x3f00, 0x3f00, 0), True, 'RNE', True) == 0x00800000
    assert dot((0x3f80, 0xc580, 0x3f80, 0x4580, 0x4b800000)) == 0x3f800000
    assert dot((0x3f80, 0x3980, 0x3f80, 0x3980, 0)) == 0x3f800001
    assert dot((0x3f80, 0x3980, 0x3f80, 0x3980, 0), True) == 0x3f800000
    assert dot((0x7f7f, 0xff7f, 0x4000, 0x4000, 0)) == NAN
    assert dot((0x7f7f, 0xff7f, 0x4000, 0x4000, 0), True) == 0
    return n + 7


def generate(out):
    checks = self_check()
    rng = random.Random(0xbf16a64)
    cases = []
    def add(a0, a1, b0, b1, c):
        cases.append((a0, a1, b0, b1, c))
    witnesses = [
        (0x0080,0x0080,0x3f00,0x3f00,0),
        (0x3f80,0xc580,0x3f80,0x4580,0x4b800000),
        (0x3f80,0x3980,0x3f80,0x3980,0),
        (0x7f7f,0xff7f,0x4000,0x4000,0),
        (0,0,0,0,1), (0x0001,0,0x7f00,0,0),
        (0x8000,0x8000,0x3f80,0x3f80,0x80000000),
        (0x0080,0x8080,0x3f80,0x3f7f,0),
        (0x0080,0,0x3f7f,0,0),
        (0x7f7f,0,0x4000,0,0),
    ]
    for c in witnesses:
        for signs in range(32):
            add(*(x ^ ((0x8000 if i<4 else SIGN) if signs & (1<<i) else 0) for i,x in enumerate(c)))
    half_special = (0,1,0x007f,0x0080,0x0081,0x3f00,0x3f80,0x3f81,0x7f7f,0x7f80,0x7f81,0x7fc1)
    half_special += tuple(w | 0x8000 for w in half_special)
    acc_special = (0,1,0x7fffff,0x800000,0x3f800000,0x3f800001,0x7f7fffff,INF,NAN,0x7f800001)
    acc_special += tuple(w | SIGN for w in acc_special)
    for a in half_special:
        for b in half_special:
            add(a,rng.choice(half_special),b,rng.choice(half_special),rng.choice(acc_special))
    for _ in range(512):
        hs = [rng.randrange(2)<<15 | rng.choice((1,2,63,64,65,126,127,128,129,190,191,192,253,254))<<7 | rng.getrandbits(7) for _ in range(4)]
        add(*hs,rng.getrandbits(32))
    while len(cases)<2048:
        add(*(rng.getrandbits(16) for _ in range(4)),rng.getrandbits(32))
    rows=[]
    for c in cases:
        expected=[dot(c,True,m,f) for m in MODES for f in (False,True)]
        fields=[f'0x{x:04x}' for x in c[:4]]+[f'0x{c[4]:08x}',f'0x{dot(c):08x}']
        rows.append('  {'+','.join(fields)+',{'+','.join(f'0x{x:08x}' for x in expected)+'}},')
    header='''// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <cstddef>
#include <cstdint>
// Exact rational oracle. AH=FIZ=0; enhanced index = RMode*2+FZ.
namespace neon_bf16_reference {
struct sample {std::uint16_t a0,a1,b0,b1; std::uint32_t accumulator,baseline,enhanced[8];};
inline constexpr sample cases[] = {
'''+ '\n'.join(rows)+'\n};\ninline constexpr std::size_t case_count=sizeof(cases)/sizeof(cases[0]);\n}\n'
    out.mkdir(parents=True,exist_ok=True)
    (out/'reference_cases.h').write_text(header,encoding='utf-8',newline='\n')
    manifest={'cases':len(cases),'results':len(cases)*9,'self_checks':checks,'sha256':hashlib.sha256(header.encode()).hexdigest(),'scope':'AH=FIZ=0; default NaN exact; no exception flags; independent exact rational math'}
    (out/'reference_manifest.json').write_text(json.dumps(manifest,indent=2)+'\n',encoding='utf-8',newline='\n')
    print(json.dumps(manifest))


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir",type=Path,default=Path(__file__).parent)
    generate(parser.parse_args().output_dir)
