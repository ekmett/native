# x86 AES: AES-NI rounds and key helpers

[x86 instruction sets](x86.md)

## Why use it

AES-NI replaces the byte substitutions, row permutations and column arithmetic
of an AES round with a register operation. It provides the round primitives
needed to build an AES implementation without software lookup tables.

## Operations

`import native.x86.aes;` provides operations on
`simd<std::uint8_t,16,Arch>`. Link `native::native`; `native.x86` and `native`
also export the module. All operands and results share the same tag.

| Operation | Result |
| --- | --- |
| `aesenc<Arch>(state, key)` | Encryption round, including MixColumns and final key XOR |
| `aesenclast<Arch>(state, key)` | Final encryption round, omitting MixColumns |
| `aesdec<Arch>(state, key)` | Decryption round, including inverse MixColumns and final key XOR |
| `aesdeclast<Arch>(state, key)` | Final decryption round, omitting inverse MixColumns |
| `aesimc<Arch>(state)` | Inverse MixColumns on the four state columns |
| `aeskeygenassist<Arch, Imm8>(state)` | Substituted and rotated words with a round constant |

State byte `4 * column + row` follows the AES matrix's column order.
`aeskeygenassist` uses the second and fourth input words and XORs `Imm8` into
the rotated outputs. The immediate is a compile-time unsigned byte, 0–255.

## Caveats

The x86 round instructions add the key after their transformations. ARM
AESE/AESD add it first, so porting a round sequence requires rearranging keys.
Intermediate AESDEC rounds use inverse-mixed keys; AESDECLAST uses the original
initial key. These functions supply neither a complete key schedule nor a
cipher mode.

Runtime calls require AES and SSE2 storage. Use
`target_features<native::x86>("aes")`, compile the caller for that target and
admit it with `classify_isa` before entry. Importing does not enable instructions
or dispatch. With AES in `Arch`, the overloads are `constexpr` with native
runtime paths. An SSE2 storage tag without AES has `consteval` overloads only.
Integer round operations leave floating-point status unchanged.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
