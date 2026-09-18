# Definition ownership

C++ consumers import `simd` or individual named modules. The omnibus is generated
from the package's configured profiles and contains only re-exports. Headers
under `simd/` are implementation inputs for rebuilding consumer BMIs. Register and intrinsic definitions live in
the global module fragment. Array math kernels use `std::array<V,N>` and C++26
structured-binding packs; they do not depend on `wide`.

| Path | Responsibility |
| --- | --- |
| `*.ccm` | C++26 module interfaces and common utilities |
| `simd/vec.h` | Raw float, integer, boolean and mask SIMD, with a custom-element extension |
| `simd/simd/` | Array math, shared element/memory policies and explicit exports |
| `simd/attributes.h` | Named compiler attributes, usable by downstream libraries |

The implementation umbrella is named `vec.h` so it does not shadow Apple's
SDK `<simd/simd.h>`; the SDK keeps ownership of that include path.

The generic container, operators, forwarding and tuple protocol belong to
`simd.wide`. ADL selects an element's array kernel without a dependency on SIMD.
`simd.scalar` supplies a baseline scalar register implementation and the common
SIMD extension declarations. ISA modules expose `vec<T,N,Arch>` specializations for their architecture tags.
Custom numerical elements use one common extension, independent of the ISA.

`simd.static_string`, `simd.types`, `simd.memory`, `simd.cpuid`, `simd.wait` and
`simd.numerics` define their APIs directly in their modules. System and intrinsic
headers remain in the global module fragment. The x86-only CPUID/wait modules
are omitted from ARM builds. `simd.numerics` owns fp16/bf16 and their explicit
instantiations; there is no companion implementation translation unit.

Reproducible FTZ arithmetic, CPU floating-point admission and HLSL math belong
to the separate downstream `ftz` package. They are not part of `simd.lib`.
A consumer needing attribute macros includes `<simd/attributes.h>` and links
the header-only CMake target `simd::headers`; modules cannot export macros.

Source files use `.h` for textual inputs, `.cc` for ordinary translation units,
and `.ccm` for module interfaces. `legacy/ein` retains the original SIMD and
profiler sources under their original names, outside the build and installation.
New implementation code uses the standard library, including `std::forward_like`.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
