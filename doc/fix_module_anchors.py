# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Restore Doxygen module navigation anchors at their existing declarations.

Doxygen 1.18 indexes namespace/group members as module-page details, although
those pages contain only declaration rows and links to the actual details.
Add aliases at those exact rows (or enum-value links). Keep every existing ID,
link and description; refuse ambiguous or missing declarations before writing.
Newer generators that already emit the anchors need no changes.
"""
import argparse
from html.parser import HTMLParser
import json
from pathlib import Path
import re
from urllib.parse import urlsplit


class Page(HTMLParser):
    def __init__(self, text):
        super().__init__()
        self.ids = set()
        self.rows = {}
        self.links = {}
        self.row = None
        self.offsets = [0]
        for line in text.splitlines(keepends=True):
            self.offsets.append(self.offsets[-1] + len(line))
        self.feed(text)

    def handle_starttag(self, tag, attrs):
        attrs = dict(attrs)
        identity = attrs.get('id') or (attrs.get('name') if tag == 'a' else None)
        if identity:
            self.ids.add(identity)
        line, column = self.getpos()
        offset = self.offsets[line - 1] + column
        if tag == 'tr':
            self.row = attrs.get('id')
        elif tag == 'td' and self.row:
            self.rows.setdefault(self.row, []).append(offset + len(self.get_starttag_text()))
            self.row = None  # Only the first cell in this declaration row.
        elif tag == 'a' and 'href' in attrs and not identity:
            anchor = urlsplit(attrs['href']).fragment
            self.links.setdefault(anchor, []).append(offset + 2)

    def handle_endtag(self, tag):
        if tag == 'tr':
            self.row = None


def repair(root):
    root = Path(root)
    if not root.is_dir():
        raise ValueError(f'No generated HTML directory: {root}')
    # Crawl and module navigation use the same generated member destinations.
    references = set()
    for path in [root / 'doxygen_crawl.html', *root.glob('module__*.js')]:
        if path.is_file():
            references.update(re.findall(
                r'"(module__[^"/#]+\.html)#([A-Za-z_][A-Za-z_0-9]*)"',
                path.read_text(encoding='utf-8')))
    pending = {}
    changes = []
    for filename in sorted({name for name, _ in references}):
        path = root / filename
        text = path.read_text(encoding='utf-8')
        page = Page(text)
        insertions = []
        for _, anchor in sorted(item for item in references if item[0] == filename):
            if anchor in page.ids:
                continue
            rows = page.rows.get('r_' + anchor, [])
            links = page.links.get(anchor, [])
            if len(rows) == 1:
                offset, addition, kind = rows[0], f'<a id="{anchor}"></a>', 'declaration'
            elif not rows and len(links) == 1:
                offset, addition, kind = links[0], f' id="{anchor}"', 'enum value'
            else:
                raise ValueError(f'No unique declaration for {filename}#{anchor}')
            insertions.append((offset, addition))
            changes.append({'page': filename, 'anchor': anchor, 'kind': kind})
        for offset, addition in sorted(insertions, reverse=True):
            text = text[:offset] + addition + text[offset:]
        if insertions:
            assert page.ids <= Page(text).ids
            pending[path] = text
    # Validate every destination before changing any generated page.
    for path, text in pending.items():
        with path.open('w', encoding='utf-8', newline='') as output:
            output.write(text)
    return changes


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('html', type=Path)
    args = parser.parse_args()
    print(json.dumps({'module_anchor_aliases': repair(args.html)}, indent=2))


if __name__ == '__main__':
    main()
