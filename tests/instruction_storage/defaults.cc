// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/config.h>
#include <native/targets.h>
#include <type_traits>

#if NATIVE_DEFAULT_IMPORT_ORDER == 1
import native.scalar;
import native.simd;
#else
import native.simd;
import native.scalar;
#endif

using default_vector = native::simd<float, 4>;
using explicit_vector = native::simd<float, 4, NATIVE_BASELINE>;
static_assert(std::is_same_v<default_vector, explicit_vector>);
static_assert(sizeof(default_vector) == sizeof(explicit_vector));
static_assert(alignof(default_vector) == alignof(explicit_vector));
static_assert(default_vector::architecture == NATIVE_BASELINE);
static_assert(std::is_same_v<native::mask<default_vector>, native::mask<explicit_vector>>);
static_assert(std::is_same_v<native::simd<float, 1, native::scalar>,
                            native::simd<float, 1, native::isa{}>>);

// The primary SIMD and mask templates accept only the compilation target's family.
// Even an empty foreign set must not become an unconditional storage permission.
template<auto A> concept accepts_simd_tag=requires { typename native::simd<float,1,A>; };
template<auto A> concept accepts_mask_tag=requires { typename native::predicate<1,A>; };
static_assert(accepts_simd_tag<native::isa{}> && accepts_mask_tag<native::isa{}>);
static_assert(accepts_simd_tag<native::isa<native::arm>{}> == (native::target_arch==native::arm));
static_assert(accepts_simd_tag<native::isa<native::x86>{}> == (native::target_arch==native::x86));
static_assert(accepts_simd_tag<native::isa<native::wasm>{}> == (native::target_arch==native::wasm));
static_assert(accepts_mask_tag<native::isa<native::arm>{}> == (native::target_arch==native::arm));
static_assert(accepts_mask_tag<native::isa<native::x86>{}> == (native::target_arch==native::x86));
static_assert(accepts_mask_tag<native::isa<native::wasm>{}> == (native::target_arch==native::wasm));

// Function target attributes do not change a previously imported default.
#if NATIVE_HOST_X86
__attribute__((target("avx2,fma")))
#elif NATIVE_HOST_NEON
__attribute__((target("neon,fullfp16")))
#endif
void stronger_function() {
  static_assert(std::is_same_v<native::simd<float, 4>, explicit_vector>);
}

int main() { return 0; }
