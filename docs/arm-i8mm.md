# ARM I8MM: byte matrix products and mixed-sign dot products

[ARM instruction sets](arm.md)

## Why use it

Small matrix tiles let a quantized matrix kernel reuse each byte across several
outputs. I8MM accumulates a 2×8 by 8×2 product into four 32-bit lanes in one
operation. It also supplies unsigned-by-signed dot products, useful when
activations and weights have different signedness.

## Operations

Import `native.arm.i8mm`, or use the `native.arm` or `native` hub.
All vector arguments and results use `native::simd<T,N,Arch>` with a common `Arch`.

| Operation | Accumulator/result | Inputs |
| --- | --- | --- |
| `smmla<Arch>(acc,a,b)` | `simd<std::int32_t,4,Arch>` | Two `simd<std::int8_t,16,Arch>` vectors |
| `ummla<Arch>(acc,a,b)` | `simd<std::uint32_t,4,Arch>` | Two `simd<std::uint8_t,16,Arch>` vectors |
| `usmmla<Arch>(acc,a,b)` | `simd<std::int32_t,4,Arch>` | Unsigned bytes in `a`, signed bytes in `b`; 16 lanes each |
| `usdot<Arch>(acc,a,b)` | `simd<std::int32_t,2,Arch>` or `simd<std::int32_t,4,Arch>` | Unsigned `a`, signed `b`; 8 or 16 bytes to match the accumulator width |
| `usdot_lane<Arch,Lane>(acc,a,b)` | Either dot shape | Unsigned `a`, signed `b`; matching-width `a`, either width for `b` |
| `sudot_lane<Arch,Lane>(acc,a,b)` | Either dot shape | Signed `a`, unsigned `b`; matching-width `a`, either width for `b` |

For matrix multiplication, store the rows of `a` consecutively and the columns
of `b` consecutively. Output lane `2*r+c` accumulates
`sum(a[8*r+k] * b[8*c+k], k=0..7)`. A row-major 8×2 right matrix needs
rearrangement before the call.

`usdot` accumulates corresponding groups of four bytes. In an indexed form,
output lane `j` accumulates `sum(a[4*j+k] * b[4*Lane+k], k=0..3)`.
`Lane` is an immediate in 0–1 for an eight-byte `b`, or 0–3 for a sixteen-byte
`b`. Exchanging the operands of `usdot_lane` changes which operand supplies the
selected group; use `sudot_lane` for the opposite signedness.

```cpp
#include <cstdint>
#include <native/targets.h>
import native.arm.i8mm;

constexpr auto matrix_isa = native::feature_closure(native::arm_feature::i8mm);

__attribute__((target("i8mm")))
native::simd<std::int32_t,4,matrix_isa> multiply(
    native::simd<std::int32_t,4,matrix_isa> acc,
    native::simd<std::int8_t,16,matrix_isa> a,
    native::simd<std::int8_t,16,matrix_isa> b) noexcept {
  return native::smmla<matrix_isa>(acc, a, b);
}
```

## Caveats

Every accumulation wraps modulo 2³². Signed results interpret the resulting
bits as two's complement. There is no saturation or floating-point environment
dependency.

Native runtime calls require `arm_feature::i8mm` and an `"i8mm"` caller target. Before
calling a target function, admit its requirements with
`classify_isa(observe_arm_capabilities(), matrix_isa, NATIVE_TARGET_MINIMUM)`.
I8MM requires NEON and is independent of DotProd, FP16 and BF16. Importing the
module does not enable instructions or perform dispatch.

All forms support constant evaluation. Without I8MM, `polyfill` permission
allows the same matrix or dot-product operation in software. Without that
permission, missing-feature calls remain `consteval`-only. Hardware-capable
tags keep the native instruction path even when permission is present.

See the [Arm Advanced SIMD intrinsic reference](https://arm-software.github.io/acle/neon_intrinsics/advsimd.html#matrix-multiplication-intrinsics-from-armv86-a).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
