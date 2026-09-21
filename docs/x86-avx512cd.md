# Conflict detection and leading-zero counts

`import native.x86.avx512cd;` provides lane conflict masks and leading-zero
counts. Link `native::native`; `native.x86` and `native` also export the module.
The operations take `simd` values and preserve their element type, lane count
and architecture tag.

| Operations | Input and result | Lane counts |
| --- | --- | --- |
| `vpconflictd`, `vplzcntd` | `simd<std::uint32_t,N,Arch>` | 4, 8, 16 |
| `vpconflictq`, `vplzcntq` | `simd<std::uint64_t,N,Arch>` | 2, 4, 8 |

For each input lane `i`, conflict detection returns a bit mask: bit `j` is set
when `j < i` and input lanes `j` and `i` are equal. The first lane therefore
returns zero. Comparisons span the entire vector, including earlier lanes in
other 128-bit blocks.

Leading-zero counts range from zero to the lane's bit width. A zero input
returns 32 for `vplzcntd` or 64 for `vplzcntq`.

Each operation has three forms:

- `operation<Arch>(value)` computes every lane.
- `mask_operation<Arch>(source, mask, value)` retains `source` in inactive lanes.
- `maskz_operation<Arch>(mask, value)` clears inactive lanes.

The mask has type `predicate<N,Arch>`. Its bit `i` controls result lane `i`.
For conflict detection, masking affects the result only: a masked-off earlier
lane still participates in comparisons for later active lanes. In particular,
masking does not partition the input or remove duplicates from it.

Runtime calls require `avx512f` and `avx512cd`; 128-bit and 256-bit forms also
require `avx512vl`. Construct the tag with
`target_features<native::x86>("avx512f,avx512cd")` or the corresponding target
with `avx512vl`. The caller needs matching compiler permissions and runtime
admission, including the OS vector state required by AVX-512. Scalar LZCNT
support does not authorize these vector operations.

All forms support constant evaluation with the same lane and mask semantics.
Feature-bearing tags use `constexpr` overloads whose runtime paths call the
native instructions. Tags without the instruction features use `consteval`
overloads and still need complete storage: SSE2 for 128 bits, AVX for 256 bits,
or AVX512F for 512 bits, including the register prerequisites. Results retain
the exact tag. These overloads do not add a software fallback for runtime data.

The [fixture](../tests/x86_avx512cd/README.md) describes the shape checks,
constant-evaluation references, runtime admission and instruction probes.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
