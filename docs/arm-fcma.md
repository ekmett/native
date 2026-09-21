# ARM complex arithmetic

FCMA operates on complex numbers stored as adjacent `(real,imaginary)`
lanes in `native::simd<T, N, Arch>`. Import `native.arm.fcma`, `native.arm`, or `native`
for `fcadd<Arch,Rotation>`, `fcmla<Arch,Rotation>`, and
`fcmla_lane<Arch,Rotation,Lane>`. All operands and results share their element
type and `Arch`. Raw NEON registers are private implementation details.

| Vector shape | Complex pairs | Required feature bits |
| --- | --- | --- |
| `simd<float, 2, Arch>` | 1 | `complxnum` |
| `simd<float, 4, Arch>` | 2 | `complxnum` |
| `simd<double, 2, Arch>` | 1 | `complxnum` |
| `simd<fp16, 4, Arch>` | 2 | `complxnum` and `neon_fp16` |
| `simd<fp16, 8, Arch>` | 4 | `complxnum` and `neon_fp16` |

The feature is FEAT_FCMA. Runtime admission adds the compiler prerequisites,
including baseline NEON. FP32/FP64 calls target `"complxnum"`; FP16 calls target
`"complxnum,fullfp16"`. FHM, BF16 and FP16 alone do not supply FCMA. Runtime calls with missing features, incompatible element types or architectures, raw register arguments,
and invalid immediates are rejected at compile time.


Constant evaluation uses a fixed floating-point environment: nearest-even
rounding, gradual inputs and results, payload-preserving NaNs, standard IEEE
half precision, and masked exceptions. FPCR controls DN, AH, AHP, FZ, FZ16, FIZ,
and EBF are zero. No status flags, traps, or control-register accesses occur.
An architecture tag lacking the instruction feature admits a `consteval`-only
overload when every operand/result storage type is complete; runtime inputs
remain compile-time errors. Lane, rotation, element-type and architecture
requirements still apply. With the feature present, the same function is
`constexpr` and its runtime branch executes the native instruction.

For one pair `a=(ar,ai)` and `b=(br,bi)`, `fcadd` permits rotations 90 and 270:

| Rotation | Result |
| --- | --- |
| 90 | `(ar-bi, ai+br)` |
| 270 | `(ar+bi, ai-br)` |

`fcmla` performs one partial complex multiply-add per call. For accumulator
`c=(cr,ci)`, the components are computed as follows:

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

Compile the caller for its target and check CPU support before entering it.
An import or template argument alone does neither:

```cpp
#include <native/targets.h>
import native.arm.fcma;

#define NATIVE_TARGET_complex_float "complxnum"
constexpr auto complex_isa = NATIVE_TARGET_ISA(complex_float);

NATIVE_TARGET_PUSH(complex_float)
void complex_float(float* output, float const* a, float const* b) noexcept {
  using vector = native::simd<float, 4, complex_isa>;
  auto va = vector::load_memory(a), vb = vector::load_memory(b);
  auto partial = native::fcmla<complex_isa, 0>(vector(0.0f), va, vb);
  native::fcmla<complex_isa, 90>(partial, va, vb).store_memory(output);
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

`tests/arm_fcma` runs through the granular module and main module on
Apple M3 with Clang 23. It compares all five formats, every rotation and every
indexed pair with scalar arithmetic under all four rounding modes. It also
checks exact public `simd` types, unchanged FPCR, sticky FPSR, and
invalid-operation effects with used and discarded results. The FP32/FP64
reference uses `std::fma`; the half-precision cases use bounded dyadic values
whose binary64 expressions are exact, followed by one half conversion. These
tests do not establish NaN payload identity or cover every FPCR setting and
hardware trap configuration.

The Armv8-A assembly fixture verifies 102 vector/indexed forms and baseline
isolation. Compilation must fail for missing FCMA targets, missing FP16
targets or features, and invalid FP32 lanes even when the supplied ISA also
includes FP16. Twenty-eight big-endian compiler memory mappings check byte and
pair selection symbolically, including ACLE vector controls. Native big-endian
execution remains untested. Standalone CMake consumers exercise the
installed granular and main modules.

Paired assembly checks compare the public `simd` call with its private native
helper under identical target attributes and register signatures. The complete
instruction sequences must match, including moves, loads, stores and calls.
This checks abstraction overhead in the tested leaf contexts; it is not a
benchmark or a guarantee about surrounding application code.

See the [Arm Neon complex-operation reference](https://arm-software.github.io/acle/neon_intrinsics/advsimd.html#complex-operations-from-armv83-a)
and the [Arm Architecture Reference Manual](https://developer.arm.com/documentation/ddi0487/latest/).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
