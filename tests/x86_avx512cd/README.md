# AVX512CD checks

These fixtures exercise `native.x86.avx512cd` and the `native` hub through
`simd` and `predicate` values. Compile-time banks cover conflict bits and
leading-zero counts for 32-bit and 64-bit lanes at 128, 256 and 512 bits. They
use storage-only tags, minimal instruction tags, and a broader AVX512DQ/BW/VL
tag. The large banks use a test-local Clang constant-evaluation step budget.
Hand-written anchors check bit positions across 128-bit boundaries while earlier
destination lanes are masked off. Shape assertions cover all twelve operation
names, including qword and masked leading-zero forms.

The runtime checks compare plain, merge and zero forms with an independent
scalar reference. Inputs include zero, distinct lanes, repeated values, set-bit
patterns and random values. They exhaust every logical mask over a duplicate
bank and repeat with all unused high bits set. Pattern banks also cover isolated
and complemented mask bits. Conflict comparisons include every earlier source
lane regardless of its destination mask bit.

Each native entry has an explicit target attribute. Baseline callers admit
CPU features and OS vector state before entering it. CTest reports a skip
(return code 77) when AVX512CD cannot run. AVX512VL and broader-tag forms are
separately admitted. A skip still requires the static assertions to compile,
but does not establish native instruction execution correctness.

From a configured x86 source build with `NATIVE_BUILD_TESTS=ON`:

```sh
cmake --build build --parallel
ctest --test-dir build -R '^native\.x86\.avx512cd\.' --output-on-failure
```

The checks require Python and LLVM objdump, discovered beside the selected
compiler when available. Their responsibilities are separate:

- `module.baseline` and `hub.baseline` inspect the actual runtime consumers'
  object code and reject optional instructions in the baseline probe and `main`.
- `metadata` checks normalized feature admission, raw CPUID decoding, missing
  features and observations, and required XCR0 state without executing CD.
- Four `codegen` checks require exactly one expected CD opcode per wrapper,
  the correct register width and native merge/zero mask mode, and no helper
  calls. Minimal callers permit only AVX512F mask moves; explicitly broader
  callers may use mask moves enabled by their additional features.
- Two `codegen_pairs` checks compare 37 raw/public function pairs each: all
  36 instruction forms and one baseline control, at minimal and broader caller
  targets. These checks establish assembly equality in those caller contexts,
  not performance measurements.
- Compile-failure groups require a successful positive control before checking
  all expected operation diagnostics. They cover missing F/CD/VL features,
  each missing caller target feature and a baseline caller, runtime use of
  storage-only tags, signed/floating/raw vectors, mismatched vector tags,
  scalar masks, wrong predicate lane counts, and mismatched predicate tags.
  Target-mismatch calls compile separately so Clang reaches every operation.

The broader-tag probes ensure that scalar compact-mask extraction does not
silently add AVX512DQ/BW requirements to minimal CD wrappers. CPU instructions
remain behind runtime admission even when a broader tag selects arithmetic
storage specializations.

Configuring this directory against an installed `native` package builds the
public module/hub consumers, their baseline checks, and the admission fixture.
Private-helper and compile-failure probes belong to the source build. Native
execution on a supported CPU remains necessary in addition to cross-compilation,
static assertions and assembly inspection.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
