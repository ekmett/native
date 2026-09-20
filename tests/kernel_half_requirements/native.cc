// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/targets.h>
#include <array>
#include <cstdio>
#include <type_traits>
import native;

template<class V> concept can_load=requires(float const * p) { native::load_simd<V>(p); };
template<class V> concept can_store=requires(float * p,V value) { native::store_simd(p,value); };
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
struct malformed_constant {
  static constexpr int architecture=0;
  template<std::size_t> static malformed_constant load_memory(float const *) { return {}; }
  template<std::size_t> void store_memory(float *) const {}
};
static_assert(!can_load<malformed_constant> && !can_store<malformed_constant>);

// A foreign value may use this member name for ordinary data. It must remain
// on wide's generic ADL path rather than opting into compile-time ISA routing.
namespace foreign {
  struct value {
    native::isa architecture{};
    int number=0;
    friend constexpr value operator+(value a,value b) noexcept { return {{},a.number+b.number}; }
  };
  struct mutable_value {
    inline static native::isa architecture{};
    int number=0;
    friend constexpr mutable_value operator+(mutable_value a,mutable_value b) noexcept { return {a.number+b.number}; }
  };
}
static_assert(!can_load<foreign::value> && !can_store<foreign::value> &&
  !can_load<foreign::mutable_value> && !can_store<foreign::mutable_value>);
static_assert([] {
  native::wide<foreign::value,1> a{foreign::value{{},3}};
  native::wide<foreign::mutable_value,1> b{foreign::mutable_value{4}};
  return (a+a).registers[0].number==6 && (b+b).registers[0].number==8;
}());

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
constexpr auto advertised=NATIVE_TARGET_ISA(FP_TARGET)&NATIVE_TARGET_ISA(BF_TARGET);
using raw=native::vec<float,lanes,advertised>;
using half=native::vec<native::fp16,lanes*2,advertised>;
using brain=native::vec<native::bf16,lanes*2,advertised>;
static_assert(raw::architecture==advertised);
static_assert(half::architecture==advertised);
static_assert(brain::architecture==advertised);

// Each function is compiled without the other advertised extension. Compiling
// these actual bodies catches an always-inline callee with an excessive target.
NATIVE_TARGET_PUSH(RAW_TARGET)
__attribute__((noinline)) bool check_raw() {
  using pack=native::wide<raw,2>;
  std::array<float,lanes+1> input{},output{};
  input.fill(4.f);
  auto a=native::load_simd<raw>(input.data());
  auto qualified=native::load_simd<raw const>(input.data());
  (void)qualified;
  pack x{a,a},y{raw(2.f),raw(2.f)},z{raw(1.f),raw(1.f)};
  auto result=floor(sqrt(abs(fma(x,y,z))));
  auto masks=result==native::broadcast<raw,2>(raw(3.f));
  static_assert(raw::mask::architecture==advertised);
  auto chosen=select(~masks,pack::broadcast(raw(0.f)),result);
  native::wide<raw,0> empty{};
  native::wide<raw::mask,0> empty_masks{};
  (void)empty;(void)empty_masks;
  for(auto const & value:chosen.registers) {
    native::store_simd(output.data(),value);
    for(std::size_t i=0;i<lanes;++i) if(output[i]!=3.f) return false;
  }
  for(std::size_t count: {std::size_t(0),std::size_t(1),lanes-1,lanes}) {
    output.fill(-1.f);
    auto value=native::load_simd_partial<raw>(input.data(),count,2.f);
    native::store_simd_partial(output.data(),value,count);
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
NATIVE_TARGET_POP()

NATIVE_TARGET_PUSH(FP_TARGET)
__attribute__((noinline)) bool check_fp16() {
  std::array<native::fp16,lanes*2> values{};
  values.fill(native::fp16::from_bits(0x4400)); // 4
  auto v=native::load_simd<half>(values.data());
  auto result=sqrt(native::wide<half,2>{v,v});
  for(auto const & value:result.registers) {
    native::store_simd(values.data(),value);
    for(auto lane:values) if(lane.to_bits()!=0x4000) return false; // 2
  }
  return true;
}
NATIVE_TARGET_POP()

NATIVE_TARGET_PUSH(BF_TARGET)
__attribute__((noinline)) bool check_bf16() {
  std::array<native::bf16,lanes*2> values{};
  values.fill(native::bf16::from_bits(0x3f80)); // 1
  auto v=native::load_simd<brain>(values.data());
  auto pack=native::wide<brain,2>::broadcast(v);
  auto result=dot2(pack.registers[0],pack.registers[1],raw(0.f));
  std::array<float,lanes> output{};
  native::store_simd(output.data(),result);
  for(float lane:output) if(lane!=2.f) return false;
  native::store_simd(values.data(),pack.registers[0]);
  for(auto lane:values) if(lane.to_bits()!=0x3f80) return false;
  return true;
}
NATIVE_TARGET_POP()

int main() {
#if defined(__x86_64__) || defined(_M_X64)
  auto cpu=native::observe_x86_capabilities();
#else
  auto cpu=native::observe_arm_capabilities();
#endif
  int executed=0;
#define RUN(target,body) \
  if(native::classify_isa(cpu,NATIVE_TARGET_ISA(target),NATIVE_TARGET_MINIMUM).admitted()) { \
    ++executed;if(!body()) return 1;std::puts(#body ": pass"); \
  } else std::puts(#body ": CPU/OS skip");
  RUN(RAW_TARGET,check_raw)
  RUN(FP_TARGET,check_fp16)
  RUN(BF_TARGET,check_bf16)
#undef RUN
  return executed?0:77;
}
