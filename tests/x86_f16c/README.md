<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
# F16C fixture

The normal baseline build runs header, granular module, x86 umbrella and main
hub consumers. Every runtime entry checks F16C/AVX CPU support and XCR0 before
calling targeted code. Missing support returns CTest skip code 77. MXCSR is
saved and restored, including incoming exception masks and status bits.

```sh
cmake --build build/core --parallel
ctest --test-dir build/core -R '^native.x86.f16c\.' --output-on-failure
```

The same directory is a standalone installed-package consumer:

```sh
cmake -S tests/x86_f16c -B build/f16c-installed -G Ninja \
  -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_FLAGS=-stdlib=libc++ -DCMAKE_PREFIX_PATH=/path/to/native
cmake --build build/f16c-installed --parallel
ctest --test-dir build/f16c-installed --output-on-failure
```

Standalone consumers import installed modules and use public headers only.
Consumer targets retain the module provider's baseline options. Disabled-feature
codegen, compiler-minimum metadata and compile-failure tests use isolated
textual/object probes in the source build. GNU and clang-cl option spellings are
selected by CMake's compiler frontend variant.

Thirteen additional compile-failure checks import `native.x86.f16c` with the
provider's baseline options. They verify that all six call forms still reject
an AVX-only caller, packed arguments still require their exact register type,
and a missing F16C feature is rejected across the module boundary.

The integer oracle never uses a native half conversion. Widening tests every
16-bit encoding in scalar, four-lane and eight-lane forms. Narrowing covers the
neighbors of every finite half value and every midpoint, both signs, 32,768
random binary32 encodings per environment, all rounding modes, all immediate
bytes, and all FTZ/DAZ combinations. Linux additionally checks 27 unmasked
exceptions and their saved MXCSR flags in child processes. Other platforms
retain the masked-status and discarded-result checks.
