# SIMD values, masks and application dispatch

`import native;` provides the host's SIMD types, instruction families and common
utilities. This guide starts with values and their memory contracts, then shows
how ISA requirements connect to compilation and execution. The
[README example](../README.md) includes a target scope and runtime check; vector
snippets below belong inside a function compiled for their chosen ISA.

## Values and generic algorithms

`native::simd<T,N,A>` takes an element type, a lane count and an `isa<>` value as a
non-type template argument. Vectors with different ISA values remain distinct
even when their register widths match. Imports control visibility; template
arguments control overload resolution and ABI. Using an AVX2 vector in an
AVX-512 function keeps its original mask representation and type identity.

`native::simd` is the class template itself. Extension specializations and
template-template arguments name `native::simd` directly.

The architecture argument defaults to the compiler baseline used to build
`native.simd`, so `simd<float,4>` names the same type as an explicit vector with
that ISA. This records compiler permissions, not runtime CPU detection. A
stronger function target or importer does not change a previously built
module's default.

Choose an explicit ISA for kernels with different requirements:

```cpp
native::simd<float,4,native::avx2> lanes{1.f, 2.f, 3.f, 4.f};
```

The default is supplied by `native.simd` (also imported by `native`). Standalone
headers and `native.scalar` alone keep the architecture explicit. Target-list
ordering and feature constraints apply equally to defaulted and explicit tags.

Generic algorithms take the ISA as a value parameter:

```cpp
template<native::isa<> A, std::size_t N>
struct kernel {
  using V = native::simd<float,N,A>;
  static void run(float const * a, float const * b, float * out) {
    auto x = V::load(a), y = V::load(b);
    fma(x,y,x).store(out);
  }
};
// Instantiate in a translation unit compiled for the selected profile:
// kernel<native::avx2,8>::run(a,b,out);
```

Presets such as `native::avx2` and `native::avx512` are `isa<native::x86>` values.
Use `.has(...)`, feature properties or subset comparisons to inspect them.
Single-feature construction is exact; `feature_closure` adds prerequisites
explicitly. The [ISA guide](abi-lookup.md) covers feature conjunction and
compile-time target selection.

`native::target_arch` is an `inline constexpr architecture` value identifying
the compiler target's family. Compare it with `native::arm`, `native::x86` or
`native::wasm` in generic code. It is available from `native.isa`,
`native.features`, `native` and the dependency-free `<native/config.h>` header.
It describes neither runtime CPU detection nor optional instruction support;
both 32-bit and 64-bit targets belong to their respective family. Platform
headers and unavailable native declarations still need preprocessing guards.

`isa<>` uses this family. Explicit `isa<arm>`, `isa<x86>` and `isa<wasm>` values
can describe foreign metadata, but cannot be mixed or supplied to a host vector
type from another family.

The element type supplies the arithmetic contract; the ISA determines storage
and available operations. Some instruction-specific shapes provide storage and
transfer without the full arithmetic interface of a vector profile. Having a
`simd` type does not by itself promise every operation on that type.

For example, base NEON supports four- and eight-lane `fp16` and `bf16` storage:
`simd<fp16,4,neon>` and `simd<bf16,8,neon>` can transfer their elements without
half arithmetic instructions. FP16 arithmetic needs its own `neon_fp16` feature;
BF16 dot and matrix operations need `neon_bf16`. Storing either format does not
authorize those operations or change its scalar conversion policy.

Keep dependent mathematical calls unqualified so ADL can select the register's
overload. There is no runtime architecture branch in individual operations.

## Masks and memory

`V::mask` is the type produced by comparisons of `V`. AVX2 and NEON use vector
masks; the AVX-512 profile uses compact predicates where the declared feature
set supports the lane width. Masks for custom numerical elements use the raw
storage register's representation.

`native::mask<V>` names the mask associated with `V`, ignoring cv/ref qualifiers.
Use it when a generic algorithm needs the comparison result's type.

```cpp
using V = native::simd<float, 8, native::avx2>;
using M = native::mask<V>;
V x(2.f), y(3.f);
M active = x < y;
auto chosen = select(active,x,y);
```

