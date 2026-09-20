# Installed profile package check

This standalone consumer finds the installed package and builds importing
objects against the shared hub, with explicit AVX2 and AVX-512 kernel targets.
The baseline dispatcher links the hub and common archives without importing the
hub or enabling IPO and uses the configured common minimum (the toolchain
baseline by default). Type checks require
distinct profile identities, shared wide storage and the AVX2 mask topology
even in a broader AVX512 importing translation unit.

```powershell
cmake -S tests/profile_package -B build/profile-consumer -G Ninja -DCMAKE_CXX_COMPILER=C:/path/to/clang-cl.exe -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/relocated simd package"
cmake --build build/profile-consumer --parallel 2
ctest --test-dir build/profile-consumer --output-on-failure
```

Build/install the producer first with the same
compiler and exception mode. Moving the prefix to a path containing spaces also
exercises relocation. The consumer uses the adjacent test-only
`../core_regression/support/fp_environment.h`; copy that fixture too when moving
the consumer sources. No production header is borrowed from the source checkout.

Run through the host resource gate. The dispatcher admits CPU/OS capabilities
before checking raw FMA outputs, wide exp and cross-translation-unit static
strings. The mixed-profile runtime requires both AVX2 and AVX-512; unsupported hosts
skip with status 77. The dispatcher uses the platform CPUID/XGETBV spellings on
Windows and Linux while retaining the configured minimal compiler flags.
That minimum is a prerequisite for starting the executable; admission does
not make it safe on weaker CPUs. Linux AVX2 execution
is covered separately by the root tests and the AVX2-only
[hub consumer](../omnibus/README.md). This is a package smoke test,
not exhaustive accuracy, codegen or performance qualification.

The CMake profile target names used here are compatibility aliases for the hub.
The build-graph check requires one provider per imported module and rejects
the former per-ISA BMIs.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
