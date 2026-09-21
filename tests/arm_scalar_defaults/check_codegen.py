# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Compare every scalar default with explicit ISA and native instruction bodies."""
import argparse
from pathlib import Path
import re
import subprocess

p = argparse.ArgumentParser()
p.add_argument('--objdump', required=True)
p.add_argument('--output', type=Path, required=True)
p.add_argument('objects', nargs='+')
a = p.parse_args()
r = subprocess.run([a.objdump, '--syms', '--disassemble', '--no-show-raw-insn', *a.objects],
                   text=True, capture_output=True)
a.output.write_text(r.stdout + r.stderr, encoding='utf-8')
if r.returncode:
    raise SystemExit(r.stderr)
parts = re.split(r'(?m)^([0-9a-f]+) <([^\n]+)>:\s*$', r.stdout)
bodies = {parts[i+1].lstrip('_'): parts[i+2] for i in range(1, len(parts), 3)}
addresses = {int(parts[i], 16): parts[i+2] for i in range(1, len(parts), 3)}
for address, name in re.findall(r'(?m)^([0-9a-f]+)\s+[gw]\s+F\s+\S+\s+(_?native_default_\w+)$', r.stdout):
    if int(address, 16) in addresses:
        bodies.setdefault(name.lstrip('_'), addresses[int(address, 16)])

def instructions(body):
    return [re.sub(r'\s+', ' ', x.strip()) for x in re.findall(r'(?m)^\s*[0-9a-f]+:\s+(.*)$', body)
            if not re.match(r'(?:nop\w*|int3)\b', x.strip())]

expected = {f'crc32{c}_{width}': f'crc32{c}{op}'
            for c in ('', 'c') for width, op in ((8, 'b'), (16, 'h'), (32, 'w'), (64, 'x'))}
expected.update({f'{op}_{width}': op for op in ('sqrdmlah', 'sqrdmlsh') for width in (16, 32)})
expected.update(sha1h='sha1h', pmull='pmull')
for short, opcode in expected.items():
    name = 'native_default_' + short
    default, explicit, raw = [instructions(bodies.get(name + suffix, ''))
                              for suffix in ('', '_explicit', '_raw')]
    if not default or default != explicit or default != raw:
        raise SystemExit(f'{name}: instruction sequences differ\ndefault={default}\nexplicit={explicit}\nraw={raw}')
    if sum(x.split()[0].split('.')[0] == opcode for x in raw) != 1:
        raise SystemExit(f'{name}: expected exactly one {opcode}: {raw}')
    if any(re.match(r'(?:ld\w*|st\w*|bl|blr)\b', x) or re.search(r'\bsp\b', x) for x in default):
        raise SystemExit(f'{name}: unexpected memory, stack, or call: {default}')
print('All 14 scalar defaults exactly match explicit ISA and raw instructions; no memory, stack, or calls.')
