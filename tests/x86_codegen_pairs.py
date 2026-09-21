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
args = parser.parse_args()


def functions(path):
    result = subprocess.run([args.objdump, '--disassemble', '--no-show-raw-insn', path],
                            text=True, capture_output=True, check=True)
    bodies = {}
    current = None
    for line in result.stdout.splitlines():
        symbol = re.fullmatch(r'[0-9a-fA-F]+ <([^>]+)>:', line)
        if symbol:
            name = symbol[1].lstrip('_')
            current = name if name.startswith('native_') else None
            if current:
                bodies[current] = []
            continue
        instruction = re.match(r'\s*[0-9a-fA-F]+:\s+(.+)', line)
        if instruction and current:
            text = instruction[1].split('#')[0].strip()
            # Function alignment changes when independent module initializers exist.
            if re.match(r'(?:nop\w*|int3)\b', text):
                continue
            # Pool locations depend on surrounding functions, not wrapper work.
            text = re.sub(r'(?:0x)?[0-9a-fA-F]+\(%rip\)', 'constant(%rip)', text)
            bodies[current].append(text)
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
if not raw or missing or extra or failures:
    raise SystemExit(report)
print(report + 'Public bindings add no instructions in these caller contexts.')
