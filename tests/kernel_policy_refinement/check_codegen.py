# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Use the existing section-aware parser; assert this fixture's own opcode cuts."""
import argparse
import importlib.util
from pathlib import Path
import re
import subprocess
import sys
sys.dont_write_bytecode = True

parser_path = Path(__file__).resolve().parents[1] / "exp_policy_refinement/check_codegen.py"
spec = importlib.util.spec_from_file_location("section_parser", parser_path)
parser_module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(parser_module)
# Retain the leading encoding byte in this fixture's parser view. EVEX can use
# only YMM registers, so register-name checks alone cannot prove an AVX2 bound.
original_instructions = parser_module.instructions

def encoded_instructions(text):
    code, relocations = original_instructions(text)
    evex = set()
    for line in text.splitlines():
        match = parser_module.INSTRUCTION.match(line)
        if match and match[2].split()[0] == "62":
            evex.add(int(match[1], 16))
    return [(address, size, ("EVEX:" if address in evex else "") + opcode, operands)
            for address, size, opcode, operands in code], relocations

parser_module.instructions = encoded_instructions

EXPECTED = {
    "refined_codegen_kernel_avx2": "ymm",
    "refined_codegen_kernel_kernel_base": "zmm",
    "refined_codegen_kernel_kernel_vl": "ymm",
}


def check(text):
    bodies = parser_module.function_bodies(text)
    names = [name for name, *_ in bodies]
    if len(names) != len(set(names)) or set(names) != set(EXPECTED):
        raise ValueError(f"Incorrect exact kernel entry set: {names}")
    for name, body, relocations, _ in bodies:
        if name.endswith("_avx2") and any(row[2].startswith("EVEX:") for row in body):
            raise ValueError(f"EVEX instruction leaked into AVX2 entry {name}")
        body = [(a, n, op.removeprefix("EVEX:"), args) for a, n, op, args in body]
        width = EXPECTED[name]
        fmas = [row for row in body if re.fullmatch(r"vfmadd\d*ps", row[2])]
        roots = [row for row in body if row[2] == "vsqrtps"]
        if not fmas or not roots:
            raise ValueError(f"Missing packed FMA or sqrt in {name}")
        for _, _, opcode, operands in fmas + roots:
            if set(re.findall(r"\b(xmm|ymm|zmm)\d+\b", operands)) != {width}:
                raise ValueError(f"Wrong width in {name}: {opcode} {operands}")
        addresses = {row[0] for row in body}
        for address, size, opcode, operands in body:
            if opcode.startswith("call"):
                raise ValueError(f"Outlined call in {name}")
            if opcode.startswith(("j", "loop")):
                if any(address <= offset < address + size for offset, *_ in relocations):
                    raise ValueError(f"Relocated branch in {name}")
                match = re.fullmatch(r"(?:0x)?([0-9a-f]+)(?:\s+<([^>]+)>)?", operands)
                if not match or int(match[1], 16) not in addresses:
                    raise ValueError(f"External/indirect branch in {name}")
                if match[2] and match[2].split("+0x", 1)[0].lstrip("_") != name:
                    if not match[2].startswith((".L", "LBB", "ltmp")):
                        raise ValueError(f"Cross-symbol branch in {name}")
            if name.endswith("_avx2") and re.search(r"\b(?:zmm\d+|k[0-7])\b", operands):
                raise ValueError(f"AVX512 register leaked into {name}")
            if "bf16" in opcode or re.search(r"(?:ph|sh)(?:2|$)|2(?:ph|sh)", opcode):
                raise ValueError(f"Half instruction in FP32 kernel {name}")
    return "3 exact non-exp entries: FMA/sqrt widths, no helper calls/tails, AVX2 register/encoding bound"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--objdump", required=True)
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("objects", nargs="+")
    args = parser.parse_args()
    result = subprocess.run([args.objdump, "--syms", "--disassemble", "--reloc",
                             *args.objects], text=True, capture_output=True)
    args.output.write_text(result.stdout + result.stderr, encoding="utf-8")
    if result.returncode:
        raise SystemExit(result.stderr)
    print(check(result.stdout))


if __name__ == "__main__":
    main()
