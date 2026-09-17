# Installed omnibus consumer

This standalone project consumes an installed package. `kernel.cc` imports only
`simd`, exercises tag-based vector deduction, comparison-mask identity, FMA,
wide arithmetic and owning swizzles, and compares results with a granular-import
translation unit. A combined x86 package also checks AVX2 and AVX-512 types in
one importing translation unit. Profile selection is explicit; see
[the omnibus guide](../../docs/omnibus.md).

`archive_only.cc` has no imports. `baseline.cc` uses only granular common modules.
Both reject AVX/FMA compile flags. Each native executable has a separate baseline
main that admits CPU features and OS vector state before calling native code.
Unsupported hosts return the CTest skip code 77; skips are not execution passes.

```sh
cmake -S . -B build/producer -G Ninja -DCMAKE_CXX_COMPILER=clang++ \
  -DSIMD_BUILD_TESTS=OFF -DSIMD_ENABLE_EXCEPTIONS=ON \
  -DSIMD_ENABLE_PCH=ON -DSIMD_ENABLE_IPO=ON \
  '-DSIMD_PROFILES=AVX2;AVX512' -DCMAKE_BUILD_TYPE=Release
cmake --build build/producer --parallel 2
cmake --install build/producer --prefix '/tmp/simd original'
mv '/tmp/simd original' '/tmp/simd relocated'
cmake -S tests/omnibus -B build/consumer -G Ninja \
  -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release \
  '-DCMAKE_PREFIX_PATH=/tmp/simd relocated' \
  -DOMNIBUS_PCH=ON -DOMNIBUS_IPO=ON
cmake --build build/consumer --parallel 2
ctest --test-dir build/consumer --output-on-failure
```

Use distinct unused prefix paths, and `clang-cl` in a Windows MSVC SDK environment.
Repeat with a separate producer using `SIMD_PROFILES=AVX2` to check the narrower
package. On AArch64 use `SIMD_PROFILES=NEON`; x86 CPUID/wait are absent there.
The fixture chooses the strongest installed profile by default. The override
`OMNIBUS_CONSUMER_PROFILE` exists for explicit compiler/BMI diagnostics; choosing
a weaker profile than the package supports is not a successful-consumer mode.

The Windows Clang 23.1.1/CMake 4.4.3 qualification passed four tests for the
combined relocated package and three for AVX2-only, with exceptions enabled,
consumer PCH and native ThinLTO. The unchanged granular installed-profile test
also passed. The existing FTZ producer built against the combined relocated
package while importing its baseline scalar dependency. This is focused CPU
and package evidence; it adds no numerical approximation, GPU, performance or
ARM execution claim.

A separate Apple M3 run at source `53d9a44` passes all three NEON-only consumer
tests, plus 30 producer tests and one granular relocated consumer. It uses the
same compiler/CMake versions, exceptions, PCH and native ThinLTO; baseline
executables have IPO disabled. Source hashes and the granular numerical packet
are unchanged. See [validation](../../docs/validation.md) for the exact source
and receipt pins. Neither platform run is a GPU or throughput measurement.
