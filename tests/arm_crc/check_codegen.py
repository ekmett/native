# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
import argparse,pathlib,re,subprocess
p=argparse.ArgumentParser();p.add_argument('--objdump',required=True);p.add_argument('--output',required=True,type=pathlib.Path);p.add_argument('objects',nargs='+');a=p.parse_args()
r=subprocess.run([a.objdump,'--syms','--disassemble','--no-show-raw-insn',*a.objects],capture_output=True,text=True,check=True)
a.output.write_text(r.stdout+r.stderr)
parts=re.split(r'(?m)^([0-9a-f]+) <([^\n]+)>:\s*$',r.stdout)
bodies={parts[i+1].lstrip('_'):parts[i+2] for i in range(1,len(parts),3)}
# Mach-O may label the first body by its section symbol; recover its public name.
addresses={int(parts[i],16):parts[i+2] for i in range(1,len(parts),3)}
for address,name in re.findall(r'(?m)^([0-9a-f]+)\s+[gw]\s+F\s+\S+\s+(_?native_\w+)$',r.stdout):
 if int(address,16) in addresses:bodies.setdefault(name.lstrip('_'),addresses[int(address,16)])
for name,stem in [('crc32','crc32'),('crc32c','crc32c')]:
 for width,suffix in [(8,'b'),(16,'h'),(32,'w'),(64,'x')]:
  body=bodies.get(f'native_{name}_{width}','')
  if len(re.findall(r'\b'+stem+suffix+r'\b',body))!=1 or re.search(r'\bbl\b',body):
   raise SystemExit(f'Expected one inlined {stem+suffix} in {name}_{width}: {body}')
body=bodies.get('native_crc_baseline','')
if not body or re.search(r'\bcrc32',body):raise SystemExit('Missing or nonbaseline control')
print('All eight CRC instructions inline at their exact widths; baseline remains clear')
