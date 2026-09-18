# Building and consuming simd

Use Clang 23, CMake 4.4 and Ninja. On Windows use `clang-cl` with a configured
MSVC SDK environment; on macOS select an LLVM toolchain explicitly instead of
the system compiler. Configuration compiles the required language features.

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
target_link_libraries(example PRIVATE simd::simd)
simd_target_profile(example AVX512)  # default x86 package
```

```cpp
import simd;
using V = simd::vec<float, 8, simd::avx2>;
using M = V::mask;
```

`simd::simd` supplies compiled objects and the omnibus module metadata.
`simd::common` and individual profile targets remain available for granular
imports. CMake rebuilds compatible BMIs from installed source.
Installed PCMs would bind the package to one compiler configuration, so the
package distributes module source instead. A baseline dispatcher links
the archive and common modules without ISA compile flags, then invokes separately
compiled native entries after CPUID/OS vector-state checks.

For the default x86 package, the omnibus imports both native profiles, so its
consumer explicitly selects `AVX512`. An AVX2-only omnibus requires a package
built with `-DSIMD_PROFILES=AVX2`; its consumer selects `AVX2`. AArch64 packages
default to `NEON`, and their consumers select `NEON`. A combined x86 package also
supports a narrower translation unit using `import simd.avx2;` with the AVX2
provider and flags. See [the omnibus guide](../docs/omnibus.md) for the observed
CMake 4.4.3 dependency-BMI restriction and installed-consumer checks.

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

## Compiler caching

The producer CI jobs use sccache 0.16.0 through Mozilla's commit-pinned
[sccache action](https://github.com/Mozilla-Actions/sccache-action/tree/fc920bf0ec8de6ee65d409111f7ec508035751ba)
and GitHub Actions cache backend. The action verifies the release archive's
published SHA-256 and supplies the cache service environment. The workflow keeps
`contents: read` permissions and needs no additional repository secrets or
separate `actions/cache` step.

For an opt-in local disk cache, install sccache separately, put it on `PATH`, and
add `-DCMAKE_CXX_COMPILER_LAUNCHER=sccache` to the producer configure command.
Keep PCH and IPO enabled. This launcher is a build-tree setting; installed
packages neither require sccache nor select a consumer's launcher. Configure
without that argument for an uncached new tree, or set
`-DCMAKE_CXX_COMPILER_LAUNCHER=` to clear an existing tree's launcher.

Cache coverage is partial. The pinned release understands explicit Clang named
module flags, but its
[response-file reader](https://github.com/mozilla/sccache/blob/v0.16.0/src/compiler/gcc.rs)
bypasses files containing quotes. CMake's generated `.modmap` files quote module
paths, so those compilations report the non-cacheable reason `@` and run the
compiler normally. Some CMake-synthesized BMI commands do not use the launcher
at all. Clang PCH generation can be cached; clang-cl's `/Yc` and `/Fp` PCH flags
and forwarded module flags are
[unsupported](https://github.com/mozilla/sccache/blob/v0.16.0/src/compiler/msvc.rs)
and bypass caching. Do not remove PCH, change module generation or weaken compiler
settings to inflate cache hits. Dependency scanning and linking still execute.

Every CI job records `sccache --show-adv-stats`, JSON statistics and the cache
version in its logs artifact, including after a failed build when setup succeeded.
The job summary includes non-cacheable reasons as well as hits, misses and cache
errors. Inspect those counters before attributing a speedup to the cache; a
successful build alone does not demonstrate reuse across workflow runs.

To check local reuse, build and run CTest, record the statistics, run
`cmake --build build/core --target clean`, then `sccache --zero-stats`, rebuild
and run CTest again. An incremental build with no work does not exercise the
cache. Use the same source/build paths and compiler; changed paths, compiler
contents or flags can prevent hits. See the measured native result in
[validation](../docs/validation.md#compiler-cache).

## Toolchain recipes and CI

Keep `clang++` and `clang-scan-deps` from the same LLVM 23 installation on
`PATH`. Select `CMAKE_CXX_COMPILER` explicitly when several Clang installations
are available. Keep the compiler resource directory, standard-library headers
and linker consistent with that installation. `CMAKE_PREFIX_PATH` points to
installed library packages, not to a producer's build directory.

The CI workflow configures Ninja directly, builds with PCH and IPO, runs CTest,
and checks installation. It selects AVX2 tests on Linux and Windows x86-64 runners and NEON
on Linux, macOS and Windows ARM64 runners; the x86 archives also build the AVX-512 module. The
baseline profile tests check CPU and OS support before entering AVX-512 code.
The workflow is a reproducible build recipe; platform execution claims are
listed separately in [validation](../docs/validation.md).

Each job retains its actual CPU features, OS and toolchain identity, test list,
JUnit results and configure logs. Standard hosted runner labels select an OS
and architecture, not a fixed CPU model. AVX-512 compilation and unsupported
profile skips are not AVX-512 execution coverage. These jobs qualify native CPU
packages; they do not establish GPU behavior.

Intel macOS is pending a qualified Clang 23 toolchain artifact: the hosted
image compiler is too old, and the current LLVM/Homebrew releases do not
provide an Intel macOS binary for the required compiler. It is not an
execution lane in this workflow.

`make`, `make test` and `make install` wrap the `clang-release` preset. Override
`PRESET=clang-cl-release` when using clang-cl, or pass explicit configure options
through `CMAKE_ARGS`. The preset takes the test ISA from the host default.
For example, `make test CMAKE_ARGS=-DSIMD_TEST_ISA=AVX512` requires a matching CPU
and OS vector state. Runtime tests must not be used as feature probes.

`Dockerfile` is an optional Ubuntu 24.04 / LLVM 23 build environment. It installs
build tools but does not build the library or establish a Linux qualification.
Build a local image explicitly with `docker build -t simd-build .`. CI does not
depend on a prepublished container.
## API documentation

Doxygen 1.18 generates the guides, individual API contracts and compiled
examples. A documentation-only build does not require the host library:

```sh
cmake -S . -B build/docs -G Ninja -DSIMD_BUILD_HOST=OFF -DSIMD_BUILD_DOCS=ON
cmake --build build/docs --target simd_docs
```

Open `build/docs/docs/html/index.html`. Warnings fail the build. With Python
available, `python tests/api/audit_docs.py build/docs/docs/xml` checks indexed
public callable documentation; it rejects missing descriptions and empty input.
The [example project](../tests/api/README.md) compiles the snippets against an
installed package. Generation alone does not compile examples or qualify an ISA.

The Documentation workflow builds this reference for pull requests and `main`
without enabling a C++ compiler. It checks callable descriptions, module
navigation, and local page and fragment links, and retains HTML and diagnostics
as an Actions artifact. Successful `main` builds publish the same HTML to
[GitHub Pages](https://ekmett.github.io/simd/). Pull requests do not deploy.

Run the generated-site checks locally with:

```sh
python doc/test_module_anchors.py
python doc/test_links.py
python doc/check_links.py build/docs/docs/html
```
