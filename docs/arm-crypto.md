# ARM AES/PMULL/SHA: cipher rounds, polynomial products and hash transforms

[ARM instruction sets](arm.md)

## Why use it

AES rounds, carryless polynomial multiplication and SHA transforms have enough
internal structure to benefit from dedicated instructions. These operations let
you build the state machine around those instructions while keeping vector
values in the same typed SIMD interface as the rest of a kernel.

## Operations

Import `native.arm.aes`, `native.arm.pmull` or `native.arm.sha`, or use the
`native.arm` or `native` hub. Each module reexports `native.simd`.
All vector operands and results preserve a common `Arch`.

| Operations | Shape | Transformation |
| --- | --- | --- |
| `aese(state,key)`, `aesd(state,key)` | `simd<std::uint8_t,16,Arch>` | Forward/inverse AES substitution and row permutation after key XOR |
| `aesmc(state)`, `aesimc(state)` | Same byte shape | Forward/inverse MixColumns |
| `pmull<Arch>(a,b)` | Two `std::uint64_t` inputs; `simd<std::uint64_t,2,Arch>` result | Carryless 64×64-bit product |
| `pmull2(a,b)` | `simd<std::uint64_t,2,Arch>` | Carryless product of the high lanes |
| Byte `pmull(a,b)` | Two `simd<std::uint8_t,8,Arch>` inputs; `simd<std::uint16_t,8,Arch>` result | Eight carryless byte products |
| Byte `pmull2(a,b)` | Two `simd<std::uint8_t,16,Arch>` inputs; same 16-bit result | Eight carryless products of the high byte lanes |
| `sha1c`, `sha1p`, `sha1m` | Four 32-bit state words, scalar `std::uint32_t e`, four prepared round words | Four SHA-1 rounds using choice, parity or majority |
| `sha1h<Arch>(word)` | `std::uint32_t` input/result | SHA-1 fixed rotation |
| `sha1su0`, `sha1su1` | `simd<std::uint32_t,4,Arch>` | Paired four-word SHA-1 schedule updates |
| `sha256h`, `sha256h2` | `simd<std::uint32_t,4,Arch>` | Paired four-round SHA-256 state updates |
| `sha256su0`, `sha256su1` | Same 32-bit shape | Paired four-word SHA-256 schedule updates |
| `sha512h`, `sha512h2` | `simd<std::uint64_t,2,Arch>` | Packed SHA-512 round updates |
| `sha512su0`, `sha512su1` | Same 64-bit shape | Paired two-word SHA-512 schedule updates |
| `eor3(a,b,c)`, `bcax(a,b,c)` | Signed or unsigned 128-bit integer vectors, with 8-, 16-, 32- or 64-bit lanes | `a ^ b ^ c`, or `a ^ (b & ~c)` |
| `rax1(a,b)` | `simd<std::uint64_t,2,Arch>` | `a ^ rotl(b,1)` per lane |
| `xar<Arch,Rotate>(a,b)` | Same 64-bit shape | `rotr(a ^ b,Rotate)` per lane; immediate 0–63 |

AES state has four consecutive bytes per column. `aese` and `aesd` XOR the
round key before substitution and row permutation; MixColumns is separate.
The final encryption round uses `aese` without `aesmc`, followed by the final
round-key XOR.

For polynomial products, bit `i` is the coefficient of `x^i`. There is no
integer carry or modular reduction. The 64-bit product puts coefficients 0–63
in result lane zero and 64–127 in lane one on either endian layout.

SHA-1 and SHA-256 state and schedule words occupy increasing lanes. Prepared
round words already include the message word plus its round constant.
`sha256h2` takes the original `abcd` state, before `sha256h` changes it.

SHA-512 uses packed pairs. For `sha512h(sum,fg,de)`, `fg` contains f,g and
`de` contains d,e in increasing lane order. `sum` contains the prepared k+w+h
terms for the later and earlier round, respectively; the high lane is processed
first. `sha512h2(sum,c_,ab)` also processes the high lane first and uses only
lane zero of `c_`. Schedule helpers produce two successive words.

```cpp
#include <cstdint>
import native.arm.aes;

constexpr auto aes_instruction = native::feature_closure(native::arm_feature::aes);
using bytes = native::simd<std::uint8_t,16,aes_instruction>;

__attribute__((target("aes"), noinline))
bytes round(bytes state, bytes key) {
  return native::aese<aes_instruction>(state, key);
}

bool supported() {
  return native::classify_isa(native::observe_arm_capabilities(),
    native::target_features<native::arm>("aes")).admitted();
}
```

## Caveats

These are individual instructions. Callers supply cipher modes, key expansion,
message padding and byte-order conversion. The operations do not alter FPCR,
FPSR or NZCV.

Hardware feature constraints and compiler targets differ. AES requires
`arm_feature::aes`, and 64-bit polynomial multiplication requires
`arm_feature::pmull`; Clang's `"aes"` target enables both plus NEON. The byte
polynomial forms require only NEON. SHA-1 requires `sha1` and SHA-256 requires
`sha2`; the `"sha2"` target enables both plus NEON. SHA-512 requires `sha512`
and the SHA-3 transforms require `sha3`; the `"sha3"` target enables both plus
SHA-1, SHA-256 and NEON.

Admit the whole `target_features` set and any inherited compiler minimum before
entering the target function: the compiler may use sibling instructions enabled
by that target. PMULL alone does not admit `"aes"`. The hardware names `pmull`,
`sha1` and `sha512` are not supported standalone target strings. Importing a
module neither enables the caller's target nor performs runtime dispatch.

Scalar `sha1h` and scalar-input `pmull` may omit `Arch`; their default is the
owning module's captured `NATIVE_BASELINE`. An importer's target scope cannot
change it. The polynomial result retains that default tag. Vector calls deduce
`Arch` from their operands.

All operations support constant evaluation. Without an instruction feature,
`polyfill` permission enables runtime software evaluation, including byte
polynomial multiplication. Without permission, missing-feature calls remain
`consteval`-only where storage exists. Native instruction paths retain priority.
Table-based cipher polyfills make no constant-time guarantee. Explicitly convert
vector types when a bit reinterpretation is intended.

See the [Arm Neon Intrinsics Reference](https://arm-software.github.io/acle/neon_intrinsics/advsimd.html)
and [Arm A64 instruction reference](https://documentation-service.arm.com/static/67e40f3398aa3c3b6eea6a85).
