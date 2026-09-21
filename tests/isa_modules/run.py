# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Rebuild the standalone fixture. Invoke through the host CPU resource gate."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import shutil
import subprocess


def main():
  parser = argparse.ArgumentParser(description=__doc__)
  parser.add_argument('--build-dir', type=Path, required=True)
  parser.add_argument('--compiler', required=True, help='Path to clang-cl.exe')
  parser.add_argument('--cmake', default='cmake')
  parser.add_argument('--ninja', default='ninja')
  parser.add_argument('--thin-lto', action='store_true')
  parser.add_argument('--check-incompatible-imports', action='store_true')
  args = parser.parse_args()
  source = Path(__file__).resolve().parent
  output = args.build_dir.resolve()
  if output.exists() and any(output.iterdir()):
    parser.error('--build-dir must be new or empty; previous receipts are preserved')
  output.mkdir(parents=True, exist_ok=True)
  build = output / 'cmake'
  cmake = shutil.which(args.cmake)
  ninja = shutil.which(args.ninja)
  compiler = shutil.which(args.compiler)
  if not all((cmake, ninja, compiler)):
    parser.error('CMake, Ninja, and clang-cl must be available in the configured compiler environment')
  sha = lambda data: hashlib.sha256(data).hexdigest()
  paths = ['../../src/native.isa.ccm', '../../src/native.x86.features.ccm', '../../src/native/attributes.h', '../../etc/cmake/nativeProfile.cmake', 'CMakeLists.txt', 'backend.inc', 'consumer.cc', 'dispatcher.cc', 'engine.h', 'fixture.engine.ccm']
  for backend in ('avx2', 'avx512'):
    paths += [backend + '-header.h', backend + '.cc', 'bridge_' + backend + '.cc', 'simd.' + backend + '.ccm']
  pins = {name: sha((source / name).read_bytes()) for name in paths}
  report = {'schema': 'native.isa-modules.run.v1', 'source_pins_before': pins,
            'compiler_sha256': sha(Path(compiler).read_bytes()), 'thin_lto': args.thin_lto,
            'commands': [], 'skipped': [], 'passed': False}

  def run(name, command, allowed=(0,)):
    result = subprocess.run(list(map(str, command)), stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    (output / (name + '.log')).write_bytes(result.stdout)
    report['commands'].append({'name': name, 'argv': list(map(str, command)),
                              'exit': result.returncode, 'log_sha256': sha(result.stdout)})
    print(name, result.returncode, flush=True)
    if result.returncode not in allowed:
      raise RuntimeError(name + ' failed; inspect its retained log')
    return result

  def compile_target(target, allowed=(0,)):
    return run(target + '-build', [cmake, '--build', build, '--target', target,
                                  '--parallel', '2', '--verbose'], allowed)

  try:
    run('configure', [cmake, '-S', source, '-B', build, '-G', 'Ninja',
                     '-DCMAKE_CXX_COMPILER=' + compiler, '-DCMAKE_BUILD_TYPE=Release',
                     '-DCMAKE_MAKE_PROGRAM=' + ninja,
                     '-DPROBE_THIN_LTO=' + ('ON' if args.thin_lto else 'OFF')])
    compile_target('dispatcher')
    observed = run('dispatcher-run', [build / 'dispatcher.exe'], allowed=(0, 3))
    match = re.search(rb'supported=(\d+)', observed.stdout)
    supported = int(match[1]) if match else 0
    if observed.returncode == 0 and supported not in (1, 3):
      raise RuntimeError('successful dispatcher lacks the actual CPU/OS admission result')
    report['observed_supported_backends'] = supported
    for name, flag in [('avx2', 1), ('avx512', 2)]:
      if supported & flag:
        run('dispatcher-' + name, [build / 'dispatcher.exe', name])
      else:
        report['skipped'].append('dispatcher-' + name + ': CPU/OS admission unavailable')
    for target, flag in [('only_avx2', 1), ('only_avx512', 2), ('both_avx512', 2)]:
      compile_target(target)
      if supported & flag:
        run(target + '-run', [build / (target + '.exe')])
      else:
        report['skipped'].append(target + ': CPU/OS admission unavailable')
    if args.check_incompatible_imports:
      for target in ('both_avx2', 'both_baseline'):
        result = compile_target(target, allowed=(0, 1))
        if result.returncode == 0 or b'configuration mismatch' not in result.stdout:
          raise RuntimeError(target + ': expected the retained Clang BMI configuration rejection')
    report['passed'] = True
  finally:
    report['source_pins_after'] = {name: sha((source / name).read_bytes()) for name in paths}
    report['source_unchanged'] = report['source_pins_after'] == pins
    report['passed'] = report['passed'] and report['source_unchanged']
    (output / 'report.json').write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
  return 0 if report['passed'] else 1


if __name__ == '__main__':
  raise SystemExit(main())
