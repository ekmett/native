# Building and consuming simd

Use Clang 23, CMake 4.4 and Ninja. On Windows use `clang-cl` with a configured
MSVC SDK environment; on ARM use a Clang installation with the required C++26
pack support. The configure check tests the language feature itself.

```sh
cmake -S . -B build/core -G Ninja -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_BUILD_TYPE=Release -DSIMD_ENABLE_PCH=ON -DSIMD_ENABLE_IPO=ON
cmake --build build/core --parallel 2
ctest --test-dir build/core --output-on-failure
cmake --install build/core --prefix /path/to/simd
```

`SIMD_PROFILES` names the ISA modules to build. The default is `AVX2;AVX512` on
x86 and `NEON` on arm64. `SIMD_TEST_ISA` chooses the test implementation; it does
not choose a runtime backend. Keep ISA flags off global CMake variables.
`SIMD_ENABLE_EXCEPTIONS` defaults to OFF; consumer and producer runtime/STL modes
must agree. `SIMD_ENABLE_ASAN` supports focused host-memory checks.

Named swizzles require Clang's property extension. Linking the exported header
or module targets supplies `-fms-extensions` for `clang++`; `clang-cl` already
accepts the syntax. The configure-time check exercises a property implemented
with an explicit object parameter, rather than accepting a compiler version
number alone. Direct header consumers must provide the same language option.

## Installed C++ modules

```cmake
cmake_minimum_required(VERSION 4.4)
project(example LANGUAGES CXX)
find_package(simd CONFIG REQUIRED COMPONENTS simd)
add_executable(example example.cc)
target_link_libraries(example PRIVATE simd::simd simd::common simd::avx2)
simd_target_profile(example AVX2)
```

```cpp
import simd.avx2;
using V = simd::vec<float, 8, simd::avx2>;
using M = V::mask;
```

`simd::simd` supplies compiled objects. `simd::common` and the selected profile
supply module metadata; CMake rebuilds compatible BMIs from installed source.
No precompiled PCM is a portable package interface. A baseline dispatcher links
the archive and common modules without ISA compile flags, then invokes separately
compiled native entries after CPUID/OS vector-state checks.

## Headers and downstream libraries

An import does not export macros. For the named compiler modifiers, include
`<simd/attributes.h>` and consume `simd::headers`. The same target carries textual
implementation inputs needed to rebuild modules; they are not a second public
SIMD header API.

```sh
cmake -S . -B build/headers -G Ninja -DSIMD_BUILD_HOST=OFF
cmake --install build/headers --prefix /path/to/simd-headers
```

A shader-only or tooling consumer can use `project(... LANGUAGES NONE)` and
`find_package(simd CONFIG REQUIRED COMPONENTS headers)` without enabling a C++
compiler. FTZ's shader wrapper and mathematical contract belong to its own
`ftz::hlsl` target, which depends on these headers.

## PCH and LTO

PCHs belong to individual producers. Namespace, ISA, compiler/STL, exception
settings and macros must match the consuming translation unit. The library
build never exports its PCH to an application.

A consumer may build its own PCH with the standard headers and
`<simd/attributes.h>`. Use ordinary native objects at the baseline dispatch
boundary when other code uses ThinLTO; disabling a target IPO property cannot
undo manually inherited LTO flags. See [the module guide](../docs/modules.md)
for intrinsic linkage and transitive BMI details.

## Toolchain recipes and CI

Keep `clang++` and `clang-scan-deps` from the same LLVM 23 installation on
`PATH`. [LLVM's Ubuntu packages](https://apt.llvm.org/) provide the versioned
compiler and `clang-tools-23`; [Homebrew LLVM](https://formulae.brew.sh/formula/llvm)
provides `llvm@23` on macOS. Use `brew --prefix llvm@23` instead of assuming an
Intel or Apple Silicon installation path. The
[CMake Python distributions](https://pypi.org/project/cmake/) provide CMake 4.4
in an isolated virtual environment when the system package is older.

The CI workflow configures Ninja directly, builds with PCH and IPO, runs CTest,
and checks installation. It selects AVX2 tests on the Linux x86 runner and NEON
on the macOS ARM runner; the x86 archive also builds the AVX-512 module. The
baseline profile tests check CPU and OS support before entering AVX-512 code.
Build or test failures fail the job. These recipes describe the intended CI
configuration; a changed toolchain still needs its own successful run.

`make`, `make test` and `make install` wrap the `clang-release` preset. Override
`PRESET=clang-cl-release` when using clang-cl, or pass explicit configure options
through `CMAKE_ARGS`. The preset takes the test ISA from the host default.
For example, `make test CMAKE_ARGS=-DSIMD_TEST_ISA=AVX512` requires a matching CPU
and OS vector state. Runtime tests must not be used as feature probes.

`Dockerfile` is an optional Ubuntu 24.04 / LLVM 23 build environment. It installs
build tools but does not build the library or establish a Linux qualification.
Build a local image explicitly with `docker build -t simd-build .`; the old
automatic registry-upload and `act` helper scripts are retired. CI does not
depend on a prepublished container.
Documentation is separate: enable `SIMD_BUILD_DOCS=ON` and build `simd_docs`
with Doxygen 1.12 or newer.
