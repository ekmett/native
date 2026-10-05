# x86 PCLMULQDQ/VPCLMULQDQ: carry-less polynomial products

[x86 instruction sets](x86.md)

## Why use it

Binary polynomials add with XOR instead of carry. Carry-less multiplication
provides the product used by CRC folding and binary-field arithmetic, including
the multiplication stage of GHASH. Vector forms handle several independent
products in one register.

## Operations

`import native.x86.pclmul;` provides `pclmulqdq<Arch, Imm8>` on
`simd<std::uint64_t,2,Arch>`. `import native.x86.vpclmul;` provides
`vpclmulqdq<Arch, Imm8>` on `simd<std::uint64_t,N,Arch>` for `N = 2, 4, 8`.
Both belong to `native::native` and are re-exported by `native.x86` and `native`.

Each input bit is a GF(2) polynomial coefficient. The operation multiplies two
selected 64-bit polynomials and returns their exact 128-bit product. Bit 0 of
`Imm8` selects the half of `a`; bit 4 selects the half of `b`. Zero means the
low half, one the high half. The useful selectors are `0x00`, `0x01`, `0x10`
and `0x11`; other immediate bits are ignored. `Imm8` must be in 0–255.

Wider forms contain two or four independent products, one per 128-bit block.
For example, `vpclmulqdq<arch, 0x10>(a, b)` multiplies the low qword of each
`a` block by the high qword of the corresponding `b` block.

## Caveats

The product has no integer carry or polynomial reduction. Its degree is at
most 126, so output bit 127 is zero. The caller supplies reduction and any
byte/bit-order changes required by a CRC or field representation. There are
no cross-block products or writemasks.

| Operation | Width | Runtime features | Caller target |
| --- | --- | --- | --- |
| `pclmulqdq` | 128 | PCLMUL | `"pclmul"` |
| `vpclmulqdq` | 128 | PCLMUL, AVX | `"avx,pclmul"` |
| `vpclmulqdq` | 256 | VPCLMULQDQ, AVX | `"avx,vpclmulqdq"` |
| `vpclmulqdq` | 512 | VPCLMULQDQ, AVX512F | `"avx512f,vpclmulqdq"` |

The 256-bit form needs no AVX2; the 512-bit form needs no BW, DQ or VL.
Narrow EVEX encodings additionally require VPCLMULQDQ and AVX512VL; the compiler
may choose an encoding permitted by the caller. `target_features` includes the
compiler prerequisites. Admit the matching target and OS vector state before
entry. PCLMUL and VPCLMULQDQ are independent CPU feature bits.

Feature-bearing overloads are `constexpr` with native runtime paths. Weaker
tags accept `consteval` calls with complete SSE2, AVX or AVX512F storage for
the chosen width. Inputs must have the same tag and unsigned qword shape;
results preserve that tag. These integer operations leave FP status unchanged.

See Intel's [PCLMULQDQ reference](https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-2b-manual.pdf),
[VPCLMULQDQ reference](https://kib.kiev.ua/x86docs/Intel/ISAFuture/319433-031.pdf)
and Clang's [intrinsic declarations](https://clang.llvm.org/doxygen/vpclmulqdqintrin_8h_source.html).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
