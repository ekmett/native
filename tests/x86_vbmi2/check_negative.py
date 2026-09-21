#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Require a successful positive build and every AVX512VBMI2 boundary diagnostic."""
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
parser.add_argument("--kind",
                    choices=("feature", "target", "weak", "shape", "mask", "pointer", "immediate"), required=True)
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

count = 0
operations = [
    ('mask_vpcompressb', 'merge', False),
    ('maskz_vpcompressb', 'zero', False),
    ('mask_vpcompressb', 'store', False),
    ('mask_vpexpandb', 'merge', False),
    ('maskz_vpexpandb', 'zero', False),
    ('mask_vpexpandb', 'load_merge', False),
    ('maskz_vpexpandb', 'load_zero', False),
    ('mask_vpcompressw', 'merge', False),
    ('maskz_vpcompressw', 'zero', False),
    ('mask_vpcompressw', 'store', False),
    ('mask_vpexpandw', 'merge', False),
    ('maskz_vpexpandw', 'zero', False),
    ('mask_vpexpandw', 'load_merge', False),
    ('maskz_vpexpandw', 'load_zero', False),
    ('vpshldw', 'plain', True),
    ('mask_vpshldw', 'merge', True),
    ('maskz_vpshldw', 'zero', True),
    ('vpshldvw', 'plain', False),
    ('mask_vpshldvw', 'merge', False),
    ('maskz_vpshldvw', 'zero', False),
    ('vpshrdw', 'plain', True),
    ('mask_vpshrdw', 'merge', True),
    ('maskz_vpshrdw', 'zero', True),
    ('vpshrdvw', 'plain', False),
    ('mask_vpshrdvw', 'merge', False),
    ('maskz_vpshrdvw', 'zero', False),
    ('vpshldd', 'plain', True),
    ('mask_vpshldd', 'merge', True),
    ('maskz_vpshldd', 'zero', True),
    ('vpshldvd', 'plain', False),
    ('mask_vpshldvd', 'merge', False),
    ('maskz_vpshldvd', 'zero', False),
    ('vpshrdd', 'plain', True),
    ('mask_vpshrdd', 'merge', True),
    ('maskz_vpshrdd', 'zero', True),
    ('vpshrdvd', 'plain', False),
    ('mask_vpshrdvd', 'merge', False),
    ('maskz_vpshrdvd', 'zero', False),
    ('vpshldq', 'plain', True),
    ('mask_vpshldq', 'merge', True),
    ('maskz_vpshldq', 'zero', True),
    ('vpshldvq', 'plain', False),
    ('mask_vpshldvq', 'merge', False),
    ('maskz_vpshldvq', 'zero', False),
    ('vpshrdq', 'plain', True),
    ('mask_vpshrdq', 'merge', True),
    ('maskz_vpshrdq', 'zero', True),
    ('vpshrdvq', 'plain', False),
    ('mask_vpshrdvq', 'merge', False),
    ('maskz_vpshrdvq', 'zero', False),
]
for name, form, immediate in operations:
    if args.kind == "mask" and not name.startswith("mask"):
        continue
    if args.kind == "pointer" and form not in ("store", "load_merge", "load_zero"):
        continue
    if args.kind == "pointer" and "_4" in args.negative and form != "store":
        continue
    if args.kind == "immediate" and not immediate:
        continue
    if args.kind == "shape" and form == "load_zero":
        continue
    if args.kind == "feature":
        suffix = r"_(?:128|256|512)" if form == "load_zero" else ""
        diagnostic = rf"error: no matching function for call to '{name}{suffix}'"
    elif args.kind == "target":
        diagnostic = rf"error: always_inline function '{name}'.*requires target feature"
    elif args.kind == "weak":
        diagnostic = rf"error: call to consteval function 'native::{name}<.*is not a constant expression"
    else:
        diagnostic = rf"error: call to deleted function '{name}'"
    if not re.search(diagnostic, negative_text):
        print(negative_text)
        raise SystemExit(f"Missing {args.kind} rejection for {name}.")
    negative_text = re.sub(diagnostic, "[verified diagnostic]", negative_text, count=1)
    count += 1
print(f"Positive control compiled; all {count} {args.kind} rejections confirmed.")
