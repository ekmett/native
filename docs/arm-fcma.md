# ARM complex arithmetic

Import `native.arm.fcma`, `native.arm`, or `native` for
`fcadd<Arch,Rotation>`, `fcmla<Arch,Rotation>`, and
`fcmla_lane<Arch,Rotation,Lane>`. Source-tree header consumers can include
`<native/arm/fcma.h>`; installed consumers use the named modules. Inputs and
results are NEON registers, with adjacent elements holding `(real,imaginary)`.
The existing `native::simd` native-register bridges also accept these results
and supply their inputs.

| Register shape | Complex pairs | Required feature bits |
| --- | --- | --- |
| `float32x2_t` | 1 | `complxnum` |
| `float32x4_t` | 2 | `complxnum` |
| `float64x2_t` | 1 | `complxnum` |
| `float16x4_t` | 2 | `complxnum` and `neon_fp16` |
| `float16x8_t` | 4 | `complxnum` and `neon_fp16` |

The feature is FEAT_FCMA. Runtime admission adds the compiler prerequisites,
including baseline NEON. FP32/FP64 calls target exactly `"complxnum"`; FP16 calls
target `"complxnum,fullfp16"`. Neither FHM nor BF16 substitutes for FCMA, and an
FP16 feature alone is insufficient. Exact rejection overloads prevent Clang's
implicit conversions between same-size NEON types from bypassing a missing
feature or invalid immediate by selecting another element format.

For one pair `a=(ar,ai)` and `b=(br,bi)`, `fcadd` permits rotations 90 and 270:

| Rotation | Result |
| --- | --- |
| 90 | `(ar-bi, ai+br)` |
| 270 | `(ar+bi, ai-br)` |

`fcmla` performs one partial complex multiply-add per call. For accumulator
`c=(cr,ci)`, the component graph is:

| Rotation | Result |
| --- | --- |
| 0 | `(fma(ar,br,cr), fma(ar,bi,ci))` |
| 90 | `(fma(ai,-bi,cr), fma(ai,br,ci))` |
| 180 | `(fma(ar,-br,cr), fma(ar,-bi,ci))` |
| 270 | `(fma(ai,bi,cr), fma(ai,-br,ci))` |

Each component has one fused product and one rounding in its destination
format. A call at rotation 0 followed by one at rotation 90 accumulates a full
complex product, with two successive fused rounding stages. It is not an
infinitely precise complex product rounded once. The other real/imaginary
component of `a` is not multiplied in that individual call.

The lane forms cover the FP16 and FP32 register shapes above. `Lane` indexes
complex pairs in `b`, so a four-float source has two selectable pairs, and an
eight-half source has four. Source and destination register widths may differ
within each format. The chosen pair is broadcast to every destination pair.
FP32 two-element results use a selected 64-bit pair and the vector instruction;
other shapes use the indexed instruction. A 64-bit half result selecting either
upper pair of a 128-bit source first extracts the upper 64 bits. There is no
indexed FP64 overload. Rotations and pair indices are compile-time immediates.

Target and admit the caller; imports and template arguments do neither:

```cpp
#include <arm_neon.h>
#include <native/targets.h>
import native.arm.fcma;

#define NATIVE_TARGET_complex_float "complxnum"
constexpr auto complex_isa = NATIVE_TARGET_ISA(complex_float);

NATIVE_TARGET_PUSH(complex_float)
void complex_float(float* output, float const* a, float const* b) noexcept {
  auto va = vld1q_f32(a), vb = vld1q_f32(b);
  auto partial = native::fcmla<complex_isa, 0>(vdupq_n_f32(0), va, vb);
  auto result = native::fcmla<complex_isa, 90>(partial, va, vb);
  vst1q_f32(output, result);
}
NATIVE_TARGET_POP()

// Before calling complex_float, after satisfying the process minimum:
// auto cpu = native::observe_arm_capabilities();
// if(native::classify_isa(cpu, complex_isa, NATIVE_TARGET_MINIMUM).admitted())
//   complex_float(output, a, b);
```

FPCR remains unchanged. Rounding, subnormal handling, NaNs and signed zeros
follow the instruction and caller's controls; applicable exceptions accumulate
in FPSR. No software normalization, flag reset or environment installation
occurs. Hardware traps are not masked by `noexcept`. Surrounding code that
changes or observes the floating environment needs the compiler's corresponding
support.

Clang 23's ACLE wrappers and underlying builtins require the whole Armv8.3-A
bundle. The implementation instead uses the exact native instruction under its
independent target attribute. Volatile assembly preserves FPSR effects even
when the result is discarded, and a compiler memory barrier orders surrounding
memory-based environment operations. This barrier is not a CPU memory fence.
The operations carry neither `pure` nor `const`. A shared private helper handles
Clang's different 64-bit and 128-bit big-endian asm register coercions, including
bytes within each floating element.

`tests/arm_fcma` executes the header, granular module and main module on Apple M3
with Clang 23. It checks all five formats, every rotation and every indexed pair
against scalar arithmetic under all four rounding modes, plus real
`native::simd` round trips, unchanged FPCR, sticky FPSR and used/discarded invalid
results. FP32/FP64 use `std::fma`; the bounded dyadic half bank uses an exact
binary64 expression followed by one half conversion. NaN payload identity and
all possible FPCR/trap configurations are outside this qualification.

The Armv8-A assembly fixture verifies 102 vector/indexed forms and baseline
isolation. Real compiler-failure fixtures reject missing FCMA targets, missing
FP16 targets/features, and invalid FP32 lanes even when the supplied ISA also
includes FP16. Twenty-eight big-endian compiler memory mappings check byte and
pair selection symbolically, including ACLE vector controls; there is no
big-endian hardware execution claim. Standalone CMake consumers exercise the
installed granular and main modules.

See the [Arm Neon complex-operation reference](https://arm-software.github.io/acle/neon_intrinsics/advsimd.html#complex-operations-from-armv83-a)
and the [Arm Architecture Reference Manual](https://developer.arm.com/documentation/ddi0487/latest/).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
