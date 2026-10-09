# SPDX-FileCopyrightText: 2026 Edward Kmett
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Write Shields endpoint JSON for the latest main-branch workflow runs."""

import argparse
import json
from pathlib import Path
import subprocess


def badge(name, run):
    if not run:
        color = "lightgrey"
    elif run["status"] != "completed":
        color = "yellow"
    else:
        color = {
            "success": "brightgreen",
            "failure": "red",
            "timed_out": "red",
            "startup_failure": "red",
            "action_required": "orange",
            "cancelled": "lightgrey",
            "skipped": "lightgrey",
            "neutral": "lightgrey",
        }.get(run["conclusion"], "lightgrey")
    return {"schemaVersion": 1, "label": "", "message": name, "color": color}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repository", default="ekmett/native")
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=True)
    for name in ("build", "docs", "coverage", "docker", "nix"):
        endpoint = (f"repos/{args.repository}/actions/workflows/{name}.yml/runs"
                    "?branch=main&per_page=1")
        data = json.loads(subprocess.check_output(["gh", "api", endpoint], text=True))
        runs = data["workflow_runs"]
        run = runs[0] if runs else None
        (args.output / f"{name}.json").write_text(
            json.dumps(badge(name, run), indent=2) + "\n")


if __name__ == "__main__":
    main()
