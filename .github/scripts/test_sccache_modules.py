#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check module invalidation and ordinary caching using an isolated local server."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import socket
import subprocess
import sys
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--compiler', required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--cmake', default='cmake')
    parser.add_argument('--launcher', type=Path,
                        default=Path(__file__).with_name('sccache_launcher.py'))
    parser.add_argument('--cmake-arg', action='append', default=[])
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=True)
    root = Path(tempfile.mkdtemp(prefix='module-cache-', dir=args.output.resolve()))
    source = root / 'source'
    build = root / 'build'
    source.mkdir()
    (source / 'CMakeLists.txt').write_text('''cmake_minimum_required(VERSION 4.4)
project(module_cache_regression LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_library(owner STATIC)
target_sources(owner PUBLIC FILE_SET CXX_MODULES FILES owner.ccm)
add_executable(consumer main.cc)
target_link_libraries(consumer PRIVATE owner)
add_executable(ordinary ordinary.cc)
set_target_properties(ordinary PROPERTIES CXX_SCAN_FOR_MODULES OFF)
set_target_properties(consumer ordinary PROPERTIES CXX_COMPILER_LAUNCHER "${TEST_LAUNCHER}")
''')
    (source / 'main.cc').write_text('import cache_regression;\nint main() { return answer; }\n')
    (source / 'ordinary.cc').write_text('int main() { return 7; }\n')
    provider = source / 'owner.ccm'
    provider.write_text('export module cache_regression;\nexport constexpr int answer=1;\n')
    # Neither the workflow's cache nor its daemon is stopped, cleared or reused.
    environment = {key: value for key, value in os.environ.items()
                   if not key.upper().startswith('SCCACHE_')}
    with socket.socket() as address:
        address.bind(('127.0.0.1', 0))
        port = address.getsockname()[1]
    config = root / 'sccache.toml'
    config.write_text('')
    environment.update(SCCACHE_CONF=str(config), SCCACHE_CACHED_CONF=str(root / 'cached-config'),
                       SCCACHE_DIR=str(root / 'cache'),
                       SCCACHE_SERVER_PORT=str(port), SCCACHE_IDLE_TIMEOUT='0',
                       SCCACHE_ERROR_LOG=str(root / 'sccache.log'))
    compiler = shutil.which(args.compiler) or args.compiler
    # Resolve the same PATH entry that the compiler launcher invokes.
    cache = shutil.which('sccache') or 'sccache'
    observations = []

    def run(command, **options):
        print('+', ' '.join(map(str, command)), flush=True)
        return subprocess.run(list(map(str, command)), env=environment, check=True, **options)

    def statistics():
        result = run([cache, '--show-stats', '--stats-format=json'], capture_output=True, text=True)
        return json.loads(result.stdout)

    def hit_count(stats):
        return sum(stats['stats']['cache_hits']['counts'].values())

    def object_file(target, basename):
        objects = [path for path in (build / ('CMakeFiles/' + target + '.dir')).rglob(basename + '.*')
                   if path.suffix in ('.o', '.obj')]
        if len(objects) != 1:
            raise RuntimeError('Expected one object for ' + target + ': ' + str(objects))
        return objects[0]

    def record(label):
        run([args.cmake, '--build', build, '--parallel'])
        executable = build / ('consumer.exe' if os.name == 'nt' else 'consumer')
        answer = subprocess.run([str(executable)], env=environment, check=False).returncode
        contents = object_file('consumer', 'main.cc').read_bytes()
        observation = {'phase': label, 'returned_answer': answer,
                       'object_sha256': hashlib.sha256(contents).hexdigest()}
        observations.append(observation)
        print(json.dumps(observation), flush=True)
        (root / 'results.json').write_text(json.dumps(observations, indent=2) + '\n')
        return answer

    run([cache, '--start-server'])
    try:
        launcher = str(Path(sys.executable).resolve()) + ';' + str(args.launcher.resolve())
        run([args.cmake, '-S', source, '-B', build, '-G', 'Ninja',
             '-DCMAKE_BUILD_TYPE=Release', '-DCMAKE_CXX_COMPILER=' + compiler,
             '-DTEST_LAUNCHER=' + launcher, *args.cmake_arg])
        initial = record('initial')
        provider.write_text('export module cache_regression;\nexport constexpr int answer=2;\n')
        changed = record('changed_module')
        before = statistics()
        object_file('ordinary', 'ordinary.cc').unlink()
        run([args.cmake, '--build', build, '--target', 'ordinary', '--parallel'])
        after = statistics()
        (root / 'cache-statistics.json').write_text(json.dumps({'before': before, 'after': after}, indent=2) + '\n')
        executable = build / ('ordinary.exe' if os.name == 'nt' else 'ordinary')
        ordinary = subprocess.run([str(executable)], env=environment, check=False).returncode
        if initial != 1 or changed != 2:
            raise RuntimeError('The importer did not observe the changed module: ' + str(observations))
        if ordinary != 7 or hit_count(after) <= hit_count(before):
            raise RuntimeError('An unchanged ordinary translation unit did not produce a valid cache hit')
        print('Changed module observed; unchanged ordinary translation unit reused from cache.', flush=True)
    finally:
        subprocess.run([cache, '--stop-server'], env=environment, check=False)


if __name__ == '__main__':
    main()
