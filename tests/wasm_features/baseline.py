# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Capture actual Wasm compiler predefines; type-check the expansions on the host."""
import argparse
import json
import pathlib
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument('--compiler', required=True)
parser.add_argument('--include', required=True)
parser.add_argument('--output', type=pathlib.Path, required=True)
args = parser.parse_args()
scalar = '::native::isa{}'
standard = '::native::isa(::native::wasm_feature::simd128)'
relaxed = '(::native::wasm_feature::simd128&::native::wasm_feature::relaxed_simd)'
cases = [
    ('baseline', [], scalar),
    ('simd128', ['-msimd128'], standard),
    ('relaxed', ['-mrelaxed-simd'], relaxed),
    ('disable_simd', ['-msimd128', '-mno-simd128'], scalar),
    ('disable_dependency', ['-mrelaxed-simd', '-mno-simd128'], scalar),
    ('disable_relaxed', ['-mrelaxed-simd', '-mno-relaxed-simd'], scalar),
    ('reenable_relaxed', ['-mno-simd128', '-mrelaxed-simd'], relaxed),
    ('reenable_standard', ['-mrelaxed-simd', '-mno-relaxed-simd', '-msimd128'], standard),
]
lines = ['// Generated from real Wasm target predefines; not simulated macros.', '#include <native/isa.h>']
records = []
for target in ('wasm32-unknown-unknown', 'wasm64-unknown-unknown'):
    for name, flags, expected in cases:
        command = [args.compiler, '--driver-mode=g++', '--target=' + target,
                   '-std=c++20', '-E', '-P', '-x', 'c++', '-I' + args.include, *flags, '-']
        result = subprocess.run(command, input='#include <native/targets.h>\nNATIVE_BASELINE\n',
                                text=True, capture_output=True)
        if result.returncode:
            raise RuntimeError(' '.join(command) + '\n' + result.stdout + result.stderr)
        expression = result.stdout.strip()
        label = target.split('-')[0] + '_' + name
        lines.append('static_assert((' + expression + ') == ' + expected + ', "' + label + '");')
        records.append({'name': label, 'command': command, 'expression': expression,
                        'expected': expected, 'diagnostics': result.stderr})
args.output.write_text('\n'.join(lines) + '\n', encoding='utf-8')
args.output.with_suffix('.json').write_text(json.dumps(records, indent=2), encoding='utf-8')
print('Captured 16 Wasm compiler feature snapshots for native metadata assertions.')
