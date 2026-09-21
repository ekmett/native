# Building and consuming native

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

`NATIVE_ENABLE_EXCEPTIONS` defaults to OFF. Producer and consumer compiler,
standard-library and exception modes must agree. `NATIVE_ENABLE_ASAN` enables
host-memory checks. ISA properties and named swizzles require Clang's property
extension; exported targets supply `-fms-extensions` for `clang++`, and `clang-cl`
accepts it directly.

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

The producer CI jobs use sccache 0.16.0 through Mozilla's commit-pinned
[sccache action](https://github.com/Mozilla-Actions/sccache-action/tree/fc920bf0ec8de6ee65d409111f7ec508035751ba)
and GitHub Actions cache backend. The action verifies the release archive's
published SHA-256 and supplies the cache service environment. The workflow keeps
`contents: read` permissions and needs no additional repository secrets or
separate `actions/cache` step.

For an opt-in local disk cache, install sccache separately, put it on `PATH`, and
add `-DCMAKE_CXX_COMPILER_LAUNCHER=sccache` to the producer configure command.
IPO remains supported; the single-module providers do not use PCHs. This
launcher is a build-tree setting; installed
packages neither require sccache nor select a consumer's launcher. Configure
without that argument for an uncached new tree, or set
`-DCMAKE_CXX_COMPILER_LAUNCHER=` to clear an existing tree's launcher.

On Linux and macOS, CI places the small
[module-map launcher](https://github.com/ekmett/simd/blob/main/.github/scripts/sccache_launcher.py) before sccache.
CMake quotes paths in its `.modmap` response files, while sccache 0.16.0's
[response-file reader](https://github.com/mozilla/sccache/blob/v0.16.0/src/compiler/gcc.rs)
bypasses any quoted response file with reason `@`. The launcher recognizes only
CMake's `-x c++-module`, quoted `-fmodule-output` and named `-fmodule-file` lines
with simple nonempty ASCII values, expanding them to equivalent argv entries.
It never changes CMake's files. This makes the pinned cache's existing module
input hashing and object/BMI output storage available to those commands.

Unknown flags, malformed or compound quotes, single quotes, escapes, whitespace
inside values, nested/other response files and oversized inputs execute the
original compiler arguments directly without caching. Expanded argv has a
conservative size limit; an `E2BIG` retry also runs the original compiler
directly. This is intentionally not a general response-file parser.

Only POSIX compiler names `clang` and `clang++`, optionally followed by a
numeric version suffix, enter this cache path. Other names, including
`c++` and target-prefixed Clang aliases, execute the original compiler
arguments directly without caching so they cannot bypass PCH input hashing.

Explicit `-include-pch` binary inputs, including CMake's `-Xclang` spelling,
are appended to `SCCACHE_EXTRAFILES`, preserving existing entries. The pinned
sccache release otherwise treats this flag only as a preprocessing argument;
an unchanged preprocessor result can conceal a changed PCH binary recorded
inside a cached module. Unknown or missing PCH inputs bypass caching.

POSIX CI runs `test_sccache_launcher.py` and the real PCH/module warm-cache
fixture `test_sccache_pch.py`. Compiler validation remains enabled. See the
[validation boundary](https://github.com/ekmett/simd/blob/main/docs/validation.md#pch-dependent-module-invalidation)
for the cache invalidation checks.

To opt into the same launcher locally, replace the plain sccache configure
argument with this CMake list (Python 3 and sccache must be available):

```sh
-DCMAKE_CXX_COMPILER_LAUNCHER="$(command -v python3);$PWD/.github/scripts/sccache_launcher.py"
```

Windows retains direct sccache; clang-cl's PCH and forwarded module flags remain
[unsupported](https://github.com/mozilla/sccache/blob/v0.16.0/src/compiler/msvc.rs).
Some CMake-synthesized BMI commands do not use a compiler launcher at all.
Dependency scanning and linking still execute, and cache misses still compile
normally. The launcher does not change module generation or compiler settings;
its PCH handling applies when a consumer or cache regression fixture uses one.

Every CI job records `sccache --show-adv-stats`, JSON statistics and the cache
version in its logs artifact, including after a failed build when setup succeeded.
The job summary includes non-cacheable reasons as well as hits, misses and cache
errors. Inspect those counters before attributing a speedup to the cache; a
successful build alone does not demonstrate reuse across workflow runs.

To check local reuse, build and run CTest, record the statistics, run
`cmake --build build/core --target clean`, then `sccache --zero-stats`, rebuild
and run CTest again. An incremental build with no work does not exercise the
cache. Use the same source/build paths and compiler; changed paths, compiler
contents or flags can prevent hits. See the cache checks in
[validation](https://github.com/ekmett/simd/blob/main/docs/validation.md#compiler-cache).

## Toolchain recipes and CI

Keep `clang++` and `clang-scan-deps` from the same LLVM 23 installation on
`PATH`. Select `CMAKE_CXX_COMPILER` explicitly when several Clang installations
are available. Keep the compiler resource directory, standard-library headers
and linker consistent with that installation. `CMAKE_PREFIX_PATH` points to
installed library packages, not to a producer's build directory.

The CI workflow configures Ninja directly, builds the providers with IPO and
without PCHs, exercises consumer-owned PCHs in relocated fixtures, runs CTest,
and checks installation. It selects AVX2 tests on Linux and Windows x86-64 runners and NEON
on Linux, macOS and Windows ARM64 runners; the hub includes every implemented host ISA family. The
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
For example, `make test CMAKE_ARGS=-DNATIVE_TEST_ISA=AVX512` requires a matching CPU
and OS vector state. Runtime tests must not be used as feature probes.

`Dockerfile` is an optional Ubuntu 24.04 / LLVM 23 build environment. It installs
build tools but does not build the library or establish a Linux qualification.
Build a local image explicitly with `docker build -t simd-build .`. CI does not
depend on a prepublished container.
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

The minimal target exports `NATIVE_MINIMAL_HAS_AVX2`, `NATIVE_MINIMAL_HAS_AVX512`,
`NATIVE_MINIMAL_HAS_AVX512_BF16`, `NATIVE_MINIMAL_HAS_AVX512_FP16`, `NATIVE_MINIMAL_HAS_NEON_FP16` and
`NATIVE_MINIMAL_HAS_NEON_BF16` as 0/1 compile
definitions from feature probes using the selected options. The NEON FP16 probe
compiles native arithmetic intrinsics because feature macros alone can survive
an explicit target-feature disable. Admission tests can distinguish the
intentional project minimum from accidental propagation of a stronger profile.
