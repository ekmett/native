# DotProd instruction fixture

The header, `native.arm.dotprod` and `native` variants run the same exact checks
for twelve SDOT/UDOT overloads and every immediate lane. Each of the four base
shapes uses 2,048 directed/edge/random input sets. The oracle multiplies scalar
bytes and accumulates as `uint32_t`, explicitly checking signed and unsigned
wraparound. Compile-time checks reject absent/unrelated features and invalid
lanes, and synthetic observations reject unavailable or unobserved capabilities.

Runtime entry requires `observe_arm_capabilities()` and `classify_isa()` to admit
both DotProd and the translation-unit minimum; unavailable observations return
skip code 77. Skips are not native execution evidence.

The twelve code-generation symbols compile with `-mcpu=generic
-march=armv8-a+nodotprod+nordm`, enabling only `target("dotprod")` per function.
Predefined-feature guards reject an accidentally strengthened fixture. The
object check requires exactly one SDOT/UDOT, the selected native lane where
applicable, no helper calls and no unrelated optional instruction family.
The negative-target test recompiles a caller without its function target and
requires the missing-feature diagnostic.

Run `ctest --test-dir build -R 'native.arm.dotprod' --output-on-failure` after
building the family targets. This directory also configures as a standalone
installed consumer with `native_DIR` set to the package: that mode exercises the
granular and omnibus modules, without private implementation headers.

Qualification: Clang 23.1.1/CMake 4.4.3 on AArch64 macOS passed all five tests with
actual runtime execution. Other operating systems and big-endian execution are
not newly qualified. See [the API](../../docs/arm-dotprod.md).
