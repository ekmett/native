# ARM crypto regression fixture

The same scalar-reference checks run through a textual implementation header,
each granular module and the `native` hub. Installed-package mode runs the two
module consumers. Runtime entry points are admitted using the full compiler
`aes`, `sha2` and `sha3` feature bundles; unsupported hosts return CTest skip code
77 before entering a crypto leaf.

The scalar oracles compute AES substitution through GF(2^8) inversion and affine
maps, AES column transforms, unreduced binary polynomial products, SHA rounds and
schedules, and SHA-3 Boolean/rotate operations. Inputs cover every AES substitution
byte, polynomial basis vectors, zero/all-one products, and 512 seeded cases per
family. Every XAR immediate is exercised. FPCR and FPSR are checked for retention.

The code-generation fixture instantiates every overload and all 64 XAR rotations
from a baseline ARMv8-A translation unit. Its disassembly check verifies expected
instructions and a baseline control function. Separate compile-failure fixtures
cover absent `Arch` features, wrong register shapes, missing compiler targets and
an out-of-range rotation. Availability checks include every raw overload.

See the [ARM crypto guide](../../docs/arm-crypto.md) for state layout and feature
requirements. These tests qualify instruction wrappers; they do not constitute a
complete cipher, hash protocol or cryptographic validation suite.
