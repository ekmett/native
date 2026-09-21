# Installed omnibus consumer

This installed-package fixture imports the hub at the configured minimum and
from separately targeted kernels. It checks vector deduction, mask identity,
FMA, wide arithmetic, owning swizzles and native type identity across source
files. The `tests/source_targets` fixture checks variants in
one translation unit without per-variant CMake settings.

`archive_only.cc` links without imports. `baseline.cc` imports the full hub and
executes scalar utilities. Both reject accidental AVX-512 flags when the project
minimum does not include them. Each native kernel's baseline main admits only
that kernel's profile. Unsupported hardware returns CTest skip code 77.

```sh
cmake -S . -B build/producer -G Ninja -DCMAKE_CXX_COMPILER=clang++ \
  -DNATIVE_BUILD_TESTS=OFF -DNATIVE_ENABLE_EXCEPTIONS=ON \
  -DNATIVE_ENABLE_IPO=ON \
  '-DNATIVE_PROFILES=AVX2;AVX512' -DCMAKE_BUILD_TYPE=Release
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
`NATIVE_PROFILES` changes regression coverage, not the installed hub's API.
On AArch64, the hub contains NEON and native half families; x86 CPUID/wait are
absent. `OMNIBUS_CONSUMER_PROFILE` is an optional whole-kernel flag override for
compiler diagnostics. Normal execution uses each kernel's own profile.

## Earlier separate-module qualification

The following records describe the previous provider layout, not validation
of the current hub change.

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
