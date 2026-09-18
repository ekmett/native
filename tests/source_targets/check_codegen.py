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
r=subprocess.run([a.objdump,'--demangle','--syms','--disassemble',*a.objects],capture_output=True,text=True)
a.output.write_text(r.stdout+r.stderr,encoding='utf-8')
if r.returncode: raise SystemExit(r.stderr)
if re.search(r'__cpu_indicator_init|__cpu_model|source_scalar[^\n]*resolver',r.stdout):
    raise SystemExit('Scalar source scope unexpectedly emits compiler multiversioning machinery.')
def function_bodies(text):
    functions=re.split(r'(?m)^([0-9a-f]+) <([^\n]+)>:\s*$',text)
    return [(int(functions[i],16),functions[i+1],functions[i+2])
        for i in range(1,len(functions),3)]

def source_kernel(name):
    # LLVM's Microsoft demangler does not yet decode structural array NTTPs.
    # Match the exact raw template name, excluding helper functions and thunks.
    return bool(re.search(r'\bsource_kernel[<(]',name) or name.startswith('??$source_kernel@'))

kernels=[]
objects=re.split(r'(?m)^.+:\s+file format ([^\n]+)$',r.stdout)
for i in range(1,len(objects),2):
    file_format,contents=objects[i:i+2]
    if file_format.startswith('mach-o'):
        # Mach-O's local section symbol ltmp0 can be the displayed label for
        # the first function. Count the defined global kernel symbols instead,
        # and resolve each body within this object and section, never by name
        # or address alone across all input objects.
        sections=re.split(r'(?m)^Disassembly of section ([^\n]+):\s*$',contents)
        bodies={}
        for j in range(1,len(sections),2):
            for address,name,body in function_bodies(sections[j+1]):
                bodies[sections[j],address]=body
        symbols=re.findall(r'(?m)^([0-9a-f]+)\s+g\s+F\s+(\S+)\s+([^\n]+)$',sections[0])
        for address,section,name in symbols:
            if not source_kernel(name): continue
            body=bodies.get((section,int(address,16)))
            if body is None or not re.search(r'(?m)^\s*[0-9a-f]+:',body):
                raise SystemExit('Source variant has no disassembled body: '+name)
            kernels.append((name,body))
    else:
        kernels.extend((name,body) for address,name,body in function_bodies(contents)
            if source_kernel(name))
if len(kernels)!=3: raise SystemExit(f'Expected precisely three source variants, found {len(kernels)}')
for name,body in kernels:
    if not re.search(r'\bvaddps\b|\bfadd(?:\s+v\d+\.4s|\.4s\s+v\d+)',body):
        raise SystemExit('Source variant has no native packed addition: '+name)
assembly='\n'.join(body for name,body in kernels)
if re.search(r'\b(?:zmm|ymm)\d+',assembly):
    if not re.search(r'vaddps[^\n]*ymm',assembly) or not re.search(r'vaddps[^\n]*zmm',assembly):
        raise SystemExit('Both native AVX2 and AVX512 packed additions are required.')
elif not re.search(r'\bfadd(?:\s+v\d+\.4s|\.4s\s+v\d+)',assembly):
    raise SystemExit('Native NEON four-lane addition is required.')
instrumented=any('source_targets_asan_instrumented' in name
    for address,name,body in function_bodies(r.stdout)) or re.search(
        r'(?m)^[0-9a-f]+\s+g\s+F\s+\S+\s+_?source_targets_asan_instrumented$',r.stdout)
if instrumented:
    print('ASan object: native instructions and variant count checked; instrumentation calls permitted.')
else:
    for name,body in kernels:
        if re.search(r'\b(?:callq?|bl)\s',body):
            raise SystemExit('Source variant unexpectedly contains a helper call: '+name)
    print('Exactly three selected variants contain native vector additions without helper calls.')
