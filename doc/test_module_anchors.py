# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Controls for generated module declaration anchors, with no Doxygen needed."""
from pathlib import Path
import tempfile
import unittest
from fix_module_anchors import repair, Page


class Anchors(unittest.TestCase):
    def fixture(self, root, body, anchors):
        path = root / 'module__example.html'
        path.write_text(body, encoding='utf-8')
        (root / 'module__example.js').write_text(
            'var module__example = ' + repr([]) + ';\n' +
            '\n'.join(f'"module__example.html#{anchor}"' for anchor in anchors), encoding='utf-8')
        return path

    def test_declaration_and_enum_ownership(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            text = '<table><tr id="r_ga123"><td>int</td><td><a href="group.html#ga999">f</a></td></tr></table>\n<a href="namespace.html#a456">choice</a>'
            path = self.fixture(root, text, ['ga123', 'a456'])
            changes = repair(root)
            self.assertEqual(len(changes), 2)
            result = path.read_text(encoding='utf-8')
            self.assertIn('<td><a id="ga123"></a>int</td>', result)
            self.assertIn('<a id="a456" href="namespace.html#a456">', result)
            self.assertIn('href="group.html#ga999"', result)
            self.assertEqual(Page(result).ids, {'r_ga123', 'ga123', 'a456'})
            self.assertEqual(repair(root), [])
            self.assertEqual(path.read_text(encoding='utf-8'), result)

    def test_already_correct_generator(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            text = '<a id="ga123"></a><p>Actual details.</p>'
            path = self.fixture(root, text, ['ga123'])
            self.assertEqual(repair(root), [])
            self.assertEqual(path.read_text(encoding='utf-8'), text)

    def test_ambiguous_or_missing_fails_before_writing(self):
        for body in ['', '<a href="one.html#a456">x</a><a href="two.html#a456">y</a>']:
            with self.subTest(body=body), tempfile.TemporaryDirectory() as directory:
                root = Path(directory)
                text = '<table><tr id="r_ga123"><td>f</td></tr></table>' + body
                path = self.fixture(root, text, ['ga123', 'a456'])
                with self.assertRaises(ValueError):
                    repair(root)
                self.assertEqual(path.read_text(encoding='utf-8'), text)

    def test_missing_directory(self):
        with tempfile.TemporaryDirectory() as directory:
            with self.assertRaises(ValueError):
                repair(Path(directory) / 'missing')

    def test_invalid_second_page_does_not_change_first(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            text = '<table><tr id="r_ga123"><td>f</td></tr></table>'
            first = self.fixture(root, text, ['ga123'])
            (root / 'module__z.html').write_text('<p>No declaration.</p>', encoding='utf-8')
            (root / 'doxygen_crawl.html').write_text('<a href="module__z.html#ga456"/>', encoding='utf-8')
            with self.assertRaises(ValueError):
                repair(root)
            self.assertEqual(first.read_text(encoding='utf-8'), text)

    def test_crawler_without_module_javascript(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            path = root / 'module__example.html'
            path.write_text('<table><tr id="r_ga123"><td>f</td></tr></table>', encoding='utf-8')
            (root / 'doxygen_crawl.html').write_text('<a href="module__example.html#ga123"/>', encoding='utf-8')
            self.assertEqual(len(repair(root)), 1)


if __name__ == '__main__':
    unittest.main()
