#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Build a F16C negative control and retain its compiler diagnostic."""
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
diagnostic = result.stdout + result.stderr
args.log.write_text(diagnostic, encoding="utf-8")
if result.returncode == 0 or not re.search(args.diagnostic, diagnostic, re.I):
    print(diagnostic)
    raise SystemExit("Expected F16C compiler diagnostic was absent.")
print("F16C compiler boundary rejected as expected: " + args.target)
