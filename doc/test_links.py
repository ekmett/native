# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check the published-site boundary as well as local fragment resolution."""
from contextlib import redirect_stderr, redirect_stdout
from io import StringIO
from pathlib import Path
from tempfile import TemporaryDirectory
import unittest

from check_links import check


class LinkTests(unittest.TestCase):
    def result(self, href):
        with TemporaryDirectory() as directory:
            base = Path(directory)
            root = base / 'site'
            (root / 'guide').mkdir(parents=True)
            (root / 'index.html').write_text(f'<a href="{href}">link</a>', encoding='utf-8')
            (root / 'guide/index.html').write_text('<h1 id="section">Section</h1>', encoding='utf-8')
            (base / 'unpublished.md').write_text('Not in the uploaded artifact.', encoding='utf-8')
            with redirect_stdout(StringIO()), redirect_stderr(StringIO()):
                return check(root)

    def test_no_shader_index_required(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            (root / 'index.html').write_text('<h1 id="api">API</h1>', encoding='utf-8')
            with redirect_stdout(StringIO()), redirect_stderr(StringIO()):
                self.assertFalse(check(root))

    def test_index_required(self):
        with TemporaryDirectory() as directory:
            with redirect_stdout(StringIO()), redirect_stderr(StringIO()):
                self.assertTrue(check(Path(directory)))

    def test_nested_site_anchor(self):
        self.assertFalse(self.result('guide/index.html#section'))

    def test_missing_anchor(self):
        self.assertTrue(self.result('guide/index.html#missing'))

    def test_existing_file_outside_upload(self):
        self.assertTrue(self.result('../unpublished.md'))

    def test_graph_assets_and_destinations(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            (root / 'index.html').write_text(
                '<h1 id="api">API</h1><iframe src="graph.svg"></iframe>', encoding='utf-8')
            graph = root / 'graph.svg'
            script = root / 'svg.min.js'
            graph.write_text(
                '<svg xmlns:xlink="http://www.w3.org/1999/xlink">'
                '<a xlink:href="index.html#api">API</a>'
                '<script xlink:href="svg.min.js"/></svg>', encoding='utf-8')
            script.write_text('// Interactive graph support.', encoding='utf-8')
            with redirect_stdout(StringIO()), redirect_stderr(StringIO()):
                self.assertFalse(check(root))
                graph.write_text(graph.read_text().replace('#api', '#missing'), encoding='utf-8')
                self.assertTrue(check(root))
                graph.write_text(graph.read_text().replace('#missing', '#api'), encoding='utf-8')
                script.unlink()
                self.assertTrue(check(root))
                graph.unlink()
                self.assertTrue(check(root))


if __name__ == '__main__':
    unittest.main()
