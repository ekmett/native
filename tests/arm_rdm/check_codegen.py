#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check every RDM wrapper at a baseline AArch64 source minimum."""
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

names = (Path(__file__).with_name("codegen_names.txt")).read_text().splitlines()
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
            if name.lstrip("_") in names:
                bodies[name.lstrip("_")] = by_address.get((section, int(address, 16)), "")
    else:
        for address, name, body in functions(contents):
            if name.lstrip("_") in names:
                bodies[name.lstrip("_")] = body

for name in names:
    body = bodies.get(name, "")
    instruction = name.split("_")[1]
    matches = re.findall(r"\b" + instruction + r"\b[^\n]*", body)
    if len(matches) != 1:
        raise SystemExit(f"{name} must have exactly one {instruction.upper()} instruction")
    if "_lane" in name:
        lane = int(name.rsplit("_lane", 1)[1]) - 1
        if not re.search(r"\[" + str(lane) + r"\]", matches[0]):
            raise SystemExit(f"{name} did not use its native by-element lane {lane}")
    if re.search(r"\b(?:bl|blr)\s", body):
        raise SystemExit(f"{name} contains an unexpected helper call")
    if re.search(r"\b(?:sdot|udot|fmlal|fmlal2|fcmla|bfmmla|bfdot)\b", body):
        raise SystemExit(f"{name} requires an unrelated instruction family")
print(f"{len(names)} RDM fixtures contain their native instructions without helper calls.")
