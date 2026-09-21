#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check VEX F16C forms, discarded results, immediates and baseline imports."""
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
            if name.lstrip("_").startswith("native_f16c_"):
                bodies[name.lstrip("_")] = by_address.get((section, int(address, 16)), "")
    else:
        for address, name, body in functions(contents):
            bodies[name.lstrip("_")] = body


def instructions(name):
    body = bodies.get("native_f16c_" + name, "")
    if not body:
        raise SystemExit("Missing F16C fixture function: " + name)
    rows = re.findall(
        r"(?m)^\s*[0-9a-f]+:\s+((?:[0-9a-f]{2}\s+)+)([a-z][a-z0-9]*)\s*([^\n]*)", body)
    if not rows:
        raise SystemExit("No decoded instructions for F16C fixture: " + name)
    if any(mnemonic.startswith("call") for _, mnemonic, _ in rows):
        raise SystemExit("Unexpected helper call in F16C fixture: " + name)
    return [(encoding.split(), mnemonic, operands) for encoding, mnemonic, operands in rows]


baseline = instructions("baseline")
for _, mnemonic, operands in baseline:
    if mnemonic.startswith(("v", "k")) or re.search(r"\b(?:[xyz]mm\d+|k[0-7])\b", operands):
        raise SystemExit("Omnibus import enabled optional instructions in baseline code")
if args.baseline_only:
    print("Omnibus importer retains scalar baseline instructions.")
    raise SystemExit(0)

for name, opcode, width, immediate in (
    ("narrow1", "vcvtps2ph", 128, 0), ("narrow4", "vcvtps2ph", 128, 3),
    ("narrow8", "vcvtps2ph", 256, 4), ("widen1", "vcvtph2ps", 128, None),
    ("widen4", "vcvtph2ps", 128, None), ("widen8", "vcvtph2ps", 256, None),
    ("discard_narrow1", "vcvtps2ph", 128, 8), ("discard_narrow4", "vcvtps2ph", 128, 255),
    ("discard_narrow8", "vcvtps2ph", 256, 7), ("discard_widen1", "vcvtph2ps", 128, None),
    ("load_widen4", "vcvtph2ps", 128, None), ("discard_load_widen4", "vcvtph2ps", 128, None),
    ("discard_widen4", "vcvtph2ps", 128, None), ("discard_widen8", "vcvtph2ps", 256, None)):
    rows = instructions(name)
    matches = [row for row in rows if row[1] == opcode]
    if len(matches) != 1:
        raise SystemExit(f"{name}: expected exactly one {opcode}")
    encoding, _, operands = matches[0]
    if encoding[0] != "c4" or bool(int(encoding[2], 16) & 4) != (width == 256):
        raise SystemExit(f"{name}: expected VEX.{width}")
    if immediate is not None and int(encoding[-1], 16) != immediate:
        raise SystemExit(f"{name}: wrong immediate byte")
    for encoded, mnemonic, operands in rows:
        if encoded[0] == "62" or re.search(r"\b(?:zmm\d+|k[0-7])\b", operands):
            raise SystemExit(f"{name}: unexpected AVX-512 instruction")
print("Six used and six discarded conversions retain exact F16C VEX instructions; baseline import is scalar.")
