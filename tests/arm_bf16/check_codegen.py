# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Inspect independently targeted wrappers emitted from an ARMv8-A translation unit."""
import argparse
import json
import pathlib
import re
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument("--objdump", required=True)
parser.add_argument("--output", required=True, type=pathlib.Path)
parser.add_argument("objects", nargs="+")
args = parser.parse_args()
result = subprocess.run([args.objdump, "--syms", "--disassemble", "--no-show-raw-insn",
                         "--triple=aarch64-none-linux-gnu",
                         *args.objects], capture_output=True, text=True)
args.output.write_text(result.stdout + result.stderr, encoding="utf-8")
if result.returncode:
    raise SystemExit(result.stderr)
parts = re.split(r"(?m)^([0-9a-f]+) <([^\n]+)>:\s*$", result.stdout)
bodies = {parts[i + 1].lstrip("_"): parts[i + 2] for i in range(1, len(parts), 3)}
addresses = {int(parts[i], 16): parts[i + 2] for i in range(1, len(parts), 3)}
for address, name in re.findall(r"(?m)^([0-9a-f]+)\s+[gw]\s+F\s+\S+\s+(_?native_\w+)$", result.stdout):
    if int(address, 16) in addresses:
        bodies.setdefault(name.lstrip("_"), addresses[int(address, 16)])
checks = json.loads(pathlib.Path(__file__).with_name("codegen_patterns.json").read_text())
for name, pattern in checks.items():
    if not re.search(pattern, bodies.get(name, "")):
        raise SystemExit("Missing expected instruction in " + name + ": " + bodies.get(name, "absent"))
    if len(re.findall(r"\b(?:bfdot|bfmmla|bfmlalb|bfmlalt)\s", bodies[name])) != 1:
        raise SystemExit("Expected one BF16 instruction in " + name)
    if re.search(r"\bbl\b", bodies[name]):
        raise SystemExit("Unexpected out-of-line call in " + name)
if "native_baseline" not in bodies:
    raise SystemExit("Missing baseline control")
if re.search(r"\b(?:bfdot|bfmmla|bfmlalb|bfmlalt)\b", bodies["native_baseline"]):
    raise SystemExit("Optional instruction leaked into baseline function")
for opcode in ("bfdot", "bfmmla", "bfmlalb", "bfmlalt"):
    assert len(re.findall(r"\b" + opcode + r"\s", bodies.get("native_discard", ""))) == 1, opcode
assert len(re.findall(r"\bbfdot\s", bodies.get("native_environment", ""))) == 2
assert re.search(r"\bmsr\s+FPCR", bodies["native_environment"], re.I)
print(f"Verified {len(checks)} instruction forms, baseline isolation, discarded results and FPCR ordering")
