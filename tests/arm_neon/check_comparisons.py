#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Require vector FP comparisons, including in strict-rounding consumers."""
import argparse
import json
import pathlib
import re
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--objdump', required=True)
parser.add_argument('--output', required=True, type=pathlib.Path)
parser.add_argument('--half', action='store_true', help='also require six module-owned FP16 leaves')
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
for address, name in re.findall(
        r'(?m)^([0-9a-f]+)\s+[gw]\s+F\s+\S+\s+(_?comparison_\w+)$', result.stdout):
    if int(address, 16) in addresses:
        bodies[name.lstrip('_')] = addresses[int(address, 16)]


def instructions(body):
    result = []
    for text in re.findall(r'(?m)^\s*[0-9a-f]+:\s+(.+)$', body):
        text = ' '.join(text.split())
        if result and result[-1] == 'ret' and text == 'nop':
            continue
        result.append(text)
    return result


records = []
shapes = [(str(lanes), '4s', lanes == 4) for lanes in (2, 3, 4)]
if args.half:
    shapes.append(('half8', '8h', True))
for shape, arrangement, full_register in shapes:
    for op, mnemonic in [('eq', 'fcmeq'), ('ne', 'fcmeq'), ('lt', 'fcmgt'),
                         ('gt', 'fcmgt'), ('le', 'fcmge'), ('ge', 'fcmge')]:
        name = f'comparison_{op}_{shape}'
        insns = instructions(bodies.get(name, ''))
        errors = []
        compares = [text for text in insns if re.match(r'fcm\w*\b', text)]
        if len(compares) != 1 or not re.fullmatch(
                rf'{mnemonic}\s+v\d+\.{arrangement},\s*v\d+\.{arrangement},\s*v\d+\.{arrangement}',
                compares[0] if compares else ''):
            errors.append(f'expected exactly one vector {mnemonic}.{arrangement}')
        for text in insns:
            # Padded outputs may clear inactive lanes with MOV/FMOV. Forbid
            # extraction into scalar registers, retaining those padding writes.
            if (re.match(r'(?:fcmp|fcmpe|cset|csetm|umov|smov|ext|bl|blr)\b', text)
                    or re.match(r'(?:mov|fmov)\s+[swxd]\d+,\s*v\d+\.', text)
                    or re.match(r'(?:fadd|fsub|fmul|fdiv|fcsel)\b', text)):
                errors.append('scalarization or unexpected arithmetic: ' + text)
        if not insns or insns[-1] != 'ret':
            errors.append('missing leaf return')
        if full_register:
            expected_length = 3 if op == 'ne' else 2
            if len(insns) != expected_length:
                errors.append(f'full register leaf must have {expected_length} instructions')
            if op == 'ne' and not any(re.fullmatch(
                    r'(?:not|mvn)\s+v\d+\.16b,\s*v\d+\.16b', text) for text in insns):
                errors.append('unequal must invert the vector equality mask')
        records.append({'name': name, 'instructions': insns, 'errors': errors})

args.output.write_text(json.dumps(records, indent=2) + '\n', encoding='utf-8')
failures = [record for record in records if record['errors']]
if failures:
    raise SystemExit(json.dumps(failures, indent=2))
print(f'{len(records)} comparison leaves use one vector compare each; full registers have no extra work')
