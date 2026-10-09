# ARM DotProd: byte dot products

[ARM instruction sets](arm.md)

## Why use it

Byte dot products appear in quantized linear algebra and integer filters. DotProd
multiplies four pairs of bytes per accumulator lane and adds their sum directly
to a 32-bit accumulator. The indexed forms let several dot products share the
same four coefficients.

## Operations

Import `native.arm.dotprod`, or use the `native.arm` or `native` hub.
Operands and results use `native::simd<T,N,Arch>` with a common `Arch`.

| Operation | Accumulator/result | Byte operands | Right-hand selection |
| --- | --- | --- | --- |
| `sdot<Arch>(acc,a,b)` | `simd<std::int32_t,2,Arch>` or `simd<std::int32_t,4,Arch>` | Matching-width signed 8-bit vectors: 8 or 16 lanes | Corresponding groups of four bytes |
| `udot<Arch>(acc,a,b)` | `simd<std::uint32_t,2,Arch>` or `simd<std::uint32_t,4,Arch>` | Matching-width unsigned 8-bit vectors: 8 or 16 lanes | Corresponding groups of four bytes |
| `sdot_lane<Arch,Lane>(acc,a,b)` | Either signed accumulator | Matching-width `a`; 8 or 16 signed bytes in `b` | One four-byte group of `b` for every result lane |
| `udot_lane<Arch,Lane>(acc,a,b)` | Either unsigned accumulator | Matching-width `a`; 8 or 16 unsigned bytes in `b` | One four-byte group of `b` for every result lane |

For the ordinary forms, output lane `j` accumulates
`sum(a[4*j+k] * b[4*j+k], k=0..3)`. The indexed forms replace the second index
with `4*Lane+k`. `Lane` is a compile-time immediate: 0–1 for an eight-byte source,
0–3 for a sixteen-byte source.

```cpp
#include <cstdint>
#include <native/targets.h>
import native.arm.dotprod;

constexpr auto required = native::feature_closure(native::arm_feature::dotprod);

__attribute__((target("dotprod")))
native::simd<std::int32_t,4,required> accumulate(
    native::simd<std::int32_t,4,required> acc,
    native::simd<std::int8_t,16,required> a,
    native::simd<std::int8_t,16,required> b) {
  return native::sdot<required>(acc, a, b);
}

bool available() {
  return native::classify_isa(native::observe_arm_capabilities(),
    required, NATIVE_TARGET_MINIMUM).admitted();
}
```

## Caveats

Accumulation wraps modulo 2³². Signed overflow has the instruction's wrapping
result; it does not saturate or invoke C++ signed-overflow undefined behavior.

Native runtime calls require `arm_feature::dotprod` and a caller compiled for
`"dotprod"`. Check admission before entering the target function. DotProd adds
NEON as a prerequisite and can be requested independently of RDM, FP16 or I8MM.
An import does not enable the target or dispatch to a supported implementation.

All forms support constant evaluation with the same modular arithmetic. Without
DotProd, runtime calls require explicit `polyfill` permission. `neon | polyfill`
uses NEON storage; `polyfill` alone permits scalar storage. Without permission,
missing-feature calls remain `consteval`-only.

See the [Arm Advanced SIMD intrinsic reference](https://arm-software.github.io/acle/neon_intrinsics/advsimd.html#dot-product).
