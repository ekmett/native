// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <cstring>
#include <immintrin.h>
#if NATIVE_SCALEB_VL
#define NATIVE_SCALEB_TARGET "avx2,fma,avx512f,avx512dq,avx512vl"
#else
#define NATIVE_SCALEB_TARGET "avx2,fma,avx512f,avx512dq"
#endif
#if !NATIVE_SCALEB_RAW
import native.math;
constexpr auto arch=native::target_features<native::x86>(NATIVE_SCALEB_TARGET);
#endif

template<unsigned N,unsigned Mode>
[[gnu::target(NATIVE_SCALEB_TARGET),gnu::always_inline]] inline void evaluate(
    float const * prior,float const * value,float const * exponent,unsigned mask,float * output) {
#if NATIVE_SCALEB_RAW
  if constexpr(N==1) {
    __m128 result;
    if constexpr(Mode==0) result=_mm_scalef_ss(_mm_set_ss(*value),_mm_set_ss(*exponent));
    else if constexpr(Mode==1) result=_mm_mask_scalef_ss(_mm_set_ss(*prior),__mmask8(mask&1u),_mm_set_ss(*value),_mm_set_ss(*exponent));
    else result=_mm_maskz_scalef_ss(__mmask8(mask&1u),_mm_set_ss(*value),_mm_set_ss(*exponent));
    *output=_mm_cvtss_f32(result);
  } else if constexpr(N==16) {
    auto a=_mm512_loadu_ps(value),b=_mm512_loadu_ps(exponent),p=_mm512_loadu_ps(prior);
    __m512 result;
    if constexpr(Mode==0) result=_mm512_scalef_ps(a,b);
    else if constexpr(Mode==1) result=_mm512_mask_scalef_ps(p,__mmask16(mask),a,b);
    else result=_mm512_maskz_scalef_ps(__mmask16(mask),a,b);
    _mm512_storeu_ps(output,result);
  }
#if NATIVE_SCALEB_VL
  else if constexpr(N==8) {
    auto a=_mm256_loadu_ps(value),b=_mm256_loadu_ps(exponent),p=_mm256_loadu_ps(prior);
    __m256 result;
    if constexpr(Mode==0) result=_mm256_scalef_ps(a,b);
    else if constexpr(Mode==1) result=_mm256_mask_scalef_ps(p,__mmask8(mask),a,b);
    else result=_mm256_maskz_scalef_ps(__mmask8(mask),a,b);
    _mm256_storeu_ps(output,result);
  } else {
    __m128 a{},b{},p{};
    if constexpr(N==3) {
      a=_mm_maskz_loadu_ps(7,value);b=_mm_maskz_loadu_ps(7,exponent);p=_mm_maskz_loadu_ps(7,prior);
    } else {
      std::memcpy(&a,value,N*4);std::memcpy(&b,exponent,N*4);std::memcpy(&p,prior,N*4);
    }
    constexpr unsigned active=(1u<<N)-1;
    __m128 result;
    if constexpr(Mode==0) result=_mm_maskz_scalef_ps(__mmask8(active),a,b);
    else if constexpr(Mode==1) result=_mm_mask_scalef_ps(p,__mmask8(mask&active),a,b);
    else result=_mm_maskz_scalef_ps(__mmask8(mask&active),a,b);
    if constexpr(N==3) _mm_mask_storeu_ps(output,7,result);
    else std::memcpy(output,&result,N*4);
  }
#endif
#else
  using V=native::simd<float,N,arch>;
  auto a=V::load(value),b=V::load(exponent),p=V::load(prior);
  auto m=V::mask_type::from_bitset(mask);
  if constexpr(Mode==0) native::scaleb(a,b).store(output);
  else if constexpr(Mode==1) native::masked_scaleb(m,p,a,b).store(output);
  else native::masked_scaleb_zero(m,a,b).store(output);
#endif
}
#define NATIVE_SCALEB_CASE(N,M) \
  extern "C" [[gnu::target(NATIVE_SCALEB_TARGET),gnu::noinline]] void native_scale_##N##_##M( \
      float const * p,float const * a,float const * b,unsigned m,float * out) {evaluate<N,M>(p,a,b,m,out);}
#define NATIVE_SCALEB_WIDTH(N) NATIVE_SCALEB_CASE(N,0) NATIVE_SCALEB_CASE(N,1) NATIVE_SCALEB_CASE(N,2)
NATIVE_SCALEB_WIDTH(1)
NATIVE_SCALEB_WIDTH(16)
#if NATIVE_SCALEB_VL
NATIVE_SCALEB_WIDTH(2)
NATIVE_SCALEB_WIDTH(3)
NATIVE_SCALEB_WIDTH(4)
NATIVE_SCALEB_WIDTH(8)
#endif
