# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check isolated I8MM wrappers and their baseline control."""
import argparse
import pathlib
import re
import subprocess
p=argparse.ArgumentParser()
p.add_argument('--objdump',required=True)
p.add_argument('--output',type=pathlib.Path,required=True)
p.add_argument('objects',nargs='+')
a=p.parse_args()
r=subprocess.run([a.objdump,'--syms','--disassemble','--no-show-raw-insn',*a.objects],capture_output=True,text=True,check=True)
a.output.write_text(r.stdout+r.stderr)
parts=re.split(r'(?m)^([0-9a-f]+) <([^\n]+)>:\s*$',r.stdout)
bodies={parts[i+1].lstrip('_'):parts[i+2] for i in range(1,len(parts),3)}
# Mach-O may name the first function after its local section symbol. Resolve
# public wrapper names through their symbol-table addresses as well.
addresses={int(parts[i],16):parts[i+2] for i in range(1,len(parts),3)}
for address,name in re.findall(r'(?m)^([0-9a-f]+)\s+[gw]\s+F\s+\S+\s+(_?native_\w+)$',r.stdout):
    if int(address,16) in addresses:
        bodies.setdefault(name.lstrip('_'),addresses[int(address,16)])
for name,op in [('smmla','smmla'),('ummla','ummla'),('usmmla','usmmla'),('usdot','usdot'),('usdot_lane','usdot'),('sudot_lane','sudot')]:
    body=bodies.get('native_'+name,'')
    if not re.search(r'\b'+op+r'\b',body):
        raise SystemExit('Missing native '+name+' instruction')
    if name.endswith('_lane') and not re.search(r'\[3\]',body):
        raise SystemExit('Missing lane-3 immediate for '+name)
body=bodies.get('native_baseline','')
if not body or re.search(r'\b(?:smmla|ummla|usmmla|usdot|sudot)\b',body):
    raise SystemExit('Invalid baseline control')
print('I8MM instructions, lane immediates and baseline control verified')
