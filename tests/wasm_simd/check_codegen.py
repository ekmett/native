#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Require identical instruction streams for every typed/intrinsic SIMD128 pair."""
import argparse
import json
from pathlib import Path
import re
import subprocess


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--objdump', required=True)
    parser.add_argument('--source', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('objects', nargs='+')
    args = parser.parse_args()
    names = re.findall(r'^PAIR\((\w+),', args.source.read_text(), re.M)
    functions = {}
    for obj in args.objects:
        result = subprocess.run([args.objdump, '-d', '--no-show-raw-insn', obj],
                                check=True, text=True, capture_output=True)
        current = None
        for line in result.stdout.splitlines():
            symbol = re.match(r'^[0-9a-f]+ <([^>]+)>:', line)
            if symbol:
                current = symbol.group(1)
                functions[current] = []
            instruction = re.match(r'^\s*[0-9a-f]+:\s+(.+?)\s*$', line)
            if instruction and current:
                functions[current].append(' '.join(instruction.group(1).split()))
    failures = []
    records = {}
    for name in names:
        raw = functions.get('raw_' + name, [])
        public = functions.get('public_' + name, [])
        records[name] = {'raw': raw, 'public': public}
        if not raw or not public:
            failures.append(f'{name}: missing or empty function')
        elif raw != public:
            failures.append(f'{name}: instruction streams differ\n  raw: {raw}\n  public: {public}')
        elif any(re.match(r'(call|return_call)(\s|$)', instruction) for instruction in public):
            failures.append(f'{name}: unexpected out-of-line call')
    if not names:
        failures.append('No pairs discovered')
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps({'pair_count': len(names), 'pairs': records,
                                       'failures': failures}, indent=2) + '\n')
    print(f'{len(names)} SIMD128 public/intrinsic pairs; {len(failures)} failures')
    for failure in failures:
        print(failure)
    return bool(failures)


if __name__ == '__main__':
    raise SystemExit(main())
