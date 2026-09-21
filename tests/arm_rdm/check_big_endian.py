#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Compare big-endian RDM register mapping with Clang's trusted ACLE wrappers.

This is a compiler-layout regression, not native big-endian execution. Reuse the
actual instruction header bodies; stub only the standard-library-backed ISA
metadata and attribute includes so cross-compilation needs no target sysroot.
Native feature admission and the complete header are tested separately.

The interpreter intentionally accepts only the register instructions emitted by
these thin fixtures. A new lowering fails for review instead of being ignored.
"""
import argparse
from pathlib import Path
import hashlib
import json
import random
import re
import subprocess
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--compiler', required=True)
parser.add_argument('--header', required=True, type=Path)
parser.add_argument('--source', required=True, type=Path)
parser.add_argument('--output-dir', required=True, type=Path)
args = parser.parse_args()
out = args.output_dir
out.mkdir(parents=True, exist_ok=True)
header = args.header.read_text()
for line in ('#pragma once', '#include "native/config.h"', '#include "native/attributes.h"', '#include "native/isa.h"'):
    header = header.replace(line, '')
prefix = '#define NATIVE_HOST_NEON 1\n#define native_inline inline __attribute__((always_inline))\n#define native_nodiscard [[nodiscard]]\nnamespace native { enum class arm_feature { rdm }; struct isa { constexpr bool has(arm_feature) const { return true; } }; }\n'
fixture = args.source.read_text()
fixture = fixture[fixture.index('extern "C"'):]
fixture = fixture.replace('requirements', 'native::isa{}')
refs = []
wrappers = []
records = []
pattern = re.compile('extern "C" __attribute__\\(\\(target\\("rdm"\\), noinline\\)\\)\\n(\\w+) native_(sqrdml(?:ah|sh))_(\\w+)\\(([^\\n]+)\\) noexcept \\{\\n  return ([^\\n]+);\\n\\}')
for m in pattern.finditer(fixture):
    ret, op, tag, params, body = m.groups()
    width = 16 if '16' in ret else 32
    shape = re.search('x([248])', ret)
    q = 'q' if shape and int(shape[1]) * width == 128 else 'h' if not shape and width == 16 else 's' if not shape else ''
    lane = re.search('_lane([248])', tag)
    selections = range(int(lane[1])) if lane else [None]
    modifier = ('_laneq' if int(lane[1]) * width == 128 else '_lane') if lane else ''
    intrinsic = 'v' + op[1:] + q + modifier + f'_s{width}'
    for selected in selections:
        suffix = tag + (f'_at{selected}' if lane else '')
        operands = 'a,b,c' + (',' + str(selected) if lane else '')
        native_body = re.sub(', \\d+>', f', {selected}>', body) if lane else body
        wrappers.append(f'extern "C" __attribute__((target("rdm"), noinline))\n{ret} native_{op}_{suffix}({params}) noexcept {{ return {native_body}; }}')
        refs.append(f'extern "C" __attribute__((target("v8.1a"),noinline))\n{ret} reference_{op}_{suffix}({params}) noexcept {{ return {intrinsic}({operands}); }}')
        records.append({'name': op + '_' + suffix, 'return': ret, 'params': params, 'width': width})
assert len(refs) == 120
source = prefix + header + '\n' + '\n'.join(wrappers + refs) + '\n'
(out / 'rdm-be-comparison.cc').write_text(source)
compiler = args.compiler
command = [compiler, '--driver-mode=g++', '-std=c++26', '-target', 'aarch64_be-none-elf', '-ffreestanding', '-mcpu=generic', '-march=armv8-a', '-O2', '-S', str(out / 'rdm-be-comparison.cc'), '-o', str(out / 'rdm-be-comparison.s')]
result = subprocess.run(command, capture_output=True, text=True)
(out / 'be-compile.json').write_text(json.dumps({'command': command, 'header_sha256': hashlib.sha256(args.header.read_bytes()).hexdigest(), 'exit_code': result.returncode, 'stdout': result.stdout, 'stderr': result.stderr, 'records': records}, indent=2) + '\n')
if result.returncode:
    raise SystemExit(result.stderr)
meta = json.loads((out / 'be-compile.json').read_text())
assembly = (out / 'rdm-be-comparison.s').read_text()
functions = {}
name = None
for line in assembly.splitlines():
    if re.match('^(native|reference)_\\w+:', line):
        name = line.split(':')[0]
        functions[name] = []
    elif name and re.match('^\\s+\\.Lfunc_end', line):
        name = None
    elif name and re.match('^\\t[a-z]', line):
        text = line.split('//')[0].strip()
        if text and text != 'ret':
            functions[name].append(text)

def execute(instructions, seed, shape):
    rng = random.Random(seed)
    v = {i: rng.getrandbits(128) for i in range(32)}
    w = {i: rng.getrandbits(32) for i in range(32)}

    def read(x):
        m = re.fullmatch('([whsdv])(\\d+)(?:\\.([0-9]*)([bhsd])(?:\\[(\\d+)\\])?)?', x)
        assert m, x
        kind, num, count, lane, idx = m.groups()
        num = int(num)
        if kind == 'w':
            return w[num]
        width = {'h': 16, 's': 32, 'd': 64, 'v': 128}[kind]
        if idx is not None:
            width = {'b': 8, 'h': 16, 's': 32, 'd': 64}[lane]
            return v[num] >> int(idx) * width & (1 << width) - 1
        if lane:
            width = int(count) * {'b': 8, 'h': 16, 's': 32, 'd': 64}[lane]
        return v[num] & (1 << width) - 1

    def write(x, value):
        if x.startswith('w'):
            w[int(x[1:])] = value & 4294967295
        else:
            num = int(re.search('\\d+', x)[0])
            bits = {'h': 16, 's': 32, 'd': 64, 'v': 128}[x[0]]
            if '.' in x:
                count, unit = re.fullmatch('v\\d+\\.(\\d+)([bhsd])', x).groups()
                bits = int(count) * {'b': 8, 'h': 16, 's': 32, 'd': 64}[unit]
            v[num] = value & (1 << bits) - 1

    def signed(x, width):
        return x - (1 << width) if x & 1 << width - 1 else x
    for line in instructions:
        op, args = line.split(None, 1)
        xs = [x.strip() for x in args.split(',')]
        if op in ('fmov', 'umov', 'mov'):
            write(xs[0], read(xs[1]))
        elif op.startswith('rev'):
            bits = int(op[3:])
            count, unit = re.fullmatch('v\\d+\\.(\\d+)([bhsd])', xs[0]).groups()
            size = {'b': 8, 'h': 16, 's': 32, 'd': 64}[unit]
            total = int(count) * size
            old = read(xs[1])
            new = 0
            for i in range(total // size):
                j = i // (bits // size) * (bits // size) + (bits // size - 1 - i % (bits // size))
                new |= (old >> j * size & (1 << size) - 1) << i * size
            write(xs[0], new)
        elif op == 'ext':
            offset = int(xs[3].lstrip('#')) * 8
            write(xs[0], read(xs[1]) + (read(xs[2]) << 128) >> offset)
        elif op in ('sqrdmlah', 'sqrdmlsh'):
            if xs[0][0] in ('h', 's'):
                width = {'h': 16, 's': 32}[xs[0][0]]
                count = 1
            else:
                count, unit = re.fullmatch('v\\d+\\.(\\d+)([hs])', xs[0]).groups()
                width = {'h': 16, 's': 32}[unit]
                count = int(count)
            mask = (1 << width) - 1
            aa = read(xs[0])
            bb = read(xs[1])
            cc = read(xs[2])
            res = 0
            for i in range(count):
                a = signed(aa >> i * width & mask, width)
                b = signed(bb >> i * width & mask, width)
                c = signed(cc & mask if '[' in xs[2] else cc >> i * width & mask, width)
                value = a * (1 << width) + (1 if op == 'sqrdmlah' else -1) * 2 * b * c + (1 << width - 1) >> width
                value = max(-(1 << width - 1), min((1 << width - 1) - 1, value))
                res |= (value & mask) << i * width
            write(xs[0], res)
        else:
            raise AssertionError(line)
    ret = shape['return']
    bits = shape['width']
    return v[0] & (1 << bits * int(re.search('x([248])', ret)[1])) - 1 if 'x' in ret else w[0] & (1 << bits) - 1
checked = 0
for record in meta['records']:
    for seed in range(128):
        lhs = execute(functions['native_' + record['name']], seed, record)
        rhs = execute(functions['reference_' + record['name']], seed, record)
        assert lhs == rhs, (record['name'], seed, hex(lhs), hex(rhs))
        checked += 1
receipt = {'header_sha256': meta['header_sha256'], 'symbols': len(meta['records']), 'comparisons': checked, 'result': 'pass', 'scope': 'Python interpretation of generated big-endian register instructions versus Clang ACLE v8.1a code; no native big-endian execution. Metadata dependency replaced for freestanding compilation; exact production wrapper bodies retained.'}
(out / 'be-comparison.json').write_text(json.dumps(receipt, indent=2) + '\n')
print(json.dumps(receipt, indent=2))
