# Tests

The default host build checks raw SIMD arithmetic, numerics, integers, masks,
memory tails, modules and per-profile identity. Use a C++26 compiler with
structured-binding packs, CMake 4.4 or newer, and Ninja. Select an ISA already
admitted on the host. In an initialized Windows Clang toolchain environment:

```sh
cmake -S . -B build/test-avx2 -G Ninja -DCMAKE_CXX_COMPILER=clang-cl -DCMAKE_BUILD_TYPE=Release -DSIMD_TEST_ISA=AVX2 -DSIMD_BUILD_TESTS=ON
cmake --build build/test-avx2 --parallel 2
ctest --test-dir build/test-avx2 --output-on-failure
```

Use separate build directories for each ISA, exception setting and sanitizer
configuration. Numerical fixtures set and restore their own floating-point
controls; raw operations retain the active environment's semantics. Use the
host's resource gate when builds or native tests share a machine with other work.

Focused standalone projects document their commands in adjacent READMEs.
The optional [AVX512 FP16 fixture](avx512_fp16/README.md) checks baseline admission,
native code generation and relocated mixed-profile BMIs, even on hosts where
the native execution test must report an unsupported-profile skip.
Generated packets, compiler output and machine reports belong in build
directories. A passing CPU test does not establish GPU behavior or measured
performance.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
