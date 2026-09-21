#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Regression controls for the Doxygen documentation coverage audit."""
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

from audit_docs import audit


def member(identifier, name, description='', kind='function', prot='public', arguments='()'):
    arguments = f'<argsstring>{arguments}</argsstring>' if arguments else ''
    return f'''<memberdef id="{identifier}" kind="{kind}" prot="{prot}">
      <name>{name.split('::')[-1]}</name><qualifiedname>{name}</qualifiedname>
      {arguments}<briefdescription><para>{description}</para></briefdescription>
      <location file="src/native/example.h" line="10"/>
    </memberdef>'''


class AuditTests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.xml = Path(self.directory.name)

    def write(self, groups):
        index = []
        for ref, kind, name, members in groups:
            index.append(f'<compound refid="{ref}" kind="{kind}"><name>{name}</name></compound>')
            (self.xml / (ref + '.xml')).write_text(
                '<doxygen><compounddef><sectiondef>' + ''.join(members) +
                '</sectiondef></compounddef></doxygen>')
        (self.xml / 'index.xml').write_text('<doxygenindex>' + ''.join(index) + '</doxygenindex>')

    def test_group_only_definition_cannot_disappear_from_coverage(self):
        self.write([('utilities', 'group', 'utilities', [member('missing', 'native::floor')])])
        # A stale prior render must not provide documentation for the current ID.
        (self.xml / 'stale.xml').write_text('<doxygen>' + member('missing', 'native::floor', 'Old docs') + '</doxygen>')
        result = audit(self.xml, ['src/'])
        self.assertEqual(result['public_callable_ids'], 1)
        self.assertEqual(result['documented_callable_ids'], 0)
        self.assertEqual([m['name'] for m in result['missing']], ['native::floor'])

    def test_duplicate_ids_merge_descriptions_but_not_helpers_or_friend_types(self):
        self.write([
            ('namespace', 'namespace', 'native', [member('one', 'native::floor')]),
            ('utilities', 'group', 'utilities', [
                member('one', 'native::floor', 'Round down.'),
                member('hidden', 'native::detail::implementation'),
                member('private', 'native::vec::helper', prot='private'),
                member('friend_type', 'native::static_c_string', kind='friend', arguments=''),
                member('friend_call', 'native::static_c_string::operator==', 'Compare identities.', kind='friend'),
            ]),
        ])
        result = audit(self.xml, ['src/'])
        self.assertEqual(result['public_callable_ids'], 2)
        self.assertEqual(result['documented_callable_ids'], 2)
        self.assertEqual(result['missing'], [])

    def test_empty_current_index_fails_the_command(self):
        self.write([])
        command = subprocess.run([sys.executable, str(Path(__file__).with_name('audit_docs.py')), str(self.xml)],
                                 capture_output=True, text=True)
        self.assertEqual(command.returncode, 1)
        self.assertEqual(json.loads(command.stdout)[0]['public_callable_ids'], 0)


if __name__ == '__main__':
    unittest.main()
