# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Reuse actual CMake-produced BMIs without letting CMake rebuild their defaults."""
import argparse
import ctypes
import json
import os
from pathlib import Path
import re
import shlex
import subprocess

p = argparse.ArgumentParser()
p.add_argument('--build', required=True, type=Path)
p.add_argument('--output', required=True, type=Path)
p.add_argument('--config', default='')
a = p.parse_args()
a.output.mkdir(parents=True, exist_ok=True)
commands = json.loads((a.build / 'compile_commands.json').read_text(encoding='utf-8'))
source = Path(__file__).with_name('availability.cc').resolve()

def split(command):
    if os.name != 'nt':
        return shlex.split(command)
    # CMake emits Windows command-line quoting for clang-cl. Preserve backslashes
    # and quoted SDK paths rather than parsing them as POSIX shell escapes.
    argc = ctypes.c_int()
    parse = ctypes.windll.shell32.CommandLineToArgvW
    parse.argtypes = [ctypes.c_wchar_p, ctypes.POINTER(ctypes.c_int)]
    parse.restype = ctypes.POINTER(ctypes.c_wchar_p)
    argv = parse(command, ctypes.byref(argc))
    if not argv:
        raise ctypes.WinError()
    try:
        return [argv[i] for i in range(argc.value)]
    finally:
        ctypes.windll.kernel32.LocalFree(ctypes.cast(argv, ctypes.c_void_p))

records = []
for present in (0, 1):
    candidates = [c for c in commands if Path(c['file']).resolve() == source
                  and f'NATIVE_ARM_DEFAULT_PRESENT={present}' in c.get('command', ' '.join(c.get('arguments', [])))]
    if a.config and len(candidates) > 1:
        candidates = [c for c in candidates if a.config in c['output']]
    if len(candidates) != 1:
        raise SystemExit(f'Expected one baseline-{present} compilation, found {len(candidates)}')
    c = candidates[0]
    argv = c.get('arguments') or split(c['command'])
    cl = Path(argv[0]).stem.lower() == 'clang-cl'
    syntax = []
    skip = False
    for arg in argv:
        if skip:
            skip = False
            continue
        if arg in ('-o', '-MF', '-MT', '-MQ'):
            skip = True
        elif arg in ('-c', '/c', '-MD', '-MMD') or arg.startswith(('/Fo', '/Fd')):
            continue
        else:
            syntax.append(arg)
    # CMake's clang-cl command ends its options with -- before the source path.
    # Keep our extra option ahead of that marker so it is not treated as a file.
    syntax.insert(1, '/clang:-fsyntax-only' if cl else '-fsyntax-only')
    for name, arch, succeeds in (
        ('same-importer', None, True),
        ('stronger-importer', 'armv8.1-a+nolse+crc+aes+sha2+rdm+dotprod', None),
        ('weaker-importer', 'armv8-a+nocrc+noaes+nosha2+nordm', None if present else True),
    ):
        args = [('/clang:' if arg.startswith('/clang:') else '') + '-march=' + arch
                if arch and arg.startswith(('-march=', '/clang:-march=')) else arg for arg in syntax]
        # Existing @...modmap arguments keep the exact providers from the build.
        result = subprocess.run(args, cwd=c['directory'], text=True, capture_output=True)
        label = f'baseline-{present}-{name}'
        output = result.stdout + result.stderr
        (a.output / (label + '.log')).write_text(output, encoding='utf-8')
        records.append(dict(name=label, command=args, cwd=c['directory'], exit_code=result.returncode))
        if succeeds and result.returncode:
            raise SystemExit(label + '\n' + output)
        # Clang may reject a target-incompatible BMI before name lookup. When
        # accepted, the original source assertions must still see provider ISA.
        if result.returncode and not re.search(r'configuration mismatch', output):
            raise SystemExit(label + ': expected compiler feature/configuration rejection\n' + output)
(a.output / 'receipt.json').write_text(json.dumps(records, indent=2), encoding='utf-8')
print('Frozen absent/present providers pass unchanged; altered importers retain provider defaults or diagnose incompatible BMIs.')
