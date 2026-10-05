# Instruction sets

- [x86 instruction sets](x86.md)
- [ARM instruction sets](arm.md)
- [WebAssembly instruction sets](wasm.md)

Start with the ordinary `simd` operations. Reach for an instruction family when
its particular operation or arithmetic contract does useful work for you.

## Choosing a family

### Dot products and small matrices

Packed dot products do several multiplies and additions per result lane. They
are useful for quantized inference, small matrix kernels and reductions where
the input precision is lower than the accumulator precision. Choose the input
signedness and accumulation rule before choosing a register width.

- [x86 VNNI](x86-vnni.md): byte and word products, wrapping or saturating accumulation.
- [ARM DotProd](arm-dotprod.md): four-byte dot products.
- [ARM I8MM](arm-i8mm.md): byte matrix products and mixed-sign dot products.
- [ARM BF16](arm-bf16.md) and [x86 AVX-512 BF16](modules.md#half-precision-values): BF16 products accumulated into FP32.

### Integer products and carry chains

Large integers need products and carries that ordinary lane-wise arithmetic
cannot express. These operations expose the pieces without choosing a big
integer representation for you.

- [x86 IFMA](x86-ifma.md): accumulate halves of 52-bit products in 64-bit lanes.
- [x86 ADX](x86-adx.md): scalar unsigned addition with carry.
- [x86 BMI2](x86-bmi2.md): wide scalar products, alongside bit deposit/extract.

### Conversions, fixed-point and complex arithmetic

Conversions often sit at the boundary between compact storage and a wider
calculation. Fixed-point and complex instructions combine arithmetic steps
with a particular rounding or packing convention.

- [ARM NEON](arm-neon.md): integer conversions, saturation, narrowing, multiply-high and shifts.
- [x86 F16C](x86-f16c.md): binary16/binary32 conversion.
- [x86 AVX-NE-CONVERT](x86-avxneconvert.md): FP16/BF16 memory conversion.
- [ARM FHM](arm-fp16fml.md): FP16 products accumulated into FP32.
- [ARM RDM](arm-rdm.md): rounded, saturating fixed-point accumulation.
- [ARM FCMA](arm-fcma.md): arithmetic on adjacent real/imaginary pairs.
- [ARM JSCVT](arm-jscvt.md): scalar double conversion with JavaScript integer semantics.
- [Half-precision profiles](modules.md#half-precision-values): elementwise FP16 arithmetic and BF16 storage.

### Bits and rearrangement

Bit counts, extraction and permutation are useful for packed data structures,
parsers and moving selected elements into the next stage of a calculation.
Conflict detection finds repeated destinations before an indexed update.

- [x86 BMI1](x86-bmi1.md) and [BMI2](x86-bmi2.md): bit fields, lowest-bit operations and deposit/extract.
- [x86 POPCNT](x86-popcnt.md) and [LZCNT](x86-lzcnt.md): scalar counts.
- [x86 VPOPCNTDQ](x86-vpopcntdq.md) and [BITALG](x86-bitalg.md): packed counts and bit selection.
- [x86 VBMI](x86-vbmi.md): byte permutations and bit windows.
- [x86 VBMI2](x86-vbmi2.md): compaction and double-source shifts.
- [x86 AVX-512CD](x86-avx512cd.md): conflict detection and leading-zero counts.
- [ARM NEON](arm-neon.md): bit counts and reversals.

### Polynomials, checksums and cryptography

Carry-less products implement polynomial multiplication over GF(2), rather
than ordinary integer multiplication. CRC instructions update a checksum;
cryptographic instructions expose round and schedule steps. They save work
inside an algorithm but do not supply a complete hash or cipher mode.

- [x86 PCLMULQDQ/VPCLMULQDQ](x86-pclmul.md): carry-less products.
- [x86 GFNI](x86-gfni.md): byte-field multiplication and affine maps.
- [ARM CRC](arm-crc.md) and [x86 CRC32C](x86-crc32c.md): checksum updates.
- [x86 AES](x86-aes.md) and [VAES](x86-vaes.md): AES rounds and key helpers.
- [x86 SHA](x86-sha.md): SHA-1/SHA-256 primitives.
- [x86 SHA512/SM3/SM4](x86-extended-crypto.md): round and schedule operations.
- [ARM AES/PMULL/SHA](arm-crypto.md): cipher rounds, polynomial products and hash primitives.
- [ARM SM3/SM4](arm-sm-crypto.md): hash and cipher primitives.

### Indexed memory and waiting

Gather/scatter follow several indices at once, where a contiguous load cannot.
Wait instructions let a polling loop give back execution resources while it
waits for a condition to change.

- [x86 indexed memory](x86-memory.md): gathers and scatters with inactive-lane memory suppression.
- [x86 WAITPKG/MWAITX](x86-wait.md): monitored waits, TSC deadlines and a spin-loop fallback.

### WebAssembly

WebAssembly packages vector operations behind a portable instruction format.
SIMD128 gives a fixed 128-bit vocabulary; relaxed SIMD permits host-dependent
results for operations where that latitude can avoid extra work.

- [SIMD128](wasm-simd.md): integer and floating arithmetic, memory, conversions and rearrangement.
- [Relaxed SIMD](wasm-relaxed.md): fused arithmetic, selection, conversion and dot products with explicitly permitted variation.

## Common caveats

Import the family module directly, or use `native.x86`, `native.arm`,
`native.wasm`, or the host's `native` hub. Vector operands and results use
`simd<T,N,Arch>`; scalar forms use ordinary C++ values. Vector families link
`native::native`; scalar-only families and feature observation link
`native::minimal`. See [imports and build targets](modules.md#imports-and-build-targets).

An ISA tag, a compiler target and runtime admission do different jobs. The tag
permits the API operation, the target permits generated instructions, and
admission establishes whether the CPU and OS can execute them. Importing a
module does none of the latter two. Admit the whole containing function, not
just the one operation you happened to call. See [target selection](omnibus.md).

WebAssembly validates the complete linked module. A runtime branch cannot hide
unsupported instructions; build and load separate modules for different feature
levels. Relaxed operations can produce different permitted results on different
hosts.

Shapes, signedness and masks are part of the contract. Equal register sizes do
not make vector types interchangeable. AVX-512 instruction masks use
`predicate<N,Arch>`; AVX2 gathers instead use full-vector sign-bit masks. The
family guides spell out packing, rounding, side effects and any extra work.

Instruction families have semantic implementations for constant evaluation.
With the required features in `Arch`, the same `constexpr` overload evaluates
at compile time or emits the native instruction at runtime. Without those
features, a separate `consteval` overload accepts constant inputs only. Vector
storage must still exist for the element type, lane count and architecture tag.
An unevaluated `requires` check can see the immediate-only overload; that does
not make a later runtime call valid.

Floating constant evaluation uses nearest-even rounding, gradual underflow and
masked exceptions, with ARM's DN, AHP, AH and EBF controls clear. An instruction's
fixed rules or rounding immediate take precedence. Constant evaluation does not
read or update the thread's floating-point environment; runtime operations do.
Hardware observations, waits and control-register operations remain runtime-only.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
