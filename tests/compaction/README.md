<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->

# Compaction validation and measurements

The public contract and example are in [the module guide](../../docs/modules.md).
These tests cover stable logical lane order, count, explicit fill/merge, exact
bounded stores, and bitwise preservation for float, int32_t, and uint32_t.

## Correctness receipts

Windows clang-cl 23.1.1 on a Ryzen 9 7950X3D passed all seven native tests and
all seven AddressSanitizer tests. Each header and imported-module executable
reported 1,261 checks for scalar, 835,801 for AVX2, and 143,376,616 for AVX512.
The seventh executable checks implicit AVX512 F/DQ textual configuration without
VL/BW at widths 4, 8, and 16. It excludes existing short-vector memory operations
that already require VL; official AVX512 profile short-vector coverage includes
widths 2 and 3. These are test assertion counts, not independent input counts.

All masks are enumerated at every supported width, including all 65,536 masks
at width 16. Cases include poisoned physical padding, signed zero and NaN
payloads (including signaling NaNs), fill and merge behavior, scalar reference
relationships, floating-point control-state preservation, every bounded output
capacity, null zero-access cases, and writes ending immediately before a guard
page. Test bodies receive profile flags; common-baseline drivers perform
admission before entering the body. Every executable requires the configured
minimum before startup, which defaults to the toolchain's baseline. Driver
guards permit AVX-512 only when minimal feature probes report it as configured;
the guards do not provide a launcher for hardware below the minimum. The hosted Linux ARM job passed 39/39 core tests, including 37,021 checks
in each NEON header/import compaction executable, plus the relocated public
header and omnibus fixtures. Its native lowering and measurements are below.

After configuring the project normally, build the compaction targets and run:

```sh
ctest --test-dir build -R '^native[.]compaction[.]' --output-on-failure
build/tests/compaction/native_compaction_avx2_header --bench
build/tests/compaction/native_compaction_avx512_header --bench
```

Use the executable suffix/layout appropriate to the generator. Configure a
separate build with `NATIVE_ENABLE_ASAN=ON` for the sanitizer run. The installed
`tests/api` fixture also compiles public compaction calls after package relocation.

## Local selection measurements

The table gives median nanoseconds per block, not per lane, on the Windows host
above. AVX2 uses eight uint32_t lanes and AVX512 sixteen. Each observation runs
4,096 blocks 64 times; seven paired observations alternate implementation and
reference order. The reference is a simple scalar-source loop compiled with
the same ISA flags and can be optimized by the compiler. Measurements include
mask construction, loads, stores and count accumulation. Store capacity is the
full vector width here; correctness tests separately exhaust smaller capacities.

| Profile | Operation | Mask | Prototype | Final | Final reference |
|---|---|---|---:|---:|---:|
| AVX2 | `compress` | full | 4.197 | 1.907 | 1.790 |
| AVX2 | `compress` | random | 4.148 | 1.898 | 5.416 |
| AVX2 | `expand` | full | 4.189 | 2.007 | 1.869 |
| AVX2 | `expand` | random | 4.174 | 1.989 | 3.899 |
| AVX2 | `compress_store` | full | 4.702 | 2.626 | 1.775 |
| AVX2 | `compress_store` | random | 6.487 | 4.046 | 3.606 |
| AVX512 | `compress` | full | 5.954 | 4.589 | 3.286 |
| AVX512 | `compress` | random | 5.912 | 4.620 | 11.589 |
| AVX512 | `expand` | full | 5.981 | 4.646 | 3.692 |
| AVX512 | `expand` | random | 5.951 | 4.686 | 9.421 |
| AVX512 | `compress_store` | full | 15.160 | 4.647 | 3.300 |
| AVX512 | `compress_store` | random | 15.027 | 4.618 | 10.159 |

[Raw observations](measurements-windows.json) retain all seven pairs and medians
for scalar, AVX2 and AVX512, all three operations, and empty/full/alternating/random
masks, with hashes of the original logs. Final source is commit `c9dc7b1`.
The prototype has retained receipts but no separately committed source revision.
Prototype and final were separate runs, each with its own paired reference;
differences between runs alone do not prove causation. In particular, AVX512
register compress/expand were unchanged, so their timing differences are not
attributed to the store implementation change.

AVX2 uses byte-shuffle lookup at four lanes and expanded index tables plus
`vpermd` at eight lanes. Direct 32-bit mask extraction and native output predicates
remove the prototype's generic mask conversions. AVX512 uses `vpcompressd` and
`vpexpandd`; the final bounded store uses register compression followed by an
exact prefix masked store. On this Zen 4 host that store measured substantially
less time than the prototype's memory compress instruction. Disassembly confirms
the intended instructions and the zero-write branch before destination access.

Random masks favor register compaction in this sample, but predictable masks
often favor the reference. AVX2 random bounded store remains slower than the
reference (4.046 versus 3.606 ns/block); its exact memcpy-prefix fallback is
retained for the bounded memory contract. No universal speedup or cross-CPU
conclusion follows. NEON uses table lookup and native mask reduction; the CI
workflow retains its paired observations and focused disassembly for review
without imposing a timing threshold on shared runners.


## Hosted NEON observation

The [Linux ARM job](https://github.com/ekmett/native/actions/runs/35305098483/job/105475557603)
used Clang 23.1.2 on Neoverse-N2 with exceptions OFF. Its four-lane uint32_t
benchmark uses the same seven-pair procedure. Median ns/block:

| Operation | Mask | Implementation | Reference |
|---|---|---:|---:|
| `compress` | full | 3.556 | 1.572 |
| `compress` | random | 3.556 | 7.611 |
| `expand` | full | 3.643 | 1.354 |
| `expand` | random | 3.642 | 4.124 |
| `compress_store` | full | 4.936 | 1.576 |
| `compress_store` | random | 5.356 | 4.376 |

[Raw NEON pairs](measurements-linux-arm.json) include every mask pattern.
Focused native disassembly confirms `tbl` for both register operations, `addv`
for mask reduction, and a bounded-store zero-count branch before the copying
path. Random register compaction/expansion beat the scalar-source reference in
this observation; bounded store and predictable masks favor the reference.
The exact memcpy-prefix store remains the portable bounded implementation.
Shared-runner timings are observational, with no performance threshold or
claim about other ARM CPUs. There is no before/after NEON optimization comparison.
