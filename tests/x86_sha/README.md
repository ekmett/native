# SHA instruction fixture

The source-tree fixture imports `native.x86.sha` and the `native` hub from
baseline translation units. It checks all seven SHA primitives, including all
four SHA1RNDS4 selectors, using:

- Independent scalar FIPS 180-4 state transitions and schedule equations, with
  unsigned overflow, zero and all-one inputs, 64 constexpr operand banks and
  1,024 runtime banks per operation.
- Strong, SSE2-only constant, and broader AVX-512 architecture tags. Composed
  SHA-1 and SHA-256 `"abc"` digests check the complete schedule and round packing
  under both strong and weak tags, including unused SHA256RNDS2 message lanes.
- CPUID bit isolation, absent observations, missing SSE2 storage prerequisites,
  source `-msha` minima, feature property accessors and stable enum ordinals.
- Ten expected SHA opcode/selector forms and exact raw/public assembly pairs
  under both minimum and broader caller targets. Each pair bank also includes a
  baseline scalar function. Separate disassembly checks cover the importing
  baseline `main` and the real admission classifier.
- Positive-controlled rejection of missing SHA in the private feature tag,
  missing SHA in the caller target, weak-tag runtime calls, four wrong public
  shapes, incomplete storage, and selectors 4, 255, unsigned -1 and nonconstant
  selectors under strong and weak tags. Each target-mismatch operation is
  compiled separately so one error cannot hide another.

Runtime checks call a baseline admission classifier before entering the SHA
function. They return CTest skip code 77 when SHA is unavailable. A successful
compile or assembly comparison does not establish native instruction behavior.
Run the runtime entries on a SHA-capable CPU for that qualification.

With a configured source build, build these targets (without a compiler job cap):

```sh
cmake --build build --parallel --target \
  native_test_x86_sha_module native_test_x86_sha_hub \
  native_test_x86_sha_metadata_baseline native_test_x86_sha_metadata_minimum \
  native_test_x86_sha_codegen_raw native_test_x86_sha_codegen_public \
  native_test_x86_sha_codegen_raw_broad native_test_x86_sha_codegen_public_broad
ctest --test-dir build -R '^native\.x86\.sha\.' --output-on-failure
```

The negative tests build their matching positive controls first. They retain
compiler logs; assembly checks retain disassemblies and pair reports in the
fixture build directory. The source suite has 26 CTests.

An installed-consumer build uses C++26 modules and regenerated BMIs:

```sh
cmake -S tests/x86_sha -B build-sha-installed -G Ninja \
  -DCMAKE_PREFIX_PATH=/absolute/path/to/install
cmake --build build-sha-installed --parallel
ctest --test-dir build-sha-installed --output-on-failure
```

Supply the same Clang toolchain and platform settings as the package build.
The installed fixture runs six tests: metadata, direct-module and hub runtime
entries, their baseline disassemblies, and the admission disassembly. Private
raw-helper boundaries and source-tree codegen pairs are source-only tests.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
