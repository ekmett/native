# x86 SHA512/SM3/SM4: hash and cipher rounds

[x86 instruction sets](x86.md)

## Why use it

These extensions accelerate the repetitive round and schedule work inside
SHA-512, SM3 and SM4. They let an implementation retain the algorithm's state
in vectors while delegating several rounds or schedule steps to one operation.

## Operations

`import native.x86.sha512;`, `import native.x86.sm3;` and
`import native.x86.sm4;` provide the following primitives. `native.x86` and
`native` re-export them. Operands and results are `simd` values; their tag is
deduced from the operands.

| Module | Operations | Shape |
| --- | --- | --- |
| `native.x86.sha512` | `sha512msg1`, `sha512msg2`, `sha512rnds2` | Four `uint64_t` result lanes; `msg1` operand 2 and `rnds2` operand 3 have two lanes |
| `native.x86.sm3` | `sm3msg1`, `sm3msg2`, `sm3rnds2<Arch, Imm8>` | Four `uint32_t` lanes |
| `native.x86.sm4` | `sm4key4`, `sm4rnds4` | Four or eight `uint32_t` lanes |

SHA-512 and SM3 rounds use high-to-low ABEF/CDGH packing. SHA-512 message words
use ascending lanes. SM3's CDGH input contains the unrotated previous ABEF
state: the instruction rotates C/D and G/H before its two rounds. SM3's
compile-time byte `Imm8` selects the first round with `Imm8 & 0x3e`.

SM4 state, keys and constants use ascending lanes. Its calls take state first,
keys or constants second; 256-bit forms contain independent four-word blocks.

## Caveats

These are round and schedule primitives. The caller still supplies complete
hashing or encryption, including padding, key schedules, byte-order changes
and any protocol. All integer arithmetic wraps; the operations leave FP
control and status unchanged.

SHA512, SM3 and SM4 have independent feature bits and require AVX plus enabled
XMM/YMM state. They do not imply the older SHA-1/SHA-256 extension. Use the
corresponding `target_features<native::x86>("sha512")`, `"sm3"` or `"sm4"`
and admit that matching caller target before entry. Clang's SHA512 and SM4
targets also enable AVX2; `target_features` records those prerequisites.

Feature-bearing overloads are `constexpr` with native runtime paths. Weaker
tags have `consteval` overloads only, with complete storage for the documented
shapes. Inputs must have matching tags, exact element types and lane counts;
SM3 immediates must be in 0–255. Calls do not dispatch at runtime.

See Intel's [instruction specification](https://cdrdv2-public.intel.com/868137/325462-089-sdm-vol-1-2abcd-3abcd-4.pdf).
