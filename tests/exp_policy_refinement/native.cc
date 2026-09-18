// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "policies.h"
#include <array>
#include <bit>
#include <cstdio>
#include <cstdint>
#include <utility>
#include <immintrin.h>
import simd;

static_assert((SIMD_TARGET_MINIMUM&~simd::avx2::features)==0,
  "This exp refinement prototype requires a translation-unit minimum no stronger than AVX2/FMA/BMI2");
#if __has_feature(address_sanitizer)
extern "C" int exp_refinement_asan_instrumented() { return 1; }
#endif

namespace refinement_test {
  // The original Arch is deliberately independent of the policy's minimal tag.
  // Calling the unchanged array kernel preserves every polynomial/scaling step.
  // Explicit empty-array construction also avoids the separate MSVC STL issue
  // where default construction of wide<vec<float,1,avx512>,0> needs AVX512 in
  // std::array's implicit constructor, even inside a correctly targeted caller.
#define EMIT_EXP_POLICY(i,name,raw,result) \
  SIMD_TARGET_PUSH(name) \
  template<bool Flush=false,std::size_t L,std::size_t N,simd::architecture A> \
    requires simd::requires_abi<A,exp_policies,i> \
  __attribute__((always_inline)) inline simd::wide<simd::vec<float,L,A>,N> \
  refined_exp(simd::wide<simd::vec<float,L,A>,N> const & input) \
    noexcept(noexcept(simd::wide<simd::vec<float,L,A>,N>{simd::exp<Flush>(input.registers)})) { \
    return simd::wide<simd::vec<float,L,A>,N>{simd::exp<Flush>(input.registers)}; \
  } \
  template<bool Reference,bool Flush,std::size_t L,std::size_t N,simd::architecture A> \
    requires simd::requires_abi<A,exp_policies,i> \
  __attribute__((noinline)) void evaluate(std::uint32_t const * input,std::uint32_t * output) { \
    using V=simd::vec<float,L,A>; \
    auto values=[&]<std::size_t... K>(std::index_sequence<K...>) { \
      if constexpr(N==0) return simd::wide<V,0>{std::array<V,0>{{}}}; \
      else return simd::wide<V,N>{V::load_bits(input+K*L)...}; \
    }(std::make_index_sequence<N>{}); \
    static_assert(noexcept(refined_exp<Flush>(values))==noexcept(simd::exp<Flush>(values))); \
    auto computed=[&] { \
      if constexpr(Reference) return simd::exp<Flush>(values); \
      else return refined_exp<Flush>(values); \
    }(); \
    static_assert(std::same_as<decltype(computed),simd::wide<V,N>>); \
    for(std::size_t k=0;k<N;++k) computed.registers[k].store_bits(output+k*L); \
  } \
  extern "C" __attribute__((noinline)) void refined_codegen_##name##_narrow(float const * input,float * output) { \
    using V=simd::vec<float,8,SIMD_TARGET_TYPE(name)>; \
    auto value=refined_exp(simd::wide<V,2>{V::load(input),V::load(input+8)}); \
    value.registers[0].store(output);value.registers[1].store(output+8); \
  } \
  extern "C" __attribute__((noinline)) void refined_codegen_##name##_native(float const * input,float * output) { \
    constexpr std::size_t lanes=simd::has_features<SIMD_TARGET_TYPE(name),SIMD_TARGET_TYPE(exp_base)::features>?16:8; \
    using V=simd::vec<float,lanes,SIMD_TARGET_TYPE(name)>; \
    auto value=refined_exp(simd::wide<V,2>{V::load(input),V::load(input+lanes)}); \
    value.registers[0].store(output);value.registers[1].store(output+lanes); \
  } \
  SIMD_TARGET_POP()
  EXP_POLICY_CELLS(EMIT_EXP_POLICY)
#undef EMIT_EXP_POLICY

  constexpr std::array<std::uint32_t,24> special{
    0,0x80000000u,1,0x80000001u,0x007fffffu,0x00800000u,
    0x3f800000u,0xbf800000u,0x7f800000u,0xff800000u,0x7fc00000u,0x7f800001u,
    0xffc12345u,0xffffffffu,0xc2aeac4fu,0xc2aeac50u,0xc2aeac51u,
    0xc2d00000u,0xc2d00001u,0xc2cfffffu,0x42b17217u,0x42b17218u,0x42b17219u,0x7f7fffffu};
  template<simd::architecture A,std::size_t L,std::size_t N,bool Flush>
  bool compare_shape() {
    std::array<std::uint32_t,L*N> input{},reference{},candidate{};
    for(std::size_t seed=0;seed<48;++seed) {
      for(std::size_t k=0;k<input.size();++k) {
        auto position=k+seed;
        input[k]=position<special.size()?special[position]:
          std::bit_cast<std::uint32_t>((float(int(position%97)-48))*.375f);
      }
      // Separate non-inline pointer entry points prevent merging the candidate
      // and reference graphs into one common expression before comparison.
      evaluate<true,Flush,L,N,A>(input.data(),reference.data());
      evaluate<false,Flush,L,N,A>(input.data(),candidate.data());
      if(reference!=candidate) {
        std::printf("mismatch L=%zu N=%zu Flush=%d seed=%zu\n",L,N,int(Flush),seed);
        return false;
      }
    }
    return true;
  }
  template<simd::architecture A,std::size_t L> bool compare_width() {
    return compare_shape<A,L,0,false>() && compare_shape<A,L,1,false>() &&
      compare_shape<A,L,3,false>() && compare_shape<A,L,0,true>() &&
      compare_shape<A,L,1,true>() && compare_shape<A,L,3,true>();
  }
  template<simd::architecture A> bool compare_case() {
    bool equal=compare_width<A,1>() && compare_width<A,2>() &&
      compare_width<A,3>() && compare_width<A,4>() && compare_width<A,8>();
    if constexpr(simd::has_features<A,SIMD_TARGET_TYPE(exp_base)::features>)
      equal=equal && compare_width<A,16>();
    return equal;
  }
}

int main() {
  auto cpu=simd::observe_x86_capabilities();
  auto controls=_mm_getcsr()&~0x3fu;
  unsigned executed=0,skipped=0;
#define RUN_EXP_POLICY(i,name,raw,result) \
  if(simd::classify_isa(cpu,SIMD_TARGET_TYPE(name){},SIMD_TARGET_MINIMUM).admitted()) { \
    if(!refinement_test::compare_case<SIMD_TARGET_TYPE(name)>()) return 1; \
    ++executed;std::printf("exp policy %u %s: executed\n",i,#name); \
  } else { ++skipped;std::printf("exp policy %u %s: skipped (not admitted)\n",i,#name); }
  EXP_POLICY_CELLS(RUN_EXP_POLICY)
#undef RUN_EXP_POLICY
  using extra=simd::isa<simd::avx2::features|simd::feature::aes>;
  if(simd::classify_isa(cpu,extra{},SIMD_TARGET_MINIMUM).admitted()) {
    if(!refinement_test::compare_case<extra>()) return 2;
    std::puts("AVX2 caller with extra AES tag: original type retained, outputs exact");
  }
  if((_mm_getcsr()&~0x3fu)!=controls) return 3;
  std::printf("%u policy cases executed, %u skipped; MXCSR controls unchanged: 0x%x\n",executed,skipped,controls);
  return executed?0:77;
}
