#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Keep platform selection and the required build-shard inventory together."""
import json
import os
from pathlib import Path


def matrix(event, coverage):
    platforms = [
        ('Linux x64', 'ubuntu-24.04', 'AVX2', 6),
        ('Windows x64', 'windows-2025', 'AVX2', 6),
        ('Windows ARM64', 'windows-11-arm', 'NEON', 2),
        ('Linux ARM64', 'ubuntu-24.04-arm', 'NEON', 1),
        ('macOS ARM64', 'macos-15', 'NEON', 1),
    ]
    extended = coverage or event in ('schedule', 'workflow_dispatch')
    if not coverage and event != 'workflow_dispatch':
        names = {'Linux ARM64'}
        if event == 'schedule':
            names.add('Linux x64')
        if event == 'pull_request':
            names.add('Windows ARM64')
        platforms = [p for p in platforms if p[0] in names]
    modes = ['ON', 'OFF'] if event == 'workflow_dispatch' and not coverage else ['ON']
    jobs = []
    for name, runner, profile, count in platforms:
        count = count if extended else 1
        profiles = ('AVX2;AVX512;AVX512_BF16;AVX512_FP16' if profile == 'AVX2'
                    else 'NEON;NEON_FP16;NEON_BF16')
        for mode in modes:
            for number in range(1, count + 1):
                jobs.append(dict(name=name, runner=runner, profile=profile, profiles=profiles,
                                 exceptions=mode, shard=number if count > 1 else 0,
                                 count=count, package=number == 1,
                                 key=f'{runner}-exceptions-{mode}-shard-{number}',
                                 label=f'{name} / {number} of {count}' +
                                       (' (no exceptions)' if mode == 'OFF' else '')))
    return {'include': jobs}


if __name__ == '__main__':
    value = matrix(os.environ['EVENT_NAME'], os.environ['COVERAGE'] == 'true')
    with open(os.environ['GITHUB_OUTPUT'], 'a') as output:
        output.write('matrix=' + json.dumps(value, separators=(',', ':')) + '\n')
