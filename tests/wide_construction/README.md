# Wide default construction

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->

These header and module consumers compile at the configured project minimum.
Native calls receive explicit source target scopes and execute only after CPU/OS
admission. Empty floating, integer, mask and nested packs cover every existing
x86 or ARM constructor family. The original failing case is default/value
construction of `wide<vec<float,1,avx512>,0>` followed by public exp at a weaker
translation-unit target.

MSVC's `array<T,0>` contains a dummy element. Its implicit default constructor
can therefore call a targeted nontrivial `T` constructor from an unattributed
function. Only nontrivial, default-constructible empty storage containing such a
`T` receives aggregate initialization inside wide's existing target family.
Trivial/deleted defaults and nonempty packs keep the defaulted constructor.
The array member, layout, and element initialization rules remain unchanged.

The tests compare layout, default/copy/move/assignment/destructor traits and
exception specifications with the former storage/default-constructor form.
They check constant evaluation, scalar value initialization, member initializers,
deleted and move-only types, and observable custom construction/destruction.
N=0/1/3 and default/value initialization are covered independently.

With exceptions enabled, ordinary throwing construction is checked in-process.
Empty throwing construction runs in child processes: MSVC's original empty
array reports `noexcept(true)` yet can construct a throwing dummy element.
The test compares both the returned effects and this existing termination
behavior using a deterministic terminate handler. It does not change that
standard-library contract. With exceptions disabled the same cases check
ordinary construction effects and the declared exception specifications.
