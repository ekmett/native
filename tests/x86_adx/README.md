# Addition-with-carry fixture

The source fixture imports `native.x86.adx` and the `native` hub. An independent
bitwise full-adder oracle checks both scalar widths against the implementation's
unsigned additions and overflow comparisons. Constexpr banks exercise all 256
carry-byte values with strong, weak and provider-default architecture arguments.
Runtime banks check 4,096 additions and four-limb carry chains, including
carry propagation through all-one limbs.

Metadata tests cover the independent ADX bit, properties, stable ordinals,
unobserved CPUID leaves, source `-madx` minima and admission without SIMD or
XCR0 prerequisites. The runtime entry admits ADX before calling targeted code;
it returns CTest skip code 77 if the feature is unavailable.

Both scalar widths have exact public/raw assembly comparisons for a single
addition and a four-limb chain, plus a baseline scalar pair. Opcode checks
require the scalar carry additions and exclude helper calls, branches and
SIMD operations. Clang 23 currently emits ADD/ADC for the raw intrinsics in
these callers. This is not dedicated ADCX/ADOX or dual-flag-chain coverage;
that limitation is retained explicitly in the public documentation.

Positive-controlled rejections cover missing private feature tags, missing
caller targets, weak-tag runtime calls, provider-default runtime calls at the
baseline, signed output pointers and const output pointers. Baseline `main`,
scalar import probes and the actual admission classifier are disassembled.
Checker regression controls inject ADCX and ADOX disassembly into each baseline
entry, including `main`, and require rejection while accepting ordinary ADC.

```sh
cmake --build build --parallel --target \
  native_test_x86_adx_module native_test_x86_adx_hub \
  native_test_x86_adx_metadata_0 native_test_x86_adx_metadata_1 \
  native_test_x86_adx_codegen_raw native_test_x86_adx_codegen_public
ctest --test-dir build -R '^native\.x86\.adx\.' --output-on-failure
```

The source suite has 22 CTests, retains compiler/disassembly/pair reports and
imposes no compiler job cap. A source build and codegen pass do not establish
native runtime qualification; run the executable on an ADX-capable CPU.

For installed-module consumers:

```sh
cmake -S tests/x86_adx -B build-adx-installed -G Ninja \
  -DCMAKE_PREFIX_PATH=/absolute/path/to/install
cmake --build build-adx-installed --parallel
ctest --test-dir build-adx-installed --output-on-failure
```

Use matching Clang and platform settings, with an explicit compiler target for
cross builds. Six installed tests cover metadata, direct-module and hub runtime
entries, baseline disassemblies and admission codegen. The provider default is
checked in the constexpr banks; source-only tests cover runtime rejection.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
