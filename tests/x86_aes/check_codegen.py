#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check legacy AES opcodes, immediate bytes and baseline admission code."""
import argparse
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--objdump", required=True)
parser.add_argument("--output", required=True, type=Path)
parser.add_argument("--baseline", required=True)
parser.add_argument("--raw")
parser.add_argument("--public")
args = parser.parse_args()
transcript = []


def disassemble(path):
    result = subprocess.run(
        [args.objdump, "--syms", "--disassemble", "--x86-asm-syntax=intel", path],
        capture_output=True, text=True, check=True)
    transcript.append(result.stdout)
    args.output.write_text("\n".join(transcript), encoding="utf-8")
    bodies = {}
    addresses = {}
    sections = re.split(r"(?m)^Disassembly of section ([^\n]+):\s*$", result.stdout)
    for section in range(1, len(sections), 2):
        functions = re.split(r"(?m)^([0-9a-fA-F]+) <([^\n]+)>:\s*$", sections[section + 1])
        for index in range(1, len(functions), 3):
            address, name, body = functions[index:index + 3]
            bodies[name.lstrip("_")] = body
            addresses[sections[section], int(address, 16)] = body
    # Mach-O may label a function with its local section symbol instead.
    for address, section, name in re.findall(
            r"(?m)^([0-9a-fA-F]+)\s+[gw]\s+F\s+(\S+)\s+([^\n]+)$", sections[0]):
        body = addresses.get((section, int(address, 16)))
        if body is not None:
            bodies[name.lstrip("_")] = body
    return bodies


def instructions(body):
    return re.findall(
        r"(?m)^\s*[0-9a-f]+:\s+((?:[0-9a-f]{2}\s+)+)([a-z][a-z0-9]*)\s*([^\n]*)", body)


baseline = disassemble(args.baseline)
for name in ("native_aes_baseline", "native_aes_admission"):
    if not instructions(baseline.get(name, "")):
        raise SystemExit("Missing baseline function: " + name)
# Inspect the whole object, including any emitted classifier helper bodies.
for body in baseline.values():
    for _, mnemonic, operands in instructions(body):
        if mnemonic.startswith(("aes", "v", "k")) or re.search(r"\b(?:[yz]mm\d+|k[0-7])\b", operands):
            raise SystemExit("Optional vector instruction in baseline admission object: " + mnemonic)

expected = {"native_aes_" + op: (op, None) for op in
            ("aesenc", "aesenclast", "aesdec", "aesdeclast", "aesimc")}
expected.update({f"native_aes_aeskeygenassist_{immediate}": ("aeskeygenassist", immediate)
                 for immediate in (0, 1, 255)})
for path in (args.raw, args.public):
    if path is None:
        continue
    bodies = disassemble(path)
    if {name for name in bodies if name.startswith("native_aes_")} != expected.keys():
        raise SystemExit("Unexpected or missing AES codegen probes: " + path)
    for name, (opcode, immediate) in expected.items():
        rows = instructions(bodies[name])
        matches = [row for row in rows if "aes" in row[1]]
        if len(matches) != 1 or matches[0][1] != opcode:
            raise SystemExit(f"{name}: expected exactly one {opcode}")
        encoding, _, operands = matches[0]
        encoded = encoding.split()
        if encoded[0] != "66" or not re.search(r"\bxmm\d+\b", operands):
            raise SystemExit(f"{name}: expected legacy 128-bit AES encoding")
        if immediate is not None and int(encoded[-1], 16) != immediate:
            raise SystemExit(f"{name}: incorrect round constant")
        if any(mnemonic.startswith(("call", "j", "v", "k")) for _, mnemonic, _ in rows):
            raise SystemExit(f"{name}: unexpected call, branch or optional vector instruction")
print("AES instruction probes and baseline admission retain their required ISA boundaries.")
