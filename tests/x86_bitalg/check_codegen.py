#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check AVX512BITALG opcodes, widths, native masks and baseline importers."""
import argparse
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--objdump", required=True)
parser.add_argument("--output", required=True, type=Path)
parser.add_argument("--baseline-only", action="store_true")
parser.add_argument("--broad-target", action="store_true",
                    help="caller explicitly enables AVX512DQ as well as BITALG")
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

baseline = "native_bitalg_baseline_import" if args.baseline_only else "native_bitalg_baseline"
baseline_ops = instructions(baseline)
for mnemonic, operands in baseline_ops:
    if mnemonic.startswith(("v", "k")) or re.search(r"\b[xyz]mm\d+\b", operands):
        raise SystemExit(f"Baseline function contains an optional vector instruction: {mnemonic}.")
    if re.fullmatch(r"(?:popcnt|lzcnt|tzcnt|pdep|pext|bzhi|bextr)[wlq]?", mnemonic):
        raise SystemExit(f"Baseline function contains an optional scalar instruction: {mnemonic}.")
if args.baseline_only:
    for mnemonic, operands in instructions("main"):
        if mnemonic.startswith(("v", "k")) or re.search(r"\b(?:[yz]mm\d+|k[0-7])\b", operands):
            raise SystemExit(f"Baseline main contains an optional vector instruction: {mnemonic}.")
        if re.fullmatch(r"(?:popcnt|lzcnt|tzcnt|pdep|pext|bzhi|bextr)[wlq]?", mnemonic):
            raise SystemExit(f"Baseline main contains an optional scalar instruction: {mnemonic}.")
    print("Baseline importer probe and main have no optional vector or scalar instructions.")
    raise SystemExit(0)

# BW admits all mask move widths. No helper call or other instruction family
# may replace the requested BITALG opcode, even when the caller enables DQ.
allowed = re.compile(
    r"(?:kmov[bwdq]|kand[bwdq]|vpopcnt[bw]|vpshufbitqmb|vzeroupper|"
    r"vmov(?:dqa(?:32|64)?|dqu(?:8|16|32|64)?|aps|ups)|"
    r"mov(?:[bwlq]|absq)?|movz[bw][wlq]|lea[wlq]?|ret[ql]?|nop[wlq]?|"
    r"pushq?|popq?|and[wlq]?|sub[wlq]?|add[wlq]?|int3|data16)"
)
for width, register in ((128, "xmm"), (256, "ymm"), (512, "zmm")):
    for opcode in ("vpopcntb", "vpopcntw", "vpshufbitqmb"):
        variants = ("plain", "zero") if opcode == "vpshufbitqmb" else ("plain", "merge", "zero")
        for variant in variants:
            name = f"native_{opcode}_{width}_{variant}"
            decoded = instructions(name)
            counts = [(mnemonic, operands) for mnemonic, operands in decoded
                      if mnemonic.startswith(("vpopcnt", "vpshufbit"))]
            if len(counts) != 1 or counts[0][0] != opcode:
                raise SystemExit(f"{name} must contain exactly one {opcode.upper()}.")
            operands = counts[0][1]
            if not re.search(rf"\b{register}\d+\b", operands):
                raise SystemExit(f"{name} has the wrong register width.")
            if any(reg != register for reg in re.findall(r"\b([xyz]mm)\d+\b", operands)):
                raise SystemExit(f"{name} mixes unexpected register widths.")
            masked = bool(re.search(r"\{\s*%?k[1-7]\s*\}", operands))
            zeroed = bool(re.search(r"\{\s*z\s*\}", operands))
            if opcode == "vpshufbitqmb":
                # When returning a scalar mask, Clang can move masking after
                # VPSHUFBITQMB. The raw/public pair must still match exactly.
                result_masks = [mnemonic for mnemonic, operands in decoded
                                if re.fullmatch(r"kand[bwdq]", mnemonic) or
                                (re.fullmatch(r"and[wlq]", mnemonic) and
                                 re.search(r",\s*%?(?:ax|eax|rax)\s*$", operands))]
                if zeroed or int(masked) + len(result_masks) != (variant == "zero"):
                    raise SystemExit(f"{name} must apply its output mask exactly once.")
            elif masked != (variant != "plain") or zeroed != (variant == "zero"):
                raise SystemExit(f"{name} does not use the expected hardware mask mode.")
            for mnemonic, operands in decoded:
                if not allowed.fullmatch(mnemonic):
                    raise SystemExit(f"{name} contains an unexpected instruction: {mnemonic}.")
print("All 24 forms use the expected BITALG opcode, width and output masking without helper calls.")
print("The ordinary function in the baseline translation unit has no optional instructions.")
