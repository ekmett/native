# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check platform constants against real target predefines, without a sysroot."""
import argparse
import json
from pathlib import Path
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument('--compiler', required=True)
parser.add_argument('--include', required=True)
parser.add_argument('--source', required=True)
parser.add_argument('--output', type=Path, required=True)
args = parser.parse_args()
records = []
for target, arm, x86, wasm in [
    ('aarch64-unknown-linux-gnu', 1, 0, 0),
    ('armv7-unknown-linux-gnueabihf', 1, 0, 0),
    ('arm64ec-pc-windows-msvc', 1, 0, 0),
    ('x86_64-unknown-linux-gnu', 0, 1, 0),
    ('i386-unknown-linux-gnu', 0, 1, 0),
    ('wasm32-unknown-unknown', 0, 0, 1),
    ('wasm64-unknown-unknown', 0, 0, 1),
    ('riscv64-unknown-linux-gnu', 0, 0, 0),
]:
    command = [args.compiler, '--driver-mode=g++', '--target=' + target,
               '-std=c++20', '-fsyntax-only', '-I' + args.include,
               f'-DEXPECT_ARM={arm}', f'-DEXPECT_X86={x86}', f'-DEXPECT_WASM={wasm}',
               args.source]
    result = subprocess.run(command, text=True, capture_output=True)
    records.append({'command': command, 'exit': result.returncode,
                    'diagnostics': result.stdout + result.stderr})
args.output.write_text(json.dumps(records, indent=2) + '\n', encoding='utf-8')
failed = [record for record in records if record['exit']]
for record in failed:
    print(' '.join(record['command']) + '\n' + record['diagnostics'])
if failed:
    raise SystemExit(1)
print('Platform constants passed for eight real compiler targets.')
