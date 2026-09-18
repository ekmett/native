#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Real POSIX Clang/sccache regression; run without concurrent cache requests.

Usage: python3 test_sccache_pch.py /absolute/clang++ /evidence/directory
Set PATH to sccache before running. A unique fixture child is created each run.
The cache and counters are never cleared; assertions use counter deltas.
"""
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import uuid


def main():
    compiler = sys.argv[1]
    root = Path(sys.argv[2]).absolute() / uuid.uuid4().hex
    root.mkdir(parents=True, exist_ok=False)
    launcher = Path(__file__).with_name('sccache_launcher.py').absolute()
    os.chdir(root)

    def run(arguments):
        result = subprocess.run(arguments, text=True, stdout=subprocess.PIPE,
                                stderr=subprocess.STDOUT)
        with (root / 'commands.log').open('a') as stream:
            stream.write(json.dumps(arguments) + '\n' + result.stdout + '\n')
        if result.returncode:
            raise RuntimeError(f'exit {result.returncode}: {arguments}\n{result.stdout}')
        return result.stdout

    run([compiler, '--version'])
    run(['sccache', '--version'])
    flags = ['-std=c++20', '-O2', '-flto=thin']
    header = Path('prefix.h')
    header.write_text('#pragma once\ninline constexpr int value = 42;\n')
    Path('probe.cppm').write_text('module;\n#include "prefix.h"\n'
                                'export module probe;\nexport int answer() { return value; }\n')
    Path('consumer.cc').write_text('import probe;\nint use() { return answer(); }\n')
    Path('probe.modmap').write_text('-x c++-module\n-fmodule-output="probe.pcm"\n')
    pch = ['-Xclang', '-include-pch', '-Xclang', str(root / 'prefix.pch')]
    producer = [sys.executable, str(launcher), compiler, *flags, *pch,
                '@probe.modmap', '-c', 'probe.cppm', '-o', 'probe.o']

    def build_pch():
        run([compiler, *flags, '-x', 'c++-header', 'prefix.h', '-o', 'prefix.pch'])

    def preprocess():
        return run([compiler, *flags, *pch, '-E', '-P', '-fminimize-whitespace',
                    '-x', 'c++-module', 'probe.cppm'])

    def counters(stats):
        return tuple(int(re.search(r'^Cache ' + field + r'\s+(\d+)\s*$', stats, re.M)[1])
                     for field in ['hits', 'misses'])

    def stage(name, expected_hits):
        # Remove only these two fixture outputs, retaining all cached artifacts.
        for filename in ['probe.o', 'probe.pcm']:
            Path(filename).unlink(missing_ok=True)
        previous = counters(run(['sccache', '--show-stats']))
        run(producer)
        stats = run(['sccache', '--show-stats'])
        (root / (name + '.stats')).write_text(stats)
        hits, misses = (after - before for after, before in zip(counters(stats), previous))
        assert (hits, misses) == (expected_hits, 1 - expected_hits), (name, stats)
        # Deliberately uncached: Clang must validate the actual restored BMI/PCH.
        run([compiler, *flags, '-fmodule-file=probe=probe.pcm', '-c', 'consumer.cc',
             '-o', 'consumer.o'])
        print(f'{name}: hits={hits}, misses={misses}; fresh importer passed', flush=True)

    build_pch()
    initial_preprocessed = preprocess()
    initial_hash = hashlib.sha256(Path('prefix.pch').read_bytes()).hexdigest()
    stage('cold', 0)
    stage('warm', 1)
    header.write_text(header.read_text() + '// A longer comment changes serialized input metadata.\n')
    build_pch()
    assert initial_hash != hashlib.sha256(Path('prefix.pch').read_bytes()).hexdigest()
    assert initial_preprocessed == preprocess(), 'fixture must keep preprocessing equivalent'
    stage('changed-pch', 0)
    stage('changed-pch-warm', 1)
    before = Path('prefix.pch').read_bytes()
    stat = Path('prefix.pch').stat()
    os.utime('prefix.pch', ns=(stat.st_atime_ns, stat.st_mtime_ns + 10_000_000_000))
    assert before == Path('prefix.pch').read_bytes()
    stage('mtime-only', 1)
    print('PASS: PCH binary invalidation, warm reuse, timestamp-only restoration; ThinLTO enabled')


if __name__ == '__main__':
    main()
