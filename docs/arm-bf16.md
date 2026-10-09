# ARM BF16: dot products and matrix accumulation

[ARM instruction sets](arm.md)

## Why use it

BF16 keeps the eight-bit exponent of FP32 in a sixteen-bit representation,
trading fraction precision for smaller inputs. These instructions multiply
BF16 values and accumulate into FP32, giving neural-network and other dense
linear-algebra kernels a direct dot-product or small matrix-tile operation.

## Operations

Import `native.arm.bf16`, or use the `native.arm` or `native` hub.
All operands use `native::simd<T,N,Arch>` with a common `Arch`.

| Operation | Accumulator/result | Inputs | Selection |
| --- | --- | --- | --- |
| `bfdot<Arch>(acc,a,b)` | `simd<float,2,Arch>` or `simd<float,4,Arch>` | Matching-width `simd<bf16,4,Arch>` or `simd<bf16,8,Arch>` | Adjacent pairs |
| `bfdot_lane<Arch,Lane>(acc,a,b)` | Either dot shape | Matching-width `a`; either width for `b` | Pair `b[2*Lane]`, `b[2*Lane+1]` |
| `bfmmla<Arch>(acc,a,b)` | `simd<float,4,Arch>` | Two `simd<bf16,8,Arch>` vectors | 2×4 by 4×2 matrix product |
| `bfmlalb<Arch>(acc,a,b)` | `simd<float,4,Arch>` | Two `simd<bf16,8,Arch>` vectors | Even lanes of both inputs |
| `bfmlalt<Arch>(acc,a,b)` | `simd<float,4,Arch>` | Two `simd<bf16,8,Arch>` vectors | Odd lanes of both inputs |
| `bfmlalb_lane<Arch,Lane>(acc,a,b)` | `simd<float,4,Arch>` | Eight BF16 lanes in `a`; four or eight in `b` | Even lanes of `a`, scalar `b[Lane]` |
| `bfmlalt_lane<Arch,Lane>(acc,a,b)` | `simd<float,4,Arch>` | Eight BF16 lanes in `a`; four or eight in `b` | Odd lanes of `a`, scalar `b[Lane]` |

Dot-product indices select pairs: 0–1 for a four-element `b`, 0–3 for an
eight-element `b`. Widening multiply-add indices select elements: 0–3 or 0–7.
All lane indices are compile-time immediates.

For `bfmmla`, store the left matrix's rows and the right matrix's columns
consecutively. Output lane `2*r+c` starts with `acc[2*r+c]`, accumulates the
pair of products at inner indices 0 and 1, then the pair at indices 2 and 3.
Each step follows the `bfdot` arithmetic contract. A row-major 4×2 right matrix
needs rearrangement.

```cpp
#include <native/targets.h>
import native.arm.bf16;

#define NATIVE_TARGET_bfloat_dot "bf16"
constexpr auto dot_isa = NATIVE_TARGET_ISA(bfloat_dot);

NATIVE_TARGET_PUSH(bfloat_dot)
void accumulate(float* output, float const* acc,
                native::bf16 const* a, native::bf16 const* b) noexcept {
  using F = native::simd<float,4,dot_isa>;
  using B = native::simd<native::bf16,8,dot_isa>;
  native::bfdot<dot_isa>(F::load(acc), B::load(a), B::load(b)).store(output);
}
NATIVE_TARGET_POP()
```

## Caveats

Native runtime calls require `arm_feature::neon_bf16` and a `"bf16"` caller target.
Admission includes NEON. BF16 is independent of FP16, DotProd and I8MM.
Admit the target requirement and `NATIVE_TARGET_MINIMUM` before entering the
function. Importing the module does not enable instructions or perform dispatch.
The wrappers add no BF16 elementwise arithmetic or conversion policy.

With FEAT_EBF16 absent or FPCR.EBF clear, `bfdot` and `bfmmla` round each
product, pair sum and accumulator addition separately to odd: an inexact finite
result has its low bit set. Subnormal inputs and tiny results flush to signed
zero, independently of RMode, FZ and FIZ. Overflow produces signed infinity.
NaNs become default NaNs; AH affects their sign.

With FEAT_EBF16 present and FPCR.EBF set, each pair is a fused sum of two
products, rounded once to FP32 before a separate accumulator addition. RMode,
FZ, AH and FIZ control single-precision behavior. Matrix multiplication still
performs two ordered pair accumulations per output. Neither mode is equivalent
to a chain of ordinary FP32 fused multiply-adds.

In both modes, dot and matrix instructions ignore exception enables and leave
cumulative FPSR flags unchanged. They do not write FPCR. Check `arm_feature::ebf16`
before setting EBF; the feature tag does not set that control. Clang 23 has no
standalone `"ebf16"` target string; enhanced arithmetic uses the same BF16
instructions.

`bfmlalb` and `bfmlalt` each compute one fused multiply-add per output, rounded
to FP32. With AH clear, they follow ordinary FP32 rounding, flushing and NaN
controls and accumulate exception flags. With FEAT_AFP and AH set, they force
nearest-even rounding and input/output flushing and suppress exceptions.
EBF does not change these operations. `noexcept` does not mask hardware traps.

The wrappers retain FPCR-sensitive instruction execution and FPSR effects even
when results are discarded. A compiler memory barrier orders surrounding
environment accesses and adds no CPU memory fence. Surrounding code still
needs the compiler's floating-environment support when it observes or changes
controls.

Constant evaluation fixes nearest-even rounding, gradual inputs and results,
payload-preserving NaNs, IEEE half precision and masked exceptions, with
DN=AH=AHP=FZ=FZ16=FIZ=EBF=0 and no machine status effects. The fixed BFDOT/BFMMLA
rules override this: round-to-odd steps, flushing, infinity on overflow and
positive default NaNs. BFMLALB/T use the ordinary AH=0 fused result, preserving
NaN payloads and signs. A tag without BF16 and without `polyfill` permits only `consteval` calls when
the SIMD storage types exist. Explicit `polyfill` permission enables runtime
software evaluation under the same FPCR result policy; available native BF16
instructions retain priority. Hardware admission remains a separate requirement.

See Arm's [BF16 instruction overview](https://developer.arm.com/community/arm-community-blogs/b/ai-blog/posts/bfloat16-processing-for-neural-networks-on-armv8_2d00_a),
[SME supplement, B3.1.2 and E2.2](https://documentation-service.arm.com/static/62015c6c965f7d118e3f5f4c),
[BFMMLA and BFMLAL shared pseudocode](https://www.scs.stanford.edu/~zyedidia/arm64/shared_pseudocode.html)
and [ACLE BF16 types](https://arm-software.github.io/acle/main/acle.html#16-bit-brain-floating-point-arithmetic-scalar-intrinsics).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
