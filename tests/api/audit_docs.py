#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check callable documentation in the current Doxygen XML index, not stale files."""
import argparse
import json
from pathlib import Path
import xml.etree.ElementTree as ET


def audit(xml, prefixes):
    public = {}
    for compound in ET.parse(xml / 'index.xml').findall('compound'):
        if compound.get('kind') not in ('namespace', 'class', 'struct', 'group'):
            continue
        name = compound.findtext('name', '')
        if 'detail' in name.split('::'):
            continue
        tree = ET.parse(xml / (compound.get('refid') + '.xml'))
        for member in tree.findall('.//memberdef'):
            if member.get('kind') not in ('function', 'friend'):
                continue
            if member.get('prot') not in (None, 'public'):
                continue
            # A documented friend class is not a callable.
            if member.get('kind') == 'friend' and not member.findtext('argsstring', ''):
                continue
            qualified = member.findtext('qualifiedname', member.findtext('name', ''))
            # Groups can hold definitions omitted from the namespace XML. Their
            # group name says nothing about the namespace of an individual member.
            if 'detail' in qualified.split('::'):
                continue
            location = member.find('location')
            if location is None:
                continue
            file = location.get('file', '').replace('\\', '/')
            if not any(file.startswith(prefix) for prefix in prefixes):
                continue
            record = public.setdefault(member.get('id'), {
                'name': qualified,
                'file': file, 'line': int(location.get('line', 0)),
                'documented': False,
            })
            record['documented'] |= any(
                ''.join(description.itertext()).strip()
                for tag in ('briefdescription', 'detaileddescription')
                for description in member.findall(tag)
            )
    missing = [record for record in public.values() if not record['documented']]
    return {'xml': str(xml), 'public_callable_ids': len(public),
            'documented_callable_ids': len(public) - len(missing), 'missing': missing}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('xml', type=Path, nargs='+', help='Doxygen XML output directories')
    parser.add_argument('--source-prefix', action='append', default=[],
                        help='include only these source prefixes (default: src/)')
    args = parser.parse_args()
    results = [audit(xml, args.source_prefix or ['src/']) for xml in args.xml]
    print(json.dumps(results, indent=2))
    # An empty index is not successful documentation coverage.
    return int(any(not result['public_callable_ids'] or result['missing'] for result in results))


if __name__ == '__main__':
    raise SystemExit(main())
