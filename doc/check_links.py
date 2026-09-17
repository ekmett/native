# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Reject broken local links in generated HTML and Doxygen module navigation."""
from html.parser import HTMLParser
from pathlib import Path
from urllib.parse import unquote, urlsplit
import re
import sys


class Page(HTMLParser):
    def __init__(self, text):
        super().__init__()
        self.ids = set()
        self.links = []
        self.feed(text)

    def handle_starttag(self, tag, attributes):
        attrs = dict(attributes)
        if attrs.get('id'):
            self.ids.add(attrs['id'])
        if tag == 'a':
            if attrs.get('name'):
                self.ids.add(attrs['name'])
            if attrs.get('href'):
                self.links.append(attrs['href'])


def check(root):
    root = root.resolve(strict=True)
    pages = {p: Page(p.read_text(encoding='utf-8')) for p in root.rglob('*.html')}
    errors = []
    if root / 'index.html' not in pages:
        errors.append('The site index page is required.')
    links = [(p, href) for p, page in pages.items() for href in page.links]
    for p in root.rglob('module*.js'):
        links.extend((p, href) for href in re.findall(r'"([^"\n]+\.html(?:#[^"\n]*)?)"', p.read_text(encoding='utf-8')))
    for source, href in links:
        url = urlsplit(href)
        if url.scheme or url.netloc or not (url.path or url.fragment):
            continue
        target = (source.parent / unquote(url.path)).resolve() if url.path else source
        if not target.is_relative_to(root):
            errors.append(f'{source.relative_to(root)}: outside site {href}')
        elif not target.is_file():
            errors.append(f'{source.relative_to(root)}: missing file {href}')
        elif url.fragment and target in pages and unquote(url.fragment) not in pages[target].ids:
            errors.append(f'{source.relative_to(root)}: missing anchor {href}')
    for error in errors:
        print(error, file=sys.stderr)
    print(f'{len(pages)} HTML pages; {len(links)} local/external links; {len(errors)} broken destinations')
    return bool(errors)


if __name__ == '__main__':
    raise SystemExit(check(Path(sys.argv[1])))
