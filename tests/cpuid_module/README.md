# CPUID and wait modules

The x86-only `native.x86.features` and `native.x86.wait` module definitions belong to
`native::minimal` (also named `native::common`), whose minimum defaults to the
toolchain baseline and is configurable at project setup. All intrinsic and system
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

The standalone CMake fixture builds the real `native::common` provider and imports
both modules without implementation includes. It compares CPUID registers and
vendor to direct native CPUID, checks feature admission, and runs the maintained
`noexcept_wait` control using mock waiters and callbacks. The four optional wait
instruction probes are compiled and disassembled only. No MONITORX, MWAITX,
UMONITOR or UMWAIT instruction is executed by a test, regardless of CPUID bits.

With the initialized compiler environment and the established CPU build gate:

```powershell
cmake -S tests/cpuid_module -B build/cpuid-check -G Ninja -DCMAKE_CXX_COMPILER=clang-cl -DCMAKE_BUILD_TYPE=Release -DNATIVE_ENABLE_EXCEPTIONS=OFF
cmake --build build/cpuid-check --parallel 1
ctest --test-dir build/cpuid-check --output-on-failure
```

Repeat in a separate build directory with exceptions ON to exercise actual
exception propagation. The fixture disables PCH and IPO. It does not execute
optional wait instructions or measure wait latency. `common-archive.txt`
records the common archive path for disassembly; `wait-codegen-object.txt`
records the separate optional-instruction probe object.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->

## Baseline profile admission

Import `native.x86.features` and link `native::common` for baseline module metadata and
the common archive's compiled observer and raw CPUID definitions. This is the
same target in build-tree and installed consumers. No aggregate target, ISA
module import or profile selection is needed. `observe_x86_capabilities()`
checks leaf ranges and only reads XCR0 after both XSAVE and OSXSAVE are present.
Its typed `present` and `observed` sets are authoritative for admission; raw
registers are nested under `raw` for diagnostics. XCR0 and vendor fields remain
separate. The synthetic register-removal cases use an explicit raw fixture so
they continue to exercise the shared decoder.
`classify_isa(snapshot, avx2)` (or `avx512`) is constexpr,
performs no hardware queries, and returns missing ISA features and XCR0 state plus a first `reason()`. Its reason
is the target spelling of a missing feature or an OS-state description. Unknown
or foreign-architecture features reject. Vendor names are not inputs.

The contract matches `native_target_profile`: AVX2 and FMA, and for AVX512
the F, DQ, BW and VL subsets. It also checks Clang's implied SSE3, SSSE3,
SSE4.1/4.2 and POPCNT requirements, MMX/SSE/SSE2 baseline bits, and F16C for
AVX512. XCR0 must enable XMM/YMM and, for AVX512, opmask and both ZMM components.
Custom ISA flags can impose additional requirements outside this contract.
BMI2 is independent: PDEP/PEXT require it, while SIMD profile admission does not.
MWAITX and WAITPKG are also independent typed features. The observer records
their leaf validity and support, and the wait module uses the same admission
checks for its cached `supported` flags. These features need no SIMD or XCR0
state. Tests check synthetic missing bits and stale leaves without executing
any wait instructions.
An observation describes the executing logical CPU; callers remain responsible
for affinity or a suitable common capability set on heterogeneous systems.

`admission.cc` independently removes every required CPU/state bit, checks
unavailable-leaf snapshots, unread state, invalid features, and CPU/OS reasons.
It is a configured-minimum module consumer with compile-time ISA guards, linked
only to the build-tree `native::common` archive; both the standalone fixture and
ordinary x86 CTest suite run it. The `tests/cpuid_package` fixture reuses the
same caller against an installed/relocated package and links only
`native::common`, proving its runtime definitions are independently usable. Its compilation guards and exported compile commands check that this
common consumer inherits no AVX-512 flags unless `NATIVE_MINIMAL_HAS_AVX512`
reports them in the configured minimum. The executable already requires that
minimum; an admission query cannot make it safe on weaker hardware. No test enters a native SIMD kernel based on a synthetic snapshot.
