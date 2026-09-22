# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Regenerate the exact arithmetic bank and detect stale expected results."""
from pathlib import Path
import generate_reference

_, actual, _ = generate_reference.split_region(Path(__file__).with_name('checks.h').read_bytes())
if actual != generate_reference.generate():
    raise SystemExit('BF16 reference cases differ from the exact-rational generator')
print('Checked-in matrix/FMA bank matches exact-rational generation')
