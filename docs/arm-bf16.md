# ARM BF16 dot products and matrix operations

BF16 stores a sign, an eight-bit exponent and seven fraction bits in sixteen
bits. These operations multiply BF16 inputs and accumulate into FP32 registers.
Import `native.arm.bf16`, `native.arm`, or `native` to use them. Each module reexports `native.simd`; vector arguments and results use
`simd<T,N,Arch>` with the same feature tag. The implementation headers contain
private raw helpers and are not a standalone SIMD interface.

| Operation | Accumulator/result | Inputs | Selection |
| --- | --- | --- | --- |
| `bfdot<Arch>(acc,a,b)` | `simd<float,2,Arch>` or `simd<float,4,Arch>` | Matching-width `simd<bf16,4,Arch>` or `simd<bf16,8,Arch>` | Adjacent pairs |
| `bfdot_lane<Arch,Lane>(acc,a,b)` | Either dot shape | Matching-width `a`; either width for `b` | Pair `b[2*Lane]`, `b[2*Lane+1]` |
| `bfmmla<Arch>(acc,a,b)` | `simd<float,4,Arch>` | Two `simd<bf16,8,Arch>` registers | 2×4 by 4×2 matrix product |
| `bfmlalb<Arch>(acc,a,b)` | `simd<float,4,Arch>` | Two `simd<bf16,8,Arch>` registers | Even lanes of both inputs |
| `bfmlalt<Arch>(acc,a,b)` | `simd<float,4,Arch>` | Two `simd<bf16,8,Arch>` registers | Odd lanes of both inputs |
| `bfmlalb_lane<Arch,Lane>(acc,a,b)` | `simd<float,4,Arch>` | `simd<bf16,8,Arch>` for `a`; either width for `b` | Even lanes of `a`, scalar `b[Lane]` |
| `bfmlalt_lane<Arch,Lane>(acc,a,b)` | `simd<float,4,Arch>` | `simd<bf16,8,Arch>` for `a`; either width for `b` | Odd lanes of `a`, scalar `b[Lane]` |

A dot-product lane index selects a pair, so its range is 0–1 for a four-element
right operand and 0–3 for an eight-element operand. The widening multiply-add
forms select one element, with ranges 0–3 and 0–7 respectively. Lane indices
are compile-time immediates. Wrong operand types and invalid lanes are rejected;
raw intrinsic vectors and mismatched feature tags cannot select an overload.
The two-lane FP32 result clears the unused lanes of its padded storage.

For `bfmmla`, store the left matrix's rows consecutively and the right matrix's
columns consecutively. Output lane `2*r+c` starts with `acc[2*r+c]`, accumulates
the pair of products at inner indices 0 and 1, then accumulates the pair at
indices 2 and 3. Each step has the same arithmetic contract as `bfdot`.
A row-major 4×2 right matrix needs rearrangement before this call.

## Arithmetic and floating-point controls

With FEAT_EBF16 absent or FPCR.EBF clear, `bfdot` and `bfmmla` round each product,
pair sum and accumulator addition separately to odd. An inexact finite result
has its low bit set; overflow gives signed infinity. Input and output
subnormals flush to zero, irrespective of FPCR.RMode, FZ or FIZ. FPCR.AH affects
the sign of a default NaN but otherwise does not change this mode.

With FEAT_EBF16 present and FPCR.EBF set, each pair is a fused sum of two
products, rounded once to FP32 before a separate accumulator addition. RMode,
FZ, AH and FIZ then control single-precision behavior. Matrix multiplication
still performs two ordered pair accumulations per output; neither instruction
is equivalent to a chain of ordinary FP32 fused multiply-adds.

In both modes, dot and matrix instructions produce default NaNs, ignore
floating-point exception enables and leave cumulative FPSR flags unchanged.
They do not write FPCR. The application chooses whether to enable enhanced
behavior and owns any changes to the environment.

`bfmlalb` and `bfmlalt` have a different contract: each output is one fused
multiply-add, rounded to FP32. With AH clear, they follow ordinary FP32
rounding, flushing and NaN controls, and accumulate floating-point exception
flags. With FEAT_AFP and AH set, they force nearest-even rounding and input and
output flushing, and suppress floating-point exceptions. EBF does not change
these operations. `noexcept` does not mask enabled hardware traps.

The wrappers preserve these instruction behaviors even when a result is
unused. Clang 23's BF16 intrinsics can merge calls across FPCR writes and remove
unused BFMLAL instructions despite floating-environment access being enabled.
The implementation uses volatile assembly to retain FPCR-sensitive results and
FPSR effects. A compiler memory barrier orders surrounding environment accesses;
it adds no CPU memory fence. Surrounding floating-point code still needs the
compiler's floating-environment support when it observes or changes controls.

## Compilation and execution

All operations require `Arch.has(arm_feature::neon_bf16)` and a `"bf16"` compiler
target. This existing BF16 feature is independent of FP16, DotProd and I8MM.
Its compiler prerequisite closure includes NEON. Importing the module neither
enables instructions in the caller nor checks the executing CPU:

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

bool available() {
  auto cpu = native::observe_arm_capabilities();
  return native::classify_isa(cpu, dot_isa, NATIVE_TARGET_MINIMUM).admitted();
}
```

`arm_feature::ebf16` describes enhanced arithmetic support. Check its admission
before setting FPCR.EBF; the feature bit alone does not set the control.
`target_features<native::arm>("ebf16")` is rejected because Clang 23 has no standalone target
feature with that spelling. Enhanced arithmetic uses the same BF16 instructions.

Pass BF16 and FP32 `simd` values directly between vector algorithms and these APIs. The
wrappers add no BF16 elementwise arithmetic or conversion policy.

The [tests](../tests/arm_bf16/README.md) cover every shape and legal lane,
independent exact-rational arithmetic, FPCR preservation, FPSR effects and
compiler rejection. Native default-BF16 and AFP controls pass on Apple M3 with
Clang 23. EBF16 was not advertised there, so enhanced native execution remains
unverified. Big-endian checks inspect compiler mappings without executing
big-endian hardware.

The arithmetic follows Arm's [BF16 instruction overview](https://developer.arm.com/community/arm-community-blogs/b/ai-blog/posts/bfloat16-processing-for-neural-networks-on-armv8_2d00_a)
and [SME supplement, B3.1.2 and E2.2](https://documentation-service.arm.com/static/62015c6c965f7d118e3f5f4c).
The current Arm-authored [BFMMLA and BFMLAL shared pseudocode](https://www.scs.stanford.edu/~zyedidia/arm64/shared_pseudocode.html),
mirrored from the architecture specification, gives the matrix association and
alternative BFMLAL controls. The raw types follow [ACLE](https://arm-software.github.io/acle/main/acle.html#16-bit-brain-floating-point-arithmetic-scalar-intrinsics).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
