# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check every crypto wrapper and immediate from a baseline translation unit."""
import argparse
import json
from pathlib import Path
import re
import subprocess

parser=argparse.ArgumentParser()
parser.add_argument('--objdump',required=True)
parser.add_argument('--output',required=True,type=Path)
parser.add_argument('objects',nargs='+')
args=parser.parse_args()
result=subprocess.run([args.objdump,'--syms','--disassemble','--no-show-raw-insn',
    '--triple=aarch64-none-linux-gnu',*args.objects],capture_output=True,text=True)
args.output.write_text(result.stdout+result.stderr)
if result.returncode: raise SystemExit(result.stderr)
parts=re.split(r'(?m)^([0-9a-f]+) <([^\n]+)>:\s*$',result.stdout)
bodies={parts[i+1].lstrip('_'):parts[i+2] for i in range(1,len(parts),3)}
addresses={int(parts[i],16):parts[i+2] for i in range(1,len(parts),3)}
for address,name in re.findall(r'(?m)^([0-9a-f]+)\s+[gw]\s+F\s+\S+\s+(_?native_\w+)$',result.stdout):
    if int(address,16) in addresses: bodies.setdefault(name.lstrip('_'),addresses[int(address,16)])
checks=json.loads(Path(__file__).with_name('instructions.json').read_text())
# ACLE reverses vector lane order on big-endian targets. Clang selects the high
# polynomial inputs with EXT before a low-lane PMULL in that representation.
big_endian=all(Path(obj).read_bytes()[:6] == b'\x7fELF\x02\x02' for obj in args.objects)
if big_endian:
    for name in ('native_crypto_pmull_1','native_crypto_pmull_3'):
        body=bodies.get(name,'')
        if len(re.findall(r'\bext\s+.*#(?:8|0x8)\b',body)) < 2:
            raise SystemExit(f'{name}: missing big-endian high-lane extraction')
        checks[name]=r'\bpmull\s'
for name,pattern in checks.items():
    if not re.search(pattern,bodies.get(name,'')):
        raise SystemExit(f'{name}: missing instruction {pattern}\n{bodies.get(name,"missing symbol")}')
baseline=bodies.get('native_crypto_baseline')
if baseline is None or re.search(r'\b(?:aes\w*|pmull2?|sha\w*|eor3|bcax|rax1|xar)\s',baseline):
    raise SystemExit('Crypto instructions escaped into the baseline control')
print(f'{len(checks)} instruction forms and baseline isolation verified')

paired = list(checks) + ["native_crypto_pmull_vector", "native_crypto_pmull2_vector"]

# Compare the public semantic boundary with the same target and raw signature.
# Reject memory traffic or calls even if an expected instruction is still present.
def sequence(body):
    return [re.sub(r"\s+", " ", line.strip()) for line in
            re.findall(r"(?m)^\s*[0-9a-f]+:\s+(.*)$", body)
            if line.strip() != "nop"]

for name in paired:
    public = sequence(bodies.get(name, ""))
    direct = sequence(bodies.get(name + "_raw", ""))
    if not public or public != direct:
        raise SystemExit(f"{name}: public/raw instruction sequences differ\npublic: {public}\nraw: {direct}")
    if any(re.match(r"(?:ld\w*|st\w*|bl|blr)\b", line) or re.search(r"\bsp\b", line) for line in public):
        raise SystemExit(f"{name}: unexpected memory, stack or out-of-line call: {public}")
print(f"{len(paired)} public/raw instruction sequences match without loads, stores, spills or calls")
