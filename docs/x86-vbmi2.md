# x86 AVX-512VBMI2: compaction and double-source shifts

[x86 instruction sets](x86.md)

## Why use it

Filtering a vector leaves holes. Byte and word compaction closes them, producing
a dense prefix that can be stored without a scalar loop. Expansion puts that
prefix back into selected positions. Double-source shifts assemble bit fields
from adjacent pieces held in corresponding lanes.

## Operations

`import native.x86.vbmi2;` exports these operations through `native::native`.
`native.x86` and `native` also export them. All families have 128-, 256- and
512-bit forms with matching `simd` and `predicate` architecture tags.

| Operation family | Unsigned element | Meaning |
| --- | --- | --- |
| `vpcompressb`, `vpcompressw` masked forms | `uint8_t`, `uint16_t` | Pack selected lanes into a prefix |
| `vpexpandb`, `vpexpandw` masked forms | `uint8_t`, `uint16_t` | Expand a prefix into selected positions |
| `vpshldw`, `vpshldd`, `vpshldq` | `uint16_t`, `uint32_t`, `uint64_t` | Immediate double-source left shift |
| `vpshrdw`, `vpshrdd`, `vpshrdq` | Same widths | Immediate double-source right shift |
| `vpshldvw`, `vpshldvd`, `vpshldvq` | Same widths | Per-lane variable left shift |
| `vpshrdvw`, `vpshrdvd`, `vpshrdvq` | Same widths | Per-lane variable right shift |

### Compaction

`mask_vpcompressb(source, mask, value)` packs active input lanes in increasing
order across the whole vector and keeps `source` above the packed prefix.
`maskz_vpcompressb(mask, value)` zeros that suffix. Word forms work the same
way. Mask bits 1 and 5, for example, place `value[1]` and `value[5]` in result
lanes 0 and 1; the merge suffix starts at lane 2.

`mask_vpexpandb(source, mask, value)` consumes the first `popcount(mask)` input
lanes and places them in active result positions. Inactive positions keep
`source`; `maskz_vpexpandb(mask, value)` clears them. Word forms are analogous.

Typed memory overloads use the same names:

- `mask_vpcompressb(destination, mask, value)` writes a packed byte prefix and
  returns `void`; the word form takes `std::uint16_t*`.
- `mask_vpexpandb(source, mask, memory)` and `maskz_vpexpandb(mask, memory)` read
  a packed prefix; word forms read `std::uint16_t` elements.

Memory forms access exactly `popcount(mask)` elements. A typed null pointer is
valid for an empty logical mask. Loads accept const or mutable pointers;
stores need writable elements. Ordinary element alignment is sufficient.

### Double-source shifts

Immediate forms take `<Arch, Imm8>(a, b)` with `Imm8` in 0–255. Variable forms
take `<Arch>(a, b, counts)` with identical unsigned element types and shapes.
Counts wrap modulo the lane's bit width. Zero returns `a`; for nonzero `c`:

- Left: `(a << c) | (b >> (bits - c))`.
- Right: `(a >> c) | (b << (bits - c))`.

Results are truncated to the lane width. Immediate merging forms take
`source, mask, a, b`; variable merging forms take `a, mask, b, counts` and keep
inactive lanes from `a`. Zeroing forms put the mask first.

## Caveats

Compaction's mask chooses input lanes; expansion's mask chooses output lanes.
A compression merge keeps the suffix above the packed count, rather than the
positions whose mask bits were clear. Double-source shifts never move bits
between lanes.

Memory forms have no capacity argument. The prefix must fit in live objects
of the exact element type; volatile, `void*` and unrelated pointers are rejected.
Use generic `compress_store` for its separate bounded-store contract.

Runtime wrappers need AVX512F, AVX512BW and AVX512VBMI2, plus AVX512VL for
128/256 bits. Use `target_features<native::x86>("avx512vbmi2")`, adding
`avx512vl` for short forms. BW reflects Clang's target prerequisites. VBMI,
BITALG, DQ and VPOPCNTDQ are independent. Admit the matching compiler target
and OS AVX-512 state before entry.

Feature-bearing overloads are `constexpr` with native runtime paths; weaker
tags have `consteval` overloads with complete storage for the chosen width.
Constant evaluation retains the same memory extent and empty/null rules.

See Intel's [Software Developer's Manual](https://cdrdv2-public.intel.com/868137/325462-089-sdm-vol-1-2abcd-3abcd-4.pdf)
and Clang's [VBMI2 definitions](https://clang.llvm.org/doxygen/avx512vbmi2intrin_8h_source.html).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
