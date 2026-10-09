# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check instruction guide structure and Doxygen's published page hierarchy."""
import json
from pathlib import Path
import re
import sys


def check(html):
    docs = Path(__file__).resolve().parent.parent / 'docs'

    def tree(name):
        text = (html / (name + '.js')).read_text()
        value = text.split('var ' + name + ' =', 1)[1].lstrip()
        return json.JSONDecoder().raw_decode(value)[0]

    def page_children(name):
        return {row[1] for row in tree('md_docs_2' + name) if '#' not in row[1]}

    count = 0
    for arch, title in [('x86', 'x86'), ('arm', 'ARM'), ('wasm', 'WebAssembly')]:
        pages = sorted(docs.glob(arch + '-*.md'))
        index = (docs / (arch + '.md')).read_text()
        links = re.findall(r'^- \[([^]]+)\]\(([^)]+)\)$', index, re.M)
        assert {p.name for p in pages} == {url for _, url in links}, arch
        assert len(links) == len(pages), f'duplicate index entry: {arch}'
        labels = {url: label for label, url in links}
        for page in pages:
            text = page.read_text()
            heading = text.splitlines()[0].removeprefix('# ')
            assert heading.startswith(title) and ': ' in heading, page
            label = heading.removeprefix(title + ': ').removeprefix(title + ' ')
            assert labels[page.name] == label, f'stale index title: {page}'
            assert f'[{title} instruction sets]({arch}.md)' in text, page
            if page.name != 'wasm-features.md':
                assert re.findall(r'^## (.+)$', text, re.M) == [
                    'Why use it', 'Operations', 'Caveats'], page
                for section in re.split(r'^## .+\n', text, flags=re.M)[1:]:
                    assert section.strip(), f'empty section: {page}'
            count += 1
        expected = {'md_docs_2' + p.stem + '.html' for p in pages}
        assert page_children(arch) == expected, f'wrong sidebar children: {arch}'

    assert page_children('instructions') == {
        'md_docs_2' + arch + '.html' for arch in ['x86', 'arm', 'wasm']}
    assert page_children('index') == {'md_doc_2building.html'} | {
        'md_docs_2' + page + '.html' for page in
        ['modules', 'polyfill-operations', 'abi-lookup', 'omnibus', 'transcendentals', 'instructions']}
    topics = {row[1] for row in tree('topics')}
    for arch in ['arm', 'x86', 'wasm']:
        parent = 'group__cpu__' + arch
        assert parent + '.html' in topics, f'missing CPU topic: {arch}'
        children = tree(parent)
        expected = {p.name for p in html.glob('group__' + arch + '__*.html')}
        if arch == 'x86':
            expected |= {'group__capabilities.html', 'group__wait.html'}
        assert {row[1] for row in children} == expected, f'wrong CPU topics: {arch}'
        assert not topics & expected, f'instruction topic leaked to root: {arch}'
        names = [row[0] for row in children]
        assert names == sorted(names), f'unsorted instruction topics: {arch}'
    print(f'{count} guides: titles, sections, index coverage and sidebar hierarchy pass')
    print('API topics: CPU families, instruction-set names and alphabetical order pass')


if __name__ == '__main__':
    check(Path(sys.argv[1]))
