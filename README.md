# simd

<!-- SPDX-FileCopyrightText: 2024-2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->

C++26 SIMD types, masks and wide register-pack evaluation.

```cpp
#include <array>
import simd.avx2;

using V = simd::vec<float, 8, simd::avx2>;
using M = V::mask;

V x(2.f), y(3.f);
M active = x < y;
auto z = select(active, fma(x, y, V(1.f)), x);

simd::wide<V, 12> batch(x);  // 96 values in twelve native registers
```

`simd::vec<T,N,Arch>` is the primary class template. The architecture is a tag:
`simd::avx2`, `simd::avx512`, `simd::neon` or `simd::scalar`. Lane count remains
independent: an AVX-512 profile can use 128-bit and 256-bit registers too.
Shared operations use the same implementation across compatible register shapes.
Comparison masks preserve the selected architecture's vector or compact-predicate
representation. Arithmetic does not dispatch at runtime.

| Module | Public interface |
| --- | --- |
| `simd.avx2` | `simd::vec<T, N, simd::avx2>`, AVX2/FMA/BMI2 operations |
| `simd.avx512` | `simd::vec<T, N, simd::avx512>`, additionally AVX-512 F/DQ/BW/VL |
| `simd.neon` | `simd::vec<T, N, simd::neon>`, AArch64 NEON |
| `simd.scalar` | `simd::vec<T, 1, simd::scalar>`, baseline scalar operations and extension declarations |
| `simd.wide` | Generic `simd::wide<V,M>` with pointwise operators and array-kernel forwarding |
| `simd.numerics` | fp16/bf16 storage and conversions |
| `simd.types`, `simd.memory`, `simd.static_string` | Common type, memory and string utilities |
| `simd.cpuid`, `simd.wait` | Baseline x86 feature queries and wait utilities |

See the [module guide](docs/modules.md) for template identity, masks, extension
points, module ownership, dispatch and PCH/LTO constraints. The [source guide](src/README.md)
explains which definitions belong in modules and which remain textual inputs.
The downstream FTZ library supplies reproducible numerical types; this core has
no dependency on it.

## Build and consume

The qualified development toolchain is Clang 23, CMake 4.4 and Ninja. Configuration
compiles a real C++26 structured-binding-pack check. Consumer BMIs are rebuilt
from installed module sources; compiler-specific PCMs are not distribution artifacts.

```sh
cmake -S . -B build/core -G Ninja -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_BUILD_TYPE=Release -DSIMD_ENABLE_PCH=ON -DSIMD_ENABLE_IPO=ON
cmake --build build/core --parallel 2
ctest --test-dir build/core --output-on-failure
cmake --install build/core --prefix /path/to/simd
```

Use `clang-cl` on Windows. `SIMD_PROFILES` selects the archived producers:
`AVX2;AVX512` on x86 by default, `NEON` on arm64. `SIMD_TEST_ISA` selects the
native test target. An application remains responsible for CPU/OS admission before
executing an ISA-specific entry point.

```cmake
find_package(simd CONFIG REQUIRED COMPONENTS simd)
add_executable(example example.cc)
target_link_libraries(example PRIVATE simd::simd simd::common simd::avx2)
simd_target_profile(example AVX2)
```

The profile helper applies flags to that target only. A baseline dispatcher can
link the archive without acquiring AVX flags. A translation unit importing both
x86 profiles uses AVX-512 compilation flags and explicitly links both module providers.

`simd::headers` exports `<simd/attributes.h>` and the module implementation inputs.
It works in a `LANGUAGES NONE` consumer; configure `SIMD_BUILD_HOST=OFF` for a
headers-only installation. Include the attributes header when using macros such
as `simd_inline`: importing a module does not import macros.

[Build details](doc/building.md) cover installation and separate consumer packages.
The repository is [ekmett/simd](https://github.com/ekmett/simd).

## License and contact

See [LICENSE.md](LICENSE.md) for the dual BSD-2-Clause/Apache-2.0 license and
individual source notices for retained upstream terms.

Contributions and bug reports are welcome through GitHub. Edward Kmett can also
be reached as `ekmett` on Libera Chat and `@kmett` on Twitter/X.
