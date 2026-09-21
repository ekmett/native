#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Prove BE register/lane mappings against ACLE for every NEON helper shape.

Compile exact production instruction and storage-bridge bodies freestanding.
Only standard type/sequence declarations and attributes are supplied locally;
no target sysroot or differently configured named-module provider is needed.
Interpret emitted register permutations symbolically. Arithmetic results retain
all input bits as opaque expressions, so equality proves operand/lane mapping
without duplicating saturation or shift arithmetic. Unknown lowering fails.
This is compiler-layout evidence, not execution on big-endian hardware.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--compiler', required=True)
parser.add_argument('--header', required=True, type=Path)
parser.add_argument('--output-dir', required=True, type=Path)
args = parser.parse_args()
out = args.output_dir
out.mkdir(parents=True, exist_ok=True)
original = args.header.read_text()
header = '\n'.join(line for line in original.splitlines()
                   if not line.startswith('#include') and line != '#pragma once')
prefix = '''#include <arm_neon.h>
#define NATIVE_HOST_NEON 1
#define native_inline inline __attribute__((always_inline))
#define native_target(x) __attribute__((target(x)))
namespace std {
  using size_t = __SIZE_TYPE__;
  template<class T, T... I> struct integer_sequence {};
  template<size_t... I> using index_sequence = integer_sequence<size_t, I...>;
  template<size_t N> using make_index_sequence = __make_integer_seq<integer_sequence, size_t, N>;
}
template<class R> struct vector_storage {
  using native_type = R;
  R value;
  R to_native() const { return value; }
  static vector_storage from_native(R x) { return {x}; }
};
'''
pattern = r'native_inline native_target\("neon"\) (u?int\d+x\d+_t) (\w+)\(([^)]+)\) noexcept'
records = []
source = prefix + header + '\n'
for ret, op, params in re.findall(pattern, original):
    arguments = [(typ, name) for typ, name in re.findall(r'(\w+) (\w+)', params)]
    width = int(re.search(r'int(\d+)', arguments[-1][0])[1])
    unsigned = ret.startswith('u')
    q = 'q' if int(re.search(r'x(\d+)', ret)[1]) * int(re.search(r'int(\d+)', ret)[1]) == 128 else ''
    if op.startswith(('sqxt', 'uqxt')):
        base = 'qmovun' if op.startswith('sqxtun') else 'qmovn'
        intrinsic = f'v{base}{"_high" if op.endswith("_high") else ""}_{"u" if op.startswith("uq") else "s"}{width}'
    else:
        intrinsic = 'v' + {'sqadd':'qadd', 'uqadd':'qadd', 'sqsub':'qsub', 'uqsub':'qsub',
                          'sqdmulh':'qdmulh', 'sqrdmulh':'qrdmulh', 'sshl':'shl', 'ushl':'shl',
                          'srshl':'rshl', 'urshl':'rshl', 'sqshl':'qshl', 'uqshl':'qshl',
                          'sqrshl':'qrshl', 'uqrshl':'qrshl'}[op] + q + ('_u' if unsigned else '_s') + str(width)
    name = op + '_' + arguments[-1][0]
    record = {'name': name, 'return': ret, 'params': arguments}
    records.append(record)
    raw_args = ', '.join(name for typ, name in arguments)
    source += f'extern "C" {ret} native_{name}({params}) {{ return native::detail::arm_neon::{op}({raw_args}); }}\n'
    source += f'extern "C" {ret} reference_{name}({params}) {{ return {intrinsic}({raw_args}); }}\n'
    def storage(typ):
        return {'int32x2_t':'int32x4_t', 'uint32x2_t':'uint32x4_t',
                'int64x1_t':'int64_t', 'uint64x1_t':'uint64_t'}.get(typ, typ)
    bridge_args = ', '.join(f'native::detail::arm_neon::to_register<{typ}>(native::detail::arm_neon::from_register<vector_storage<{storage(typ)}>>({arg}))' for typ, arg in arguments)
    result = f'native::detail::arm_neon::{op}({bridge_args})'
    result = f'native::detail::arm_neon::to_register<{ret}>(native::detail::arm_neon::from_register<vector_storage<{storage(ret)}>>({result}))'
    source += f'extern "C" {ret} bridge_{name}({params}) {{ return {result}; }}\n'
assert len(records) == 122, len(records)
(out / 'comparison.cc').write_text(source)
command = [args.compiler, '--driver-mode=g++', '-std=c++26', '-target', 'aarch64_be-none-elf',
           '-ffreestanding', '-mcpu=generic', '-march=armv8-a', '-O2', '-S', str(out / 'comparison.cc'),
           '-o', str(out / 'comparison.s')]
