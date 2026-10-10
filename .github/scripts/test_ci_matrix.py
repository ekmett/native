#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check the small CI matrix contract without building the library."""
import json
import re
import unittest
from collections import Counter
from pathlib import Path
from ci_matrix import matrix


class MatrixTests(unittest.TestCase):
    def test_platforms_and_package_owner(self):
        cases = [('push', False, 1), ('pull_request', False, 2),
                 ('schedule', False, 7), ('push', True, 21),
                 ('workflow_dispatch', False, 42)]
        for event, coverage, count in cases:
            jobs = matrix(event, coverage)['include']
            self.assertEqual(len(jobs), count)
            self.assertEqual(len({p['key'] for p in jobs}), count)
            owners = Counter((p['runner'], p['exceptions']) for p in jobs if p['package'])
            self.assertEqual(set(owners), {(p['runner'], p['exceptions']) for p in jobs})
            self.assertTrue(all(n == 1 for n in owners.values()))

    def test_complete_group_inventory(self):
        root = Path(__file__).resolve().parents[2]
        groups = json.loads((root / 'etc/cmake/test-shards.json').read_text())
        directories = set(re.findall(r'native_test_directory\(([^)]+)\)',
                                     (root / 'CMakeLists.txt').read_text()))
        expected = directories - {'core_regression'}
        expected |= {'core:other', 'core:promoted_base2', 'core:promoted_exp_degree'}
        self.assertEqual(set(groups), expected)
        self.assertTrue(all(isinstance(bucket, int) and 1 <= bucket <= 6 for bucket in groups.values()))


if __name__ == '__main__':
    unittest.main()
