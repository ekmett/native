# AVX512VBMI2 checks

These fixtures import `native.x86.vbmi2` and the `native` hub. They cover
byte/word register compression and expansion, typed memory compression and
expansion, and word/dword/qword immediate and variable double-source shifts.
All 150 applicable forms span 128, 256 and 512 bits. Public vector operands
and results use `simd`; masks use `predicate`.

Constant banks use storage-only, minimal VBMI2 and broader DQ/BW/VL tags.
The independent compaction reference derives each lane's rank from preceding
mask bits. The shift reference transfers one bit at a time between scalar
lanes. Banks cover all 256 low count-byte values, ignored count bits,
alternating/complemented masks, zero/full masks, and immediate boundaries through
255. Literal anchors verify compaction across 128/256-bit boundaries and the
compressed merge suffix. Exact-size constexpr allocations cover every packed
count, mutable/const load pointers and zero-mask null pointers.

Native tests use patterned and random inputs, all 128-bit logical masks
(including unused high bits), and walking/complemented masks at every width.
Memory probes reuse the shared guarded-page fixture and place every packed
extent immediately before an inaccessible page. They verify canaries, exact
contents, both load mask modes, empty inaccessible ranges and typed null
pointers. Targeted entry functions have literal target attributes; baseline
callers admit CPU features and OS ZMM state before entry. Unsupported native
execution returns 77; VL and broader tags are admitted separately. A skip is
not native correctness or memory qualification.

From an x86 source build with `NATIVE_BUILD_TESTS=ON`:

```sh
cmake --build build --parallel
ctest --test-dir build -R '^native\.x86\.vbmi2\.' --output-on-failure
```

The suite requires Python and LLVM objdump, discovered beside the compiler.
Metadata checks cover CPUID decoding, target closure, properties and OS
admission. Compile-only fixtures check enabled and disabled compiler minima.
Baseline audits inspect the actual module/hub consumers. Two assembly
comparisons require identical raw/public code for all 150 forms and a baseline
control under minimal and broader targets. Four opcode audits check operation,
register width, writemask mode, direct memory transfers and absence of helper
calls. Constant shifts may use the opposite direction with exchanged inputs
and the complementary lane-width count; audits verify that immediate too.

Negative groups require successful positive controls and every expected
rejection for missing F/BW/VBMI2/VL, mismatched callers, weak-tag runtime calls,
invalid vector/mask types or tags, incorrect pointer types and out-of-range
immediates. Caller-target mismatches compile one operation per translation
unit. Installed consumers exercise public module/hub, baseline, metadata and
semantic fixtures; source builds own private helpers and rejection probes.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
