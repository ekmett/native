# Byte/word population counts and bit selection

`import native.x86.bitalg;` provides AVX512BITALG operations on `simd` values.
Link `native::native`; the `native.x86` and `native` hubs also export the module.

| Operation | Input | Result | Lane counts |
| --- | --- | --- | --- |
| `vpopcntb` | `simd<std::uint8_t,N,Arch>` | Same vector type | 16, 32, 64 |
| `vpopcntw` | `simd<std::uint16_t,N,Arch>` | Same vector type | 8, 16, 32 |
| `vpshufbitqmb` | `simd<std::uint64_t,Q,Arch>`, `simd<std::uint8_t,8*Q,Arch>` | `predicate<8*Q,Arch>` | Q = 2, 4, 8 |

Population counts return the number of set bits in each lane, from 0 to 8 or
16. Each operation has three forms:

- `operation<Arch>(value)` computes every lane.
- `mask_operation<Arch>(source, mask, value)` retains `source` in inactive lanes.
- `maskz_operation<Arch>(mask, value)` clears inactive lanes.

The mask is `predicate<N,Arch>`; bit `i` controls result lane `i`.

`vpshufbitqmb<Arch>(value, control)` selects one bit per control byte. Output bit
`i` comes from bit `control[i] & 63` of source qword `value[i / 8]`. Each group
of eight control bytes therefore addresses its corresponding qword. Control
bits 6 and 7 are ignored, and no selection crosses a qword boundary.
`mask_vpshufbitqmb<Arch>(mask, value, control)` clears inactive predicate bits.
The instruction has no merging destination, so this operation has no separate
merge-source or `maskz_` form.

Use `target_features<native::x86>("avx512bitalg")` for 512-bit forms or
`target_features<native::x86>("avx512bitalg,avx512vl")` for shorter forms.
Runtime overloads require AVX512F, AVX512BW and AVX512BITALG; 128/256-bit forms
also require AVX512VL. The AVX512BW requirement records Clang's BITALG target
prerequisites, including the wide compact-mask moves used by these wrappers.
AVX512DQ and AVX512VPOPCNTDQ are independent and are not required. Scalar POPCNT
or VPOPCNTDQ support alone does not authorize BITALG instructions.

The caller must admit CPU features and OS AVX-512 state before entering a
matching compiler target. `x86_feature::avx512bitalg` is decoded from CPUID leaf
7, subleaf 0, ECX bit 12. Registering the feature does not add it to the general
`avx512` profile.

All forms support constant evaluation. Instruction-bearing tags use `constexpr`
overloads with native runtime paths. Tags lacking instruction features use
`consteval` overloads and still require complete storage: SSE2 for 128 bits,
AVX for 256 bits, or AVX512F for 512 bits, including their prerequisites. They
preserve the architecture tag and do not provide a software runtime fallback.

The [fixture](../tests/x86_bitalg/README.md) documents the constant and runtime
references, feature rejection checks, and raw/public assembly comparisons.
Instruction semantics follow Intel's [Software Developer's Manual](https://cdrdv2-public.intel.com/868137/325462-089-sdm-vol-1-2abcd-3abcd-4.pdf).
Intrinsic forms are documented in Clang's
[BITALG](https://clang.llvm.org/doxygen/avx512bitalgintrin_8h_source.html) and
[VL BITALG](https://clang.llvm.org/doxygen/avx512vlbitalgintrin_8h_source.html)
headers.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
