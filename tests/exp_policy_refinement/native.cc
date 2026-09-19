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

static_assert(SIMD_TARGET_MINIMUM<=simd::avx2,
  "This exp refinement check requires a translation-unit minimum no stronger than AVX2/FMA/BMI2");
#if __has_feature(address_sanitizer)
extern "C" int exp_refinement_asan_instrumented() { return 1; }
#endif

namespace custom_exp {
  struct tagged { int value{}; };
  struct plain { int value{}; };
  struct throwing { int value{}; };
  inline int array_calls=0;
  template<bool F> tagged exp(tagged x,std::bool_constant<F>) noexcept { return {x.value+(F?2:1)}; }
  template<bool F,std::size_t N> auto exp(std::array<tagged,N> values,std::bool_constant<F>) noexcept {
    ++array_calls;
    for(auto & value:values) value=exp(value,std::bool_constant<F>{});
    return values;
  }
  plain exp(plain x) noexcept { return {x.value+3}; }
  throwing exp(throwing x) noexcept(false) { return x; }
  template<class T> concept has_flush=requires(simd::wide<T,2> const & x) { simd::exp<true>(x); };
  static_assert(has_flush<tagged> && !has_flush<plain>);
  static_assert(noexcept(simd::exp(simd::wide<tagged,2>{})));
  static_assert(!noexcept(simd::exp(simd::wide<throwing,2>{})));
  bool check() {
    auto a=simd::exp<true>(simd::wide<tagged,2>{tagged{4},tagged{5}});
    auto b=simd::exp(simd::wide<plain,2>{plain{4},plain{5}});
    auto c=simd::exp(simd::wide<float,2>{0.f,0.f});
    return array_calls==1 && a.registers[0].value==6 && a.registers[1].value==7 &&
      b.registers[0].value==7 && b.registers[1].value==8 &&
      c.registers[0]==1.f && c.registers[1]==1.f;
  }
}

namespace refinement_test {
  // The original Arch is deliberately independent of the policy's minimal tag.
  // Calling the unchanged array kernel preserves every polynomial/scaling step.
  // Explicit empty arrays keep this comparison independent of the separate
  // default-constructor regression, covered by tests/wide_construction.
#define EXP_EVALUATE(function,policies,i) \
  template<bool Reference,bool Flush,std::size_t L,std::size_t N,simd::isa A> \
    requires (simd::abi_lookup<A,policies>::index == i) \
  __attribute__((noinline)) void function(std::uint32_t const * input,std::uint32_t * output) { \
    using V=simd::vec<float,L,A>; \
    auto values=[&]<std::size_t... K>(std::index_sequence<K...>) { \
      if constexpr(N==0) return simd::wide<V,0>{std::array<V,0>{{}}}; \
      else return simd::wide<V,N>{V::load_bits(input+K*L)...}; \
    }(std::make_index_sequence<N>{}); \
    static_assert(noexcept(simd::exp<Flush>(values))==noexcept(simd::exp<Flush,V,N>(values))); \
    using W=simd::wide<V,N>; \
    using F=W (*)(W const &); \
    static_assert(static_cast<F>(&simd::exp<Flush>)==static_cast<F>(&simd::exp<Flush,L,N,A>)); \
    static_assert(static_cast<F>(&simd::math::exp<Flush>)==static_cast<F>(&simd::exp<Flush>)); \
    static_assert(static_cast<F>(&simd::exp<Flush>)!=static_cast<F>(&simd::exp<Flush,V,N>)); \
    auto computed=[&] { \
      if constexpr(Reference) return simd::exp<Flush,V,N>(values); \
      else return simd::exp<Flush>(values); \
    }(); \
    static_assert(std::same_as<decltype(computed),simd::wide<V,N>>); \
    for(std::size_t k=0;k<N;++k) computed.registers[k].store_bits(output+k*L); \
  }

#define EMIT_EXP_CALLER(i,name,raw) \
  SIMD_TARGET_PUSH(name) \
  EXP_EVALUATE(evaluate,caller_targets,i) \
  extern "C" __attribute__((noinline)) void refined_codegen_##name##_narrow(float const * input,float * output) { \
    using V=simd::vec<float,8,SIMD_TARGET_ISA(name)>; \
    auto value=simd::exp(simd::wide<V,2>{V::load(input),V::load(input+8)}); \
    value.registers[0].store(output);value.registers[1].store(output+8); \
  } \
  extern "C" __attribute__((noinline)) void refined_codegen_##name##_native(float const * input,float * output) { \
    constexpr std::size_t lanes=SIMD_TARGET_ISA(name).has(SIMD_TARGET_ISA(exp_base))?16:8; \
    using V=simd::vec<float,lanes,SIMD_TARGET_ISA(name)>; \
    auto value=simd::exp(simd::wide<V,2>{V::load(input),V::load(input+lanes)}); \
    value.registers[0].store(output);value.registers[1].store(output+lanes); \
  } \
  SIMD_TARGET_POP()
  EXP_CALLER_CASES(EMIT_EXP_CALLER)
