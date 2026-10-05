# x86 AVX-512VBMI: byte permutations and bit windows

[x86 instruction sets](x86.md)

## Why use it

A byte permutation can rearrange packed records or implement a small table
lookup without being confined to 128-bit blocks. VBMI adds whole-vector byte
selection from one or two tables. Multishift extracts several eight-bit windows
from each qword, useful when values arrive in a bit-packed representation.

## Operations

`import native.x86.vbmi;` exports the following through `native::native`.
`native.x86` and `native` also export them. Byte vectors have type
`simd<std::uint8_t,N,Arch>` with `N = 16, 32, 64`.

| Operation | Operands | Result byte `i` |
| --- | --- | --- |
| `vpermb` | `indices, value` | `value[indices[i] % N]` |
| `vpermt2b` | `a, indices, b` | Selection from the concatenation of `a` and `b` |
| `vpermi2b` | `indices, a, b` | Same two-table selection, with indices as tied destination |
| `vpmultishiftqb` | `control, value` | Eight wrapping bits from qword `value[i / 8]` |

Single-table indices use their low `log2(N)` bits. Two-table indices use
`log2(2*N)` bits: indices below `N` choose `a`, the rest choose `b`.
Selection spans the complete vector.

Multishift control is a byte vector, while `value` is
`simd<std::uint64_t,N/8,Arch>`. Byte `i` selects eight bits starting at
`control[i] & 63` of qword `value[i / 8]`. Positions wrap from 63 to zero within
that qword; the first selected bit becomes result bit zero.

All names take an explicit `<Arch>` and provide merging and zeroing forms
with `predicate<N,Arch>` masks:

- `mask_vpermb(source, mask, indices, value)` and
  `mask_vpmultishiftqb(source, mask, control, value)` keep inactive source bytes.
- `mask_vpermt2b(a, mask, indices, b)` keeps inactive bytes from `a`.
- `mask_vpermi2b(indices, mask, a, b)` keeps inactive index bytes.
- `maskz_NAME(mask, ...)` clears inactive bytes and otherwise uses the ordinary
  operand order.

## Caveats

Writemasks control output bytes. A masked-off table byte can still supply data
to an active output byte. The two merging permutations differ in which operand
survives; the compiler may choose either encoding for unmasked or zeroing calls.

Multishift never crosses a qword boundary and ignores control bits 6 and 7.
It is a wrapping bit window, not a whole-vector variable shift.

Runtime wrappers need AVX512F, AVX512BW and AVX512VBMI, plus AVX512VL for short
forms. Use `target_features<native::x86>("avx512vbmi")`, adding `avx512vl` for
128/256 bits. BW reflects Clang's target prerequisites. DQ, BITALG and VBMI2
are independent. Admit the full matching caller target and OS AVX-512 state
before entry.

Feature-bearing overloads are `constexpr` with native runtime paths; weaker
tags are `consteval`-only with complete SSE2, AVX or AVX512F storage for the
chosen width. Results preserve the exact architecture tag.

See Intel's [Software Developer's Manual](https://cdrdv2-public.intel.com/868137/325462-089-sdm-vol-1-2abcd-3abcd-4.pdf)
and Clang's [VBMI definitions](https://clang.llvm.org/doxygen/avx512vbmiintrin_8h_source.html).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
