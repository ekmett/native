# Indexed x86 memory

Import `native.x86.memory`, or the `native` hub, for AVX2 gathers and AVX512
masked gathers and scatters. Vector operands and results are `simd` values.
The architecture is deduced from those values. Pointers retain their ordinary
C++ types, and integer data may be signed or unsigned without changing bits.

```cpp
import native.x86.memory;

constexpr auto a = native::target_features<native::x86>("avx2");
using indices = native::simd<std::int32_t, 4, a>;

// Inside an admitted AVX2 target scope, load four floats at signed byte offsets.
auto values = native::vgatherdps<4, 4>(base, indices(offsets));
```

The first template argument is the address scale in bytes: `1`, `2`, `4` or
`8`. Plain gathers also require the result lane count as their second template
argument. That count distinguishes forms such as a four-dword index register
loading either two or four doubles. Masked operations deduce the result shape
from their source or value, so they take only the scale.

| Data | 32-bit indices | 64-bit indices |
| --- | --- | --- |
| `float` | `vgatherdps` | `vgatherqps` |
| `double` | `vgatherdpd` | `vgatherqpd` |
| `int32_t` / `uint32_t` | `vpgatherdd` | `vpgatherqd` |
| `int64_t` / `uint64_t` | `vpgatherdq` | `vpgatherqq` |

AVX2 supplies the plain gather names and their `mask_` forms. A masked call is
`mask_vgatherdps<Scale>(source, mask, base, indices)`, with the corresponding
name for other data. AVX2 masks are signed integer `simd` values with the same
lane width and count as the data. Only each mask lane's sign bit is consulted;
positive nonzero values do not enable a load. The full mask is passed directly
to the instruction.

AVX512 masked gathers use the same names and argument order, but take
`predicate<N, Arch>` masks. Masked stores replace `gather` with `scatter`:
`mask_vscatterdps<Scale>(base, mask, indices, value)`, for example. Scatters
require AVX512F. Short EVEX forms also require AVX512VL; full EVEX forms require
AVX512F alone. AVX2 gather forms require AVX2. CPU features and OS-enabled
vector state must be admitted before entering the matching compiler target.

| Index bits / data bits | Short forms: index lanes → data lanes | Full EVEX form |
| --- | --- | --- |
| 32 / 32 | 4 → 4; 8 → 8 | 16 → 16 |
| 32 / 64 | 4 → 2; 4 → 4 | 8 → 8 |
| 64 / 32 | 2 → 4; 4 → 4 | 8 → 8 |
| 64 / 64 | 2 → 2; 4 → 4 | 8 → 8 |

These are hardware register shapes. In a two-index/four-data-lane gather, the
upper two result lanes are zero, including merging gathers. The corresponding
scatter ignores the upper two data and mask lanes. A four-index/two-data-lane
operation ignores the upper two indices. No other shape is synthesized.

The effective address is the base plus the signed index multiplied by the
byte scale. Runtime accesses may be unaligned. Every active address must permit
a complete data-element access. Inactive lanes do not access memory, so a null
base is valid with an empty mask. Inactive addressable gather lanes retain the
source value. These operations do not provide bounds checks, atomic access or
an all-or-nothing transaction when an active address faults.

Overlapping scatter writes are ordered from lower to higher lane. The highest
active lane wins when addresses coincide; partially overlapping writes obey
the same order. Hardware may omit an earlier store completely overwritten by
a later one. These semantics follow the gather/scatter chapters in the
[Intel instruction reference](https://cdrdv2-public.intel.com/671200/325462-sdm-vol-1-2abcd-3abcd.pdf).

Constant evaluation uses the same public functions and typed arrays. It
supports negative offsets within an array, masks, ignored lanes and duplicate
scatter destinations. Offsets must resolve to complete `T` elements because
constant evaluation cannot reproduce arbitrary byte addressing through a
`T *`. Non-element-aligned offsets, overflowing element offsets and active
out-of-bounds accesses are rejected. Weaker ISA tags retain constant-evaluation
only overloads when their SIMD storage is available. Calling those overloads
with runtime values is ill-formed; they never introduce scalar runtime gathers
or scatters.

The fixture compares 480 public-wrapper instruction bodies with the matching
raw intrinsic, covering every hardware shape, byte scale and signed/unsigned
data type under minimal and broad caller feature sets. It separately checks semantic constant evaluation, runtime memory
results, protected inactive addresses, unaligned accesses and overlapping
stores, and rejects invalid shapes, masks, scales, runtime feature tags and
target scopes. See the [fixture](../tests/x86_memory/README.md).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
