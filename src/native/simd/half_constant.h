// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
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
