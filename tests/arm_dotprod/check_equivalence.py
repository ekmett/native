#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Require identical instructions for public SIMD and raw-helper leaves.

Pairs have identical native-register signatures and target attributes. Compare
all instructions, including register movement and memory traffic; ignore only
addresses, whitespace and alignment NOPs after the terminating RET.
"""
import argparse
import json
import pathlib
import re
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--objdump', required=True)
parser.add_argument('--output', required=True, type=pathlib.Path)
parser.add_argument('objects', nargs='+')
args = parser.parse_args()
result = subprocess.run([args.objdump, '--syms', '--disassemble', '--no-show-raw-insn',
                         '--triple=aarch64-none-linux-gnu', *args.objects],
                        capture_output=True, text=True)
if result.returncode:
    raise SystemExit(result.stderr)
args.output.with_suffix('.disassembly').write_text(result.stdout, encoding='utf-8')
parts = re.split(r'(?m)^([0-9a-f]+) <([^\n]+)>:\s*$', result.stdout)
addresses = {int(parts[i], 16): parts[i + 2] for i in range(1, len(parts), 3)}
bodies = {parts[i + 1].lstrip('_'): parts[i + 2] for i in range(1, len(parts), 3)}
for address, name in re.findall(r'(?m)^([0-9a-f]+)\s+[gw]\s+F\s+\S+\s+(_?(?:native|raw)_\w+)$', result.stdout):
    if int(address, 16) in addresses:
        bodies[name.lstrip('_')] = addresses[int(address, 16)]


def instructions(body):
    result = []
    ended = False
    for text in re.findall(r'(?m)^\s*[0-9a-f]+:\s+(.+)$', body):
        text = ' '.join(text.split())
        if ended and text == 'nop':
            continue
        result.append(text)
        ended = text == 'ret'
    return result


records = []
for name in sorted(bodies):
    if not name.startswith('native_'):
        continue
    peer = 'raw_' + name.removeprefix('native_')
    if peer not in bodies:
        raise SystemExit('Missing raw comparison leaf for ' + name)
    public, raw = instructions(bodies[name]), instructions(bodies[peer])
    if not public or not raw:
        raise SystemExit('Empty comparison leaf: ' + name)
    records.append({'name': name, 'public': public, 'raw': raw, 'identical': public == raw})
args.output.write_text(json.dumps(records, indent=2) + '\n', encoding='utf-8')
if not records:
    raise SystemExit('No comparison leaves found')
failures = [record for record in records if not record['identical']]
if failures:
    raise SystemExit(json.dumps(failures, indent=2))
print(f'{len(records)} public/raw leaves have identical instruction sequences')
