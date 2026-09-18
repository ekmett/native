# Production common refinement for wide binary32 exp

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->

The public x86 `simd::exp(wide<vec<float,L,Arch>,N>)` overloads use an internal
common refinement of actual callee declaration policies. The original Arch is
retained; a selected minimal tag never replaces the input or output type.
`src/simd/abi_refinement.h` owns the internal metadata computation and
`src/simd/exp_policies.h` records operation summaries. No new public refinement
API is exported. The unchanged array kernel still owns every arithmetic step.

Comparison/select, multiply/FMA, rounding and scaling currently inherit the same
five raw backend partitions. Their named summaries are checked for equality and
composed once with the result-constructor partition, rather than multiplying
identical policies for each polynomial stage. VL is a real narrow-mask/scaling
boundary. BW is still a backend attribute obligation, and BF16/FP16 remain
result-constructor attribute obligations even for binary32; this patch does not
claim they are new exponential algorithms or remove those requirements.

For each Cartesian tuple, it unions the closed positive requirements of the
selected entries. A mixed x86/ARM union is discarded before creating a tag.
Otherwise that minimal tag is passed to the existing `abi_lookup` for each
original list. The tuple survives precisely when all those selections match it.
Remaining cells keep lexicographic tuple order and become an `isa_list` of
minimal requirement tags. `target_entry` minima are included in the union.

This criterion is complete for ordered lists with positive requirements: any
tag realizing a tuple contains its minimal union. If an earlier entry already
matched that union, it would also match the larger tag, contradicting the tuple.
Conversely an exact match at the union is itself a witness. For selection, if a
cell's requirements fit tag A, each callee's selected ordinal at A is no greater
than that cell's ordinal. A's own realized tuple therefore occurs first in
lexicographic order. Reordering cells by feature count breaks this property.

`metadata.cc` checks incomparable priorities, reversed order, explicit fallback,
inherited minima, mixed-host rejection, empty lists, and 128 exp boundary tags.
Its smaller-priority-feature example fails if larger feature sets are preferred
over declared order. A no-match remains a no-match; no fallback is invented.

The production refinement composes two distinct dependency partitions:

- Five x86 raw-exp backend scopes from `src/simd/simd/for_each_backend.h`.
- Eleven wide-result constructor scopes from `src/simd/wide.h` and
  `wide_members.h`, including the required BF16/FP16 combinations.

Their 55 tuples produce 11 cells. The internal policy header has a complete source-name and
literal-target record for those cells; static assertions compare every record
with the computed union and both original ordinals. C++ does not convert the
computed strings into attributes. Existing `SIMD_TARGET_PUSH` uses the written
literal. This first native fixture requires an inherited translation-unit
minimum no stronger than AVX2/FMA/BMI2, diagnosed explicitly.

Each generated public `simd::exp` overload is constrained by its disjoint selected
ordinal, retains the full caller architecture in its types, calls the unchanged
array exp kernel, and constructs the same wide result. Its conditional `noexcept`
covers that full expression and is checked against legacy generic exp for every tested
shape. Lane count, register
count, and Flush remain independent template parameters. Current helper and
constructor target requirements remain intact; no BW or half-feature case was
merged away. Scalar, custom ADL and ARM extension paths retain the existing generic wide
implementation. Runtime checks cover a tagged array customization, an untagged
element fallback, scalar values and conditional noexcept. Flush=true must not
accept an untagged-only customization.

The native test compiles all cells and executes only CPU/OS-admitted cases.
Separate non-inline production/reference entry points compare exact outputs for
one-lane, short two/three-lane, four/eight/sixteen-lane shapes where supported;
zero, one and three registers; both Flush values; and boundary/special inputs.
The reference explicitly calls `simd::exp<Flush,V,N>`, selecting the unchanged
legacy generic template by its type argument; it cannot select the new concrete
vec overload, whose second parameter is a lane-count value. Function-pointer
static assertions prove ordinary `simd::exp<Flush>` selects that new overload,
and has a different function identity from the legacy generic route. The `simd::math::exp`
export is checked to select the same new overload as `simd::exp`. Thus the
comparison does not accidentally call the new overload twice.

It also exercises an original caller tag with extra AES features. Floating-point
control bits must remain unchanged. Unsupported case counts are printed.

Empty inputs use explicit `wide<V,0>{std::array<V,0>{{}}}` construction to keep
this comparison independent of the default-constructor regression. The companion
fix `f6283f2` (integrated as `954377a`) covers the former Clang/MSVC STL
empty-storage attribution failure in `tests/wide_construction`. This exp-source
checkpoint does not change constructors or require their separate fix; its
explicit empty-array setup also works on the original constructor baseline.

The same native object supplies 22 code-generation entries, comparing eight-lane
and native-width forms. The checker requires packed FMA, native SCALEF exactly
at the existing F/VL cut points, expected register widths and no outlined helper
calls in uninstrumented builds. It does not claim fewer concrete instantiations.
The native fixture currently targets x86; ARM participates in metadata checks.

The separate import-only fixture uses only public `simd/targets.h`, standard
headers and `import simd`; it includes no private refinement/policy definitions.
It verifies both exported names choose the concrete overload and evaluates it.
This migration specializes the public hub module. Internal textual vec/wide
consumers keep the existing generic route; no header-only native public API is
introduced.
