<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
# Integer instruction constant evaluation

This fixture imports the public PCLMUL, VPCLMUL, GFNI, VPOPCNTDQ and VNNI
modules. Each supported width and masked form is evaluated with both its
instruction tag and a weaker tag that supplies register storage. Assertions
compare those results with independent scalar oracles. They cover all 65,536
byte-field products, every 64-by-64 polynomial basis pair, all 256 immediate
encodings, matrix bit order, field inverses, population counts, lane masks,
signedness, wrapping and saturation after the complete dot-product sum.

Seeded cases use the shared [property harness](../property_check.h). A fixed
corpus is evaluated through the public API during compilation, then compared
with native instruction results after CPU and OS-state admission. Additional
runtime cases compare native results with independent scalar oracles. Set
`NATIVE_TEST_SEED` and `NATIVE_TEST_CASES` to reproduce or extend those cases;
failures print the seed, case index and operand bits. Missing runtime features
are reported as skips; compile-time assertions still run on every x86 build.

`reject_runtime.cc` calls all 109 weak overloads with runtime operands. Its
checker requires a constant-expression diagnostic at each marked source line,
so an earlier unrelated compile failure cannot count as coverage. Ten additional
calls verify that weak tags still reject out-of-range immediates. Existing
family fixtures continue to reject raw registers, mixed tags, wrong shapes,
out-of-range immediates and insufficient caller targets. Their paired assembly
checks compare the public wrapper with the corresponding private intrinsic
helper in the same calling context.

Configure this directory by itself with `native_DIR` to exercise a physically
relocated installed package. Its sibling fixture headers and shared property
harness remain test sources; all library declarations come from the package.
