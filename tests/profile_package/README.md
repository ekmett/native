# Installed profile package check

This standalone consumer finds the installed package and builds importing
objects against the common provider and each explicit raw profile. The baseline
dispatcher links the archive without ISA imports or IPO. Type checks require
distinct profile identities, shared wide storage and the AVX2 mask topology
even in a broader AVX512 importing translation unit.

```powershell
cmake -S tests/profile_package -B build/profile-consumer -G Ninja -DCMAKE_CXX_COMPILER=C:/path/to/clang-cl.exe -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/relocated simd package"
cmake --build build/profile-consumer --parallel 2
ctest --test-dir build/profile-consumer --output-on-failure
```

Build/install the producer first with AVX2 and AVX512 providers and the same
compiler and exception mode. Moving the prefix to a path containing spaces also
exercises relocation. The consumer uses the adjacent test-only
`../core_regression/support/fp_environment.h`; copy that fixture too when moving
the consumer sources. No production header is borrowed from the source checkout.

Run through the host resource gate. The dispatcher admits CPU/OS capabilities
before checking raw FMA outputs, wide exp and cross-translation-unit static
strings. Unsupported hosts skip with status 77. This is a package smoke test,
not exhaustive accuracy, codegen or performance qualification.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
