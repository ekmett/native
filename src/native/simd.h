#pragma once
#include "native/config.h"
#include "native/simd/common.h"
#include "native/targets.h"
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "native/detail/constexpr_float.h"
#include <tuple>

namespace native::detail::float_constant {
  namespace cf=constexpr_float;
  using f32=cf::binary32;
  constexpr std::uint32_t add(std::uint32_t a,std::uint32_t b) noexcept {return cf::add_bits<f32>(a,b);}
  constexpr std::uint32_t subtract(std::uint32_t a,std::uint32_t b) noexcept {return cf::sub_bits<f32>(a,b);}
  constexpr std::uint32_t multiply(std::uint32_t a,std::uint32_t b) noexcept {return cf::mul_bits<f32>(a,b);}
  constexpr std::uint32_t divide(std::uint32_t a,std::uint32_t b) noexcept {return cf::div_bits<f32>(a,b);}
  constexpr std::uint32_t negate(std::uint32_t a) noexcept {return a^f32::sign_mask;}
  constexpr std::uint32_t multiply_add(std::uint32_t a,std::uint32_t b,std::uint32_t c) noexcept {return cf::fma_bits<f32>(a,b,c);}
  constexpr std::uint32_t square_root(std::uint32_t a) noexcept {return cf::sqrt_bits<f32>(a);}
  constexpr std::uint32_t nearest(std::uint32_t a) noexcept {return cf::round_integral_bits<f32>(a);}
  constexpr std::uint32_t floor(std::uint32_t a) noexcept {return cf::round_integral_bits<f32>(a,cf::rounding::downward);}
  constexpr std::uint32_t ceil(std::uint32_t a) noexcept {return cf::round_integral_bits<f32>(a,cf::rounding::upward);}
  constexpr std::uint32_t trunc(std::uint32_t a) noexcept {return cf::round_integral_bits<f32>(a,cf::rounding::toward_zero);}
  constexpr std::int32_t fcvtzs(std::uint32_t bits) noexcept {
    auto const magnitude = bits & 0x7fffffffu;
    if (magnitude > 0x7f800000u) return 0;
    if (magnitude >= 0x4f000000u)
      return (bits >> 31) ? INT32_MIN : INT32_MAX;
    if (magnitude < 0x3f800000u) return 0;
    auto const exponent = int(magnitude >> 23) - 127;
    auto const significand = (magnitude & 0x007fffffu) | 0x00800000u;
    auto const value = std::int32_t(exponent < 23
      ? significand >> (23 - exponent) : significand << (exponent - 23));
    return (bits >> 31) ? -value : value;
  }
  constexpr std::uint32_t fcvtzu(std::uint32_t bits) noexcept {
    auto const magnitude = bits & 0x7fffffffu;
    if ((bits >> 31) || magnitude > 0x7f800000u || magnitude < 0x3f800000u) return 0;
    if (magnitude >= 0x4f800000u) return UINT32_MAX;
    auto const exponent = int(magnitude >> 23) - 127;
    auto const significand = (magnitude & 0x007fffffu) | 0x00800000u;
    return exponent < 23 ? significand >> (23 - exponent) : significand << (exponent - 23);
  }
  constexpr std::uint32_t power_of_two(std::uint32_t a) noexcept {return std::uint32_t(int(std::bit_cast<float>(a))+127)<<23;}
  constexpr bool less(std::uint32_t a,std::uint32_t b) noexcept {return cf::less_bits<f32>(a,b);}
  constexpr bool equal(std::uint32_t a,std::uint32_t b) noexcept {return cf::equal_bits<f32>(a,b);}
  constexpr std::uint32_t scale(std::uint32_t x,std::uint32_t exponent) noexcept {
    if(cf::is_signaling_nan<f32>(x)) return cf::quiet_nan<f32>(x);
    if(cf::is_nan<f32>(exponent)) return cf::select_nan<f32>(std::array{x,exponent},{});
    if(cf::is_infinite<f32>(exponent)) {
      bool down=(exponent&f32::sign_mask)!=0;
      if(cf::is_nan<f32>(x)) return down?0u:f32::exponent_mask;
      if(cf::is_infinite<f32>(x)) return down?cf::default_nan<f32>({}):x;
      if(cf::is_zero<f32>(x)) return down?x:cf::default_nan<f32>({});
      return (x&f32::sign_mask)|(down?0u:f32::exponent_mask);
    }
    if(cf::is_nan<f32>(x)) return cf::quiet_nan<f32>(x);
    if(cf::is_infinite<f32>(x) || cf::is_zero<f32>(x)) return x;
    float n=std::bit_cast<float>(floor(exponent));
    int shift=n < -512.f?-512:n > 512.f?512:static_cast<int>(n);
    auto a=cf::unpack<f32>(x);
    return cf::round_pack<f32>(a.sign,cf::magnitude<1>{{a.significand}},a.exponent+shift,cf::rounding::nearest_even,{});
  }
  template<class V> constexpr auto words(V value) noexcept {
    std::array<std::uint32_t,V::lanes> words{};
    value.store_bits(words.data());
    return words;
  }
  template<class F,class V,class... W>
  constexpr V map(F operation,V value,W... rest) noexcept {
    auto inputs=std::tuple{words(value),words(rest)...};
    std::array<std::uint32_t,V::lanes> result{};
    for(std::size_t i=0;i<V::lanes;++i)
      result[i]=std::apply([&](auto const&... input) {return operation(input[i]...);},inputs);
    return V::load_bits(result.data());
  }
  template<class F,class V>
  constexpr auto compare(F operation,V a,V b) noexcept {
    auto x=words(a),y=words(b);
    std::uint64_t bits=0;
    for(std::size_t i=0;i<V::lanes;++i) bits|=std::uint64_t(operation(x[i],y[i]))<<i;
    return V::mask_type::from_bitset(bits);
  }
  template<class M,class V>
  constexpr V select(M mask,V a,V b) noexcept {
    auto x=words(a),y=words(b);
    auto bits=mask.to_bitset();
    for(std::size_t i=0;i<V::lanes;++i) if(!((bits>>i)&1)) x[i]=y[i];
    return V::load_bits(x.data());
  }
}
#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <cmath>
#include <cstring>
#include <span>
#include <limits>
#if NATIVE_HOST_WASM
#include <wasm_simd128.h>
#endif
#if NATIVE_HOST_X86
#include <immintrin.h>
#endif
#if NATIVE_HOST_NEON
#include <arm_neon.h>
#include "native/arm/detail/register_order.h"
#endif
#define NATIVE_BACKEND_BODY "native/simd/simd_family.h"
#include "native/simd/for_each_backend.h"
#undef NATIVE_BACKEND_BODY

