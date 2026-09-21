# RDM instruction fixture

The `native.arm.rdm`, `native.arm` and `native` variants exercise all thirty-six
SQRDMLAH/SQRDMLSH overloads and every legal immediate lane. Each of the twelve
base shapes uses 2,048 directed/edge/random input sets. An independent `__int128`
oracle retains the full product and accumulator before its rounding shift and
final clamp. Sixteen directed triples run through every shape and lane,
covering both signs of ties, final saturation and minimum-times-minimum
cancellation. Explicit FPSR checks cover saturation with a discarded result,
stickiness, cancellation without saturation and a saturating vector lane form;
the status-check helper restores its incoming FPSR.

Direct public type checks reject raw registers and mixed architectures.
Compile-time participation checks reject absent/unrelated features and invalid
lanes, including ordinary-call checks that prevent Clang's lax vector conversions
or scalar narrowing from selecting halfword overloads for invalid word lanes.
A separate negative compilation checks those calls with deduced return types.
Synthetic observations reject missing and unobserved RDM. Native entry
requires observation/classification of RDM and the translation-unit minimum;
unavailable observations return skip code 77. Skips are not runtime evidence.

All thirty-six code-generation symbols start with `-mcpu=generic
-march=armv8-a+nodotprod+nordm` and enable only `target("rdm")`. Predefined-feature
guards reject an accidentally strengthened fixture. The object check requires
exactly one SQRDMLAH/SQRDMLSH, its selected native lane where applicable, no helper
calls and no unrelated optional instruction family. The negative caller has
the correct template feature but no function target, and must diagnose the
missing `rdm` target.

Run `ctest --test-dir build -R 'native.arm.rdm' --output-on-failure` after building
the family targets. As a standalone project this directory finds an installed
`native` package and checks the granular/omnibus modules, without private headers.

Qualification: Clang 23.1.1/CMake 4.4.3 on AArch64 macOS passed all eight tests with
actual execution. Clang's ACLE and direct-builtin alternatives failed the genuine
baseline probe because they required `v8.1a`; the exact-instruction implementation
passes. The separate `big_endian_codegen` test reuses the actual instruction
header bodies while stubbing only ISA metadata/attributes for freestanding
cross-compilation. It compares 15,360 interpreted register operations against
broadly targeted ACLE assembly across 120 entry forms (all shapes and lanes). This checks
compiler register/byte mapping, not native big-endian runtime behavior; no
Windows/Linux execution is claimed. See
[semantics and the compiler workaround](../../docs/arm-rdm.md).

The zero-overhead test compares paired public `simd` and raw-helper leaves under
the same target and calling context. It requires identical full instruction
sequences, including register moves and memory traffic.
