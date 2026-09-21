#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check AVX512CD opcodes, widths, native masks and baseline importers."""
import argparse
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--objdump", required=True)
parser.add_argument("--output", required=True, type=Path)
parser.add_argument("--baseline-only", action="store_true")
parser.add_argument("--broad-target", action="store_true",
                    help="caller explicitly enables AVX512DQ/BW as well as CD")
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

baseline = "native_avx512cd_baseline_import" if args.baseline_only else "native_avx512cd_baseline"
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

# Minimal callers allow only AVX512F mask moves. Broader callers can choose
# their own scalar mask width; the CD opcode and destination mask mode stay fixed.
# No other instruction family or helper call may implement a fallback.
mask_move = r"kmov[bwdq]" if args.broad_target else r"kmovw"
allowed = re.compile(
    rf"(?:{mask_move}|vp(?:conflict|lzcnt)[dq]|vzeroupper|vmov(?:dqa(?:32|64)?|dqu(?:32|64)?|aps|ups)|"
    r"mov(?:[bwlq]|absq)?|movz[bw][wlq]|lea[wlq]?|ret[ql]?|nop[wlq]?|"
    r"pushq?|popq?|and[wlq]?|sub[wlq]?|add[wlq]?|int3|data16)"
)
for width, register in ((128, "xmm"), (256, "ymm"), (512, "zmm")):
    for operation in ("vpconflict", "vplzcnt"):
        for lane in ("d", "q"):
            for variant in ("plain", "merge", "zero"):
                opcode = operation + lane
                name = f"native_{opcode}_{width}_{variant}"
                decoded = instructions(name)
                counts = [(mnemonic, operands) for mnemonic, operands in decoded
                          if mnemonic.startswith(("vpconflict", "vplzcnt"))]
                if len(counts) != 1 or counts[0][0] != opcode:
                    raise SystemExit(f"{name} must contain exactly one {opcode.upper()}.")
                operands = counts[0][1]
                if not re.search(rf"\b{register}\d+\b", operands):
                    raise SystemExit(f"{name} has the wrong register width.")
                if any(reg != register for reg in re.findall(r"\b([xyz]mm)\d+\b", operands)):
                    raise SystemExit(f"{name} mixes unexpected register widths.")
                masked = bool(re.search(r"\{\s*%?k[1-7]\s*\}", operands))
                zeroed = bool(re.search(r"\{\s*z\s*\}", operands))
                if masked != (variant != "plain") or zeroed != (variant == "zero"):
                    raise SystemExit(f"{name} does not use the expected hardware mask mode.")
                for mnemonic, operands in decoded:
                    if not allowed.fullmatch(mnemonic):
                        raise SystemExit(f"{name} contains an unexpected instruction: {mnemonic}.")
print("All 36 forms use the expected AVX512CD opcode, width and mask mode without helper calls.")
print("The ordinary function in the baseline translation unit has no optional instructions.")
