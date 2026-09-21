# AVX512VBMI checks

These fixtures import `native.x86.vbmi` and the `native` hub. They cover VPERMB,
VPERMT2B, VPERMI2B and VPMULTISHIFTQB at 128, 256 and 512 bits, in ordinary,
merging and zeroing forms. The public operands/results use `simd`; writemasks
use `predicate`.

Constant banks use storage-only, minimal VBMI and broader DQ/BW/VL tags. An
independent reference selects table elements by modulo indexing and constructs
multishift bytes one bit at a time. All 256 index/control values are covered,
including ignored high bits. Literal anchors verify full-vector selection
across 128/256-bit boundaries, both two-table merge destinations, and qword
bit-window wrapping from 63 to 0. Separate banks bound the Clang constexpr
step budget while retaining the complete control-byte domain.

Native runtime tests compare all forms against the scalar reference using
patterned and random tables, controls, source qwords and masks. They exhaust
16-bit logical masks, repeat with unused high bits set, and cover walking and
complemented mask bits at every width. Each native entry has a literal target
attribute; baseline callers first admit CPU and OS vector state. CTest skips
native execution with return code 77 when VBMI is unavailable; VL and broader
tags are admitted separately. A skip is not native correctness qualification.

From an x86 source build with `NATIVE_BUILD_TESTS=ON`:

```sh
cmake --build build --parallel
ctest --test-dir build -R '^native\.x86\.vbmi\.' --output-on-failure
```

The suite requires Python and LLVM objdump, discovered beside the compiler.
Metadata tests check independent CPUID decoding, properties, target closure
and OS admission. Compile-only objects check enabled and explicitly disabled
compiler minima. Baseline probes inspect the actual module/hub consumers.
Two assembly comparisons require identical raw/public code for all 36 forms
and a baseline control, under minimal and broader targets. Four opcode audits
verify expected operations, register widths, mask modes and absence of helper
calls. Plain/zero two-table forms permit either tied-destination encoding;
merging forms must use the encoding that retains the designated operand.
Probe parameters keep the index between the two tables, preventing Clang from
combining adjacent stack tables into one wider single-table permutation.

Negative compile groups require positive controls and every expected diagnostic
for missing F/BW/VBMI/VL features, mismatched caller permissions, weak-tag
runtime calls, unsupported vector shapes/tags, and invalid mask types/shapes.
Target failures compile one operation per translation unit so Clang diagnoses
every operation. Installed consumers run the public module/hub, baseline and
metadata checks; private helpers and rejection probes belong to source builds.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
