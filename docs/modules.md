# Types, modules and application dispatch

`import simd;` exposes the native ISA families and common utilities in one
baseline hub. [Source target lists](omnibus.md) select which application
kernels to compile and how to admit them before execution.

## Identity and generic algorithms

`simd::vec<T,N,A>` takes an element type, a lane count and an `isa` value as a
non-type template argument. Vectors with different ISA values remain distinct
even when their register widths match. The module name
controls visibility; the template arguments control overload resolution and ABI.
Using an AVX2 vector in an AVX-512 function keeps its original
mask representation and type identity.

Choose the ISA explicitly when constructing a vector:

```cpp
simd::vec<float,4,simd::avx2> lanes{1.f, 2.f, 3.f, 4.f};
```

Generic algorithms take the ISA as a value parameter:

```cpp
template<simd::isa A, std::size_t N>
struct kernel {
  using V = simd::vec<float,N,A>;
  static void run(float const * a, float const * b, float * out) {
    auto x = V::load(a), y = V::load(b);
    fma(x,y,x).store(out);
  }
};
// Instantiate in a translation unit compiled for the selected profile:
// kernel<simd::avx2,8>::run(a,b,out);
```

Presets such as `simd::avx2` and `simd::avx512` are `constexpr isa` values.
Use `.has(...)`, feature properties or subset comparisons to inspect them.
Single-feature construction is exact; `feature_closure` adds prerequisites
explicitly. The [ISA guide](abi-lookup.md) covers feature conjunction and
compile-time target selection.

Changing the ISA argument from a tag type to a structural value changes template
identity and symbol names. Rebuild BMIs and every library or executable that
exchanges these vector types when updating.

The element type supplies its arithmetic contract; the architecture supplies
native storage and instruction capabilities. A numerical extension specializes
the common element hooks once and composes with every supported architecture.
Shared x86 operations use common definitions for compatible register shapes;
mask handling and instructions that differ remain architecture-sensitive.

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
using V = simd::vec<float,3,simd::avx2>;
V position{1.f,2.f,3.f};
auto pair = position.xy;       // vec<float,2,avx2>
auto saved = position.xyz;     // an independent vec<float,3,avx2>
position.xyz = position.zyx;   // snapshot the right side, then scatter
position.x = 4.f;              // a single component has element type float
```

Named `xyzw` swizzles apply to vectors of at most four logical lanes where the
requested result shape exists. Reads may repeat indices. Writes require a
mutable lvalue and distinct destination indices. `position.xxy` is readable;
assigning to it is ill-formed. Accessing nonexistent input lanes is ill-formed
as well. Architecture and element type are retained in vector results.

Properties invoke accessors and preserve the register layout. Lane construction
supports constant evaluation, and `{}` value-initializes the backing register.
Ordinary default initialization without braces leaves it uninitialized.

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

```cpp
using V = simd::vec<float, 8, simd::avx2>;
using M = V::mask;
M active = x < y;
auto chosen = select(active,x,y);
```

A mask is tied to the comparison shape. Boolean vectors, all-zero/all-one lane
masks and compact predicates have different storage contracts. Named conversion
operations make crossings explicit. `vector_mask_type` remains available when
an algorithm needs full vector lanes. Common mask tags describe lane width;
the chosen profile determines the register representation.

Pointer operations select the result type explicitly rather than guessing a
profile from the pointer:

```cpp
auto x = simd::load_simd<V>(p);
auto aligned = simd::load_simd<V>(p,simd::simd_memory<32>{});
simd::store_simd(q,x);
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

`simd.wide` owns the container, tuple protocol and composed operations. Native
element families select matching target overloads, while custom elements keep
their ADL array-kernel preference and generic fallback. Raw array math kernels
do not depend on `wide`.

Common string, type, memory and numerical utilities retain independent named
modules with one provider each. `simd.numerics` owns fp16/bf16 storage and scalar
conversions. `simd.cpu.x86` and `simd.wait` are x86-only; `simd.cpu.arm` supplies Arm
observation. Optional wait functions have their own target requirements.

## Extending the element type

`simd_traits<T>` identifies a custom element's raw storage type.
`simd_customization<T,Raw,Self>` supplies its value semantics. The common
`vec` specialization instantiates that customization for the selected raw register.
Existing raw float/integer/mask specializations remain direct implementations.

An extension must define the arithmetic semantics of its custom element.
The downstream FTZ library uses this boundary: it owns normalization,
reproducible math and environment admission, while using this library's raw
registers, masks and arrays. The dependency goes from FTZ to SIMD only.
Every ISA family can use the common scalar type.

## Build and dispatch

The qualified toolchain is Clang 23, CMake 4.4 and Ninja. Configuration compiles
structured-binding-pack and property/deducing-this feature tests. ISA properties
and named swizzles use Clang's `__declspec(property)` extension; this is not
standard C++26 syntax.
The `simd::headers` target propagates `-fms-extensions` to Clang's GNU-style
driver, including installed consumers. The clang-cl driver enables it already.
Installed module sources and build metadata permit consumer BMI regeneration;
PCMs are compiler-specific artifacts.

