// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "common.h"
import native.arm.fcma;
namespace {
  constexpr auto strong=native::feature_closure(native::arm_feature::complxnum)&native::neon_fp16;
  constexpr auto weak=native::neon;
  template<class T> using word=std::conditional_t<std::same_as<T,native::fp16>,std::uint16_t,
    std::conditional_t<std::same_as<T,float>,std::uint32_t,std::uint64_t>>;
  template<class T> struct input {std::array<word<T>,8> acc,a,b;};
  template<class T> constexpr auto generate(native_test::property_rng & rng) {
    input<T> x{};
    for(auto & v:x.acc) v=floating_fixture::special_bits<word<T>>(rng);
    for(auto & v:x.a) v=floating_fixture::special_bits<word<T>>(rng);
    for(auto & v:x.b) v=floating_fixture::special_bits<word<T>>(rng);
    return x;
  }
  template<class T,std::size_t N,native::isa<native::arm> A>
  constexpr auto load(std::array<word<T>,8> const & x) {
    if constexpr(std::same_as<T,double>) {
      auto values=std::bit_cast<std::array<double,8>>(x);
      return native::simd<T,N,A>::load(values.data());
    } else return native::simd<T,N,A>::load_bits(x.data());
  }
  template<native::isa<native::arm> A,class T,unsigned Rotation,std::size_t N,std::size_t M,int Lane,bool Add>
  constexpr __attribute__((target("complxnum,fullfp16"))) auto apply(input<T> const & x) {
    auto c=load<T,N,A>(x.acc);auto a=load<T,N,A>(x.a);auto b=load<T,M,A>(x.b);
    auto value=[&]() __attribute__((target("complxnum,fullfp16"))) {
      if constexpr(Add) return native::fcadd<A,Rotation>(a,b);
      else if constexpr(Lane<0) return native::fcmla<A,Rotation>(c,a,b);
      else return native::fcmla_lane<A,Rotation,Lane>(c,a,b);
    }();
    std::array<word<T>,N> result{};
    if constexpr(std::same_as<T,double>) {
      std::array<double,N> values{};value.store(values.data());
      result=std::bit_cast<std::array<word<T>,N>>(values);
    } else value.store_bits(result.data());
    return result;
  }
  template<class T,unsigned R,std::size_t N,std::size_t M,int L,bool Add>
  __attribute__((noinline,target("complxnum,fullfp16"))) auto hardware(input<T> const & x) {
    return apply<strong,T,R,N,M,L,Add>(x);
  }
  template<class T,unsigned R,std::size_t N,std::size_t M,int L,bool Add=false>
  bool check() {
    struct entry {input<T> x;std::array<word<T>,N> expected;bool equivalent;};
    constexpr auto seed=native_test::property_seed^(R*8192+N*1024+M*64+L+1+Add);
    static constexpr auto cases=native_test::property_cases<8>(seed,[](auto & rng) {
      auto x=generate<T>(rng);auto expected=apply<strong,T,R,N,M,L,Add>(x);
      return entry{x,expected,expected==apply<weak,T,R,N,M,L,Add>(x)};
    });
    static_assert([]{for(auto const & x:cases) if(!x.equivalent) return false;return true;}());
    for(std::size_t i=0;i<cases.size();++i) {
      auto const & c=cases[i];
      if(!native_test::property_equal("FCMA consteval/native",seed,i,c.expected,
        hardware<T,R,N,M,L,Add>(c.x),c.x.acc,c.x.a,c.x.b,R,N,M,L,Add)) return false;
    }
    return true;
  }
  template<class T,unsigned R,std::size_t N,std::size_t M,std::size_t... L>
  bool lanes(std::index_sequence<L...>) {return (check<T,R,N,M,int(L)>() && ...);}
  template<class T,unsigned R,std::size_t N> bool rotation() {
    bool ok=check<T,R,N,N,-1>();
    if constexpr(!std::same_as<T,double>) {
      constexpr auto min=std::same_as<T,float>?2:4;
      ok=ok && lanes<T,R,N,min>(std::make_index_sequence<min/2>{}) &&
        lanes<T,R,N,2*min>(std::make_index_sequence<min>{});
    }
    return ok;
  }
  template<class T,std::size_t N> bool shape() {
    return check<T,90,N,N,-1,true>() && check<T,270,N,N,-1,true>() &&
      rotation<T,0,N>() && rotation<T,90,N>() && rotation<T,180,N>() && rotation<T,270,N>();
  }
}
int main() {
  auto cpu=native::observe_arm_capabilities();
  if(!cpu.present.valid() || !cpu.observed.valid()) return 1;
  if(!native::classify_isa(cpu,strong).admitted()) return 77;
  floating_fixture::environment env;
  return shape<float,2>() && shape<float,4>() && shape<double,2>() &&
    shape<native::fp16,4>() && shape<native::fp16,8>()?0:1;
}
