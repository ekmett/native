#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Export LLVM coverage faithfully; use grcov for browsable line coverage."""

import argparse
import json
from pathlib import Path
import subprocess
import sys


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ("source", "coverage", "binaries", "llvm", "grcov"):
        parser.add_argument(f"--{name}", required=True, type=Path)
    args = parser.parse_args()
    report = args.coverage / "report"
    report.mkdir(parents=True, exist_ok=True)
    profiles = sorted((args.coverage / "raw").glob("*.profraw"))
    binaries = [Path(p) for p in args.binaries.read_text().splitlines() if Path(p).is_file()]
    if not profiles or not binaries:
        raise RuntimeError("Coverage requires built test executables and runtime profiles")

    # Response files avoid Windows command-length limits with the extended suite.
    inputs = args.coverage / "profiles.txt"
    inputs.write_text("".join(f"{p.as_posix()}\n" for p in profiles))
    merged = args.coverage / "merged.profdata"
    subprocess.run([str(args.llvm / "llvm-profdata"), "merge", "-sparse",
                    "-f", str(inputs), "-o", str(merged)], check=True)
    objects = args.coverage / "objects.rsp"
    objects.write_text("\n".join(
        f"-object {json.dumps(p.as_posix())}" for p in binaries))
    export = [str(args.llvm / "llvm-cov"), "export", f"@{objects}",
              f"-instr-profile={merged}"]
    result = subprocess.run([*export, "-format=lcov"], text=True, capture_output=True)
    (report / "llvm-cov.log").write_text(result.stderr)
    sys.stderr.write(result.stderr)
    result.check_returncode()
    raw = result.stdout
    source = args.source.resolve()
    records = []
    for record in raw.split("end_of_record\n"):
        lines = record.splitlines()
        filename = next((line[3:] for line in lines if line.startswith("SF:")), None)
        if filename is None:
            continue
        try:
            relative = Path(filename).resolve().relative_to(source)
        except ValueError:
            continue
        if relative.parts[0] != "src":
            continue
        lines = [f"SF:{relative.as_posix()}" if line.startswith("SF:") else line for line in lines]
        records.append("\n".join(lines) + "\nend_of_record\n")
    if not records:
        raise RuntimeError("LLVM coverage contained no library sources")
    (report / "coverage.info").write_text("".join(records))

    # grcov 0.10.8 treats zero-count branches as covered. Do not publish those
    # branch metrics: Codecov consumes the unmodified LLVM branch counts above.
    subprocess.run([str(args.grcov), str(report / "coverage.info"),
                    "--source-dir", str(source), "--no-demangle",
                    "--output-types", "html,covdir", "--output-path", str(report)], check=True)


if __name__ == "__main__":
    main()
