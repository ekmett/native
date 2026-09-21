# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check the separately targeted BMI2 wrappers in the built fixture object."""
import argparse
import pathlib
import re
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument("--objdump", required=True)
parser.add_argument("--output", required=True, type=pathlib.Path)
parser.add_argument("objects", nargs="+")
args = parser.parse_args()
result = subprocess.run(
    [args.objdump, "--syms", "--disassemble", "--no-show-raw-insn",
     "--x86-asm-syntax=intel", *args.objects], capture_output=True, text=True)
args.output.write_text(result.stdout + result.stderr, encoding="utf-8")
if result.returncode:
    raise SystemExit(result.stderr)

parts = re.split(r"(?m)^([0-9a-f]+) <([^\n]+)>:\s*$", result.stdout)
bodies = {parts[i + 1].lstrip("_"): parts[i + 2] for i in range(1, len(parts), 3)}
# Mach-O can label the first function with a local section symbol. Resolve its
# public symbol by address when the label is not the public wrapper name.
addresses = {int(parts[i], 16): parts[i + 2] for i in range(1, len(parts), 3)}
for address, name in re.findall(
        r"(?m)^([0-9a-f]+)\s+[gw]\s+F\s+\S+\s+(_?native_\w+)$", result.stdout):
    if int(address, 16) in addresses:
        bodies.setdefault(name.lstrip("_"), addresses[int(address, 16)])

def body(name):
    found = bodies.get("native_" + name)
    if not found:
        raise SystemExit("Missing BMI2 fixture function: " + name)
    if re.search(r"\b[xyz]mm\d+\b", found):
        raise SystemExit("BMI2 scalar wrapper unexpectedly uses vector registers: " + name)
    return found

for mnemonic in ("bzhi", "shlx", "shrx", "sarx", "rorx"):
    for width in (32, 64):
        name = mnemonic + str(width)
        code = body(name)
        destination = r"e(?:ax|bx|cx|dx|si|di|bp|sp)|r\d+d" if width == 32 else r"r(?:ax|bx|cx|dx|si|di|bp|sp|\d+)"
        if not re.search(r"\b" + mnemonic + r"\s+(?:" + destination + r"),", code):
            raise SystemExit("Missing native " + name + " lowering")

code64 = body("mulx64")
product64 = re.search(r"\bmulx\s+(r\w+),\s*(r\w+),", code64)
if not product64 or product64[1] == product64[2]:
    raise SystemExit("64-bit MULX must retain both product halves")
if not re.search(r"\bmov\s+qword ptr \[[^\n]+\],\s*" + product64[1] + r"\b", code64):
    raise SystemExit("64-bit MULX must store the high product")

code32 = body("mulx32")
# LLVM can profitably implement the 32-bit intrinsic using a 64-bit IMUL.
if not re.search(r"\bmulx\b", code32):
    if not re.search(r"\bimul\s+r\w+,", code32) or not re.search(r"\bshr\s+r\w+,\s*(?:32|0x20)\b", code32):
        raise SystemExit("32-bit MULX must retain a full 64-bit product")
if not re.search(r"\bmov\s+dword ptr \[[^\n]+\],\s*\w+", code32):
    raise SystemExit("32-bit MULX must store the high product")
print("BMI2 32/64-bit shifts, rotates, BZHI, and both multiplication halves verified.")
