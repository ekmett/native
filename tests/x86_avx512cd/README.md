# AVX512CD checks

These fixtures exercise `native.x86.avx512cd` and the `native` hub with the
public `simd` and `predicate` overloads. The compile-time banks cover conflict
bits and leading-zero counts for both lane types at 128, 256 and 512 bits,
using instruction-enabled tags and storage-only tags. Shape assertions reject
unsupported dword signatures, raw-register arguments and mismatched predicates.
The large constant-evaluation banks use a test-local Clang step budget.

The runtime checks compare all plain, merge and zero forms with an independent
scalar reference. Inputs include zero, distinct lanes, repeated values, set-bit
patterns and random values. Masks include empty, full, alternating, isolated
and complemented bits. Conflict comparisons include earlier lanes across
128-bit boundaries even when those lanes' output mask bits are clear.

Each runtime entry has an explicit target attribute. Baseline callers admit
CPU features and OS vector state before entering it. CTest reports a skip
(return code 77) when AVX512CD cannot run; AVX512VL forms are tested only when
separately admitted. A skip still requires the static assertions to compile,
but does not establish native execution correctness.

From a configured x86 source build with `NATIVE_BUILD_TESTS=ON`:

```sh
cmake --build build --parallel --target \
  native_test_x86_avx512cd_module native_test_x86_avx512cd_hub \
  native_test_x86_avx512cd_codegen native_test_x86_avx512cd_codegen_public
ctest --test-dir build -R '^native\.x86\.avx512cd\.' --output-on-failure
```

The `codegen_pairs` test uses the shared comparison script to compare 37 raw
and public function bodies: 36 instruction forms and one baseline control.
It requires LLVM objdump and Python. This checks that the typed wrappers add
no instructions in these caller contexts; compilation alone does not establish
assembly equality. It is not a separate expected-opcode audit or a measurement
of execution time.

This directory can also be configured against an installed `native` package;
that mode builds only the public module and hub consumers. The fixtures do not
yet contain compile-failure tests for missing features, mismatched target scopes
or runtime use of storage-only tags. Those checks and native AVX512CD execution
remain separate qualification work.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
