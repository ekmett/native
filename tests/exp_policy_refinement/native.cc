// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "policies.h"
#include <array>
#include <bit>
#include <cstdio>
#include <cstdint>
#include <utility>
#include <immintrin.h>
import native;
import native.math;

static_assert(NATIVE_TARGET_MINIMUM<=native::avx2,
  "This exp refinement check requires a translation-unit minimum no stronger than AVX2/FMA");
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
  template<class T> concept has_flush=requires(native::wide<T,2> const & x) { native::exp<true>(x); };
  static_assert(has_flush<tagged> && !has_flush<plain>);
  static_assert(noexcept(native::exp(native::wide<tagged,2>{})));
  static_assert(!noexcept(native::exp(native::wide<throwing,2>{})));
  bool check() {
    auto a=native::exp<true>(native::wide<tagged,2>{tagged{4},tagged{5}});
    auto b=native::exp(native::wide<plain,2>{plain{4},plain{5}});
    auto c=native::exp(native::wide<float,2>{0.f,0.f});
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
  template<bool Reference,bool Flush,std::size_t L,std::size_t N,native::isa A> \
    requires (native::abi_lookup<A,policies>::index == i) \
  __attribute__((noinline)) void function(std::uint32_t const * input,std::uint32_t * output) { \
    using V=native::simd<float,L,A>; \
    auto values=[&]<std::size_t... K>(std::index_sequence<K...>) { \
      if constexpr(N==0) return native::wide<V,0>{std::array<V,0>{{}}}; \
      else return native::wide<V,N>{V::load_bits(input+K*L)...}; \
    }(std::make_index_sequence<N>{}); \
    static_assert(noexcept(native::exp<Flush>(values))==noexcept(native::exp<Flush,6,V,N>(values))); \
    using W=native::wide<V,N>; \
    using F=W (*)(W const &); \
    static_assert(static_cast<F>(&native::exp<Flush>)==static_cast<F>(&native::exp<Flush,6,L,N,A>)); \
    static_assert(static_cast<F>(&native::math::exp<Flush>)==static_cast<F>(&native::exp<Flush>)); \
    static_assert(static_cast<F>(&native::exp<Flush>)!=static_cast<F>(&native::exp<Flush,6,V,N>)); \
    auto computed=[&] { \
      if constexpr(Reference) return native::exp<Flush,6,V,N>(values); \
      else return native::exp<Flush>(values); \
    }(); \
    static_assert(std::same_as<decltype(computed),native::wide<V,N>>); \
    for(std::size_t k=0;k<N;++k) computed.registers[k].store_bits(output+k*L); \
  }

#define EMIT_EXP_CALLER(i,name,raw) \
  NATIVE_TARGET_PUSH(name) \
  EXP_EVALUATE(evaluate,caller_targets,i) \
  extern "C" __attribute__((noinline)) void refined_codegen_##name##_narrow(float const * input,float * output) { \
    using V=native::simd<float,8,NATIVE_TARGET_ISA(name)>; \
    auto value=native::exp(native::wide<V,2>{V::load(input),V::load(input+8)}); \
    value.registers[0].store(output);value.registers[1].store(output+8); \
  } \
  extern "C" __attribute__((noinline)) void refined_codegen_##name##_native(float const * input,float * output) { \
    constexpr std::size_t lanes=NATIVE_TARGET_ISA(name).has(NATIVE_TARGET_ISA(exp_base))?16:8; \
    using V=native::simd<float,lanes,NATIVE_TARGET_ISA(name)>; \
    auto value=native::exp(native::wide<V,2>{V::load(input),V::load(input+lanes)}); \
    value.registers[0].store(output);value.registers[1].store(output+lanes); \
  } \
  NATIVE_TARGET_POP()
  EXP_CALLER_CASES(EMIT_EXP_CALLER)
#undef EMIT_EXP_CALLER

  // These bodies retain all caller Arch bits but have no BF16/FP16 attribute.
  // Both production and legacy reference must inline under that actual scope.
