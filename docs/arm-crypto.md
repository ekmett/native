# ARM cryptographic instructions

`native.arm.aes`, `native.arm.pmull` and `native.arm.sha` expose individual
Advanced SIMD instructions. They are also exported by `native.arm` and `native`.
The functions take ACLE register types and an explicit `isa` template argument.
They perform integer operations without changing FPCR, FPSR or NZCV.

These operations do not implement a cipher mode, key expansion, message padding,
byte-order conversion or a complete hash. Their input state and prepared round
words must already have the form required by the instruction. The
[Arm Neon Intrinsics Reference](https://arm-software.github.io/acle/neon_intrinsics/advsimd.html)
and [Arm A64 instruction reference](https://documentation-service.arm.com/static/67e40f3398aa3c3b6eea6a85)
define the corresponding transformations.

## Features and caller targets

An operation's `Arch` constraint names the hardware instruction feature.
Clang's target strings describe larger compiler bundles:

| Operations | `arm_feature` | Caller target | Required compiler bundle |
|---|---|---|---|
| `aese`, `aesd`, `aesmc`, `aesimc` | `aes` | `aes` | NEON, AES, PMULL |
| 64-bit `pmull`, `pmull2` | `pmull` | `aes` | NEON, AES, PMULL |
| Byte `pmull`, `pmull2` | `neon` | baseline AArch64 | NEON |
| `sha1c`, `sha1p`, `sha1m`, `sha1h`, `sha1su0`, `sha1su1` | `sha1` | `sha2` | NEON, SHA-1, SHA-256 |
| `sha256h`, `sha256h2`, `sha256su0`, `sha256su1` | `sha2` | `sha2` | NEON, SHA-1, SHA-256 |
| `sha512h`, `sha512h2`, `sha512su0`, `sha512su1` | `sha512` | `sha3` | NEON, SHA-1, SHA-256, SHA-512, SHA-3 |
| `eor3`, `bcax`, `rax1`, `xar` | `sha3` | `sha3` | NEON, SHA-1, SHA-256, SHA-512, SHA-3 |

Check the whole `target_features` set before calling a target-attributed leaf.
The compiler may use any instruction enabled in that leaf, including sibling
crypto features. Observing PMULL alone does not admit the compiler's `aes`
bundle. Hardware-only names `pmull`, `sha1` and `sha512` are not supported target
strings.

```cpp
#include <arm_neon.h>
import native.arm.aes;

constexpr native::isa aes_instruction{native::arm_feature::aes};

__attribute__((target("aes"), noinline))
uint8x16_t round(uint8x16_t state, uint8x16_t key) {
  return native::aese<aes_instruction>(state, key);
}

bool supported() {
  return native::classify_isa(native::observe_arm_capabilities(),
    native::target_features("aes")).admitted();
}
```

Only call `round` after `supported` succeeds. The module provider and admission
code retain the project's baseline target. Wrong register shapes, unsupported
features and invalid `xar` rotation immediates are rejected; convert vector types
explicitly when a bit reinterpretation is intended. For `native::simd` values,
pass `to_native()` and explicitly construct the result from the returned register.

## AES and polynomial state

AES functions use a `uint8x16_t` state with four consecutive bytes per column.
`aese` and `aesd` XOR the round key before the substitution and row permutation.
They do not include MixColumns. `aesmc` and `aesimc` perform that separate forward
or inverse column transform. The final encryption round therefore uses `aese`
without `aesmc`, followed by the final round-key XOR.

For polynomial products, bit *i* is the coefficient of *x^i*. No integer carry or
modular reduction is performed. `pmull(poly64_t, poly64_t)` returns `poly128_t`;
`pmull2(poly64x2_t, poly64x2_t)` multiplies the high lanes and ignores the low
lanes. `pmull(poly8x8_t, poly8x8_t)` and the high-half `poly8x16_t` form return
eight `poly16_t` products and require only baseline NEON.

## SHA state and schedule

SHA-1 and SHA-256 vectors place consecutive 32-bit state or schedule words in
increasing lane order. Each round function processes four rounds; `wk` already
contains the message word plus its round constant. `sha256h2` takes the original
`abcd` state, not the result of `sha256h`. The paired schedule helpers generate
four successive schedule words.

SHA-512 helpers process pairs of 64-bit words. For `sha512h(sum, fg, de)`, the
high lane is processed first: `fg` contains f,g and `de` contains d,e in increasing
lane order. `sum` contains the prepared k+w+h terms for the later and earlier
round, respectively. `sha512h2(sum, c_, ab)` likewise produces the high lane before
the low lane; only lane zero of `c_` participates. The schedule helpers produce
two successive words. These packed intermediate forms differ from the four-word
SHA-256 round interface.

`eor3` computes `a ^ b ^ c`; `bcax` computes `a ^ (b & ~c)`. Both support signed
and unsigned 128-bit integer vectors with 8-, 16-, 32- or 64-bit elements.
`rax1` computes `a ^ rotl(b, 1)` in each unsigned 64-bit lane.
`xar<Arch, Rotate>` computes `rotr(a ^ b, Rotate)` with a compile-time rotation
from 0 through 63.

The [crypto tests](../tests/arm_crypto/README.md) check the individual operations,
compiler admission boundaries and installed module consumers.
