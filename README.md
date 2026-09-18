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
| `simd.avx512_bf16` | Optional AVX-512 BF16 storage and pairwise FP32 dot accumulation |
| `simd.neon` | `vec<T,N,neon>`, AArch64 NEON |
| `simd.neon_fp16` | Optional native eight-lane FP16 storage and arithmetic |
| `simd.neon_bf16` | Optional native eight-lane BF16 storage and pairwise FP32 dot accumulation |
| `simd.arm` | Baseline AArch64 OS capability observation and profile admission |
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
  -DCMAKE_BUILD_TYPE=Release -DSIMD_ENABLE_IPO=ON
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

Add `NEON_FP16` to the ARM `SIMD_PROFILES` list to opt into native half arithmetic.
Its `vec<fp16,8,neon_fp16>` follows the caller's FPCR and uses full-lane masks.
Compile optional kernels with `simd_target_profile(target NEON_FP16)` and admit
`arm_profile::neon_fp16` through `simd.arm` before entry. An omnibus importing this
profile also requires NEON_FP16 consumer flags. See the [native FP16 guide](tests/neon_fp16/README.md)
for arithmetic, OS admission and relocated package tests.

Add `NEON_BF16` to the ARM `SIMD_PROFILES` list for exact native BF16 storage
and `dot2(a,b,accumulator)` with `vec<bf16,8,neon_bf16>` inputs and a four-lane
FP32 accumulator/result. Admit `arm_profile::neon_bf16` through `simd.arm` before
entry. Native BFDOT has its own rounding/FPCR contract, including optional EBF16;
see the [BF16 guide](tests/neon_bf16/README.md). FP16 and BF16 are independent
optional features; default profiles and scalar conversions remain unchanged.

Add `AVX512_FP16` to `SIMD_PROFILES` for native half arithmetic with
`vec<fp16,32,avx512_fp16>`. It follows MXCSR rounding and uses gradual half
underflow regardless of DAZ/FTZ. Compile granular kernels for `AVX512_FP16` and
admit `x86_profile::avx512_fp16` before entry. See the [native AVX-512 half guide](tests/avx512_fp16/README.md).
For `import simd;`, use `simd_target_omnibus(target)` to enable the installed
package's exact feature union, and admit every included optional profile.

Add `AVX512_BF16` to `SIMD_PROFILES` to opt into native BF16 pairwise dot
products. Its distinct `avx512_bf16` tag adds `vec<bf16,N,avx512_bf16>` storage
for N = 8, 16, or 32 and `dot2(a,b,accumulator)` with an FP32 accumulator/result
of N/2 lanes. Compile
that consumer with `simd_target_profile(target AVX512_BF16)` and admit
`x86_profile::avx512_bf16` before entering it. The configured omnibus then also
requires that compilation profile. Default profiles and scalar half conversions
are unchanged. [The focused fixture](tests/bf16_profile/README.md) documents the
instruction contract and remaining half-format work.

Each ABI has its own static archive. `simd::simd` supplies omnibus module
metadata and links the configured archives; minimal and profile targets remain
available for granular imports. CMake rebuilds consumer
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
Linux x86-64 installed-package checks use LLVM 23.1.1 with libc++ 23; AVX2
execution and AVX-512 compile coverage are recorded separately.

## License and contact

See [LICENSE.md](LICENSE.md) for the dual BSD-2-Clause/Apache-2.0 license and
individual source notices for retained upstream terms.

Contributions and bug reports are welcome through [GitHub](https://github.com/ekmett/simd).
Edward Kmett can also be reached as `ekmett` on Libera Chat and `@kmett` on Twitter/X.


## Package baseline

`simd::minimal` owns the common ABI. Project setup chooses
`SIMD_MINIMAL_COMPILE_OPTIONS`; defaults are AVX2/FMA/BMI2 on x86 and NEON on
ARM. `simd::common` remains an alias. Linking minimal carries its configured
requirements to consumers; stronger profile code lives in separate libraries.
Admission checks may select a stronger implementation, but the process must
already satisfy its configured minimum.
