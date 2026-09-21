# IFMA instruction fixture

The source fixture imports `native.x86.ifma` and the `native` hub from baseline
translation units. It checks 22 native operation forms: two product halves at
128/256-bit VEX widths, and both halves with plain, merge and zero masks at
128/256/512-bit EVEX widths.

An independent shift/add oracle constructs the unsigned 104-bit product in
two 64-bit words. The implementation instead multiplies 26-bit limbs. Thirty-two
constexpr banks cover strong, weak, broader AVX-512 and combined AVX-IFMA/EVEX
tags, ignored upper multiplicand bits, full-width accumulator wrap and inactive
lanes. Runtime banks compare 1,024 input sets for each encoding/width, including
all 256 byte-mask values, zero inputs and all-one inputs.

Metadata tests check hardware versus compiler prerequisite closure, independent
feature bits, unobserved CPUID subleaf 1, all 64 XCR0 bit positions, properties,
ordinals and enabled compiler minima. Runtime admission is performed separately
for each encoding/width; unavailable forms print a skip, and the executable
returns CTest skip code 77 when none can run.

The source suite also checks:

- Expected VEX/EVEX encodings, low/high opcodes, register widths and hardware
  merge/zero masking; exact raw/public instruction streams under minimum and
  broader targets, including a baseline scalar pair in each bank.
- Baseline importing `main`, scalar probe and actual admission classifier
  disassemblies, without optional instructions.
- Positive-controlled feature, caller-target, weak-runtime, vector-shape,
  mask-shape and storage rejections. Each caller-target operation is compiled
  separately so earlier diagnostics cannot hide a later operation.

Build the source qualification targets without a compiler job cap:

```sh
cmake --build build --parallel --target \
  native_test_x86_ifma_module native_test_x86_ifma_hub \
  native_test_x86_ifma_metadata_0 native_test_x86_ifma_metadata_1 \
  native_test_x86_ifma_metadata_2 native_test_x86_ifma_metadata_3 \
  native_test_x86_ifma_codegen_raw native_test_x86_ifma_codegen_public \
  native_test_x86_ifma_codegen_raw_broad native_test_x86_ifma_codegen_public_broad
ctest --test-dir build -R '^native\.x86\.ifma\.' --output-on-failure
```

The source suite has 71 CTests. Negative tests retain compiler logs; codegen
tests retain disassemblies and pair reports. A compile or assembly pass does
not establish native instruction behavior: AVX-IFMA and AVX512IFMA execution
on supporting CPUs remains necessary for runtime qualification.

An installed consumer uses regenerated C++26 module BMIs:

```sh
cmake -S tests/x86_ifma -B build-ifma-installed -G Ninja \
  -DCMAKE_PREFIX_PATH=/absolute/path/to/install
cmake --build build-ifma-installed --parallel
ctest --test-dir build-ifma-installed --output-on-failure
```

Use the package's Clang toolchain and matching platform settings. Cross builds
must set the compiler target explicitly so imported synthetic BMI targets also
use the intended architecture. The installed suite has six tests: metadata,
direct and hub runtime checks, their baseline disassemblies and admission
codegen. Private boundaries and raw/public pairs belong to the source suite.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
