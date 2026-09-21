# native

<!-- SPDX-FileCopyrightText: 2024-2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->

C++26 native operations and CPU capability detection for x86-64 and AArch64.
`import native;` exposes operations for the compilation target and a detector
for their CPU and OS-state requirements. Compile each kernel for the instructions
it uses, and check optional requirements before calling it.

`native::simd<T,N,Arch>` describes one register; `native::wide<V,M>` describes a
pack of registers. Element type, lane count and ISA remain explicit, so an
algorithm can use short vectors, native widths and independent instruction
chains without changing its arithmetic.

```cpp
#include <native/targets.h>
import native;
using namespace native;

NATIVE_TARGET_PUSH(avx2)
void arithmetic(float * output) {
  using V = simd<float, 8, avx2>;
  using M = V::mask;

  V x(2.f), y(3.f);
  M active = x < y;
  auto z = select(active, fma(x, y, V(1.f)), x);
  z.store(output);
}
NATIVE_TARGET_POP()
```

This example targets x86 AVX2. Check that the CPU and OS admit `avx2` before
calling it; the import only makes the API visible. The following snippets use
the same `using namespace native;` directive.

The ISA value is part of the type. `native::avx2`, `native::avx512`, `native::neon` and
`native::scalar` are `constexpr isa` presets; operations have no runtime dispatch.
An AVX-512 profile can also use 128-bit and 256-bit registers. Comparisons return
`V::mask`, retaining the profile's vector-mask or compact-predicate representation.

Feature requirements compose with `&` and compare by inclusion:

