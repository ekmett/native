#!/usr/bin/env python3
"""Bounded Apple ARM64 CPU qualification. The caller supplies the host resource gate."""
# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import time


def sha(path):
    h = hashlib.sha256()
    with path.open('rb') as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b''):
            h.update(chunk)
    return h.hexdigest()


def main():
    p = argparse.ArgumentParser(__doc__)
    p.add_argument('--source', type=Path, required=True)
    p.add_argument('--out', type=Path, required=True)
    p.add_argument('--tools', type=Path, required=True)
    p.add_argument('--source-commit', required=True)
    p.add_argument('--lease-env', help='optional host-gate lease variable to require')
    args = p.parse_args()
    root, out, tools = (v.resolve() for v in (args.source, args.out, args.tools))
    if out.exists():
        p.error('--out must be fresh to preserve prior attempts')
    if args.lease_env and not os.environ.get(args.lease_env):
        p.error('--lease-env names an unset or empty host lease variable')
    out.mkdir(parents=True)
    llvm = tools / 'LLVM-23.1.1-macOS-ARM64/bin'
    cmake = tools / 'cmake-4.4.3-macos-universal/CMake.app/Contents/bin/cmake'
    ctest = cmake.with_name('ctest')
    report = dict(source_commit=args.source_commit, lease_env=args.lease_env, commands=[], passed=False,
                  scope='Apple ARM64 CPU; no GPU, native Metal or timing claim')
    source_files = [q for d in ('src', 'etc/cmake', 'tests')
                    for q in (root/d).rglob('*') if q.is_file() and '__pycache__' not in q.parts]
    source_files.append(root/'CMakeLists.txt')
    def pins():
        return {q.relative_to(root).as_posix(): sha(q) for q in sorted(source_files)}
    report['source_before'] = pins()
    def save():
        (out/'report.json').write_text(json.dumps(report, indent=2)+'\n')
    def run(label, argv):
        argv = list(map(str, argv)); start = time.time()
        with (out/(label+'.stdout')).open('w') as stdout, (out/(label+'.stderr')).open('w') as stderr:
            result = subprocess.run(argv, cwd=root, stdout=stdout, stderr=stderr)
        item = dict(label=label, argv=argv, cwd=str(root), exit_code=result.returncode,
                    seconds=time.time()-start, stdout_sha256=sha(out/(label+'.stdout')),
                    stderr_sha256=sha(out/(label+'.stderr')))
        report['commands'].append(item); save(); print(label, result.returncode, flush=True)
        if result.returncode:
            print((out/(label+'.stdout')).read_text()[-12000:])
            print((out/(label+'.stderr')).read_text()[-12000:])
            raise RuntimeError(label+' failed')
    try:
        run('compiler-version', [llvm/'clang++', '--version'])
        run('cmake-version', [cmake, '--version'])
        run('ninja-version', ['/opt/homebrew/bin/ninja', '--version'])
        report['tool_hashes'] = {str(q): sha(q) for q in (llvm/'clang++', llvm/'clang-scan-deps', cmake, ctest)}
        common = ['-G', 'Ninja', '-DCMAKE_MAKE_PROGRAM=/opt/homebrew/bin/ninja',
                  '-DCMAKE_CXX_COMPILER='+str(llvm/'clang++'), '-DCMAKE_BUILD_TYPE=Release']
        for tag in ('raw',):
            build = out/tag
            run(tag+'-configure', [cmake, '-S', root, '-B', build, *common,
                '-DNATIVE_TEST_ISA=NEON', '-DNATIVE_PROFILES=NEON',
                '-DNATIVE_BUILD_TESTS=ON', '-DNATIVE_ENABLE_PCH=ON', '-DNATIVE_ENABLE_IPO=ON'])
            cache = (build/'CMakeCache.txt').read_text()
            required_cache = ('NATIVE_TEST_ISA:STRING=NEON', 'NATIVE_PROFILES:STRING=NEON',
                              'NATIVE_BUILD_TESTS:BOOL=ON', 'NATIVE_ENABLE_PCH:BOOL=ON',
                              'NATIVE_ENABLE_IPO:BOOL=ON')
            if not all(line in cache.splitlines() for line in required_cache):
                raise RuntimeError('configured package did not consume the requested NEON/PCH/IPO options')
            report[tag+'_configured_options'] = list(required_cache)
            run(tag+'-build', [cmake, '--build', build, '--parallel', '2'])
            run(tag+'-ctest', [ctest, '--test-dir', build, '-j1', '--output-on-failure',
                              '--output-junit', out/(tag+'-ctest.xml')])
            prefix = out/(tag+' install original')
            run(tag+'-install', [cmake, '--install', build, '--prefix', prefix])
            relocated = out/(tag+' relocated package')
            prefix.rename(relocated)
            consumer = out/(tag+' consumer source'); consumer.mkdir()
            shutil.copyfile(root/'tests/neon/package/CMakeLists.txt', consumer/'CMakeLists.txt')
            for name in ('bridge.cc', 'neon.cc', 'contract.h'):
                shutil.copyfile(root/'tests/isa_profiles'/name, consumer/name)
            (consumer/'support').mkdir()
            for name in ('guarded_pages.h', 'failure.h', 'fp_environment.h', 'profile.h'):
                shutil.copyfile(root/'tests/core_regression/support'/name, consumer/'support'/name)
            cb = out/(tag+' consumer build')
            run(tag+'-consumer-configure', [cmake, '-S', consumer, '-B', cb, *common,
                                            '-DCMAKE_PREFIX_PATH='+str(relocated)])
            run(tag+'-consumer-build', [cmake, '--build', cb, '--parallel', '2'])
            run(tag+'-consumer-ctest', [ctest, '--test-dir', cb, '-j1', '--output-on-failure',
                                      '--output-junit', out/(tag+'-consumer.xml')])
            # Installed module rebuilds must not refer back to build/source headers.
            ninja = (cb/'build.ninja').read_text()
            for forbidden in (root/'src', build, prefix):
                if str(forbidden)+'/' in ninja:
                    raise RuntimeError('consumer leaked '+str(forbidden))
            report[tag+'_capture_sha256'] = sha(build/'tests/isa_profiles/neon.bin')
            report[tag+'_consumer_sha256'] = sha(cb/'neon.bin')
            if report[tag+'_capture_sha256'] != report[tag+'_consumer_sha256']:
                raise RuntimeError('build-tree and relocated-package NEON packets differ')
        for mode in ('ordinary', 'asan'):
            build=out/('memory-'+mode)
            run('memory-'+mode+'-configure', [cmake, '-S', root/'tests/memory_portability',
                '-B', build, *common, '-DNATIVE_MEMORY_ASAN='+('ON' if mode=='asan' else 'OFF')])
            run('memory-'+mode+'-build', [cmake, '--build', build, '--parallel', '2'])
            run('memory-'+mode+'-ctest', [ctest, '--test-dir', build, '-j1', '--output-on-failure',
                                       '--output-junit', out/('memory-'+mode+'.xml')])
        build = out/'swizzle-asan'
        run('swizzle-asan-configure', [cmake, '-S', root, '-B', build, *common,
            '-DNATIVE_TEST_ISA=NEON', '-DNATIVE_PROFILES=NEON',
            '-DNATIVE_BUILD_TESTS=ON', '-DNATIVE_ENABLE_PCH=ON',
            '-DNATIVE_ENABLE_IPO=OFF', '-DNATIVE_ENABLE_ASAN=ON'])
        run('swizzle-asan-build', [cmake, '--build', build, '--parallel', '2',
            '--target', 'native_test_swizzle_header', 'native_test_swizzle_import'])
        run('swizzle-asan-ctest', [ctest, '--test-dir', build, '-j1',
            '-R', '^native[.]swizzle[.]', '--output-on-failure',
            '--output-junit', out/'swizzle-asan.xml'])
        report['source_after'] = pins()
        if report['source_before'] != report['source_after']:
            raise RuntimeError('source changed during qualification')
        report['passed'] = True
    except Exception as error:
        report['error'] = str(error)
        report['source_after'] = pins()
        save(); raise
    save()


if __name__ == '__main__':
    main()
