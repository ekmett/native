# Packed compaction and double-source shifts

`import native.x86.vbmi2;` exposes AVX512VBMI2 through `simd` values,
`predicate` masks and typed memory pointers. Link `native::native`; the
`native.x86` and `native` hubs also export this module. Every operation has
128, 256 and 512-bit forms with matching vector and mask architecture tags.

| Operation | Element type | Result |
| --- | --- | --- |
| `vpcompressb`, `vpcompressw` masked forms | `uint8_t`, `uint16_t` | Pack selected input lanes into a contiguous prefix |
| `vpexpandb`, `vpexpandw` masked forms | `uint8_t`, `uint16_t` | Expand a packed prefix into selected result lanes |
| `vpshldw`, `vpshldd`, `vpshldq` | `uint16_t`, `uint32_t`, `uint64_t` | Shift `a` left, bringing high bits of `b` into the low end |
| `vpshrdw`, `vpshrdd`, `vpshrdq` | `uint16_t`, `uint32_t`, `uint64_t` | Shift `a` right, bringing low bits of `b` into the high end |
| `vpshldvw`, `vpshldvd`, `vpshldvq` | Same unsigned types | Per-lane variable left shifts |
| `vpshrdvw`, `vpshrdvd`, `vpshrdvq` | Same unsigned types | Per-lane variable right shifts |

## Compaction

Register compaction takes `mask_vpcompressb(source, mask, value)` or
`maskz_vpcompressb(mask, value)`, with word analogues. Selected input lanes
are packed in increasing lane order across the complete vector. Merging
preserves `source` above the packed prefix; zeroing clears that suffix.
For example, mask bits 1 and 5 select `value[1]` and `value[5]` into result
lanes 0 and 1. The merging suffix begins at lane 2, irrespective of mask bits.

Register expansion uses `mask_vpexpandb(source, mask, value)` or
`maskz_vpexpandb(mask, value)`, again with word analogues. It consumes the
first `popcount(mask)` input lanes and writes them in order to active output
positions. Inactive positions retain `source` or become zero.

The same names provide typed memory overloads:

- `mask_vpcompressb(destination, mask, value)` writes a packed byte prefix;
  the word overload takes `std::uint16_t*`.
- `mask_vpexpandb(source, mask, memory)` and
  `maskz_vpexpandb(mask, memory)` read a packed byte prefix and expand it;
  the word overloads read `std::uint16_t` elements.

The pointer must identify at least `popcount(mask)` live elements of the
matching unsigned type. Stores require writable elements. Loads accept
mutable or const pointers. Volatile, raw `void*`, unrelated element types
and mismatched masks are rejected. Exactly the packed prefix is accessed;
no full-vector alignment is required beyond ordinary element alignment.
A typed null pointer is valid when the logical mask is empty, including when
unused high bits were supplied to `predicate::from_bitset`. Stores return
`void`. These instruction forms have no capacity argument; the generic
`compress_store` API supplies a separate bounded-store contract.

## Double-source shifts

Immediate shifts use an unsigned byte template argument after the architecture:
`vpshldw<Arch, 7>(a, b)`. Values from 0 through 255 are accepted; other values
are rejected. Variable shifts take `a, b, counts`, with all three operands
using the same unsigned element type and vector shape.

Counts wrap modulo the element's bit width. A wrapped zero returns `a`.
For a nonzero count `c`, left shifts return `(a << c) | (b >> (bits - c))`,
and right shifts return `(a >> c) | (b << (bits - c))`, truncated to the
unsigned element width. Counts never move data between lanes.

Every shift has ordinary, merging and zeroing forms. Immediate merging forms
take `source, mask, a, b`; variable merging forms take `a, mask, b, counts`
and retain inactive lanes from `a`, matching the instruction's destination.
Each `maskz_` form takes its mask first and clears inactive lanes.

## Features and constant evaluation

Use `target_features<native::x86>("avx512vbmi2")` for 512-bit forms or add
`,avx512vl` for shorter forms. Runtime overloads require F/BW/VBMI2 and, for
128/256 bits, VL. BW reflects Clang's VBMI2 target prerequisites. DQ, VBMI,
BITALG and VPOPCNTDQ remain independent. CPUID leaf 7, subleaf 0, ECX bit 6
identifies VBMI2; callers must also admit OS AVX-512 state before entering a
matching compiler target.

Strong tags provide `constexpr` operations with native runtime paths. Weaker
tags provide `consteval` operations when their storage exists: SSE2 for
128 bits, AVX for 256 bits or AVX512F for 512 bits, including prerequisites.
Memory forms preserve the same extent and empty/null contract during constant
evaluation. These overloads do not add a software fallback for runtime calls.

See the [fixture](../tests/x86_vbmi2/README.md) for semantic, memory, admission
and assembly validation. Semantics follow Intel's
[Software Developer's Manual](https://cdrdv2-public.intel.com/868137/325462-089-sdm-vol-1-2abcd-3abcd-4.pdf)
and Clang's [VBMI2 intrinsic definitions](https://clang.llvm.org/doxygen/avx512vbmi2intrin_8h_source.html).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
