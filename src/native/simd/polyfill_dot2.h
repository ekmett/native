// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#if NATIVE_HOST_NEON
#pragma clang attribute push(__attribute__((target("neon,bf16"))), apply_to=function)
namespace native::detail {
  // Reconstitute native register groups from exact logical representations.
  // The last native group has zeroed inactive inputs and is trimmed on return.
  template<class H,class V>
  hint_inline constexpr V polyfill_dot2_native(H a,H b,V accumulator) noexcept {
    if consteval { return half_constant::dot2<true>(a,b,accumulator); } else {
      std::array<std::uint16_t,H::lanes> left{},right{};
      std::array<std::uint32_t,V::lanes> result{};
      a.store_bits(left.data()); b.store_bits(right.data()); accumulator.store_bits(result.data());
      for(std::size_t begin=0;begin<V::lanes;begin+=4) {
        auto const count=std::min(std::size_t{4},V::lanes-begin);
        std::array<std::uint16_t,8> x{},y{}; std::array<std::uint32_t,4> z{};
        for(std::size_t i=0;i<count;++i) {
          x[2*i]=left[2*(begin+i)]; x[2*i+1]=left[2*(begin+i)+1];
          y[2*i]=right[2*(begin+i)]; y[2*i+1]=right[2*(begin+i)+1]; z[i]=result[begin+i];
        }
        z=__builtin_bit_cast(decltype(z),neon_bf16_backend::dot2_native(
          __builtin_bit_cast(bfloat16x8_t,x),__builtin_bit_cast(bfloat16x8_t,y),__builtin_bit_cast(float32x4_t,z)));
        for(std::size_t i=0;i<count;++i) result[begin+i]=z[i];
      }
      return V::load_bits(result.data());
    }
  }
}
#pragma clang attribute pop
#elif NATIVE_HOST_X86
// The 512-bit opcode does not require AVX512VL or AVX512DQ. Tags carrying only
// BF16 and its actual prerequisites still use this available native operation.
#pragma clang attribute push(__attribute__((target("avx512bf16"))), apply_to=function)
namespace native::detail {
  template<class H,class V>
  hint_inline constexpr V polyfill_dot2_native(H a,H b,V accumulator) noexcept {
    if consteval { return half_constant::dot2<false>(a,b,accumulator); } else {
      std::array<std::uint16_t,H::lanes> left{},right{};
      std::array<std::uint32_t,V::lanes> result{};
      a.store_bits(left.data()); b.store_bits(right.data()); accumulator.store_bits(result.data());
      for(std::size_t begin=0;begin<V::lanes;begin+=16) {
        auto const count=std::min(std::size_t{16},V::lanes-begin);
        std::array<std::uint16_t,32> x{},y{}; std::array<std::uint32_t,16> z{};
        for(std::size_t i=0;i<count;++i) {
          x[2*i]=left[2*(begin+i)]; x[2*i+1]=left[2*(begin+i)+1];
          y[2*i]=right[2*(begin+i)]; y[2*i+1]=right[2*(begin+i)+1]; z[i]=result[begin+i];
        }
        z=__builtin_bit_cast(decltype(z),_mm512_dpbf16_ps(__builtin_bit_cast(__m512,z),
          __builtin_bit_cast(__m512bh,x),__builtin_bit_cast(__m512bh,y)));
        for(std::size_t i=0;i<count;++i) result[begin+i]=z[i];
      }
      return V::load_bits(result.data());
    }
  }
}
#pragma clang attribute pop
#endif
