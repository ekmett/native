#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Require identical raw/public bodies and the finalized 0xfd100..113 opcodes."""
import argparse
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--objdump', required=True)
parser.add_argument('--raw', required=True)
parser.add_argument('--public', required=True)
parser.add_argument('--engine', required=True)
parser.add_argument('--output', required=True, type=Path)
args = parser.parse_args()
operations = [
    ('i8x16_relaxed_swizzle', 2),
    ('i32x4_relaxed_trunc_f32x4', 1),
    ('u32x4_relaxed_trunc_f32x4', 1),
    ('i32x4_relaxed_trunc_f64x2_zero', 1),
    ('u32x4_relaxed_trunc_f64x2_zero', 1),
    ('f32x4_relaxed_madd', 3),
    ('f32x4_relaxed_nmadd', 3),
    ('f64x2_relaxed_madd', 3),
    ('f64x2_relaxed_nmadd', 3),
    ('i8x16_relaxed_laneselect', 3),
    ('i16x8_relaxed_laneselect', 3),
    ('i32x4_relaxed_laneselect', 3),
    ('i64x2_relaxed_laneselect', 3),
    ('f32x4_relaxed_min', 2),
    ('f32x4_relaxed_max', 2),
    ('f64x2_relaxed_min', 2),
    ('f64x2_relaxed_max', 2),
    ('i16x8_relaxed_q15mulr', 2),
    ('i16x8_relaxed_dot_i8x16_i7x16', 2),
    ('i32x4_relaxed_dot_i8x16_i7x16_add', 3),
]
transcript = []
bodies = []
for path in (args.raw, args.public):
    result = subprocess.run([args.objdump, '-d', '--mattr=+simd128,+relaxed-simd', path],
                            text=True, capture_output=True, check=True)
    transcript.append(result.stdout)
    args.output.write_text('\n'.join(transcript), encoding='utf-8')
    sections = re.split(r'(?m)^[0-9a-f]+ <([^>]+)>:\s*$', result.stdout)
    functions = {}
    for index in range(1, len(sections), 2):
        name = sections[index]
        if name.startswith('native_relaxed_'):
            rows = re.findall(r'(?m)^\s*[0-9a-f]+:\s*((?:[0-9a-f]{2}\s+)+)', sections[index + 1])
            functions[name] = bytes.fromhex(' '.join(rows))
    if set(functions) != {'native_relaxed_' + name for name, _ in operations}:
        raise SystemExit('Missing or unexpected relaxed SIMD probe: ' + path)
    for opcode, (name, arity) in enumerate(operations, 0x100):
        expected = bytes(byte for lane in range(arity) for byte in (0x20, lane))
        expected += bytes((0xfd, (opcode & 0x7f) | 0x80, opcode >> 7, 0x0b))
        actual = functions['native_relaxed_' + name]
        if actual != expected:
            raise SystemExit(f'{path}: {name}: expected {expected.hex()}, got {actual.hex()}')
    bodies.append(functions)
if bodies[0] != bodies[1]:
    raise SystemExit('Raw/public relaxed SIMD instruction bodies differ')
result = subprocess.run([args.objdump, '-d', '--mattr=+simd128,+relaxed-simd', args.engine],
                        text=True, capture_output=True, check=True)
transcript.append(result.stdout)
args.output.write_text('\n'.join(transcript), encoding='utf-8')
for name, opcode, arity in (('raw_dot', 0x112, 2), ('raw_dot_add', 0x113, 3)):
    match = re.search(r'(?m)^[0-9a-f]+ <' + name + r'>:\s*$(.*?)(?=^[0-9a-f]+ <|\Z)',
                      result.stdout, re.S | re.M)
    if match is None:
        raise SystemExit('Missing raw engine probe: ' + name)
    rows = re.findall(r'(?m)^\s*[0-9a-f]+:\s*((?:[0-9a-f]{2}\s+)+)', match[1])
    actual = bytes.fromhex(' '.join(rows))
    expected = bytes(byte for lane in range(arity) for byte in (0x20, lane))
    expected += bytes((0xfd, (opcode & 0x7f) | 0x80, opcode >> 7, 0x0b))
    if actual != expected:
        raise SystemExit('Raw engine probe changed: ' + name)
print('All 20 raw/public bodies match exactly and contain their finalized relaxed opcode.')
