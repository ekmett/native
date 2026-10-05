# ARM FCMA: complex arithmetic

[ARM instruction sets](arm.md)

## Why use it

Complex multiplication otherwise needs component shuffles, sign changes and
separate real arithmetic. FCMA works directly on adjacent `(real,imaginary)`
pairs. Two partial multiply-adds accumulate a full complex product, making the
operation useful in filters, Fourier transforms and other complex kernels.

## Operations

Import `native.arm.fcma`, or use the `native.arm` or `native` hub.
All operands use `native::simd<T,N,Arch>` with the same element type and `Arch`.

- `fcadd<Arch,Rotation>(a,b)` adds `b` rotated by 90 or 270 degrees.
- `fcmla<Arch,Rotation>(acc,a,b)` accumulates a partial complex product, with
  rotation 0, 90, 180 or 270.
- `fcmla_lane<Arch,Rotation,Lane>(acc,a,b)` selects one complex pair from `b`
  and broadcasts it to every destination pair.

The non-indexed operations accept `simd<float,2,Arch>`, `simd<float,4,Arch>`,
`simd<double,2,Arch>`, `simd<fp16,4,Arch>` and `simd<fp16,8,Arch>`.
Indexed operations accept the FP16 and FP32 shapes; source and destination
widths may differ within the same format. `Lane` indexes pairs, so four floats
contain two selectable pairs and eight halves contain four. There is no indexed
FP64 form. Rotations and pair indices are compile-time immediates.

For `a=(ar,ai)`, `b=(br,bi)` and `acc=(cr,ci)`:

| Operation | Rotation | Result |
| --- | --- | --- |
| `fcadd(a,b)` | 90 | `(ar-bi, ai+br)` |
| `fcadd(a,b)` | 270 | `(ar+bi, ai-br)` |
| `fcmla(acc,a,b)` | 0 | `(fma(ar,br,cr), fma(ar,bi,ci))` |
| `fcmla(acc,a,b)` | 90 | `(fma(ai,-bi,cr), fma(ai,br,ci))` |
| `fcmla(acc,a,b)` | 180 | `(fma(ar,-br,cr), fma(ar,-bi,ci))` |
| `fcmla(acc,a,b)` | 270 | `(fma(ai,bi,cr), fma(ai,-br,ci))` |

```cpp
#include <native/targets.h>
import native.arm.fcma;

#define NATIVE_TARGET_complex_float "complxnum"
constexpr auto complex_isa = NATIVE_TARGET_ISA(complex_float);

NATIVE_TARGET_PUSH(complex_float)
void complex_float(float* output, float const* a, float const* b) noexcept {
  using vector = native::simd<float,4,complex_isa>;
  auto va = vector::load_memory(a), vb = vector::load_memory(b);
  auto partial = native::fcmla<complex_isa,0>(vector(0.0f), va, vb);
  native::fcmla<complex_isa,90>(partial, va, vb).store_memory(output);
}
NATIVE_TARGET_POP()
```

## Caveats

Each `fcmla` component has one fused product and one destination-format
rounding. Rotation 0 followed by rotation 90 accumulates the full complex
product with two successive fused rounding stages. The unselected component
of `a` does not participate in an individual call.

Runtime calls require `arm_feature::complxnum` and a `"complxnum"` caller
target. FP16 calls additionally require `arm_feature::neon_fp16` and
`"complxnum,fullfp16"`. Admission includes NEON. Check the target requirement
and `NATIVE_TARGET_MINIMUM` before entering the function. FP16, FHM and BF16
do not imply FCMA; importing a module does not enable the target or dispatch.

Runtime arithmetic follows the caller's FPCR controls and accumulates applicable
exceptions in FPSR. The wrappers leave FPCR unchanged and retain instruction
execution even when the result is discarded. They do not mask hardware traps;
`noexcept` concerns C++ exceptions. Surrounding code that observes or changes
the environment still needs the compiler's floating-environment support.
The compiler memory barrier orders memory-based environment accesses and adds
no CPU memory fence.

Constant evaluation uses nearest-even rounding, gradual inputs and results,
payload-preserving NaNs, IEEE half precision and masked exceptions, with
DN=AH=AHP=FZ=FZ16=FIZ=EBF=0 and no machine status effects. Without FCMA,
`consteval`-only overloads remain available when the SIMD storage types exist.
They provide no runtime fallback.

For indexed FP32 two-element results, the implementation selects a 64-bit pair
and uses the vector instruction. An indexed 64-bit FP16 result selecting an
upper pair of a 128-bit source first extracts the upper 64 bits.

See the [Arm Neon complex-operation reference](https://arm-software.github.io/acle/neon_intrinsics/advsimd.html#complex-operations-from-armv83-a)
and [Arm Architecture Reference Manual](https://developer.arm.com/documentation/ddi0487/latest/).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
