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

The ISA value is part of the type. `simd::avx2`, `simd::avx512`, `simd::neon` and
`simd::scalar` are `constexpr isa` presets; operations have no runtime dispatch.
An AVX-512 profile can also use 128-bit and 256-bit registers. Comparisons return
`V::mask`, retaining the profile's vector-mask or compact-predicate representation.

Feature requirements compose with `&` and compare by inclusion:

```cpp
constexpr simd::isa needs = simd::feature::avx2 & simd::feature::fma;
static_assert(needs.has(simd::feature::fma));
static_assert(needs <= simd::avx2);
static_assert(simd::target<simd::avx2, simd::avx512, simd::avx2> == 1);

auto requirements = simd::avx2;
requirements.f16c = true;
```

`&` unions the required bits. A single feature sets exactly one bit;
`feature_closure` adds compiler prerequisites explicitly. The presets already
include their closure. [ISA values and target selection](docs/abi-lookup.md)
covers properties, partial ordering, and the checked first-match selector.

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
The [module guide](docs/modules.md) covers construction, masks, memory, swizzles,
custom element types and application dispatch.

## Modules

| Module | Public interface |
| --- | --- |
| `simd` | All native ISA variants for the host architecture and common utilities |
| `simd.cpu` | Shared feature/ISA vocabulary and native CPU utilities, without vector operations |
| `simd.cpu.arm` | AArch64 OS capability observation and shared ISA admission |
| `simd.scalar` | `vec<T,1,scalar>`, baseline scalar operations and extension declarations |
| `simd.wide` | Generic `wide<V,M>`, pointwise operations and array-kernel forwarding |
| `simd.numerics` | fp16/bf16 storage, conversions and scalar numerical utilities |
| `simd.types`, `simd.memory`, `simd.static_string` | Type, memory and string utilities |
| `simd.cpu.x86`, `simd.wait` | x86 CPU/OS capability observation, shared ISA admission and wait utilities |

The hub exposes the common vector template, ISA values and `wide`. Generic math
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
```

The hub compiles once at the project minimum: AVX2/FMA/BMI2 on x86 and NEON on
AArch64 by default. Set `SIMD_MINIMAL_COMPILE_OPTIONS` in project setup to choose
a different minimum. Importing `simd` exposes stronger APIs without enabling
their instructions in ordinary caller code. Each native implementation carries
its own Clang target requirements; common utilities have one provider.

Use [source target lists](docs/omnibus.md) to compile a body for the feature sets
you choose, then pass the matching list to `with_isa`. It checks CPU and OS
support and invokes `callback.operator()<A>()` with the first supported ISA
value. Write the callback as `[]<simd::isa A> { ... }`; selection does not
retarget it. Generated variants have distinct constrained overloads and matching
function attributes, with no per-variant CMake targets or BMIs.

Presets include AVX2, AVX-512, AVX-512 BF16/FP16, NEON and NEON BF16/FP16. They
are ISA values; supported feature combinations can have their own source names.
CPU-model bundles remain future work. The native half operations retain their
instruction contracts: [AVX-512 FP16](tests/avx512_fp16/README.md),
[AVX-512 BF16](tests/bf16_profile/README.md),
[NEON FP16](tests/neon_fp16/README.md), and [NEON BF16](tests/neon_bf16/README.md).
Importing those APIs does not require that the CPU can execute them. Admission
belongs at the call boundary, and the process must already meet its configured
minimum.

`simd::simd` owns the hub and links `simd::minimal`, which owns the common
utilities. Old profile target names are aliases to the hub. The former
`simd.avx2`, `simd.avx512` and native-half modules are replaced by `import simd;`.
`SIMD_PROFILES` selects regression coverage, not the public API or BMI set.
Applications that prefer separately compiled kernels may still use
`simd_target_profile`; the source target-list helper needs no such setup.

`simd::headers` exposes configuration, attributes, ISA metadata and
`<simd/targets.h>` for source generation. It also supports a `LANGUAGES NONE` consumer and a headers-only
installation with `SIMD_BUILD_HOST=OFF`. Include the attribute header when using
macros such as `simd_inline`; imports do not carry macros.
Using `isa.h` without modules requires C++20; the host modules require C++26.
The configuration and attribute headers impose no new C++ language mode.

The switch from architecture tag types to ISA value template arguments changes
template identity and symbol names. Rebuild BMIs and all code that exchanges
SIMD vector types across library boundaries when updating.

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
requirements to consumers; stronger functions carry their own target attributes.
Admission checks may select a stronger implementation, but the process must
already satisfy its configured minimum.