Masks support lane-wise `&`, `|`, `^`, `~` and `!`, with `&=`, `|=` and `^=`
for updates. Both complements invert each logical lane. `==` and `!=` also
return masks: they compare each pair of truth values rather than reducing the
whole vector. Use `any(active)`, `all(active)` or `none(active)` to obtain one
`bool` for a branch. Mask expressions do not short-circuit individual lanes;
all operands are evaluated.

Masked x86 instruction interfaces use `native::predicate<N,Arch>` directly,
where `N` is the operation's mask lane count. This represents the architectural
predicate without first packing a full vector mask. A full AVX-512 profile's
associated mask may be that same type; do not assume this for every minimal
instruction feature set. The [family guides](instructions.md) give each
operation's mask type and inactive-lane behavior.

Boolean vectors, all-zero/all-one lane masks and compact predicates have
different storage contracts. Convert between them with the named conversion
operations. Use `vector_mask_type` when an algorithm needs full vector lanes.
Mask tags describe lane width; the chosen profile determines the register
representation.

For pointer operations, specify the vector type to load:

```cpp
auto x = native::load_simd<V>(p);
auto aligned = native::load_simd<V>(p,native::simd_memory<32>{});
native::store_simd(q,x);
```

Alignment policies are caller promises. Use `load_simd_partial<V>(p,count,fill)`
and `store_simd_partial(p,value,count)` for tails. Only the requested logical
lanes are accessed; the load supplies `fill` for the rest. The streaming policy
currently uses ordinary accesses, so it carries no non-temporal-store guarantee.

## Short vectors and swizzles

The native profiles also support two- and three-lane `float`, `int32_t`,
`uint32_t` and `mask32` vectors in a 16-byte register. Lane count describes
logical elements: a three-float load or store touches twelve bytes. It does not
require a readable or writable fourth element. Padding does not participate in
comparison-mask reductions.

```cpp
using V = native::simd<float,3,native::avx2>;
V position{1.f,2.f,3.f};
auto pair = position.xy;       // simd<float,2,avx2>
auto saved = position.xyz;     // an independent simd<float,3,avx2>
position.xyz = position.zyx;   // snapshot the right side, then scatter
position.x = 4.f;              // a single component has element type float
```

Named `xyzw` swizzles apply to vectors of at most four logical lanes where the
requested result shape exists. Reads may repeat indices. Writes require a
mutable lvalue and distinct destination indices. `position.xxy` is readable;
assigning to it is ill-formed. Accessing nonexistent input lanes is ill-formed
as well. Architecture and element type are retained in vector results.

Swizzle properties invoke accessors without changing the register layout.
Lane construction supports constant evaluation. Initialize with `{}` to
value-initialize the backing register; ordinary default initialization without
braces leaves it uninitialized.

A copied swizzle is a value, not a view. Address-taking, mutable-reference binding
and assignments through a temporary swizzle such as `position.xyz.x = 5.f` are
rejected. Assign directly to the intended parent's property. Compound assignments
use the compiler's getter, binary-operation, setter rewrite; they do not invoke
the vector's compound-assignment member on a stored proxy.

## Stable compaction and expansion

The scalar, AVX2, AVX512 and NEON profiles support `float`, `int32_t` and
`uint32_t` lanes with the same compaction contract:

```cpp
using V = native::simd<uint32_t, 4, native::avx2>;
auto active = V::mask::from_bitset(0b1010);
V values{10u, 20u, 30u, 40u};
auto packed = native::compress(active, values, 99u);
// packed.value == {20, 40, 99, 99}; packed.count == 2
auto restored = native::expand(active, packed.value, V(77u));
// restored == {77, 20, 77, 40}
uint32_t output[2]{};
auto written = native::compress_store(output, 1, active, values);
// written == 1, output[0] == 20; output[1] was not accessed
```

`compress` preserves increasing logical lane order and returns both the packed
register and selected count. Its scalar fill defaults to zero and supplies
every unused logical output lane. `expand` consumes the first selected-count
lanes from its packed register, in order, and requires an explicit prior
register for the unselected positions. Both rearrange bits without
floating-point arithmetic, preserving signed zero, subnormals and NaN payloads.
Short vectors exclude physical padding from masks
and counts; their output padding is zero.

