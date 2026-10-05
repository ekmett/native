# x86 AVX-512VPOPCNTDQ: packed population counts

[x86 instruction sets](x86.md)

## Why use it

When bitsets occupy several words, a packed population count handles each word
without unpacking the vector into scalar registers. XOR followed by counting
measures Hamming distance; AND followed by counting measures intersections.
The caller can then reduce the lane counts or keep one result per word.

## Operations

`import native.x86.vpopcntdq;` exports the operations below through
`native::native`. `native.x86` and `native` re-export the module.

| Operation | Input and result | Lane counts | Count range |
| --- | --- | --- | --- |
| `vpopcntd<Arch>(value)` | `simd<std::uint32_t,N,Arch>` | 4, 8, 16 | 0–32 |
| `vpopcntq<Arch>(value)` | `simd<std::uint64_t,N,Arch>` | 2, 4, 8 | 0–64 |

Each name also has `mask_NAME<Arch>(source, mask, value)` to keep inactive
source lanes and `maskz_NAME<Arch>(mask, value)` to clear them. Masks are
`predicate<N,Arch>`; bit `i` selects lane `i`. Results retain their element
width and exact architecture tag. All calls are `noexcept`.

## Caveats

These are per-lane counts, without an implicit horizontal sum. For byte and
word counts use [BITALG](x86-bitalg.md); for one scalar word use
[POPCNT](x86-popcnt.md). Their feature bits are independent.

Runtime calls need AVX512F and AVX512VPOPCNTDQ, plus AVX512VL for 128/256 bits.
Use `target_features<native::x86>("avx512vpopcntdq")`, adding `avx512vl` for
shorter forms. Admit the caller's whole compiler target and OS AVX-512 state
before entry, including for VL forms. The general AVX-512 profile does not
imply VPOPCNTDQ.

Feature-bearing overloads are `constexpr` with native runtime paths. Weaker
tags accept `consteval` calls if the vector storage exists: SSE2 for 128 bits,
AVX for 256 bits or AVX512F for 512 bits, with prerequisites. They supply no
runtime software fallback.

See Clang's [512-bit](https://clang.llvm.org/doxygen/avx512vpopcntdqintrin_8h_source.html)
and [VL](https://clang.llvm.org/doxygen/avx512vpopcntdqvlintrin_8h_source.html) intrinsic interfaces.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
