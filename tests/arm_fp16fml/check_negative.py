# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Build a rejected consumer and retain its actual compiler diagnostic."""
import argparse
import pathlib
import re
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument("--cmake", required=True)
parser.add_argument("--build", required=True)
parser.add_argument("--target", required=True)
parser.add_argument("--config", default="")
parser.add_argument("--log", required=True, type=pathlib.Path)
parser.add_argument("--diagnostic", required=True)
args = parser.parse_args()
command = [args.cmake, "--build", args.build, "--target", args.target, "--parallel"]
if args.config:
    command += ["--config", args.config]
result = subprocess.run(command, text=True, capture_output=True)
output = result.stdout + result.stderr
args.log.write_text(output, encoding="utf-8")
if result.returncode == 0 or not re.search(args.diagnostic, output, re.I):
    print(output)
    raise SystemExit("Expected compiler rejection was absent")
print("Expected compiler rejection confirmed")