LLVM 23 can emit `-Wmodules-ambiguous-internal-linkage` at feature-property use
when the declarations occur in several module global fragments. Focused
constexpr reads, writes and constraint checks pass; the warning remains.
Consumers that treat it as an error can use `A.has(simd::x86_feature::fma)` for
feature checks. See the [tooling limits](validation.md).

Link `simd::simd` and import `simd`. The [target-list guide](omnibus.md) shows
how to compile a body for a chosen ordered list of ISA values and dispatch
after CPU/OS admission. The helper uses ordinary Clang function attributes;
users can also write attributed functions themselves or retain separate
translation units with `simd_target_profile`.

Native register conversions remain implicit. Include the platform intrinsic
header before the module import when directly calling those intrinsics, and
put the containing function under the appropriate target scope.

## PCHs, attributes and transitive modules

Include `<simd/attributes.h>` for named compiler modifiers and
`<simd/targets.h>` for source target generation. Modules do not export macros.
Provider modules compile without PCHs; consumer PCHs are optional and must
match their translation unit's compiler, exception and preprocessing settings.

The hub and common modules each have one compatible baseline BMI. Installed
module sources carry the metadata needed for regeneration. A downstream
numerical library should put its public `CXX_MODULES` file set directly on its
archive target so CMake can discover the transitive providers.

Raw approximate math retains each function's stated domain and operation
graph; wrapping it in `wide` does not strengthen its accuracy or floating-point
environment contract. The [validation record](validation.md) distinguishes
compiler fixtures, numerical tests and native execution results.

## Stable compaction and expansion

The scalar, AVX2, AVX512 and NEON profiles support `float`, `int32_t` and
`uint32_t` lanes with the same compaction contract:

```cpp
using V = simd::vec<uint32_t, 4, simd::avx2>;
auto active = V::mask::from_bitset(0b1010);
V values{10u, 20u, 30u, 40u};
auto packed = simd::compress(active, values, 99u);
// packed.value == {20, 40, 99, 99}; packed.count == 2
auto restored = simd::expand(active, packed.value, V(77u));
// restored == {77, 20, 77, 40}
uint32_t output[2]{};
auto written = simd::compress_store(output, 1, active, values);
// written == 1, output[0] == 20; output[1] was not accessed
```

`compress` preserves increasing logical lane order and returns both the packed
register and selected count. Its scalar fill defaults to zero and supplies
every unused logical output lane. `expand` consumes the first selected-count
lanes from its packed register, in order, and requires an explicit prior
register for the unselected positions. Both rearrange object representations:
floating-point signed zero, subnormal bits and NaN payloads are preserved without
floating-point arithmetic. Short vectors exclude physical padding from masks
and counts; their output padding is zero.

`compress_store` writes the first `min(capacity, selected_count)` selected
elements and returns the number **written**, which may be less than the count
returned by `compress`. No later destination element is read or written. A null
destination is allowed when capacity is zero or no logical lane is selected.
For a nonzero write, the destination must provide that many writable elements
of the vector's element type. No cross-register compaction or runtime backend
selection is introduced; applications can assemble coherent batches using the
returned counts.


## Package baseline

`simd::minimal` owns the common ABI. Project setup chooses
`SIMD_MINIMAL_COMPILE_OPTIONS`; defaults are AVX2/FMA/BMI2 on x86 and NEON on
ARM. `simd::common` remains an alias. Linking minimal carries its configured
requirements to consumers; stronger functions carry their own target attributes.
Admission checks may select a stronger implementation, but the process must
already satisfy its configured minimum.

## CPU capabilities

`import simd.cpu;` exposes the shared feature/ISA vocabulary and the native
platform's capability observer, independently of vector operations. Link
`simd::common`. The umbrella re-exports `simd.cpu.x86` and `simd.wait` on x86,
or `simd.cpu.arm` on AArch64. Direct architecture imports remain available.
Both feature families use the same structural ISA bitset; the native observer
and OS-state checks determine which requirements the host can execute.

Native capability records contain `present` and `observed` typed sets:
`feature_set<x86_feature>` or `feature_set<arm_feature>`. Admission requires each
feature in both sets. X86 retains `xcr0`, `xcr0_observed`, `vendor_id` and `vendor`
separately. Nested `raw` members retain the original register/query diagnostics;
editing diagnostics does not update normalized features or OS state. Synthetic
native snapshots should fill the typed sets explicitly. Structural raw fixtures
remain usable with `classify_isa` and run through the same decoder.

ARM entries still describe the existing compiler requirements: NEON joins FP
and Advanced SIMD, FP16 joins scalar and vector arithmetic, and AES joins AES
and PMULL. The typed set is not yet a complete list of independent architectural
extension bits. Enhanced BF16 remains informational under `raw.ebf16` and
`raw.ebf16_observed`; it does not authorize or enable FPCR.EBF by itself.
