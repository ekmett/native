# AES round operations

`import native.x86.aes;` provides AES-NI operations on
`simd<std::uint8_t,16,Arch>`. Link `native::native`; the `native.x86` and
`native` hubs also export the module. Raw intrinsic register types remain
implementation details.

All operands and results have the same architecture tag. Runtime calls require
`x86_feature::aes`, and the vector needs SSE2 storage. Use
`target_features<native::x86>("aes")` to include the register prerequisites.
Enter the caller's AES target only after `classify_isa` admits its requirements.
An import neither enables instructions nor performs runtime dispatch.

| Operation | Result |
| --- | --- |
| `aesenc<Arch>(state, key)` | Encryption round, including MixColumns and the final key XOR |
| `aesenclast<Arch>(state, key)` | Final encryption round, omitting MixColumns |
| `aesdec<Arch>(state, key)` | Decryption round, including inverse MixColumns and the final key XOR |
| `aesdeclast<Arch>(state, key)` | Final decryption round, omitting inverse MixColumns |
| `aesimc<Arch>(state)` | Inverse MixColumns on the four state columns |
| `aeskeygenassist<Arch, Imm8>(state)` | Substituted and rotated words with the round constant |

The state uses byte order `4 * column + row` for the AES matrix. The x86 round
instructions add the key after their transformations. ARM AESE/AESD add it
first, so the two instruction sequences require different arrangements of the
round keys. Intermediate AESDEC rounds take inverse-mixed keys; AESDECLAST
takes the original initial key.

`Imm8` is a compile-time unsigned byte in the range 0–255. The key helper uses
the second and fourth input words and XORs the immediate into the rotated
outputs. It supplies one step of a key schedule, not a complete schedule or
cipher mode. The round operations likewise operate on one 16-byte state.

Every form supports constant evaluation. With AES in `Arch`, the same
`constexpr` overload evaluates constant inputs or emits the native operation
at runtime. An SSE2 storage tag without AES can call a `consteval` overload;
it cannot use that overload with runtime inputs. The result retains the exact
input tag. These integer operations leave floating-point status unchanged.

The [fixture](../tests/x86_aes/README.md) describes the compiled shape checks,
constant-evaluation references and admitted runtime tests.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