#if NATIVE_HOST_X86 && (!defined(NATIVE_PROFILE) || NATIVE_PROFILE != 0)
#define NATIVE_COMMON_ARCH(...) (::native::abi_lookup<__VA_ARGS__,::native::detail::memory_kernel_policies>::index == 1)
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_9))), apply_to=function)
#include "native/simd/common_body.h"
#pragma clang attribute pop
#undef NATIVE_COMMON_ARCH
#define NATIVE_COMMON_ARCH(...) (::native::abi_lookup<__VA_ARGS__,::native::detail::memory_kernel_policies>::index == 2)
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_13))), apply_to=function)
#include "native/simd/common_body.h"
#pragma clang attribute pop
#undef NATIVE_COMMON_ARCH
#define NATIVE_COMMON_ARCH(...) (::native::abi_lookup<__VA_ARGS__,::native::detail::memory_kernel_policies>::index == 0)
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_17))), apply_to=function)
#include "native/simd/common_body.h"
#pragma clang attribute pop
#undef NATIVE_COMMON_ARCH
#endif

#if NATIVE_HOST_NEON && (!defined(NATIVE_PROFILE) || NATIVE_PROFILE != 0)
#define NATIVE_COMMON_ARCH(...) (::native::abi_lookup<__VA_ARGS__,::native::detail::memory_kernel_policies>::index == 1)
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_21))), apply_to=function)
#include "native/simd/common_body.h"
#pragma clang attribute pop
#undef NATIVE_COMMON_ARCH
#define NATIVE_COMMON_ARCH(...) (::native::abi_lookup<__VA_ARGS__,::native::detail::memory_kernel_policies>::index == 2)
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_22))), apply_to=function)
#include "native/simd/common_body.h"
#pragma clang attribute pop
#undef NATIVE_COMMON_ARCH
#define NATIVE_COMMON_ARCH(...) (::native::abi_lookup<__VA_ARGS__,::native::detail::memory_kernel_policies>::index == 0)
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_23))), apply_to=function)
#include "native/simd/common_body.h"
#pragma clang attribute pop
#undef NATIVE_COMMON_ARCH
#endif

// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "native/detail/constexpr_float.h"
#include "native/arm/bf16_constexpr.h"

namespace native::detail::half_constant {
  namespace fp=constexpr_float;
  using format=fp::binary16;

  template<bool Arm> constexpr fp::policy arithmetic_policy() noexcept {
    fp::policy p;
    if constexpr(!Arm) {
      p.nan=fp::nan_propagation::first;
      p.fma_order=fp::fma_nan_order::multiplicands_first;
      p.default_nan_negative=true;
      p.invalid_product_overrides_quiet_addend=false;
    }
    return p;
  }

  template<class V,class F> constexpr V unary(V a,F operation) noexcept {
    std::array<std::uint16_t,V::lanes> words{}; a.store_bits(words.data());
    for(auto & word:words) word=operation(word);
    return V::load_bits(words.data());
  }
  template<class V,class F> constexpr V binary(V a,V b,F operation) noexcept {
    std::array<std::uint16_t,V::lanes> left{},right{};
    a.store_bits(left.data()); b.store_bits(right.data());
    for(std::size_t i=0;i<V::lanes;++i) left[i]=operation(left[i],right[i]);
    return V::load_bits(left.data());
  }
  enum class operation { add, subtract, multiply, divide };
  template<operation Op,bool Arm,class V> constexpr V arithmetic(V a,V b) noexcept {
    return binary(a,b,[](auto x,auto y) {
      constexpr auto mode=fp::rounding::nearest_even;
      constexpr auto policy=arithmetic_policy<Arm>();
      if constexpr(Op==operation::add) return fp::add_bits<format>(x,y,mode,policy);
      else if constexpr(Op==operation::subtract) return fp::sub_bits<format>(x,y,mode,policy);
      else if constexpr(Op==operation::multiply) return fp::mul_bits<format>(x,y,mode,policy);
      else return fp::div_bits<format>(x,y,mode,policy);
    });
  }
  template<bool Arm,class V> constexpr V square_root(V a) noexcept {
    return unary(a,[](auto x) {
      return fp::sqrt_bits<format>(x,fp::rounding::nearest_even,arithmetic_policy<Arm>());
    });
  }
  template<bool Arm,class V> constexpr V fused(V a,V b,V c) noexcept {
    std::array<std::uint16_t,V::lanes> left{},right{},result{};
    a.store_bits(left.data()); b.store_bits(right.data()); c.store_bits(result.data());
    for(std::size_t i=0;i<V::lanes;++i)
      result[i]=fp::fma_bits<format>(left[i],right[i],result[i],
        fp::rounding::nearest_even,arithmetic_policy<Arm>());
    return V::load_bits(result.data());
  }
  template<class V,class F> constexpr typename V::mask compare(V a,V b,F operation) noexcept {
    std::array<std::uint16_t,V::lanes> left{},right{};
    a.store_bits(left.data()); b.store_bits(right.data());
    std::uint64_t result=0;
    for(std::size_t i=0;i<V::lanes;++i) result|=std::uint64_t(operation(left[i],right[i]))<<i;
    return V::mask::from_bitset(result);
  }
  template<class V> constexpr V select(typename V::mask mask,V a,V b) noexcept {
    std::array<std::uint16_t,V::lanes> left{},right{};
    a.store_bits(left.data()); b.store_bits(right.data());
    auto active=mask.to_bitset();
    for(std::size_t i=0;i<V::lanes;++i) if(!((active>>i)&1)) left[i]=right[i];
    return V::load_bits(left.data());
  }

