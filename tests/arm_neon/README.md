# NEON bit-operation checks

The `native.arm.neon.bits.*` tests cover `clz`, `cls`, `rbit`, `rev16`, `rev32`
and `rev64`: 46 concrete signedness, element-width and logical-vector-width
combinations. Production definitions remain in `native.arm.neon.ccm`.

`bits.cc` uses only public SIMD operands/results. Its independent reference scans
original bit positions for counts and reversals, and copies element blocks for
lane reversal. Explicit expected constexpr examples distinguish reversal of
whole elements from reversal of bytes inside them. Every shape also has a
constexpr boundary bank and 4,096 runtime vectors, repeated with initially clear
and populated FPSR. Every byte encoding occurs; larger elements receive a fixed
PRNG sequence. The test preserves the caller's FPSR. All three imports are built
and executed, and result-type checks retain a stronger architecture tag.

`bits_codegen.cc` pairs every public operation with its direct ACLE intrinsic.
`check_bits.py` requires the complete 46-name set, identical raw/public machine
instructions, and exactly one intended instruction followed by `ret`. No extra
movement, padding work, memory traffic or calls are accepted. These leaves use a
generic Armv8-A compiler target. The feature/target/shape compiler controls are
expected failures and check their diagnostics; participation assertions cover
additional signedness and width boundaries.

Build and run the focused checks:

```sh
cmake --build build-ninja-release --parallel --target \
  native_test_arm_neon_bits_neon native_test_arm_neon_bits_arm \
  native_test_arm_neon_bits_omnibus native_test_arm_neon_bits_codegen
ctest --test-dir build-ninja-release -R '^native.arm.neon.bits' --output-on-failure
```

The three runtime/module tests also run in the installed-package fixture.
Qualification on Apple M3 with Clang 23.1.1, CMake 4.4.3 and the MacOSX15.5 SDK
passed all seven bit-operation checks and four existing NEON regressions.
Code-generation comparisons and negative diagnostics are retained in the build
directory. This evidence covers little-endian AArch64; it does not establish
big-endian lowering or performance on other CPUs.

The supported intrinsic shapes and architectural meanings were checked against
[Arm's NEON intrinsic reference](https://arm-software.github.io/acle/neon_intrinsics/advsimd.html)
and the installed Clang 23 `arm_neon.h` declarations. The rest of this directory
continues to cover integer saturation, shifts, conversions and comparisons.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
