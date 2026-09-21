// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/config.h>
#include <native/attributes.h>
#include <cmath>
#if NATIVE_HOST_NEON
#include <arm_neon.h>
#elif NATIVE_HOST_X86
#include <immintrin.h>
#endif
import native.simd;
using namespace native;

#define PAIR(name,attribute,T,V,public_expression,raw_expression) \
extern "C" attribute native_noinline T native_float_##name(T a,T b,T c) { \
  auto x=V::from_native(a),y=V::from_native(b),z=V::from_native(c); \
  return (public_expression).to_native(); \
} \
extern "C" attribute native_noinline T native_float_##name##_raw(T a,T b,T c) {return raw_expression;}
using scalar_float=simd<float,1,scalar>;
PAIR(scalar_add,,float,scalar_float,x+y,a+b)
PAIR(scalar_sub,,float,scalar_float,x-y,a-b)
PAIR(scalar_mul,,float,scalar_float,x*y,a*b)
PAIR(scalar_div,,float,scalar_float,x/y,a/b)
PAIR(scalar_fma,,float,scalar_float,fma(x,y,z),std::fma(a,b,c))
PAIR(scalar_sqrt,,float,scalar_float,sqrt(x),std::sqrt(a))
PAIR(scalar_floor,,float,scalar_float,floor(x),std::floor(a))
PAIR(scalar_ceil,,float,scalar_float,ceil(x),std::ceil(a))
PAIR(scalar_neg,,float,scalar_float,-x,-a)
#if NATIVE_HOST_NEON
using native_float4=simd<float,4,neon>;
#define FULL(name,expression,raw) PAIR(arm4_##name,native_target("neon"),float32x4_t,native_float4,expression,raw)
FULL(add,x+y,vaddq_f32(a,b))
FULL(sub,x-y,vsubq_f32(a,b))
FULL(mul,x*y,vmulq_f32(a,b))
FULL(div,x/y,vdivq_f32(a,b))
FULL(fma,fma(x,y,z),vfmaq_f32(c,a,b))
FULL(sqrt,sqrt(x),vsqrtq_f32(a))
FULL(floor,floor(x),vrndmq_f32(a))
FULL(ceil,ceil(x),vrndpq_f32(a))
FULL(round,round_even(x),vrndnq_f32(a))
#undef FULL
#define SHORT_ARCH neon
#define SHORT_TARGET native_target("neon")
#define SHORT_FMA(a,b,c) __builtin_bit_cast(short_native,vfmaq_f32(__builtin_bit_cast(float32x4_t,c),__builtin_bit_cast(float32x4_t,a),__builtin_bit_cast(float32x4_t,b)))
#elif NATIVE_HOST_X86
using native_float4=simd<float,4,avx2>;
using native_float8=simd<float,8,avx2>;
using native_float16=simd<float,16,avx512>;
#define FULL(name,expression,raw) PAIR(x86_4_##name,native_target("avx2,fma"),__m128,native_float4,expression,raw)
FULL(add,x+y,_mm_add_ps(a,b))
FULL(sub,x-y,_mm_sub_ps(a,b))
FULL(mul,x*y,_mm_mul_ps(a,b))
FULL(div,x/y,_mm_div_ps(a,b))
FULL(fma,fma(x,y,z),_mm_fmadd_ps(a,b,c))
FULL(sqrt,sqrt(x),_mm_sqrt_ps(a))
FULL(floor,floor(x),_mm_round_ps(a,_MM_FROUND_TO_NEG_INF|_MM_FROUND_NO_EXC))
FULL(ceil,ceil(x),_mm_round_ps(a,_MM_FROUND_TO_POS_INF|_MM_FROUND_NO_EXC))
FULL(round,round_even(x),_mm_round_ps(a,_MM_FROUND_TO_NEAREST_INT|_MM_FROUND_NO_EXC))
#undef FULL
#define FULL(name,expression,raw) PAIR(x86_8_##name,native_target("avx2,fma"),__m256,native_float8,expression,raw)
FULL(add,x+y,_mm256_add_ps(a,b))
FULL(sub,x-y,_mm256_sub_ps(a,b))
FULL(mul,x*y,_mm256_mul_ps(a,b))
FULL(div,x/y,_mm256_div_ps(a,b))
FULL(fma,fma(x,y,z),_mm256_fmadd_ps(a,b,c))
FULL(sqrt,sqrt(x),_mm256_sqrt_ps(a))
FULL(floor,floor(x),_mm256_round_ps(a,_MM_FROUND_TO_NEG_INF|_MM_FROUND_NO_EXC))
FULL(ceil,ceil(x),_mm256_round_ps(a,_MM_FROUND_TO_POS_INF|_MM_FROUND_NO_EXC))
FULL(round,round_even(x),_mm256_round_ps(a,_MM_FROUND_TO_NEAREST_INT|_MM_FROUND_NO_EXC))
#undef FULL
#define FULL(name,expression,raw) PAIR(x86_16_##name,native_target("avx512f,avx512dq,avx512bw,avx512vl,avx2,fma"),__m512,native_float16,expression,raw)
FULL(add,x+y,_mm512_add_ps(a,b))
FULL(sub,x-y,_mm512_sub_ps(a,b))
FULL(mul,x*y,_mm512_mul_ps(a,b))
FULL(div,x/y,_mm512_div_ps(a,b))
FULL(fma,fma(x,y,z),_mm512_fmadd_ps(a,b,c))
FULL(sqrt,sqrt(x),_mm512_sqrt_ps(a))
FULL(floor,floor(x),_mm512_roundscale_ps(a,_MM_FROUND_TO_NEG_INF|_MM_FROUND_NO_EXC))
FULL(ceil,ceil(x),_mm512_roundscale_ps(a,_MM_FROUND_TO_POS_INF|_MM_FROUND_NO_EXC))
FULL(round,round_even(x),_mm512_roundscale_ps(a,_MM_FROUND_TO_NEAREST_INT|_MM_FROUND_NO_EXC))
#undef FULL
#define SHORT_ARCH avx2
#define SHORT_TARGET native_target("avx2,fma")
#define SHORT_FMA(a,b,c) __builtin_bit_cast(short_native,_mm_fmadd_ps(__builtin_bit_cast(__m128,a),__builtin_bit_cast(__m128,b),__builtin_bit_cast(__m128,c)))
#endif
#if NATIVE_HOST_X86 || NATIVE_HOST_NEON
using short_native=float __attribute__((ext_vector_type(4)));
// Both sides implement the documented short-vector padding contract explicitly.
#define SHORT(N) \
using short##N=simd<float,N,SHORT_ARCH>; \
PAIR(short##N##_add,SHORT_TARGET,short_native,short##N,x+y, \
  (__builtin_shufflevector(a,short_native{},0,1,N==3?2:4,4)+__builtin_shufflevector(b,short_native{},0,1,N==3?2:4,4))) \
PAIR(short##N##_div,SHORT_TARGET,short_native,short##N,x/y, \
  (__builtin_shufflevector(a,short_native{},0,1,N==3?2:4,4)/__builtin_shufflevector(b,short_native{1.f,1.f,1.f,1.f},0,1,N==3?2:4,4))) \
PAIR(short##N##_fma,SHORT_TARGET,short_native,short##N,fma(x,y,z), \
  ([&]() SHORT_TARGET { auto x=__builtin_shufflevector(a,short_native{},0,1,N==3?2:4,4); \
          auto y=__builtin_shufflevector(b,short_native{},0,1,N==3?2:4,4); \
          auto z=__builtin_shufflevector(c,short_native{},0,1,N==3?2:4,4); \
          return SHORT_FMA(x,y,z); }()))
SHORT(2)
SHORT(3)
#undef SHORT
#endif
#undef PAIR
