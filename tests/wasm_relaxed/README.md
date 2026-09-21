# Relaxed SIMD checks

This fixture builds C++26 named-module consumers of all 20 relaxed SIMD
operations. It runs in the source build or as an installed-package consumer:

```sh
cmake -S tests/wasm_relaxed -B build-consumer -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/wasi-sdk/share/cmake/wasi-sdk-p1.cmake \
  -DCMAKE_PREFIX_PATH=/path/to/native-install
cmake --build build-consumer --parallel
ctest --test-dir build-consumer --output-on-failure
```

Node must admit SIMD128 and relaxed SIMD. Validation failure is a test failure,
not a skip. The fixture runs these checks:

- `module` imports the family directly. `main` imports the main module. Both
  compile independent expected values for all 20 operations using strong and
  SIMD128-only tags. Cases include fused cancellation, floating overflow,
  invalid products, NaNs, signed zero, truncation limits, Q15 overflow and dot
  accumulation overflow.
- `main` executes every operation on edge cases and deterministic random inputs.
  Its scalar floating oracle uses libc `fma` and separately rounded arithmetic,
  independently of the library's constant evaluator. It checks all swizzle
  indices, raw/public agreement for partial masks of every width, exact canonical
  mask selection, Q15 rounding, valid truncations and zero
  upper lanes, seven-bit dot arithmetic and full-bit raw/public dot agreement.
  Invalid relaxed truncations have no value assertion under the current core
  specification; their constant saturating policy is checked separately.
- `codegen` compares 20 raw/public Wasm instruction streams exactly, requiring
  only argument loads, the assigned finalized opcode and `end`. It also retains
  the raw dot reproducer's disassembly and verifies its two intrinsic leaves.
- Each operation has separately compiled rejection cases for a caller without
  the instruction target, a SIMD128-only runtime tag, a private helper without
  the feature, and raw-register public arguments. A matching successful control
  precedes each rejection. Installed consumers omit the private-helper cases.
  Diagnostics remain in `reject-*.log`; disassembly remains in `codegen.log`.
- `engine.dot` is a standalone raw-intrinsic executable with no native library or
  module dependency. It checks the current specification's globally fixed,
  saturating interpretation on two full-bit dot inputs. It is a conformance gate,
  with no expected-failure or skip annotation.
- `engine.laneselect` independently checks raw lane-selection instructions against
  one fixed bit-selection or whole-lane interpretation shared by all four widths,
  lanes and calls. Its partial mask checks remain strict even when a wrapper
  matches the raw instruction.

When `NATIVE_WASM_WASMTIME` names a Wasmtime executable, CTest also runs the
wrapper/property executable and both raw engine gates there, then both raw gates
with Wasmtime's deterministic relaxed-SIMD option. The raw executables accept
`--deterministic` to require parameter zero: signed, saturated dot pairs and bit
selection. This argument changes the oracle; it does not configure the engine.

## Raw-engine conformance discrepancies

The pinned semantic source is
[WebAssembly/spec ba9fd9f5c23e569201265d5bda6fb8dde18ad8c0](https://github.com/WebAssembly/spec/blob/ba9fd9f5c23e569201265d5bda6fb8dde18ad8c0/document/core/exec/numerics.rst).
The [extended binary operation table](https://webassembly.github.io/spec/core/exec/numerics.html#op-vextbinop)
uses saturated adjacent pairs; the [extended ternary definition](https://webassembly.github.io/spec/core/exec/numerics.html#op-vextternop)
adds those pairs; [relaxed-operation parameters](https://webassembly.github.io/spec/core/exec/numerics.html#relaxed-ops)
remain fixed during a program. These are lines 2224, 2243–2245 and 2314 in the
pinned source.

With every lane of `a` equal to -128 and `b` equal to byte 128, signed saturation
requires `(dot, dot_add) = (32767, 65534)`. Unsigned interpretation requires
`(-32768, -65536)`. With `a = 1`, `b = 255`, signed interpretation gives `(-2,-4)`
and unsigned interpretation gives `(510,1020)`.

The raw test currently fails on ARM64 in Node 23.11.0: its first pair is
`(-32768,-65536)` and its second pair is `(-2,-4)`, so no fixed interpretation
matches both. Wasmtime 49.0.0 (`17830bd3c`), both default and deterministic mode,
produces `(-32768,65536)` for the first case and `(-2,-4)` for the second. Its
first case matches neither specified choice. This failure is reproducible
without the public wrappers. These tests retain the pinned core requirement.

The x86 Node 24 runtime also fails partial-mask lane-selection checks. For a
16-bit lane with `a = 0x1234`, `b = 0xabcd` and `mask = 0x8000`, the
[core definition](https://webassembly.github.io/spec/core/exec/numerics.html#op-irelaxed-laneselect)
permits `0x2bcd` from bit selection or `0x1234` from whole-lane selection.
Selecting each byte by its own mask sign bit would produce `0x12cd`, which
is outside that set. The independent raw-engine fixture includes this witness
and randomized cases; canonical zero/all-one masks have one exact result.

The tested toolchain is WASI SDK 34 / Clang 23.1.0. Passing wrapper, constant and
opcode checks does not establish full engine conformance or agreement across
other engines or architectures.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
