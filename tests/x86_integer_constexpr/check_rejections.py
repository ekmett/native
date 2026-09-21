#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Require a constant-expression failure at every marked runtime call."""
import argparse
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--cmake", required=True)
parser.add_argument("--build", required=True)
parser.add_argument("--config", default="")
parser.add_argument("--source", type=Path, required=True)
parser.add_argument("--log", type=Path, required=True)
args = parser.parse_args()
command = [args.cmake, "--build", args.build, "--target", "native_test_x86_integer_constexpr_reject_runtime"]
if args.config:
    command += ["--config", args.config]
result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
args.log.write_text(result.stdout, encoding="utf-8")
expected = {i for i, line in enumerate(args.source.read_text().splitlines(), 1)
            if "// expected-consteval-rejection" in line}
# Accept GNU and clang-cl diagnostic locations. Do not count secondary notes
# or unrelated failures elsewhere in the module graph as negative coverage.
location = re.escape(args.source.name) + r"(?::|\()(?P<line>\d+)(?::\d+:|,\d+\):)"
observed = {int(match.group("line")) for match in re.finditer(
    location + r" (?:fatal )?error: call to consteval function[^\n]*not a constant expression", result.stdout)}
immediates = {i for i, line in enumerate(args.source.read_text().splitlines(), 1)
              if "// expected-immediate-rejection" in line}
rejected_immediates = {int(match.group("line")) for match in re.finditer(
    location + r" error: call to deleted function", result.stdout)}
missing = sorted((expected - observed) | (immediates - rejected_immediates))
if result.returncode == 0 or not expected or missing:
    raise SystemExit(f"Weak runtime rejection failed: exit={result.returncode}, "
                     f"missing source lines={missing}; see {args.log}")
print(f"All {len(expected)} weak vector overloads reject runtime operands; "
      f"{len(immediates)} immediate-range controls rejected; {args.log}")
