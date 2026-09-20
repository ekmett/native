# native

<!-- SPDX-FileCopyrightText: 2024-2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->

C++26 native operations and CPU capability detection for x86-64 and AArch64.
`import native;` exposes the platform's supported operations and the detector
used to check their instruction and OS-state requirements. Importing an API
does not enable its instructions in the caller.

`native::simd<T,N,Arch>` describes one register; `native::wide<V,M>` describes a
pack of registers. Element type, lane count and ISA remain explicit, so an
algorithm can use short vectors, native widths and independent instruction
chains without changing its arithmetic. `native::vec` is the underlying class
template and remains available for extension specializations.

```cpp
#include <native/targets.h>
import native;

NATIVE_TARGET_PUSH(avx2)
void arithmetic(float * output) {
  using V = native::simd<float, 8, native::avx2>;
  using M = V::mask;

  V x(2.f), y(3.f);
  M active = x < y;
  auto z = select(active, fma(x, y, V(1.f)), x);
  z.store(output);

  native::wide<V, 12> batch(x);  // 96 values in twelve registers
}
NATIVE_TARGET_POP()
```

Call the function after checking that the CPU admits `native::avx2`.

The ISA value is part of the type. `native::avx2`, `native::avx512`, `native::neon` and
`native::scalar` are `constexpr isa` presets; operations have no runtime dispatch.
An AVX-512 profile can also use 128-bit and 256-bit registers. Comparisons return
`V::mask`, retaining the profile's vector-mask or compact-predicate representation.

Feature requirements compose with `&` and compare by inclusion:

```cpp
constexpr native::isa needs = native::x86_feature::avx2 & native::x86_feature::fma;
static_assert(needs.has(native::x86_feature::fma));
static_assert(needs <= native::avx2);
static_assert(native::target<native::avx2, native::avx512, native::avx2> == 1);

auto requirements = native::avx2;
requirements.f16c = true;
```

`&` unions the required bits. A single feature sets exactly one bit;
`feature_closure` adds compiler prerequisites explicitly. The presets already
include their closure. [ISA values and target selection](docs/abi-lookup.md)
covers properties, partial ordering, and the checked first-match selector.

Short vectors have a logical lane count: a three-float load touches twelve bytes.
Named swizzles return owning values and support overlapping assignment:

```cpp
// Inside an AVX2-targeted function, as above:
using V3 = native::simd<float, 3, native::avx2>;
V3 position{1.f, 2.f, 3.f};
auto saved = position.xy;
position.xyz = position.zyx;
```

Reads may repeat components; writes require distinct destinations on a mutable
lvalue. `saved` remains independent of subsequent changes to `position`.
The [module guide](docs/modules.md) covers construction, masks, memory, swizzles,
custom element types and application dispatch.

## Modules

| Module | Public interface |
| --- | --- |
| `native` | All native ISA variants for the host architecture and common utilities |
| `native.simd` | SIMD registers, masks, primitive operations and common utilities |
| `native.math` | Optional promoted exponential, trigonometric and other numerical kernels |
| `native.isa` | Shared feature sets, ISA values, target metadata and admission interfaces |
| `native.features` | Shared feature/ISA vocabulary and native CPU utilities, without vector operations |
| `native.x86` | x86 feature detection, bit operations and wait utilities |
| `native.arm` | AArch64 feature detection and admission |
| `native.x86.bmi1` | [BMI1 bit operations](docs/x86-bmi1.md), including defined zero-input TZCNT |
| `native.x86.bmi2` | ISA-constrained 32-bit and 64-bit `pdep` and `pext` |
| `native.arm.features` | AArch64 OS capability observation and shared ISA admission |
| `native.scalar` | `vec<T,1,scalar>`, baseline scalar operations and extension declarations |
| `native.wide` | Generic `wide<V,M>`, pointwise operations and array-kernel forwarding |
| `native.numerics` | fp16/bf16 storage, conversions and scalar numerical utilities |
| `native.types`, `native.memory`, `native.static_string` | Type, memory and string utilities |
| `native.x86.features`, `native.x86.wait` | x86 CPU/OS capability observation, shared ISA admission and wait utilities |

The hub exposes the common vector template, ISA values and `wide`. Import
`native.math` explicitly for `math::exp`, `math::sin`, `math::cos`, `math::sincos`
and their batch forms. Generic math
uses argument-dependent lookup, so an element library can supply its own
arithmetic and batched kernels. The downstream FTZ library
uses that extension for reproducible binary32 arithmetic. SIMD itself leaves
the floating-point environment under application control.

## Build and consume

The tested toolchain is Clang 23, CMake 4.4 and Ninja. Configuration checks C++26
structured-binding packs and the Clang property extension used by ISA values
and swizzles.

