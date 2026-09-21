// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/config.h>
#include <native/targets.h>
#include <cstdint>
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
// A baseline may have scalar instruction flags without selecting a vector
// arithmetic profile. Its one-lane values must remain usable with that tag.
using default_scalar = native::simd<float, 1>;
static_assert(sizeof(default_scalar) == sizeof(float));
static_assert(default_scalar::architecture == NATIVE_BASELINE);
#if NATIVE_HOST_X86
constexpr auto scalar_features = native::feature_closure(native::x86_feature::sse2);
#elif NATIVE_HOST_NEON
constexpr auto scalar_features = native::isa<>(native::arm_feature::crc);
#else
constexpr auto scalar_features = native::scalar;
#endif
using tagged_scalar = native::simd<float, 1, scalar_features>;
using tagged_integer = native::simd<std::uint32_t, 1, scalar_features>;
static_assert(tagged_scalar::architecture == scalar_features);
static_assert(sizeof(tagged_scalar) == sizeof(float));
static_assert(sizeof(tagged_integer) == sizeof(std::uint32_t));
template<class V> concept adds = requires(V a) { a + a; };
static_assert(adds<tagged_scalar> && adds<tagged_integer>);
static_assert(!adds<native::simd<float,4,scalar_features>>);
static_assert([] {
  tagged_scalar a(2.f), b(3.f);
  tagged_integer x(0xffffffffu), y(1u);
  return (a + b).to_native() == 5.f && all(a < b) &&
    (x + y).to_native() == 0 && all(x > y);
}());

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

int main() {
  float input=3.f, output=0.f;
  auto value=native::load_simd<default_scalar>(&input);
  native::store_simd(&output,value+default_scalar(2.f));
  if(output!=5.f) return 1;
  value=native::load_simd_partial<default_scalar>(static_cast<float const *>(nullptr),0,7.f);
  native::store_simd_partial(&output,value,1);
  return output==7.f?0:2;
}
