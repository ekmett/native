# Tests

The default host suite is a smoke test: ordinary SIMD and `wide` operations,
math, masks, memory tails, and a few known results per instruction family.
It uses the documented API examples and one instruction executable. Optional
instructions are checked at compile time and run only when the CPU and OS
admit them.

Use Clang 23, CMake 4.4 or newer, and Ninja. Choose a profile the host supports:

```sh
cmake -S . -B build/test -G Ninja -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release -DNATIVE_TEST_ISA=AVX2 -DNATIVE_BUILD_TESTS=ON
cmake --build build/test --parallel
ctest --test-dir build/test --parallel --output-on-failure
```

On ARM use `NEON`; on Windows use `clang-cl` in an initialized toolchain
shell. `NATIVE_TEST_EXTENDED=ON` adds the exhaustive numerical checks,
code-generation comparisons and compiler-rejection tests. These run nightly
on Linux ARM64 and x86-64, or locally when the change warrants them. CTest
serializes tests that deliberately rebuild invalid targets in the same tree.

Installed-package validation uses one consumer graph for public headers,
the omnibus and API examples. Move the install prefix before configuring it:

```sh
cmake --install build/test --prefix build/install
cmake -E rename build/install 'build/relocated package'
cmake -S tests/package -B build/package -G Ninja -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release -Dnative_DIR="$PWD/build/relocated package/lib/cmake/native"
cmake --build build/package --parallel
ctest --test-dir build/package --parallel --output-on-failure
```

Each required module is built once for these consumers. The tests check that
core imports share a BMI. Keep build output outside the source tree.

See [validation](validation.md) for CI cadence and the contracts checked by
the extended suite. Unsupported hardware is reported; a compiled or skipped
operation is not evidence of its runtime behavior.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