```sh
cmake -S . -B build/core -G Ninja -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_BUILD_TYPE=Release -DNATIVE_ENABLE_IPO=ON
cmake --build build/core --parallel 2
ctest --test-dir build/core --output-on-failure
cmake --install build/core --prefix /path/to/native
```

Use `clang-cl` on Windows. Exceptions default to disabled; set
`NATIVE_ENABLE_EXCEPTIONS=ON` when building for an exception-enabled application.
Producer and consumer compiler, standard-library and runtime modes must agree.

```cmake
find_package(native CONFIG REQUIRED COMPONENTS native)
add_executable(example example.cc)
target_link_libraries(example PRIVATE native::native)
```

The hub compiles once at the toolchain's default baseline. Set
`NATIVE_MINIMAL_COMPILE_OPTIONS` in project setup to choose a stronger minimum.
Importing `native` exposes stronger APIs without enabling
their instructions in ordinary caller code. Each native implementation carries
its own Clang target requirements; common utilities have one provider.

Use [source target lists](docs/omnibus.md) to compile a body for the feature sets
you choose, then pass the matching list to `with_isa`. It checks CPU and OS
support and invokes `callback.operator()<A>()` with the first supported ISA
value. Write the callback as `[]<native::isa A> { ... }`; selection does not
retarget it. Generated variants have distinct constrained overloads and matching
function attributes, with no per-variant CMake targets or BMIs.

`native::observe_cpu()` returns the current platform's capability record.
`classify_isa` and `with_isa` use that record to check hardware and required OS
state. `<native/targets.h>` retains `NATIVE_TARGET_PUSH(name)` and
`NATIVE_TARGET_POP()`: target scopes control compiler code generation, while
capability checks decide whether the resulting code may run.

Presets include AVX2, AVX-512, AVX-512 BF16/FP16, NEON and NEON BF16/FP16. They
are ISA values; supported feature combinations can have their own source names.
CPU-model bundles remain future work. The native half operations retain their
instruction contracts: [AVX-512 FP16](tests/avx512_fp16/README.md),
[AVX-512 BF16](tests/bf16_profile/README.md),
[NEON FP16](tests/neon_fp16/README.md), and [NEON BF16](tests/neon_bf16/README.md).
Importing those APIs does not require that the CPU can execute them. Admission
belongs at the call boundary, and the process must already meet its configured
minimum.

`native::native` owns the hub and links `native::minimal`, which owns the common
utilities. Old profile target names are aliases to the hub. The former
`simd.avx2`, `simd.avx512` and native-half modules are replaced by `import native;`.
`NATIVE_PROFILES` selects regression coverage, not the public API or BMI set.
Applications that prefer separately compiled kernels may still use
`native_target_profile`; the source target-list helper needs no such setup.

`native::headers` exposes configuration, attributes, ISA metadata and
`<native/targets.h>` for source generation. It also supports a `LANGUAGES NONE` consumer and a headers-only
installation with `NATIVE_BUILD_HOST=OFF`. Include the attribute header when using
macros such as `native_inline`; imports do not carry macros.
Using `isa.h` without modules requires C++20; the host modules require C++26.
The configuration and attribute headers impose no new C++ language mode.

The cutover changes module names, the C++ namespace, public header prefixes,
target macros and CMake package names from `simd` to `native`. Rebuild BMIs and
all code that exchanges vector types across library boundaries. The GitHub
repository remains `ekmett/simd`.

[Compiled API examples](tests/api/README.md) exercise vector construction, masks,
memory, swizzles, wide values and the common utilities. [Build details](doc/building.md)
cover profiles, installation and PCH/LTO.
The [source guide](src/README.md) explains definition ownership, and
[validation](docs/validation.md) records tested configurations and their limits.
Linux x86-64 installed-package checks use LLVM 23.1.1 with libc++ 23; AVX2
execution and AVX-512 compile coverage are recorded separately.

## License and contact

See [LICENSE.md](LICENSE.md) for the dual BSD-2-Clause/Apache-2.0 license and
individual source notices for retained upstream terms.

Contributions and bug reports are welcome through [GitHub](https://github.com/ekmett/simd).
Edward Kmett can also be reached as `ekmett` on Libera Chat and `@kmett` on Twitter/X.


## Package baseline

`native::minimal` owns the common ABI. Project setup chooses
`NATIVE_MINIMAL_COMPILE_OPTIONS`; the default leaves the toolchain baseline
unchanged. `native::common` remains an alias. Linking minimal carries its configured
requirements to consumers; stronger functions carry their own target attributes.
Admission checks may select a stronger implementation, but the process must
already satisfy its configured minimum.
