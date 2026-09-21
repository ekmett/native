<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
# PCLMULQDQ and VPCLMULQDQ fixture

The granular-module, `native.x86` and `native` executables check all
four callable overloads against an independent scalar bit-polynomial oracle.
Each admitted form exercises all 4096 one-hot pairs per half selector and lane,
2048 deterministic random inputs, directed zero/one/high-bit/alternating-bit
patterns, and lane isolation. All 256 immediate bytes are checked on the
directed patterns and 128 random inputs. The fixture also checks that MXCSR
is unchanged.

Admission runs at the compiler baseline. CPUID and XCR0 are printed before
entering targeted functions; unsupported forms print `SKIP`. A return code
of 77 means no form could execute. The 256-bit path requires AVX and
VPCLMULQDQ and does not require AVX2 or AVX-512.

Separate textual probes disable PCLMUL, VPCLMULQDQ, AVX, AVX2 and AVX-512.
Function attributes then enable each instruction's requirements. Disassembly
checks 48 operations: eight selectors for legacy128, VEX128/256 and
EVEX128/256/512. High source registers prevent narrow EVEX probes from being
compressed to VEX. The importer probe checks that importing the main hub does
not enable optional instructions. Compiler-only feature settings never alter
imported module targets or request alternate BMIs.

Twenty-five textual compile-failure tests cover missing feature bits, missing
caller targets, out-of-range/runtime/negative/overflowing immediates, floating
registers, mixed widths and scalar operands. Six further compile-failure tests
import the granular modules and verify caller target requirements for all four
overloads and deleted floating-register fallbacks for both families. The
granular-module executable supplies the corresponding positive controls.
Four metadata compilations check
the baseline, PCLMUL, VPCLMULQDQ and an unregistered VAES feature. Synthetic
snapshots check CPUID leaf bounds, bit 10 and its neighbors, present versus
observed, compiler closure, XSAVE/OSXSAVE, AVX and AVX-512 XCR0 masks.

Build the repository with Clang 23, CMake 4.4, Ninja, Release, libc++ and IPO
disabled. Then run:

```sh
cmake --build build/core --parallel
ctest --test-dir build/core -R native.x86.pclmul --output-on-failure
```

The directory also configures independently against an installed package:

```sh
cmake -S tests/x86_pclmul -B build/consumer -G Ninja \
  -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_FLAGS=-stdlib=libc++ \
  -DCMAKE_BUILD_TYPE=Release -Dnative_DIR='/relocated prefix/lib/cmake/native'
cmake --build build/consumer --parallel
ctest --test-dir build/consumer --output-on-failure
```

That mode uses only installed modules and public configuration/attribute
headers. It builds the granular, x86 umbrella and main-hub consumers plus
the baseline importer check. To test relocation, install to one prefix,
physically move the entire installation, remove the original prefix, and
configure a fresh consumer against the destination. CMake regenerates provider
BMIs using the installed module sources.

The public vector calls use `native::simd` with exact element types, lane counts
and architecture tags. Masked calls use `native::predicate` with the result lane count and tag.
Private register probes and public module probes independently retain instruction
and immediate checks; scalar oracles operate outside optional target scopes.
