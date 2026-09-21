# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Regenerate the exact arithmetic bank and detect stale expected results."""
from pathlib import Path
from tempfile import TemporaryDirectory
import generate_reference

with TemporaryDirectory() as directory:
    output=Path(directory)/'reference_cases.h'
    generate_reference.generate(output)
    if output.read_bytes()!=Path(__file__).with_name('reference_cases.h').read_bytes():
        raise SystemExit('BF16 reference cases differ from the exact-rational generator')
print('Checked-in matrix/FMA bank matches exact-rational generation')
