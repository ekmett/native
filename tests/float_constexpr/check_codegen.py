# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Compare floating SIMD leaves with native expressions under the same ABI."""
import argparse
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument("--objdump", required=True)
parser.add_argument("--output", required=True, type=Path)
parser.add_argument("objects", nargs="+")
args = parser.parse_args()
result = subprocess.run([args.objdump, "--syms", "--disassemble", "--reloc", "--full-contents", "--no-show-raw-insn",
                         *args.objects], text=True, capture_output=True)
args.output.write_text(result.stdout + result.stderr, encoding="utf-8")
if result.returncode:
    raise SystemExit(result.stderr)
parts = re.split(r"(?m)^([0-9a-f]+) <([^\n]+)>:\s*$", result.stdout)
bodies = {parts[i+1].lstrip("_"): parts[i+2] for i in range(1, len(parts), 3)}
addresses = {int(parts[i], 16): parts[i+2] for i in range(1, len(parts), 3)}
# Mach-O may label the first body with ltmp0 instead of its exported name.
for address, name in re.findall(r"(?m)^([0-9a-f]+)\s+[gw]\s+F\s+\S+\s+(_?native_float_\w+)$", result.stdout):
    if int(address, 16) in addresses:
        bodies.setdefault(name.lstrip("_"), addresses[int(address, 16)])

# Mach-O can retain duplicate equal literals from two inline-definition owners.
# Compare the bytes, not their pool placement; never discard the load itself.
literal_bytes = {}
literal_width = {}
for match in re.finditer(r"Contents of section __TEXT,__literal(4|16):\n(.*?)(?=Contents of section|Disassembly of section|\Z)", result.stdout, re.S):
    width = int(match[1])
    for address, groups in re.findall(r"(?m)^\s*([0-9a-f]+)\s+((?:[0-9a-f]{8} ?){1,4})", match[2]):
        start = int(address, 16)
        for offset, byte in enumerate(bytes.fromhex(groups)):
            literal_bytes[start + offset] = byte
            literal_width[start + offset] = width

literal_symbols = {}
for address, width, name in re.findall(r"(?m)^([0-9a-f]+)\s+l\s+O\s+__TEXT,__literal(4|16)\s+(\S+)$", result.stdout):
    address = int(address, 16)
    literal_symbols[name] = bytes(literal_bytes[address+i] for i in range(int(width))).hex()

# ELF labels carry section-relative offsets; preserve addends while replacing
# only equal constant-pool labels with their verified bytes.
for section in re.finditer(r"Contents of section (\.rodata\.cst(4|16)):\n(.*?)(?=Contents of section|Disassembly of section|\Z)", result.stdout, re.S):
    data = {}
    for address, groups in re.findall(r"(?m)^\s*([0-9a-f]+)\s+((?:[0-9a-f]{8} ?){1,4})", section[3]):
        start = int(address, 16)
        data.update((start+i, byte) for i, byte in enumerate(bytes.fromhex(groups)))
    pattern = r"(?m)^([0-9a-f]+)\s+l\s+(?:O\s+)?" + re.escape(section[1]) + r"\s+[0-9a-f]+\s+(\S+)$"
    for address, name in re.findall(pattern, result.stdout):
        start = int(address, 16)
        literal_symbols[name] = bytes(data[start+i] for i in range(int(section[2]))).hex()

def instructions(body, name):
    result = []
    for line in re.findall(r"(?m)^\s*[0-9a-f]+:\s+(.*)$", body):
        line = re.sub(r"\s+", " ", line.strip())
        target = re.search(r"## 0x([0-9a-f]+)", line)
        if "(%rip)" in line and target and int(target[1], 16) in literal_width:
            address = int(target[1], 16)
            data = bytes(literal_bytes[address + i] for i in range(literal_width[address]))
            line = re.sub(r"-?0x[0-9a-f]+\(%rip\)", "literal(" + data.hex() + ")(%rip)", line)
        # Disassembler comments are section-layout annotations, not operands.
        line = line.split(" ## ", 1)[0]
        if re.match(r"(?:nop\w*|int3)\b", line):
            continue
        # An unresolved relocation names its actual target on the next line.
        # Normalize only this function's address; retain relocation type/name.
        line = re.sub(r"0x[0-9a-f]+ <_?" + re.escape(name) + r"(\+0x[0-9a-f]+)?>",
                      lambda m: "<self" + (m[1] or "") + ">", line)
        for symbol, data in literal_symbols.items():
            line = re.sub(r"(?<![\w.])" + re.escape(symbol) + r"(?![\w.])", "literal("+data+")", line)
        result.append(line)
    return result

names = sorted(name for name in bodies if name.startswith("native_float_") and not name.endswith("_raw"))
expected = 24 if any(name.startswith("native_float_arm4_") for name in names) else 42
if len(names) != expected:
    raise SystemExit(f"Expected {expected} public floating leaves, found {len(names)}")
for name in names:
    public = instructions(bodies[name], name)
    raw = instructions(bodies.get(name + "_raw", ""), name + "_raw")
    if not public or public != raw:
        raise SystemExit(f"{name}: floating wrapper differs from native expression\npublic: {public}\nnative: {raw}")
print(f"{len(names)} floating leaves match native expressions exactly")
