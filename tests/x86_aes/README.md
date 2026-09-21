# AES-NI fixture

The CMake fixture builds consumers of `native.x86.aes` and the `native` hub.
Both compile shape and immediate-range assertions, strong- and weak-feature
constant evaluation, and FIPS-197 AES-128 encrypt/decrypt known-answer checks.
After admitting AES at runtime, each consumer checks the known-answer cipher
and 256 deterministic input/key cases against an independent substitution-table
oracle. These cases cover all six operations, every substitution input byte,
and twelve AESKEYGENASSIST immediates. Runtime admission failure returns CTest's
skip code 77.

Both consumers also exercise a separately compiled baseline classifier with
synthetic CPUID records before checking the host. The records test AES at
leaf 1 ECX bit 25, unrelated bits, missing MMX/SSE/SSE2 prerequisites, and an
unobserved leaf. Legacy AES admission succeeds without AVX or observed XCR0.
Disassembly checks the whole baseline object, including emitted classifier
helpers, for AES, AVX and AVX-512 instructions.

Eight raw/public function pairs cover AESENC, AESENCLAST, AESDEC, AESDECLAST,
AESIMC, and AESKEYGENASSIST with immediates 0, 1 and 255. The paired check requires
identical instruction streams in the same caller context. Separate opcode checks
require exactly one expected legacy 128-bit AES instruction, the exact keygen
immediate, and no calls, branches or additional AVX/AVX-512 requirements.

Thirty-one rejection checks each build a matching positive control first:

- Every operation rejects a caller without the AES target, a private raw-helper
  tag without AES, runtime inputs under a weak public tag, and public raw-register
  arguments. Each operation is compiled separately.
- AESENC rejects floating-point elements, wrong integer elements and mixed tags.
- AESKEYGENASSIST rejects an out-of-range byte, a negative immediate, an unsigned
  overflow and a runtime immediate.

For an already configured source-tree x86 build:

```sh
cmake --build build --target native_test_x86_aes_module native_test_x86_aes_main native_test_x86_aes_baseline native_test_x86_aes_codegen native_test_x86_aes_codegen_public
ctest --test-dir build -R '^native\.x86\.aes\.' --output-on-failure
```

The 36 CTests retain disassembly, paired comparisons and compiler diagnostics in
the fixture's build directory. Installed-package consumers run the two public
runtime tests and the baseline disassembly check; private-helper and compile
rejection fixtures run in the source tree. Compilation and disassembly establish
compiler behavior. A cross-build or translated execution does not establish
native x86 instruction execution or performance.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
