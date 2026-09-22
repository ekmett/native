#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Require all fourteen hardware operations and reject calls or tail calls."""
import argparse
import re
import subprocess
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--objdump',required=True)
p.add_argument('object')
a=p.parse_args()
s=subprocess.check_output([a.objdump,'--disassemble','--no-show-raw-insn',a.object],text=True)
for op in ('vbcstnebf162ps','vbcstnesh2ps','vcvtneebf162ps','vcvtneeph2ps',
           'vcvtneobf162ps','vcvtneoph2ps','vcvtneps2bf16'):
    if len(re.findall(r'\b'+op+r'(?:x|y)?\b',s))!=2:
        raise SystemExit(f'Expected both widths of {op}:\n{s}')
# Module initializers may call other initializers; only inspect test wrappers.
for body in re.split(r'(?m)^[0-9a-f]+ <',s)[1:]:
    name=body.split('>:',1)[0]
    if name.lstrip('_').startswith('native_') or name=='ltmp0':
        if re.search(r'\b(call\w*|jmp\w*)\s',body):
            raise SystemExit(f'Unexpected out-of-line call in {body}')
print('All fourteen instruction forms are present; wrappers contain no calls.')
