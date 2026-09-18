# Building and consuming simd

Use Clang 23, CMake 4.4 and Ninja. On Windows use `clang-cl` with a configured
MSVC SDK environment; on macOS select an LLVM toolchain explicitly instead of
the system compiler. Configuration compiles the required language features.

```sh
cmake -S . -B build/core -G Ninja -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_BUILD_TYPE=Release -DSIMD_ENABLE_IPO=ON
cmake --build build/core --parallel 2
ctest --test-dir build/core --output-on-failure
cmake --install build/core --prefix /path/to/simd
```

The optional [native NEON FP16 profile](../tests/neon_fp16/README.md) adds a
separate `simd::neon_fp16` archive. Configure `SIMD_PROFILES=NEON;NEON_FP16`, select
`NEON_FP16` on its consumers, and admit it through the baseline `simd.arm` module.
Use `simd_target_omnibus(target)` for a consumer of the omnibus module; the
helper selects the configured feature union.
The added `fullfp16` feature does not replace a configured minimum CPU/ISA.
Profile options follow inherited minimum options for literal C++ sources and the
qualified Ninja generators' C++ PCH sources; explicit source overrides remain last.
If a generator expression computes a source filename, the helper cannot discover
that eventual path automatically. Register each possible absolute path with
`simd_context_source_profile(target "/absolute/path/kernel.cc")` after selecting
the target profile. This is required to override a conflicting minimum feature
for such conditional sources; ordinary `COMPILE_FLAGS` alone come too early.

The independent [native NEON BF16 profile](../tests/neon_bf16/README.md) adds
`simd::neon_bf16`, enabled by `SIMD_PROFILES=NEON;NEON_BF16`. It provides exact
eight-lane storage and BFDOT pairwise accumulation into four FP32 lanes. BF16 and
FP16 can coexist; the omnibus helper enables both, while baseline dispatchers
retain the configured minimum and admit each required feature before entry.

`SIMD_PROFILES` names the ISA modules to build. The default is `AVX2;AVX512` on
x86 and `NEON` on arm64. `SIMD_TEST_ISA` chooses the test implementation; it does
not choose a runtime backend. The default package minimum is AVX2/FMA/BMI2 on x86
and NEON on ARM; project setup can change `SIMD_MINIMAL_COMPILE_OPTIONS`. Keep ISA flags off global CMake variables. Each single-module
profile compiles directly to a BMI without a PCH; `SIMD_ENABLE_PCH` is retained
only for compatibility with older build commands.
`SIMD_ENABLE_EXCEPTIONS` defaults to OFF; consumer and producer runtime/STL modes
must agree. `SIMD_ENABLE_ASAN` supports focused host-memory checks.

Named swizzles require Clang's property extension. Linking the exported header
or module targets supplies `-fms-extensions` for `clang++`; `clang-cl` already
accepts the syntax. The configure-time check exercises a property implemented
with an explicit object parameter, rather than accepting a compiler version
number alone. Direct header consumers must provide the same language option.

For an omnibus containing independent extensions such as AVX512_BF16 and
AVX512_FP16, use `simd_target_omnibus(target)`. It reads the exported
`SIMD_OMNIBUS_PROFILES` property on `simd::simd` and enables their union locally.
Existing `simd_target_profile` calls still select exactly one granular profile.
Admission remains the application's responsibility for every compiled extension.
See the [AVX-512 half profile](../tests/avx512_fp16/README.md) for its contract.

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

`simd::minimal` owns the baseline archive and common modules. Project setup
selects `SIMD_MINIMAL_COMPILE_OPTIONS` (a CMake list in native compiler spelling);
the defaults are AVX2/FMA/BMI2 on x86 and the platform NEON baseline on ARM.
`simd::common` is a compatibility alias to `simd::minimal`, with no second BMI. Each profile
(`simd::avx2`, `simd::avx512`, `simd::neon`) owns its own archive and module,
and depends on common. `simd::simd` provides the omnibus and transitively links
the configured archives. Granular consumers can link only their provider.

The installed package distributes module sources, not compiler-specific PCMs.
CMake builds a local BMI for each compatible compiler configuration. Profile
source options keep AVX-512 implementation flags off the common provider;
AVX2 and AVX-512 consumers share its baseline BMI. Use `simd_target_profile`
to select the consumer's code-generation profile. Exception/STL modes must
still match; this does not disable compiler module validation.

The profile helper keeps implementation ISA flags local to the target, including
its generated PCH if a consumer uses one. Module sources retain their provider's
ISA when CMake regenerates a BMI; imported dependencies retain the minimal ABI.

## Headers and downstream libraries

An import does not export macros. For the named compiler modifiers, include
`<simd/attributes.h>` and consume `simd::headers`. Only `config.h` and
`attributes.h` are exported textual headers. Native module
implementation headers are installed privately under `lib/simd/include`; CMake
uses them to regenerate consumer BMIs without adding them to the public include
path.

```sh
cmake -S . -B build/headers -G Ninja -DSIMD_BUILD_HOST=OFF
cmake --install build/headers --prefix /path/to/simd-headers
```

A shader-only or tooling consumer can use `project(... LANGUAGES NONE)` and
`find_package(simd CONFIG REQUIRED COMPONENTS headers)` without enabling a C++
compiler. FTZ's shader wrapper and mathematical contract belong to its own
`ftz::hlsl` target, which depends on these headers.

## PCH and LTO

The single-module profile providers compile directly without PCHs. Consumer-
owned PCHs remain supported with IPO; their namespace, ISA, compiler/STL,
exception settings and macros must match the consuming translation unit.
The package does not export a PCH.

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
for the observed regression and checks required of this repair.

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
contents or flags can prevent hits. See the measured native result in
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

The minimal target exports `SIMD_MINIMAL_HAS_AVX2`, `SIMD_MINIMAL_HAS_AVX512`,
`SIMD_MINIMAL_HAS_AVX512_BF16`, `SIMD_MINIMAL_HAS_AVX512_FP16`, `SIMD_MINIMAL_HAS_NEON_FP16` and
`SIMD_MINIMAL_HAS_NEON_BF16` as 0/1 compile
definitions from feature probes using the selected options. The NEON FP16 probe
compiles native arithmetic intrinsics because feature macros alone can survive
an explicit target-feature disable. Admission tests can distinguish the
intentional project minimum from accidental propagation of a stronger profile.