```cpp
constexpr isa needs = x86_feature::avx2 & x86_feature::fma;
static_assert(needs.has(x86_feature::fma));
static_assert(needs <= avx2);
static_assert(target<avx2, avx512, avx2> == 1);

auto requirements = avx2;
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
using V3 = simd<float, 3, avx2>;
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
| `native.x86` | x86 feature detection, raw instruction families and wait utilities |
| `native.arm` | AArch64 feature detection, admission and independently targeted instruction families |
| `native.x86.bmi1` | [BMI1 bit operations](docs/x86-bmi1.md), including defined zero-input TZCNT |
| `native.x86.bmi2` | [BMI2 bit operations](docs/x86-bmi2.md): deposit/extract, zero high bits, widening multiply, shifts and immediate rotate |
| `native.x86.popcnt` | [POPCNT](docs/x86-popcnt.md) for 16-, 32- and 64-bit values, with its own feature requirement |
| `native.x86.lzcnt` | [LZCNT](docs/x86-lzcnt.md) for 16-, 32- and 64-bit values, including defined zero-input counts |
| `native.x86.crc32c` | [Raw CRC32C updates](docs/x86-crc32c.md) for 8-, 16-, 32- and 64-bit operands |
| `native.x86.gfni` | [GFNI byte operations](docs/x86-gfni.md): field multiplication, affine maps and inverse-affine maps |
| `native.x86.pclmul`, `native.x86.vpclmul` | [Carry-less polynomial multiplication](docs/x86-pclmul.md) within 128-bit lanes |
| `native.x86.f16c` | [Binary32 / binary16 conversions](docs/x86-f16c.md), scalar and packed four/eight lanes |
| `native.x86.vpopcntdq` | [VPOPCNTDQ](docs/x86-vpopcntdq.md) for 32- and 64-bit lanes, including merge and zero masks |
| `native.x86.vnni` | [VNNI integer dot products](docs/x86-vnni.md), including saturation, EVEX masks and INT8/INT16 signedness extensions |
| `native.arm.crc` | [CRC32 and CRC32C updates](docs/arm-crc.md) for unsigned 8-, 16-, 32- and 64-bit operands |
| `native.arm.aes`, `native.arm.pmull`, `native.arm.sha` | [AES rounds, polynomial products and SHA instruction helpers](docs/arm-crypto.md) |
| `native.arm.features` | AArch64 OS capability observation and shared ISA admission |
| `native.arm.dotprod` | [Signed and unsigned byte dot products](docs/arm-dotprod.md) |
| `native.arm.rdm` | [Rounding, saturating fixed-point multiply-add and multiply-subtract](docs/arm-rdm.md) |
| `native.arm.fp16fml` | [FP16 products accumulated in FP32](docs/arm-fp16fml.md) |
| `native.arm.fcma` | [Complex multiply-add and rotated addition](docs/arm-fcma.md) |
| `native.arm.i8mm` | [I8MM matrix and mixed-sign dot products](docs/arm-i8mm.md) |
| `native.arm.bf16` | [BF16 dot products, matrix products and widening multiply-adds](docs/arm-bf16.md) |
| `native.scalar` | `simd<T,1,scalar>`, baseline scalar operations and extension declarations |
| `native.wide` | Generic `wide<V,M>`, pointwise operations and array-kernel forwarding |
| `native.numerics` | fp16/bf16 storage, conversions and scalar numerical utilities |
| `native.types`, `native.memory`, `native.static_string` | Type, memory and string utilities |
| `native.x86.features`, `native.x86.wait` | x86 CPU/OS capability observation, shared ISA admission and wait utilities |

The hub exposes the common vector template, ISA values and `wide`. Import
`native.math` separately for `math::exp`, `math::sin`, `math::cos`, `math::sincos`
and their batch forms. Generic math uses argument-dependent lookup, allowing an
element library to supply its own arithmetic and batched kernels. The separately
versioned FTZ library uses this extension for reproducible binary32 arithmetic.
Native arithmetic leaves the floating-point environment under application control.

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
The process must satisfy that minimum before any runtime selection can help.
Stronger implementations carry their own Clang target requirements; importing
them leaves the ordinary caller's target unchanged.

`NATIVE_BASELINE` from `<native/targets.h>` captures the current translation
unit's enabled features as a constant `isa` value. It uses Clang's resolved
flags, including explicit feature disables, and is separate from runtime CPU
detection. See [compiler baseline](docs/abi-lookup.md#compiler-baseline).

Use [source target lists](docs/omnibus.md) to compile a body for the feature sets
you choose. `native::observe_cpu()` supplies the current platform's capability
record; pass it and the matching list to `with_isa` to select the first entry
whose CPU and OS-state requirements are met. The callback takes the selected
ISA as a template argument: `[]<native::isa A> { ... }`.

Selection does not change the callback's compiler target. Keep native operations
in the generated overloads, or put your own functions inside
`NATIVE_TARGET_PUSH(name)` / `NATIVE_TARGET_POP()` scopes from
`<native/targets.h>`. Use `classify_isa` when checking one set of requirements.
The generated variants use constrained overloads and function attributes, so
they need no separate CMake targets or BMIs.

Presets include AVX2, AVX-512, AVX-512 BF16/FP16, NEON and NEON BF16/FP16. They
are ISA values; supported feature combinations can have their own source names.
CPU-model bundles remain future work. The native half operations retain their
instruction contracts: [AVX-512 FP16](tests/avx512_fp16/README.md),
[AVX-512 BF16](tests/bf16_profile/README.md),
[NEON FP16](tests/neon_fp16/README.md), and [NEON BF16](tests/neon_bf16/README.md).
Check their requirements at the call boundary, as for the other optional
instructions.

`native::native` owns the hub and links `native::minimal`, which supplies the
common utilities and ABI. `native::common` is an alias for `native::minimal`;
linking either propagates its configured minimum to consumers. Old profile
target names are aliases to the hub. The former
`simd.avx2`, `simd.avx512` and native-half modules are replaced by `import native;`.
`NATIVE_PROFILES` selects regression coverage, not the public API or BMI set.
Applications that prefer separately compiled kernels may still use
`native_target_profile`; the source target-list helper needs no such setup.

`native::headers` exposes configuration, attributes, ISA metadata and
`<native/targets.h>` for source generation. It supports `LANGUAGES NONE`
consumers and a headers-only installation with `NATIVE_BUILD_HOST=OFF`.
Include `<native/attributes.h>` when using macros such as `native_inline`;
imports do not carry macros. Using `isa.h` without modules requires C++20,
while host modules require C++26. The configuration and attribute headers
impose no new C++ language mode.

The cutover changes module names, the C++ namespace, public header prefixes,
target macros and CMake package names from `simd` to `native`. The register class
template is now `native::simd`; the former `native::vec` class name is removed.
Update extension specializations as well as ordinary uses, and use matching
`native` imports and package versions throughout downstream libraries. Earlier
`simd` packages are not interchangeable with this one. Rebuild BMIs and all
code that exchanges vector types across library boundaries. The GitHub
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
