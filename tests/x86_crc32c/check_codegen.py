#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check exact CRC operand widths and the ordinary baseline importer."""
import argparse
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--objdump", required=True)
parser.add_argument("--output", required=True, type=Path)
parser.add_argument("objects", nargs="+")
args = parser.parse_args()
result = subprocess.run([args.objdump, "--syms", "--disassemble", *args.objects],
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
        # Mach-O can label the first function ltmp0; resolve its global symbol
        # by object, section and address instead of relying on the printed name.
        sections = re.split(r"(?m)^Disassembly of section ([^\n]+):\s*$", contents)
        by_address = {}
        for section in range(1, len(sections), 2):
            for address, name, body in functions(sections[section + 1]):
                by_address[sections[section], address] = body
        symbols = re.findall(r"(?m)^([0-9a-f]+)\s+[gw]\s+F\s+(\S+)\s+([^\n]+)$", sections[0])
        for address, section, name in symbols:
            if re.fullmatch(r"_?native_crc32c_(?:u(?:8|16|32|64)|baseline)", name):
                bodies[name.lstrip("_")] = by_address.get((section, int(address, 16)), "")
    else:
        for address, name, body in functions(contents):
            if re.fullmatch(r"_?native_crc32c_(?:u(?:8|16|32|64)|baseline)", name):
                bodies[name.lstrip("_")] = body

for width, suffix in ((8, "b"), (16, "w"), (32, "l"), (64, "q")):
    name = f"native_crc32c_u{width}"
    body = bodies.get(name, "")
    if len(re.findall(rf"\bcrc32{suffix}\b", body)) != 1:
        raise SystemExit(f"{name} must contain exactly one CRC32{suffix.upper()} instruction")
    if re.search(r"\b(?:callq?|jmpq?)\s", body):
        raise SystemExit(f"{name} contains an unexpected helper call or tail call")
    if re.search(r"\b(?:[xyz]mm\d+|popcnt[wlq]?|lzcnt[wlq]?|pdep[wlq]?|pext[wlq]?|bzhi[wlq]?|bextr[wlq]?)\b", body):
        raise SystemExit(f"{name} requires an unrelated instruction feature")
body = bodies.get("native_crc32c_baseline", "")
if not re.search(r"\bxorl?\b", body) or re.search(r"\b(?:crc32[bwlq]?|[xyz]mm\d+)\b", body):
    raise SystemExit("Ordinary baseline importer changed its scalar instruction requirements")
print("CRC32C fixtures contain one CRC32B/W/L/Q each, no helpers, and a scalar baseline importer.")
