# Source and definition ownership

The public interface is organized around values, feature requirements and
instruction families. This guide describes where their definitions belong;
start with the [value guide](../docs/modules.md) or
[instruction guide](../docs/instructions.md) for application code.

| Source | Responsibility |
| --- | --- |
| `native.isa.ccm`, `native/isa.h` | Family-typed ISA values, target metadata and admission declarations |
| `native/targets.h` | Textual macros for source targets and compiler-baseline snapshots |
| `native.scalar.ccm` | Scalar register operations and common element extension declarations |
| `native.wide.ccm`, `native/wide.h` | Register packs, tuple protocol and generic operation forwarding |
| `native.simd.ccm`, `native/simd.h`, `native/simd/` | SIMD storage, masks, memory policies and native operations |
| `native.math.ccm` | Promoted numerical kernels and targeted math forwarding |
| `native.{x86,arm}.*.ccm`, `native/{x86,arm}/` | Capability observers and instruction families |
| `native.numerics.ccm` | Scalar FP16/BF16 storage, conversions and numerical utilities |
| `native/attributes.h` | Named compiler modifiers for textual inclusion |

The shared implementation is `<native/simd.h>`; Apple's SDK remains available
as `<simd/simd.h>`. The public class template is `native::simd`.
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
The WebAssembly `native.simd` backend and `native.wasm` hub supply SIMD128
values separately from observation. Applications still own module loading; see
[the SIMD128 guide](../docs/wasm-simd.md).

## Native definitions

The SIMD provider compiles at the configured project minimum and contains
supported host implementations under Clang target attributes. Structural ISA
values select constrained definitions; an importer's feature macros do not
change the module's definitions.

**Every definition that calls a platform intrinsic stays in the global module
fragment**, between `module;` and `export module`. This includes inline and
always-inline wrappers, template bodies and raw instruction/assembly helpers.
The boundary preserves the intrinsic declarations needed when an importing
consumer inlines those definitions. It is a library invariant, not a formatting
choice. Named-module bindings call the global helpers rather than the intrinsics.

Headers included there are ordinary C++ headers, with ordinary include guards
and shared declaration identity across modules and translation units. Keep a
header when multiple global fragments need its definitions, or when textual
macro expansion requires it. Folding a single-use header must preserve its
position relative to the module declaration; moving its contents across that
boundary changes their meaning.

Vector instruction modules import `native.simd` before defining their typed
bindings. Bindings that call intrinsics are defined in the global fragment and
exported by name afterward; named-module bindings may instead call global
helpers. Textual implementation headers serve shared consumers or repeated
target expansion. Provider defaults stay with the module that supplies them.
Vector parameters and results use
`simd<T,N,Arch>`; scalar operations use ordinary C++ values. AVX-512 masked
instruction forms use `predicate<N,Arch>`; AVX2 gathers use the instruction's
full-vector sign-bit mask.
Each operation constrains its required features and retains the appropriate
compiler target. Raw intrinsic helpers remain private. The `native.arm.sm3` and
`native.arm.sm4` modules follow this split with four-word public vectors and
separate constant-evaluation semantics; their [family guide](../docs/arm-sm-crypto.md)
describes independent hardware feature bits and the coupled compiler target.

`native/simd/for_each_backend.h` expands the shared operation bodies under
separate target attributes. Those bodies are deliberately repeatable; ordinary
include guards would suppress supported targets. Constant-evaluation helpers
live beside the operations they implement. AES and SM4 word algorithms are
shared between ARM and x86, while each instruction family retains its own key
ordering and register layout. Runtime paths still call the hardware intrinsics.
Historical numerical graphs used only as test oracles belong under `tests`,
not in the installed implementation headers.

Internal requirement lists describe operations, memory and storage. Their
compiler-prerequisite closure must agree with the literal target attributes.
Public `target<A, Choices...>` selection compares exact feature sets, whereas
`abi_lookup` retains closure for internal lists. See the
[ISA guide](../docs/abi-lookup.md) for that distinction.

