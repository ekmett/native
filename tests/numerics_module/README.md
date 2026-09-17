# Scalar numerics module tests

`simd.numerics` owns `fp16`/`bf16`, their numeric limits and `std::isnan`
overloads, and scalar predicate/scaling utilities. It re-exports `simd.types`.
Rebuild the archive and consumer BMIs together after a compiler or configuration
change; module-defined types must not be mixed with stale textual definitions.

The root suite's `simd.numerics.import` test runs the exhaustive half reference
bank, eight FP environments, ADL/noexcept controls and limits checks.
`simd.numerics.api` checks specialization visibility, literals, predicates,
defaults, assignment, swap and fast conversion through imports. Run with
`ctest --test-dir <build> -R simd.numerics --output-on-failure` after the normal
root build. Use separate exception-ON/OFF and ASan configurations as needed.

The three `*_legacy_checks.h` files retain embedded Catch bodies as test-only
reference banks; they are not additional active CTests. Scalar `scalef` has
constexpr/runtime and boundary limitations and is not the repaired FTZ SIMD
`scaleb` contract. These tests make no GPU or timing claim.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
