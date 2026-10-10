#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check actual producer/consumer commands, including installed BMIs."""
import argparse
import json
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--build-dir', type=Path, required=True)
parser.add_argument('--ninja', required=True)
args = parser.parse_args()
seen = set()


def check(command, kind):
    if kind != 'dispatcher' and re.search(r'cmake_pch|/Yu|/Fp', command):
        raise SystemExit(f'Module provider uses PCH: {command}')
    for feature in ('FP16', 'BF16'):
        if (f'NATIVE_MINIMAL_HAS_AVX512_{feature}=0' in command
                and f'mavx512{feature.lower()}' in command):
            raise SystemExit(f'Optional feature leaked into baseline: {command}')
    seen.add(kind)


with (args.build_dir / 'compile_commands.json').open() as stream:
    commands = json.load(stream)
for entry in commands:
    source = entry['file'].replace('\\', '/')
    if source.endswith('native.scalar.ccm'):
        kind = 'common'
    elif re.search(r'native(?:[.]simd)?[.]ccm$', source):
        kind = 'hub'
    elif source.endswith('avx512_fp16/main.cc'):
        kind = 'dispatcher'
    else:
        continue
    command = entry.get('command')
    if command is None:
        command = ' '.join(entry['arguments'])
    check(command, kind)

# CMake omits synthetic installed BMI commands from compile_commands.json.
# Inspect Ninja's actual graph instead of assuming those providers are absent.
if not {'common', 'hub'} <= seen:
    targets = []
    for line in (args.build_dir / 'build.ninja').read_text().splitlines():
        match = re.match(r'^build (.+[.]bmi): ', line)
        if match:
            targets.append(re.sub(r'\$([ :$])', r'\1', match[1]))
    graph = subprocess.run([args.ninja, '-C', str(args.build_dir), '-t', 'commands', *targets],
                           text=True, capture_output=True, check=True).stdout
    for command in graph.splitlines():
        if '--precompile' not in command:
            continue
        if 'native.scalar.ccm' in command:
            check(command, 'common')
        elif re.search(r'native(?:[.]simd)?[.]ccm', command):
            check(command, 'hub')

if not {'common', 'hub', 'dispatcher'} <= seen:
    raise SystemExit('Missing compile commands for common/native/dispatcher boundaries')
print('Hub and common providers compile without PCH; optional ISA flags stay out of the baseline')