#undef EMIT_EXP_CALLER

  // These bodies retain all caller Arch bits but have no BF16/FP16 attribute.
  // Both production and legacy reference must inline under that actual scope.
#define EMIT_RAW_EXP(i,name) \
  SIMD_TARGET_PUSH(name) \
  EXP_EVALUATE(evaluate_raw,exp_policies,i) \
  SIMD_TARGET_POP()
  SIMD_EXP_TARGETS(EMIT_RAW_EXP)
#undef EMIT_RAW_EXP
#undef EXP_EVALUATE

  constexpr std::array<std::uint32_t,24> special{
    0,0x80000000u,1,0x80000001u,0x007fffffu,0x00800000u,
    0x3f800000u,0xbf800000u,0x7f800000u,0xff800000u,0x7fc00000u,0x7f800001u,
    0xffc12345u,0xffffffffu,0xc2aeac4fu,0xc2aeac50u,0xc2aeac51u,
    0xc2d00000u,0xc2d00001u,0xc2cfffffu,0x42b17217u,0x42b17218u,0x42b17219u,0x7f7fffffu};
  template<simd::isa A,std::size_t L,std::size_t N,bool Flush,bool Raw>
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
      if constexpr(Raw) {
        evaluate_raw<true,Flush,L,N,A>(input.data(),reference.data());
        evaluate_raw<false,Flush,L,N,A>(input.data(),candidate.data());
      } else {
        evaluate<true,Flush,L,N,A>(input.data(),reference.data());
        evaluate<false,Flush,L,N,A>(input.data(),candidate.data());
      }
      if(reference!=candidate) {
        std::printf("mismatch L=%zu N=%zu Flush=%d seed=%zu\n",L,N,int(Flush),seed);
        return false;
      }
    }
    return true;
  }
  template<simd::isa A,std::size_t L,bool Raw> bool compare_width() {
    return compare_shape<A,L,0,false,Raw>() && compare_shape<A,L,1,false,Raw>() &&
      compare_shape<A,L,3,false,Raw>() && compare_shape<A,L,0,true,Raw>() &&
      compare_shape<A,L,1,true,Raw>() && compare_shape<A,L,3,true,Raw>();
  }
  template<simd::isa A,bool Raw=false> bool compare_case() {
    bool equal=compare_width<A,1,Raw>() && compare_width<A,2,Raw>() &&
      compare_width<A,3,Raw>() && compare_width<A,4,Raw>() && compare_width<A,8,Raw>();
    if constexpr(A.has(SIMD_TARGET_ISA(exp_base)))
      equal=equal && compare_width<A,16,Raw>();
    return equal;
  }
}

int main() {
  if(!custom_exp::check()) return 4;
  auto cpu=simd::observe_x86_capabilities();
  auto controls=_mm_getcsr()&~0x3fu;
  unsigned executed=0,skipped=0;
#define RUN_EXP_CALLER(i,name,raw) \
  if(simd::classify_isa(cpu,SIMD_TARGET_ISA(name),SIMD_TARGET_MINIMUM).admitted()) { \
    if(!refinement_test::compare_case<SIMD_TARGET_ISA(name)>()) return 1; \
    ++executed;std::printf("exp caller %u %s: executed\n",i,#name); \
  } else { ++skipped;std::printf("exp caller %u %s: skipped (not admitted)\n",i,#name); }
  EXP_CALLER_CASES(RUN_EXP_CALLER)
#undef RUN_EXP_CALLER
  unsigned raw_executed=0,raw_skipped=0;
#define RUN_RAW_CALLER(i,name,raw) \
  if(simd::classify_isa(cpu,simd::abi_lookup<SIMD_TARGET_ISA(name),refinement_test::exp_policies>::architecture,SIMD_TARGET_MINIMUM).admitted()) { \
    if(!refinement_test::compare_case<SIMD_TARGET_ISA(name),true>()) return 5; \
    ++raw_executed;std::printf("raw scope for caller %u %s: executed\n",i,#name); \
  } else { ++raw_skipped;std::printf("raw scope for caller %u %s: skipped (not admitted)\n",i,#name); }
  EXP_CALLER_CASES(RUN_RAW_CALLER)
#undef RUN_RAW_CALLER
  std::printf("%u raw-scope caller cases executed, %u skipped\n",raw_executed,raw_skipped);
  constexpr auto extra=simd::feature_closure(simd::avx2&simd::feature::aes);
  if(simd::classify_isa(cpu,extra,SIMD_TARGET_MINIMUM).admitted()) {
    if(!refinement_test::compare_case<extra>() || !refinement_test::compare_case<extra,true>()) return 2;
    std::puts("AVX2 caller with extra AES tag: original type retained, outputs exact");
  }
  if((_mm_getcsr()&~0x3fu)!=controls) return 3;
  std::printf("%u caller cases executed, %u skipped; MXCSR controls unchanged: 0x%x\n",executed,skipped,controls);
  return executed?0:77;
}
