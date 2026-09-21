#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Require successful VNNI positive controls and every selected rejection."""
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
parser.add_argument("--kind", choices=("feature", "target", "shape"), required=True)
parser.add_argument("--operation", action="append", required=True)
parser.add_argument("--log", required=True, type=Path)
args = parser.parse_args()


def build(target):
    command = [args.cmake, "--build", args.build, "--target", target, "--parallel"]
    if args.config:
        command += ["--config", args.config]
    # Collect all negative translation units with the Ninja fixture generator.
    command += ["--", "-k", "0"]
    result = subprocess.run(command, capture_output=True, text=True)
    transcript = "Command: " + repr(command) + "\nExit status: " + str(result.returncode) + "\n"
    return result.returncode, transcript + result.stdout + result.stderr


positive_status, positive_text = build(args.positive)
args.log.write_text("Positive control:\n" + positive_text, encoding="utf-8")
if positive_status:
    print(positive_text)
    raise SystemExit("VNNI positive controls did not compile.")
negative_status, negative_text = build(args.negative)
with args.log.open("a", encoding="utf-8") as log:
    log.write("\nNegative controls:\n" + negative_text)
if negative_status == 0:
    raise SystemExit("VNNI negative controls unexpectedly compiled.")

for operation in args.operation:
    if args.kind == "target":
        diagnostic = rf"error: always_inline function '{re.escape(operation)}'.*requires target feature"
    else:
        diagnostic = rf"error: (?:no matching function for call to|call to deleted function) '{re.escape(operation)}'"
    if args.kind == "feature":
        # Public weak tags now select an immediate-only overload. A runtime
        # call must still fail, with the operation named in its diagnostic.
        diagnostic += rf"|error: call to consteval function 'native::{re.escape(operation)}<[^\n]*not a constant expression"
    if not re.search(diagnostic, negative_text):
        print(negative_text)
        raise SystemExit(f"Missing {args.kind} rejection for {operation}.")
print(f"Positive controls compiled; all {len(args.operation)} {args.kind} rejections confirmed.")
