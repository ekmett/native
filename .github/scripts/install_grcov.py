#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Install the coverage reporter used by the existing CI test build."""

import hashlib
import os
from pathlib import Path
import tarfile
import urllib.request
import zipfile

VERSION = "0.10.8"
ARCHIVES = {
    ("Linux", "X64"): ("x86_64-unknown-linux-gnu.tar.bz2", "25ef3d08b660fe82cd7306dd5af4fa59aa7342718fe89d98ef95064c557e28ab"),
    ("Linux", "ARM64"): ("aarch64-unknown-linux-gnu.tar.bz2", "c23533dbca2e1500ebbedc6d89b9926a5193faa772f849ade9291d5365af31e6"),
    ("macOS", "ARM64"): ("aarch64-apple-darwin.tar.bz2", "c5441aae995470192afcd93f8da9fd56cf38a2caf910d3eafa7927ff847deba3"),
    ("macOS", "X64"): ("x86_64-apple-darwin.tar.bz2", "7100963cce45cdebe1a3fcda3e9472ba10b3fc117d6da741671af2a01dcbbdf3"),
    ("Windows", "X64"): ("x86_64-pc-windows-msvc.zip", "a4da82112c8ddd7bd2db618b85d8cc40606ddd74b43140db9209895f5e73b771"),
    ("Windows", "ARM64"): ("aarch64-pc-windows-msvc.zip", "80eba01238b735407f5674d9f677c51567ea3a1dba3c48cec2bf999defffe9da"),
}


def main():
    name, digest = ARCHIVES[os.environ["RUNNER_OS"], os.environ["RUNNER_ARCH"]]
    destination = Path(os.environ["RUNNER_TEMP"]) / "grcov"
    destination.mkdir(parents=True, exist_ok=True)
    archive = destination / name
    urllib.request.urlretrieve(
        f"https://github.com/mozilla/grcov/releases/download/v{VERSION}/grcov-{name}", archive)
    if hashlib.sha256(archive.read_bytes()).hexdigest() != digest:
        raise RuntimeError(f"SHA256 mismatch: {archive}")
    if name.endswith(".zip"):
        with zipfile.ZipFile(archive) as package:
            package.extractall(destination)
    else:
        with tarfile.open(archive) as package:
            package.extractall(destination, filter="data")
    with open(os.environ["GITHUB_PATH"], "a", encoding="utf-8") as path:
        path.write(f"{destination}\n")


if __name__ == "__main__":
    main()
