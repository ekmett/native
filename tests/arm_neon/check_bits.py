#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Require all 46 raw/public bit-operation pairs and a single instruction each."""
import argparse
import json
from pathlib import Path
import subprocess
import sys

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--objdump', required=True)
parser.add_argument('--output', required=True, type=Path)
parser.add_argument('objects', nargs='+')
args = parser.parse_args()
subprocess.run([sys.executable, str(Path(__file__).parent.parent / 'arm_dotprod/check_equivalence.py'),
                '--objdump', args.objdump, '--output', str(args.output), *args.objects], check=True)
records = json.loads(args.output.read_text())
expected = set()
for op, widths, signs in [('clz', [8, 16, 32], 'su'), ('cls', [8, 16, 32], 's'),
                          ('rbit', [8], 'su'), ('rev16', [8], 'su'),
                          ('rev32', [8, 16], 'su'), ('rev64', [8, 16, 32], 'su')]:
    for sign in signs:
        for width in widths:
            for bits in [64, 128]:
                expected.add(f'native_{op}_{sign}{width}_{bits // width}')
assert {record['name'] for record in records} == expected
for record in records:
    opcode = record['name'].split('_')[1]
    assert len(record['public']) == 2, record
    assert record['public'][0].startswith(opcode + ' '), record
    assert record['public'][1] == 'ret', record
print('All 46 concrete shapes contain one bit instruction and return')
