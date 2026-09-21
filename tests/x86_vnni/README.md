<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
# VNNI integer dot-product fixture

The header, granular module, `native.x86` and `native` consumers compare each
admitted VNNI family with independent 64-bit scalar arithmetic. Directed
signedness boundaries, accumulator extremes, cancellation, lane impulses,
random inputs and complete mask sweeps cover wrapping and saturation. The
unsigned-by-unsigned saturating forms use unsigned accumulators and saturation.
Each unsupported runtime family prints its admission reason and returns 77.

Baseline checks inspect the actual importer objects, including `main`, so
they retain symbol names even on platforms whose linked executables omit them.
Textual code-generation probes check all 76 instruction encodings, widths and
mask modes. Separate compiler rejections cover feature requirements, caller
targets and implicit register conversions. Granular-module rejections repeat
these three checks for each instruction family, including EVEX mask forms, to
verify the exported constraints and target attributes. Module consumers retain
the provider's baseline compiler flags.

Build with Clang 23, CMake 4.4, Ninja, libc++ and IPO disabled, then run:

```sh
cmake --build build/core --parallel
ctest --test-dir build/core -R native.x86.vnni --output-on-failure
```

For an installed package, the directory also configures independently:

```sh
cmake -S tests/x86_vnni -B build/consumer -G Ninja \
  -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_FLAGS=-stdlib=libc++ \
  -DCMAKE_BUILD_TYPE=Release -Dnative_DIR='/relocated prefix/lib/cmake/native'
cmake --build build/consumer --parallel
ctest --test-dir build/consumer --output-on-failure
```

That mode builds the three public module consumers and inspects their objects.
Move the whole installation before configuring a fresh consumer to test physical
relocation. Compare installed file hashes before and after the consumer build;
provider BMIs should be generated in the consumer build directory.
