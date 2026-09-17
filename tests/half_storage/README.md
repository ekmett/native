# Half storage consumer

This installed-package test imports `simd.numerics` and the omnibus in separate
translation units. It checks all 65,536 encodings of each half format, finite
encode/decode round trips, comparisons, exact bit transport, and every finite
adjacent-value midpoint plus its binary32 neighbors, both signs and overflow.
NaN payloads are exempt only when checking numerical decode; raw storage is exact.

The test requires and checks nearest-even gradual binary32. It does not claim
that WebAssembly exposes configurable rounding or FTZ controls. Native host tests
in `core_regression` separately retain the eight environment controls.

Native `_Float16` / `__bf16` interop is checked where the configured target supports
it. The wrappers' public `underlying_type` and `content` retain native storage on
those targets; otherwise they hold `uint16_t` bits. `from_bits` / `to_bits` are the
portable representation interface. The fallback adds no integer projection or
raw integer constructor; numerical integer construction still passes through float.

Configure with CMake 4.4 and the same target toolchain as the installed package:

```sh
cmake -S tests/half_storage -B build/half-storage -G Ninja \
  -Dsimd_DIR="/path/to/relocated/package/lib/cmake/simd"
cmake --build build/half-storage --parallel 2
ctest --test-dir build/half-storage --output-on-failure
```

For Emscripten, pass its CMake toolchain to both producer and consumer, and configure
the producer with `SIMD_PROFILES` empty and `SIMD_BUILD_TESTS=OFF`. CTest uses the
toolchain's Node emulator. Consumer PCH is enabled by default; IPO is optional.
