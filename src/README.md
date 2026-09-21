# Definition ownership

C++ consumers import `native` for native operations and CPU capability
detection, or individual common modules. The `native.simd` provider compiles
at the project minimum and holds all supported host ISA families under Clang
target attributes. Internal definition fragments are
emitted under the matching scopes. They select structural ISA values instead
of inheriting the importer's compiler macros.

| Path | Responsibility |
| --- | --- |
| `*.ccm` | C++26 module interfaces and common utilities |
| `native/vec.h` | Raw float, integer, boolean and mask SIMD, with a custom-element extension |
| `native/simd/` | SIMD implementations, shared element/memory policies and explicit exports |
| `native/attributes.h` | Named compiler attributes, usable by downstream libraries |
| `native.isa.ccm` | Sole module export provider for shared feature, ISA and admission declarations |
| `native.math.ccm` | Optional promoted numerical kernels and targeted math forwarding |

The implementation umbrella is named `vec.h` so it does not shadow Apple's
SDK `<simd/simd.h>`; the SDK keeps ownership of that include path.

The generic container, operators, forwarding and tuple protocol belong to
`native.wide`. ADL selects an element's array kernel without a dependency on SIMD.
`native.scalar` supplies the scalar register implementation and common extension
declarations. `native.simd` adds constrained native families and half-vector operations
after importing their numerical storage types. Intrinsic bridges stay in the
global module fragment. `native/isa.h` owns feature metadata, exported to module
consumers only by `native.isa`; `native/targets.h` supplies textual source-generation
macros. Other modules re-export `native.isa` when they expose that vocabulary.
Custom numerical elements use one common extension, independent of the ISA.

`native` re-exports `native.simd`, `native.features`, and the host's `native.x86`
or `native.arm` umbrella. `native.x86.features` and `native.arm.features` retain
architecture-specific observation APIs; `native.features` adds `observe_cpu()`
for portable callers. The `native.x86.bmi1`, `native.x86.bmi2`,
`native.x86.popcnt` and `native.x86.lzcnt` modules expose independently constrained
integer instructions. `native.x86.crc32c` adds scalar Castagnoli updates;
`native.x86.gfni` adds byte field arithmetic and affine maps;
`native.x86.vpopcntdq` counts bits in 32- and 64-bit vector lanes. These modules
belong to `native::minimal`, use raw scalar or intrinsic register types, and keep
their implementation headers in the global module fragment. Each operation
constrains `Arch` and carries its own function target attribute. Their Doxygen
groups use the corresponding feature names.
`native.x86.wait` supplies wait operations. The main hub does not re-export
`native.math`; numerical consumers import it explicitly.

Target selection is a first-match feature check: `target<A, avx512, avx2>`
returns an `int` position whose requirements fit `A`, or `-1`. Every pair is
checked for a weaker earlier choice that would make a later choice unreachable.
The selector compares exact sets. Shared `isa_list` metadata describes raw operations, memory and wide
storage; literal Clang attributes are checked against those requirements.
`abi_lookup` retains compiler-prerequisite closure for those internal lists.
Scalar raw vectors still require the empty `scalar` value.

`x86_feature` and `arm_feature` enumerators are architecture-local bit indices. `isa` owns one public `flags`
array; feature properties read and update that storage. `&` unions requirements,
`has` checks containment, and comparisons use set inclusion. Construction from
one feature is exact. Presets and compiler admission apply `feature_closure`
explicitly. Generic native definitions use `template<isa A>` and
`NATIVE_ARCH_REQUIRES(A)` constraints. The [ISA guide](../docs/abi-lookup.md)
describes the public value API.

Built-in values use internal traits to select their implementation scope.
Ordinary float, integer and mask values ignore unrelated half features; native
FP16 and BF16 values require their own extension. The complete caller ISA stays
in the value's type. Custom domains keep their declared architecture without
having to supply additional metadata. Arrays, nested packs and pairs contribute
their element requirements to mixed-input operations.

Generic `wide` preserves ADL, array-hook priority, result types and exception
behavior. Requirements of arbitrary user callbacks and ADL functions remain
the caller's responsibility. `log` and `sincos` remain extension adapters where
no raw vector overload exists.

`native.static_string`, `native.types`, `native.memory`, `native.x86.features`, `native.x86.wait` and
`native.numerics` define their APIs directly in their modules. System and intrinsic
headers remain in the global module fragment. The x86-only CPUID/wait modules
are omitted from ARM builds. `native.numerics` owns fp16/bf16 and their explicit
instantiations; there is no companion implementation translation unit.

Reproducible FTZ arithmetic, CPU floating-point admission and HLSL math belong
to the separate downstream `ftz` package. They are not part of `native.lib`.
A consumer needing attribute macros includes `<native/attributes.h>` and links
the header-only CMake target `native::headers`; modules cannot export macros.

Source files use `.h` for textual inputs, `.cc` for ordinary translation units,
and `.ccm` for module interfaces. `legacy/ein` retains the original SIMD and
profiler sources under their original names, outside the build and installation.
New implementation code uses the standard library, including `std::forward_like`.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
