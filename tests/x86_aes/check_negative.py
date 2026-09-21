#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Build matched AES positive and negative controls and retain diagnostics."""
import argparse
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--cmake", required=True)
parser.add_argument("--build", required=True)
parser.add_argument("--positive", required=True)
parser.add_argument("--negative", required=True)
parser.add_argument("--config", default="")
parser.add_argument("--log", required=True, type=Path)
parser.add_argument("--diagnostic", required=True)
args = parser.parse_args()

transcript = []
for target, must_succeed in ((args.positive, True), (args.negative, False)):
    command = [args.cmake, "--build", args.build, "--target", target, "--parallel"]
    if args.config:
        command.extend(["--config", args.config])
    result = subprocess.run(command, text=True, capture_output=True)
    diagnostic = result.stdout + result.stderr
    transcript.append(f"Target: {target}\nExit: {result.returncode}\n{diagnostic}")
    args.log.write_text("\n".join(transcript), encoding="utf-8")
    if must_succeed and result.returncode:
        raise SystemExit("AES positive control failed:\n" + diagnostic)
    if not must_succeed and (result.returncode == 0 or not re.search(args.diagnostic, diagnostic, re.I)):
        raise SystemExit("Expected AES rejection diagnostic was absent:\n" + diagnostic)
print("AES positive and rejection controls passed: " + args.negative)
