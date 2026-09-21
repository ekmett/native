# WebAssembly SIMD values

`native.simd` and `native.wasm` provide typed SIMD128 values on WebAssembly
compiler targets. The first backend checkpoint supports complete 16-byte vectors
of signed and unsigned 8-, 16-, 32- and 64-bit integers, binary32 and binary64.
The architecture tag must contain `wasm_feature::simd128`. Comparison masks are
canonical zero/all-one lanes with the same width as their corresponding values.

The provider remains at the configured baseline. SIMD operations carry
`target("simd128")`; callers must compile the corresponding kernel for SIMD128.
A Wasm engine validates the complete linked module, so runtime branches cannot
hide unsupported instructions. The tests validate the final module before
instantiation, without setting engine feature flags.

This checkpoint provides loads/stores and bounded partial transfers, broadcasts,
lane access/replacement, add/subtract, native multiply (16-bit lanes and wider),
floating divide, comparisons, bitwise operations, selection, scalar-count shifts,
shuffle/swizzle, and floating square root/rounding/absolute-value operations.
Integer arithmetic wraps; shifts reduce their counts modulo the lane width.
There is no native byte multiply or vector integer division instruction.
Integer and basic floating arithmetic support constant evaluation, using the
shared IEEE binary-format implementation for floating operations.

Validation uses C++26 named modules built with WASI SDK 34 (Clang 23), CMake 4.4
and Ninja, with a module consumer executed by Node's WASI preview1 runtime.
The backend remains under expansion; this checkpoint does not claim complete
SIMD128 or relaxed-SIMD instruction coverage.
