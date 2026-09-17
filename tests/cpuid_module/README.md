# CPUID and wait modules

The x86-only `simd.cpuid` and `simd.wait` module definitions belong to
`simd::common`, which receives no SIMD profile flags. All intrinsic and system
headers are in the global module fragment; the public types, templates and
function definitions are below the module declaration. There is no remaining
CPUID or wait implementation header.

`cpuid(leaf, subleaf)` preserves raw native register results, including the
hardware behavior for unsupported leaves. It is deliberately not marked pure:
per-logical-CPU results can change across migration. This API makes no additional
C++ memory-ordering guarantee. `cpu_vendor` remains cached vendor
identification. Wait admission checks maximum basic/extended leaves before
reading MWAITX/MONITORX (extended ECX29) or WAITPKG (leaf7 ECX5), without assuming
features from a vendor name. These bit definitions are documented in the
[AMD family15h BKDG](https://www.amd.com/content/dam/amd/en/documents/archived-tech-docs/programmer-references/55072_AMD_Family_15h_Models_70h-7Fh_BKDG.pdf)
and [Intel SDM](https://cdrdv2-public.intel.com/868137/325462-089-sdm-vol-1-2abcd-3abcd-4.pdf).

Optional instruction methods are ordinary module-owned definitions with target
attributes. Baseline callers cross a function boundary. Generic wait adapters
retain the exception specifications of their callback expressions.

The standalone CMake fixture builds the real `simd::common` provider and imports
both modules without implementation includes. It compares CPUID registers and
vendor to direct native CPUID, checks feature admission, and runs the maintained
`noexcept_wait` control using mock waiters and callbacks. The four optional wait
instruction probes are compiled and disassembled only. No MONITORX, MWAITX,
UMONITOR or UMWAIT instruction is executed by a test, regardless of CPUID bits.

With the initialized compiler environment and the established CPU build gate:

```powershell
cmake -S tests/cpuid_module -B build/cpuid-check -G Ninja -DCMAKE_CXX_COMPILER=clang-cl -DCMAKE_BUILD_TYPE=Release -DSIMD_ENABLE_EXCEPTIONS=OFF
cmake --build build/cpuid-check --parallel 1
ctest --test-dir build/cpuid-check --output-on-failure
```

Repeat in a separate build directory with exceptions ON to exercise actual
exception propagation. The fixture disables PCH and IPO. It does not execute
optional wait instructions or measure wait latency.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
