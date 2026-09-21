#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check IFMA opcodes, widths and baseline admission."""
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
    body = re.sub(r"\{vex\}\s*", "", body)
    result = re.findall(r"(?m)^\s*[0-9a-f]+:\s+([a-z][a-z0-9]*)\s*([^\n]*)$", body)
    if not result:
        raise SystemExit(f"No decoded instructions for {name}.")
    return result


baseline = "native_ifma_baseline_import" if args.baseline_only else "native_ifma_baseline"
if args.admission_only:
    baseline = "native_ifma_admission"
for mnemonic, operands in instructions(baseline):
    if mnemonic.startswith(("v", "k", "aes", "ifma")) or re.search(r"\b[yz]mm\d+\b", operands):
        raise SystemExit(f"Baseline contains optional instruction: {mnemonic}.")
    if re.fullmatch(r"(?:popcnt|lzcnt|tzcnt|pdep|pext|bzhi|bextr)[wlq]?", mnemonic):
        raise SystemExit(f"Baseline contains optional scalar instruction: {mnemonic}.")
if args.baseline_only:
    for mnemonic, operands in instructions("main"):
        if mnemonic.startswith(("v", "k", "aes", "ifma")) or re.search(r"\b[yz]mm\d+\b", operands):
            raise SystemExit(f"Baseline main contains optional instruction: {mnemonic}.")
if args.baseline_only or args.admission_only:
    print("Baseline importer/admission body has no optional instructions.")
    raise SystemExit(0)
for form, width, masked in (("vex128", 128, False), ("vex256", 256, False),
                          ("evex128", 128, True), ("evex256", 256, True), ("evex512", 512, True)):
    register = {128: "xmm", 256: "ymm", 512: "zmm"}[width]
    for half in ("lo", "hi"):
        opcode = "vpmadd52luq" if half == "lo" else "vpmadd52huq"
        for mode in (("plain", "mask", "maskz") if masked else ("plain",)):
            prefix = "" if mode == "plain" else mode + "_"
            name = f"native_{prefix}madd52{half}_{form}"
            decoded = instructions(name)
            if ("{vex}" in bodies[name]) != form.startswith("vex"):
                raise SystemExit(f"{name} has the wrong VEX/EVEX encoding.")
            selected = [(m, o) for m, o in decoded if m.startswith("vpmadd52")]
            if len(selected) != 1 or selected[0][0] != opcode:
                raise SystemExit(f"{name} must contain exactly one {opcode}.")
            operands = selected[0][1]
            if not re.search(rf"\b{register}\d+\b", operands):
                raise SystemExit(f"{name} has the wrong width.")
            has_mask = bool(re.search(r"\{\s*%?k[1-7]\s*\}", operands))
            has_zero = bool(re.search(r"\{\s*z\s*\}", operands))
            if has_mask != (mode != "plain") or has_zero != (mode == "maskz"):
                raise SystemExit(f"{name} has incorrect hardware masking.")
            for mnemonic, operands in decoded:
                if mnemonic.startswith(("call", "j")):
                    raise SystemExit(f"{name} contains helper call/control flow: {mnemonic}.")
print("All 22 IFMA forms contain the expected opcode, width and hardware mask.")
