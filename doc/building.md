# Building and installation

Use Clang 23, CMake 4.4 and Ninja. On Windows use `clang-cl` with a configured
MSVC SDK environment; on macOS select an LLVM toolchain explicitly instead of
the system compiler. Configuration compiles the required language features.

```sh
cmake -S . -B build/core -G Ninja -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_BUILD_TYPE=Release -DNATIVE_ENABLE_IPO=ON
cmake --build build/core --parallel
ctest --test-dir build/core --output-on-failure
cmake --install build/core --prefix /path/to/native
```

`NATIVE_MINIMAL_COMPILE_OPTIONS` selects the project minimum using native compiler
options. The default is empty, retaining the toolchain's baseline. The hub and
common modules compile at that minimum; stronger implementations carry Clang function
target attributes. `NATIVE_PROFILES` selects test coverage and `NATIVE_TEST_ISA`
selects the primary regression implementation. Neither changes the hub API.

The default host tests are a small smoke suite. Set `NATIVE_TEST_EXTENDED=ON`
for exhaustive numerical checks, compiler-rejection tests and assembly comparisons.

CI splits the expensive extended builds at configuration time. To reproduce one
shard, set `NATIVE_TEST_SHARD` to its number and `NATIVE_TEST_SHARD_COUNT` to 6
(x64 and Windows ARM64) or 2 (macOS ARM64). The default, `NATIVE_TEST_SHARD=0`, builds the full
selected suite. Assignments live in `etc/cmake/test-shards.json`; new fixture
groups must be assigned explicitly. Package relocation runs once per platform.

`NATIVE_ENABLE_EXCEPTIONS` defaults to ON. Set it to OFF for an exception-free
build. Compiler, standard-library and exception modes must agree throughout
the dependency graph. `NATIVE_ENABLE_ASAN` enables
host-memory checks. ISA properties and named swizzles require Clang's property
extension; exported targets supply `-fms-extensions` for `clang++`, and `clang-cl`
accepts it directly.

## Runtime coverage

