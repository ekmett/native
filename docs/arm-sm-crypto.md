# ARM SM3 and SM4 instructions

Import `native.arm.sm3` or `native.arm.sm4`, or use the `native.arm` / `native`
hub, and link `native::native`. Every operand and result is
`simd<std::uint32_t,4,Arch>`. These are individual Advanced SIMD instructions;
callers supply message padding, byte-order conversion, round constants, complete
key schedules and cipher modes.

| Module | Public operation | Meaning |
| --- | --- | --- |
| `native.arm.sm3` | `sm3ss1(a,b,c)` | Rotate `a[3]` left by 12, add `b[3]` and `c[3]`, rotate the sum left by 7; return it in lane 3 with lanes 0–2 zero |
| `native.arm.sm3` | `sm3tt1a<Arch,Lane>(state,ss1,words)` | One early ABCD round, using parity and `words[Lane]` |
| `native.arm.sm3` | `sm3tt1b<Arch,Lane>(state,ss1,words)` | One late ABCD round, using majority and `words[Lane]` |
| `native.arm.sm3` | `sm3tt2a<Arch,Lane>(state,ss1,words)` | One early EFGH round, using parity, `words[Lane]`, and the P0 permutation |
| `native.arm.sm3` | `sm3tt2b<Arch,Lane>(state,ss1,words)` | One late EFGH round, using choice, `words[Lane]`, and the P0 permutation |
| `native.arm.sm3` | `sm3partw1(a,b,c)` | First four-word message-schedule update, including dependent fourth-word feedback |
| `native.arm.sm3` | `sm3partw2(a,b,c)` | Finish the four-word schedule update and its fourth-word correction |
| `native.arm.sm4` | `sm4e(state,keys)` | Four data rounds, consuming four consecutive round keys |
| `native.arm.sm4` | `sm4ekey(state,constants)` | Four key-schedule rounds, consuming four consecutive schedule constants |

`Lane` is an immediate in `[0,3]`. All operations preserve the complete `Arch`
tag. Signed vectors, other lane counts, mixed tags and out-of-range immediates
are rejected. There are no scalar or 64-bit-vector forms for these instructions.

SM3 TT state lanes are reversed: `{D,C,B,A}` or `{H,G,F,E}`. The `ss1` argument
holds SS1 in lane 3. TT1 consumes the derived message word `W[j] ^ W[j+4]`;
TT2 consumes `W[j]`. For the schedule block starting at `j`, use
`sm3partw1(W[j-16..j-13], W[j-9..j-6], W[j-4..j-1])`, then pass that result to
`sm3partw2` with `W[j-6..j-3]` and `W[j-13..j-10]`. The fourth output depends
on the newly computed first output.

SM4 state, key and constant words occupy ascending lanes. `sm4e` returns the
four newly computed state words after four rounds. Reverse the final four state
words after all 32 rounds to obtain the standard ciphertext word order.
Decryption uses `sm4e` with round keys in reverse order. No wrapper performs
byte swapping or a final state reversal.

Runtime calls require `arm_feature::sm3` or `arm_feature::sm4` as appropriate,
and a caller compiled for Clang's `"sm4"` target. Hardware observations remain
independent: each feature implies NEON storage, but neither hardware bit implies
the other. The compiler target enables both, so
`target_features<arm>("sm4")` requires **NEON, SM3 and SM4**. The name `"sm3"`
is not a standalone Clang target and is rejected by `target_features`.

```cpp
#include <cstdint>
import native.arm.sm4;

constexpr auto requirements = native::target_features<native::arm>("sm4");
using words = native::simd<std::uint32_t,4,requirements>;

__attribute__((target("sm4"), noinline))
void four_rounds(std::uint32_t* state, std::uint32_t const* keys) {
  native::sm4e(words::load(state), words::load(keys)).store(state);
}

bool try_four_rounds(std::uint32_t* state, std::uint32_t const* keys) {
  auto cpu = native::observe_arm_capabilities();
  if (!native::classify_isa(cpu, requirements).admitted()) return false;
  four_rounds(state, keys);
  return true;
}
```

Linux observes `HWCAP_SM3` and `HWCAP_SM4` independently. macOS queries
`hw.optional.arm.FEAT_SM3` and `hw.optional.arm.FEAT_SM4`; missing keys remain
unknown. Windows has no corresponding selectors in this observer and also
leaves them unknown. Neither a CPU model nor another crypto feature authorizes
the instructions. Importing a module does not strengthen the caller's target.

All nine operations support constant evaluation with the same integer results.
Without the instruction feature, overloads are immediate-only and still need
NEON storage, for example `simd<std::uint32_t,4,native::neon>`. There is no scalar
runtime fallback. These integer instructions do not read or modify FPCR, FPSR
or NZCV. Constant-evaluation substitution tables are not a runtime cipher
implementation or a timing guarantee.

The [validation fixture](../tests/arm_sm_crypto/README.md) covers all nine
operations and all four TT lane immediates: 21 exact raw/public assembly pairs,
2,016 independent reference inputs evaluated with strong and weak feature tags,
complete SM3 and SM4 known answers, feature/target/shape/immediate rejection,
compiler minimum metadata, and installed named-module consumers. Hardware
execution is gated by the whole compiler target; a skip records missing
admission and does not establish hardware execution.

Instruction semantics follow Arm's
[A64 instruction specification](https://documentation-service.arm.com/static/67e40f3398aa3c3b6eea6a85)
and [ACLE intrinsic mapping](https://arm-software.github.io/acle/neon_intrinsics/advsimd.html).
The SM3 and SM4 known answers use the examples reproduced in the
[SM3 draft](https://datatracker.ietf.org/doc/html/draft-sca-cfrg-sm3-01#appendix-A.1)
and [SM4 draft](https://datatracker.ietf.org/doc/html/draft-ribose-cfrg-sm4-10#appendix-A.1).
