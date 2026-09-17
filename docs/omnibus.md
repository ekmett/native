# One import, explicit execution profile

`import simd;` re-exports the common utilities and every native profile selected
when the package is built. It defines no additional vector types or arithmetic.
The granular imports remain available and refer to the same entities.

```cpp
import simd;

using V = simd::vec<float, 8, simd::avx2>;
V x(2.f), y(3.f);
auto z = fma(x, y, V(1.f));
simd::wide<V, 12> batch(z);
```

The installed archive owns the omnibus module's public `CXX_MODULES` file set.
Linking it supplies the module and dependency metadata:

```cmake
find_package(simd CONFIG REQUIRED COMPONENTS simd)
add_executable(example example.cc)
target_link_libraries(example PRIVATE simd::simd)
# This package was built with SIMD_PROFILES=AVX2;AVX512.
simd_target_profile(example AVX512)
```

## Match the configured profiles

CMake generates `simd.ccm` from `SIMD_PROFILES` and installs that concrete source.
The common scalar, wide, numerics, types, memory and static-string modules are
always re-exported. x86 packages also re-export CPUID and wait. Only the selected
native profile modules appear in the import list.

With Clang 23.1.1 and CMake 4.4.3, an omnibus importer must compile for the
strongest profile included in that list:

| Package profiles | Importing target profile |
| --- | --- |
| `AVX2;AVX512` or `AVX512` | `AVX512` |
| `AVX2` | `AVX2` |
| `NEON` | `NEON` |

CMake exports each provider's `IMPORTED_CXX_MODULES_COMPILE_OPTIONS`, but its
synthesized dependency BMIs in this configuration use the importing target's
ISA options. A baseline or AVX2-only compilation of the combined omnibus fails
the existing AVX-512 feature guard. Select the profile explicitly; the package
does not add native ISA flags to every target that links its archive.

An AVX2-only application can build or select a package with
`-DSIMD_PROFILES=AVX2`. An application using a combined package can instead retain
`import simd.avx2;` in an AVX2 translation unit. Importing the combined omnibus
permits both vector families, with their distinct types and mask representations,
but that translation unit's generated code requires the selected AVX-512 ISA.

## Keep dispatch at baseline

A baseline dispatcher can link `simd::simd` without importing the omnibus. It may
use granular common modules such as `simd.cpuid`, `simd.scalar` or `simd.numerics`.
Native functions belong in separately compiled targets with explicit profiles
and pointer/scalar entry signatures. Check CPU and OS vector-state support before
calling them. Keep IPO disabled on the baseline dispatch object when preserving
that boundary; the native implementation may still use ThinLTO.

The re-export producer itself uses the strongest selected profile privately.
Its body contains only imports. Granular consumers and downstream libraries that
import only `simd.scalar` keep their existing compilation requirements.

The [installed-consumer fixture](../tests/omnibus/README.md) exercises relocation,
PCH/ThinLTO, both vector families, granular imports and baseline flag isolation.
A separate Apple M3 run passes all three NEON-only omnibus consumer tests,
plus the 30 core tests and one granular relocated consumer. See the
[source-specific validation record](validation.md).
