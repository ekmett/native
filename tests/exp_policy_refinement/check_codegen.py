# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check the native object that also supplies the admitted execution tests."""
import argparse
from pathlib import Path
import re
import subprocess

p = argparse.ArgumentParser()
p.add_argument('--objdump', required=True)
p.add_argument('--output', required=True, type=Path)
p.add_argument('objects', nargs='+')
a = p.parse_args()
r = subprocess.run([a.objdump, '--demangle', '--disassemble', *a.objects],
                   capture_output=True, text=True)
a.output.write_text(r.stdout + r.stderr, encoding='utf-8')
if r.returncode:
    raise SystemExit(r.stderr)
labels = re.split(r'(?m)^[0-9a-f]+ <([^\n]+)>:\s*$', r.stdout)
bodies = {labels[i]: labels[i+1] for i in range(1, len(labels), 2)
          if labels[i].startswith('refined_codegen_')}
if len(bodies) != 22:
    raise SystemExit(f'Expected 22 exp codegen entries, found {len(bodies)}')
vl = {'exp_full_half', 'avx512_bf16', 'avx512_fp16', 'avx512', 'exp_vl'}
instrumented = 'exp_refinement_asan_instrumented' in r.stdout
for name, body in bodies.items():
    target, width = name.removeprefix('refined_codegen_').rsplit('_', 1)
    if not re.search(r'\bvfmadd\w*ps\b', body):
        raise SystemExit('Missing polynomial packed FMA: ' + name)
    native_scale = width == 'native' and target != 'avx2' or target in vl
    has_scale = bool(re.search(r'\bvscalefps\b', body))
    if has_scale != native_scale:
        raise SystemExit(f'SCALEF cut mismatch: {name}, found={has_scale}, expected={native_scale}')
    if not instrumented and re.search(r'\bcallq?\s', body):
        raise SystemExit('Unexpected outlined helper call: ' + name)
    if width == 'native' and target != 'avx2' and not re.search(r'\bzmm\d+', body):
        raise SystemExit('Missing sixteen-lane native register: ' + name)
    if width == 'narrow' and not re.search(r'\bymm\d+', body):
        raise SystemExit('Missing eight-lane register: ' + name)
print('22 attributed exp entries: packed FMA, exact F/VL scaling cuts, native widths; ' +
      ('ASan instrumentation calls permitted.' if instrumented else 'no helper calls.'))
