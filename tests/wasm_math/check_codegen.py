#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check complete Wasm math leaves for vector lowering without relaxed operations."""
import argparse
import json
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--objdump', required=True)
parser.add_argument('--output', type=Path, required=True)
parser.add_argument('objects', nargs='+')
args = parser.parse_args()
functions = {}
for obj in args.objects:
    result = subprocess.run([args.objdump, '-d', '--no-show-raw-insn', obj],
                            check=True, text=True, capture_output=True)
    current = None
    for line in result.stdout.splitlines():
        symbol = re.match(r'^[0-9a-f]+ <([^>]+)>:', line)
        if symbol:
            current = symbol.group(1)
            functions[current] = []
        instruction = re.match(r'^\s*[0-9a-f]+:\s+(.+?)\s*$', line)
        if instruction and current:
            functions[current].append(' '.join(instruction.group(1).split()))
failures = []
names = ['exp_single', 'exp_batch', 'exp_flush', 'sin_single', 'cos_single',
         'sincos_single', 'sincos_batch', 'exp_relaxed_caller', 'promoted_exp_single',
         'promoted_exp_batch', 'array_exp_batch', 'promoted_sincos_batch']
for name in names:
    code = '\n'.join(functions.get(name, []))
    if not code:
        failures.append(f'{name}: missing body')
    for forbidden in [r'\b(?:call|return_call)\b', 'relaxed', r'\bextract_lane\b',
                      r'\bf32\.(?:add|sub|mul|div)\b', r'\bf64\.(?:add|sub|mul|div)\b']:
        if re.search(forbidden, code):
            failures.append(f'{name}: forbidden instruction {forbidden}')
    for required in ['f32x4.mul', 'f32x4.add', 'v128.load', 'v128.store', 'i32x4.trunc_sat_f32x4_u']:
        if required not in code:
            failures.append(f'{name}: missing {required}')
for a, b in [('exp_single', 'promoted_exp_single'), ('exp_single', 'exp_relaxed_caller'),
             ('exp_batch', 'promoted_exp_batch'), ('exp_batch', 'array_exp_batch'),
             ('sincos_batch', 'promoted_sincos_batch')]:
    if functions.get(a) != functions.get(b):
        failures.append(f'{a}/{b}: alias/shape instruction streams differ')
args.output.write_text(json.dumps({'functions': functions, 'failures': failures}, indent=2)+'\n')
print(f'{len(names)} vector polynomial entries; {len(failures)} failures')
for failure in failures:
    print(failure)
raise SystemExit(bool(failures))
