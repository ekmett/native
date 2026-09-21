// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/attributes.h>
#include <array>
#include <cstddef>
#include <cstdint>
#if NATIVE_CONSTEXPR_HEADERS
#include <native/vec.h>
#else
import native.simd;
#endif
#if defined(__aarch64__) || defined(_M_ARM64)
constexpr auto diagnostic_arch=native::neon;
#define diagnostic_target native_target("neon")
#else
constexpr auto diagnostic_arch=native::avx2;
#define diagnostic_target native_target("avx2,fma")
#endif
using I=native::simd<std::int32_t,4,diagnostic_arch>;
using F=native::simd<float,4,diagnostic_arch>;
using Short=native::simd<float,2,diagnostic_arch>;
using B=native::simd<bool,16,diagnostic_arch>;

// These functions are compiled but never invoked: dynamic counts keep their
// documented preconditions, and valid literal counts remain accepted.
diagnostic_target void valid_counts(std::int32_t * p,float * q,std::size_t n) {
  auto a=I::load_partial(p,n); a.store_partial(p,n);
  auto b=F::load_partial(q,n); b.store_partial(q,n);
  auto c=Short::load_partial(q,2); c.store_partial(q,2);
  auto d=I::load_partial(nullptr,0); d.store_partial(nullptr,0);
  auto e=native::load_simd_partial<I>(p,4); native::store_simd_partial(p,e,4);
  auto f=native::load_simd_partial<F>(q,n); native::store_simd_partial(q,f,n);
}

diagnostic_target void invalid_counts(std::int32_t * p,float * q,std::uint32_t * bits,bool * flags) {
#if NATIVE_DIAGNOSTIC_CASE == 1
  (void)I::load_partial(p,5);
#elif NATIVE_DIAGNOSTIC_CASE == 2
  I(1).store_partial(p,5);
#elif NATIVE_DIAGNOSTIC_CASE == 3
  (void)F::load_partial(q,5);
#elif NATIVE_DIAGNOSTIC_CASE == 4
  F(1.f).store_partial(q,5);
#elif NATIVE_DIAGNOSTIC_CASE == 5
  (void)Short::load_partial(q,3);
#elif NATIVE_DIAGNOSTIC_CASE == 6
  Short(1.f).store_partial(q,3);
#elif NATIVE_DIAGNOSTIC_CASE == 7
  (void)F::load_bits_partial(bits,5);
#elif NATIVE_DIAGNOSTIC_CASE == 8
  F(1.f).store_bits_partial(bits,5);
#elif NATIVE_DIAGNOSTIC_CASE == 9
  (void)native::load_simd_partial<I>(p,5);
#elif NATIVE_DIAGNOSTIC_CASE == 10
  native::store_simd_partial(p,I(1),5);
#elif NATIVE_DIAGNOSTIC_CASE == 11
  (void)B::load_partial(flags,17);
#elif NATIVE_DIAGNOSTIC_CASE == 12
  B(true).store_partial(flags,17);
#else
  (void)p; (void)q; (void)bits; (void)flags;
#endif
}
int main() {}
