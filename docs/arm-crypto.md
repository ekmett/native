# ARM cryptographic instructions

`native.arm.aes`, `native.arm.pmull` and `native.arm.sha` expose individual
Advanced SIMD instructions. They are also exported by `native.arm` and `native`.
The vector arguments and results use `simd<T,N,Arch>`, preserving the same
feature tag throughout each operation. Scalar SHA-1 and polynomial operands
use C++ integer types. Each module reexports `native.simd`.
The scalar `sha1h(word)` and scalar-input `pmull(a,b)` forms may omit `Arch`.
Their defaults are captured from `NATIVE_BASELINE` by the owning module, and
retain their respective runtime SHA-1 and PMULL requirements. The polynomial result keeps
that exact default tag in `simd<std::uint64_t,2,Arch>`. An importing target scope
does not change a module's default; optional instruction leaves can still give
an explicit ISA. Vector arguments continue to deduce their own `Arch`.
They perform integer operations without changing FPCR, FPSR or NZCV.

`sha1h` also supports constant evaluation. Its feature-capable overload is
`constexpr` and still emits SHA1H for runtime input. A separate `consteval`
overload permits constant words with an ISA that lacks SHA-1, using the same
32-bit rotation. Runtime values with that ISA are rejected, even though the
immediate overload can appear in an unevaluated `requires` expression. Scalar
width and signedness checks remain exact. Scalar-input `pmull` likewise evaluates constant operands with or without the
PMULL feature, provided `simd<std::uint64_t,2,Arch>` is an available storage
shape. On AArch64 this still requires NEON in `Arch`. Its constant result retains
the same tag and all 128 polynomial coefficients. An ISA without PMULL gets a
`consteval` overload, so runtime operands are rejected. Feature-capable runtime
calls keep the native instruction. Vector-input polynomial forms and other SHA
and AES operations retain their instruction requirements.

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

Only call `round` after `supported` succeeds. The module provider and admission
code retain the project's baseline target. Wrong register shapes, unsupported
features and invalid `xar` rotation immediates are rejected; convert vector types
explicitly when a bit reinterpretation is intended. All vector operands must carry the same `Arch`; raw intrinsic vectors are not
public overloads. The implementation headers are private to the module provider.

## AES and polynomial state

AES functions use a `simd<std::uint8_t,16,Arch>` state with four consecutive bytes per column.
`aese` and `aesd` XOR the round key before the substitution and row permutation.
They do not include MixColumns. `aesmc` and `aesimc` perform that separate forward
or inverse column transform. The final encryption round therefore uses `aese`
without `aesmc`, followed by the final round-key XOR.

For polynomial products, bit *i* is the coefficient of *x^i*. No integer carry or
modular reduction is performed. `pmull<Arch>(std::uint64_t, std::uint64_t)` returns
`simd<std::uint64_t,2,Arch>`, with coefficients 0–63 in lane zero and 64–127
in lane one on either endian layout. `pmull2` takes that same vector shape,
multiplies lane one from each operand and ignores lane zero. The byte forms
take `simd<std::uint8_t,8,Arch>` or the high half of
`simd<std::uint8_t,16,Arch>` and return `simd<std::uint16_t,8,Arch>`; they
require only baseline NEON.

## SHA state and schedule

SHA-1 and SHA-256 use `simd<std::uint32_t,4,Arch>` vectors and place consecutive 32-bit state or schedule words in
increasing lane order. Each round function processes four rounds; `wk` already
contains the message word plus its round constant. `sha256h2` takes the original
`abcd` state, not the result of `sha256h`. The paired schedule helpers generate
four successive schedule words.

SHA-512 helpers use `simd<std::uint64_t,2,Arch>` to process pairs of 64-bit words. For `sha512h(sum, fg, de)`, the
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
