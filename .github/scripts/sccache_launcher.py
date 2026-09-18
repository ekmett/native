#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Expand a conservative subset of CMake's Clang module maps before sccache.

This is deliberately not a general response-file parser. Unknown response or
PCH syntax runs the original compiler invocation without caching. Explicit PCH
binary inputs are hashed through SCCACHE_EXTRAFILES. CMake's files are never
rewritten. Unsupported POSIX compiler names bypass caching directly; Windows
retains its original arguments and direct sccache routing.
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


def pch_inputs(arguments):
    """Return explicit PCH inputs, or None when their dependencies are opaque.

    sccache 0.16 hashes module files but treats -include-pch only as a
    preprocessor argument. Hash the binary too: a cached BMI embeds its PCH's
    identity even when the preprocessed source is unchanged.
    """
    result = []
    index = 1
    while index < len(arguments):
        arg = arguments[index]
        if arg.startswith('@') or arg.startswith('-Xclang=@'):
            return None
        if arg == '-include-pch':
            forwarded = index > 1 and arguments[index - 1] == '-Xclang'
            index += 1
            if forwarded:
                if index >= len(arguments) or arguments[index] != '-Xclang':
                    return None
                index += 1
            if index >= len(arguments):
                return None
            path = arguments[index]
            if not path or path.startswith(('-', '@')) or os.pathsep in path:
                return None
            if not Path(path).is_file():
                return None
            result.append(str(Path(path).absolute()))
        elif 'include-pch' in arg or 'include-pth' in arg:
            return None
        index += 1
    return result


def main(arguments):
    if not arguments:
        print('usage: sccache_launcher.py COMPILER [ARGUMENT ...]', file=sys.stderr)
        return 2
    # An alias may still resolve to Clang, but its PCH syntax has not been
    # checked. Never send it through the cache without dependency hashing.
    if os.name != 'nt' and not COMPILER.fullmatch(Path(arguments[0]).name):
        os.execvp(arguments[0], arguments)
        return 0
    original = ['sccache', *arguments]
    normalized = normalize(arguments)
    if os.name != 'nt':
        inputs = pch_inputs(normalized)
        if inputs is None:
            os.execvp(arguments[0], arguments)
            return 0
        if inputs:
            previous = os.environ.get('SCCACHE_EXTRAFILES')
            os.environ['SCCACHE_EXTRAFILES'] = os.pathsep.join(
                ([previous] if previous else []) + inputs)
    command = ['sccache', *normalized]
    try:
        os.execvp(command[0], command)
    except OSError as error:
        if error.errno != errno.E2BIG or command == original:
            raise
        # The unexpanded invocation may hide PCH dependencies. Keep the
        # original compiler arguments, but bypass the cache on this retry.
        os.execvp(arguments[0], arguments)


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
