# Native ISA module regression

Each AVX2 and AVX512 bridge compiles with private profile flags and imports the
shared hub. The baseline x86-64 dispatcher checks
CPUID and OS vector state, then enters an admitted bridge through a C function.
The `none` control enters neither profile. Type checks verify distinct profile
types and architecture values with shared `native::wide` storage. Generic consumers
use `template<native::isa Arch>` and `native::simd<T,N,Arch>`; constructor calls name
the vector type explicitly. Copy deduction preserves its existing ISA value. Comparisons return `V::mask`, full vectors for AVX2 and compact
predicates for the supported AVX512 shapes.

Each current capture contains eight operation-major columns of 96 words: add,
multiply, FMA, sign selection, wide exp, integer arithmetic, masked scaling and
square root. Scalar references check the elementary operations; wide exp is
compared with the direct raw kernel. Header/import captures and admitted profiles
must agree exactly on this finite bank. Protected-page tails, fill lanes, empty null-pointer access
and caller FP-state restoration are also checked.

Configure the root with `-DNATIVE_PROFILES="AVX2;AVX512" -DNATIVE_BUILD_TESTS=ON`, build
`native_test_profiles`, then run `ctest --test-dir <build> -R native.profiles
--output-on-failure`. CPU/OS-incompatible profiles skip with status 77. The
capture executable optionally accepts an independently supplied current-format
packet for cross-host comparison. These are bounded CPU regressions, not a
complete-domain accuracy proof or GPU validation.

The separate NEON fixture follows the same packet contract; see
[its runner](../neon/README.md). Both bridges use the same baseline hub BMI.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
