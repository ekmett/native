# Integer floating-point semantics

This fixture checks the internal bit-encoding arithmetic used during constant
evaluation. It covers binary16 and bfloat16 conversions, binary32 and binary64
addition, subtraction, multiplication, division, fused multiply-add, square root,
rounding to an integer, and ordered comparisons, including signed zero,
subnormals, exact cancellation, overflow, directed rounding, and explicit NaN
selection policies. No helper operation reads or writes the floating-point
control or status registers.

Compile-time generated cases are compared with native arithmetic at runtime.
Additional reproducible generated inputs compare all four rounding modes,
including binary64-to-binary32 conversion. The half conversion checks use the
separate F16C integer oracle and exhaust every binary16 input encoding.
`NATIVE_TEST_SEED` and `NATIVE_TEST_CASES` select additional runtime cases;
failures report the seed, case index, and input/result encodings.

The test saves and restores the caller's floating-point environment. Native
comparisons run with gradual underflow and exceptions masked. NaN payload
precedence and instruction-specific policies are checked by exact assertions,
because C++ floating-point expressions do not specify a portable NaN choice.
