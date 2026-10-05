# x86 AVX2/AVX-512: gather and scatter

[x86 instruction sets](x86.md)

## Why use it

Indexed data does not always fit a contiguous load. Gather reads a vector from
several addresses; scatter writes one back. These operations are useful for
table lookups, sparse arrays and batches of indirect updates when changing the
data layout is impractical.

## Operations

`import native.x86.memory;` provides AVX2 gathers and AVX-512 masked gathers
and scatters. The `native` hub also exports them. Data and indices use `simd`
values; their architecture tag is deduced. Pointers retain their C++ element
types. Integer data can be signed or unsigned without changing bits.

| Data | 32-bit signed indices | 64-bit signed indices |
| --- | --- | --- |
| `float` | `vgatherdps` | `vgatherqps` |
| `double` | `vgatherdpd` | `vgatherqpd` |
| `int32_t` / `uint32_t` | `vpgatherdd` | `vpgatherqd` |
| `int64_t` / `uint64_t` | `vpgatherdq` | `vpgatherqq` |

Plain gathers take `<Scale, N>(base, indices)`, where `N` is the result lane
count and `Scale` is 1, 2, 4 or 8 bytes. Each effective address is
`base + signed_index * Scale` in bytes. For example, `<4,4>` loads four floats
at signed element offsets; `<1,4>` uses signed byte offsets.

AVX2 merging gathers take `mask_NAME<Scale>(source, mask, base, indices)`.
Their masks are signed integer vectors with the data's lane width and count;
only the sign bit of each mask lane enables a load.

AVX-512 masked gathers use the same names and argument order with
`predicate<N,Arch>` masks. Scatters replace `gather` with `scatter`:
`mask_vscatterdps<Scale>(base, mask, indices, value)`, for example. Masked forms
deduce the result or data shape and take only `Scale`.

| Index / data bits | Short forms: index lanes → data lanes | Full EVEX form |
| --- | --- | --- |
| 32 / 32 | 4 → 4; 8 → 8 | 16 → 16 |
| 32 / 64 | 4 → 2; 4 → 4 | 8 → 8 |
| 64 / 32 | 2 → 4; 4 → 4 | 8 → 8 |
| 64 / 64 | 2 → 2; 4 → 4 | 8 → 8 |

In the two-index/four-data-lane gather, the upper two results are zero even
with merging. Its scatter ignores the upper data and mask lanes. A
four-index/two-data-lane operation ignores the upper indices.

## Caveats

Every active address must allow a complete element access. Unaligned runtime
addresses are supported; inactive lanes access no memory, so an empty mask
permits a null base. Inactive addressable gather lanes retain `source`.
There are no bounds checks, atomic accesses or all-or-nothing fault semantics.

Overlapping scatter writes are ordered from lower to higher lane. The highest
active lane wins at identical addresses; partially overlapping writes follow
the same order. Hardware may omit an earlier write completely overwritten by
a later one. This is not an atomic indexed reduction.

A gather can still miss several cache lines; an instruction cannot repair a
poor access pattern. Compare against contiguous layouts and scalar accesses
in the actual workload.

AVX2 gather forms require AVX2. EVEX gathers and scatters require AVX512F,
with AVX512VL for short forms. Admit the full matching compiler target and OS
vector state before entry. No other register shape is synthesized.

Constant evaluation supports typed arrays, negative offsets within an array,
masks and duplicate destinations. Offsets must resolve to complete elements;
non-element-aligned, overflowing or active out-of-bounds offsets are rejected.
Weaker tags have `consteval` overloads when their vector storage exists, with
no scalar runtime fallback.

See Intel's [gather and scatter instruction definitions](https://cdrdv2-public.intel.com/671200/325462-sdm-vol-1-2abcd-3abcd.pdf).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