`compress_store` writes the first `min(capacity, selected_count)` selected
elements and returns the number **written**, which may be less than the count
returned by `compress`. No later destination element is read or written. A null
destination is allowed when capacity is zero or no logical lane is selected.
For a nonzero write, the destination must provide that many writable elements
of the vector's element type. These operations compact within one register.
Applications can use the returned counts to assemble batches across registers,
without runtime backend selection inside the operations.

## Several registers

`native::wide<V,M>` holds `M` values of type `V`. It separates the number of
independent registers from the number of lanes in each register. Generic
pointwise operations can use element overloads or an ADL array kernel supplied
by a numerical library; the container itself does not choose an ISA.

Numerical batches also use `std::array<V,M>`. The promoted math interface below
advances each polynomial stage across the array before starting the next stage,
so the independent chains remain visible to the compiler.

## Promoted math batches

`import native.math;` provides `math::exp` and the other promoted numerical
kernels. `import native;` provides `wide::promote`/`wide::demote<Original>` and
the register primitives used by those kernels.
Batches use `std::array`. A float promotes to
`std::array<native::simd<float,1,native::scalar>,1>`; a SIMD value promotes to a
one-element array retaining its lane count and ISA. An array adapts its elements
without adding another outer dimension. `native::wide` also adapts
to a standard array. Tuples are not accepted by promotion or promoted math.

```cpp
#include <array>
import native;
import native.math;
using V = native::simd<float,8,native::avx2>;

auto scalar_result = math::exp(1.f);                // float
auto vector_result = math::exp(V(1.f));             // V
auto batch_result = math::exp(std::array{V(1.f), V(2.f)});
// batch_result is std::array<V,2>.
```

Compile the caller for the selected vector target, as with other native
operations. Each polynomial stage advances all independent chains before the
next stage begins, rather than finishing one `exp` call per element. Results
preserve the input scalar, SIMD, array, or `native::wide` shape, including
empty and one-element containers.
These kernels support binary32 elements.

Promotion owns its values. Demotion uses the original type to restore shape,
while retaining transformed element types: a scalar comparison demotes to
`bool`, whereas a SIMD comparison retains its mask. `wide::map` performs one
elementwise stage over equal-length standard arrays. Access their elements
with ordinary indexing or `std::get`.

Pointwise operations use named functions. `std::array` arithmetic and comparison
operators are not changed: container equality still returns one `bool`, and
ordering remains lexicographic. Use `wide::cmp_lt`, `cmp_eq`, and the other
`cmp_*` functions for an array of element masks, and `wide::mask_not` to
complement those masks.

```cpp
// r and y are std::array<V,3> values.
y = wide::fma(r, y, V(0x1.555555c673724p-3f));
auto active = wide::cmp_lt(r, V(0.f));
auto doubled = wide::mul(r, V(2.f));
```

Lifted operations accept a SIMD operand alongside arrays and reuse it for every
chain. Array operands must have equal lengths. SIMD operands must match the
array element types; no implicit conversion occurs between register widths or
ISAs. `wide::constant_like(batch, value)` returns one SIMD value, with the
coefficient's scalar type matching its SIMD element type exactly.

`wide::add`, `sub`, `mul`, `div`, and `negate` provide arithmetic; `bit_and`,
`bit_or`, `bit_xor`, and `bit_not` provide bitwise operations. Comparisons,
selection, `min`/`max`, `abs`, `sqrt`, rounding, `fma`, and exponent scaling use
the same lifting rule. These stages use compile-time pack expansion.

`math::sin`, `math::cos`, and `math::sincos` also promote and restore the input
shape. Their reducer and polynomial advance stage by stage across the array.
They retain the native approximation's domain: every lane must be finite with
absolute value below 8192 radians. `sincos` shares the reducer and returns a pair
of results, each in the original shape; it retains the original paired kernel's
signed-zero behavior.

