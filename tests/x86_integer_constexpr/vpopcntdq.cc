// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.x86.vpopcntdq;
#include "../x86_vpopcntdq/checks.h"

namespace vpopcnt_constant_test {
  using namespace native_test;
  template<class T,std::size_t N> constexpr auto storage=native::target_features<native::x86>(
    sizeof(T)*N==16 ? "sse2" : sizeof(T)*N==32 ? "avx" : "avx512f");
  template<class T,std::size_t N> struct input {
    std::array<T,N> value{},source{};
    std::uint64_t mask{};
  };
  template<class T,std::size_t N> constexpr auto make_case(property_rng& random) {
    input<T,N> result{};
    for(auto& lane:result.value) lane=random.bits<T>();
    for(auto& lane:result.source) lane=random.bits<T>();
    result.mask=random.next(); return result;
  }
  template<native::isa<native::x86> A,class T,std::size_t N>
  consteval auto calculate(input<T,N> in) {
    using V=native::simd<T,N,A>;
    auto value=V::load(in.value.data()),source=V::load(in.source.data());
    auto mask=native::predicate<N,A>::from_bitset(in.mask);
    std::array<std::array<T,N>,3> result{};
    if constexpr(sizeof(T)==4) {
      native::vpopcntd<A>(value).store(result[0].data());
      native::mask_vpopcntd<A>(source,mask,value).store(result[1].data());
      native::maskz_vpopcntd<A>(mask,value).store(result[2].data());
    } else {
      native::vpopcntq<A>(value).store(result[0].data());
      native::mask_vpopcntq<A>(source,mask,value).store(result[1].data());
      native::maskz_vpopcntq<A>(mask,value).store(result[2].data());
    }
    return result;
  }
  template<class T,std::size_t N> constexpr auto oracle(input<T,N> in) {
    std::array<std::array<T,N>,3> result{};
    for(std::size_t lane=0;lane<N;++lane) {
      // Clear the lowest set bit until none remain, independently of popcount.
      T count=0;
      for(T value=in.value[lane];value;value&=value-1) ++count;
      result[0][lane]=count;
      result[1][lane]=((in.mask>>lane)&1) ? count : in.source[lane];
      result[2][lane]=((in.mask>>lane)&1) ? count : 0;
    }
    return result;
  }
  template<native::isa<native::x86> A,class T,std::size_t N>
  consteval bool check_constants() {
    property_rng random{property_seed};
    for(unsigned bit=0;bit<sizeof(T)*8;++bit) {
      auto in=make_case<T,N>(random);
      for(std::size_t lane=0;lane<N;++lane) in.value[lane]=T{1}<<((bit+lane)%(sizeof(T)*8));
      if(calculate<A>(in)!=oracle(in)) return false;
      for(auto& value:in.value) value=~value;
      if(calculate<A>(in)!=oracle(in)) return false;
    }
    for(unsigned bit=0;bit<64;++bit) {
      auto in=make_case<T,N>(random); in.mask=std::uint64_t{1}<<bit;
      if(calculate<A>(in)!=oracle(in)) return false;
      in.mask=~in.mask;
      if(calculate<A>(in)!=oracle(in)) return false;
    }
    return true;
  }
#define CHECK(T,N) \
  static_assert(check_constants<storage<T,N>,T,N>()); \
  static_assert(check_constants<exact_vl,T,N>())
  CHECK(std::uint32_t,4); CHECK(std::uint32_t,8); CHECK(std::uint32_t,16);
  CHECK(std::uint64_t,2); CHECK(std::uint64_t,4); CHECK(std::uint64_t,8);
#undef CHECK
  static_assert(check_constants<exact_512,std::uint32_t,16>());
  static_assert(check_constants<exact_512,std::uint64_t,8>());

  template<class T,std::size_t N> bool compare() {
    constexpr auto cases=property_cases<16>(property_seed,make_case<T,N>);
    constexpr auto expected=[cases] consteval {
      std::array<std::array<std::array<T,N>,3>,16> result{};
      for(unsigned i=0;i<16;++i) result[i]=calculate<storage<T,N>>(cases[i]);
      return result;
    }();
    auto one=[](input<T,N> const& in,auto const& wanted,std::uint64_t seed,std::size_t index) {
      std::array<std::array<T,N>,3> actual{};
      if constexpr(sizeof(T)*N==64) invoke_512(actual[0].data(),actual[1].data(),actual[2].data(),in.source.data(),in.value.data(),unsigned(in.mask));
      else if constexpr(sizeof(T)*N==32) invoke_256(actual[0].data(),actual[1].data(),actual[2].data(),in.source.data(),in.value.data(),unsigned(in.mask));
      else invoke_128(actual[0].data(),actual[1].data(),actual[2].data(),in.source.data(),in.value.data(),unsigned(in.mask));
      return property_equal("VPOPCNT lanes/masks",seed,index,wanted,actual,in.value,in.source,in.mask);
    };
    for(unsigned i=0;i<16;++i) if(!one(cases[i],expected[i],property_seed,i)) return false;
    auto settings=property_config(); property_rng random{settings.seed};
    for(std::size_t i=0;i<settings.cases;++i) {
      auto in=make_case<T,N>(random);
      if(!one(in,oracle(in),settings.seed,i)) return false;
    }
    return true;
  }
}
int main() {
  using namespace vpopcnt_constant_test;
  auto cpu=native::observe_x86_capabilities();
  auto base=native::classify_isa(cpu,requirements_512);
  if(base.invalid_features) return 1;
  if(!base.admitted()) { std::printf("SKIP VPOPCNT: %s\n",base.reason()); return 77; }
  if(!compare<std::uint32_t,16>() || !compare<std::uint64_t,8>()) return 1;
  if(native::classify_isa(cpu,requirements_vl).admitted() &&
      (!compare<std::uint32_t,4>() || !compare<std::uint32_t,8>() ||
       !compare<std::uint64_t,2>() || !compare<std::uint64_t,4>())) return 1;
  return 0;
}
