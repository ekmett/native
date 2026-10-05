# x86 AVX-512CD: conflict detection and leading-zero counts

[x86 instruction sets](x86.md)

## Why use it

A vectorized histogram or indexed update cannot blindly write several lanes to
the same destination. Conflict detection identifies repeated indices within a
batch, so the algorithm can separate independent updates from collisions.
AVX-512CD also counts leading zeros in each integer lane for normalization and
magnitude classification.

## Operations

`import native.x86.avx512cd;` provides the operations below. Link
`native::native`; `native.x86` and `native` also export them. Each preserves
the vector's element type, lane count and architecture tag.

| Operations | Input and result | Lane counts |
| --- | --- | --- |
| `vpconflictd`, `vplzcntd` | `simd<std::uint32_t,N,Arch>` | 4, 8, 16 |
| `vpconflictq`, `vplzcntq` | `simd<std::uint64_t,N,Arch>` | 2, 4, 8 |

For lane `i`, `vpconflict*` sets result bit `j` when `j < i` and input lanes
`j` and `i` are equal. Lane zero therefore returns zero. Comparisons span the
whole vector. `vplzcnt*` returns a count from zero to the lane width; zero
inputs give 32 or 64.

Each name has three forms:

- `operation<Arch>(value)` computes every lane.
- `mask_operation<Arch>(source, mask, value)` keeps inactive `source` lanes.
- `maskz_operation<Arch>(mask, value)` clears inactive lanes.

Masks are `predicate<N,Arch>`; bit `i` controls result lane `i`.

## Caveats

A conflict writemask controls results, not comparisons. A masked-off earlier
lane still contributes to a later lane's conflict mask. Applying a mask does
not remove those indices from the batch.

Runtime calls need AVX512F and AVX512CD; 128/256-bit forms also need AVX512VL.
Use `target_features<native::x86>("avx512cd")`, adding `avx512vl` for shorter
forms, and admit the caller's full target and OS AVX-512 state before entry.
Scalar LZCNT does not authorize vector leading-zero instructions.

Feature-bearing overloads are `constexpr` with native runtime paths. Weaker
tags are `consteval`-only and still need register storage: SSE2 for 128 bits,
AVX for 256 bits or AVX512F for 512 bits, with prerequisites. Constant
evaluation preserves the same conflict and mask semantics.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
