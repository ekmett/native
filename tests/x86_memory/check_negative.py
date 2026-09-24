#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Require a successful boundary control and the selected deliberate diagnostic."""
import argparse
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--cmake', required=True)
parser.add_argument('--build', required=True)
parser.add_argument('--config', default='')
parser.add_argument('--case', type=int, required=True)
parser.add_argument('--log', type=Path, required=True)
args = parser.parse_args()

def build(target):
    command = [args.cmake, '--build', args.build, '--target', target, '--parallel']
    if args.config:
        command += ['--config', args.config]
    return subprocess.run(command, text=True, capture_output=True)

positive = build('native_test_x86_memory_boundaries')
args.log.write_text('Positive control:\n' + positive.stdout + positive.stderr)
if positive.returncode:
    raise SystemExit('Positive boundary control failed; see ' + str(args.log))
negative = build(f'native_test_x86_memory_reject_{args.case}')
with args.log.open('a') as log:
    log.write('\nNegative control:\n' + negative.stdout + negative.stderr)
text = negative.stdout + negative.stderr
expected = ('no matching function' if args.case <= 8 else
            'requires target feature' if args.case == 9 else
            'call to consteval function' if args.case <= 12 or args.case >= 17 else
            'must be initialized by a constant expression')
if not negative.returncode or expected not in text:
    raise SystemExit(f'Expected boundary diagnostic {expected!r}; see {args.log}')
print(f'Boundary {args.case} rejected with the expected diagnostic.')
