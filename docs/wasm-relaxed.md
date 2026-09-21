# WebAssembly relaxed SIMD

Import `native.wasm.relaxed`, `native.wasm`, or `native` and link
`native::native`. The family exposes the 20 finalized relaxed SIMD operations
through 16-byte `native::simd<T,N,Arch>` values. Runtime calls require
`wasm_feature::relaxed_simd` in `Arch` and `target("relaxed-simd")` on the caller.
The feature closure includes `simd128`. An ISA tag describes requirements;
the embedding must admit the complete linked Wasm module before execution.

```cpp
#include <cstdint>
import native.wasm.relaxed;

constexpr auto arch = native::feature_closure(native::wasm_feature::relaxed_simd);
using floats = native::simd<float, 4, arch>;

[[gnu::target("relaxed-simd")]]
floats accumulate(floats a, floats b, floats c) {
  return native::f32x4_relaxed_madd(a, b, c);
}
```

Each name follows the corresponding Clang intrinsic with the `wasm_` prefix
removed. All operands share the same `Arch`. The public operations do not accept
raw `v128_t` operands.

| Operations | Operand and result lanes |
| --- | --- |
| `i8x16_relaxed_swizzle` | Two `uint8_t × 16` values; byte result |
| `i32x4_relaxed_trunc_f32x4`, `u32x4_relaxed_trunc_f32x4` | `float × 4` to signed/unsigned 32-bit lanes |
| `i32x4_relaxed_trunc_f64x2_zero`, `u32x4_relaxed_trunc_f64x2_zero` | `double × 2` to four signed/unsigned 32-bit lanes; upper two are zero |
| `f32x4_relaxed_madd`, `f32x4_relaxed_nmadd` | Three `float × 4` values |
| `f64x2_relaxed_madd`, `f64x2_relaxed_nmadd` | Three `double × 2` values |
| `i8x16_relaxed_laneselect`, `i16x8_relaxed_laneselect`, `i32x4_relaxed_laneselect`, `i64x2_relaxed_laneselect` | Three unsigned integer vectors of the named lane width; third operand is the mask |
| `f32x4_relaxed_min`, `f32x4_relaxed_max` | Two `float × 4` values |
| `f64x2_relaxed_min`, `f64x2_relaxed_max` | Two `double × 2` values |
| `i16x8_relaxed_q15mulr` | Two `int16_t × 8` values |
| `i16x8_relaxed_dot_i8x16_i7x16` | `int8_t × 16`, `uint8_t × 16`; `int16_t × 8` result |
| `i32x4_relaxed_dot_i8x16_i7x16_add` | Same byte inputs plus `int32_t × 4` accumulator/result |

Lane selection accepts integer masks so partial-bit masks retain their relaxed
semantics. Canonical masks can be converted with `mask_bits<U>(mask)`, where `U`
is the unsigned lane type. Zero and all-one mask lanes select the second and
first data operand respectively.

Constant evaluation chooses a documented deterministic member of each allowed
result set. Swizzle clears out-of-range indices. Truncation saturates and converts
NaNs to zero. Multiply-add is fused with nearest-even rounding and gradual
underflow. Negative multiply-add negates the first multiplicand. Lane selection
uses bit selection. Min/max uses a canonical quiet NaN and strict signed-zero
selection. Q15 multiplication saturates its overflow case. Dot products interpret
both byte inputs as signed, saturate each adjacent pair to 16 bits, and accumulate
the pairs modulo 32 bits for the add form.

A tag containing `simd128` without `relaxed_simd` can use these operations only
in constant evaluation. Constant results need not agree bit-for-bit with an
engine's relaxed instructions. They also do not implement the optional Wasm
*deterministic profile*, which chooses unfused multiply-add.

The [focused checks](../tests/wasm_relaxed/README.md) distinguish public-wrapper
validation from raw-engine conformance probes. Exact raw/public comparisons
check compiled Wasm bytecode, not engine JIT machine code or execution overhead.
Full-bit dot operands and partial 16-bit lane-selection masks expose
engine/specification disagreements
in tested runtimes. Seven-bit second operands avoid the dot ambiguity, and
canonical zero/all-one masks have exact lane-selection semantics.

CI reports library qualification and raw-engine conformance independently.
Raw probes use the same built modules and retain ordinary test failures.
The public wrappers inherit the documented engine deviations on affected inputs;
passing the library checks does not establish full runtime specification
conformance on those engines.

The semantic reference is the [WebAssembly core numerics specification](https://webassembly.github.io/spec/core/exec/numerics.html#relaxed-ops),
pinned for these checks at [revision ba9fd9f5](https://github.com/WebAssembly/spec/blob/ba9fd9f5c23e569201265d5bda6fb8dde18ad8c0/document/core/exec/numerics.rst).
The native mappings use [Clang's `wasm_simd128.h`](https://clang.llvm.org/doxygen/wasm__simd128_8h_source.html).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
