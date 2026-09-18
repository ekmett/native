#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Semantic and fail-closed controls for the optional CI compiler launcher."""
import errno
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

import sccache_launcher as launcher


class ParserTests(unittest.TestCase):
    def test_generated_producer_and_import_map(self):
        source = ('-x c++-module\n-fmodule-output="src/foo.pcm"\n'
                  '-fmodule-file="simd.wide=src/common@synth_0/a.bmi"\n'
                  '-fmodule-file="simd.part:detail=../module.pcm"\n')
        expected = ['-x', 'c++-module', '-fmodule-output=src/foo.pcm',
                    '-fmodule-file=simd.wide=src/common@synth_0/a.bmi',
                    '-fmodule-file=simd.part:detail=../module.pcm']
        self.assertEqual(launcher.parse_modmap(source), expected)
        self.assertEqual(launcher.parse_modmap(source.replace('\n', '\r\n')), expected)
        self.assertEqual(launcher.parse_modmap(source.rstrip('\n')), expected)
        self.assertEqual(launcher.parse_modmap(''), [])

    def test_unproven_syntax_is_not_transformed(self):
        cases = ['""', "''", '-x "c++-module"', '@nested.modmap', '-x\tc++-module',
                 '-fmodule-output=""', '-fmodule-output="foo""bar"',
                 '-fmodule-output="foo"tail', '-fmodule-output=foo',
                 '-fmodule-output="unterminated', '-fmodule-output=foo"',
                 '-fmodule-output="foo" -DVALUE=1', '-fmodule-file="foo.pcm"',
                 '-fmodule-file="=foo.pcm"', '-fmodule-file="foo="',
                 '-fmodule-output="foo"\n@nested.modmap', '\n',
                 '-fmodule-output="foo"\r-x c++-module',
                 '-fmodule-output="foo"\n\n']
        for separator in [' ', '\t', '\r', '\n', '\v', '\f', '\u00a0']:
            cases.append(f'-fmodule-output="foo{separator}bar"')
        for unsafe in ["'", '\\', '"', '\0', 'é', '$', '`']:
            cases.append(f'-fmodule-output="foo{unsafe}bar"')
        for source in cases:
            with self.subTest(source=source):
                self.assertIsNone(launcher.parse_modmap(source))


class LauncherTests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name)
        # The response filename itself can contain a space: argv already keeps it whole.
        self.path = self.root / 'with space.modmap'
        self.path.write_text('-fmodule-file="simd=src/simd.pcm"\n')
        self.arguments = ['/toolchain/clang++', '-c', 'source with space.cc',
                          '@' + str(self.path), '-o', 'output with space.o']

    def test_expansion_preserves_other_argv_and_file(self):
        before = self.path.read_bytes()
        result = launcher.normalize(self.arguments)
        self.assertEqual(result, [*self.arguments[:3], '-fmodule-file=simd=src/simd.pcm',
                                  *self.arguments[4:]])
        self.assertEqual(self.path.read_bytes(), before)

    def test_any_unknown_response_keeps_entire_invocation(self):
        for suffix in ['.rsp', '.missing.modmap']:
            arguments = [*self.arguments, '@' + str(self.root / ('other' + suffix))]
            self.assertEqual(launcher.normalize(arguments), arguments)
        self.path.write_bytes(b'-fmodule-output="bad\xffpath"')
        self.assertEqual(launcher.normalize(self.arguments), self.arguments)
        self.path.write_text('-fmodule-output="path with space.pcm"')
        self.assertEqual(launcher.normalize(self.arguments), self.arguments)

    def test_platform_compiler_and_size_bypasses(self):
        for compiler in ['clang-cl', 'clang-cl.exe', 'g++', 'ccache', 'not-clang++']:
            arguments = [compiler, *self.arguments[1:]]
            self.assertEqual(launcher.normalize(arguments), arguments)
        with patch.object(launcher.os, 'name', 'nt'):
            self.assertEqual(launcher.normalize(self.arguments), self.arguments)
        with patch.object(launcher, 'argv_fits', return_value=False):
            self.assertEqual(launcher.normalize(self.arguments), self.arguments)
        self.path.write_bytes(b'a' * (launcher.MAX_BYTES + 1))
        self.assertEqual(launcher.normalize(self.arguments), self.arguments)
        self.assertFalse(launcher.argv_fits(['a' * (launcher.MAX_BYTES + 1)]))

    def test_exec_retries_original_only_for_argument_size(self):
        with patch.object(launcher.os, 'execvp', side_effect=[OSError(errno.E2BIG, 'long'), None]) as execute:
            launcher.main(self.arguments)
            self.assertEqual(execute.call_count, 2)
            self.assertEqual(execute.call_args_list[1].args,
                             ('sccache', ['sccache', *self.arguments]))
        for error in [errno.ENOENT, errno.EACCES]:
            with patch.object(launcher.os, 'execvp', side_effect=OSError(error, 'error')) as execute:
                with self.assertRaises(OSError):
                    launcher.main(self.arguments)
                self.assertEqual(execute.call_count, 1)

    @unittest.skipIf(os.name == 'nt', 'POSIX exec and GNU-style Clang only')
    def test_real_launcher_forwards_exit_output_and_arguments(self):
        fake = self.root / 'sccache'
        fake.write_text('#!' + sys.executable + '\nimport json, sys\n'
                        'print(json.dumps(sys.argv[1:]))\n'
                        'print("compiler diagnostic", file=sys.stderr)\nsys.exit(19)\n')
        fake.chmod(0o755)
        environment = {**os.environ, 'PATH': str(self.root) + os.pathsep + os.environ['PATH']}
        result = subprocess.run([sys.executable, launcher.__file__, *self.arguments],
                                env=environment, capture_output=True, text=True)
        self.assertEqual(result.returncode, 19)
        self.assertEqual(result.stderr, 'compiler diagnostic\n')
        self.assertEqual(json.loads(result.stdout), launcher.normalize(self.arguments))


if __name__ == '__main__':
    unittest.main()
