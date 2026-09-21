# ARM FP16 widening fused multiply-add

`fmlal<Arch>(acc,a,b)` adds binary16 products directly to binary32 accumulators;
`fmlsl` subtracts them. Import `native.arm.fp16fml`, `native.arm`, or `native`
for these operations, their upper-half forms `fmlal2` and `fmlsl2`, and the
corresponding `_lane` forms. Source-tree header consumers can include
`<native/arm/fp16fml.h>`; installed consumers use the named modules.

Each overload accepts and returns raw NEON registers. Use the `native::simd`
native-register conversions with types such as `simd<fp16,8,neon_fp16>` for
inputs and `simd<float,4,neon>` for results.

For a result with `N` lanes, the inputs have `2*N` half lanes. The unsuffixed
instructions select lanes `[0,N)`, and the `2` variants select `[N,2*N)`. Each
result component is one fused operation with one binary32 rounding. There is no intermediate binary16 product or binary32
multiply rounding. These are FEAT_FHM operations; ordinary FP16 arithmetic and
BF16 arithmetic have different feature requirements and numerical contracts.

| Operation shape | Accumulator/result | Multiplicands | Selected lanes |
| --- | --- | --- | --- |
| `fmlal`, `fmlsl` | `float32x2_t` | `float16x4_t` | 0–1 |
| `fmlal2`, `fmlsl2` | `float32x2_t` | `float16x4_t` | 2–3 |
| `fmlal`, `fmlsl` | `float32x4_t` | `float16x8_t` | 0–3 |
| `fmlal2`, `fmlsl2` | `float32x4_t` | `float16x8_t` | 4–7 |

Every `_lane<Arch,Lane>(acc,a,b)` form selects the same lanes of `a` and broadcasts
one scalar half from `b`. The source `b` may have four or eight half lanes;
`Lane` is an immediate less than four or eight respectively. Invalid lane
indices are rejected at compile time. The widening result has the same
binary32 shape as `acc`.

The API requires `Arch.has(arm_feature::fp16fml)` and a `"fp16fml"` compiler
target. Runtime admission includes the compiler prerequisites NEON and FP16.
The `neon_fp16` preset, BF16 and FCMA do not supply the FHM feature. Compile the
caller for the matching target and check its requirements before entering it;
importing the module does neither:

```cpp
#include <arm_neon.h>
#include <native/targets.h>
import native.arm.fp16fml;

#define NATIVE_TARGET_widen "fp16fml"
constexpr auto widen_isa = NATIVE_TARGET_ISA(widen);

NATIVE_TARGET_PUSH(widen)
void widen(float* output, __fp16 const* a, __fp16 const* b) noexcept {
  auto result = native::fmlal<widen_isa>(vdupq_n_f32(0), vld1q_f16(a), vld1q_f16(b));
  vst1q_f32(output, result);
}
NATIVE_TARGET_POP()

// Before calling widen, after satisfying the configured process minimum:
// auto cpu = native::observe_arm_capabilities();
// if(native::classify_isa(cpu, widen_isa, NATIVE_TARGET_MINIMUM).admitted())
//   widen(output, a, b);
```

The instructions retain their architectural floating-point behavior: FPCR is
read and left unchanged, and applicable exception flags accumulate in FPSR.
The wrappers do not install a rounding mode, flush subnormals in software,
canonicalize NaNs, clear flags, or save/restore the environment. Signed zero,
NaNs, infinities and subnormal handling follow the instruction and caller's
controls. An enabled hardware exception is not a C++ exception suppressed by
`noexcept`. Use the compiler's floating-environment support when surrounding
code changes or observes that environment.

Clang 23 can discard an unused ACLE FHM result even with floating-environment
access enabled. These wrappers therefore use volatile native instructions and
a compiler memory barrier, preserving instruction execution and FPSR effects
when the result is discarded. They have no `pure` or `const` annotation. The
barrier also orders surrounding memory-based floating-environment operations;
it is not a general CPU memory fence. The implementation normalizes Clang's
big-endian register coercion separately for 64-bit and 128-bit vectors.

`tests/arm_fp16fml` checks the header, granular and main modules, conversions
through `native::simd`, missing or unobserved features, and invalid lanes.
Native execution on Apple M3 with Clang 23 checks all vector and indexed shapes
against scalar `std::fma`, all four rounding modes, signed zeros, a subnormal
input, unchanged FPCR, sticky flags, and invalid-operation effects with both
used and discarded results. The scalar reference cases assume ordinary gradual
underflow; they do not exhaust FPCR settings, NaN behavior or hardware traps.
The assembly checks compile from Armv8-A, verify sixteen instruction forms and
baseline isolation, and reject a caller missing the target attribute.
Thirty-two big-endian cross-compiled memory mappings verify input bytes, lane
selection and stored results symbolically, with ACLE vector controls. They do
not execute big-endian hardware. Standalone CMake consumers exercise the
installed granular and main modules.

The API follows the FHM entries in the
[Arm Neon Intrinsics Reference](https://arm-software.github.io/acle/neon_intrinsics/advsimd.html#fp16-armv84-a)
and the floating-point instruction semantics in the
[Arm Architecture Reference Manual](https://developer.arm.com/documentation/ddi0487/latest/).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
