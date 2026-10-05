# x86 AVX-IFMA/AVX-512IFMA: 52-bit multiply-add

[x86 instruction sets](x86.md)

## Why use it

A 52-bit limb leaves twelve spare bits in a qword for accumulation. IFMA
multiplies two such limbs and adds either half of their 104-bit product into
64-bit lanes. This is useful for batched multi-precision multiplication and
modular arithmetic, where the algorithm controls carry propagation.

## Operations

`import native.x86.ifma;` supplies operations on
`simd<std::uint64_t,N,Arch>` with `N = 2, 4, 8`. Link `native::native`;
`native.x86` and `native` also export them.

| Operation | Per-lane result |
| --- | --- |
| `madd52lo<Arch>(accumulator, a, b)` | Add product bits 0–51 to the accumulator |
| `madd52hi<Arch>(accumulator, a, b)` | Add product bits 52–103 to the accumulator |

The unsigned product uses only the low 52 bits of each multiplicand.
Every accumulator bit participates, and addition wraps modulo 2⁶⁴.

Both names have `mask_NAME<Arch>(accumulator, mask, a, b)` to retain inactive
accumulator lanes and `maskz_NAME<Arch>(mask, accumulator, a, b)` to clear them.
Masks are `predicate<N,Arch>`.

## Caveats

There is no carry propagation between lanes, saturation or modular reduction.
The caller must account for accumulated carries before a 64-bit lane wraps.
The top twelve bits of multiplicands are ignored even when they contain a carry.

| Form | Runtime features | Caller target |
| --- | --- | --- |
| Unmasked 128/256-bit VEX | AVX-IFMA, AVX | `"avxifma"` |
| 128/256-bit EVEX, all mask modes | AVX512IFMA, AVX512F, AVX512VL | `"avx512ifma,avx512vl"` |
| 512-bit EVEX, all mask modes | AVX512IFMA, AVX512F | `"avx512ifma"` |

Short unmasked forms prefer AVX-IFMA when the tag supports it. Masked runtime
forms always need EVEX features. BW and DQ are unnecessary. AVX-IFMA and
AVX512IFMA are independent features, absent from the general AVX2 and AVX-512
profiles. Clang's AVX-IFMA target also enables AVX2; use `target_features` to
include the compiler prerequisites. Admit the complete target and OS vector
state before entering a matching caller.

Feature-bearing overloads are `constexpr` with native runtime paths. Weaker
tags have `consteval` overloads if the chosen SSE2, AVX or AVX512F register
storage exists, with prerequisites. Constant evaluation gives exact integer
results and preserves the tag.

See Intel's [Software Developer's Manual](https://cdrdv2-public.intel.com/868137/325462-089-sdm-vol-1-2abcd-3abcd-4.pdf)
and Clang's [AVX-IFMA](https://clang.llvm.org/doxygen/avxifmaintrin_8h_source.html),
[AVX512IFMA](https://clang.llvm.org/doxygen/avx512ifmaintrin_8h_source.html) and
[VL IFMA](https://clang.llvm.org/doxygen/avx512ifmavlintrin_8h_source.html) headers.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
