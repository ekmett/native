# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Preserve even an empty-array implementation's noexcept termination behavior."""
import subprocess
import sys

for form in ('default', 'value'):
    before = subprocess.run([sys.argv[1], 'old-' + form], capture_output=True, text=True)
    after = subprocess.run([sys.argv[1], 'new-' + form], capture_output=True, text=True)
    if before.returncode not in (0, 86):
        raise SystemExit(f'{form}: unexpected original exit {before.returncode}: {before.stderr}')
    if (before.returncode, before.stdout, before.stderr) != (after.returncode, after.stdout, after.stderr):
        raise SystemExit(f'{form}: changed empty construction: {before!r} != {after!r}')
    print(f'{form}: old/new exit {after.returncode}, {after.stdout.strip()}')
