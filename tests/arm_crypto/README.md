# ARM crypto regression fixture

The same scalar-reference checks run through each granular module,
the `native.arm` architecture module and the `native` hub. Installed-package
mode runs all three consumers and the constant-evaluation property corpus. Each operation takes or returns semantic `simd`
values; oracle adapters convert the fixture’s intrinsic storage at the boundary.
The private implementation headers do not provide a standalone public SIMD API. Runtime entry points are admitted using the full compiler
`aes`, `sha2` and `sha3` feature bundles; unsupported hosts return CTest skip code
77 before entering a crypto leaf.

The scalar oracles compute AES substitution through GF(2^8) inversion and affine
maps, AES column transforms, unreduced binary polynomial products, SHA rounds and
schedules, and SHA-3 Boolean/rotate operations. Inputs cover every AES substitution
byte, polynomial basis vectors, zero/all-one products, and 512 seeded cases per
family. Every XAR immediate is exercised. FPCR and FPSR are checked for retention.

The constant corpus covers 102 operation/shape/immediate forms with 24 reproducible
full-bit generated cases per form. Public calls with the exact instruction tag
and with baseline NEON are compared in `static_assert` checks, together with zero,
all-one and every basis-bit input. The computed outputs are then compared with
native hardware calls. Failures report the seed, case index, expected and actual
bits, and all operands. AES inverse identities cover all substitution bytes;
known AES columns and complete SHA-1/SHA-256 `abc` digests provide fixed oracles
from [FIPS 197](https://csrc.nist.gov/pubs/fips/197/final) and
[FIPS 180-4](https://csrc.nist.gov/pubs/fips/180-4/upd1/final).
Actual compile-failure cases exercise every immediate-only vector overload with
runtime operands, retaining exact-shape, target and immediate-range checks.

The code-generation fixture instantiates every overload and all 64 XAR rotations
from a baseline ARMv8-A translation unit. Its disassembly check verifies expected
instructions and a baseline control function. It also compares exact sequences
against raw helpers under the same caller targets, rejecting loads, stores,
stack references and calls. The 103 instruction forms and two additional
PMULL vector-result probes must all match. Separate compile-failure fixtures
cover absent `Arch` features, wrong register shapes, missing compiler targets and
an out-of-range rotation. Availability checks include every semantic overload and reject raw vectors.
Every result is checked to retain its input feature tag; actual compile failures
also cover raw arguments and mismatched tags.

See the [ARM crypto guide](../../docs/arm-crypto.md) for state layout and feature
requirements. These tests qualify instruction wrappers; they do not constitute a
complete cipher, hash protocol or cryptographic validation suite.