#define EMIT_RAW_EXP(i,name) \
  NATIVE_TARGET_PUSH(name) \
  EXP_EVALUATE(evaluate_raw,exp_policies,i) \
  NATIVE_TARGET_POP()
  NATIVE_EXP_TARGETS(EMIT_RAW_EXP)
#undef EMIT_RAW_EXP
#undef EXP_EVALUATE

  constexpr std::array<std::uint32_t,24> special{
    0,0x80000000u,1,0x80000001u,0x007fffffu,0x00800000u,
    0x3f800000u,0xbf800000u,0x7f800000u,0xff800000u,0x7fc00000u,0x7f800001u,
    0xffc12345u,0xffffffffu,0xc2aeac4fu,0xc2aeac50u,0xc2aeac51u,
    0xc2d00000u,0xc2d00001u,0xc2cfffffu,0x42b17217u,0x42b17218u,0x42b17219u,0x7f7fffffu};
  template<native::isa A,std::size_t L,std::size_t N,bool Flush,bool Raw>
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
  template<native::isa A,std::size_t L,bool Raw> bool compare_width() {
    return compare_shape<A,L,0,false,Raw>() && compare_shape<A,L,1,false,Raw>() &&
      compare_shape<A,L,3,false,Raw>() && compare_shape<A,L,0,true,Raw>() &&
      compare_shape<A,L,1,true,Raw>() && compare_shape<A,L,3,true,Raw>();
  }
  template<native::isa A,bool Raw=false> bool compare_case() {
    bool equal=compare_width<A,1,Raw>() && compare_width<A,2,Raw>() &&
      compare_width<A,3,Raw>() && compare_width<A,4,Raw>() && compare_width<A,8,Raw>();
    if constexpr(A.has(NATIVE_TARGET_ISA(exp_base)))
      equal=equal && compare_width<A,16,Raw>();
    return equal;
  }
}

int main() {
  if(!custom_exp::check()) return 4;
  auto cpu=native::observe_x86_capabilities();
  auto controls=_mm_getcsr()&~0x3fu;
  unsigned executed=0,skipped=0;
#define RUN_EXP_CALLER(i,name,raw) \
  if(native::classify_isa(cpu,NATIVE_TARGET_ISA(name),NATIVE_TARGET_MINIMUM).admitted()) { \
    if(!refinement_test::compare_case<NATIVE_TARGET_ISA(name)>()) return 1; \
    ++executed;std::printf("exp caller %u %s: executed\n",i,#name); \
  } else { ++skipped;std::printf("exp caller %u %s: skipped (not admitted)\n",i,#name); }
  EXP_CALLER_CASES(RUN_EXP_CALLER)
#undef RUN_EXP_CALLER
  unsigned raw_executed=0,raw_skipped=0;
#define RUN_RAW_CALLER(i,name,raw) \
  if(native::classify_isa(cpu,native::abi_lookup<NATIVE_TARGET_ISA(name),refinement_test::exp_policies>::architecture,NATIVE_TARGET_MINIMUM).admitted()) { \
    if(!refinement_test::compare_case<NATIVE_TARGET_ISA(name),true>()) return 5; \
    ++raw_executed;std::printf("raw scope for caller %u %s: executed\n",i,#name); \
  } else { ++raw_skipped;std::printf("raw scope for caller %u %s: skipped (not admitted)\n",i,#name); }
  EXP_CALLER_CASES(RUN_RAW_CALLER)
#undef RUN_RAW_CALLER
  std::printf("%u raw-scope caller cases executed, %u skipped\n",raw_executed,raw_skipped);
  constexpr auto extra=native::feature_closure(native::avx2&native::x86_feature::aes);
  if(native::classify_isa(cpu,extra,NATIVE_TARGET_MINIMUM).admitted()) {
    if(!refinement_test::compare_case<extra>() || !refinement_test::compare_case<extra,true>()) return 2;
    std::puts("AVX2 caller with extra AES tag: original type retained, outputs exact");
  }
  if((_mm_getcsr()&~0x3fu)!=controls) return 3;
  std::printf("%u caller cases executed, %u skipped; MXCSR controls unchanged: 0x%x\n",executed,skipped,controls);
  return executed?0:77;
}
