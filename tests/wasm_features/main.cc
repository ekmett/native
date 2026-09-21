// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdio>
#include <concepts>
#include <cstdint>
#include <string_view>
#include <optional>
#include <span>
#if NATIVE_TEST_INTERFACE == 0
#include <native/wasm/features.h>
#elif NATIVE_TEST_INTERFACE == 1
import native.wasm.features;
#elif NATIVE_TEST_INTERFACE == 2
import native.features;
#else
import native;
#endif

namespace {
  using namespace native;
  static_assert(wasm_feature_count==2);
  static_assert(static_cast<unsigned>(wasm_feature::simd128)==0);
  static_assert(static_cast<unsigned>(wasm_feature::relaxed_simd)==1);
  template<feature_set<wasm_feature> A> struct requirement {};
  static_assert(!std::same_as<requirement<wasm_feature::simd128>,requirement<wasm_feature::relaxed_simd>>);
  static_assert(arch<feature_set<wasm_feature>>);
  template<class S,class E> concept accepts=requires(S s,E e) { S(e); s.get(e); s.set(e,true); };
  static_assert(accepts<feature_set<wasm_feature>,wasm_feature>);
  static_assert(!accepts<feature_set<wasm_feature>,x86_feature>);
  static_assert(!accepts<feature_set<wasm_feature>,arm_feature>);
  static_assert(!accepts<feature_set<x86_feature>,wasm_feature>);
  static_assert(!accepts<feature_set<arm_feature>,wasm_feature>);
  static_assert(!std::constructible_from<feature_set<wasm_feature>,feature_set<arm_feature>>);
  static_assert(!std::constructible_from<feature_set<wasm_feature>,feature_set<x86_feature>>);
  constexpr auto standard_isa=isa(wasm_feature::simd128);
  constexpr auto relaxed=standard_isa&wasm_feature::relaxed_simd;
  static_assert(target_features("simd128")==standard_isa);
  static_assert(target_features("relaxed-simd")==relaxed);
  static_assert(feature_closure(isa(wasm_feature::relaxed_simd))==relaxed);
  static_assert(feature_closure(standard_isa)==standard_isa);
  static_assert(target<relaxed,relaxed,standard_isa,scalar> == 0);
  static_assert(target<standard_isa,relaxed,standard_isa,scalar> == 1);
  static_assert(wasm_feature_probe(wasm_feature::simd128).size()==43);
  static_assert(wasm_feature_probe(wasm_feature::relaxed_simd).size()==34);
  static_assert(wasm_feature_probe(static_cast<wasm_feature>(wasm_feature_count)).empty());
  static_assert(wasm_feature_probe(static_cast<wasm_feature>(-1)).empty());

  constexpr bool properties_and_families() {
    isa bits;
    bits.wasm_simd128=true;
    bits.wasm_relaxed_simd=true;
    if(bits!=relaxed || !bits.wasm_simd128 || !bits.wasm_relaxed_simd) return false;
    for(unsigned i=0;i<x86_feature_count;++i)
      if(bits.has(static_cast<x86_feature>(i)) || isa(static_cast<x86_feature>(i)).has(standard_isa)) return false;
    for(unsigned i=0;i<arm_feature_count;++i)
      if(bits.has(static_cast<arm_feature>(i)) || isa(static_cast<arm_feature>(i)).has(standard_isa)) return false;
    bits.wasm_relaxed_simd=false;
    if(bits!=standard_isa) return false;
    bits.wasm_simd128=false;
    return bits==scalar;
  }
  static_assert(properties_and_families());

