# SPDX-FileCopyrightText: 2026 Edward Kmett
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Reshape existing Shields workflow badges into single-section endpoint JSON."""

import argparse
import json
from pathlib import Path
from urllib.request import Request, urlopen


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repository", default="ekmett/native")
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=True)
    for name in ("build", "docs", "coverage", "docker", "nix"):
        endpoint = ("https://img.shields.io/github/actions/workflow/status/"
                    f"{args.repository}/{name}.yml.json?branch=main")
        request = Request(endpoint, headers={"User-Agent": "native-ci-badges"})
        with urlopen(request, timeout=30) as response:
            data = json.load(response)
        color = data["color"]
        if not isinstance(color, str) or not color:
            raise ValueError(f"Invalid Shields color for {name}: {color!r}")
        badge = {"schemaVersion": 1, "label": "", "message": name, "color": color}
        (args.output / f"{name}.json").write_text(
            json.dumps(badge, indent=2) + "\n")


if __name__ == "__main__":
    main()
