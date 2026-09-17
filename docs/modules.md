# Modules, native templates and application dispatch

One primary class template describes element, lane count and architecture:

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

`simd::vec<T,N,Arch>` is the actual primary class template, not a type-selection
alias. Architecture tags are ordinary types. Vectors with different architecture
arguments remain distinct even when their register widths match. The module name
controls visibility; the template arguments control overload resolution and ABI.
Importing an AVX2 module into an AVX-512 translation unit does not upgrade its
vectors or mask representation.

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

Properties invoke accessors; they add no proxy objects or pointers to a vector.
An empty accessor base shares their declarations; `simd_empty_bases` preserves
the native layout when the Microsoft ABI combines multiple empty bases.
The implementation shares an index-pack helper and generates the property names
from Cartesian products of component labels. Constructors still initialize only
the native backing value, preserving constexpr lane construction and `{}` value
initialization. Ordinary default initialization without braces remains
uninitialized, as with the other native vectors.

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

Alignment policies are caller promises. Partial helpers handle tails explicitly.
The existing streaming policy is an ordinary-access fallback; naming a policy
does not claim a non-temporal instruction that the implementation does not emit.

## Definition placement

Standard and intrinsic headers belong above `export module`, in the global
module fragment. Native templates and their intrinsic wrappers are defined there
and exported through using-declarations. This keeps private intrinsic symbols
from becoming missing linkage dependencies in downstream archives.

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
The downstream FTZ library is the first real consumer: it owns normalization,
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
target_link_libraries(application PRIVATE simd::simd simd::common)
```

Add an AVX-512 object target with its own profile when needed. The baseline
translation unit checks the full CPU and OS-enabled vector-state requirements
before invoking an admitted entry. Pointer/scalar entry signatures keep the
boundary independent of register ABI. SIMD profile flags are PRIVATE to each
producer/consumer target; linking the aggregate archive does not enable them.

A translation unit that imports both x86 modules uses AVX-512 compiler flags
and explicitly links both providers. For an ordinary baseline dispatch boundary
with ThinLTO elsewhere, disable IPO on its object target and avoid manually
inherited LTO flags. Review global initializers as well as explicit calls.

## PCHs, attributes and transitive modules

Modules do not export macros. Include `<simd/attributes.h>` to use modifiers such
as `simd_inline`, `simd_lifetimebound` or `simd_noescape`. The independent
`simd::headers` target makes these available to host and shader libraries.
Use the attributes only where their actual contracts hold.

PCHs are private to individual producers and must match their profile, ISA,
compiler/STL, exception mode and preprocessing state. An application's PCH is
built separately; it can include standard headers and the attribute header.

A downstream library needs both the native archive and the correct module
provider metadata. Numerical addon modules should own their public CXX_MODULES
file set directly on their archive target: hiding it behind an additional
imported interface provider lost transitive module discovery in our CMake 4.4
consumer test.

The consumer also needs one consistent effective BMI chain. A direct baseline
producer BMI combined with a differently configured synthetic dependency BMI
triggered Clang 23 imported-STL crashes in the FTZ checks. Rebuilding the addon
and its dependency BMIs together through installed module metadata passed the
same fixtures with PCH and ThinLTO retained. This is a tested packaging constraint,
not a reason to relax numerical checks or silently disable the build options.

The standalone miniature fixtures under tests demonstrate compiler mechanisms;
they are not evidence for every production numerical operation. Production
regressions and relocated installed-package consumers establish their own scope.
