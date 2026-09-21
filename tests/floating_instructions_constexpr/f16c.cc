// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "common.h"
#include "../x86_f16c/oracle.h"
#include <immintrin.h>
import native.x86.f16c;
namespace {
  constexpr auto strong=native::feature_closure(native::x86_feature::f16c);
  constexpr auto weak=native::feature_closure(native::x86_feature::avx);
  constexpr auto weak4=native::feature_closure(native::x86_feature::sse2);
  constexpr native::isa<native::x86> scalar{};
  static_assert(native::cvtss_sh<0>(1.f)==0x3c00);
  static_assert(native::cvtsh_ss(0x3c00)==1.f);
  static_assert(native::cvtss_sh<scalar,0>(std::bit_cast<float>(0x33000000u))==0);
  static_assert(native::cvtss_sh<scalar,2>(std::bit_cast<float>(0x33000000u))==1);
  static_assert(native::cvtss_sh<scalar,4>(std::bit_cast<float>(0x33000000u))==0);
  static_assert(native::cvtss_sh<scalar,0>(std::bit_cast<float>(0xff812345u))==0xfe09);
  template<unsigned First> consteval bool exhaustive_widen() {
    for(unsigned i=First;i<First+4096;++i) {
      auto expected=f16c_fixture::widen(std::uint16_t(i));
      if(std::bit_cast<std::uint32_t>(native::cvtsh_ss<scalar>(std::uint16_t(i)))!=expected ||
         std::bit_cast<std::uint32_t>(native::cvtsh_ss<strong>(std::uint16_t(i)))!=expected) return false;
    }
    return true;
  }
  template<unsigned I> struct widen_page {static_assert(exhaustive_widen<I*4096>());};
  template<unsigned... I> consteval bool widen_pages(std::integer_sequence<unsigned,I...>) {
    return ((sizeof(widen_page<I>)>0) && ...);
  }
  static_assert(widen_pages(std::make_integer_sequence<unsigned,16>{}));
  template<unsigned I> consteval bool immediate() {
    constexpr std::array<std::uint32_t,12> source{0,0x80000000,1,0x80000001,0x33000000,
      0x33000001,0x387fe000,0x477ff000,0xc77ff000,0x7f800000,0x7f800001,0xffc54321};
    for(auto x:source) {
      auto expected=f16c_fixture::narrow(x,I&4?0:I&3,false);
      auto f=std::bit_cast<float>(x);
      if(native::cvtss_sh<scalar,I>(f)!=expected || native::cvtss_sh<strong,I>(f)!=expected ||
         native::cvtss_sh<I,scalar>(f)!=expected) return false;
    }
    return true;
  }
  template<unsigned I> struct immediate_check {static_assert(immediate<I>());};
  template<unsigned... I> consteval bool immediates(std::integer_sequence<unsigned,I...>) {
    return ((sizeof(immediate_check<I>)>0) && ...);
  }
  static_assert(immediates(std::make_integer_sequence<unsigned,256>{}));

  struct input {std::array<std::uint32_t,8> floats;std::array<std::uint16_t,8> halves;};
  constexpr input generate(native_test::property_rng & rng) {
    input x{};
    for(auto & f:x.floats) f=floating_fixture::special_bits<std::uint32_t>(rng);
    for(auto & h:x.halves) h=floating_fixture::special_bits<std::uint16_t>(rng);
    return x;
  }
  template<native::isa<native::x86> A,std::size_t N,unsigned I>
  constexpr __attribute__((target("f16c"))) auto narrow(input const & x) {
    auto values=std::bit_cast<std::array<float,8>>(x.floats);
    auto a=native::simd<float,N,A>::load(values.data());
    auto h=native::cvtps_ph<A,I>(a);
    std::array<std::uint16_t,N> result{};h.store_bits(result.data());return result;
  }
  template<native::isa<native::x86> A,std::size_t N>
  constexpr __attribute__((target("f16c"))) auto widen(input const & x) {
    auto h=native::simd<native::fp16,N,A>::load_bits(x.halves.data());
    auto a=native::cvtph_ps<A,N>(h);
    std::array<float,N> values{};a.store(values.data());
    return std::bit_cast<std::array<std::uint32_t,N>>(values);
  }
  template<std::size_t N,unsigned I>
  __attribute__((noinline,target("f16c"))) auto hardware_narrow(input const & x) {return narrow<strong,N,I>(x);}
  template<std::size_t N>
  __attribute__((noinline,target("f16c"))) auto hardware_widen(input const & x) {return widen<strong,N>(x);}
  template<std::size_t N,unsigned I> bool check() {
    struct entry {input x;std::array<std::uint16_t,N> half;std::array<std::uint32_t,N> full;bool equivalent;};
    constexpr auto seed=native_test::property_seed^(N*1024+I);
    static constexpr auto cases=native_test::property_cases<32>(seed,[](auto & rng) {
      auto x=generate(rng);auto h=narrow<strong,N,I>(x);auto f=widen<strong,N>(x);
      bool equivalent=h==narrow<weak,N,I>(x) && f==widen<weak,N>(x);
      if constexpr(N==4) equivalent=equivalent && h==narrow<weak4,N,I>(x) && f==widen<weak4,N>(x);
      return entry{x,h,f,equivalent};
    });
    static_assert([]{for(auto const & x:cases) if(!x.equivalent) return false;return true;}());
    for(std::size_t i=0;i<cases.size();++i) {
      auto const & c=cases[i];
      if(!native_test::property_equal("F16C narrow consteval/native",seed,i,c.half,
          hardware_narrow<N,I>(c.x),c.x.floats,I) ||
         !native_test::property_equal("F16C widen consteval/native",seed,i,c.full,
          hardware_widen<N>(c.x),c.x.halves)) return false;
      for(std::size_t lane=0;lane<N;++lane)
        if(c.half[lane]!=f16c_fixture::narrow(c.x.floats[lane],I&4?0:I&3,false) ||
           c.full[lane]!=f16c_fixture::widen(c.x.halves[lane])) return false;
    }
    return true;
  }
  template<unsigned... I> bool checks(std::integer_sequence<unsigned,I...>) {
    return ((check<4,I>() && check<8,I>()) && ...);
  }
}
int main() {
  auto cpu=native::observe_x86_capabilities();
  if(!cpu.present.valid() || !cpu.observed.valid()) return 1;
  if(!native::classify_isa(cpu,strong).admitted()) return 77;
  auto saved=_mm_getcsr();_mm_setcsr(0x1f80);
  bool ok=checks(std::integer_sequence<unsigned,0,1,2,3,4,7,8,255>{});
  _mm_setcsr(saved);return ok?0:1;
}
