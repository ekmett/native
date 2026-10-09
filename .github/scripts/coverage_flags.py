#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Label coverage runs using completed, hardware-admitted profile tests."""

import json
import os
from pathlib import Path
import xml.etree.ElementTree as ET


def main():
    results = Path("build/package-tests.xml")
    profiles = {}
    if results.is_file():
        for case in ET.parse(results).iter("testcase"):
            name = case.get("name", "")
            if name not in {"omnibus.avx2", "omnibus.avx512", "omnibus.neon",
                            "omnibus.neon_fp16", "omnibus.neon_bf16"}:
                continue
            status = ("skipped" if case.find("skipped") is not None else
                      "failed" if case.find("failure") is not None or case.find("error") is not None else
                      "passed" if case.get("status", "run") == "run" else "not_run")
            profiles[name.removeprefix("omnibus.")] = status
    prefix = f'{os.environ["RUNNER_OS"]}_{os.environ["RUNNER_ARCH"]}'
    flags = [f"{prefix}_extended"]
    avx512 = profiles.get("avx512") == "passed"
    if avx512:
        flags.append(f"{prefix}_avx512_extended")
    Path("build/core/coverage/report/execution.json").write_text(
        json.dumps({"profile_tests": profiles, "codecov_flags": flags}, indent=2) + "\n")
    with open(os.environ["GITHUB_OUTPUT"], "a", encoding="utf-8") as output:
        output.write(f'flags={",".join(flags)}\n')
        output.write(f'hardware={"avx512" if avx512 else "baseline"}\n')


if __name__ == "__main__":
    main()
