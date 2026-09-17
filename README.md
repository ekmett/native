# simd

<!-- SPDX-FileCopyrightText: 2024-2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->

C++26 SIMD with explicit element types, lane counts and architectures. A
`vec<T,N,Arch>` describes one register; `wide<V,M>` describes a pack of registers.
Keeping those choices separate lets an algorithm use short vectors, native
widths and independent instruction chains without changing its arithmetic.

```cpp
import simd;

using V = simd::vec<float, 8, simd::avx2>;
using M = V::mask;

V x(2.f), y(3.f);
M active = x < y;
auto z = select(active, fma(x, y, V(1.f)), x);

simd::wide<V, 12> batch(x);  // 96 values in twelve registers
```

Architecture is part of the type. `simd::avx2`, `simd::avx512`, `simd::neon` and
`simd::scalar` select instruction profiles; operations have no runtime dispatch.
An AVX-512 profile can also use 128-bit and 256-bit registers. Comparisons return
`V::mask`, retaining the profile's vector-mask or compact-predicate representation.

Short vectors have a logical lane count: a three-float load touches twelve bytes.
Named swizzles return owning values and support overlapping assignment:

```cpp
using V3 = simd::vec<float, 3, simd::avx2>;
V3 position{1.f, 2.f, 3.f};
auto saved = position.xy;
position.xyz = position.zyx;
```

Reads may repeat components; writes require distinct destinations on a mutable
lvalue. `saved` remains independent of subsequent changes to `position`.
The [module guide](docs/modules.md) covers deduction, masks, memory, swizzles,
custom element types and application dispatch.

## Modules

| Module | Public interface |
| --- | --- |
| `simd` | Common utilities and every configured native profile |
| `simd.avx2` | `vec<T,N,avx2>`, AVX2/FMA/BMI2 operations |
| `simd.avx512` | `vec<T,N,avx512>`, additionally AVX-512 F/DQ/BW/VL |
| `simd.neon` | `vec<T,N,neon>`, AArch64 NEON |
| `simd.scalar` | `vec<T,1,scalar>`, baseline scalar operations and extension declarations |
| `simd.wide` | Generic `wide<V,M>`, pointwise operations and array-kernel forwarding |
| `simd.numerics` | fp16/bf16 storage, conversions and scalar numerical utilities |
| `simd.types`, `simd.memory`, `simd.static_string` | Type, memory and string utilities |
| `simd.cpuid`, `simd.wait` | Baseline x86 feature queries and wait utilities |

The profile modules expose the common vector template and `wide`. Generic math
uses argument-dependent lookup, so an element library can supply its own
arithmetic and batched kernels. The downstream FTZ library
uses that extension for reproducible binary32 arithmetic. SIMD itself leaves
the floating-point environment under application control.

## Build and consume

The tested toolchain is Clang 23, CMake 4.4 and Ninja. Configuration checks C++26
structured-binding packs and the Clang property extension used by swizzles.

```sh
cmake -S . -B build/core -G Ninja -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_BUILD_TYPE=Release -DSIMD_ENABLE_PCH=ON -DSIMD_ENABLE_IPO=ON
cmake --build build/core --parallel 2
ctest --test-dir build/core --output-on-failure
cmake --install build/core --prefix /path/to/simd
```

Use `clang-cl` on Windows. Exceptions default to disabled; set
`SIMD_ENABLE_EXCEPTIONS=ON` when building for an exception-enabled application.
Producer and consumer compiler, standard-library and runtime modes must agree.

```cmake
find_package(simd CONFIG REQUIRED COMPONENTS simd)
add_executable(example example.cc)
target_link_libraries(example PRIVATE simd::simd)
simd_target_profile(example AVX512)  # default x86 package includes both profiles
```

The default x86 package contains AVX2 and AVX-512. With the tested CMake/Clang
module toolchain, `import simd;` requires an AVX-512 compilation in that package,
even when the code uses an AVX2 vector type. For an AVX2-only omnibus, build with
`-DSIMD_PROFILES=AVX2` and select `AVX2` on the consumer. On AArch64 the default
is `NEON`; use `simd::vec<float,4,simd::neon>` and select `NEON` on the consumer.
[The omnibus guide](docs/omnibus.md) explains this BMI constraint and the granular
imports available to baseline dispatchers.

The archive supplies compiled objects and omnibus module metadata. Common and
profile targets remain available for granular imports. CMake rebuilds consumer
BMIs from installed sources. `simd_target_profile` applies ISA flags only to its
target.
An application checks CPU and OS vector-state support before calling a native
entry point; linking the archive does not grant that support.

`simd::headers` exposes `<simd/attributes.h>` and the textual inputs needed to
build modules. It also supports a `LANGUAGES NONE` consumer and a headers-only
installation with `SIMD_BUILD_HOST=OFF`. Include the attribute header when using
macros such as `simd_inline`; imports do not carry macros.

[Compiled API examples](tests/api/README.md) exercise vector construction, masks,
memory, swizzles, wide values and the common utilities. [Build details](doc/building.md)
cover profiles, installation and PCH/LTO.
The [source guide](src/README.md) explains definition ownership, and
[validation](docs/validation.md) records tested configurations and their limits.

## License and contact

See [LICENSE.md](LICENSE.md) for the dual BSD-2-Clause/Apache-2.0 license and
individual source notices for retained upstream terms.

Contributions and bug reports are welcome through [GitHub](https://github.com/ekmett/simd).
Edward Kmett can also be reached as `ekmett` on Libera Chat and `@kmett` on Twitter/X.