  template<bool Arm,class H,class V> constexpr V dot2(H a,H b,V accumulator) noexcept {
    std::array<std::uint16_t,H::lanes> left{},right{};
    std::array<std::uint32_t,V::lanes> result{};
    a.store_bits(left.data()); b.store_bits(right.data()); accumulator.store_bits(result.data());
    fp::policy p;
    p.nan=fp::nan_propagation::first;
    p.fma_order=fp::fma_nan_order::multiplicands_first;
    p.flush_inputs=p.flush_outputs=true;
    p.default_nan_negative=true;
    p.invalid_product_overrides_quiet_addend=false;
    for(std::size_t i=0;i<V::lanes;++i) {
      if constexpr(Arm) result[i]=arm_bfdot_bits(result[i],left[2*i],left[2*i+1],right[2*i],right[2*i+1]);
      else {
        // The instruction evaluates the high product first. The low product's
        // NaNs therefore take precedence over both the high pair and addend.
        auto high=fp::fma_bits<fp::binary32>(std::uint32_t(left[2*i+1])<<16,
          std::uint32_t(right[2*i+1])<<16,result[i],fp::rounding::nearest_even,p);
        result[i]=fp::fma_bits<fp::binary32>(std::uint32_t(left[2*i])<<16,
          std::uint32_t(right[2*i])<<16,high,fp::rounding::nearest_even,p);
      }
    }
    return V::load_bits(result.data());
  }
}

#if NATIVE_HOST_X86
// Intrinsic calls are owned by the global module fragment.
#pragma clang attribute push(__attribute__((target("avx2,fma,avx512f,avx512dq,avx512bw,avx512vl,avx512bf16"))), apply_to=function)
namespace native::detail::avx512_bf16_backend {
  native_inline __m128 dot2_native(__m128bh a, __m128bh b, __m128 accumulator) noexcept {
    return _mm_dpbf16_ps(accumulator, a, b);
  }
  native_inline __m256 dot2_native(__m256bh a, __m256bh b, __m256 accumulator) noexcept {
    return _mm256_dpbf16_ps(accumulator, a, b);
  }
  native_inline __m512 dot2_native(__m512bh a, __m512bh b, __m512 accumulator) noexcept {
    return _mm512_dpbf16_ps(accumulator, a, b);
  }
}
#pragma clang attribute pop
// Intrinsic calls are owned by the global module fragment.
#pragma clang attribute push(__attribute__((target("avx2,fma,avx512f,avx512dq,avx512bw,avx512vl,avx512fp16"))), apply_to=function)
namespace native::detail::avx512_fp16_backend {
  // Explicit native builtins avoid TU-level excess-precision widening when the
  // provider is compiled below AVX512-FP16. MXCSR still supplies rounding.
  native_inline __m512h add_half(__m512h a, __m512h b) noexcept { return _mm512_add_round_ph(a,b,_MM_FROUND_CUR_DIRECTION); }
  native_inline __m512h sub_half(__m512h a, __m512h b) noexcept { return _mm512_sub_round_ph(a,b,_MM_FROUND_CUR_DIRECTION); }
  native_inline __m512h mul_half(__m512h a, __m512h b) noexcept { return _mm512_mul_round_ph(a,b,_MM_FROUND_CUR_DIRECTION); }
  native_inline __m512h div_half(__m512h a, __m512h b) noexcept { return _mm512_div_round_ph(a,b,_MM_FROUND_CUR_DIRECTION); }
  native_inline __m512h sqrt_half(__m512h a) noexcept { return _mm512_sqrt_ph(a); }
  native_inline __m512h neg_half(__m512h a) noexcept {
    return _mm512_castsi512_ph(_mm512_xor_si512(_mm512_castph_si512(a),_mm512_set1_epi16(short(0x8000))));
  }
  native_inline __m512h fma_half(__m512h a, __m512h b, __m512h c) noexcept { return _mm512_fmadd_ph(a,b,c); }
  native_inline __mmask32 eq_half(__m512h a, __m512h b) noexcept { return _mm512_cmp_ph_mask(a,b,_CMP_EQ_OQ); }
  native_inline __mmask32 lt_half(__m512h a, __m512h b) noexcept { return _mm512_cmp_ph_mask(a,b,_CMP_LT_OQ); }
  native_inline __mmask32 le_half(__m512h a, __m512h b) noexcept { return _mm512_cmp_ph_mask(a,b,_CMP_LE_OQ); }
  native_inline __m512h select_half(__mmask32 m, __m512h a, __m512h b) noexcept {
    return _mm512_mask_blend_ph(m,b,a);
  }
}
#pragma clang attribute pop
#endif

