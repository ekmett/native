// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "simd_adapter.h"
#include "simd_contract.h"
namespace i8mm_fixture {
  constexpr auto feature=native::isa(native::arm_feature::i8mm);
  static_assert(native::feature_closure(feature)==(feature&native::neon));
  static_assert(native::target_features("i8mm")==native::feature_closure(feature));
  static_assert(std::uint64_t(native::arm_feature::pauth)==14);
  static_assert(std::uint64_t(native::arm_feature::i8mm)==15);
  template<native::isa A> concept accepts=requires(int32x4_t c,int8x16_t a) {
    i8mm_api::smmla<A>(c,a,a);
  };
  static_assert(accepts<feature> && !accepts<native::neon>);
  static_assert(!accepts<native::isa(native::arm_feature::dotprod)>);
  template<unsigned L> concept accepts_lane=requires(int32x4_t c,uint8x16_t a,int8x8_t b) {
    i8mm_api::usdot_lane<feature,L>(c,a,b);
  };
  static_assert(accepts_lane<1> && !accepts_lane<2> && !accepts_lane<~0u>);

  // Scalar reference uses unsigned accumulation so signed overflow is never UB.
  template<class A,class B> std::uint32_t matrix(std::uint32_t c,A a,B b,unsigned row,unsigned col) {
    for(unsigned k=0;k<8;++k) c+=std::uint32_t(int(a[8*row+k])*int(b[8*col+k]));
    return c;
  }
  template<class A,class B> std::uint32_t dot(std::uint32_t c,A a,B b,unsigned group,unsigned bg) {
    for(unsigned k=0;k<4;++k) c+=std::uint32_t(int(a[4*group+k])*int(b[4*bg+k]));
    return c;
  }
  template<unsigned L,class C,class A,class B>
  __attribute__((target("i8mm"))) bool lanes(C c,A a,B b) {
    auto us=i8mm_api::usdot_lane<feature,L>(c,a,b);
    for(unsigned j=0;j<sizeof(C)/4;++j)
      if(std::uint32_t(us[j])!=dot(std::uint32_t(c[j]),a,b,j,L)) return false;
    return true;
  }
  template<unsigned L,class C,class A,class B>
  __attribute__((target("i8mm"))) bool signed_lanes(C c,A a,B b) {
    auto su=i8mm_api::sudot_lane<feature,L>(c,a,b);
    for(unsigned j=0;j<sizeof(C)/4;++j)
      if(std::uint32_t(su[j])!=dot(std::uint32_t(c[j]),a,b,j,L)) return false;
    return true;
  }
  __attribute__((target("i8mm"),noinline)) bool check(int32x4_t c,int8x16_t a,int8x16_t b) {
    auto au=__builtin_bit_cast(uint8x16_t,a),bu=__builtin_bit_cast(uint8x16_t,b);
    auto cu=__builtin_bit_cast(uint32x4_t,c);
    auto s=i8mm_api::smmla<feature>(c,a,b);
    auto u=i8mm_api::ummla<feature>(cu,au,bu);
    auto us=i8mm_api::usmmla<feature>(c,au,b);
    auto d=i8mm_api::usdot<feature>(c,au,b);
    int32x2_t c2{c[0],c[1]};
    int8x8_t a8{},b8{};uint8x8_t au8{},bu8{};
    for(unsigned j=0;j<8;++j) { a8[j]=a[j]; b8[j]=b[j]; au8[j]=au[j]; bu8[j]=bu[j]; }
    auto d2=i8mm_api::usdot<feature>(c2,au8,b8);
    for(unsigned j=0;j<4;++j) {
      if(std::uint32_t(s[j])!=matrix(cu[j],a,b,j/2,j%2) ||
         u[j]!=matrix(cu[j],au,bu,j/2,j%2) ||
         std::uint32_t(us[j])!=matrix(cu[j],au,b,j/2,j%2) ||
         std::uint32_t(d[j])!=dot(cu[j],au,b,j,j)) return false;
      if(j<2 && std::uint32_t(d2[j])!=dot(cu[j],au8,b8,j,j)) return false;
    }
    return
      lanes<0>(c,au,b) &&
      lanes<1>(c,au,b) &&
      lanes<2>(c,au,b) &&
      lanes<3>(c,au,b) &&
      lanes<0>(c,au,b8) &&
      lanes<1>(c,au,b8) &&
      lanes<0>(c2,au8,b) &&
      lanes<1>(c2,au8,b) &&
      lanes<2>(c2,au8,b) &&
      lanes<3>(c2,au8,b) &&
      lanes<0>(c2,au8,b8) &&
      lanes<1>(c2,au8,b8) &&
      signed_lanes<0>(c,a,bu) &&
      signed_lanes<1>(c,a,bu) &&
      signed_lanes<2>(c,a,bu) &&
      signed_lanes<3>(c,a,bu) &&
      signed_lanes<0>(c,a,bu8) &&
      signed_lanes<1>(c,a,bu8) &&
      signed_lanes<0>(c2,a8,bu) &&
      signed_lanes<1>(c2,a8,bu) &&
      signed_lanes<2>(c2,a8,bu) &&
      signed_lanes<3>(c2,a8,bu) &&
      signed_lanes<0>(c2,a8,bu8) &&
      signed_lanes<1>(c2,a8,bu8);
  }
  int run() {
    auto cpu=native::observe_arm_capabilities();
    auto admission=native::classify_isa(cpu,feature);
    if(!admission.admitted()) { std::printf("I8MM skipped: %s\n",admission.reason()); return 77; }
    int32x4_t c{0,2147483647,(-2147483647-1),-1};
    int8x16_t a{},b{};
    // Distinct matrix row/column sentinels, then all individual product locations.
    for(unsigned j=0;j<16;++j) { a[j]=std::int8_t(j+1); b[j]=std::int8_t(2*int(j)-17); }
    if(!check(c,a,b)) return 1;
    for(unsigned i=0;i<16;++i) for(unsigned j=0;j<16;++j) {
      a={};b={};a[i]=-128;b[j]=127;
      if(!check(c,a,b)) return 2;
    }
    for(int edge:{-128,-1,0,1,127}) {
      for(unsigned j=0;j<16;++j) a[j]=b[j]=std::int8_t(edge);
      if(!check(c,a,b)) return 3;
    }
    std::uint32_t seed=0x9e3779b9;
    auto random=[&] { seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;return seed; };
    for(unsigned iteration=0;iteration<10000;++iteration) {
      for(unsigned j=0;j<16;++j) { a[j]=std::bit_cast<std::int8_t>(std::uint8_t(random())); b[j]=std::bit_cast<std::int8_t>(std::uint8_t(random())); }
      for(unsigned j=0;j<4;++j) c[j]=std::bit_cast<std::int32_t>(random());
      if(!check(c,a,b)) return 4;
    }
    std::puts("I8MM matrix layout, wrapping and all mixed-sign dot lanes passed");
    return 0;
  }
}
