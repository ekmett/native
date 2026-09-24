#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Compare complete public gather/scatter wrapper bodies with direct intrinsics."""
import argparse
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--objdump', required=True)
parser.add_argument('--output', type=Path, required=True)
parser.add_argument('objects', nargs='+')
args = parser.parse_args()
result = subprocess.run([args.objdump, '--syms', '--disassemble', '--no-show-raw-insn', *args.objects], text=True, capture_output=True)
args.output.write_text(result.stdout + result.stderr)
if result.returncode:
    raise SystemExit(result.stderr)
bodies = {}
for _, contents in re.findall(r'(?ms)^(.+):\s+file format [^\n]+\n(.*?)(?=^.+:\s+file format |\Z)', result.stdout):
    sections = re.split(r'(?m)^Disassembly of section ([^\n]+):\s*$', contents)
    by_address = {}
    for i in range(1, len(sections), 2):
        entries = re.split(r'(?m)^([0-9a-fA-F]+) <([^\n]+)>:\s*$', sections[i + 1])
        for j in range(1, len(entries), 3):
            address, name, body = entries[j:j + 3]
            by_address[sections[i], int(address, 16)] = body
            bodies[name.lstrip('_')] = body
    # Mach-O may call the first function ltmp0; recover names from its symbol table.
    for address, section, name in re.findall(r'(?m)^([0-9a-f]+)\s+[gw]\s+F\s+(\S+)\s+([^\n]+)$', sections[0]):
        body = by_address.get((section, int(address, 16)))
        if body is not None:
            bodies[name.lstrip('_')] = body

def instructions(name):
    if name not in bodies:
        raise SystemExit(f'Missing function {name}')
    ops = re.findall(r'(?m)^\s*[0-9a-f]+:\s+([a-z][a-z0-9]*)[ \t]*([^\n]*)$', bodies[name])
    return [(op, re.sub(r'\s+', ' ', operands).strip()) for op, operands in ops
            if not op.startswith(('nop', 'int3')) and op != 'data16']

count = 0
for name in sorted(bodies):
    if not name.startswith('public_'):
        continue
    public = instructions(name)
    raw = instructions('raw_' + name.removeprefix('public_'))
    if public != raw:
        raise SystemExit(f'{name} differs from its raw intrinsic:\npublic: {public}\nraw: {raw}')
    indexed = [op for op, _ in public if 'gather' in op or 'scatter' in op]
    if len(indexed) != 1:
        raise SystemExit(f'{name} must contain exactly one indexed memory instruction: {public}')
    if any(op.startswith(('call', 'j')) for op, _ in public):
        raise SystemExit(f'{name} contains a call or branch: {public}')
    count += 1
if count != 480:
    raise SystemExit(f'Expected 480 raw/public pairs, found {count}')
print(f'All {count} public wrappers match complete direct-intrinsic instruction bodies.')
