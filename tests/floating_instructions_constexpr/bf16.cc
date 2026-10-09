// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "common.h"
import native.arm.bf16;
namespace {
#if defined(NATIVE_FLOAT_POLYFILL)
  bool hardware_admitted=false;
#endif
  constexpr auto strong=native::feature_closure(native::arm_feature::neon_bf16);
  constexpr auto weak=native::neon;
  struct input {std::array<std::uint32_t,4> acc;std::array<std::uint16_t,8> a,b;};
  constexpr input generate(native_test::property_rng & rng) {
    input x{};
    for(auto & v:x.acc) v=floating_fixture::special_bits<std::uint32_t>(rng);
    // Use binary32 special exponents/payloads, truncated to BF16 encodings.
    for(auto & v:x.a) v=std::uint16_t(floating_fixture::special_bits<std::uint32_t>(rng)>>16);
    for(auto & v:x.b) v=std::uint16_t(floating_fixture::special_bits<std::uint32_t>(rng)>>16);
    return x;
  }
  template<native::isa<native::arm> A,unsigned Op,std::size_t N,std::size_t M,int Lane>
  constexpr __attribute__((target("bf16"))) auto apply(input const & x) {
    auto c=native::simd<float,N,A>::load_bits(x.acc.data());
    auto a=native::simd<native::bf16,2*N,A>::load_bits(x.a.data());
    auto b=native::simd<native::bf16,M,A>::load_bits(x.b.data());
    auto value=[&]() __attribute__((target("bf16"))) {
      if constexpr(Op==0) {
        if constexpr(Lane<0) return native::bfdot<A>(c,a,b);
        else return native::bfdot_lane<A,Lane>(c,a,b);
      } else if constexpr(Op==1) return native::bfmmla<A>(c,a,b);
      else if constexpr(Op==2) {
        if constexpr(Lane<0) return native::bfmlalb<A>(c,a,b);
        else return native::bfmlalb_lane<A,Lane>(c,a,b);
      } else {
        if constexpr(Lane<0) return native::bfmlalt<A>(c,a,b);
        else return native::bfmlalt_lane<A,Lane>(c,a,b);
      }
    }();
    std::array<std::uint32_t,N> result{};value.store_bits(result.data());return result;
  }
  template<unsigned Op,std::size_t N,std::size_t M,int Lane>
  __attribute__((noinline,target("bf16"))) auto hardware(input const & x) {
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
#if defined(NATIVE_FLOAT_POLYFILL)
    native_test::property_rng random{native_test::property_config(16).seed^seed};
#endif
    for(std::size_t i=0;i<cases.size();++i) {
      auto const & c=cases[i];
#if defined(NATIVE_FLOAT_POLYFILL)
      if(apply<weak|native::polyfill,Op,N,M,Lane>(c.x)!=c.expected || apply<native::isa<native::arm>(native::polyfill),Op,N,M,Lane>(c.x)!=c.expected) return false;
      if(!hardware_admitted) continue;
      auto runtime_input=generate(random);
      for(unsigned state=0;state<256;++state) {
        std::uint64_t requested=(std::uint64_t(state&3)<<22)|
          (std::uint64_t((state>>2)&1)<<24)|(std::uint64_t((state>>3)&1)<<25)|
          (std::uint64_t((state>>4)&1)<<19)|(std::uint64_t((state>>5)&1)<<1)|
          std::uint64_t((state>>6)&1)|(std::uint64_t((state>>7)&1)<<13);
        asm volatile("msr fpcr, %0" :: "r"(requested) : "memory");
        std::uint64_t observed;asm volatile("mrs %0, fpcr" : "=r"(observed) :: "memory");
        if(observed!=requested) continue;
        auto expected=hardware<Op,N,M,Lane>(runtime_input);
        auto permitted=apply<weak|native::polyfill,Op,N,M,Lane>(runtime_input);
        auto scalar=apply<native::isa<native::arm>(native::polyfill),Op,N,M,Lane>(runtime_input);
        if(expected!=permitted || expected!=scalar) {
          std::printf("bf16 polyfill state %u case %zu mismatch\n",state,i);return false;
        }
      }
      asm volatile("msr fpcr, xzr" ::: "memory");
#else
      if(!native_test::property_equal("BF16 consteval/native",seed,i,c.expected,
        hardware<Op,N,M,Lane>(c.x),c.x.acc,c.x.a,c.x.b,Op,N,M,Lane)) return false;
#endif
    }
    return true;
  }
  template<unsigned Op,std::size_t N,std::size_t M,std::size_t... L>
  bool lanes(std::index_sequence<L...>) {return (check<Op,N,M,int(L)>() && ...);}
  template<unsigned Op,std::size_t N> bool shapes() {
    return check<Op,N,2*N,-1>() && lanes<Op,N,4>(std::make_index_sequence<Op==0?2:4>{}) &&
      lanes<Op,N,8>(std::make_index_sequence<Op==0?4:8>{});
  }
}
int main() {
  auto cpu=native::observe_arm_capabilities();
  if(!cpu.present.valid() || !cpu.observed.valid()) return 1;
#if defined(NATIVE_FLOAT_POLYFILL)
  hardware_admitted=native::classify_isa(cpu,strong).admitted();
#else
  if(!native::classify_isa(cpu,strong).admitted()) return 77;
#endif
  floating_fixture::environment env;
  return shapes<0,2>() && shapes<0,4>() && check<1,4,8,-1>() && shapes<2,4>() && shapes<3,4>()?0:1;
}
