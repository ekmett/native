# x86 AVX-512BITALG: byte counts and bit selection

[x86 instruction sets](x86.md)

## Why use it

Byte and word population counts work directly on small bitsets, packed flags
and Hamming-distance data. Bit selection turns positions within packed qwords
into a predicate, avoiding a separate scalar extraction for each selected bit.

## Operations

`import native.x86.bitalg;` supplies these operations through `native::native`.
`native.x86` and `native` also export them.

| Operation | Input | Result | Lane counts |
| --- | --- | --- | --- |
| `vpopcntb` | `simd<std::uint8_t,N,Arch>` | Same vector type | 16, 32, 64 |
| `vpopcntw` | `simd<std::uint16_t,N,Arch>` | Same vector type | 8, 16, 32 |
| `vpshufbitqmb` | `simd<std::uint64_t,Q,Arch>`, `simd<std::uint8_t,8*Q,Arch>` | `predicate<8*Q,Arch>` | Q = 2, 4, 8 |

Population counts are in 0–8 or 0–16. Their ordinary forms take `<Arch>(value)`;
`mask_NAME<Arch>(source, mask, value)` merges inactive source lanes and
`maskz_NAME<Arch>(mask, value)` clears them. Masks are `predicate<N,Arch>`.

`vpshufbitqmb<Arch>(value, control)` sets result bit `i` from bit
`control[i] & 63` of qword `value[i / 8]`. Each group of eight control bytes
addresses its own qword. `mask_vpshufbitqmb<Arch>(mask, value, control)` clears
inactive predicate bits. It has no merge-source or separate `maskz_` form.

## Caveats

Bit selection does not cross qword boundaries; control bits 6 and 7 are ignored.
For full-vector byte permutations, use [VBMI](x86-vbmi.md).

Runtime wrappers need AVX512F, AVX512BW and AVX512BITALG, plus AVX512VL for
128/256 bits. Use `target_features<native::x86>("avx512bitalg")`, adding
`avx512vl` for shorter forms. BW is part of Clang's compiler prerequisites and
mask operations. DQ and VPOPCNTDQ are independent. Scalar POPCNT does not
establish BITALG support, nor does the general AVX-512 profile.

Admit the matching compiler target and OS AVX-512 state before entry.
Feature-bearing overloads are `constexpr` with native runtime paths; weaker
tags are `consteval`-only with complete SSE2, AVX or AVX512F storage for the
chosen width. Results preserve their architecture tag.

See Intel's [Software Developer's Manual](https://cdrdv2-public.intel.com/868137/325462-089-sdm-vol-1-2abcd-3abcd-4.pdf)
and Clang's [BITALG](https://clang.llvm.org/doxygen/avx512bitalgintrin_8h_source.html)
and [VL BITALG](https://clang.llvm.org/doxygen/avx512vlbitalgintrin_8h_source.html) headers.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