  constexpr bool observations() {
    auto unknown=observe_wasm_capabilities();
    if(isa(unknown.present)!=scalar || isa(unknown.observed)!=scalar) return false;
    if(!classify_isa(unknown,scalar).admitted() || classify_isa(unknown,standard_isa).admitted()) return false;
    // Every combination includes failed queries with stale positive answers.
    for(unsigned mask=0;mask<16;++mask) {
      wasm_capabilities::raw_observations raw{bool(mask&1),bool(mask&2),bool(mask&4),bool(mask&8)};
      auto caps=decode_wasm_capabilities(raw);
      if(caps.raw.simd128_observed!=raw.simd128_observed || caps.raw.simd128!=raw.simd128 ||
         caps.raw.relaxed_simd_observed!=raw.relaxed_simd_observed || caps.raw.relaxed_simd!=raw.relaxed_simd)
        return false;
      auto standard=(mask&3)==3;
      auto relaxation=(mask&12)==12;
      if(caps.present.has(wasm_feature::simd128)!=standard ||
         caps.present.has(wasm_feature::relaxed_simd)!=relaxation ||
         caps.observed.has(wasm_feature::simd128)!=bool(mask&1) ||
         caps.observed.has(wasm_feature::relaxed_simd)!=bool(mask&4)) return false;
      if(classify_isa(caps,standard_isa).admitted()!=standard ||
         classify_isa(caps,wasm_feature::relaxed_simd).admitted()!=(standard && relaxation) ||
         classify_isa(caps,scalar,relaxed).admitted()!=(standard && relaxation)) return false;
      auto before=classify_isa(caps,relaxed).admitted();
      caps.raw={true,true,true,true};
      if(classify_isa(caps,relaxed).admitted()!=before) return false;
    }
    auto positive=decode_wasm_capabilities({true,true,true,true});
    auto missing=positive;
    missing.observed.set(wasm_feature::simd128,false);
    auto rejected=classify_isa(missing,wasm_feature::relaxed_simd);
    if(rejected.admitted() || rejected.missing_features!=standard_isa || std::string_view(rejected.reason())!="simd128") return false;
    auto invalid=positive;
    invalid.present.set(static_cast<wasm_feature>(-1),true);
    if(!classify_isa(invalid,scalar).invalid_features) return false;
    invalid=positive;
    invalid.observed.flags[0]|=std::uint64_t{1}<<63;
    if(!classify_isa(invalid,standard_isa).invalid_features) return false;
    if(!classify_isa(positive,static_cast<wasm_feature>(wasm_feature_count)).invalid_features) return false;
    if(!classify_isa(positive,x86_feature::popcnt).invalid_features ||
       !classify_isa(positive,arm_feature::neon).invalid_features ||
       !classify_isa(positive,standard_isa,x86_feature::sse).invalid_features ||
       !classify_isa(positive,standard_isa,arm_feature::neon).invalid_features) return false;
    struct arm { feature_set<arm_feature> present{},observed{}; } arm;
    struct x86 { feature_set<x86_feature> present{},observed{}; std::uint64_t xcr0=0; bool xcr0_observed=false; } x86;
    return classify_isa(arm,standard_isa).invalid_features && classify_isa(x86,standard_isa).invalid_features;
  }
  static_assert(observations());
  template<class V> concept validator_accepted=requires(V v) { observe_wasm_capabilities(v); };
  static_assert(!validator_accepted<decltype([](std::span<std::uint8_t const>) { return std::optional<bool>{true}; })>);
  static_assert(!validator_accepted<decltype([](std::span<std::uint8_t const>) noexcept { return true; })>);
  constexpr bool callback_observations() {
    for(int baseline=-1;baseline<=1;++baseline)
      for(int simd128=-1;simd128<=1;++simd128)
        for(int relaxed_simd=-1;relaxed_simd<=1;++relaxed_simd) {
          int calls=0;
          bool bytes_match=true;
          auto validate=[&](std::span<std::uint8_t const> bytes) noexcept -> std::optional<bool> {
            auto expected=calls==0 ? wasm_feature_probe(wasm_feature::simd128).first(8) :
              wasm_feature_probe(calls==1 ? wasm_feature::simd128 : wasm_feature::relaxed_simd);
            if(bytes.size()!=expected.size()) bytes_match=false;
            else for(unsigned i=0;i<bytes.size();++i) if(bytes[i]!=expected[i]) bytes_match=false;
            int result=calls==0 ? baseline : calls==1 ? simd128 : relaxed_simd;
            ++calls;
            if(result<0) return std::nullopt;
            return result!=0;
          };
          auto caps=observe_wasm_capabilities(validate);
          auto expected=decode_wasm_capabilities(baseline==1 ?
            wasm_capabilities::raw_observations{simd128>=0,simd128==1,relaxed_simd>=0,relaxed_simd==1} :
            wasm_capabilities::raw_observations{});
          if(caps.present!=expected.present || caps.observed!=expected.observed ||
             !bytes_match || calls!=(baseline==1 ? 3 : 1)) return false;
        }
    return true;
  }
  static_assert(callback_observations());

}

int main(int argc,char ** argv) {
  if(argc==2 && std::string_view(argv[1])=="--probes") {
    for(auto feature:{native::wasm_feature::simd128,native::wasm_feature::relaxed_simd}) {
      for(auto byte:native::wasm_feature_probe(feature)) std::printf("%02x",unsigned(byte));
      std::puts("");
    }
    return 0;
  }
  if(!properties_and_families() || !observations() || !callback_observations()) return 1;
  auto caps=native::decode_wasm_capabilities({true,true,true,false});
  int selected=-1;
  auto choose=[&]<native::isa A>() { selected=A==standard_isa ? 1 : A==relaxed ? 2 : 0; };
  if(!native::with_isa(native::isa_list<relaxed,standard_isa,native::scalar>{},caps,choose) || selected!=1) return 2;
  return 0;
}
