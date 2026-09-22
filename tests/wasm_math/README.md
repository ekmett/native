# SIMD128 promoted math qualification

This fixture imports `native` and `native.math` and can also build against an
installed package with `find_package(native)`. It covers binary32 `exp`,
`exp<true>`, `expm1`, `damping_gain`, `log`, `log1p`, `tanh`, `atan2`, `sin`, `cos`, and paired `sincos` on SIMD128 vectors, standard arrays
and `native::wide`, including empty packs and both public math namespaces.

The arithmetic contract is intentionally distinct from FMA-based scalar, x86
and ARM kernels: SIMD128 rounds each polynomial multiplication and addition
separately, in constant evaluation and at runtime. No relaxed multiply-add is
used, including when the operand type/caller permits relaxed SIMD. Public FMA
remains unavailable on these Wasm vectors. Coefficients, range/domain limits,
underflow cutoffs and sign/quadrant selection stay shared with the existing
kernels. Exponential uses two normal exponent factors; the last multiplication
performs gradual-underflow rounding. No general-purpose scaleb is added.

The CTests are:

- `native.wasm.math`: Node validation/execution, constexpr/runtime agreement,
  empty/batched shape checks, signed zeros, subnormal inputs/outputs, underflow
  and overflow neighbors, infinities, quiet/signaling NaNs, plus a scalar
  separate-rounding exp reference. An independent binary64 libm reference
  checks 65,583 exponential and 289,176 trigonometric inputs. Trig samples include
  neighbors of every in-domain multiple of pi/4 and seeded bit patterns; exp
  includes a seeded sample across its nontrivial range. The retained budgets
  are 2 ULP for finite exponential and 2^-22 absolute error for trig. These are
  sampled regression budgets, not exhaustive error proofs or correct-rounding
  guarantees. NaN signs/payloads are outside the runtime comparison contract.
- `native.wasm.math.base_engine`: runs the same module with Wasmtime SIMD enabled
  and relaxed SIMD explicitly disabled. This checks this module's actual
  feature requirement; it does not alter advisory engine-conformance tests.
- `native.wasm.math.codegen`: checks the single/batched entry bodies for
  vector arithmetic/conversions, no scalar floating arithmetic or lane
  extraction, no calls and no relaxed instructions. Namespace and batching pairs require
  identical Wasm instruction streams across namespace aliases, wide/array
  shapes and stronger caller attributes. The generated JSON retains complete
  bodies. This inspects Wasm bytecode, not engine JIT machine code or timing.

`native.wasm.tanh` and `native.wasm.atan2`, plus their `.base_engine` variants,
reuse the promoted-kernel regression banks. They check signed special values,
empty and wide shapes, public aliases and sampled accuracy with a 2 ULP budget.
The tanh bank retains a small MPFR-derived word fixture; the complete sampled
banks use double libm by default.

## Local reproduction

Use a C++26 WASI SDK with module scanning, CMake 4.4 and Ninja. Set `wasmtime`
and `llvm-objdump` explicitly if they are not on PATH:

```sh
cmake -S . -B build-wasm -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/wasi-sdk/share/cmake/wasi-sdk-p1.cmake \
  -DCMAKE_BUILD_TYPE=Release -DNATIVE_PROFILES=WASM_SIMD128 \
  -DNATIVE_TEST_ISA=WASM_SIMD128 -DNATIVE_ENABLE_IPO=OFF \
  -DNATIVE_ENABLE_PCH=OFF -DNATIVE_WASM_WASMTIME=/path/to/wasmtime \
  -DNATIVE_WASM_OBJDUMP=/path/to/wasi-sdk/bin/llvm-objdump
cmake --build build-wasm --parallel
ctest --test-dir build-wasm -LE engine-conformance --output-on-failure
cmake --install build-wasm --prefix "$PWD/build-wasm/stage"
cmake -S tests/wasm_math -B build-wasm-consumer -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/wasi-sdk/share/cmake/wasi-sdk-p1.cmake \
  -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$PWD/build-wasm/stage" \
  -DNATIVE_WASM_WASMTIME=/path/to/wasmtime \
  -DNATIVE_WASM_OBJDUMP=/path/to/wasi-sdk/bin/llvm-objdump
cmake --build build-wasm-consumer --parallel
ctest --test-dir build-wasm-consumer --output-on-failure
```

WASI SDK 34 / Clang 23.1.0 with Node 23.11.0 and Wasmtime 49.0.0 on macOS ARM64
passes the fixture. Both engines report maximum observed exponential error of
1 ULP and maximum observed trig absolute error of `7.82414814e-08`. These local
results do not qualify all browser engines or promise cross-ISA bitwise output.

The focused module suite passes 7/7 with these two kernels and the shared
code-generation check. Accuracy qualification and engine-conformance checks are
separate; the existing `engine-conformance` label remains advisory.
