#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Require a real compiler diagnostic from a deliberately invalid consumer."""
import argparse
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--cmake", required=True)
parser.add_argument("--build", required=True)
parser.add_argument("--target", required=True)
parser.add_argument("--config", default="")
parser.add_argument("--log", required=True, type=Path)
parser.add_argument("--diagnostic", required=True)
args = parser.parse_args()
command = [args.cmake, "--build", args.build, "--target", args.target, "--parallel"]
if args.config:
    command.extend(["--config", args.config])
result = subprocess.run(command, text=True, capture_output=True)
output = result.stdout + result.stderr
args.log.write_text(output, encoding="utf-8")
if "NATIVE_TARGET_NEGATIVE_UNAVAILABLE" in output:
    print("Configured minimum already includes CRC32; target negative is inapplicable.")
    raise SystemExit(77)
if result.returncode == 0 or not re.search(args.diagnostic, output, re.I):
    print(output)
    raise SystemExit("Expected CRC32C diagnostic was absent.")
print("Expected CRC32C compiler diagnostic confirmed.")
