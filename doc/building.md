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

To use a local sccache installation, put it on `PATH` and configure with
`-DCMAKE_CXX_COMPILER_LAUNCHER=sccache`. This is a build-tree setting; the
installed package does not choose the consumer's launcher. Set
`-DCMAKE_CXX_COMPILER_LAUNCHER=` to clear it in an existing build.
Module and PCH caching depend on the sccache version and compiler driver.
Check `sccache --show-stats` to see which compilations are actually cached.

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

Open `build/docs/docs/html/index.html`. The published reference contains the
library guides and API contracts. Test projects and their READMEs stay in the
source tree.

Warnings fail the documentation build. Check the generated links with:

```sh
python doc/check_links.py build/docs/docs/html
```
