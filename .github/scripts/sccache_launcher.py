#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Expand a conservative subset of CMake's Clang module maps before sccache.

This is deliberately not a general response-file parser. Unknown syntax keeps
all original arguments, allowing sccache to bypass caching as usual. CMake's
files are never rewritten. Windows/clang-cl retain their original arguments.
"""
import errno
import os
from pathlib import Path
import re
import sys


MAX_BYTES = 64 * 1024
MODULE_PATH = r'[A-Za-z0-9_./@:+,=\-]+'
MODULE_NAME = r'[A-Za-z_][A-Za-z0-9_.:]*'
OUTPUT = re.compile(r'-fmodule-output="(' + MODULE_PATH + r')"')
IMPORT = re.compile(r'-fmodule-file="(' + MODULE_NAME + '=' + MODULE_PATH + r')"')
COMPILER = re.compile(r'clang(?:\+\+)?(?:-[0-9]+)?')


def parse_modmap(contents):
    """Return argv for the exact supported grammar, or None for safe bypass.

    Each line is -x c++-module or a module flag with one nonempty quoted value.
    Only the explicit ASCII allowlist above is accepted inside the quotes.
    Spaces, tabs, escapes, single quotes, nested response files, concatenated
    quoted fragments and malformed quoting are therefore never normalized.
    LF and CRLF delimit lines; an empty map expands to no arguments.
    """
    if not contents:
        return []
    lines = contents.split('\n')
    if lines[-1] == '':
        lines.pop()
    result = []
    for line in lines:
        if line.endswith('\r'):
            line = line[:-1]
        if line == '-x c++-module':
            result.extend(['-x', 'c++-module'])
            continue
        match = OUTPUT.fullmatch(line)
        if match:
            result.append('-fmodule-output=' + match[1])
            continue
        match = IMPORT.fullmatch(line)
        if match:
            result.append('-fmodule-file=' + match[1])
            continue
        return None
    return result


def argv_fits(arguments):
    # Stay well below common exec limits, accounting for environment, pointers
    # and terminators too. exec can still reject an oversized argument/env;
    # main then retries the original response-file invocation on E2BIG only.
    size = sum(len(os.fsencode(arg)) + 1 for arg in arguments)
    if size > MAX_BYTES:
        return False
    environment = sum(len(os.fsencode(k)) + len(os.fsencode(v)) + 2
                      for k, v in os.environ.items())
    try:
        limit = os.sysconf('SC_ARG_MAX')
    except (ValueError, OSError):
        return False
    return size + environment + 8 * (len(arguments) + len(os.environ)) + 8192 < limit


def normalize(arguments):
    """Normalize an entire compiler invocation or preserve it unchanged."""
    if (os.name == 'nt' or not arguments or
            not COMPILER.fullmatch(Path(arguments[0]).name)):
        return arguments
    result = [arguments[0]]
    for argument in arguments[1:]:
        if not argument.startswith('@'):
            result.append(argument)
            continue
        path = Path(argument[1:])
        if path.suffix != '.modmap':
            return arguments
        try:
            with path.open('rb') as stream:
                contents = stream.read(MAX_BYTES + 1)
            if len(contents) > MAX_BYTES:
                return arguments
            expanded = parse_modmap(contents.decode('ascii'))
        except (OSError, UnicodeError, ValueError):
            return arguments
        if expanded is None:
            return arguments
        result.extend(expanded)
    return result if argv_fits(['sccache', *result]) else arguments


def main(arguments):
    if not arguments:
        print('usage: sccache_launcher.py COMPILER [ARGUMENT ...]', file=sys.stderr)
        return 2
    original = ['sccache', *arguments]
    command = ['sccache', *normalize(arguments)]
    try:
        os.execvp(command[0], command)
    except OSError as error:
        if error.errno != errno.E2BIG or command == original:
            raise
        os.execvp(original[0], original)


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
