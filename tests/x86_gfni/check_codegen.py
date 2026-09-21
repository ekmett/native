#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check GFNI encoding widths, immediate bytes, masks and baseline imports."""
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
            if name.lstrip("_").startswith("native_gfni_"):
                bodies[name.lstrip("_")] = by_address.get((section, int(address, 16)), "")
    else:
        for address, name, body in functions(contents):
            bodies[name.lstrip("_")] = body


def instructions(name):
    body = bodies.get("native_gfni_" + name, "")
    if not body:
        raise SystemExit("Missing GFNI fixture function: " + name)
    rows = re.findall(
        r"(?m)^\s*[0-9a-f]+:\s+((?:[0-9a-f]{2}\s+)+)([a-z][a-z0-9]*)\s*([^\n]*)", body)
    if not rows:
        raise SystemExit("No decoded instructions for GFNI fixture: " + name)
    if any(mnemonic.startswith("call") for _, mnemonic, _ in rows):
        raise SystemExit("Unexpected helper call in GFNI fixture: " + name)
    return [(encoding.split(), mnemonic, operands) for encoding, mnemonic, operands in rows]


baseline = instructions("baseline")
for _, mnemonic, operands in baseline:
    if mnemonic.startswith(("v", "gf2p8", "k")) or re.search(r"\b(?:[xyz]mm\d+|k[0-7])\b", operands):
        raise SystemExit("Omnibus import enabled optional instructions in baseline code")
if args.baseline_only:
    print("Omnibus importer retains scalar baseline instructions.")
    raise SystemExit(0)

for width, register in ((128, "xmm"), (256, "ymm"), (512, "zmm")):
    for operation, opcode, immediate in (
            ("mul", "gf2p8mulb", None),
            ("affine", "gf2p8affineqb", 0x63),
            ("inverse", "gf2p8affineinvqb", 0xa5)):
        for suffix in ("", "_mask", "_maskz"):
            name = f"{operation}{suffix}{width}"
            rows = instructions(name)
            expected_opcode = opcode if width == 128 and not suffix else "v" + opcode
            matches = [row for row in rows if row[1] == expected_opcode]
            if len(matches) != 1:
                raise SystemExit(f"{name}: expected exactly one {expected_opcode}")
            encoding, _, operands = matches[0]
            expected_prefix = "62" if suffix or width == 512 else ("c4" if width == 256 else "66")
            if encoding[0] != expected_prefix or not re.search(r"\b" + register + r"\d+\b", operands):
                raise SystemExit(f"{name}: wrong instruction encoding or register width")
            if immediate is not None and (int(encoding[-1], 16) != immediate or
                    not re.search(r",\s*(?:" + str(immediate) + "|0x" + format(immediate, "x") + r")\b", operands)):
                raise SystemExit(f"{name}: wrong immediate byte")
            has_mask = bool(re.search(r"\{k[1-7]\}", operands))
            if has_mask != bool(suffix) or ("{z}" in operands) != (suffix == "_maskz"):
                raise SystemExit(f"{name}: wrong merge or zero byte predicate")
            if not suffix and width < 512:
                for _, mnemonic, operands in rows:
                    if re.search(r"\b(?:zmm\d+|k[0-7])\b", operands):
                        raise SystemExit(f"{name}: unexpected AVX-512 register")
                    if width == 128 and mnemonic.startswith("v"):
                        raise SystemExit(f"{name}: SSE form unexpectedly requires AVX")

print("27 GFNI forms retain legacy/VEX/EVEX encodings, immediates and byte masks; baseline import is scalar.")
