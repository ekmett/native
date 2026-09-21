#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check PCLMUL encoding widths, immediate bytes and baseline imports."""
import argparse
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--objdump", required=True)
parser.add_argument("--output", required=True, type=Path)
parser.add_argument("--baseline-only", action="store_true")
parser.add_argument("objects", nargs="+")
args = parser.parse_args()
result = subprocess.run(
    [args.objdump, "--syms", "--disassemble", "--x86-asm-syntax=intel", *args.objects],
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
        # Mach-O may label the first function with a local section symbol.
        sections = re.split(r"(?m)^Disassembly of section ([^\n]+):\s*$", contents)
        by_address = {}
        for section in range(1, len(sections), 2):
            for address, name, body in functions(sections[section + 1]):
                by_address[sections[section], address] = body
        symbols = re.findall(r"(?m)^([0-9a-f]+)\s+[gw]\s+F\s+(\S+)\s+([^\n]+)$", sections[0])
        for address, section, name in symbols:
            if name.lstrip("_").startswith("native_pclmul_"):
                bodies[name.lstrip("_")] = by_address.get((section, int(address, 16)), "")
    else:
        for address, name, body in functions(contents):
            bodies[name.lstrip("_")] = body


def instructions(name):
    body = bodies.get("native_pclmul_" + name, "")
    if not body:
        raise SystemExit("Missing PCLMUL fixture function: " + name)
    rows = re.findall(
        r"(?m)^\s*[0-9a-f]+:\s+((?:[0-9a-f]{2}\s+)+)([a-z][a-z0-9]*)\s*([^\n]*)", body)
    if not rows:
        raise SystemExit("No decoded instructions for PCLMUL fixture: " + name)
    if any(mnemonic.startswith("call") for _, mnemonic, _ in rows):
        raise SystemExit("Unexpected helper call in PCLMUL fixture: " + name)
    return [(encoding.split(), mnemonic, operands) for encoding, mnemonic, operands in rows]


baseline = instructions("baseline")
for _, mnemonic, operands in baseline:
    if mnemonic.startswith(("v", "pclmul", "k")) or re.search(r"\b(?:[xyz]mm\d+|k[0-7])\b", operands):
        raise SystemExit("Omnibus import enabled optional instructions in baseline code")
if args.baseline_only:
    print("Omnibus importer retains scalar baseline instructions.")
    raise SystemExit(0)

for name, register, prefix in (("legacy", "xmm", "66"), ("vex128", "xmm", "c4"),
                               ("vex256", "ymm", "c4"), ("evex512", "zmm", "62"),
                               ("evex128", "xmm", "62"), ("evex256", "ymm", "62")):
    for immediate in (0, 1, 16, 17, 238, 239, 254, 255):
        rows = instructions(f"{name}_{immediate}")
        matches = [row for row in rows if "pclmul" in row[1]]
        if len(matches) != 1:
            raise SystemExit(f"{name}: expected one carry-less multiply")
        encoding, mnemonic, operands = matches[0]
        if encoding[0] != prefix or not re.search(r"\b" + register + r"\d+\b", operands):
            raise SystemExit(f"{name}: wrong encoding or width")
        # LLVM may canonicalize ignored immediate bits.
        if int(encoding[-1], 16) & 0x11 != immediate & 0x11:
            raise SystemExit(f"{name}: wrong half selectors")
        for encoding, mnemonic, operands in rows:
            if not name.startswith("evex") and (encoding[0] == "62" or re.search(r"\b(?:zmm\d+|k[0-7])\b", operands)):
                raise SystemExit(f"{name}: unexpected AVX-512 instruction")
            if name == "legacy" and mnemonic.startswith("v"):
                raise SystemExit("Legacy form requires AVX")
print("48 carry-less multiply probes retain legacy/VEX/EVEX widths and selectors; baseline import is scalar.")
