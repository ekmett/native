#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Compare typed public wrappers with register helpers in identical caller contexts."""
import argparse
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--objdump', required=True)
parser.add_argument('--raw', required=True)
parser.add_argument('--public', required=True)
parser.add_argument('--output', type=Path, required=True)
parser.add_argument('--expected', type=int, help='require this many public/reference function pairs')
args = parser.parse_args()


def functions(path):
    result = subprocess.run([args.objdump, '--syms', '--disassemble', '--no-show-raw-insn', path],
                            text=True, capture_output=True, check=True)
    bodies = {}
    addresses = {}
    current = None
    for line in result.stdout.splitlines():
        symbol = re.fullmatch(r'([0-9a-fA-F]+) <([^>]+)>:', line)
        if symbol:
            name = symbol[2].lstrip('_')
            current = []
            addresses[int(symbol[1], 16)] = current
            if name.startswith('native_'):
                bodies[name] = current
            continue
        instruction = re.match(r'\s*[0-9a-fA-F]+:\s+(.+)', line)
        if instruction and current is not None:
            # ARM immediates use # without following whitespace; retain them.
            text = re.sub(r'\s+#\s.*$', '', instruction[1]).strip()
            # Function alignment changes when independent module initializers exist.
            if re.match(r'(?:nop\w*|int3)\b', text):
                continue
            # Pool locations depend on surrounding functions, not wrapper work.
            text = re.sub(r'(?:0x)?[0-9a-fA-F]+\(%rip\)', 'constant(%rip)', text)
            current.append(text)
    # Mach-O can label the first function ltmp0; recover its exported alias.
    for address, name in re.findall(
            r'(?m)^([0-9a-fA-F]+)\s+[gw]\s+F\s+\S+\s+(_?native_\w+)$', result.stdout):
        if int(address, 16) in addresses:
            bodies.setdefault(name.lstrip('_'), addresses[int(address, 16)])
    return bodies


raw, public = functions(args.raw), functions(args.public)
missing = raw.keys() - public.keys()
extra = public.keys() - raw.keys()
failures = []
for name in sorted(raw.keys() & public.keys()):
    if raw[name] != public[name]:
        failures.append(f'{name}\n  raw: {raw[name]}\n  public: {public[name]}')
report = f'Compared {len(raw.keys() & public.keys())} function pairs.\n'
if missing or extra:
    report += f'Missing public functions: {sorted(missing)}\nExtra public functions: {sorted(extra)}\n'
report += '\n'.join(failures)
args.output.write_text(report, encoding='utf-8')
if args.expected is not None and (len(raw) != args.expected or len(public) != args.expected):
    raise SystemExit(f'Expected {args.expected} pairs; found {len(raw)} reference and {len(public)} public functions')
if not raw or missing or extra or failures:
    raise SystemExit(report)
print(report + 'Public bindings add no instructions in these caller contexts.')
