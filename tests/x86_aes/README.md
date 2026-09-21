# AES-NI fixture

The CMake fixture builds consumers of `native.x86.aes` and the `native` hub,
plus a baseline-only object. It can also build against an installed `native`
package. The baseline object is a compile check; no disassembly test is registered.

Both consumers compile shape and immediate-range assertions, strong- and
weak-feature constant evaluation, and the FIPS-197 AES-128 encrypt/decrypt
known-answer checks. After admitting AES at runtime, each consumer checks the
known-answer cipher and 256 deterministic input/key cases against the independent
substitution-table oracle. Those cases cover all six operations, with twelve
AESKEYGENASSIST immediates. Runtime admission failure returns CTest's skip code 77.

For an already configured x86 build:

```sh
cmake --build build --target native_test_x86_aes_module native_test_x86_aes_main native_test_x86_aes_baseline
ctest --test-dir build -R '^native\.x86\.aes\.' --output-on-failure
```

These checks do not yet include instruction disassembly, raw/public codegen
comparison, or separately compiled caller-target and weak-feature runtime
rejections. A cross-build or translated execution does not establish native
x86 instruction qualification. Native x86 execution and the omitted boundary
checks remain separate validation work.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
