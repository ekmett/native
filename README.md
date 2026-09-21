# native

<!-- SPDX-FileCopyrightText: 2024-2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->

C++26 SIMD values and native instructions for x86-64 and AArch64.
`simd<T,N,Arch>` keeps the element type, lane count and instruction requirements
in the type. Omitting `Arch` uses the `native.simd` module's compiler baseline.
Comparisons produce masks; `wide<V,M>` groups registers into
independent instruction chains. Operations have no runtime dispatch inside them.

Compile a kernel for the instructions it uses, then check that the CPU and OS
can execute it before entering. This example loads four floats, computes
`2*x + 1`, and replaces negative inputs with zero:

```cpp
#include <native/config.h>
#include <native/targets.h>
import native;
using namespace native;

#if NATIVE_HOST_X86
#define NATIVE_TARGET_example "avx2,fma"
#elif NATIVE_HOST_NEON
#define NATIVE_TARGET_example "neon"
#endif
constexpr auto example_isa = NATIVE_TARGET_ISA(example);

NATIVE_TARGET_PUSH(example)
void transform_four(float* out, float const* in) {
  using V = simd<float, 4, example_isa>;
  V x = V::load(in);
  mask<V> negative = x < V(0.f);
  auto y = select(negative, V(0.f), fma(x, V(2.f), V(1.f)));
  y.store(out);
}
NATIVE_TARGET_POP()

int main() {
  auto cpu = observe_cpu();
  if (!classify_isa(cpu, example_isa, NATIVE_TARGET_MINIMUM).admitted())
    return 0; // This application has no fallback kernel.
  float input[4]{-1.f, 0.f, 1.f, 2.f};
  float output[4]{};
  transform_four(output, input); // {0, 1, 3, 5}
  return 0;
}
```

The import makes the API visible. The target scope permits the compiler to use
those instructions; the capability check admits execution. None substitutes for
the others. Applications with several kernels can make this choice once at
startup; the [dispatch guide](docs/omnibus.md) shows how to compile and select a
list of variants.

## Build and use

The tested toolchain is Clang 23, CMake 4.4 and Ninja. Configuration checks C++26
structured-binding packs and the Clang property extension used by ISA values
and swizzles.

```sh
cmake -S . -B build/core -G Ninja -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_BUILD_TYPE=Release -DNATIVE_ENABLE_IPO=ON
cmake --build build/core --parallel
ctest --test-dir build/core --output-on-failure
cmake --install build/core --prefix /path/to/native
```

Use `clang-cl` on Windows. Exceptions default to disabled; set
`NATIVE_ENABLE_EXCEPTIONS=ON` for an exception-enabled application. Producer and
consumer compiler, standard-library and runtime modes must agree.

In an application configured with that installation on `CMAKE_PREFIX_PATH`:

```cmake
cmake_minimum_required(VERSION 4.4)
project(example LANGUAGES CXX)
find_package(native CONFIG REQUIRED COMPONENTS native)
add_executable(example example.cc)
target_link_libraries(example PRIVATE native::native)
```

`native::native` supplies SIMD and vector instruction modules. It links
`native::minimal`, which supplies capability detection, scalar instruction
utilities and common types. A detector-only application can import
`native.features` and link `native::minimal` (`native::common` is an alias).
Headers such as `<native/targets.h>` supply macros, which modules cannot export.

The package uses the toolchain's default baseline unless
`NATIVE_MINIMAL_COMPILE_OPTIONS` chooses a stronger one. The process must already
satisfy that minimum before runtime selection can help. Importing stronger
operations does not strengthen an ordinary caller's compiler target.
[Build details](doc/building.md) cover installation, compiler settings and PCH/LTO.

## Working with values and instructions

Start with [SIMD values, masks and memory](docs/modules.md) for construction,
short vectors, tails, swizzles and packs. A three-float vector has three logical
lanes: its load touches twelve bytes even if its register has room for four.
`native::mask<V>` names the mask associated with `V`.

[ISA values](docs/abi-lookup.md) describe requirements, from an individual feature
to presets such as `avx2`, `avx512` and `neon`. Each `isa<Family>` belongs to one
architecture: `isa<>` uses `target_arch`, while `isa<x86>`, `isa<arm>` and
`isa<wasm>` name it explicitly. `NATIVE_BASELINE` records the current translation
unit's enabled compiler features; runtime CPU observation is a
separate operation. [Target lists and dispatch](docs/omnibus.md) connect those
requirements to compiled kernels and runtime selection.

Use the [instruction guide](docs/instructions.md) when an algorithm needs a
particular dot product, conversion, polynomial operation or checksum. Vector
forms take `simd` values; scalar forms take ordinary C++ values. Their feature
requirements and arithmetic contracts remain specific to the instruction.

Import `native.math` separately for promoted numerical kernels such as
`math::exp` and `math::sincos`. Their domains and batching behavior are described
in the [value guide](docs/modules.md#promoted-math-batches). Floating-point
controls remain under application ownership. The separate FTZ package builds
reproducible binary32 arithmetic on this library's element extension.

The [WebAssembly detector](docs/wasm-features.md) observes SIMD support in a
particular engine. It provides admission data for an application's loader;
it does not supply a WebAssembly SIMD implementation or load code automatically.

[Compiled examples](tests/api/README.md) exercise the public API. The
[validation record](docs/validation.md) distinguishes compilation, native
execution and numerical checks; the [source guide](src/README.md) explains where
definitions belong. The [dispatch guide](docs/omnibus.md#native-intrinsics-and-packages)
also covers intrinsic interoperation and package requirements.

## License and contact

See [LICENSE.md](LICENSE.md) for the dual BSD-2-Clause/Apache-2.0 license and
individual source notices for retained upstream terms.

Contributions and bug reports are welcome through [GitHub](https://github.com/ekmett/simd).
Edward Kmett can also be reached as `ekmett` on Libera Chat and `@kmett` on Twitter/X.
