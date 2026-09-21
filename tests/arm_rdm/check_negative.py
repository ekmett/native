#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Require failure when an optional instruction is called outside its target."""
import argparse
import json
from pathlib import Path
import re
import shlex
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
for name in ("database", "source", "negative", "output"):
    parser.add_argument("--" + name, required=True, type=Path)
parser.add_argument("--lane", action="store_true")
args = parser.parse_args()
entry = next(item for item in json.loads(args.database.read_text())
             if Path(item["file"]).resolve() == args.source.resolve())
command = entry.get("arguments") or shlex.split(entry["command"])
clang_cl = "clang-cl" in Path(command[0]).name.lower()
result_command = []
skip = False
for word in command:
    if skip:
        skip = False
        continue
    if word in ("-o", "-MF", "-MT", "-MQ"):
        skip = True
    elif clang_cl and word.startswith(("/Fo", "/Fd")):
        continue
    elif word not in ("-c", "/c", "-MD", "-MMD") and Path(word) != args.source:
        result_command.append(word)
if clang_cl:
    result_command += ["/c", str(args.negative), "/Fo" + str(args.output.with_suffix(".obj"))]
else:
    result_command += ["-c", str(args.negative), "-o", str(args.output.with_suffix(".o"))]
result = subprocess.run(result_command, cwd=entry["directory"],
                        capture_output=True, text=True)
args.output.write_text(result.stdout + result.stderr)
if result.returncode == 0:
    raise SystemExit("Feature-disabled caller unexpectedly compiled")
expected = r"deleted function|no matching function" if args.lane else r"requires target feature.*rdm|target specific option mismatch"
if not re.search(expected, result.stderr):
    raise SystemExit("Failure did not diagnose the expected RDM rejection:\n" + result.stderr)
print("Invalid RDM lane rejected." if args.lane else "Missing rdm caller target rejected.")
