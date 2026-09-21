#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check ADX opcodes, widths and baseline admission."""
import argparse
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--objdump", required=True)
parser.add_argument("--output", required=True, type=Path)
parser.add_argument("--baseline-only", action="store_true")
parser.add_argument("--admission-only", action="store_true")
parser.add_argument("objects", nargs="+")
args = parser.parse_args()
result = subprocess.run([args.objdump, "--syms", "--disassemble",
                         "--no-show-raw-insn", *args.objects],
                        capture_output=True, text=True)
args.output.write_text(result.stdout + result.stderr, encoding="utf-8")
if result.returncode:
    raise SystemExit(result.stderr)

def functions(text):
    parts = re.split(r"(?m)^([0-9a-fA-F]+) <([^\n]+)>:\s*$", text)
    return [(int(parts[i], 16), parts[i + 1], parts[i + 2])
            for i in range(1, len(parts), 3)]

bodies = {}
objects = re.split(r"(?m)^.+:\s+file format ([^\n]+)$", result.stdout)
for index in range(1, len(objects), 2):
    file_format, contents = objects[index:index + 2]
    if file_format.startswith("mach-o"):
        # The first function can be labelled ltmp0. Resolve its global symbol
        # by section and address instead of depending on that printed label.
        sections = re.split(r"(?m)^Disassembly of section ([^\n]+):\s*$", contents)
        by_address = {}
        for section in range(1, len(sections), 2):
            for address, name, body in functions(sections[section + 1]):
                by_address[sections[section], address] = body
        symbols = re.findall(r"(?m)^([0-9a-f]+)\s+[gw]\s+F\s+(\S+)\s+([^\n]+)$", sections[0])
        for address, section, name in symbols:
            if name.lstrip("_").startswith("native_") or name.lstrip("_") == "main":
                bodies[name.lstrip("_")] = by_address.get((section, int(address, 16)), "")
    else:
        for address, name, body in functions(contents):
            if name.lstrip("_").startswith("native_") or name.lstrip("_") == "main":
                bodies[name.lstrip("_")] = body

def instructions(name):
    body = bodies.get(name, "")
    if not body:
        raise SystemExit(f"Missing disassembly for {name}.")
    result = re.findall(r"(?m)^\s*[0-9a-f]+:\s+([a-z][a-z0-9]*)\s*([^\n]*)$", body)
    if not result:
        raise SystemExit(f"No decoded instructions for {name}.")
    return result


baseline = "native_adx_baseline_import" if args.baseline_only else "native_adx_baseline"
if args.admission_only:
    baseline = "native_adx_admission"
for mnemonic, operands in instructions(baseline):
    if mnemonic.startswith(("v", "k", "aes", "adx")) or re.search(r"\b[yz]mm\d+\b", operands):
        raise SystemExit(f"Baseline contains optional instruction: {mnemonic}.")
    if re.fullmatch(r"(?:popcnt|lzcnt|tzcnt|pdep|pext|bzhi|bextr)[wlq]?", mnemonic):
        raise SystemExit(f"Baseline contains optional scalar instruction: {mnemonic}.")
if args.baseline_only:
    for mnemonic, operands in instructions("main"):
        if mnemonic.startswith(("v", "k", "aes", "adx")) or re.search(r"\b[yz]mm\d+\b", operands):
            raise SystemExit(f"Baseline main contains optional instruction: {mnemonic}.")
if args.baseline_only or args.admission_only:
    print("Baseline importer/admission body has no optional instructions.")
    raise SystemExit(0)
for width in (32, 64):
    suffix = "l" if width == 32 else "q"
    for operation, count in (("addcarryx", 1), ("chain", 3)):
        name = f"native_{operation}_{width}"
        decoded = instructions(name)
        carry_ops = [m for m, o in decoded if m in ("adc" + suffix, "adcx" + suffix, "adox" + suffix)]
        if len(carry_ops) != count:
            raise SystemExit(f"{name} must contain {count} scalar carry additions.")
        for mnemonic, operands in decoded:
            if mnemonic.startswith(("call", "j", "v", "k")) or re.search(r"\b[xyz]mm\d+\b", operands):
                raise SystemExit(f"{name} contains an unexpected instruction: {mnemonic}.")
print("Both widths use scalar carry additions; raw/public pairs determine exact overhead.")
print("Clang may use ADD/ADC; this check does not claim independent ADCX/ADOX chains.")
