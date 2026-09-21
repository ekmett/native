# AVX512BITALG checks

These fixtures import `native.x86.bitalg` and the `native` hub. They cover byte
and word population counts in plain, merging and zeroing forms, and bitshuffle
in plain and masked forms, at 128, 256 and 512 bits. Public operands are `simd`
values; writemasks and bitshuffle results are `predicate` values.

Constant-evaluation banks use storage-only tags, minimal BITALG tags, and a
broader AVX512DQ/BW/VL tag. Independent scalar references count individual bits
and select within source qwords. The banks cover every byte value and control
byte, zero and full masks, alternating masks, high output bits, and single-bit
source words. Literal anchors verify qword routing, bit positions 0/63 and
ignored control bits 6/7. Tests use a local Clang constexpr step budget.

Native runtime banks additionally exhaust every byte and word value at each
vector width. They check walking and complemented output masks, distinct source
qwords, all 64 source-bit positions, and deterministic random data and masks.
Every runtime entry has a literal target attribute. Baseline callers admit CPU
features and OS vector state before entering it. CTest reports a skip (77) when
BITALG cannot execute; VL and broader tags are admitted separately. A skip
establishes compilation of the static banks, not native execution correctness.

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
