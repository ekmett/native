#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Inspect the ordinary rounding.cc objects from the focused installed check."""
import argparse
import json
from pathlib import Path
import re
import subprocess

p = argparse.ArgumentParser()
p.add_argument("--objdump", required=True)
p.add_argument("--build", type=Path, required=True)
p.add_argument("--output", type=Path, required=True)
a = p.parse_args()
a.output.mkdir(parents=True, exist_ok=True)
report = []
for profile, widths in (("avx2", (1, 3, 4, 8)), ("avx512", (1, 3, 4, 8, 16))):
    for kind in ("header", "import"):
        obj = a.build / "CMakeFiles" / f"simd_rounding_{profile}_{kind}.dir" / "rounding.cc.obj"
        symbols = [f"round_{op}_{n}" for op in ("floor", "ceil", "trunc") for n in widths]
        command = [a.objdump, "-d", "--no-show-raw-insn", "--disassemble-symbols=" + ",".join(symbols), str(obj)]
        result = subprocess.run(command, capture_output=True, text=True, check=True)
        (a.output / f"{profile}-{kind}.asm").write_text(result.stdout)
        for op, mode in (("floor", "9"), ("ceil", "a"), ("trunc", "b")):
            for n in widths:
                symbol = f"round_{op}_{n}"
                match = re.search(r"<" + symbol + r">:\n(.*?)(?=\nDisassembly|\Z)", result.stdout, re.S)
                if not match:
                    raise SystemExit(f"missing {symbol} in {obj}")
                body = match.group(1)
                if re.search(r"\b(call\w*|j\w+)\s", body):
                    raise SystemExit(f"unexpected call/branch in {symbol}: {body}")
                instruction = "vrndscaleps" if n == 16 else "vroundss" if n == 1 else "vroundps"
                if len(re.findall(r"\b" + instruction + r"\s+\$0x" + mode + r",", body)) != 1:
                    raise SystemExit(f"wrong fixed rounding instruction in {symbol}: {body}")
                report.append({"profile": profile, "interface": kind, "symbol": symbol, "instruction": instruction,
                               "immediate": int(mode, 16), "calls": 0, "branches": 0})
(a.output / "report.json").write_text(json.dumps(report, indent=2) + "\n")
print(f"{len(report)} ordinary native leaves: one fixed rounding instruction, no calls or branches")