Install [grcov](https://github.com/mozilla/grcov/releases) and the `llvm-cov`
and `llvm-profdata` tools matching your Clang compiler. Enable coverage in a
separate build directory:

```sh
cmake -S . -B build/coverage -G Ninja -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_BUILD_TYPE=Release -DNATIVE_ENABLE_COVERAGE=ON
cmake --build build/coverage --parallel
ctest --test-dir build/coverage --parallel --output-on-failure
cmake --build build/coverage --target native_coverage
```

Open `build/coverage/coverage/report/html/index.html`. The same directory contains
LLVM LCOV and grcov JSON reports. Coverage is collected during the tests; generating the
report does not run them again. Codecov uses LLVM’s LCOV export, including branch
counts; the grcov HTML report shows line coverage. Delete `build/coverage/coverage/raw` before
rerunning the suite to measure just that run. LLVM recreates the profile directory.

The report includes library sources, excluding tests and dependencies. It measures
runtime execution of the instantiated code on this architecture, not every
possible template or ISA configuration. Use `NATIVE_TEST_EXTENDED=ON` to measure
the extended suite. Keep coverage builds separate from performance measurements.

The dedicated coverage workflow runs the extended suite on every push to main,
using Linux x86-64/ARM64, Windows x64/ARM64 and macOS ARM64. It reuses the normal
build/test workflow with instrumentation enabled and exceptions on. Ordinary CI
remains uninstrumented. Assembly probes disable counters while sharing the same
module build; their instruction checks still run.

Each shard retains HTML and LCOV reports with its commit, compiler, configuration
and runner hardware. Codecov receives the LCOV report through GitHub OIDC, without
an upload token. Shards upload under the same platform flags, so Codecov merges
their partial reports for that commit. The required aggregate job checks that
every expected shard completed successfully. Flags distinguish OS, architecture
and suite. Hardware flags describe the runner that produced each report. A shard
that passes
`omnibus.avx512` also receives an OS/architecture-specific `avx512_extended` flag.
That flag identifies a whole shard that executed AVX-512, not an AVX-512-only slice
of its coverage. The artifact records passed/skipped profile tests in
`execution.json`. `host.json` records every feature in Native's detector registry,
including failed/unavailable observations and x86 OS vector state. Codecov's
`host_has_*` flags identify runs where Native admitted that feature; they do not
claim instruction-specific coverage. Compiling an AVX-512 variant alone does not earn the flag. Missing platforms
are not carried forward from older commits, and coverage percentages are
informational rather than merge gates. Compare reports by platform: a shared
line covered on x86 does not establish that its ARM implementation ran.

Coverage supports native `clang++` and Windows `clang-cl` builds; cross builds
are not supported. Instrumented archives need the Clang profile
runtime when linked, so use an ordinary build for installation and distribution.

## Docker

`ghcr.io/ekmett/native:latest` supplies Clang 23, clangd, clang-tidy,
clang-format, the module dependency scanner, LLD, CMake 4.4 and Ninja, with Native
and Hint installed under `/opt/native`. `CC`, `CXX` and `CMAKE_PREFIX_PATH` are
already set. The published image runs on Linux x86-64.

```sh
docker run --rm -v "$PWD:/workspace" ghcr.io/ekmett/native:latest \
  bash -c 'cmake -S . -B build/docker -G Ninja -DCMAKE_BUILD_TYPE=Release &&
           cmake --build build/docker --parallel &&
           ctest --test-dir build/docker --output-on-failure'
```

A downstream Dockerfile can start with `FROM ghcr.io/ekmett/native:latest` and
use `find_package(native CONFIG REQUIRED COMPONENTS native)`. Use a separate
build directory from host builds: module artifacts belong to their compiler
and standard library. The development image enables C++ exceptions so downstream libraries can throw.
Consumers inherit that mode from the installed CMake targets.

The [Docker workflow](https://github.com/ekmett/native/actions/workflows/docker.yml)
builds separately from normal CI. It runs the API examples against the installed
package before publishing to GitHub Container Registry. Pull requests build and
test without publishing. Changes to the image or library on `main` publish
`latest`, `llvm23` and `sha-<full commit>` tags. In-house downstream CI follows `latest` and pulls the base on each build.
The library stage rebuilds each run to fetch Hint main; the toolchain layers stay cached.

Build locally with `docker build -t native .`. To build only the toolchain,
use `docker build --target toolchain -t native-toolchain .`.

## NixOS and Nix

The flake provides `packages.<system>.native` (also the default package) and a
matching development shell for `x86_64-linux` and `aarch64-linux`. The flake pins
nixpkgs and follows Hint's `main` branch, using nixpkgs' LLVM 23 and CMake 4.4. Enable
Nix's `nix-command` and `flakes` experimental features, then run:

```sh
nix build
nix flake check
nix develop
```

`nix build` installs Native under `result` and tests an importing application
against that installation. `nix flake check` builds the same checked derivation;
it does not run the extended instruction suite. Hint is a propagated build
dependency, fetched by Nix before the build rather than by CMake during it.

Inside `nix develop`, use the usual CMake commands with `-B build/nix` to keep
Nix module artifacts separate from other toolchains. A downstream Nix package
can put this flake's `packages.<system>.native` in `buildInputs` and use
`find_package(native CONFIG REQUIRED COMPONENTS native)` with LLVM 23.

The [Nix workflow](https://github.com/ekmett/native/actions/workflows/nix.yml)
checks both Linux architectures on native runners, separately from normal CI
and Docker. It uses the pinned Nix dependencies on Ubuntu runners; it does not
boot a NixOS virtual machine.

Nix CI refreshes Hint with `nix flake update hint` before building. The committed
lockfile remains a snapshot for local builds; use the same command to refresh
Hint locally without changing nixpkgs.

## Editor setup

The checked-in `.clangd` and VS Code test settings use `build/core`, matching the
commands above. Configure and build that tree to create `compile_commands.json`
and the module artifacts. Use the matching LLVM installation for clangd. For a
different build directory, pass `--compile-commands-dir=/path/to/build` to clangd
and update the editor's test directory. CMake presets use `build/clang-release`
or `build/clang-cl-release`.

## Installed C++ modules

```cmake
cmake_minimum_required(VERSION 4.4)
project(example LANGUAGES CXX)
find_package(native CONFIG REQUIRED COMPONENTS native)
add_executable(example example.cc)
target_link_libraries(example PRIVATE native::native)
```

```cpp
import native;
using V = native::simd<float, 8, native::avx2>;
```

Add `import native.math;` to consumers that use promoted numerical kernels
such as `math::exp` or `math::sincos`. The main module supplies primitive SIMD
operations and CPU capability detection without importing those kernels.

`native::native` owns the hub module and links `native::minimal`, which owns the common
utilities. `native::common` aliases minimal. Profile-specific CMake target aliases refer to
the same hub, with no extra archive, BMI or feature flags. `import native;`
exposes the supported host profiles.

The package installs module sources instead of compiler-specific PCMs. CMake
regenerates one compatible baseline hub BMI and shares each common module.
Clang's module validation stays enabled. Function targets do not change the
compiler/STL/exception compatibility rules.

Vector architecture arguments are structural `isa<>` values and form part of the
type identity. Producer and consumer code that exchanges vectors must use the
same declarations and compiler settings.

The [source target-list helper](../docs/omnibus.md) generates selected kernel
overloads under Clang target pragmas in one source file. CPU/OS admission uses
the same feature descriptions. No per-variant CMake target is required.
The body macro receives an ISA value and declares a constrained
`template<native::isa<> A>` function. `with_isa` selects its value argument through
a `[]<native::isa<> A> { ... }` callback; that callback retains the compiler target
of its definition.
`native_target_profile(target profile)` selects compiler options for applications
that compile kernels in separate translation units.
`native_target_omnibus(target)` is a no-op.

## Headers and downstream libraries

Imports do not export macros. `native::headers` supplies `config.h`, `attributes.h`,
`isa.h` and `targets.h` under the `native/` include directory. Native implementation
headers are installed privately under `lib/native/include` for BMI regeneration.
Compiler annotations come from [Hint](https://github.com/ekmett/hint), exposed
through `native::headers`. CMake reuses `hint::hint` or an installed Hint package,
and otherwise fetches Hint main. Include `<hint.h>` for the `hint_*`
annotations; see the [attribute reference](https://ekmett.github.io/hint/hint_8h.html).

The ISA metadata header requires C++20 and the Clang property extension; the host
modules require C++26. `NATIVE_TARGET_ISA(name)` yields a checked ISA value for a
registered source target.

```sh
cmake -S . -B build/headers -G Ninja -DNATIVE_BUILD_HOST=OFF
cmake --install build/headers --prefix /path/to/native-headers
```

A shader-only or tooling consumer can use `project(... LANGUAGES NONE)` and
`find_package(native CONFIG REQUIRED COMPONENTS headers)` without a C++ compiler.
FTZ's shader wrapper and arithmetic contract belong to its own `ftz::hlsl` target.

## PCH and LTO

Module providers compile directly without a PCH; `NATIVE_ENABLE_PCH` remains a
compatibility setting. A consumer may own a PCH with standard headers and the
textual macro headers. Its compiler, exception mode, feature flags and macros
must agree with its translation unit. The package does not export a PCH.

Target attributes preserve function requirements when using ThinLTO. Keep
baseline dispatch and native entry signatures independent of register calling
conventions. Review global initializers as well as explicit native calls.

## Compiler caching

To cache ordinary compilations, put sccache on `PATH` and configure with
`-DCMAKE_CXX_COMPILER_LAUNCHER="python3;/path/to/native/.github/scripts/sccache_launcher.py"`.
On POSIX, module cache keys include the raw sources and headers listed by
CMake's dependency scan, as well as imported BMIs and explicit PCH inputs.
Identical preprocessed tokens are insufficient: BMIs retain source locations,
so even an equivalent macro rename must invalidate the affected module.
Unchanged inputs can still reuse the cache. Missing or unsupported scan files
bypass caching. Windows module, PCH and response-file invocations currently
bypass the cache.

This is a build-tree setting; the installed package does not choose the
consumer's launcher. Set `-DCMAKE_CXX_COMPILER_LAUNCHER=` to clear it.
Check `sccache --show-stats` to see which compilations are actually cached.

## WebAssembly

The C++26 named-module build requires the same structured-binding-pack and
property checks as native targets. With WASI SDK 34, CMake 4.4 and Ninja:

```sh
cmake -S . -B build-wasm -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$WASI_SDK_PATH/share/cmake/wasi-sdk-p1.cmake" \
  -DNATIVE_TEST_ISA=WASM_SIMD128 -DNATIVE_PROFILES=WASM_SIMD128 \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build-wasm
ctest --test-dir build-wasm -LE engine-conformance --output-on-failure
```

## API documentation

Doxygen 1.18 generates the guides, individual API contracts and example listings.
Graphviz renders dependency, inheritance and caller graphs as interactive SVG.
Install both tools and put `doxygen` and `dot` on `PATH`. They are only required
when `NATIVE_BUILD_DOCS=ON`; a documentation-only build does not require the host
library:

```sh
cmake -S . -B build/docs -G Ninja -DNATIVE_BUILD_HOST=OFF -DNATIVE_BUILD_DOCS=ON
cmake --build build/docs --target native_docs
```

Pandoc 3.8.2.1 renders the guides in the shared site layout.
Open `build/docs/site/index.html`. The published reference contains the
library guides and API contracts. Test projects and their READMEs stay in the
source tree.

Warnings fail the documentation build. Check the generated links with:

```sh
python doc/check_links.py build/docs/site
```