## Reading the macros

Macros do work here that templates cannot: spelling names, emitting target
pragmas, and parsing the same body under different compiler targets. Keep the
emitted C++ readable. A macro body should look like the declaration it produces,
with constraints, attributes and statements on separate lines where needed.

### Callback lists

`NATIVE_EXP_TARGETS(X)` is an X-macro list: it calls the macro supplied as `X`
once for each row. Each row contains an ordinal and a registered target name.
For example, the first two rows produce:

```cpp
NATIVE_EMIT_WIDE_EXP(0, avx512)
NATIVE_EMIT_WIDE_EXP(1, exp_bw)
```

The callback decides what each row means. `NATIVE_EMIT_WIDE_EXP` emits a targeted
function overload; `CHECK_EXP_TARGET` emits a `static_assert`. The ordinals must
match `detail::exp_target`'s ordered choices. Keep those checks beside the list.
Define a callback, expand the list, then `#undef` the callback at the use site.

### Declaration macros

`NATIVE_WIDE_BINARY_OPERATION(add, add, +)` supplies a functor name, a backend
method and the expression checked by its constraint. It expands to:

```cpp
struct add {
  template<class V>
    requires requires(V a) { a + a; }
  native_inline constexpr auto operator()(V const & a, V const & b) const {
    return native_ops<V>::add(a, b);
  }
};
```

The operation lists below these macros are the catalog. Read one expanded
operation to understand the shape, then read the rows to see which operations
exist. Keep each row on its own line. The backend methods retain their ordinary
`inline` spelling; changing that to `native_inline` changes when target-sensitive
inlining must happen.

### Target-list mapping

`NATIVE_DETAIL_TARGET_MAP(EMIT, context, avx512, avx2)` produces
`EMIT(context, avx512) EMIT(context, avx2)`. The mapper carries the same context
through each call and peeks at the next entry to find the end of the list.

`MAP0` and `MAP1` alternate because a macro cannot expand itself while its own
replacement is being expanded. The `EVAL` layers supply further rescans; the
`GET_END`/`NEXT` macros recognize the appended `()()()` sentinels. These are
preprocessor mechanics, not target selection. The mapper is not reentrant from
its own callback.

`NATIVE_TARGET_VARIANTS` first maps the target names to an ISA choice pack, then
maps them again to emit the bodies. The two passes are separate so the complete
choice pack is expanded before the body-emitting pass starts. The public
[dispatch guide](../docs/omnibus.md) explains target registration, overload
selection and the body callback's arguments.

### Target-scoped includes

`native/simd/for_each_backend.h` deliberately has no include guard. Its caller
sets `NATIVE_BACKEND_BODY` to a header path. For each supported backend it sets
the backend namespace, feature switches and admission constraint, pushes the
matching function target, includes that body, then clears the switches and
pops the target. The scalar block needs no target attribute.

The body is ordinary C++ parsed more than once, under different compile-time
conditions. This is why these headers stay textual. Keep intrinsic-calling
expansions in the global module fragment, and keep the setup and teardown
balanced. An include guard on a repeated body would silently remove backends.

### Swizzle names

`NATIVE_SWIZZLE_FIELD(xy, 2, 0, 1)` emits `get_xy`, `set_xy` and the `xy` property.
The first argument spells the property name; the remaining arguments describe
its result width and lane indices. Getter and setter constraints decide which
uses are valid.

The row, plane and cube macros enumerate two-, three- and four-letter names.
For example, `NATIVE_SWIZZLE_ROW2(x, 0, x, y, z, w)` emits fields for `xx`, `xy`,
`xz` and `xw`. `##` joins the name tokens; the integer arguments select lanes.
Keep those two roles visible rather than compressing several expansions onto
one line. The macros generate names; the `swizzle` template owns their behavior.

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
The [validation record](../tests/validation.md) distinguishes tested contracts
from compiler and runtime limitations.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
