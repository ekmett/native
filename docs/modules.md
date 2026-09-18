# Types, modules and application dispatch

`import simd;` exposes common utilities and the package's configured native
profiles. Granular imports such as `simd.avx2` retain the same type identities
and let a translation unit use a narrower profile. The
[omnibus guide](omnibus.md) describes the explicit compilation requirements.

The type records the choices that affect storage and code generation:

```cpp
simd::vec<float, 4, simd::avx2>
simd::vec<float, 8, simd::avx512>
simd::vec<float, 4, simd::neon>
simd::vec<float, 1, simd::scalar>
```

The architecture tag names the instruction profile. `N` counts lanes in one native
register; it does not require the profile's largest register. Unsupported shapes
have no silent multi-register fallback. Use `simd::wide<V,M>` for an explicit
pack of `M` registers.

## Identity and generic algorithms

`simd::vec<T,N,Arch>` is a class template with ordinary types as architecture
tags. Vectors with different architecture arguments remain distinct even when their register widths match. The module name
controls visibility; the template arguments control overload resolution and ABI.
Importing an AVX2 module into an AVX-512 translation unit does not upgrade its
vectors or mask representation.

There is no default architecture. Class template argument deduction takes an
explicit tag, followed by lane values or an array:

```cpp
auto lanes = simd::vec(simd::avx2{}, 1.f, 2.f, 3.f, 4.f);
// vec<float,4,avx2>; copying lanes deduces the same type.
```

Generic algorithms take the architecture as a type parameter:

```cpp
template<class Arch, std::size_t N>
struct kernel {
  using V = simd::vec<float,N,Arch>;
  static void run(float const * a, float const * b, float * out) {
    auto x = V::load(a), y = V::load(b);
    fma(x,y,x).store(out);
  }
};
// Instantiate in a translation unit compiled for the selected profile:
// kernel<simd::avx2,8>::run(a,b,out);
```

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

The profile interfaces include standard headers and native intrinsic wrappers
in their global module fragments, before `export module`. The native templates
keep that linkage and are exposed through explicit exports. This arrangement
keeps intrinsic definitions available when downstream consumers instantiate them.
It is an implementation boundary, not a requirement to put every template above
the module declaration.

Each profile fixes `SIMD_PROFILE` in its producer configuration. The implementation
derives its architecture tag and private helper namespace from that value. A consumer with additional compiler features must not
change that profile's definitions. Configuration macros do not enable instructions;
normal target-local compiler flags establish the ISA.

`simd.wide` defines its container, pointwise operators, tuple protocol and
forwarding directly in its named module. It depends only on its element type.
ADL selects array kernels where provided; otherwise the element fallback applies.
Raw array kernels accept `std::array<V,M>` and use C++26 structured-binding packs.
They have no dependency on `wide`.

The common `simd.static_string`, `simd.types`, `simd.memory`, `simd.numerics`,
`simd.cpuid` and `simd.wait` APIs are also defined directly in named modules.
`simd.numerics` owns fp16/bf16 and their instantiations. CPUID/wait are x86-only;
optional wait leaves are separately targeted and do not give baseline callers
AVX requirements. These utilities have no duplicate implementation-header API.

## Extending the element type

`simd_traits<T>` identifies a custom element's raw storage type.
`simd_customization<T,Raw,Self>` supplies its value semantics. The common
`vec` specialization instantiates that customization for the selected raw register.
Existing raw float/integer/mask specializations remain direct implementations.

An extension must define the arithmetic semantics of its custom element.
The downstream FTZ library uses this boundary: it owns normalization,
reproducible math and environment admission, while using this library's raw
registers, masks and arrays. The dependency goes from FTZ to SIMD only.
All ISA modules can use the common scalar type without importing each other.

## Build and dispatch

The qualified toolchain is Clang 23, CMake 4.4 and Ninja. Configuration compiles
structured-binding-pack and property/deducing-this feature tests. Named swizzles
use Clang's `__declspec(property)` extension; this is not standard C++26 syntax.
The `simd::headers` target propagates `-fms-extensions` to Clang's GNU-style
driver, including installed consumers. The clang-cl driver enables it already.
Installed module sources and build metadata permit consumer BMI regeneration;
PCMs are compiler-specific artifacts.

```cmake
find_package(simd CONFIG REQUIRED COMPONENTS simd)
add_library(kernel_avx2 OBJECT kernel_avx2.cc)
target_link_libraries(kernel_avx2 PRIVATE simd::common simd::avx2)
simd_target_profile(kernel_avx2 AVX2)
add_executable(application dispatch.cc $<TARGET_OBJECTS:kernel_avx2>)
target_link_libraries(application PRIVATE simd::avx2 simd::common)
```

Add an AVX-512 object target with its own profile when needed. The baseline
translation unit checks the full CPU and OS-enabled vector-state requirements
before invoking an admitted entry. Pointer/scalar entry signatures keep the
boundary independent of register ABI. AVX-512 profile flags remain private to the selected producer/consumer
target; minimal usage requirements establish the configured project minimum.
The common baseline and each configured ISA profile have separate static
archives. `simd::minimal` (also named `simd::common`) supplies common modules
and runtime definitions;
`simd::avx2`, `simd::avx512` and `simd::neon` supply their respective profile
definitions and depend on common. The compatibility target `simd::simd`
provides the omnibus and links the configured archives transitively. When
extracting kernel objects with `TARGET_OBJECTS`, explicitly link their profile
archives on the final executable, as above.

A translation unit that imports both x86 modules uses AVX-512 compiler flags
and explicitly links both providers. For an ordinary baseline dispatch boundary
with ThinLTO elsewhere, disable IPO on its object target and avoid manually
inherited LTO flags. Review global initializers as well as explicit calls.

## PCHs, attributes and transitive modules

Modules do not export macros. Include `<simd/attributes.h>` to use modifiers such
as `simd_inline`, `simd_lifetimebound` or `simd_noescape`. The independent
`simd::headers` target makes these available to host and shader libraries.
Use the attributes only where their actual contracts hold.

The single-module profile providers do not use PCHs. A consumer may build its
own PCH with standard headers and the attribute header, including with IPO.
It must match the consumer's ISA, compiler/STL, exception mode and preprocessing
state.

A downstream library needs both the native archive and the correct module
provider metadata. A numerical addon places its public `CXX_MODULES` file set directly on its archive target. An additional
imported interface layer can prevent CMake 4.4 from discovering transitive module
providers; the installed third-library fixtures cover this boundary.

Build each consumer against one consistent dependency-BMI configuration. Mixing
a baseline producer BMI with another synthesized copy of the same dependency
under different ISA settings has triggered Clang 23 imported-STL crashes.
Rebuild the addon and its dependencies together from installed metadata. The
installed-package tests exercise that arrangement with PCH and ThinLTO enabled.

Compiler-mechanism fixtures, numerical regressions and relocated package tests
answer different questions. Their recorded scope is described in
[validation](validation.md). Raw approximate math retains each function's stated
domain and operation graph; wrapping it in `wide` does not strengthen its accuracy
or floating-point-environment contract.

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
requirements to consumers; stronger profile code lives in separate libraries.
Admission checks may select a stronger implementation, but the process must
already satisfy its configured minimum.