result = subprocess.run(command, capture_output=True, text=True)
(out / 'compile.json').write_text(json.dumps({'command': command, 'exit_code': result.returncode,
    'stderr': result.stderr, 'stdout': result.stdout, 'records': records}, indent=2) + '\n')
if result.returncode:
    raise SystemExit(result.stderr)
functions = {}
name = None
for line in (out / 'comparison.s').read_text().splitlines():
    if re.match(r'^(native|reference|bridge)_\w+:', line):
        name = line.split(':')[0]
        functions[name] = []
    elif name and re.match(r'^\s+\.Lfunc_end', line):
        name = None
    elif name and re.match(r'^\t[a-z]', line):
        text = line.split('//')[0].strip()
        if text and text != 'ret':
            functions[name].append(text)


def execute(instructions, record):
    registers = {i: tuple(('input', i, b) for b in range(128)) for i in range(32)}
    status = []
    def shape(token):
        m = re.fullmatch(r'([bhsdv])(\d+)(?:\.(\d*)([bhsd])(?:\[(\d+)\])?)?', token)
        assert m, token
        kind, reg, count, unit, index = m.groups()
        width = {'b':8, 'h':16, 's':32, 'd':64, 'v':128}[unit or kind]
        if index is not None:
            return int(reg), width, 1, int(index) * width
        return int(reg), width, int(count) if count else 1, 0
    def read(token):
        reg, width, count, offset = shape(token)
        return registers[reg][offset:offset + width * count]
    def write(token, value):
        reg, width, count, offset = shape(token)
        bits = width * count
        assert len(value) == bits, (token, len(value), bits)
        if '[' in token:
            registers[reg] = registers[reg][:offset] + tuple(value) + registers[reg][offset + bits:]
        else:
            registers[reg] = tuple(value) + (0,) * (128 - bits)
    for line in instructions:
        op, text = line.split(None, 1)
        xs = [x.strip() for x in text.split(',')]
        if op in ('mov', 'fmov', 'dup'):
            value = read(xs[1])
            count = len(read(xs[0])) // len(value)
            write(xs[0], value * count)
        elif op == 'movi':
            assert int(xs[1].lstrip('#'), 0) == 0, line
            write(xs[0], (0,) * len(read(xs[0])))
        elif op == 'ext':
            offset = int(xs[3].lstrip('#')) * 8
            write(xs[0], (read(xs[1]) + read(xs[2]))[offset:offset + len(read(xs[0]))])
        elif op.startswith('rev'):
            group = int(op[3:])
            width = shape(xs[0])[1]
            value = read(xs[1])
            parts = [value[i:i + width] for i in range(0, len(value), width)]
            output = []
            for i in range(0, len(parts), group // width):
                output.extend(reversed(parts[i:i + group // width]))
            write(xs[0], sum(output, ()))
        elif op.startswith(('sq', 'uq', 'sshl', 'ushl', 'srshl', 'urshl')):
            narrow = op.startswith(('sqxt', 'uqxt'))
            high = narrow and op.endswith('2')
            operation = op[:-1] if high else op
            width = shape(xs[1])[1]
            aa = read(xs[1])
            bb = read(xs[2]) if len(xs) == 3 else ()
            result = ()
            for i in range(len(aa) // width):
                a = aa[i * width:(i + 1) * width]
                b = bb[i * width:(i + 1) * width] if bb else ()
                out_width = width // 2 if narrow else width
                expression = (operation, width, a, b)
                result += tuple((expression, bit) for bit in range(out_width))
                if op.startswith(('sq','uq')):
                    status.append(expression)
            if high:
                result = read(xs[0])[:64] + result
            write(xs[0], result)
        else:
            raise AssertionError(line)
    bits, lanes = map(int, re.search(r'int(\d+)x(\d+)', record['return']).groups())
    # QC is sticky OR: the lane order of saturation observations is immaterial.
    return registers[0][:bits * lanes], frozenset(status)

for record in records:
    expected = execute(functions['reference_' + record['name']], record)
    for prefix in ('native_', 'bridge_'):
        body = functions[prefix + record['name']]
        actual = execute(body, record)
        assert actual == expected, prefix + record['name']
receipt = {'header_sha256': hashlib.sha256(original.encode()).hexdigest(), 'shapes': len(records),
           'comparisons': len(records) * 2, 'result': 'pass',
           'scope': 'Symbolic interpretation of BE compiler register mappings and sticky QC against ACLE; no BE hardware execution.'}
(out / 'comparison.json').write_text(json.dumps(receipt, indent=2) + '\n')
print(json.dumps(receipt, indent=2))
