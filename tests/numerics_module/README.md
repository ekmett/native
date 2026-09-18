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
reference banks; they are not additional active CTests. `simd.numerics.scalef` pairs constant evaluation and volatile runtime calls through
`import simd.numerics` for float and double, including both signs, normal and
subnormal boundaries, ties-to-even, signed underflow, overflow, fractional
exponents, extreme valid int exponents, zeros, infinities and quiet NaNs.
Scalar `scalef` truncates its exponent toward zero; that truncated value must
fit in int. Its constexpr result assumes nearest-even and gradual underflow.
Runtime delegates to `std::scalbn`, so other rounding/denormal environments need
not agree. This is separate from the FTZ vector `scaleb` contract. These tests make no GPU or timing claim.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
