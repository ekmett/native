#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Require a successful positive build and every SHA boundary diagnostic."""
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
parser.add_argument("--kind", choices=("feature", "target", "weak", "shape", "storage", "immediate"), required=True)
parser.add_argument("--log", required=True, type=Path)
args = parser.parse_args()

def build(target):
    command = [args.cmake, "--build", args.build, "--target", target, "--parallel"]
    if args.config:
        command += ["--config", args.config]
    # Native module fixtures use Ninja. Collect every deliberately failing
    # operation even when an earlier translation unit has already failed.
    command += ["--", "-k", "0"]
    result = subprocess.run(command, capture_output=True, text=True)
    return result.returncode, result.stdout + result.stderr

positive_status, positive_text = build(args.positive)
args.log.write_text("Positive control:\n" + positive_text, encoding="utf-8")
if positive_status:
    print(positive_text)
    raise SystemExit("Positive control did not compile.")
negative_status, negative_text = build(args.negative)
with args.log.open("a", encoding="utf-8") as log:
    log.write("\nNegative control:\n" + negative_text)
if negative_status == 0:
    raise SystemExit("Negative control unexpectedly compiled.")

if args.kind == "storage":
    if not re.search(r"error:.*(?:undefined template|constraints not satisfied|incomplete type)", negative_text):
        print(negative_text)
        raise SystemExit("Missing storage rejection.")
    print("Positive storage control compiled; unavailable storage rejected.")
    raise SystemExit(0)
if args.kind == "immediate":
    if not re.search(r"error:.*(?:deleted function|no matching function).*sha1rnds4", negative_text):
        print(negative_text)
        raise SystemExit("Missing immediate rejection.")
    print("Positive immediate control compiled; invalid selector rejected.")
    raise SystemExit(0)
count = 0
operations = ['sha1rnds4', 'sha1nexte', 'sha1msg1', 'sha1msg2', 'sha256rnds2', 'sha256msg1', 'sha256msg2']
for name in operations:
    if args.kind == "mask" and not name.startswith("mask", "storage", "immediate"):
        continue
    if args.kind == "feature":
        diagnostic = rf"error: call to deleted function '{name}'"
    elif args.kind == "target":
        diagnostic = rf"error: always_inline function '{name}'.*requires target feature"
    elif args.kind == "weak":
        diagnostic = rf"error: call to consteval function 'native::{name}<.*is not a constant expression"
    else:
        diagnostic = rf"error: call to deleted function '{name}'"
    expected_count = 4 if name == "sha1rnds4" else 1
    if len(re.findall(diagnostic, negative_text)) < expected_count:
        print(negative_text)
        raise SystemExit(f"Missing {args.kind} rejection for {name}.")
    count += 1
print(f"Positive control compiled; all {count} {args.kind} rejections confirmed.")
