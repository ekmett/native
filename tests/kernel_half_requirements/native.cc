// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <simd/targets.h>
#include <array>
#include <cstdio>
#include <type_traits>
import simd;

template<class V> concept can_load=requires(float const * p) { simd::load_simd<V>(p); };
template<class V> concept can_store=requires(float * p,V value) { simd::store_simd(p,value); };
struct missing_architecture {};
struct unrelated_architecture { using architecture=int; };
struct malformed_architecture {
  using architecture=int;
  template<std::size_t> static malformed_architecture load_memory(float const *) { return {}; }
  template<std::size_t> void store_memory(float *) const {}
};
static_assert(!can_load<int> && !can_load<missing_architecture> && !can_load<unrelated_architecture>);
static_assert(!can_store<int> && !can_store<missing_architecture> && !can_store<unrelated_architecture>);
static_assert(!can_load<malformed_architecture> && !can_store<malformed_architecture>);

#if defined(__x86_64__) || defined(_M_X64)
#define RAW_TARGET avx512
#define FP_TARGET avx512_fp16
#define BF_TARGET avx512_bf16
constexpr std::size_t lanes=16;
#else
#define RAW_TARGET neon
#define FP_TARGET neon_fp16
#define BF_TARGET neon_bf16
constexpr std::size_t lanes=4;
#endif
using advertised=simd::isa<SIMD_TARGET_TYPE(FP_TARGET)::features|SIMD_TARGET_TYPE(BF_TARGET)::features>;
using raw=simd::vec<float,lanes,advertised>;
using half=simd::vec<simd::fp16,lanes*2,advertised>;
using brain=simd::vec<simd::bf16,lanes*2,advertised>;
static_assert(std::same_as<raw::architecture,advertised>);
static_assert(std::same_as<half::architecture,advertised>);
static_assert(std::same_as<brain::architecture,advertised>);

// Each function is compiled without the other advertised extension. Compiling
// these actual bodies catches an always-inline callee with an excessive target.
SIMD_TARGET_PUSH(RAW_TARGET)
__attribute__((noinline)) bool check_raw() {
  using pack=simd::wide<raw,2>;
  std::array<float,lanes+1> input{},output{};
  input.fill(4.f);
  auto a=simd::load_simd<raw>(input.data());
  auto qualified=simd::load_simd<raw const>(input.data());
  (void)qualified;
  pack x{a,a},y{raw(2.f),raw(2.f)},z{raw(1.f),raw(1.f)};
  auto result=floor(sqrt(abs(fma(x,y,z))));
  auto masks=result==simd::broadcast<raw,2>(raw(3.f));
  static_assert(std::same_as<typename raw::mask::architecture,advertised>);
  auto chosen=select(~masks,pack::broadcast(raw(0.f)),result);
  simd::wide<raw,0> empty{};
  simd::wide<raw::mask,0> empty_masks{};
  (void)empty;(void)empty_masks;
  for(auto const & value:chosen.registers) {
    simd::store_simd(output.data(),value);
    for(std::size_t i=0;i<lanes;++i) if(output[i]!=3.f) return false;
  }
  for(std::size_t count: {std::size_t(0),std::size_t(1),lanes-1,lanes}) {
    output.fill(-1.f);
    auto value=simd::load_simd_partial<raw>(input.data(),count,2.f);
    simd::store_simd_partial(output.data(),value,count);
    for(std::size_t i=0;i<output.size();++i)
      if(output[i]!=(i<count?4.f:-1.f)) return false;
    output.fill(-1.f);
    auto inherited=raw::load_partial(input.data(),count,2.f);
    inherited.store_partial(output.data(),count);
    for(std::size_t i=0;i<output.size();++i)
      if(output[i]!=(i<count?4.f:-1.f)) return false;
  }
  return true;
}
SIMD_TARGET_POP()

SIMD_TARGET_PUSH(FP_TARGET)
__attribute__((noinline)) bool check_fp16() {
  std::array<simd::fp16,lanes*2> values{};
  values.fill(simd::fp16::from_bits(0x4400)); // 4
  auto v=simd::load_simd<half>(values.data());
  auto result=sqrt(simd::wide<half,2>{v,v});
  for(auto const & value:result.registers) {
    simd::store_simd(values.data(),value);
    for(auto lane:values) if(lane.to_bits()!=0x4000) return false; // 2
  }
  return true;
}
SIMD_TARGET_POP()

SIMD_TARGET_PUSH(BF_TARGET)
__attribute__((noinline)) bool check_bf16() {
  std::array<simd::bf16,lanes*2> values{};
  values.fill(simd::bf16::from_bits(0x3f80)); // 1
  auto v=simd::load_simd<brain>(values.data());
  auto pack=simd::wide<brain,2>::broadcast(v);
  auto result=dot2(pack.registers[0],pack.registers[1],raw(0.f));
  std::array<float,lanes> output{};
  simd::store_simd(output.data(),result);
  for(float lane:output) if(lane!=2.f) return false;
  simd::store_simd(values.data(),pack.registers[0]);
  for(auto lane:values) if(lane.to_bits()!=0x3f80) return false;
  return true;
}
SIMD_TARGET_POP()

int main() {
#if defined(__x86_64__) || defined(_M_X64)
  auto cpu=simd::observe_x86_capabilities();
#else
  auto cpu=simd::observe_arm_capabilities();
#endif
  int executed=0;
#define RUN(target,body) \
  if(simd::classify_isa(cpu,SIMD_TARGET_TYPE(target){},SIMD_TARGET_MINIMUM).admitted()) { \
    ++executed;if(!body()) return 1;std::puts(#body ": pass"); \
  } else std::puts(#body ": CPU/OS skip");
  RUN(RAW_TARGET,check_raw)
  RUN(FP_TARGET,check_fp16)
  RUN(BF_TARGET,check_bf16)
#undef RUN
  return executed?0:77;
}
