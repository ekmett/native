# Explicit polyfills

`polyfill` lets an algorithm use operations its target lacks. Add it to an ISA
with `A | polyfill`, or use `polyfill` alone for scalar storage. The feature bits
still describe real hardware requirements. Permission to emulate an instruction
does not grant permission to execute it.

```cpp
import native;
using namespace native;

using emulated = simd<float,16,polyfill>;
// In an x86 build:
using split = simd<float,16,avx2 | polyfill>;
```

The second type holds two AVX2 registers. Without permission, that shape is
unavailable on AVX2. Operations use their native implementation when the ISA
and storage support it, and otherwise use the semantic fallback. The same
permission works for scalar instruction interfaces such as `pext` and `pdep`.
Their architecture argument remains explicit when it differs from the build
baseline.

## Operations

| Family | Permitted fallback |
| --- | --- |
| SIMD values | Scalar or register-decomposed storage, arithmetic, comparisons, masks, transfers, shuffles and reductions; each operation retains its element constraints |
| Math | The existing staged kernels operate on permitted SIMD values, including `wide` batches |
| ARM integer | CRC32/CRC32C, AES, PMULL, SHA, SM3/SM4, DOTPROD, I8MM, RDM and NEON bit operations |
| ARM floating point | JSCVT, FP16FML, FCMA and BF16 instruction interfaces, preserving the documented floating-point controls |
| x86 scalar | BMI1/BMI2, POPCNT, LZCNT, CRC32C and ADX |
| x86 vector | Integer and cryptographic instruction families, including GFNI, carryless multiplication, VNNI, IFMA, VBMI/VBMI2 and conflict detection |
| x86 conversions | F16C and AVXNECONVERT |
| x86 indexed memory | Gather and scatter with byte-scaled signed indices; masked-off lanes do not access memory |
| WebAssembly | SIMD128 and the documented deterministic choices for relaxed SIMD operations |

The [SIMD guide](modules.md) describes storage and customization. Instruction
family pages specify operand shapes, immediate bounds and individual semantics.
Permission does not relax those constraints or turn BF16 into an elementwise
arithmetic type.

## Caveats

Emulation can take many instructions. Omit `polyfill` when native instruction
availability is part of the contract. Runtime dispatch and `target<>` continue
to test hardware features; this flag neither changes compiler target attributes
nor makes an unsupported hardware instruction safe to call.

Floating-point emulation follows the operation's result and rounding contract.
As elsewhere in Native, exception flags are unspecified and traps must be
disabled. Environment access, waiting and other hardware effects are not
replaced by numerical approximations.

Cryptographic fallbacks do not promise constant-time execution. In particular,
substitution tables may use data-dependent memory accesses. Relaxed Wasm
fallbacks choose permitted results; they need not reproduce every engine's
choice bit for bit.

Indexed-memory fallbacks retain byte addressing and inactive-lane suppression.
Scatter fallbacks process lanes in ascending order, so later active lanes win
where destination bytes overlap. All active accesses still require valid memory.
