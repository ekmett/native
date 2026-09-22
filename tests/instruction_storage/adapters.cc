// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/config.h>
#include <array>
#include <concepts>
#include <cstdint>
import native.math;

#define CHECK_UNARY(name, operation) \
  template<class V> concept has_##name = requires(V x) { operation; };
CHECK_UNARY(floor, native::floor(x))
CHECK_UNARY(ceil, native::ceil(x))
CHECK_UNARY(trunc, native::trunc(x))
CHECK_UNARY(abs, native::abs(x))
CHECK_UNARY(scaleb, native::scaleb(x,x))
CHECK_UNARY(fma, native::fma(x,x,x))
CHECK_UNARY(convert_int, native::convert<std::int32_t>(x))
CHECK_UNARY(convert_float, native::convert<float>(x))
CHECK_UNARY(exp, native::exp(x))
CHECK_UNARY(flush, native::flush_to_zero(x))
CHECK_UNARY(math_floor, math::floor(x))
CHECK_UNARY(math_exp, math::exp(x))
CHECK_UNARY(math_sin, math::sin(x))
CHECK_UNARY(array_floor, native::floor(std::array<V,1>{x}))
CHECK_UNARY(array_exp, native::exp(std::array<V,1>{x}))
CHECK_UNARY(wide_floor, native::floor(native::wide<V,1>{{x}}))
CHECK_UNARY(wide_exp, native::exp(native::wide<V,1>{{x}}))
CHECK_UNARY(array_bits, wide::bits(std::array<V,1>{x}))
CHECK_UNARY(array_from_bits, wide::from_bits(std::array<V,1>{x}))
#undef CHECK_UNARY

template<class V> concept has_compress = requires(V x,typename V::mask m) { native::compress(m,x); };
template<class V> concept has_expand = requires(V x,typename V::mask m) { native::expand(m,x,x); };
template<class V> concept has_compress_store = requires(V x,typename V::mask m,typename V::value_type * p) {
  native::compress_store(p,0,m,x);
};
template<class V> concept has_masked_scaleb = requires(V x,typename V::mask m) { native::masked_scaleb(m,x,x,x); };
template<class V> concept has_masked_scaleb_zero = requires(V x,typename V::mask m) { native::masked_scaleb_zero(m,x,x); };
template<class V> concept has_mask_bits = requires(typename V::mask m) { native::mask_bits<typename V::value_type>(m); };

template<class V> consteval bool storage_only() {
  static_assert(sizeof(V)>0);
  static_assert(!has_floor<V> && !has_ceil<V> && !has_trunc<V> && !has_abs<V>);
  static_assert(!has_scaleb<V> && !has_fma<V> && !has_masked_scaleb<V> && !has_masked_scaleb_zero<V>);
  static_assert(!has_convert_int<V> && !has_convert_float<V>);
  static_assert(!has_compress<V> && !has_expand<V> && !has_compress_store<V>);
  static_assert(!has_exp<V> && !has_flush<V>);
  static_assert(!has_math_floor<V> && !has_math_exp<V> && !has_math_sin<V>);
  static_assert(!has_array_floor<V> && !has_array_exp<V> && !has_wide_floor<V> && !has_wide_exp<V>);
  static_assert(!has_array_bits<V> && !has_array_from_bits<V>);
  return true;
}

template<native::isa<> A,std::size_t N> consteval bool arithmetic_float() {
  using V=native::simd<float,N,A>;
  static_assert(has_floor<V> && has_ceil<V> && has_trunc<V> && has_abs<V>);
  static_assert(has_fma<V>);
#if NATIVE_HOST_X86
  constexpr bool native_scale=A.has(native::x86_feature::avx512f) &&
    (N==1 || N==16 || A.has(native::x86_feature::avx512vl));
#else
  constexpr bool native_scale=false;
#endif
  static_assert(has_scaleb<V> == native_scale && has_masked_scaleb<V> == native_scale &&
    has_masked_scaleb_zero<V> == native_scale);
  static_assert(has_convert_int<V> && has_compress<V> && has_expand<V> && has_compress_store<V>);
  static_assert(has_exp<V> && has_flush<V> && has_math_floor<V> && has_math_exp<V> && has_math_sin<V>);
  static_assert(has_array_floor<V> && has_array_exp<V> && has_wide_floor<V> && has_wide_exp<V>);
  static_assert(has_array_bits<V> && has_array_from_bits<typename V::bits_type>);
  return true;
}

static_assert(arithmetic_float<native::scalar,1>());
#if NATIVE_HOST_X86
constexpr auto sse=native::feature_closure(native::x86_feature::sse2);
constexpr auto avx=native::feature_closure(native::x86_feature::avx);
constexpr auto f_only=native::feature_closure(native::x86_feature::avx512f);
constexpr auto partial=native::feature_closure(native::avx2 & native::x86_feature::avx512f);
static_assert(storage_only<native::simd<float,4,sse>>());
static_assert(storage_only<native::simd<float,8,avx>>());
static_assert(storage_only<native::simd<float,16,f_only>>());
static_assert(storage_only<native::simd<float,16,partial>>());
static_assert(storage_only<native::simd<std::int32_t,16,partial>>());
static_assert(storage_only<native::simd<std::uint32_t,16,partial>>());
static_assert(!has_mask_bits<native::simd<std::uint32_t,16,partial>>);
static_assert(storage_only<native::simd<std::uint8_t,8,native::avx2>>());
static_assert(storage_only<native::simd<std::uint16_t,4,native::avx2>>());
static_assert(!has_mask_bits<native::simd<std::uint16_t,4,native::avx2>>);
static_assert(storage_only<native::simd<native::fp16,4,native::avx2>>());
static_assert(storage_only<native::simd<native::fp16,8,sse>>());
static_assert(storage_only<native::simd<double,2,native::avx2>>());
static_assert(storage_only<native::simd<double,4,native::avx2>>());
static_assert(storage_only<native::simd<double,8,native::avx512>>());
static_assert(arithmetic_float<native::avx2,1>() && arithmetic_float<native::avx2,2>());
static_assert(arithmetic_float<native::avx2,3>() && arithmetic_float<native::avx2,4>());
static_assert(arithmetic_float<native::avx2,8>() && arithmetic_float<native::avx512,16>());
constexpr auto fdq=native::feature_closure(partial & native::x86_feature::avx512dq);
static_assert(arithmetic_float<fdq,16>());
#elif NATIVE_HOST_NEON
static_assert(storage_only<native::simd<std::uint8_t,8,native::neon>>());
static_assert(storage_only<native::simd<std::uint16_t,4,native::neon>>());
static_assert(!has_mask_bits<native::simd<std::uint16_t,4,native::neon>>);
static_assert(storage_only<native::simd<native::fp16,4,native::neon>>());
static_assert(storage_only<native::simd<native::fp16,8,native::neon>>());
static_assert(storage_only<native::simd<native::bf16,4,native::neon>>());
static_assert(storage_only<native::simd<native::bf16,8,native::neon>>());
static_assert(storage_only<native::simd<double,2,native::neon>>());
static_assert(arithmetic_float<native::neon,1>() && arithmetic_float<native::neon,2>());
static_assert(arithmetic_float<native::neon,3>() && arithmetic_float<native::neon,4>());
#endif

int main() {
  using V=native::simd<float,1,native::scalar>;
  auto x=V(1.75f);
  auto y=native::floor(x);
  auto packed=native::compress(typename V::mask(true),y);
  float result=0;
  native::store_simd(&result,packed.value);
  return result==1.f && packed.count==1 ? 0 : 1;
}
