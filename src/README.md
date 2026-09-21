# Source and definition ownership

The public interface is organized around values, feature requirements and
instruction families. This guide describes where their definitions belong;
start with the [value guide](../docs/modules.md) or
[instruction guide](../docs/instructions.md) for application code.

| Source | Responsibility |
| --- | --- |
| `native.isa.ccm`, `native/isa.h` | Shared feature sets, ISA values, target metadata and admission declarations |
| `native/targets.h` | Textual macros for source targets and compiler-baseline snapshots |
| `native.scalar.ccm` | Scalar register operations and common element extension declarations |
| `native.wide.ccm`, `native/wide.h` | Register packs, tuple protocol and generic operation forwarding |
| `native.simd.ccm`, `native/vec.h`, `native/simd/` | SIMD storage, masks, memory policies and native operations |
| `native.math.ccm` | Promoted numerical kernels and targeted math forwarding |
| `native.{x86,arm}.*.ccm`, `native/{x86,arm}/` | Capability observers and instruction families |
| `native.numerics.ccm` | Scalar FP16/BF16 storage, conversions and numerical utilities |
| `native/attributes.h` | Named compiler modifiers for textual inclusion |

The implementation umbrella is named `vec.h` so it does not shadow Apple's
SDK `<simd/simd.h>`. The public class template is `native::simd`.
Source files use `.h` for textual inputs, `.cc` for ordinary translation units,
and `.ccm` for module interfaces.

## Modules and targets

`native::minimal` owns the shared ISA provider, capability modules, scalar
instruction utilities and common types. `native::common` is an alias.
`native::native` owns `native.simd`, `native.math`, vector instruction modules
and the architecture hubs. The main `native` module re-exports `native.simd`,
`native.features` and the host's `native.x86` or `native.arm` hub.
Numerical consumers import `native.math` explicitly.

`native.isa` is the sole module export provider of declarations from
`native/isa.h`. Other modules re-export it when exposing that vocabulary.
`native.features` adds `observe_cpu()` and the host observer;
`native.x86.features` and `native.arm.features` retain their architecture-specific
interfaces. X86-only observation and wait modules are omitted from ARM builds.

`native.wasm.features` is independent of the native host architecture.
Its public `native/wasm/features.h` header owns the pure decoder and validation
probes; `native/wasm/features.mjs` supplies the optional JavaScript adapter.
It does not add a Wasm vector backend or application loader.

## Native definitions

The SIMD provider compiles at the configured project minimum and contains
supported host implementations under Clang target attributes. Structural ISA
values select constrained definitions; an importer's feature macros do not
change the module's definitions. System headers and intrinsic helpers stay in
the global module fragment.

Vector instruction modules import `native.simd` before defining their public
bindings. Vector parameters and results use `simd<T,N,Arch>`; scalar operations
use ordinary C++ values. Masked x86 instruction forms use `predicate<N,Arch>`.
Each operation constrains its required features and retains the appropriate
compiler target. Raw intrinsic helpers remain private.

Internal requirement lists describe operations, memory and storage. Their
compiler-prerequisite closure must agree with the literal target attributes.
Public `target<A, Choices...>` selection compares exact feature sets, whereas
`abi_lookup` retains closure for internal lists. See the
[ISA guide](../docs/abi-lookup.md) for that distinction.

## Element semantics and batching

`simd_traits<T>` selects a custom element's raw storage type;
`simd_customization<T,Raw,Self>` supplies its value semantics. Built-in values
select their native storage and operations through internal traits. The complete
caller ISA remains part of the value's type even when an operation does not use
all its features. Base NEON half storage is separate from FP16 arithmetic and
BF16 dot-product requirements; transferring bits does not enable either extension.

Generic `wide` preserves ADL, array-kernel priority, result types and exception
behavior. Arrays and nested packs contribute their elements' requirements to
mixed-input operations. Arbitrary callbacks and ADL functions remain responsible
for their own instruction requirements. Common containers do not depend on a
specific numerical library.

The separate FTZ package supplies reproducible floating-point semantics through
this extension. It is a downstream consumer, not part of the native archive.
The [validation record](../docs/validation.md) distinguishes tested contracts
from compiler and runtime limitations.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
