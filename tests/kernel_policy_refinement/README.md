# Kernel policy refinement checks

Both consumers compile the same bodies from an unchanged translation-unit
minimum: one includes the source-tree implementation headers and one imports the public hub. Each native
case uses a literal target scope; runtime execution requires CPU and OS admission
for the compiled scope plus the inherited minimum. Supplemental half features
remain in the full Arch identity, but FP32 callers omit half target attributes.
Executing these cases does not exercise native half arithmetic.

The cases compare wide arithmetic, FMA, square root, absolute value and rounding
against the corresponding element operations by lane bits. They also instantiate
classification, mask-only bit operations, selection, broadcasts, apply,
converting constructors, nested packs and zero/one/multiple register extents.
The operation graph is the reference; these are not libm accuracy tests.

A separate test value domain over each raw vector checks array-first ADL for
add and abs without an implicit raw conversion. Scalar custom domains check
array-call counters, converting construction, empty packs and exception
propagation/cleanup when exceptions are enabled. This is a test-only FTZ-like
extension boundary, not FTZ numerical qualification.

The explicit target matrix covers AVX2; AVX512 F/DQ with all four BW/VL
combinations; optional BF16, FP16 and both; and conditional A64 NEON, BF16,
FP16 and both. The half-tagged FP32 cases use only corresponding raw BW/VL or NEON scopes.
The custom wrapper explicitly publishes its raw value requirements; unknown
custom domains remain conservative. Cross-Arch, lane-width and pack-extent
mismatches are negative requires checks. The header checks pair<vec&,int>
requirement recursion. Native half arithmetic has separate production fixtures.

Rounding adapters floor/ceil/trunc intentionally use per-element ADL; the fixture
checks that they do not switch to array hooks. Other available array-first math
adapters assert one batch call. A source-tree-only header target is omitted when
this fixture is used against an installed package lacking private headers.
