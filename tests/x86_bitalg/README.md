# AVX512BITALG checks

These fixtures import `native.x86.bitalg` and the `native` hub. They cover byte
and word population counts in plain, merging and zeroing forms, and bitshuffle
in plain and masked forms, at 128, 256 and 512 bits. Public operands are `simd`
values; writemasks and bitshuffle results are `predicate` values.

The family module checks constant evaluation with storage-only tags, minimal
BITALG tags, and a broader AVX512DQ/BW/VL tag. Independent scalar references
count individual bits and select within source qwords. Population-count inputs
are zero, all ones, and mixed lanes containing low/high bits, byte boundaries
and alternating bits. Bitshuffle inputs add distinct qwords and selectors at
0/63, byte boundaries and ignored high control bits. Masks cover empty/full,
alternating lanes, first/last lanes and both sides of qword boundaries across
all three vector widths. Literal anchors independently check qword routing and
ignored selector bits. These checks fit Clang's default constexpr step budget.

Native and polyfill runtime checks use the same boundary cases. The hub has a
small constexpr/runtime export check for both population-count types and
bitshuffle; it does not repeat the family module's full fixture. Native runtime
entries have literal target attributes. Baseline callers admit CPU features and
OS vector state before entering them. CTest reports a skip (77) when BITALG
cannot execute; VL and broader tags are admitted separately. A skip establishes
compilation of the static checks, not native execution correctness.

From a configured x86 source build with `NATIVE_BUILD_TESTS=ON`:

```sh
cmake --build build --parallel
ctest --test-dir build -R '^native\.x86\.bitalg\.' --output-on-failure
```

Python and LLVM objdump are required; objdump is discovered beside the selected
compiler when available. The checks have separate responsibilities:

- `metadata` checks the enum, property, target catalog, independent CPUID bit,
  missing observations and features, and XCR0 admission. Compile-only objects
  check BITALG-enabled and explicitly disabled compiler minima.
- `module.baseline` and `hub.baseline` inspect the runtime consumers' baseline
  probe and `main`, rejecting optional scalar or vector instructions.
- Four `codegen` checks require exactly one expected BITALG opcode per wrapper,
  the proper register width and output masking, and no helper calls. Population
  counts require native merge/zero writemasks. Bitshuffle accepts either a
  native writemask or Clang's scalar-result lowering with an explicit AND.
- Two `codegen_pairs` checks compare 25 raw/public function pairs each: all
  24 instruction forms and a baseline control, under minimal and broader targets.
  Assembly equality is checked in those caller contexts; it is not a benchmark.
- Compile-failure groups require a successful positive control and every
  expected operation-specific diagnostic. They cover missing F/BW/BITALG/VL,
  missing caller target features, baseline callers, weak-tag runtime calls,
  signed and raw registers, wrong element shapes, mismatched vector tags,
  scalar masks, wrong predicate lane counts, and mismatched predicate tags.
  Target-mismatch calls compile separately so Clang reaches every operation.

The broader-tag probes exercise compact predicate construction and extraction
without adding DQ requirements to minimal BITALG wrappers. Source builds inspect
private register helpers solely as implementation controls.

Against an installed `native` package, this directory builds the module/hub
consumers, their baseline checks, and metadata/minimum fixtures. Private-helper
and compile-failure probes remain source-build checks. Native execution on a
supported CPU is required in addition to cross-compilation and disassembly.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
