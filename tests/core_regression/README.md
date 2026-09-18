# Core regression tests

These tests cover raw SIMD arithmetic, scalar/vector/wide consistency, explicit
construction, masks and bit bridges, modular integers, guarded aligned and
unaligned memory, partial tails, scaling boundaries, special values and wide exponential results.
`reference.h` supplies an independent staged split-scale exponential oracle.
Failure paths print a diagnostic and abort, including with exceptions disabled.

The public shape is `simd::vec<T,N,Arch>` with an explicit ISA value.
Constructor tests supply lanes or arrays to the explicitly named vector type;
comparison results must be exactly `V::mask`.

Build from the repository root as described in [the test guide](../README.md).
Each ISA needs a separate configuration. Non-NaN results and signed zeros compare
exactly; numerical NaN comparisons ignore payload and sign. Raw scaling uses
`SIMD_RAW_SCALEB_FTZ_PROFILE` to select the previously observed hardware tininess
rule. Its tests cover gradual and flush modes, plus separate DAZ and FTZ on x86.
This selection describes the raw instruction rather than repairing its result.

The swizzle fixtures run through both headers and imports. Two- and three-lane
32-bit vectors have padded native storage, while guarded memory tests require
exact logical lane access. They check constexpr constructors, explicit ISA
selection, owning snapshots, same-name and overlapping scatter, rejection of
const/rvalue/repeated writes, and the absence of properties for padding lanes.
Static checks preserve existing vector layouts and reject unequal logical shapes.
Short arithmetic compares the active lanes of four-lane operations, with an
independent integer wrap oracle and a floating-point status check that benign
division does not raise invalid or divide-by-zero from inactive padding.
`swizzle_codegen.cc` supplies small shuffle/store probes and direct three-lane
`loadu`/`storeu` leaves for object-code
inspection under strict floating-point flags and the selected native ISA; runtime
checks do not stand in for instruction-count evidence. Use a separate
`SIMD_TEST_ISA=AVX512` configuration and build the two `simd_test_swizzle_*`
targets to exercise compact two- and three-lane predicates; an AVX2 run does not
cover that path. The executable prints its architecture and mask representation.
Inspect the short-copy leaves for masked x86 memory operations; guarded runtime
checks independently reject any access to the fourth logical word.

The half bank checks every 16-bit decode encoding and finite round trip, all
adjacent representable midpoints and their binary32 neighbors, overflow and
limits, in all four rounding modes with flush enabled and disabled. Narrowing
uses integer round-to-nearest, ties-to-even independently of those controls.
Projected native half arithmetic remains the compiler's responsibility.

Mixed-operand and lane-conversion tests check the exception specification of the
actual expression, ref-qualified conversions, discarded-result side effects and
cleanup when exceptions are enabled. Textual versus module consumers compare
current arithmetic, wide exp and integer packets. The test-only
`support/fp_environment.h` saves and restores the caller's floating-point state;
it is not a public library control API. No GPU or performance claim is implied.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