#if NATIVE_HOST_NEON
// Native calls are owned by the global module fragment.
#include "native/arm/bf16.h"
#pragma clang attribute push(__attribute__((target("neon,bf16"))), apply_to=function)
namespace native::detail::neon_bf16_backend {
  native_inline float32x4_t dot2_native(bfloat16x8_t a, bfloat16x8_t b, float32x4_t accumulator) noexcept {
    // Share the instruction wrapper's FPCR-sensitive evaluation contract.
    return native::detail::arm_bf16::bfdot<native::isa<>(native::arm_feature::neon_bf16)>(accumulator, a, b);
  }
}
#pragma clang attribute pop
// Intrinsic calls are owned by the global module fragment.
#pragma clang attribute push(__attribute__((target("neon,fullfp16"))), apply_to=function)
namespace native::detail::neon_fp16_backend {
  native_inline float16x8_t add_half(float16x8_t a, float16x8_t b) noexcept { return vaddq_f16(a,b); }
  native_inline float16x8_t sub_half(float16x8_t a, float16x8_t b) noexcept { return vsubq_f16(a,b); }
  native_inline float16x8_t mul_half(float16x8_t a, float16x8_t b) noexcept { return vmulq_f16(a,b); }
  native_inline float16x8_t div_half(float16x8_t a, float16x8_t b) noexcept { return vdivq_f16(a,b); }
  native_inline float16x8_t sqrt_half(float16x8_t a) noexcept { return vsqrtq_f16(a); }
  native_inline float16x8_t neg_half(float16x8_t a) noexcept { return vnegq_f16(a); }
  native_inline float16x8_t fma_half(float16x8_t a, float16x8_t b, float16x8_t c) noexcept { return vfmaq_f16(c,a,b); }
  // Keep comparisons vectorized under strict FP flags and retain native status effects.
  native_inline uint8x16_t eq_half(float16x8_t a, float16x8_t b) noexcept {
    a=arm_register_order(a);
    b=arm_register_order(b);
    uint16x8_t bits;
    asm volatile("fcmeq %0.8h, %1.8h, %2.8h" : "=w"(bits) : "w"(a), "w"(b) : "memory");
    return vreinterpretq_u8_u16(arm_register_order(bits));
  }
  native_inline uint8x16_t lt_half(float16x8_t a, float16x8_t b) noexcept {
    a=arm_register_order(a);
    b=arm_register_order(b);
    uint16x8_t bits;
    asm volatile("fcmgt %0.8h, %1.8h, %2.8h" : "=w"(bits) : "w"(b), "w"(a) : "memory");
    return vreinterpretq_u8_u16(arm_register_order(bits));
  }
  native_inline uint8x16_t le_half(float16x8_t a, float16x8_t b) noexcept {
    a=arm_register_order(a);
    b=arm_register_order(b);
    uint16x8_t bits;
    asm volatile("fcmge %0.8h, %1.8h, %2.8h" : "=w"(bits) : "w"(b), "w"(a) : "memory");
    return vreinterpretq_u8_u16(arm_register_order(bits));
  }
  native_inline float16x8_t select_half(uint8x16_t m, float16x8_t a, float16x8_t b) noexcept {
    return vbslq_f16(vreinterpretq_u16_u8(m),a,b);
  }
}
#pragma clang attribute pop
#endif
