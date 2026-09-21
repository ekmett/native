# Byte permutations and qword bit windows

`import native.x86.vbmi;` exposes AVX512VBMI through `simd` values and
`predicate` writemasks. Link `native::native`; the `native.x86` and `native`
hubs also export the module. Byte vectors have 16, 32 or 64 lanes, corresponding
to 128, 256 or 512 bits, and retain their architecture tag.

| Operation | Operands | Unmasked result |
| --- | --- | --- |
| `vpermb` | `indices, value` | Byte `value[indices[i] % N]` |
| `vpermt2b` | `a, indices, b` | Byte selected from the concatenation of `a` and `b` |
| `vpermi2b` | `indices, a, b` | Same two-table selection, with indices as the tied destination |
| `vpmultishiftqb` | `control, value` | Eight wrapping bits from the corresponding source qword |

Single-table permutations use the low `log2(N)` index bits. Two-table
permutations use `log2(2*N)` bits, with indices below `N` selecting `a` and the
remaining indices selecting `b`. Other index bits are ignored. Selection spans
the complete vector, including other 128-bit blocks.

All four operations provide ordinary, merging and zeroing forms. The latter
use `predicate<N,Arch>`, whose bit `i` controls output byte `i`:

- `mask_vpermb(source, mask, indices, value)` and
  `mask_vpmultishiftqb(source, mask, control, value)` retain inactive `source` bytes.
- `mask_vpermt2b(a, mask, indices, b)` retains inactive bytes from `a`.
- `mask_vpermi2b(indices, mask, a, b)` retains inactive index bytes.
- Each `maskz_` form takes the mask first, followed by its ordinary operands,
  and clears inactive result bytes.

Both tables and the indices for active results remain unchanged by the
writemask. A masked-off byte can still be selected as source data elsewhere.
The compiler may choose either two-table encoding for ordinary or zeroing
forms; merging semantics preserve the designated destination operand.

Multishift takes `simd<std::uint8_t,N,Arch>` control bytes and
`simd<std::uint64_t,N/8,Arch>` source qwords. Result byte `i` contains eight
consecutive bits starting at `control[i] & 63` of qword `value[i / 8]`, with
bit positions wrapping from 63 to 0 within that qword. The first selected bit
becomes bit zero of the result byte. Control bits 6/7 are ignored.

Use `target_features<native::x86>("avx512vbmi")` for 512-bit forms or add
`,avx512vl` for 128/256-bit forms. Runtime overloads require F/BW/VBMI and, for
shorter forms, VL; BW reflects Clang's VBMI compiler target prerequisites.
DQ, BITALG and VBMI2 are independent. The feature is decoded from CPUID leaf
7, subleaf 0, ECX bit 1. Callers must admit CPU features and OS AVX-512 state
before entering matching compiler targets.

Every operation is constant evaluable. Strong tags use `constexpr` overloads
with native runtime paths. Weaker tags use `consteval` overloads with complete
storage: SSE2 for 128 bits, AVX for 256 bits or AVX512F for 512 bits, including
their prerequisites. These overloads preserve the tag and do not introduce a
software fallback for runtime values.

The [fixture](../tests/x86_vbmi/README.md) describes semantic, feature-boundary
and assembly checks. Semantics follow Intel's
[Software Developer's Manual](https://cdrdv2-public.intel.com/868137/325462-089-sdm-vol-1-2abcd-3abcd-4.pdf)
and Clang's [VBMI intrinsic definitions](https://clang.llvm.org/doxygen/avx512vbmiintrin_8h_source.html).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
