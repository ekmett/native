# SPDX-FileCopyrightText: 2026 Edward Kmett
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Exercise site assembly, guide links, theme assets and output boundaries."""
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest
from build import build


class SiteTest(unittest.TestCase):
    def test_assembled_site(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source, api, output = root / 'source', root / 'api', root / 'site'
            (source / 'docs').mkdir(parents=True)
            api.mkdir()
            shutil.copytree(Path(__file__).parent, source / 'docs/site', ignore=shutil.ignore_patterns('__pycache__'))
            (source / 'README.md').write_text('# Demo\n\n[Guide](docs/guide.md#details)\n\n![License](assets/badges/license.svg)\n')
            (source / 'assets/badges').mkdir(parents=True)
            (source / 'assets/badges/license.svg').write_text('<svg xmlns="http://www.w3.org/2000/svg"/>')
            (source / 'LICENSE.md').write_text('# License\n')
            (source / 'doc').mkdir()
            (source / 'doc/building.md').write_text('# Building\n')
            (source / 'docs/index.md').write_text('# Guides\n\n- [Build](../doc/building.md)\n- [CPU](cpu.md)\n')
            (source / 'docs/cpu.md').write_text('# CPU\n\n- [Guide](guide.md)\n')
            (source / 'docs/guide.md').write_text('# Guide\n\n## Details\n\n[Home](../README.md)\n')
            for name in ('index.html', 'annotated.html', 'files.html'):
                (api / name).write_text('<html><head><title>API</title></head><body>API</body></html>')
            subprocess.run(['git', 'init', '-q', str(source)], check=True)
            subprocess.run(['git', '-C', str(source), '-c', 'user.name=Test', '-c', 'user.email=test@example.invalid', 'commit', '-qm', 'Fixture', '--allow-empty'], check=True)
            build(source, api, output, 'work', 'pandoc')
            home = (output / 'home.html').read_text()
            self.assertIn('guides/docs/guide.html#details', home)
            self.assertIn('src="assets/badges/license.svg"', home)
            self.assertEqual((output / 'assets/badges/license.svg').read_text(),
                             (source / 'assets/badges/license.svg').read_text())
            self.assertIn('../../home.html', (output / 'guides/docs/guide.html').read_text())
            self.assertIn('../assets/reference.css', (output / 'api/index.html').read_text())
            shell = (output / 'index.html').read_text()
            self.assertIn('>CPU</summary>', shell)
            self.assertIn('guides/doc/building.html', shell)
            self.assertIn('location.search + location.hash', (output / 'files.html').read_text())
            revision = subprocess.check_output(['git', '-C', str(source), 'rev-parse', 'HEAD'], text=True).strip()
            self.assertIn(f'assets/site.css?v={revision}', shell)
            self.assertIn(f'home.html?v={revision}', shell)
            self.assertIn(f'assets/reference.css?v={revision}', (output / 'api/index.html').read_text())
            self.assertIn('data-thc-appearance="light"', shell)
            self.assertIn('data-thc-appearance="dark"', shell)
            self.assertIn('data-thc-appearance="system"', shell)
            (output / 'stale.html').touch()
            build(source, api, output, 'work', 'pandoc')
            self.assertFalse((output / 'stale.html').exists())
            with self.assertRaises(ValueError):
                build(source, api, source, 'work', 'pandoc')
            (source / 'docs/guide.md').write_text('# Guide\n\n[Missing](../README.md#absent)\n')
            with self.assertRaises(SystemExit):
                build(source, api, output, 'work', 'pandoc')


if __name__ == '__main__':
    unittest.main()
