// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#if NATIVE_POLYFILL_HEADERS
#include <native/simd.h>
#include <native/integer.h>
#include <native/packing.h>
#else
import native;
#endif

namespace polyfill_test {
  using namespace native;
#if defined(__aarch64__) || defined(_M_ARM64)
  constexpr auto hardware=neon;
  constexpr std::size_t native_lanes=4;
#elif defined(__wasm__)
  constexpr auto hardware=feature_closure(wasm_feature::simd128);
  constexpr std::size_t native_lanes=4;
#else
  constexpr auto hardware=avx2;
  constexpr std::size_t native_lanes=8;
#endif
  constexpr auto permitted=hardware|polyfill;
  template<class T,std::size_t N,isa<> A> concept complete=requires { sizeof(simd<T,N,A>); };
  static_assert(!complete<float,16,hardware>);
  using decomposed=simd<float,16,permitted>;
  using scalar_emulated=simd<float,16,polyfill>;
  static_assert(decomposed::register_lanes==native_lanes);
  static_assert(decomposed::register_count==16/native_lanes);
  static_assert(sizeof(decomposed)==64);
  static_assert(scalar_emulated::register_lanes==1 && scalar_emulated::register_count==16);
  static_assert(sizeof(scalar_emulated)==64);
  static_assert(std::same_as<typename simd<float,native_lanes,permitted>::native_type,
    typename simd<float,native_lanes,hardware>::native_type>);
  static_assert(!std::same_as<simd<float,native_lanes,permitted>,simd<float,native_lanes,hardware>>);

  template<class V> constexpr bool floating() {
    std::array<float,V::lanes> input{};
    for(std::size_t i=0;i<V::lanes;++i) input[i]=float(i)-3;
    auto a=V::load(input.data());
    auto b=V(2.f);
    auto c=(a+b)*b;
    std::array<float,V::lanes> output{}; c.store(output.data());
    for(std::size_t i=0;i<V::lanes;++i) if(output[i]!=(input[i]+2)*2) return false;
    auto comparison=a<b;
    auto chosen=select(comparison,a,b);
    chosen.store(output.data());
    for(std::size_t i=0;i<V::lanes;++i) if(output[i]!=(input[i]<2?input[i]:2)) return false;
    if(comparison.to_bitset()!=31) return false;
    auto fused=fma(a,b,b); fused.store(output.data());
    for(std::size_t i=0;i<V::lanes;++i) if(output[i]!=input[i]*2+2) return false;
    auto root=sqrt(V(4.f)); root.store(output.data());
    for(auto x:output) if(x!=2.f) return false;
    return true;
  }
  template<class V> constexpr bool memory() {
    using T=typename V::value_type;
    std::array<T,V::lanes+2> input{},output{};
    for(std::size_t i=0;i<input.size();++i) input[i]=T(i+1);
    for(std::size_t count=0;count<=V::lanes;++count) {
      auto value=V::load_partial(count?input.data():nullptr,count,T(99));
      output.fill(T(77)); value.store(output.data()+1);
      if(output.front()!=T(77) || output.back()!=T(77)) return false;
      for(std::size_t i=0;i<V::lanes;++i)
        if(output[i+1]!=(i<count?input[i]:T(99))) return false;
      output.fill(T(77)); value.store_partial(count?output.data()+1:nullptr,count);
      for(std::size_t i=0;i<output.size();++i)
        if(output[i]!=(i>0 && i<=count?input[i-1]:T(77))) return false;
    }
    return true;
  }
  template<class V> constexpr bool representations() {
    std::array<std::uint32_t,V::lanes> input{},output{};
    constexpr std::array patterns{0u,0x80000000u,1u,0x7f800000u,0x7fa12345u,0x7fc54321u};
    for(std::size_t i=0;i<V::lanes;++i) input[i]=patterns[i%patterns.size()];
    auto value=V::load_bits(input.data()); value.store_bits(output.data());
    if(input!=output) return false;
    auto again=V::from_native(value.to_native()); again.store_bits(output.data());
    return input==output;
  }
  template<isa<> A> constexpr bool integers() {
    using V=simd<std::uint32_t,17,A>;
    std::array<std::uint32_t,V::lanes> input{},output{};
    for(std::size_t i=0;i<V::lanes;++i) input[i]=0xfffffff0u+std::uint32_t(i);
    auto a=V::load(input.data());
    auto result=((a+V(19u))*V(3u)).template right<2>(); result.store(output.data());
    for(std::size_t i=0;i<V::lanes;++i)
      if(output[i]!=std::uint32_t(std::uint32_t(input[i]+19u)*3u)>>2) return false;
    return memory<V>();
  }
  template<isa<> A> constexpr bool masks() {
    using M=simd<mask32,17,A>;
    auto a=M::from_bitset(0x1ffff);
    auto b=M::from_bitset(0x10001);
    if(!all(a) || !any(b) || !none(a&~a)) return false;
    if((a^b).to_bitset()!=0xfffe) return false;
    using B=simd<bool,17,A>;
    if(!all(B::from_bitset(0x1ffff))) return false;
    return B::from_bitset(0xffffffffffffffffull).to_bitset()==0x1ffff;
  }
  constexpr bool normalized() {
    using B=simd<bool,17,polyfill>;
    B::native_type bytes{}; bytes.fill(255);
    auto booleans=B::from_native(bytes);
    if(booleans.to_native()[0]!=1 || !all(booleans)) return false;
    using M=simd<mask32,17,polyfill>;
    M::native_type words{}; words.fill(7);
    auto masks=M::from_native(words);
    return masks.to_native()[0]==0xffffffffu && all(masks);
  }
  static_assert(normalized());
  static_assert(floating<decomposed>() && floating<scalar_emulated>());
  static_assert(memory<simd<float,17,permitted>>() && memory<scalar_emulated>());
  static_assert(representations<decomposed>() && representations<scalar_emulated>());
  static_assert(integers<permitted>() && integers<isa<>(polyfill)>());
  static_assert(masks<permitted>() && masks<isa<>(polyfill)>());
}

int main() {
  using namespace polyfill_test;
  return !(normalized() && floating<decomposed>() && floating<scalar_emulated>() &&
    memory<simd<float,17,permitted>>() && memory<scalar_emulated>() &&
    representations<decomposed>() && representations<scalar_emulated>() &&
    integers<permitted>() && integers<isa<>(polyfill)>() &&
    masks<permitted>() && masks<isa<>(polyfill)>());
}
