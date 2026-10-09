# WebAssembly SIMD128: vector operations

[WebAssembly instruction sets](wasm.md)

## Why use it

SIMD128 processes sixteen bytes at a time without tying the program to an x86
or ARM register type. Use it for bulk arithmetic, image and audio processing,
byte classification, and the load/shuffle/convert work around those kernels.
The engine chooses how to lower each Wasm operation to its host instructions.

## Operations

On WebAssembly compiler targets, `import native.simd;`, `import native.wasm;`
and `import native;` provide complete 16-byte vectors of signed/unsigned 8-,
16-, 32- and 64-bit integers, `float` and `double`. The architecture tag must
contain `wasm_feature::simd128`. Comparison masks are canonical zero/all-one
lanes of the same width as their corresponding values.

```cpp
#include <cstdint>
import native;
constexpr auto requirement = native::feature_closure(native::wasm_feature::simd128);
using words = native::simd<std::uint32_t, 4, requirement>;

__attribute__((target("simd128")))
words sum(words a, words b) { return a + b; }
```

| Family | Public operations |
|---|---|
| Transfer and lanes | constructors, `load`, `store`, `get<I>`, `replace<I>`, `load_splat`, `load_zero`, `load_widened`, `load_lane<I>`, `store_lane<I>` |
| Bounded memory | `load_partial`, `store_partial`, `load_simd_partial`, `store_simd_partial`; zero-count operations accept null pointers |
| Arithmetic | `+`, `-`, multiplication for lanes of at least 16 bits, floating `/`; integer results wrap |
| Saturation and fixed point | `add_sat`, `sub_sat`, `average_round`, `q15mulr_sat`, signed-halfword `dot` |
| Widening and narrowing | `extend_low`, `extend_high`, `multiply_widened_low`, `multiply_widened_high`, `pairwise_add_widened`, `narrow_sat`, `narrow_concat` |
| Comparisons and bits | comparisons, `&`, `|`, `^`, `~`, `select`, `bit_select`, `mask_bits`, `bitmask`, `any`, `all`, `popcount`, `reduce_add_widened` |
| Shifts and rearrangement | scalar-count shifts, `imm<K>` shifts, `broadcast`, `shuffle<I...>`, byte `swizzle` |
| Floating point | `abs`, `sqrt`, `floor`, `ceil`, `trunc`, `round_even`, `min`, `max`, `pmin`, `pmax` |
| Conversion | `convert<To>` and `trunc_sat<To>` |

`wide<simd<T,N,A>,R>` batches several registers, including empty packs.
Import `native.math` for the [math kernels](transcendentals.md), including
`exp`, `exp2`, `log`, `log2`, `log1p`, `expm1`, `tanh`, `atan2`, and `sincos`.

### Explicit emulation permission

`polyfill` alone supplies scalar storage and the deterministic SIMD128 semantic
API without requiring engine SIMD admission. `simd128 | polyfill` retains
native SIMD128 shapes and decomposes other logical shapes into those registers,
with an explicit partial tail. Existing native overloads keep priority.
Saturation, integer absolute value, averages, minimum/maximum, widening,
permutations, conversions and bounded memory helpers accept permitted storage.
Logical widening selects the global lower or upper half; narrowing concatenates
the entire first input before the second. Fixed SIMD128 floating-width
conversions retain their native result shape and zero-fill rules.

Native SIMD128 byte multiplication with permission widens into halfword products and retains
the low byte of each product. Explicitly permitted fused floating arithmetic
uses the scalar fused operation; polynomial math graphs retain their existing
noncontracting SIMD128 evaluation. Floating minimum and maximum propagate NaNs
and preserve WebAssembly's signed-zero rules; pseudo minimum/maximum retain the
first operand on equality or unordered comparison. Shift counts retain
WebAssembly's reduction modulo the lane width, including decomposed storage.
These rules do not alter calls that omit permission.

## Caveats

The provider remains at the configured baseline. SIMD operations carry a
`simd128` function target. The caller must enable the instructions it uses;
an architecture tag records requirements and does not retarget the caller.
The baseline `simd<T,N>` default belongs to the module provider; use explicit
tags for optional kernels. Raw `v128_t` is an implementation bridge and is not
an implicit vector conversion or an instruction-family operand type.

Shift counts are reduced modulo the lane width, including immediate counts.
`shuffle` takes exactly one output register's lane indices from two concatenated
inputs. Byte `swizzle` produces zero for every index above 15.

Integer broadcasts and scalar arithmetic, bitwise and comparison operands accept
the fixed-width integer element types. Conversion retains the low lane-width
bits; floating, Boolean, enum and user-converted scalar inputs do not participate.
Integer lane constructors apply the same conversion to each argument.

Saturating arithmetic and rounded averaging operate on byte and halfword lanes.
`narrow_sat<To>` consumes signed source lanes, including when the destination
is unsigned; `narrow_concat<To>` truncates unsigned lanes instead. Widening
preserves signedness. Q15 multiplication rounds the complete product and then
saturates; the minimum-times-minimum case produces 32767. Signed halfword dot
products sum adjacent pairs modulo 2^32.

`convert<float>` accepts signed/unsigned 32-bit integers or two doubles;
double demotion zeroes the upper two float lanes. `convert<double>` consumes
the low two float or 32-bit integer lanes. `trunc_sat<int32_t/uint32_t>`
truncates toward zero, clamps overflow and maps NaN to zero; double inputs
produce four integer lanes with the upper two zero.

There is no SIMD128 byte-multiply, vector integer-division, or fused floating
multiply-add instruction. No such instruction is claimed by the baseline API.
Unsigned 64-bit comparisons/min/max and reductions use explicit compositions.
The integer popcount operation composes byte counts for wider lanes.
WebAssembly uses nearest-even arithmetic, gradual underflow and no observable
host floating-point control/status register. All supported value operations
have constant evaluation, using the shared IEEE binary-format implementation
for floating arithmetic, square root, rounding and width conversion. A constant
result chooses a permitted NaN representation; runtime NaN payloads need not
match across engines or constant evaluation. Integer values and finite exact
results have their stated lane semantics.

Floating `min`/`max` propagate NaNs and distinguish signed zeros: minimum of
opposite zeros is negative zero and maximum is positive zero. `pmin` selects
`b` when `b < a`, otherwise `a`; `pmax` similarly uses `b > a`. Their unordered
and equal cases therefore preserve the first operand. `abs` clears only the
sign bit.

A WebAssembly engine validates the complete linked module. An uncalled function
or runtime branch cannot hide unsupported instructions. Applications own
separate baseline, SIMD128 and relaxed-SIMD compilation/loading decisions.
Importing a module exposes templates and does not instantiate optional opcodes.
The [capability observer](wasm-features.md) admits features for the selected
engine configuration; loading still validates the entire final module.

SIMD128 math uses separately rounded multiply/add stages, even when relaxed
SIMD is available. Results can therefore differ from x86 and ARM's fused
kernels. Constant evaluation follows the same Wasm evaluation order.
The API provides no general-purpose SIMD128 FMA or exponent-scaling instruction.

See [building for WebAssembly](../doc/building.md#webassembly) for the toolchain.
