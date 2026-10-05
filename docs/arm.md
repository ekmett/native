# ARM instruction sets

[Instruction sets](instructions.md)

These guides cover AArch64 instructions. NEON supplies the base vectors;
crypto, dot-product, matrix and half-precision operations have additional
feature requirements. Keep the hardware feature bits distinct from Clang
target bundles when admitting a function; see [target selection](omnibus.md).

- [AES/PMULL/SHA: cipher rounds, polynomial products and hash transforms](arm-crypto.md)
- [BF16: dot products and matrix accumulation](arm-bf16.md)
- [CRC: CRC32 and CRC32C updates](arm-crc.md)
- [DotProd: byte dot products](arm-dotprod.md)
- [FCMA: complex arithmetic](arm-fcma.md)
- [FHM: FP16 products with FP32 accumulation](arm-fp16fml.md)
- [I8MM: byte matrix products and mixed-sign dot products](arm-i8mm.md)
- [JSCVT: JavaScript integer conversion](arm-jscvt.md)
- [NEON: saturation, shifts, bit operations and conversions](arm-neon.md)
- [RDM: rounded fixed-point accumulation](arm-rdm.md)
- [SM3 and SM4: hash rounds and block-cipher transforms](arm-sm-crypto.md)

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
