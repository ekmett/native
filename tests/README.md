# Tests

The default host build checks raw SIMD arithmetic, numerics, integers, masks,
memory tails, modules and per-profile identity. Use a C++26 compiler with
structured-binding packs, CMake 4.4 or newer, and Ninja. Select an ISA already
admitted on the host. In an initialized Windows Clang toolchain environment:

```sh
cmake -S . -B build/test-avx2 -G Ninja -DCMAKE_CXX_COMPILER=clang-cl -DCMAKE_BUILD_TYPE=Release -DNATIVE_TEST_ISA=AVX2 -DNATIVE_BUILD_TESTS=ON
cmake --build build/test-avx2 --parallel
ctest --test-dir build/test-avx2 --output-on-failure
```

One build can exercise several profiles through `NATIVE_PROFILES`; they share
the hub BMI. Use separate build directories when changing the primary test ISA,
exception setting or sanitizer configuration. Numerical fixtures set and restore
their own floating-point controls; raw operations retain the active environment's
semantics.

Focused standalone projects document their commands in adjacent READMEs.
The optional [AVX512 FP16 fixture](avx512_fp16/README.md) checks baseline admission,
native code generation and relocated consumers sharing one baseline hub BMI,
even on hosts where native execution reports an unsupported-profile skip.
The [instruction storage checks](instruction_storage/README.md) cover the
representation and memory boundaries of shapes used by instruction extensions.
Each instruction family also checks its public SIMD calls against the matching
raw instruction sequence. Runtime checks report a skip when the host lacks the
required feature; compiling an operation does not establish hardware behavior.

Generated packets, compiler output and machine reports belong in build
directories. Correctness and code-generation checks do not establish measured
performance.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
