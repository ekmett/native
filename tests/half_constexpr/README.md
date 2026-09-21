# Half-vector constant evaluation

The imported public FP16 operations are evaluated at compile time on a fixed,
reproducible corpus containing random encodings and signed zeros, subnormals,
boundaries, infinities and NaNs. The same operands run through native arithmetic,
comparisons, negation and selection. Arithmetic NaNs are compared by
classification; representation-only operations retain exact payload bits.

BF16 `dot2` compares exact compile-time and runtime result bits, including NaN
priority and denormal rules. ARM tests the legacy BFDOT mode with FPCR.EBF clear;
x86 tests all three native widths and the high-product-first VDPBF16PS order.

Runtime tests first admit the relevant CPU/OS features and then establish the
constant evaluator's FP environment. Unsupported hardware is an explicit skip.
The static test still checks the compile-time corpus on such hosts. Existing
FP16 and BF16 profile fixtures independently test other runtime FP environments,
status effects, rational reference values and generated instruction sequences.
