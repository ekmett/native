# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Inspect the same object used by the native source-target consumer."""
import argparse
import pathlib
import re
import subprocess
p=argparse.ArgumentParser()
p.add_argument('--objdump',required=True)
p.add_argument('--output',required=True,type=pathlib.Path)
p.add_argument('objects',nargs='+')
a=p.parse_args()
r=subprocess.run([a.objdump,'--demangle','--disassemble',*a.objects],capture_output=True,text=True)
a.output.write_text(r.stdout+r.stderr,encoding='utf-8')
if r.returncode: raise SystemExit(r.stderr)
functions=re.split(r'(?m)^([0-9a-f]+) <([^\n]+)>:\s*$',r.stdout)
kernels=[]
for i in range(1,len(functions),3):
    if 'source_kernel(' in functions[i+1]: kernels.append((functions[i+1],functions[i+2]))
if len(kernels)!=3: raise SystemExit(f'Expected precisely three source variants, found {len(kernels)}')
assembly='\n'.join(body for name,body in kernels)
if re.search(r'\b(?:zmm|ymm)\d+',assembly):
    if not re.search(r'vaddps[^\n]*ymm',assembly) or not re.search(r'vaddps[^\n]*zmm',assembly):
        raise SystemExit('Both native AVX2 and AVX512 packed additions are required.')
elif not re.search(r'\bfadd\s+v\d+\.4s',assembly):
    raise SystemExit('Native NEON four-lane addition is required.')
for name,body in kernels:
    if re.search(r'\b(?:callq?|bl)\s',body):
        raise SystemExit('Source variant unexpectedly contains a helper call: '+name)
print('Exactly three selected variants contain native vector additions without helper calls.')
