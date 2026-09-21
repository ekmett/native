# Choosing an instruction family

The general SIMD interface supplies arithmetic, masks and memory operations.
Use an instruction family when the algorithm needs a particular operation or
its exact arithmetic contract: a saturating dot product, a carry-less product,
a conversion with specified rounding, or a checksum update.

Import the family module directly, or use `native.x86`, `native.arm`, or the
host's `native` hub. Vector operands and results use `simd<T,N,Arch>`; scalar
forms use ordinary C++ values. Operations constrain the feature bits in `Arch`
and require a compatible compiler target. Different ISA values remain different
vector types even when their storage matches.

A feature bit, compiler target and runtime observation answer different questions.
The bit permits an API operation; the target permits generated instructions;
runtime admission establishes whether the CPU and OS can execute them. Check
all requirements of the containing function before entering it. Importing a
module does not enable a compiler target or perform runtime dispatch. See
[the call-boundary example](modules.md#features-compiler-targets-and-runtime-admission)
and [target-list dispatch](omnibus.md).

## Dot products and small matrices

The input element type, grouping and accumulation rule are part of the operation.
Choose them before choosing a register width.

| Algorithm needs | x86 | AArch64 |
| --- | --- | --- |
| Byte or word products accumulated into integers | [VNNI](x86-vnni.md): byte/word groups, signedness variants, wrapping or saturation | [DotProd](arm-dotprod.md): four-byte groups with wrapping accumulation |
| Small byte matrices or mixed-sign byte dots | VNNI supplies grouped dot products | [I8MM](arm-i8mm.md): 2×8 by 8×2 matrices and mixed-sign byte dots |
| BF16 products accumulated into FP32 | [AVX-512 BF16 profile](../tests/bf16_profile/README.md) | [BF16](arm-bf16.md): pair dots, 2×4 by 4×2 matrices and widening multiply-adds |

Matrix instructions require a specific row/column packing; a row-major right
matrix may need rearrangement. Saturating and wrapping integer accumulations
are distinct operations. BF16 pair accumulation also differs from a chain of
ordinary FP32 fused multiply-adds. The family guides specify those boundaries,
including the effects of ARM's enhanced BF16 mode.

## Conversions, fixed-point and complex arithmetic

[Base NEON integer instructions](arm-neon.md) provide saturating add/subtract,
saturating narrowing, signed multiply-high and per-lane variable shifts with
rounding and saturation variants. They require `arm_feature::neon`; operations
that saturate preserve their sticky FPSR.QC effects, including discarded results.

[F16C](x86-f16c.md) converts between binary32 and IEEE binary16 on x86. It does
not supply half-precision arithmetic. Its immediate controls rounding, and
conversion may update MXCSR or trap according to the caller's exception masks.

On AArch64, [FHM](arm-fp16fml.md) multiplies FP16 inputs and accumulates directly
into FP32 with one fused rounding per result. [RDM](arm-rdm.md) combines a
fixed-point product and accumulator before rounding and saturation; saturation
sets sticky FPSR.QC. [FCMA](arm-fcma.md) operates on adjacent real/imaginary
pairs. A full complex multiply-add needs two partial operations, with the
corresponding two rounding stages.

For elementwise half arithmetic, use the [AVX-512 FP16](../tests/avx512_fp16/README.md)
or [NEON FP16](../tests/neon_fp16/README.md) profile. Base NEON already supports
four- and eight-lane `fp16` and `bf16` storage and transfer, without either
arithmetic extension. The operation's feature requirements still apply.
Floating-point control and status remain
under application ownership; `noexcept` does not mask hardware exceptions.

## Bits, polynomials and checksums

For scalar x86 integers, [BMI1](x86-bmi1.md) and [BMI2](x86-bmi2.md) provide bit
field operations, deposit/extract and related primitives. [POPCNT](x86-popcnt.md)
and [LZCNT](x86-lzcnt.md) have independent feature requirements.
[VPOPCNTDQ](x86-vpopcntdq.md) counts bits in 32- or 64-bit vector lanes and has
its own width and masking requirements. [BITALG](x86-bitalg.md) counts bits in
byte/word lanes and selects source bits into compact predicates.
[VBMI](x86-vbmi.md) permutes bytes across one or two whole vectors and extracts
wrapping bit windows from qwords.
[AVX-512CD](x86-avx512cd.md) supplies leading-zero counts and masks identifying equal earlier lanes. Conflict
detection compares across the whole vector, including masked-off source lanes.

For polynomial arithmetic, [PCLMULQDQ and VPCLMULQDQ](x86-pclmul.md) on x86 and
[PMULL](arm-crypto.md#aes-and-polynomial-state) on ARM multiply polynomials over
GF(2), without integer carries or modular reduction. [GFNI](x86-gfni.md) supplies
byte field multiplication and affine maps; its field multiplication includes
reduction in the specified byte field.

[ARM CRC](arm-crc.md) supplies both IEEE CRC32 and Castagnoli CRC32C updates.
[X86 CRC32C](x86-crc32c.md) supplies only Castagnoli updates, despite the
instruction's `crc32` name. Both APIs update an accumulator without adding an
initial or final complement. Operand byte order and memory bounds belong to
the caller.

[X86 AES-NI](x86-aes.md) and [ARM AES and SHA](arm-crypto.md) expose round,
state and schedule operations. X86 and ARM AES rounds add their keys at
different stages, so their round sequences are not interchangeable.
They do not assemble a cipher mode, key schedule, message padding or a complete
hash. ARM's hardware feature bits are independent. Clang's ARM `aes`, `sha2` and
`sha3` targets enable bundles, so admission must cover each whole compiler
target even when the source calls only one of its operations.

[ARM SM3 and SM4](arm-sm-crypto.md) supply hash-round, message-schedule,
data-round and key-schedule instructions on four-word vectors. Their hardware
bits are independent; Clang's `sm4` target enables both and admission covers
that complete pair together with NEON.

## Shapes, masks and execution

The family guides list supported element types, lane counts and compile-time
immediates. A vector's logical shape matters even if another shape has the same
register size; converting signedness or reinterpreting elements must be explicit.
Scalar RDM lane forms, for example, still take a `simd` value for their vector
lane source.

Masked x86 instruction forms take `predicate<N,Arch>`, with one meaningful bit
per documented mask lane. Merge and zero forms differ in what happens to
inactive lanes. Use the exact predicate type required by the operation; a full
vector comparison mask may have a different representation under a minimal
feature set.

Vector families link `native::native` and import `native.simd`. Scalar-only
families and feature observation link `native::minimal`. The
[module guide](modules.md#imports-and-build-targets) gives the common imports.
Runtime calls do not dispatch or provide a software fallback internally.
Keep an optional kernel behind admission and select another implementation when
its requirements are unavailable.

Each guide links its validation fixture. Compile and assembly checks establish
compiler behavior; runtime tests execute only after admission. An unsupported
runtime skip does not qualify the instruction's behavior on that machine.

## Constant evaluation

Instruction families provide semantic implementations for constant evaluation.
With the required features in `Arch`, the same `constexpr` overload evaluates
at compile time or emits the native instruction at runtime. Without those
features, a separate `consteval` overload accepts constant inputs only.
Vector storage must still exist for the element type, lane count and architecture
tag. Constant evaluation does not grant a vector type registers that its tag
does not supply.

The immediate-only overload may appear in an unevaluated `requires` expression;
that does not establish that a later call with runtime inputs is valid. Feature
constraints and runtime admission remain necessary for executable kernels.

Floating-point constant evaluation uses nearest-even rounding, gradual
underflow and masked exceptions, with ARM's DN, AHP, AH and EBF controls clear.
An instruction's fixed rules or rounding immediate take precedence: legacy ARM
BF16 dot products, for example, round to odd and flush subnormals. The family
guides specify NaN selection and other instruction-specific details. Constant
evaluation neither reads nor changes the calling thread's floating-point
environment; runtime operations retain that environment's behavior.

Hardware observations, waits and control-register operations remain runtime
operations. A semantic result at compile time does not reproduce a hardware
side effect such as a sticky saturation or floating-point exception flag.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
