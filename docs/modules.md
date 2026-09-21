# Types, modules and application dispatch

`import native;` makes the platform's vector types, instruction families and
common utilities available from one module. Types record the ISA an algorithm
uses; compiler target scopes let it emit those instructions; runtime capability
checks determine whether the machine can execute them. The
[source target-list guide](omnibus.md) puts these pieces together.
Import `native.math` separately for numerical kernels that work on scalars,
vectors and batches.

## Identity and generic algorithms

`native::simd<T,N,A>` takes an element type, a lane count and an `isa` value as a
non-type template argument. Vectors with different ISA values remain distinct
even when their register widths match. The module name
controls visibility; the template arguments control overload resolution and ABI.
Using an AVX2 vector in an AVX-512 function keeps its original
mask representation and type identity.

`native::simd` is the class template itself. Extension specializations and
template-template arguments name `native::simd` directly.

Choose the ISA explicitly when constructing a vector:

```cpp
native::simd<float,4,native::avx2> lanes{1.f, 2.f, 3.f, 4.f};
```

Generic algorithms take the ISA as a value parameter:

```cpp
template<native::isa A, std::size_t N>
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

Presets such as `native::avx2` and `native::avx512` are `constexpr isa` values.
Use `.has(...)`, feature properties or subset comparisons to inspect them.
Single-feature construction is exact; `feature_closure` adds prerequisites
explicitly. The [ISA guide](abi-lookup.md) covers feature conjunction and
compile-time target selection.

When migrating from the older tag-type ISA argument to the current structural
value, rebuild BMIs and every library or executable that exchanges these vector
types: their template identities and symbol names have changed.

The element type supplies the arithmetic contract, while the ISA determines
native storage and available instructions. A numerical extension specializes
the common element hooks once and can then use every supported architecture.
Compatible x86 register shapes share operation definitions; masks and
instructions that differ still depend on the selected ISA.

Keep dependent mathematical calls unqualified so ADL can select the register's
overload. There is no runtime architecture branch in individual operations.

## Short vectors and swizzles

The native profiles also support two- and three-lane `float`, `int32_t`,
`uint32_t` and `mask32` vectors in a 16-byte register. Lane count describes
logical elements: a three-float load or store touches twelve bytes. It does not
require a readable or writable fourth element. The x86 three-lane path uses
masked memory instructions; NEON combines a two-lane access with a lane access.
Padding does not participate in comparison-mask reductions.

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

## Masks and memory

`V::mask` is the type produced by comparisons of `V`. AVX2 and NEON use vector
masks; the AVX-512 profile uses compact predicates where the declared feature
set supports the lane width. Masks for custom numerical elements use the raw
storage register's representation.

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

```cpp
using V = native::simd<float, 8, native::avx2>;
using M = native::mask<V>;
M active = x < y;
auto chosen = select(active,x,y);
```

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

## Definition placement

The hub includes system headers and native intrinsic wrappers in its global
module fragment. ISA values select constrained definitions;
Clang function target attributes establish each implementation's requirements.
Consumer feature macros do not change a module's definitions.

`native.wide` provides the container, tuple protocol and operations over packs.
Native element types select matching target overloads. Custom elements can
supply array kernels through ADL, with generic operations as a fallback. The
binary32 exponential advances its polynomial across a whole batch, as described
below.

String, type, memory and numerical utilities have independent named modules,
each with one provider. `native.numerics` supplies fp16/bf16 storage and scalar
conversions. `native.x86.features` and `native.arm.features` observe the
corresponding platform's capabilities. The x86-only `native.x86.wait` module
provides waiting instructions, with target requirements for optional operations.

The x86 instruction modules belong to `native::minimal`. Their
implementation headers remain private; vendor intrinsic declarations stay in
the global module fragment.

[F16C](x86-f16c.md) adds scalar and packed binary32/binary16 conversions in
`native.x86.f16c`, reexported by `native.x86`. The widening lane count selects
the return width explicitly. Conversions retain MXCSR effects even when their
results are unused.

On AArch64, `native.arm` reexports `native.arm.dotprod`, `native.arm.rdm`,
`native.arm.fp16fml`, `native.arm.fcma`, `native.arm.i8mm`, `native.arm.crc`,
`native.arm.aes`, `native.arm.pmull` and `native.arm.sha`.
These modules also belong to `native::minimal` and use raw native operands,
without requiring `native.simd`. Each operation takes an explicit `isa` template
argument and has its own compiler target requirement. Check the corresponding
features with `classify_isa` before calling a function compiled for that target;
the import itself neither enables instructions nor dispatches at runtime.

[ARM CRC](arm-crc.md) provides scalar CRC32 and CRC32C updates.
[ARM crypto](arm-crypto.md) provides AES rounds, unreduced polynomial products
and SHA state, schedule and logical helpers. Crypto hardware features are
independent; admit the complete `target_features` set for the compiler's
`aes`, `sha2` or `sha3` bundle before entering a leaf compiled with that target.

Use `simd::to_native()` and `simd::from_native()` to pass values between SIMD
algorithms and these instruction APIs. The documented operand shapes and lane
bounds still apply; implicit vector reinterpretation cannot make an invalid
lane valid by selecting another element format.

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

## Build and dispatch

The tested toolchain is Clang 23, CMake 4.4 and Ninja. Configuration checks
structured-binding packs, properties and deducing `this`. ISA properties and
named swizzles use Clang's `__declspec(property)` extension, so C++26 support
alone is insufficient.
The `native::headers` target propagates `-fms-extensions` to Clang's GNU-style
driver, including installed consumers. The clang-cl driver enables it already.
Installed module sources and build metadata permit consumer BMI regeneration;
PCMs are compiler-specific artifacts.

LLVM 23 can emit `-Wmodules-ambiguous-internal-linkage` at feature-property use
when the declarations occur in several module global fragments. Focused
constexpr reads, writes and constraint checks pass; the warning remains.
Consumers that treat it as an error can use `A.has(native::x86_feature::fma)` for
feature checks. See the [tooling limits](validation.md).

Link `native::native` and import `native`. The [target-list guide](omnibus.md) shows
how to compile a body for a chosen ordered list of ISA values and dispatch
after CPU/OS admission. The helper uses ordinary Clang function attributes;
users can also write attributed functions themselves or retain separate
translation units with `native_target_profile`.

Native register conversions remain implicit. Include the platform intrinsic
header before the module import when directly calling those intrinsics, and
put the containing function under the appropriate target scope.

## PCHs, attributes and transitive modules

Include `<native/attributes.h>` for named compiler modifiers and
`<native/targets.h>` for source target generation. Modules do not export macros.
Provider modules compile without PCHs; consumer PCHs are optional and must
match their translation unit's compiler, exception and preprocessing settings.

The hub and common modules each have one compatible baseline BMI. Installed
module sources carry the metadata needed for regeneration. A downstream
numerical library should put its public `CXX_MODULES` file set directly on its
archive target so CMake can discover the transitive providers.

Batching an approximation with `wide` preserves its stated domain, sequence
of operations and floating-point environment contract; it does not improve the
accuracy guarantee. The [validation record](validation.md) distinguishes
compilation checks, numerical tests and native execution.

## Promoted math batches

`import native.math;` provides `math::exp` and the other promoted numerical
kernels. `import native;` provides `wide::promote`/`wide::demote<Original>` and
the register primitives used by those kernels.
Batches use `std::array`. A float promotes to
`std::array<native::simd<float,1,native::scalar>,1>`; a SIMD value promotes to a
one-element array retaining its lane count and ISA. An array adapts its elements
without adding another outer dimension. The existing `native::wide` also adapts
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
preserve the input scalar, SIMD, array, or legacy `native::wide` shape, including
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

The legacy `log`, `log1p`, `expm1`, and `tanh` adapters delegate to an element
library; this interface does not introduce native approximations for them.

`math::exp<true>` uses the existing early underflow cutoff. Both variants retain
the original polynomial, NaN behavior, and floating-point environment policy.
The generic algorithm carries `native_inline`; targeted operation helpers use
ordinary inlining. Inlining is an optimization, not a compile-time proof of ISA
admission.

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

## Package baseline

`native::minimal` supplies the common ABI; `native::common` is an alias for it.
Set `NATIVE_MINIMAL_COMPILE_OPTIONS` during project setup to choose its minimum
ISA. The default leaves the toolchain baseline unchanged, and linking the target
propagates that minimum to consumers.

Functions that need more instructions carry their own target attributes.
Runtime admission can select one of those stronger implementations, but the
process must already be able to execute code compiled for its configured
minimum.

## CPU capabilities

`import native.isa;` exposes the shared feature sets, ISA values, target metadata
and admission interfaces without a platform observer. It is the sole module
provider of those declarations.

`import native.features;` re-exports that vocabulary and adds the native
platform's capability observer, independently of vector operations. Link
`native::common`. The umbrella also re-exports `native.x86.features` on x86,
or `native.arm.features` on AArch64. Direct architecture imports remain available.
Both feature families use the same structural ISA bitset; the native observer
and OS-state checks determine which requirements the host can execute.

`native::observe_cpu()` is the platform-neutral entry point. It returns an
`x86_capabilities` or `arm_capabilities` record for the current platform, ready
for `native::classify_isa(cpu, requirements)` or `native::with_isa`.
Architecture-specific observers remain available from their feature modules.
On x86, `native.x86` also imports [BMI1](x86-bmi1.md), [BMI2](x86-bmi2.md),
[POPCNT](x86-popcnt.md), [LZCNT](x86-lzcnt.md), [CRC32C](x86-crc32c.md),
[GFNI](x86-gfni.md), [VPOPCNTDQ](x86-vpopcntdq.md), [F16C](x86-f16c.md),
[PCLMULQDQ and VPCLMULQDQ](x86-pclmul.md),
[VNNI integer dot products](x86-vnni.md) and wait operations;
the feature-only umbrella does not import those operations. CRC32C has a
scalar feature requirement independent of the SSE4.2 compiler bundle. GFNI
requirements depend on the register width and masking mode. VPOPCNTDQ needs
AVX512F and its own feature, with AVX512VL for 128- and 256-bit forms. Each
family requires caller target attributes and CPU/OS admission before execution.
The 256-bit VPCLMULQDQ form uses AVX without requiring AVX2 or AVX-512;
512-bit products additionally require AVX512F.
VNNI keeps AVX-VNNI, AVX512VNNI and the AVX-VNNI INT8/INT16 signedness
extensions distinct. Its VEX 128/256-bit operations do not require AVX-512;
the EVEX masks and 512-bit forms carry their own requirements.

Native capability records contain `present` and `observed` typed sets:
`feature_set<x86_feature>` or `feature_set<arm_feature>`. Admission requires each
feature in both sets. X86 retains `xcr0`, `xcr0_observed`, `vendor_id` and `vendor`
separately. Nested `raw` members retain the original register/query diagnostics;
editing diagnostics does not update normalized features or OS state. Synthetic
native snapshots should fill the typed sets explicitly. Structural raw fixtures
remain usable with `classify_isa` and run through the same decoder.

ARM entries describe compiler requirements: NEON joins FP
and Advanced SIMD, FP16 joins scalar and vector arithmetic, and AES joins AES
and PMULL. The typed set is not yet a complete list of independent architectural
extension bits. Enhanced BF16 remains informational under `raw.ebf16` and
`raw.ebf16_observed`; it does not authorize or enable FPCR.EBF by itself.
