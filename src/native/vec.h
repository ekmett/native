#pragma once
#include "native/config.h"
#include "native/simd/common.h"
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
#endif
#define NATIVE_BACKEND_BODY "native/simd/vec_family.h"
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
