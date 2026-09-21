# SIMD128 qualification

The C++26 named-module fixture has four CTest entries:

- `native.wasm.simd`: constant assertions plus seeded runtime scalar comparisons
  across all ten numeric shapes, mask comparisons, bounded memory, conversions,
  fixed-point corners and floating signed-zero/NaN cases. The seed incorporates
  a volatile input so runtime cases cannot become constant-only tests.
- `native.wasm.simd.codegen`: 294 public/intrinsic pairs with identical external
  register/scalar signatures, compiled at `-O2`. Every instruction and operand
  must match and calls are rejected. The JSON output retains both bodies.
- `native.wasm.simd.clients`: constant and runtime `wide<simd<float,4,A>,2>`
  arithmetic, empty packs, and compile-time checks that the unimplemented SIMD128
  exponential does not participate.
- `native.wasm.simd.api`: integer construction and scalar operator participation
  across every integer lane and scalar width, rejection of floating, Boolean,
  enum and user-converted inputs, constant/runtime wrapping checks, and retained
  floating-vector construction behavior.

All sources import the public modules. Provider and consumer baseline flags do
not enable SIMD128 globally; each vector leaf declares its target. Node validates
final linked modules before WASI instantiation without optional engine flags.
The same fixture can be configured with `find_package(native)` after installation.

## Primitive instruction coverage

| Class | Paired cases |
|---|---|
| Constants and transfer | `v128.const`, load/store, all splats, all lane extracts/replacements |
| Partial loads/stores | all four load/store lane widths, both load-zero widths, signed/unsigned widening loads |
| Integer arithmetic | add/sub/neg/abs, all native multiply widths, all scalar-count shifts |
| Integer comparisons | every signed/unsigned native comparison, including signed i64 comparisons |
| Saturation | byte/halfword add/sub, unsigned average, both signed/unsigned narrowing widths |
| Widening | signed/unsigned low/high extensions and extended multiplication at every native width |
| Pairwise/fixed point | signed/unsigned pairwise extension sums, signed halfword dot, saturating Q15 multiply |
| Bit operations | and/or/xor/not/and-not/bitselect, byte popcount, all-true/any-true/bitmask |
| Rearrangement | byte shuffle and dynamic byte swizzle |
| Floating arithmetic | f32/f64 add/sub/mul/div/neg/abs/sqrt, min/max/pmin/pmax, ceil/floor/trunc/nearest |
| Conversion | signed/unsigned f32/f64 saturating truncation, signed/unsigned integer conversion, promote/demote |
| Canonical masks | all/any/bitset and lane selection for all four lane widths |

Typed scalar memory functions require natural scalar alignment. Their raw
intrinsic controls make the same alignment promise explicitly; the intrinsic
`void *` signatures alone only promise byte alignment. Full vector load/store
controls preserve the public unaligned contract. Mask controls assume canonical
zero/all-one inputs, matching the implementation bridge precondition.

## Composed operations

Unsigned i64 comparison/min/max, signed i64 min/max, wider-lane popcount,
32-bit unsigned pairwise sums, reductions and bounded-prefix transfers are
compositions. Truncating unsigned narrowing and constant-index permutation use
shuffle instructions. Widened multiply is written as extension followed by
multiplication; every native case is required to fuse to the matching extmul
instruction in the paired test. Bit-select and scalar lane transfers similarly
match their direct intrinsic instruction streams.

There is no SIMD128 byte multiply, vector integer divide or fused multiply-add
instruction. The range-reduced exponential graph is unavailable for these SIMD
values. Relaxed SIMD has its own module and qualification fixture.
