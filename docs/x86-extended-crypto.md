# X86 SHA512, SM3 and SM4

`native.x86.sha512`, `native.x86.sm3` and `native.x86.sm4` export individual
instruction primitives through `native.x86` and `native`. Vector operands and
results are `native::simd`; the ISA tag is deduced from those operands.

| Module | Operations | Vector words |
| --- | --- | --- |
| `native.x86.sha512` | `sha512msg1`, `sha512msg2`, `sha512rnds2` | Four `uint64_t` result lanes; `msg1`'s second operand and `rnds2`'s third operand have two lanes |
| `native.x86.sm3` | `sm3msg1`, `sm3msg2`, `sm3rnds2<Arch, Imm8>` | Four `uint32_t` lanes |
| `native.x86.sm4` | `sm4key4`, `sm4rnds4` | Four or eight `uint32_t` lanes |

SHA-512 and SM3 round states use high-to-low ABEF/CDGH packing. SHA-512 message
words use ascending lanes. SM3's CDGH input contains the unrotated previous ABEF
state; the instruction rotates C/D and G/H before its two rounds. Its immediate
accepts 0–255 and selects the first round with `Imm8 & 0x3e`. SM4 state, keys
and constants use ascending lanes, with independent four-word blocks in the
256-bit forms. The SM4 argument order is state first, keys/constants second.

The independent feature bits are `x86_feature::sha512`, `sm3` and `sm4`, from
CPUID leaf 7 subleaf 1 EAX bits 0, 1 and 2. All require AVX and OS-enabled XMM/YMM
state. Clang's `sha512` and `sm4` compiler targets additionally enable AVX2;
`target_features<x86>("sha512")` and `target_features<x86>("sm4")` record that
compiler prerequisite separately from the hardware bit. These features do not
imply the older SHA-1/SHA-256 extension.

Runtime operations require the instruction feature and a matching caller target.
For example, an SHA512 leaf uses `target_features<x86>("sha512")` for its vector
ISA and a `sha512` target scope. No instruction wrapper dispatches at runtime.
Constant evaluation implements the same lane semantics with unsigned modular
arithmetic. Feature-absent forms are consteval-only and still require storage
for the specified vector shapes. Raw registers, mixed ISA tags, wrong element
or lane counts, and out-of-range SM3 immediates are rejected.

The operations implement the [Intel instruction specification](https://cdrdv2-public.intel.com/868137/325462-089-sdm-vol-1-2abcd-3abcd-4.pdf).
They neither inspect nor modify floating-point control or status. These are
instruction leaves, not complete hashing, encryption or protocol APIs.

[Tests](../tests/x86_extended_crypto/README.md) cover complete known-answer
computations, feature admission, compile-time boundaries and paired codegen.
