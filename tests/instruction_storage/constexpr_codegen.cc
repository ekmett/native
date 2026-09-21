// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/config.h>
#include <native/attributes.h>
#include <array>
#include <cstdint>
#include <cstring>
#if NATIVE_HOST_NEON
#include <arm_neon.h>
#elif NATIVE_HOST_X86
#include <immintrin.h>
#endif
import native.simd;

#if NATIVE_HOST_NEON
constexpr auto arithmetic_arch=native::neon;
constexpr auto storage_arch=native::neon;
#define native_constexpr_target native_target("neon")
#else
constexpr auto arithmetic_arch=native::avx2;
constexpr auto storage_arch=native::feature_closure(native::x86_feature::sse2);
#define native_constexpr_target native_target("avx2,fma")
#endif
using U64=native::simd<std::uint64_t,2,arithmetic_arch>;
using U32=native::simd<std::uint32_t,4,arithmetic_arch>;
using I8=native::simd<std::int8_t,16,arithmetic_arch>;
using Small=native::simd<std::uint8_t,8,storage_arch>;
using Half=native::simd<native::fp16,4,storage_arch>;

extern "C" native_noinline float native_constexpr_scalar(float a,float b,float c,float d) {
#if NATIVE_CONSTEXPR_RAW
  return (a+b)*c/d;
#else
  using V=native::simd<float,1,native::scalar>;
  return ((V(a)+V(b))*V(c)/V(d)).to_native();
#endif
}
extern "C" native_noinline native_constexpr_target U64::native_type native_constexpr_integer_lanes(std::uint64_t a,std::uint64_t b) {
#if NATIVE_CONSTEXPR_RAW
  std::array<std::uint64_t,2> values{a,b}; U64::native_type result;
  std::memcpy(&result,values.data(),sizeof(result)); return result;
#else
  return U64(a,b).to_native();
#endif
}
extern "C" native_noinline native_constexpr_target U32::native_type native_constexpr_integer_load(std::uint32_t const * input) {
#if NATIVE_CONSTEXPR_RAW
  U32::native_type result; std::memcpy(&result,input,sizeof(result)); return result;
#else
  return U32::load(input).to_native();
#endif
}
extern "C" native_noinline native_constexpr_target I8::native_type native_constexpr_integer_broadcast(std::int8_t value) {
#if NATIVE_CONSTEXPR_RAW
#if NATIVE_HOST_NEON
  return vreinterpretq_u8_s8(vdupq_n_s8(value));
#else
  return _mm_set1_epi8(value);
#endif
#else
  return I8(value).to_native();
#endif
}
extern "C" native_noinline native_constexpr_target Small::native_type native_constexpr_narrow_load(std::uint8_t const * input) {
#if NATIVE_CONSTEXPR_RAW
  Small::native_type result{}; std::memcpy(&result,input,8); return result;
#else
  return Small::load(input).to_native();
#endif
}
extern "C" native_noinline native_constexpr_target Half::native_type native_constexpr_half_bits(std::uint16_t const * input) {
#if NATIVE_CONSTEXPR_RAW
  Half::native_type result{}; std::memcpy(&result,input,8); return result;
#else
  return Half::load_bits(input).to_native();
#endif
}
extern "C" native_noinline native_constexpr_target std::uint64_t native_constexpr_integer_extract(U64::native_type input) {
  std::array<std::uint64_t,2> values;
#if NATIVE_CONSTEXPR_RAW
  std::memcpy(values.data(),&input,sizeof(input));
#else
  U64::from_native(input).store(values.data());
#endif
  return values[1];
}
#if NATIVE_HOST_X86
using Wide=native::simd<std::uint64_t,4,native::avx2>;
using Wider=native::simd<std::uint64_t,8,native::avx512>;
extern "C" native_noinline native_target("avx2,fma") Wide::native_type native_constexpr_wide_load(std::uint64_t const * input) {
#if NATIVE_CONSTEXPR_RAW
  Wide::native_type result; std::memcpy(&result,input,sizeof(result)); return result;
#else
  return Wide::load(input).to_native();
#endif
}
extern "C" native_noinline native_target("avx2,fma,avx512f,avx512dq,avx512bw,avx512vl") Wider::native_type native_constexpr_wider_load(std::uint64_t const * input) {
#if NATIVE_CONSTEXPR_RAW
  Wider::native_type result; std::memcpy(&result,input,sizeof(result)); return result;
#else
  return Wider::load(input).to_native();
#endif
}
#endif
#undef native_constexpr_target
