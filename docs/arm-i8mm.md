# I8MM integer matrix and dot products

I8MM multiplies small byte matrices and computes mixed-sign byte dot products,
accumulating the results into 32-bit integers. Import `native.arm.i8mm`, `native.arm`, or `native` for these
AArch64 operations. Operands and results use `native::simd<T, N, Arch>`, with
semantic byte or integer lanes and a shared `Arch` containing `arm_feature::i8mm`.
Raw NEON registers are private implementation details.

| Operation | Accumulator and result | Input signedness |
| --- | --- | --- |
| `smmla<Arch>(acc, a, b)` | `simd<std::int32_t, 4, Arch>` | Both `simd<std::int8_t, 16, Arch>` |
| `ummla<Arch>(acc, a, b)` | `simd<std::uint32_t, 4, Arch>` | Both `simd<std::uint8_t, 16, Arch>` |
| `usmmla<Arch>(acc, a, b)` | `simd<std::int32_t, 4, Arch>` | Unsigned `a`, signed `b` |
| `usdot<Arch>(acc, a, b)` | `simd<std::int32_t, 2, Arch>` or `simd<std::int32_t, 4, Arch>` | Unsigned `a`, signed `b` |
| `usdot_lane<Arch, Lane>(acc, a, b)` | `simd<std::int32_t, 2, Arch>` or `simd<std::int32_t, 4, Arch>` | Unsigned `a`, signed `b` |
| `sudot_lane<Arch, Lane>(acc, a, b)` | `simd<std::int32_t, 2, Arch>` or `simd<std::int32_t, 4, Arch>` | Signed `a`, unsigned `b` |

The matrix operations multiply a 2-by-8 matrix by an 8-by-2 matrix. The first
operand stores its rows consecutively; the second stores its columns
consecutively. Output lane `2*r+c` accumulates
`sum(a[8*r+k] * b[8*c+k], k=0..7)`. An already row-major 8-by-2 right-hand
matrix must therefore be rearranged before calling these operations.

The non-indexed dot product accumulates each corresponding group of four
input bytes. Both byte vectors have the same register width as the accumulator.
The indexed operations select one four-byte group of `b` for every output
lane: output lane `j` accumulates
`sum(a[4*j+k] * b[4*Lane+k], k=0..3)`.
Their `a` and accumulator widths match, while `b` independently has 8 or 16
bytes. `Lane` must be in `[0,1]` for an 8-byte `b` or `[0,3]` for a 16-byte `b`;
out-of-range template arguments are rejected. The two indexed signedness
forms remain distinct because exchanging operands also changes which operand
provides the selected group.

Every accumulation wraps modulo 2^32. Signed results interpret the resulting
bits as two's complement. There is no saturation, floating-point arithmetic,
or floating-point control-register dependency. These contracts follow Arm's
[Advanced SIMD intrinsic reference](https://arm-software.github.io/acle/neon_intrinsics/advsimd.html#matrix-multiplication-intrinsics-from-armv86-a).

Each wrapper is `noexcept` and always inline with Clang's `target("i8mm")`
attribute. The caller must compile in a compatible target scope and check
`classify_isa(observe_arm_capabilities(), arm_feature::i8mm).admitted()` before
entering it. Importing the API does not enable I8MM instructions or dispatch
at runtime. For example:

```cpp
#include <cstdint>
#include <native/targets.h>
import native.arm.i8mm;

constexpr auto matrix_isa = native::feature_closure(native::arm_feature::i8mm);

__attribute__((target("i8mm")))
native::simd<std::int32_t, 4, matrix_isa> multiply(
    native::simd<std::int32_t, 4, matrix_isa> acc,
    native::simd<std::int8_t, 16, matrix_isa> a,
    native::simd<std::int8_t, 16, matrix_isa> b) noexcept {
  return native::smmla<matrix_isa>(acc, a, b);
}

bool can_multiply() {
  return native::classify_isa(native::observe_arm_capabilities(),
    matrix_isa, NATIVE_TARGET_MINIMUM).admitted();
}
```

I8MM requires NEON but can be requested independently of DotProd, FP16 and
BF16, matching [LLVM's AArch64 feature definition](https://github.com/llvm/llvm-project/blob/main/llvm/lib/Target/AArch64/AArch64Features.td).
The `isa::arm_i8mm` property accesses the `arm_feature::i8mm` bit.
`target_features("i8mm")` adds the NEON prerequisite, and `NATIVE_TARGET_MINIMUM`
records I8MM when the compiler defines `__ARM_FEATURE_MATMUL_INT8`.

The macOS detector queries `hw.optional.arm.FEAT_I8MM`. The Linux detector
uses [`HWCAP2_I8MM` from `AT_HWCAP2`](https://docs.kernel.org/arch/arm64/elf_hwcaps.html)
when the SDK defines that bit. Failed queries and missing SDK definitions leave
the feature unobserved; a successful false query records observed-but-absent.
The Windows detector leaves Advanced SIMD I8MM unobserved, so admission fails
there: an SVE I8MM query does not establish this instruction family's
availability.

`tests/arm_i8mm` checks the granular module and main hub against
independent scalar references with unsigned modular accumulation. It covers
matrix row/column sentinels, all 256 single-product positions, integer extremes,
10,000 deterministic random cases, both register widths, and all 24 indexed
signedness/width/lane combinations. Compile checks reject a missing feature,
an incompatible caller target and an invalid lane. A separately targeted
object is disassembled to verify matrix and mixed-dot instructions, indexed
immediates, and a baseline control. Runtime tests return the CTest skip code 77
when capability admission fails; compilation alone is not an execution claim.

Paired assembly checks compare the public `simd` call with its private native
helper under identical target attributes and register signatures. The complete
instruction sequences must match, including moves, loads, stores and calls.
This checks abstraction overhead in the tested leaf contexts; it is not a
benchmark or a guarantee about surrounding application code.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
