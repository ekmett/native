// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "common.h"
import native.arm.fp16fml;
namespace {
  constexpr auto strong=native::feature_closure(native::arm_feature::fp16fml);
  constexpr auto weak=native::neon;
  struct input {
    std::array<std::uint32_t,4> acc;
    std::array<std::uint16_t,8> a,b;
  };
  constexpr input generate(native_test::property_rng & rng) {
    input x{};
    for(auto & v:x.acc) v=floating_fixture::special_bits<std::uint32_t>(rng);
    for(auto & v:x.a) v=floating_fixture::special_bits<std::uint16_t>(rng);
    for(auto & v:x.b) v=floating_fixture::special_bits<std::uint16_t>(rng);
    return x;
  }
  template<native::isa<native::arm> A,unsigned Op,std::size_t N,std::size_t M,int Lane>
  constexpr __attribute__((target("fp16fml"))) auto apply(input const & x) {
    auto c=native::simd<float,N,A>::load_bits(x.acc.data());
    auto a=native::simd<native::fp16,2*N,A>::load_bits(x.a.data());
    auto b=native::simd<native::fp16,M,A>::load_bits(x.b.data());
    auto value=[&]() __attribute__((target("fp16fml"))) {
      if constexpr(Lane<0) {
        if constexpr(Op==0) return native::fmlal<A>(c,a,b);
        else if constexpr(Op==1) return native::fmlal2<A>(c,a,b);
        else if constexpr(Op==2) return native::fmlsl<A>(c,a,b);
        else return native::fmlsl2<A>(c,a,b);
      } else {
        if constexpr(Op==0) return native::fmlal_lane<A,Lane>(c,a,b);
        else if constexpr(Op==1) return native::fmlal2_lane<A,Lane>(c,a,b);
        else if constexpr(Op==2) return native::fmlsl_lane<A,Lane>(c,a,b);
        else return native::fmlsl2_lane<A,Lane>(c,a,b);
      }
    }();
    std::array<std::uint32_t,N> result{};value.store_bits(result.data());return result;
  }
  template<unsigned Op,std::size_t N,std::size_t M,int Lane>
  __attribute__((noinline,target("fp16fml"))) auto hardware(input const & x) {
    return apply<strong,Op,N,M,Lane>(x);
  }
  template<unsigned Op,std::size_t N,std::size_t M,int Lane>
  bool check() {
    struct entry {input x;std::array<std::uint32_t,N> expected;bool equivalent;};
    constexpr auto seed=native_test::property_seed^(Op*1024+N*128+M*16+Lane+1);
    static constexpr auto cases=native_test::property_cases<16>(seed,[](auto & rng) {
      auto x=generate(rng);auto expected=apply<strong,Op,N,M,Lane>(x);
      return entry{x,expected,expected==apply<weak,Op,N,M,Lane>(x)};
    });
    static_assert([]{for(auto const & x:cases) if(!x.equivalent) return false;return true;}());
    for(std::size_t i=0;i<cases.size();++i) {
      auto const & c=cases[i];
      if(!native_test::property_equal("FHM consteval/native",seed,i,c.expected,
        hardware<Op,N,M,Lane>(c.x),c.x.acc,c.x.a,c.x.b,Op,N,M,Lane)) return false;
    }
    return true;
  }
  template<unsigned Op,std::size_t N,std::size_t M,std::size_t... L>
  bool lanes(std::index_sequence<L...>) {return (check<Op,N,M,int(L)>() && ...);}
  template<unsigned Op,std::size_t N> bool shapes() {
    return check<Op,N,2*N,-1>() && lanes<Op,N,4>(std::make_index_sequence<4>{}) &&
      lanes<Op,N,8>(std::make_index_sequence<8>{});
  }
}
int main() {
  auto cpu=native::observe_arm_capabilities();
  if(!cpu.present.valid() || !cpu.observed.valid()) return 1;
  if(!native::classify_isa(cpu,strong).admitted()) return 77;
  floating_fixture::environment env;
  return shapes<0,2>() && shapes<1,2>() && shapes<2,2>() && shapes<3,2>() &&
    shapes<0,4>() && shapes<1,4>() && shapes<2,4>() && shapes<3,4>()?0:1;
}
