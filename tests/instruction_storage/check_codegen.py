# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Compare half-register representation bridges with identity under the same ABI."""
import argparse
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument("--objdump", required=True)
parser.add_argument("--output", required=True, type=Path)
parser.add_argument("objects", nargs="+")
args = parser.parse_args()
result = subprocess.run([args.objdump, "--syms", "--disassemble", "--no-show-raw-insn",
                         *args.objects], text=True, capture_output=True)
args.output.write_text(result.stdout + result.stderr, encoding="utf-8")
if result.returncode:
    raise SystemExit(result.stderr)
parts = re.split(r"(?m)^([0-9a-f]+) <([^\n]+)>:\s*$", result.stdout)
bodies = {parts[i+1].lstrip("_"): parts[i+2] for i in range(1, len(parts), 3)}
addresses = {int(parts[i], 16): parts[i+2] for i in range(1, len(parts), 3)}
# Mach-O may label the first body with ltmp0 instead of its exported name.
for address, name in re.findall(r"(?m)^([0-9a-f]+)\s+[gw]\s+F\s+\S+\s+(_?native_storage_\w+)$", result.stdout):
    if int(address, 16) in addresses:
        bodies.setdefault(name.lstrip("_"), addresses[int(address, 16)])

def instructions(body):
    # Alignment padding may differ with symbol placement, not with the operation.
    return [re.sub(r"\s+", " ", line.strip()) for line in
            re.findall(r"(?m)^\s*[0-9a-f]+:\s+(.*)$", body)
            if not re.match(r"(?:nop\w*|int3)\b", line.strip())]

names = sorted(name for name in bodies if name.startswith("native_storage_") and not name.endswith("_raw"))
expected = 8 if any(name.startswith("native_storage_arm_") for name in names) else 4
if len(names) != expected:
    raise SystemExit(f"Expected {expected} public bit bridges, found {len(names)}")
for name in names:
    public = instructions(bodies[name])
    raw = instructions(bodies.get(name + "_raw", ""))
    if not public or public != raw:
        raise SystemExit(f"{name}: representation bridge adds instructions\npublic: {public}\nidentity: {raw}")
print(f"{len(names)} half bits/from_bits bridges match register identity exactly")
