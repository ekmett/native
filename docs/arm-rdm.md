# ARM RDM: rounded fixed-point accumulation

[ARM instruction sets](arm.md)

## Why use it

Fixed-point filters need the product and accumulator to meet before rounding
and saturation. RDM performs that combined operation for signed 16-bit and
32-bit samples. Keeping the full intermediate matters when a large product
cancels part of the accumulator.

## Operations

Import `native.arm.rdm`, or use the `native.arm` or `native` hub.
`sqrdmlah<Arch>(acc,a,b)` adds the doubled product; `sqrdmlsh<Arch>(acc,a,b)`
subtracts it. The corresponding `_lane<Arch,Lane>` forms broadcast `b[Lane]`.

| Accumulator, `a` and result | Ordinary `b` | Indexed `b` |
| --- | --- | --- |
| `std::int16_t` | `std::int16_t` | `simd<std::int16_t,4,Arch>` or `simd<std::int16_t,8,Arch>` |
| `simd<std::int16_t,4,Arch>` or `simd<std::int16_t,8,Arch>` | Same vector type | Either 16-bit vector width |
| `std::int32_t` | `std::int32_t` | `simd<std::int32_t,2,Arch>` or `simd<std::int32_t,4,Arch>` |
| `simd<std::int32_t,2,Arch>` or `simd<std::int32_t,4,Arch>` | Same vector type | Either 32-bit vector width |

All vector operands share `Arch`. `Lane` is a compile-time immediate within the
right operand's range. For element width `w`, each result follows:

```text
sqrdmlah(acc,a,b) = signed_saturate_w(floor((acc*2^w + 2*a*b + 2^(w-1)) / 2^w))
sqrdmlsh(acc,a,b) = signed_saturate_w(floor((acc*2^w - 2*a*b + 2^(w-1)) / 2^w))
```

## Caveats

Ties round toward positive infinity. Saturation occurs after combining the
full product and accumulator. In particular, minimum-times-minimum can cancel
a negative accumulator without intermediate saturation. A saturating
multiply followed by a saturating add has different results.

Runtime saturation sets sticky `FPSR.QC`; a nonsaturating operation does not
clear it. An unused result still executes the instruction. The wrappers leave
FPCR unchanged and neither save nor restore FPSR. Constant evaluation computes
only the value, with no access to the calling thread's QC state.

Native runtime calls require `arm_feature::rdm` and an `"rdm"` caller target.
`feature_closure(arm_feature::rdm)` includes NEON. Admit that requirement and
`NATIVE_TARGET_MINIMUM` before entering the target function. RDM does not
require DotProd, FP16 or the rest of Armv8.1-A. The wrappers use exact inline
instructions to avoid Clang 23's broader `v8.1a` builtin requirement.

Scalar calls may omit `Arch`, using the owning module's captured
`NATIVE_BASELINE`. An importer's target attribute cannot change that default.
Clang 23 defines `__ARM_FEATURE_QRDMX` for Armv8.1-A, but not for
`armv8-a+rdm` alone; use an explicit ISA for the latter with the current
macro-based baseline snapshot. Vector calls deduce `Arch` from their operands.

All forms support constant evaluation. A tag without RDM permits runtime
software evaluation when it includes `polyfill`; otherwise it permits only
`consteval` calls. Software evaluation reproduces the saturated result without
setting `FPSR.QC`. Hardware-capable tags keep their native instruction path.

See the [Arm Advanced SIMD intrinsic reference](https://arm-software.github.io/acle/neon_intrinsics/advsimd.html#sqrdmlah-intrinsics-from-armv81-a)
and [LLVM's big-endian NEON representation notes](https://llvm.org/docs/BigEndianNEON.html).
