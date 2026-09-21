# Vector AES fixture

The fixture imports `native.x86.vaes` and the `native` hub from baseline
translation units. It checks all four AES rounds at 128, 256 and 512 bits:

- An independent FIPS 197 S-box table, inverse table and MixColumns oracle,
  anchored to the published first AES encryption round. The production
  constant evaluator uses finite-field inverses instead of that table.
- Sixteen constexpr banks with distinct state/key bytes in each 128-bit block,
  spanning all 256 byte values, under strong, weak and broader AVX-512 tags.
  Runtime banks compare each byte of every operation over 1,024 inputs per
  width, including zero and all-one vectors.
- Hardware VAES closure versus Clang's AES/AVX2 target bundle, independent
  feature bits, enum ordinals, property accessors, source `-mvaes` minima,
  absent CPUID observations and all 64 XCR0 bit positions.
- Twelve expected VAES opcode/width forms, with exact raw/public assembly pairs
  under minimum and broader caller targets. Separate baseline disassemblies
  cover imports, `main`, and the admission classifier.
- Positive-controlled rejections for each missing instruction/storage feature
  in private tags and compiler targets, weak-tag runtime calls, raw registers,
  signed bytes, mismatched tags, wrong element types and unavailable storage.
  Each target-mismatch operation reaches code generation separately.

Each runtime width independently checks CPU and OS admission before entry.
Unavailable widths print an explicit skip; the executable returns CTest skip
code 77 if none can run. A successful compile or assembly comparison does not
establish native instruction behavior. Complete runtime qualification needs
AES+AVX, 256-bit VAES, and 512-bit VAES execution on supporting CPUs.

With a configured source build:

```sh
cmake --build build --parallel --target \
  native_test_x86_vaes_module native_test_x86_vaes_hub \
  native_test_x86_vaes_metadata_baseline native_test_x86_vaes_metadata_minimum \
  native_test_x86_vaes_codegen_raw native_test_x86_vaes_codegen_public \
  native_test_x86_vaes_codegen_raw_broad native_test_x86_vaes_codegen_public_broad
ctest --test-dir build -R '^native\.x86\.vaes\.' --output-on-failure
```

The negative tests first compile matching positive controls and retain their
compiler logs. Assembly tests retain disassemblies and pair reports. The
source fixture has 40 CTests and does not impose a compiler job cap.

An installed-consumer build uses C++26 modules and regenerated BMIs:

```sh
cmake -S tests/x86_vaes -B build-vaes-installed -G Ninja \
  -DCMAKE_PREFIX_PATH=/absolute/path/to/install
cmake --build build-vaes-installed --parallel
ctest --test-dir build-vaes-installed --output-on-failure
```

Supply the same Clang toolchain and platform settings as the package build.
The installed fixture runs six tests: metadata, direct-module and hub runtime
entries, their baseline disassemblies, and the admission disassembly. Private
raw-helper boundaries and source-tree codegen pairs are source-only tests.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