```cpp
auto [s, c] = math::sincos(std::array{V(0.25f), V(0.5f)});
// s and c are each std::array<V,2>.
```

`math::flush_to_zero` clears subnormal mantissas using integer operations,
preserving the sign of zero and the exact bits of normal values, infinities,
and NaNs. It leaves floating-point controls unchanged. `math::abs`, `sqrt`,
`floor`, `ceil`, `trunc`, and `round_even` retain the native leaf operation's
semantics through the same shape-preserving interface. Qualified aliases
`wide::exp`, `sin`, `cos`, `sincos`, and `flush_to_zero` are also available;
standard arrays do not acquire `wide` as an associated namespace for ADL.

The `log`, `log1p`, `expm1`, and `tanh` adapters delegate to an element library;
this interface does not supply native approximations for them.

`math::exp<true>` uses the existing early underflow cutoff. Both variants retain
the original polynomial, NaN behavior, and floating-point environment policy.
Batching preserves each approximation's domain, sequence of operations and
floating-point environment contract; it does not improve its accuracy guarantee.

## Features, compiler targets and runtime admission

An `isa` value records requirements. The compiler target determines which
instructions a function may contain. A capability observation records what the
CPU and OS make available. All three matter at a call boundary:

```cpp
#include <native/targets.h>
import native;

// The body is compiled for NEON; the ordinary caller retains its baseline.
NATIVE_TARGET_PUSH(neon)
void double_four(float* out, float const* in) {
  using V = native::simd<float,4,native::neon>;
  auto x = V::load(in);
  (x + x).store(out);
}
NATIVE_TARGET_POP()

bool try_double_four(float* out, float const* in) {
  auto cpu = native::observe_cpu();
  if (!native::classify_isa(cpu, native::neon, NATIVE_TARGET_MINIMUM).admitted())
    return false;
  double_four(out, in);
  return true;
}
```

This example is for AArch64. On x86, use the corresponding target and ISA value,
for example `avx2`. The [ISA guide](abi-lookup.md) explains exact feature sets,
compiler prerequisite closure and `target<A, Choices...>` overload selection.
`NATIVE_BASELINE` captures the translation unit's enabled compiler features,
including explicit disables; it does not query the executing CPU. Target pragma
scopes do not change that preprocessor snapshot. `NATIVE_TARGET_MINIMUM` supplies
the conservative inherited requirements used for admission.

For multiple implementations, use `with_isa` with an ordered finite list.
It passes the first admitted ISA as a template argument to the callback, or
returns `false` if none qualifies. That selection does not change the callback's
compiler target. Keep the native operations in appropriately targeted functions;
the [target-list guide](omnibus.md) generates those functions and the matching
admission list from one declaration. Pointer or scalar entry parameters avoid
passing vector registers across different calling conventions.

`native::observe_cpu()` returns the current platform's capability record.
`observe_x86_capabilities()` and `observe_arm_capabilities()` remain available
for architecture-specific code. The records distinguish typed `present` and
`observed` feature sets: an unobserved requirement is unknown, not confirmed
absent. Admission requires every feature to be both observed and present.
X86 also requires the enabled XCR0 register state needed by the selected ISA.
The nested `raw` fields retain diagnostics; editing them does not update the
normalized feature sets. Fill the typed sets explicitly in synthetic native
snapshots. Structural raw fixtures passed to `classify_isa` use the decoder.

ARM crypto hardware features are independent even where Clang enables a bundle:
its `aes` target includes AES and PMULL, for example. Admit the whole compiler
target before entering that function. The typed `arm_feature::ebf16` capability
records enhanced BF16 support; the application must also set FPCR.EBF to use
that arithmetic. It is not a standalone Clang target string. The
[instruction guide](instructions.md) links the exact requirements and numerical
contracts for each family.

WebAssembly uses a separate feature family and observes a particular engine.
The [detector guide](wasm-features.md) explains validation probes, unknown
observations and admission for separately compiled bodies. Native CPU support
cannot establish a Wasm engine's capabilities.

## Imports and build targets

Most applications link `native::native` and import `native`. Use a narrower
module when its boundary is useful:

