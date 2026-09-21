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
from types import SimpleNamespace
import unittest
from unittest.mock import patch

import sccache_launcher as launcher


class ParserTests(unittest.TestCase):
    def test_generated_producer_and_import_map(self):
        source = ('-x c++-module\n-fmodule-output="src/foo.pcm"\n'
                  '-fmodule-file="native.wide=src/common@synth_0/a.bmi"\n'
                  '-fmodule-file="native.part:detail=../module.pcm"\n')
        expected = ['-x', 'c++-module', '-fmodule-output=src/foo.pcm',
                    '-fmodule-file=native.wide=src/common@synth_0/a.bmi',
                    '-fmodule-file=native.part:detail=../module.pcm']
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


class PosixLauncherTests(unittest.TestCase):
    def setUp(self):
        # Exercise POSIX routing on any host without changing pathlib's platform.
        platform = SimpleNamespace(**vars(os))
        platform.name = 'posix'
        if not hasattr(platform, 'sysconf'):
            platform.sysconf = lambda name: 2 * 1024 * 1024
        patcher = patch.object(launcher, 'os', platform)
        patcher.start()
        self.addCleanup(patcher.stop)


class LauncherTests(PosixLauncherTests):
    def setUp(self):
        super().setUp()
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name)
        # The response filename itself can contain a space: argv already keeps it whole.
        self.path = self.root / 'with space.modmap'
        self.path.write_text('-fmodule-file="native=src/native.pcm"\n')
        self.arguments = ['/toolchain/clang++', '-c', 'source with space.cc',
                          '@' + str(self.path), '-o', 'output with space.o']

    def test_expansion_preserves_other_argv_and_file(self):
        before = self.path.read_bytes()
        result = launcher.normalize(self.arguments)
        self.assertEqual(result, [*self.arguments[:3], '-fmodule-file=native=src/native.pcm',
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

    def test_unknown_compilers_execute_original_argv_without_cache(self):
        pch = self.root / 'with space.pch'
        pch.write_bytes(b'PCH binary')
        for compiler in ['c++', '/toolchain/c++', 'aarch64-linux-gnu-clang++',
                         '/toolchain/x86_64-linux-gnu-clang++-23', 'clang-cl',
                         'clang-cl.exe', 'g++', 'ccache', 'not-clang++']:
            for flags in [[], ['-include-pch', str(pch)]]:
                arguments = [compiler, *self.arguments[1:], *flags]
                with self.subTest(compiler=compiler, flags=flags):
                    with patch.dict(os.environ, {'SCCACHE_EXTRAFILES': '/existing/file'}):
                        with patch.object(launcher.os, 'execvp') as execute:
                            launcher.main(arguments)
                            execute.assert_called_once_with(compiler, arguments)
                            self.assertEqual(os.environ['SCCACHE_EXTRAFILES'], '/existing/file')

    def test_unchanged_invocation_does_not_retry_e2big(self):
        for compiler in ['clang++', 'c++']:
            arguments = [compiler, '-c', 'source with space.cc']
            with self.subTest(compiler=compiler):
                with patch.object(launcher.os, 'execvp', side_effect=OSError(errno.E2BIG, 'long')) as execute:
                    with self.assertRaises(OSError):
                        launcher.main(arguments)
                    self.assertEqual(execute.call_count, 1)

    def test_exec_retries_original_only_for_argument_size(self):
        with patch.object(launcher.os, 'execvp', side_effect=[OSError(errno.E2BIG, 'long'), None]) as execute:
            launcher.main(self.arguments)
            self.assertEqual(execute.call_count, 2)
            self.assertEqual(execute.call_args_list[1].args,
                             (self.arguments[0], self.arguments))
        for error in [errno.ENOENT, errno.EACCES]:
            with patch.object(launcher.os, 'execvp', side_effect=OSError(error, 'error')) as execute:
                with self.assertRaises(OSError):
                    launcher.main(self.arguments)
                self.assertEqual(execute.call_count, 1)

    @unittest.skipIf(os.name == 'nt', 'POSIX executable fixture')
    def test_real_alias_bypasses_cache_and_preserves_argv_and_environment(self):
        compiler = self.root / 'c++'
        compiler.write_text('#!' + sys.executable + '\nimport json, os, sys\n'
                            'print(json.dumps([sys.argv[1:], os.environ.get("SCCACHE_EXTRAFILES")]))\n'
                            'print("alias diagnostic", file=sys.stderr)\nsys.exit(19)\n')
        compiler.chmod(0o755)
        cache = self.root / 'sccache'
        cache.write_text('#!' + sys.executable + '\nimport sys\n'
                         'print("unexpected cache invocation", file=sys.stderr)\nsys.exit(99)\n')
        cache.chmod(0o755)
        pch = self.root / 'with space.pch'
        pch.write_bytes(b'PCH binary')
        arguments = [str(compiler), *self.arguments[1:], '-include-pch', str(pch)]
        environment = {**os.environ,
                       'PATH': str(self.root) + os.pathsep + os.environ['PATH'],
                       'SCCACHE_EXTRAFILES': '/existing/file'}
        result = subprocess.run([sys.executable, launcher.__file__, *arguments],
                                env=environment, capture_output=True, text=True)
        self.assertEqual(result.returncode, 19)
        self.assertEqual(result.stderr, 'alias diagnostic\n')
        self.assertEqual(json.loads(result.stdout), [arguments[1:], '/existing/file'])

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


class WindowsLauncherTests(unittest.TestCase):
    def setUp(self):
        platform = SimpleNamespace(**vars(os))
        platform.name = 'nt'
        patcher = patch.object(launcher, 'os', platform)
        patcher.start()
        self.addCleanup(patcher.stop)
        self.arguments = [r'C:\LLVM tools\bin\clang-cl.exe', '/nologo', '/c',
                          'source with space.cc', '/Fooutput with space.obj',
                          '/O2', '/fp:strict', '-std:c++latest']

    def check_route(self, arguments, cached):
        with patch.dict(os.environ, {'SCCACHE_EXTRAFILES': r'C:\existing file'}):
            with patch.object(launcher.subprocess, 'run',
                              return_value=SimpleNamespace(returncode=19)) as execute:
                self.assertEqual(launcher.main(arguments), 19)
                expected = ['sccache', *arguments] if cached else arguments
                # No output redirection: compiler output reaches the build tool.
                execute.assert_called_once_with(expected)
                self.assertEqual(os.environ['SCCACHE_EXTRAFILES'], r'C:\existing file')

    def test_ordinary_clang_cl_compilations_remain_cached(self):
        for compiler in [self.arguments[0], 'clang-cl', 'clang-cl-23.exe',
                         'C:/LLVM tools/bin/CLANG-CL.EXE']:
            with self.subTest(compiler=compiler):
                self.check_route([compiler, *self.arguments[1:]], True)

    def test_opaque_response_files_bypass_without_reading_or_expanding(self):
        for response in [r'@C:\module maps\main.cc.obj.modmap', '@missing.modmap',
                         '@unknown.rsp', '@"with spaces.rsp"',
                         r'/clang:@C:\module maps\nested.rsp',
                         '-clang:@nested.modmap', '-Xclang=@nested.rsp']:
            with self.subTest(response=response):
                with patch.object(launcher, 'Path', side_effect=AssertionError('unexpected read')):
                    self.check_route([*self.arguments, response], False)

    def test_raw_and_forwarded_module_flags_bypass(self):
        flags = ['-fmodule-file=native.isa=C:/module files/native.isa.pcm',
                 '-fmodule-output=output.pcm', '-fprebuilt-module-path=C:/modules',
                 '-fmodule-map-file=module.modulemap', '-fmodules',
                 '-fimplicit-modules', '-fno-implicit-modules',
                 '-emit-module-interface', '--precompile']
        for flag in flags:
            for forwarded in [[flag], ['/clang:' + flag], ['-clang:' + flag],
                              ['-Xclang', flag], ['-Xclang=' + flag]]:
                with self.subTest(flags=forwarded):
                    self.check_route([*self.arguments, *forwarded], False)
        for flags in [['-x', 'c++-module'], ['/clang:-x', '/clang:c++-module'],
                      ['-xc++-module'], ['/interface'], ['/internalPartition'],
                      ['/ifcOutput', 'module.ifc'], ['/reference', 'native=module.ifc'],
                      ['/headerUnit:quote', 'header=module.ifc'], ['/exportHeader'],
                      ['/stdIfcDir', 'modules'], ['/experimental:module']]:
            with self.subTest(flags=flags):
                self.check_route([*self.arguments, *flags], False)

    def test_pch_and_module_inputs_bypass(self):
        for flags in [['/FpC:/PCH files/prefix.pch'], ['/Yuprefix.h'], ['/Ycprefix.h'],
                      ['-include-pch', 'prefix.pch'], ['/clang:-include-pch', 'prefix.pch'],
                      ['-Xclang', '-include-pch', '-Xclang', 'prefix.pch'],
                      ['-include-pth', 'prefix.pth'], ['-x', 'c++-header']]:
            with self.subTest(flags=flags):
                self.check_route([*self.arguments, *flags], False)
        for suffix in ['ccm', 'cppm', 'cxxm', 'c++m', 'ixx', 'mpp', 'mxx',
                       'pcm', 'bmi', 'ifc', 'pch', 'pth']:
            with self.subTest(suffix=suffix):
                self.check_route([*self.arguments, 'source with space.' + suffix.upper()], False)

    def test_unknown_compilers_bypass(self):
        for compiler in ['cl.exe', 'clang++.exe', 'compiler-alias.exe']:
            self.check_route([compiler, *self.arguments[1:]], False)


class DirectExecutionTests(unittest.TestCase):
    def test_real_bypass_preserves_exit_output_arguments_and_environment(self):
        # Python is deliberately outside the supported compiler names. This
        # exercises the direct exec path on Windows as well as POSIX hosts.
        child = ('import json, os, sys; '
                 'print(json.dumps([sys.argv[1:], os.environ.get("SCCACHE_EXTRAFILES")])); '
                 'print("compiler diagnostic", file=sys.stderr); sys.exit(19)')
        arguments = ['with space', 'quoted"value', r'C:\module files\input.cc']
        environment = {**os.environ, 'SCCACHE_EXTRAFILES': 'existing value'}
        result = subprocess.run([sys.executable, launcher.__file__, sys.executable,
                                 '-c', child, *arguments], env=environment,
                                capture_output=True, text=True)
        self.assertEqual(result.returncode, 19)
        self.assertEqual(result.stderr, 'compiler diagnostic\n')
        self.assertEqual(json.loads(result.stdout), [arguments, 'existing value'])


class PchTests(PosixLauncherTests):
    def setUp(self):
        super().setUp()
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.pch = Path(self.directory.name) / 'with space.pch'
        self.pch.write_bytes(b'first binary')
        self.arguments = ['clang++', '-c', 'module.ccm']

    def test_explicit_and_forwarded_inputs(self):
        for flags in [['-include-pch', str(self.pch)],
                      ['-Xclang', '-include-pch', '-Xclang', str(self.pch)]]:
            self.assertEqual(launcher.pch_inputs([*self.arguments, *flags]),
                             [str(self.pch.absolute())])
        self.assertEqual(launcher.pch_inputs(self.arguments), [])

    def test_ambiguous_missing_and_opaque_inputs_bypass(self):
        for flags in [['-include-pch'], ['-include-pch', 'missing.pch'],
                      ['-include-pch=' + str(self.pch)],
                      ['-Xclang=-include-pch', str(self.pch)],
                      ['-Xclang', '-include-pch', str(self.pch)],
                      ['-include-pch', '-wrong'], ['@hidden.rsp'], ['-Xclang=@hidden.rsp'],
                      ['-include-pth', str(self.pch)],
                      ['-include-pch', 'ambiguous' + os.pathsep + 'path']]:
            with self.subTest(flags=flags):
                self.assertIsNone(launcher.pch_inputs([*self.arguments, *flags]))

    def test_main_preserves_extras_and_hashes_binary(self):
        for compiler in ['clang', 'clang++', 'clang-23', 'clang++-23', '/toolchain/clang++']:
            for previous in ['', '/existing/file']:
                arguments = [compiler, *self.arguments[1:], '-Xclang', '-include-pch',
                             '-Xclang', str(self.pch)]
                with self.subTest(compiler=compiler, previous=previous):
                    with patch.dict(os.environ, {'SCCACHE_EXTRAFILES': previous}):
                        with patch.object(launcher.os, 'execvp') as execute:
                            launcher.main(arguments)
                            expected = ([previous] if previous else []) + [str(self.pch.absolute())]
                            self.assertEqual(os.environ['SCCACHE_EXTRAFILES'], os.pathsep.join(expected))
                            execute.assert_called_once_with('sccache', ['sccache', *arguments])

    def test_e2big_keeps_original_response_and_pch_arguments(self):
        modmap = self.pch.with_suffix('.modmap')
        modmap.write_text('-fmodule-file="native=src/native.pcm"\n')
        arguments = [*self.arguments, '-include-pch', str(self.pch), '@' + str(modmap)]
        expected = ['sccache', *arguments[:-1], '-fmodule-file=native=src/native.pcm']
        with patch.dict(os.environ, {'SCCACHE_EXTRAFILES': '/existing/file'}):
            with patch.object(launcher.os, 'execvp', side_effect=[OSError(errno.E2BIG, 'long'), None]) as execute:
                launcher.main(arguments)
                self.assertEqual(execute.call_args_list[0].args, ('sccache', expected))
                self.assertEqual(execute.call_args_list[1].args, (arguments[0], arguments))
                self.assertEqual(execute.call_count, 2)
                self.assertEqual(os.environ['SCCACHE_EXTRAFILES'],
                                 '/existing/file' + os.pathsep + str(self.pch.absolute()))

    def test_opaque_input_executes_original_compiler(self):
        for flags in [['@unknown.rsp'], ['-Xclang=@hidden.rsp'], ['-include-pch', 'missing.pch']]:
            arguments = [*self.arguments, *flags]
            with patch.object(launcher.os, 'execvp') as execute:
                launcher.main(arguments)
                execute.assert_called_once_with('clang++', arguments)


if __name__ == '__main__':
    unittest.main()
