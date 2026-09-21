# Constant floating-point SIMD and math

The same fixture compiles through private headers and the public `native.math`
module. It checks scalar, short and full binary32 shapes, preserving their ISA
tags. Exact static assertions cover signed zeros, subnormal ties, infinities,
NaNs, fused cancellation, comparisons, conversions, masked scaling, mixed
scalar operands and bounded memory transfers.

Reproducible compile-time cases are compared with admitted native execution.
Additional full-bit finite inputs compare each arithmetic operation with a
separate scalar expression. `NATIVE_TEST_SEED` and `NATIVE_TEST_CASES` select
runtime cases; failures print the seed, index and operand/result bits. The
fixture saves and restores floating-point state and compares under nearest-even
rounding with gradual underflow. Short division explicitly checks that inactive
padding introduces neither invalid-operation nor divide-by-zero exceptions.

The existing exponential and trigonometric graphs are evaluated at compile time
and compared with runtime results, including scalar, SIMD, array and empty-array
forms. The core regression suite separately retains its historical trig oracle,
exponential checks and scaling environment tests. These approximations keep their
existing input bounds and accuracy contracts.

Assembly checks compare 24 ARM or 42 x86 public/native pairs under identical
targets and calling conventions. Short-vector references explicitly implement
zero padding and inactive denominator ones. Only alignment padding, symbol
placement and byte-verified equal constant-pool entries are normalized; loads,
calls, moves and instruction operands remain part of the comparison.
