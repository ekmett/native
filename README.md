# native

<!-- SPDX-FileCopyrightText: 2024-2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->

<!-- badges:start -->
[![](assets/badges/left.svg)&#8288;![build][ci-build]](https://github.com/ekmett/native/actions/workflows/build.yml?query=branch%3Amain)&#8288;[![docs][ci-docs]](https://github.com/ekmett/native/actions/workflows/docs.yml?query=branch%3Amain)&#8288;[![coverage][ci-coverage]](https://github.com/ekmett/native/actions/workflows/coverage.yml?query=branch%3Amain)&#8288;[![docker][ci-docker]](https://github.com/ekmett/native/actions/workflows/docker.yml?query=branch%3Amain)&#8288;[![nix][ci-nix]](https://github.com/ekmett/native/actions/workflows/nix.yml?query=branch%3Amain)&#8288;![](assets/badges/right.svg)

[![issues](https://img.shields.io/github/issues/ekmett/native?style=flat&label=issues&color=007ec6&logo=github&logoColor=white)](https://github.com/ekmett/native/issues)
[![commits](https://img.shields.io/github/commit-activity/w/ekmett/native?style=flat&label=commits&color=007ec6&logo=github&logoColor=white)](https://github.com/ekmett/native/activity)

[![CMake: 4.4+](https://img.shields.io/static/v1?label=CMake&message=4.4%2B&color=064F8C&style=flat&logo=cmake&logoColor=white)](CMakeLists.txt)
[![C++: 26](https://img.shields.io/static/v1?label=C%2B%2B&message=26&color=00599C&style=flat&logo=cplusplus&logoColor=white)](README.md)
[![Clang: 23](https://img.shields.io/static/v1?label=Clang&message=23&color=6f42c1&style=flat&logo=llvm&logoColor=white)](README.md)

[![OS: Linux · macOS · Windows](https://img.shields.io/static/v1?label=OS&message=Linux+%C2%B7+macOS+%C2%B7+Windows&color=64748b&style=flat)](README.md)
[![CPU: x86-64 · ARM64 · Wasm](https://img.shields.io/static/v1?label=CPU&message=x86-64+%C2%B7+ARM64+%C2%B7+Wasm&color=64748b&style=flat)](README.md)

[![license: BSD-2-Clause OR Apache-2.0](assets/badges/license.svg)](LICENSE.md)
[![Contributor Covenant: 2.0](https://img.shields.io/static/v1?label=Contributor+Covenant&message=2.0&color=007ec6&style=flat&logo=contributorcovenant&logoColor=white)](CODE_OF_CONDUCT.md)

[![docs: read](https://img.shields.io/static/v1?label=docs&message=read&color=007ec6&style=flat&logo=pandoc&logoColor=white)](https://ekmett.github.io/native/)
[![coverage](https://img.shields.io/codecov/c/github/ekmett/native?logo=codecov&logoColor=%23ffffff)](https://app.codecov.io/github/ekmett/native)
[![Docker: ghcr.io](https://img.shields.io/static/v1?label=Docker&message=ghcr.io&color=2496ED&style=flat&logo=docker&logoColor=white)](https://github.com/ekmett/native/pkgs/container/native)
[![Nix: flake](https://img.shields.io/static/v1?label=Nix&message=flake&color=5277C3&style=flat&logo=nixos&logoColor=white)](https://github.com/ekmett/native/blob/main/flake.nix)
<!-- badges:end -->

[ci-build]: https://img.shields.io/github/actions/workflow/status/ekmett/native/build.yml?branch=main&style=flat-square&label=build&logo=githubactions&logoColor=white
[ci-docs]: https://img.shields.io/github/actions/workflow/status/ekmett/native/docs.yml?branch=main&style=flat-square&label=docs
[ci-coverage]: https://img.shields.io/github/actions/workflow/status/ekmett/native/coverage.yml?branch=main&style=flat-square&label=coverage
[ci-docker]: https://img.shields.io/github/actions/workflow/status/ekmett/native/docker.yml?branch=main&style=flat-square&label=docker
[ci-nix]: https://img.shields.io/github/actions/workflow/status/ekmett/native/nix.yml?branch=main&style=flat-square&label=nix

C++26 SIMD values and instruction interfaces for x86-64, AArch64 and WebAssembly,
with a shared vocabulary for compiler features and runtime admission.

`simd<T,N,Arch>` keeps the element type, lane count and instruction requirements
in the type. Omitting `Arch` uses the `native.simd` module's compiler baseline.
Comparisons produce masks; `wide<V,M>` groups registers into
independent instruction chains. Operations have no runtime dispatch inside them.

Compile a kernel for the instructions it uses, then admit execution on the
intended CPU or Wasm engine. This x86/AArch64 example loads four floats, computes
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

[Documentation](docs/index.md) · [Instruction sets](docs/instructions.md)

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
The [Docker image](doc/building.md#docker) includes Native and the toolchain for
downstream Linux builds. A [Nix flake](doc/building.md#nixos-and-nix) provides
the package and a development shell for Linux x86-64 and AArch64.

## Working with values and instructions

Start with [SIMD values, masks and memory](docs/modules.md) for construction,
short vectors, tails, swizzles and packs. On x86 and ARM, a three-float vector has three logical
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
`math::exp`, `math::exp2`, `math::expm1`, `math::log2`, `math::log1p`, `math::tanh`, `math::atan2`, and
`math::sincos`. Their domains and batching behavior are described
in the [value guide](docs/modules.md#promoted-math-batches). The
compile-time recommendations `native::exp_width<T,K,A>`,
`atan2_width<T,K,A>` and their counterparts choose a starting register count
for `wide<simd<T,K,A>, N>`; callers can always choose another extent.
The [math guide](docs/transcendentals.md) describes polynomial degrees, accuracy
and register-count recommendations. Floating-point
controls remain under application ownership. Wasm SIMD128 promoted kernels use
separately rounded multiply/add stages; x86 and ARM use fused stages. The separate
FTZ package builds reproducible binary32 arithmetic on this library's element
extension.

The [WebAssembly backend](docs/wasm-simd.md) supplies 128-bit integer, float and
double vectors through `native.simd`, `native.wasm` and `native`. It includes
saturating arithmetic, widening and narrowing, conversions, shuffles and memory
operations. [Relaxed SIMD](docs/wasm-relaxed.md) adds the 20 relaxed operations
through `native.wasm.relaxed` and the Wasm hubs. Both use typed `simd` operands
and support constant evaluation; relaxed results can vary between engines.

The [WebAssembly detector](docs/wasm-features.md), available through
`native.wasm.features`, `native.features` or `native`, describes `simd128` and
`relaxed_simd` with `isa<wasm>`. It accepts engine observations through a C++
validation callback or an optional JavaScript adapter. On Wasm compiler targets,
`NATIVE_BASELINE` records the SIMD features enabled by the compiler separately
from runtime engine support. Applications compile and load separate modules
when they need different feature levels: an engine validates the complete
module, including instructions behind branches that are never taken.

## License and contact

See [LICENSE.md](LICENSE.md) for the dual BSD-2-Clause/Apache-2.0 license and
individual source notices for retained upstream terms.

Contributions and bug reports are welcome through [GitHub](https://github.com/ekmett/native).
Edward Kmett can also be reached as `ekmett` on Libera Chat and `@kmett` on Twitter/X.
