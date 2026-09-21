#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check every integer VNNI encoding, width and mask, and baseline importers."""
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
command = [args.objdump, "--syms", "--disassemble", "--x86-asm-syntax=intel", *args.objects]
result = subprocess.run(command, capture_output=True, text=True)
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
        # Mach-O may print a local section label for the first function.
        sections = re.split(r"(?m)^Disassembly of section ([^\n]+):\s*$", contents)
        by_address = {}
        for section in range(1, len(sections), 2):
            for address, name, body in functions(sections[section + 1]):
                by_address[sections[section], address] = body
        symbols = re.findall(r"(?m)^([0-9a-f]+)\s+[gw]\s+F\s+(\S+)\s+([^\n]+)$", sections[0])
        for address, section, name in symbols:
            name = name.lstrip("_")
            if name.startswith("native_vnni_") or name == "main":
                bodies[name] = by_address.get((section, int(address, 16)), "")
    else:
        for address, name, body in functions(contents):
            bodies[name.lstrip("_")] = body


def instructions(name):
    body = bodies.get(name, "")
    if not body:
        raise SystemExit(f"Missing disassembly for {name}.")
    rows = re.findall(
        r"(?m)^\s*[0-9a-f]+:\s+((?:[0-9a-f]{2}\s+)+)"
        r"(?:\{(?:vex|evex)\}\s+)?([a-z][a-z0-9]*)\s*([^\n]*)", body)
    if not rows:
        raise SystemExit(f"No decoded instructions for {name}.")
    return [(encoding.split(), mnemonic, operands) for encoding, mnemonic, operands in rows]


baseline_names = ("native_vnni_baseline_import", "main") if args.baseline_only else ("native_vnni_baseline",)
for name in baseline_names:
    for _, mnemonic, operands in instructions(name):
        if mnemonic.startswith(("v", "k")) or re.search(r"\b(?:[yz]mm\d+|k[0-7])\b", operands):
            raise SystemExit(f"{name} contains an optional vector instruction: {mnemonic}.")
        if re.fullmatch(r"(?:popcnt|lzcnt|tzcnt|pdep|pext|bzhi|bextr)[wlq]?", mnemonic):
            raise SystemExit(f"{name} contains an optional scalar instruction: {mnemonic}.")
if args.baseline_only:
    print("The importer and main retain their baseline instruction set.")
    raise SystemExit(0)

families = {
    "vex": ("dpbusd", "dpbusds", "dpwssd", "dpwssds"),
    "priority": ("dpbusd", "dpbusds", "dpwssd", "dpwssds"),
    "evex": ("dpbusd", "dpbusds", "dpwssd", "dpwssds"),
    "int8": ("dpbssd", "dpbssds", "dpbsud", "dpbsuds", "dpbuud", "dpbuuds"),
    "int16": ("dpwsud", "dpwsuds", "dpwusd", "dpwusds", "dpwuud", "dpwuuds"),
}
# These wrappers need one dot product and register/ABI moves, never a helper or
# arithmetic emulation. Allow alignment padding and the Windows stack prologue.
allowed = re.compile(
    r"(?:vpdp[bw][su][su]ds?|kmov[bwdq]|vzeroupper|"
    r"v?mov(?:dqa(?:32|64)?|dqu(?:32|64)?|aps|ups|apd|upd)|"
    r"mov(?:[bwlq]|absq|zx|sx|sxd)?|lea[wlq]?|ret[ql]?|nop[wlq]?|"
    r"pushq?|popq?|and[wlq]?|sub[wlq]?|add[wlq]?|int3|data16)"
)
count = 0
for family, operations in families.items():
    widths = (128, 256, 512) if family == "evex" else (128, 256)
    variants = ("plain", "merge", "zero") if family == "evex" else ("plain",)
    for operation in operations:
        for width in widths:
            register = {128: "xmm", 256: "ymm", 512: "zmm"}[width]
            for variant in variants:
                name = f"native_vnni_{family}_{operation}_{width}_{variant}"
                rows = instructions(name)
                dot_products = [row for row in rows if row[1].startswith("vpdp")]
                if len(dot_products) != 1 or dot_products[0][1] != "vp" + operation:
                    raise SystemExit(f"{name} must contain exactly one VP{operation.upper()}.")
                encoding, _, operands = dot_products[0]
                expected_prefix = "62" if family == "evex" else "c4"
                if encoding[0] != expected_prefix:
                    raise SystemExit(f"{name} has the wrong VEX/EVEX encoding.")
                registers = re.findall(r"\b([xyz]mm)\d+\b", operands)
                if not registers or any(reg != register for reg in registers):
                    raise SystemExit(f"{name} has the wrong register width.")
                masked = bool(re.search(r"\{\s*%?k[1-7]\s*\}", operands))
                zeroed = bool(re.search(r"\{\s*z\s*\}", operands))
                if masked != (variant != "plain") or zeroed != (variant == "zero"):
                    raise SystemExit(f"{name} has the wrong merge/zero mask mode.")
                for insn_bytes, mnemonic, operands in rows:
                    if not allowed.fullmatch(mnemonic):
                        raise SystemExit(f"{name} contains an unexpected instruction: {mnemonic}.")
                    if family != "evex" and (insn_bytes[0] == "62" or
                            re.search(r"\b(?:zmm\d+|k[0-7])\b", operands)):
                        raise SystemExit(f"{name} unexpectedly requires AVX-512.")
                count += 1
print(f"All {count} forms use the expected VNNI encoding, width and mask without helper calls.")
print("The ordinary function in the textual probe retains its baseline instruction set.")
