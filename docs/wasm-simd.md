# WebAssembly SIMD values

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

The provider remains at the configured baseline. SIMD operations carry a
`simd128` function target. The caller must enable the instructions it uses;
an architecture tag records requirements and does not retarget the caller.
The baseline `simd<T,N>` default belongs to the module provider; use explicit
tags for optional kernels. Raw `v128_t` is an implementation bridge and is not
an implicit vector conversion or an instruction-family operand type.

## Operations

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
`wide<simd<T,N,A>,R>` construction and element arithmetic use the SIMD128 target
scope, including empty packs. The `native::math::exp` approximation is currently
unavailable for SIMD128 values and packs: its backend FMA and exponent-scaling
graph has not been implemented and numerically qualified. No scalar or library
call fallback is selected for that API.

## Floating-point and constant semantics

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

## Compilation and engine admission

A WebAssembly engine validates the complete linked module. An uncalled function
or runtime branch cannot hide unsupported instructions. Applications own
separate baseline, SIMD128 and relaxed-SIMD compilation/loading decisions.
Importing a module exposes templates and does not instantiate optional opcodes.
The [capability observer](wasm-features.md) admits features for the selected
engine configuration; loading still validates the entire final module.

The C++26 named-module build requires the same structured-binding-pack and
property checks as native targets. With WASI SDK 34, CMake 4.4 and Ninja:

```sh
cmake -S . -B build-wasm -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$WASI_SDK_PATH/share/cmake/wasi-sdk-p1.cmake" \
  -DNATIVE_TEST_ISA=WASM_SIMD128 -DNATIVE_PROFILES=WASM_SIMD128 \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build-wasm
ctest --test-dir build-wasm --output-on-failure
```

The SIMD128 tests compile constant assertions, execute seeded integer,
conversion, permutation, bounded-memory and floating-edge cases through Node's
WASI preview1 runtime, and validate the final module before instantiation. The
wide client checks empty and nonempty packs. Paired codegen tests compare typed
operations against intrinsic leaves at the same ABI; the [coverage inventory](../tests/wasm_simd/README.md)
records primitive families and composed operations. These comparisons check
compiled Wasm bytecode, not engine JIT machine code or execution overhead.
Tests do not enable engine feature flags.
