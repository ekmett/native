# x86 GFNI: finite-field byte arithmetic

[x86 instruction sets](x86.md)

## Why use it

GFNI makes multiplication in GF(2⁸) and binary transforms of each byte direct
vector operations. That is useful for finite-field kernels, bit reversal and
changes of bit representation. An affine transform can express a whole byte's
linear bit mapping without a sequence of shifts and masks.

## Operations

`import native.x86.gfni;` provides these operations through `native::native`.
`native.x86` and `native` re-export them. Byte operands and results are
`simd<std::uint8_t,N,Arch>` with `N = 16, 32, 64`. Matrices are
`simd<std::uint64_t,N/8,Arch>`.

| Operation | Result for each byte |
| --- | --- |
| `gf2p8mulb<Arch>(a, b)` | Field product |
| `gf2p8affineqb<Arch, Imm8>(a, matrix)` | Binary matrix product XOR `Imm8` |
| `gf2p8affineinvqb<Arch, Imm8>(a, matrix)` | Field inverse, then matrix product XOR `Imm8` |

The field polynomial is x⁸ + x⁴ + x³ + x + 1 (`0x11b`). Inversion maps zero
to zero. Each matrix qword supplies an independent 8×8 matrix for eight bytes.
Numbering matrix bytes from the low end, result bit `i` is the parity of
`a_byte & matrix_byte[7-i]`, XOR bit `i` of the compile-time byte `Imm8`.
`0x0102040810204080` is the identity matrix; `0x8040201008040201` reverses bits.

Each operation also has `NAME_mask(src, k, a, b)` and `NAME_maskz(k, a, b)`
forms, with `matrix` replacing `b` for affine operations and the same template
immediate. Masks are `predicate<N,Arch>`; inactive bytes retain `src` or clear.

## Caveats

`affineinv` inverts each field element before the transform; it does not invert
the matrix. Matrix byte order matters. `Imm8` is in 0–255 and is shared by all
result bytes.

| Form | Runtime features | Caller target |
| --- | --- | --- |
| Unmasked 128-bit | GFNI | `"gfni"` |
| Unmasked 256-bit | GFNI, AVX | `"avx,gfni"` |
| Unmasked 512-bit | GFNI, AVX512F | `"avx512f,gfni"` |
| Masked 128/256-bit | GFNI, AVX512F, AVX512BW, AVX512VL | `"avx512bw,avx512vl,gfni"` |
| Masked 512-bit | GFNI, AVX512F, AVX512BW | `"avx512bw,gfni"` |

The unmasked 256-bit wrapper needs no AVX2. Masked wrappers require BW for
Clang's byte-mask operations. GFNI is independent of the general AVX2 and
AVX-512 profiles. Use `target_features<native::x86>(target)` and admit the
whole matching caller target before entry. Legacy 128-bit execution needs XMM
state; VEX also needs YMM state, and EVEX needs opmask and ZMM state.

Feature-bearing overloads are `constexpr` with native runtime paths. Weaker
tags have `consteval` overloads when the chosen SSE2, AVX or AVX512F register
storage exists. Results preserve their architecture tag; there is no runtime
software fallback.

See Intel's [GFNI instruction reference](https://cdrdv2-public.intel.com/868140/253666-089-sdm-vol-2a.pdf)
and Clang's [GFNI header](https://clang.llvm.org/doxygen/gfniintrin_8h_source.html).
