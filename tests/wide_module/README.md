# Generic wide module tests

`native.wide` owns the container, deduction guides, generic operators, math and
standard tuple protocol specializations. The common module has no SIMD, FTZ or
ISA type knowledge. Arithmetic and numerical functions prefer element-namespace ADL array hooks
and otherwise use the per-element fallback. Classification (`isfinite`, `isinf`,
`isnan`, `signbit`) maps each element to its actual bool or mask result type.
Homogeneous `copysign` preserves the element type. These value utilities directly
construct their output pack; they require neither default construction nor
assignment, and an empty pack invokes no element operation.

Import tests cover empty/one/three-register shapes, CTAD, array construction,
broadcast aliasing, const-copy and move-only conversion, binding packs, tuple
visibility and user-defined ADL. A peer imports without standard includes.
`batch.cc` checks one whole-array customization per operation, empty/single/odd
and twelve-element shapes, noncopyable broadcast operands, and the actual
lvalue assignment specification of paired results. `value.cc` covers scalar bool
and custom mask results, discarded-result side effects, rejected unsupported
operations, empty immobile results, and conditional `noexcept` for both ADL and
result construction. `value_no_headers.cc` imports only `native.wide`.

Configure with `-DNATIVE_WIDE_EXCEPTIONS=ON` to execute throwing ADL, copy and move
controls, including caller cleanup and destruction after a partially built result.
The default configuration also compiles their declaration checks with exceptions
disabled. The ordinary core suite includes these value tests.

```sh
cmake -S tests/wide_module -B build/wide -G Ninja -DCMAKE_CXX_COMPILER=clang-cl -DCMAKE_BUILD_TYPE=Release
cmake --build build/wide --parallel 2
ctest --test-dir build/wide --output-on-failure
```

Use a C++26 compiler with binding packs and the host resource gate. These
standalone tests cover the generic module; numerical profile integration has
separate tests. No timing or GPU work is performed.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