| Need | Import | CMake target |
| --- | --- | --- |
| SIMD, masks and register operations | `native.simd` | `native::native` |
| A vector instruction family | Its `native.x86.*` or `native.arm.*` module | `native::native` |
| Promoted numerical kernels | `native.math` | `native::native` |
| CPU observation and admission | `native.features` | `native::minimal` |
| ISA values without an observer | `native.isa` | `native::minimal` |
| Scalar instruction utilities | The corresponding family module | `native::minimal` |
| Scalar numerics, generic packs and utilities | `native.numerics`, `native.wide`, `native.types`, `native.memory`, `native.static_string` | `native::minimal` |

The architecture hubs `native.x86` and `native.arm` include vector instruction
families and therefore belong to `native::native`. `native::common` is an alias
for `native::minimal`. Vector instruction modules import `native.simd` and use
`simd<T,N,Arch>` in their public interfaces. Scalar forms keep ordinary C++
values. Intrinsic register types are private implementation details of those
instruction interfaces.

Set `NATIVE_MINIMAL_COMPILE_OPTIONS` during project setup to choose a stronger
package minimum. Otherwise the toolchain baseline is unchanged. Linking the
common target propagates its configured minimum to consumers; the process must
already satisfy it before executing the runtime selector.

The tested toolchain is Clang 23, CMake 4.4 and Ninja. Configuration checks
structured-binding packs, properties and deducing `this`. ISA properties and
named swizzles use Clang's `__declspec(property)` extension, so C++26 support
alone is insufficient. `native::headers` supplies `-fms-extensions` for Clang's
GNU-style driver; clang-cl enables it already.

Include `<native/attributes.h>` for named compiler modifiers and
`<native/targets.h>` for source target macros. Modules do not export macros.
Installed module sources and build metadata let CMake regenerate compatible
BMIs. Provider modules compile without PCHs; consumer PCHs must agree with their
translation unit's compiler, exception and preprocessing settings. See
[building and installation](../doc/building.md) for package configuration.

Clang 23 can warn about ambiguous internal linkage when feature properties
occur through several module global fragments. `A.has(feature)` avoids that
property syntax for feature checks. Direct property expressions in constraints
also have a Clang mangling limitation; use `has` or a named concept there.
The [tooling record](validation.md#isa-value-api-tooling) gives the scope of
these compiler limitations.

## Extending the element type

`simd_traits<T>` identifies a custom element's raw storage type.
`simd_customization<T,Raw,Self>` supplies its value semantics. The common
`simd` specialization instantiates that customization for the selected raw register.
Existing raw float/integer/mask specializations remain direct implementations.

An extension defines the arithmetic semantics of its custom element. For
example, FTZ supplies normalization, reproducible math and environment checks,
while using this library's raw registers, masks and arrays. FTZ depends on
`native`; `native` does not depend on FTZ. Every ISA family can use the same
scalar type.

`native::mask<T>` names the associated mask after removing `T`'s cv/ref
qualifiers. Arithmetic scalars, `fp16` and `bf16` map to `bool`; SIMD values map
to `V::mask_type`; mask lanes and predicates map to themselves. Standard arrays
preserve their shape: `mask<std::array<T,N>>` is `std::array<mask<T>,N>`, including
nested and empty arrays. This type mapping does not change array comparison
operators.

Import `native.types` or include `<native/mask_traits.h>` for the scalar and
array trait. SIMD and numerical imports add their type specializations. A
consumer can specialize `native::mask_traits<MyType>` with a `type` member to
define its own mask. Unsupported types have no `type`, so generic code can test
`requires { typename native::mask<T>; }` without assuming every type has a mask.

Keep dependent mathematical calls unqualified so ADL can find the element's
overloads. Native intrinsic interoperation is available through `to_native()`
and `from_native()` when needed: include the platform intrinsic header before
importing and compile the containing function for its instructions. Ordinary
instruction-family calls already take `simd` values and need no such conversion.

The [source guide](../src/README.md) describes definition ownership. The
[compiled examples](../tests/api/README.md) exercise the value interfaces;
[validation](validation.md) records what was compiled or executed and on which
configurations.
