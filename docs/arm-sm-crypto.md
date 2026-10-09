# ARM SM3 and SM4: hash rounds and block-cipher transforms

[ARM instruction sets](arm.md)

## Why use it

SM3 combines rotations, Boolean functions and a dependent message schedule.
SM4 repeatedly applies its nonlinear round transform to four words of state.
Their dedicated instructions handle these inner transforms while letting the
caller manage blocks, schedules and modes.

## Operations

Import `native.arm.sm3` or `native.arm.sm4`, or use the `native.arm` or `native`
hub. Every operand and result is `simd<std::uint32_t,4,Arch>` with a common `Arch`.

| Operation | Meaning |
| --- | --- |
| `sm3ss1(a,b,c)` | Rotate `a[3]` left by 12, add `b[3]` and `c[3]`, rotate the sum left by 7; return it in lane 3 with lanes 0–2 zero |
| `sm3tt1a<Arch,Lane>(state,ss1,words)` | One early ABCD round, using parity and `words[Lane]` |
| `sm3tt1b<Arch,Lane>(state,ss1,words)` | One late ABCD round, using majority and `words[Lane]` |
| `sm3tt2a<Arch,Lane>(state,ss1,words)` | One early EFGH round, using parity, `words[Lane]` and P0 |
| `sm3tt2b<Arch,Lane>(state,ss1,words)` | One late EFGH round, using choice, `words[Lane]` and P0 |
| `sm3partw1(a,b,c)` | First four-word schedule update, including dependent fourth-word feedback |
| `sm3partw2(a,b,c)` | Finish the schedule update and its fourth-word correction |
| `sm4e(state,keys)` | Four data rounds using four consecutive round keys |
| `sm4ekey(state,constants)` | Four key-schedule rounds using four consecutive schedule constants |

`Lane` is an immediate in 0–3. SM3 TT state lanes are reversed: `{D,C,B,A}` or
`{H,G,F,E}`. The `ss1` argument holds SS1 in lane 3. TT1 consumes
`W[j] ^ W[j+4]`; TT2 consumes `W[j]`. For a schedule block starting at `j`, use
`sm3partw1(W[j-16..j-13], W[j-9..j-6], W[j-4..j-1])`, then pass that result to
`sm3partw2` with `W[j-6..j-3]` and `W[j-13..j-10]`. The fourth output depends
on the newly computed first output.

SM4 state, key and constant words occupy ascending lanes. `sm4e` returns the
four newly computed state words after four rounds. Reverse the final four words
after all 32 rounds to obtain the standard ciphertext word order. Decryption
uses `sm4e` with reversed round keys.

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
  if (!native::classify_isa(native::observe_arm_capabilities(), requirements).admitted())
    return false;
  four_rounds(state, keys);
  return true;
}
```

## Caveats

Callers supply message padding, byte-order conversion, round constants,
complete key schedules and cipher modes. No wrapper performs byte swapping
or SM4's final state reversal. There are no scalar or 64-bit-vector forms.

Native runtime calls require `arm_feature::sm3` or `arm_feature::sm4`, respectively,
and a `"sm4"` caller target. Hardware observations are independent, but that
compiler target enables both: `target_features<arm>("sm4")` requires NEON,
SM3 and SM4. There is no standalone `"sm3"` target string. Admit the whole
compiler target and any inherited minimum before entering the function.

Linux and macOS observe SM3 and SM4 independently; missing queries remain
unknown. The Windows observer has no selectors for them and leaves them
unknown. A CPU model or another crypto feature cannot establish admission.
Importing a module does not enable the caller's target or perform dispatch.

All nine operations support constant evaluation. A tag without the instruction
feature permits only `consteval` calls unless it contains `polyfill`. Explicit
permission, such as `neon | polyfill`, enables the same complete four-word
semantics at runtime with the available storage. `polyfill` alone uses scalar
storage. It does not add SM3 or SM4 hardware feature bits. When the instruction
feature is present, the native instruction remains the selected runtime path.

These integer instructions do not read or modify FPCR, FPSR or NZCV. Semantic
SM4 substitution-table polyfills preserve values but make no constant-time
cipher guarantee.

See the [Arm A64 instruction specification](https://documentation-service.arm.com/static/67e40f3398aa3c3b6eea6a85)
and [ACLE intrinsic mapping](https://arm-software.github.io/acle/neon_intrinsics/advsimd.html).
