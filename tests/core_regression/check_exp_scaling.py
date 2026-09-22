#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Keep exp's reconstruction vectorized and remove guarded two-factor scaling."""
import argparse
import collections
import json
import pathlib
import re
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--objdump', required=True)
parser.add_argument('--output', required=True, type=pathlib.Path)
parser.add_argument('objects', nargs='+')
args = parser.parse_args()
result = subprocess.run([args.objdump, '--disassemble', '--no-show-raw-insn',
                         *args.objects], capture_output=True, text=True, check=True)
args.output.with_suffix('.disassembly').write_text(result.stdout)
parts = re.split(r'(?m)^([0-9a-f]+) <([^\n]+)>:\s*$', result.stdout)
bodies = {parts[i + 1].lstrip('_'): parts[i + 2] for i in range(1, len(parts), 3)}
records = []
for name in ('exp_avx2_4', 'exp_avx2_8', 'exp_avx512_16'):
    instructions = re.findall(r'(?m)^\s*[0-9a-f]+:\s+(.+)$', bodies.get(name, ''))
    counts = collections.Counter(line.split()[0] for line in instructions)
    errors = []
    expected = ({'vcvttps2dq': 1, 'vpmaxsd': 1, 'vpslld': 1, 'vmulps': 2,
                 'vscalefps': 0} if 'avx2_' in name else
                {'vcvttps2dq': 0, 'vpmaxsd': 0, 'vpslld': 0, 'vmulps': 1,
                 'vscalefps': 1})
    for operation, count in expected.items():
        if counts[operation] != count:
            errors.append(f'{operation}: expected {count}, got {counts[operation]}')
    for operation in counts:
        if (operation.startswith(('call', 'j')) or operation in
                ('vcmpordps', 'vcmpunordps', 'vpsrld', 'vpsubd', 'vextractps')):
            errors.append('unexpected conversion guard, factor split or fallback: ' + operation)
    if not instructions:
        errors.append('missing kernel')
    records.append({'name': name, 'instructions': instructions, 'errors': errors})
args.output.write_text(json.dumps(records, indent=2) + '\n')
failures = [record for record in records if record['errors']]
if failures:
    raise SystemExit(json.dumps(failures, indent=2))
print('AVX2 exp uses one converted factor; AVX-512 retains one VSCALEFPS')
