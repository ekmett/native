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

Each provider owns its ISA options. Clang allows a stronger target to import
the common baseline BMI, but rejects the reverse: an importer of an AVX-512
BMI must enable AVX-512 itself. Therefore a combined omnibus importer selects
AVX512; an AVX2-only package selects AVX2, and ARM selects NEON.

This requirement stays on the omnibus and profile sources. It does not rebuild
common modules with stronger ISA options. Prefer granular profile libraries
when only one implementation is needed.

## Keep dispatch at baseline

A baseline dispatcher can link `simd::simd` without importing the omnibus. It may
use granular common modules such as `simd.cpuid`, `simd.scalar` or `simd.numerics`.
Native functions belong in separately compiled targets with explicit profiles
and pointer/scalar entry signatures. Check CPU and OS vector-state support before
calling them. Keep IPO disabled on the baseline dispatch object when preserving
that boundary; the native implementation may still use ThinLTO.

The compatibility re-export producer uses the strongest selected profile.
Its body contains only imports. Granular consumers and downstream libraries that
import only `simd.scalar` keep their existing compilation requirements.

The [installed-consumer fixture](../tests/omnibus/README.md) exercises relocation,
consumer PCH/ThinLTO, both vector families, granular imports and isolation of
stronger profile flags from the configured minimum.
A separate Apple M3 run passes all three NEON-only omnibus consumer tests,
plus the 30 core tests and one granular relocated consumer. See the
[source-specific validation record](validation.md).


## Package baseline

`simd::minimal` owns the common ABI. Project setup chooses
`SIMD_MINIMAL_COMPILE_OPTIONS`; defaults are AVX2/FMA/BMI2 on x86 and NEON on
ARM. `simd::common` remains an alias. Linking minimal carries its configured
requirements to consumers; stronger profile code lives in separate libraries.
Admission checks may select a stronger implementation, but the process must
already satisfy its configured minimum.
