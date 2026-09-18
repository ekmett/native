# Experimental common refinement for binary32 exp

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->

This fixture prototypes operation-specific selection without changing the public
API or any production exp, vector, wide or numerical implementation. Its internal
`refinement<Lists...>` composes the selected ordinals of separate callee policy
lists. It does not concatenate those lists or choose by feature count.

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

The native prototype uses two actual dependencies:

- Five x86 raw-exp backend scopes from `src/simd/simd/for_each_backend.h`.
- Eleven wide-result constructor scopes from `src/simd/wide.h` and
  `wide_members.h`, including the required BF16/FP16 combinations.

Their 55 tuples produce 11 cells. `policies.h` has a complete source-name and
literal-target record for those cells; static assertions compare every record
with the computed union and both original ordinals. C++ does not convert the
computed strings into attributes. Existing `SIMD_TARGET_PUSH` uses the written
literal. This first native fixture requires an inherited translation-unit
minimum no stronger than AVX2/FMA/BMI2, diagnosed explicitly.

Each generated `refined_exp` overload is constrained by its disjoint selected
ordinal, retains the full caller architecture in its types, calls the unchanged
array exp kernel, and constructs the same wide result. Its conditional `noexcept`
covers that full expression and is checked against public exp for every tested
shape. Lane count, register
count, and Flush remain independent template parameters. Current helper and
constructor target requirements remain intact; no BW or half-feature case was
merged away. Scalar and custom ADL extension paths remain in the production
implementation, which this fixture never replaces.

The native test compiles all cells and executes only CPU/OS-admitted cases.
Separate non-inline candidate/reference entry points compare exact outputs for
one-lane, short two/three-lane, four/eight/sixteen-lane shapes where supported;
zero, one and three registers; both Flush values; and boundary/special inputs.
It also exercises an original caller tag with extra AES features. Floating-point
control bits must remain unchanged. Unsupported case counts are printed.

Empty inputs use explicit `wide<V,0>{std::array<V,0>{{}}}` construction. With
Clang 23.1.1 and MSVC STL 14.44.35207, the existing public default construction
`wide<vec<float,1,avx512>,0> value;` fails even inside `SIMD_TARGET_PUSH(avx512)`:
the STL empty-array storage invokes the targeted `vec` constructor from an
unattributed implicit array constructor. Existing array exp, public wide exp,
and the prototype wrapper compile for this empty shape. That separate constructor
limitation remains open; the fixture retains the complete zero-extent exp tests
without raising the translation-unit target or changing production constructors.

The same native object supplies 22 code-generation entries, comparing eight-lane
and native-width forms. The checker requires packed FMA, native SCALEF exactly
at the existing F/VL cut points, expected register widths and no outlined helper
calls in uninstrumented builds. It does not claim fewer concrete instantiations.
The native fixture currently targets x86; ARM participates in metadata checks.
