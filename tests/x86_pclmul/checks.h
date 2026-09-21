// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
namespace pclmul_fixture {
using native::x86_feature;
constexpr auto legacy = native::feature_closure(native::isa<native::x86>{x86_feature::pclmul});
constexpr auto vex128 = native::feature_closure(legacy & x86_feature::avx);
constexpr auto vex256 = native::feature_closure(x86_feature::vpclmulqdq & x86_feature::avx);
constexpr auto evex512 = native::feature_closure(x86_feature::vpclmulqdq & x86_feature::avx512f);
static_assert(vex256.has(x86_feature::pclmul)); // Compiler prerequisite closure.
static_assert(!vex256.has(x86_feature::avx2));
static_assert(!evex512.has(x86_feature::avx512vl));

#include "api_checks.h"

using word = std::uint64_t;
template<std::size_t N> using words = std::array<word,N>;
template<std::size_t N> struct inputs { words<N> a, b; };
template<std::size_t N, std::size_t K> using outputs = std::array<words<N>,K>;
using selectors = std::integer_sequence<unsigned,0,1,16,17>;
using all_immediates = std::make_integer_sequence<unsigned,256>;

// Schoolbook multiplication over GF(2): XOR shifted copies of a for set
// coefficients in b. Split each shift across two words without a shift by 64.
constexpr words<2> polynomial(word a, word b) {
  words<2> product{};
  for (unsigned i=0; i<64; ++i) if ((b >> i) & 1) {
    product[0] ^= a << i;
    if (i) product[1] ^= a >> (64-i);
  }
  return product;
}
static_assert(polynomial(word{1}<<63,word{1}<<63) == words<2>{0,word{1}<<62});
static_assert(polynomial(~word{0},~word{0}) == words<2>{0x5555555555555555,0x5555555555555555});
static_assert(polynomial(3,3) == words<2>{5,0});

template<unsigned... I>
native_noinline native_target("pclmul")
void evaluate_legacy(inputs<2> const& in, outputs<2,sizeof...(I)>& out,
    std::integer_sequence<unsigned,I...>) {
  __m128i a, b;
  __builtin_memcpy(&a,in.a.data(),sizeof(a));
  __builtin_memcpy(&b,in.b.data(),sizeof(b));
  unsigned index=0;
  ([&] native_target("pclmul") {
    auto value=native::pclmulqdq<legacy,I>(native::simd<std::uint64_t, sizeof(a) / sizeof(std::uint64_t), legacy>::from_native(a), native::simd<std::uint64_t, sizeof(b) / sizeof(std::uint64_t), legacy>::from_native(b)).to_native();
    __builtin_memcpy(out[index++].data(),&value,sizeof(value));
  }(), ...);
}
template<unsigned... I>
native_noinline native_target("avx,pclmul")
void evaluate_vex128(inputs<2> const& in, outputs<2,sizeof...(I)>& out,
    std::integer_sequence<unsigned,I...>) {
  __m128i a, b;
  __builtin_memcpy(&a,in.a.data(),sizeof(a));
  __builtin_memcpy(&b,in.b.data(),sizeof(b));
  unsigned index=0;
  ([&] native_target("avx,pclmul") {
    auto value=native::vpclmulqdq<vex128,I>(native::simd<std::uint64_t, sizeof(a) / sizeof(std::uint64_t), vex128>::from_native(a), native::simd<std::uint64_t, sizeof(b) / sizeof(std::uint64_t), vex128>::from_native(b)).to_native();
    __builtin_memcpy(out[index++].data(),&value,sizeof(value));
  }(), ...);
}
template<unsigned... I>
native_noinline native_target("avx,vpclmulqdq")
void evaluate_vex256(inputs<4> const& in, outputs<4,sizeof...(I)>& out,
    std::integer_sequence<unsigned,I...>) {
  __m256i a, b;
  __builtin_memcpy(&a,in.a.data(),sizeof(a));
  __builtin_memcpy(&b,in.b.data(),sizeof(b));
  unsigned index=0;
  ([&] native_target("avx,vpclmulqdq") {
    auto value=native::vpclmulqdq<vex256,I>(native::simd<std::uint64_t, sizeof(a) / sizeof(std::uint64_t), vex256>::from_native(a), native::simd<std::uint64_t, sizeof(b) / sizeof(std::uint64_t), vex256>::from_native(b)).to_native();
    __builtin_memcpy(out[index++].data(),&value,sizeof(value));
  }(), ...);
}
template<unsigned... I>
native_noinline native_target("avx512f,vpclmulqdq")
void evaluate_evex512(inputs<8> const& in, outputs<8,sizeof...(I)>& out,
    std::integer_sequence<unsigned,I...>) {
  __m512i a, b;
  __builtin_memcpy(&a,in.a.data(),sizeof(a));
  __builtin_memcpy(&b,in.b.data(),sizeof(b));
  unsigned index=0;
  ([&] native_target("avx512f,vpclmulqdq") {
    auto value=native::vpclmulqdq<evex512,I>(native::simd<std::uint64_t, sizeof(a) / sizeof(std::uint64_t), evex512>::from_native(a), native::simd<std::uint64_t, sizeof(b) / sizeof(std::uint64_t), evex512>::from_native(b)).to_native();
    __builtin_memcpy(out[index++].data(),&value,sizeof(value));
  }(), ...);
}

inline word next(word& state) {
  state ^= state << 13; state ^= state >> 7; state ^= state << 17;
  return state;
}

template<std::size_t N, unsigned... I, class Evaluate>
bool check(inputs<N> const& input, Evaluate evaluate, std::integer_sequence<unsigned,I...> sequence) {
  outputs<N,sizeof...(I)> actual{};
  evaluate(input,actual,sequence);
  // Four oracle products per lane suffice even when checking all 256 encodings.
  std::array<words<N>,4> expected{};
  for (unsigned selector=0; selector<4; ++selector)
    for (std::size_t lane=0; lane<N/2; ++lane) {
      auto product=polynomial(input.a[2*lane+(selector&1)],input.b[2*lane+(selector>>1)]);
      expected[selector][2*lane]=product[0];
      expected[selector][2*lane+1]=product[1];
    }
  unsigned index=0;
  for (auto immediate : {I...}) {
    auto const& wanted=expected[(immediate&1)|((immediate>>3)&2)];
    if (actual[index++] != wanted) {
      std::printf("FAIL width=%zu immediate=%u\n",N*64,immediate);
      return false;
    }
  }
  return true;
}

template<std::size_t N, class Four, class All>
bool check_width(Four four, All all) {
  inputs<N> input{};
  constexpr word patterns[]{0,1,2,3,word{1}<<63,~word{0},0xaaaaaaaaaaaaaaaa,0x5555555555555555};
  for (word a:patterns) for (word b:patterns) {
    for (std::size_t i=0;i<N;++i) { input.a[i]=a; input.b[i]=b; }
    if (!check(input,all,all_immediates{})) return false;
  }
  // Every one-hot pair for every selector in every lane. Distinct high halves
  // and lane offsets make half swaps and lane reuse visible.
  for (unsigned a=0;a<64;++a) for (unsigned b=0;b<64;++b) {
    for (std::size_t i=0;i<N;++i) {
      input.a[i]=word{1}<<((a+7*i)%64);
      input.b[i]=word{1}<<((b+13*i)%64);
    }
    if (!check(input,four,selectors{})) return false;
  }
  // One active lane in each source: products vanish when those lanes differ. Includes high-bit products crossing bit 63.
  for (std::size_t a=0;a<N/2;++a) for (std::size_t b=0;b<N/2;++b) {
    input={};
    input.a[2*a]=~word{0}; input.a[2*a+1]=word{1}<<63;
    input.b[2*b]=word{1}<<63; input.b[2*b+1]=~word{0};
    if (!check(input,all,all_immediates{})) return false;
  }
  word state=0x243f6a8885a308d3;
  for (unsigned iteration=0;iteration<2048;++iteration) {
    for (std::size_t i=0;i<N;++i) { input.a[i]=next(state); input.b[i]=next(state); }
    if (!check(input,four,selectors{})) return false;
    if (iteration<128 && !check(input,all,all_immediates{})) return false;
  }
  return true;
}

int run() {
  auto cpu=native::observe_x86_capabilities();
  std::printf("CPUID.1:ECX=%08x CPUID.7.0:ECX=%08x PCLMUL=%u VPCLMULQDQ=%u AVX=%u AVX512F=%u XCR0 observed=%u value=%016llx\n",
    unsigned(cpu.raw.leaf1_ecx),unsigned(cpu.raw.leaf7_ecx),unsigned(cpu.present.has(x86_feature::pclmul)),
    unsigned(cpu.present.has(x86_feature::vpclmulqdq)),unsigned(cpu.present.has(x86_feature::avx)),
    unsigned(cpu.present.has(x86_feature::avx512f)),unsigned(cpu.xcr0_observed),
    static_cast<unsigned long long>(cpu.xcr0));
  // Presence must match CPUID even when the OS cannot admit the instruction.
  if (cpu.present.has(x86_feature::pclmul) != bool(cpu.raw.leaf1_ecx & (1u<<1)) ||
      cpu.present.has(x86_feature::vpclmulqdq) != bool(cpu.raw.leaf7_ecx & (1u<<10))) return 1;
  unsigned executed=0;
  auto run_one=[&](char const* name, native::isa<native::x86> requirements, auto operation) {
    auto admission=native::classify_isa(cpu,requirements);
    if (!admission.admitted()) { std::printf("SKIP %s: %s\n",name,admission.reason()); return true; }
    auto status=_mm_getcsr();
    if (!operation() || _mm_getcsr()!=status) return false;
    ++executed;
    std::printf("PASS %s: all one-hot pairs, 2048 random inputs, all 256 immediates\n",name);
    return true;
  };
  if (!run_one("legacy",legacy,[] {
    return check_width<2>(evaluate_legacy<0,1,16,17>,
      []<unsigned... I>(inputs<2> const& in, outputs<2,sizeof...(I)>& out, std::integer_sequence<unsigned,I...> seq) {
        evaluate_legacy(in,out,seq);
      });
  })) return 1;
  if (!run_one("vex128",vex128,[] {
    return check_width<2>(evaluate_vex128<0,1,16,17>,
      []<unsigned... I>(inputs<2> const& in, outputs<2,sizeof...(I)>& out, std::integer_sequence<unsigned,I...> seq) {
        evaluate_vex128(in,out,seq);
      });
  })) return 1;
  if (!run_one("vex256",vex256,[] {
    return check_width<4>(evaluate_vex256<0,1,16,17>,
      []<unsigned... I>(inputs<4> const& in, outputs<4,sizeof...(I)>& out, std::integer_sequence<unsigned,I...> seq) {
        evaluate_vex256(in,out,seq);
      });
  })) return 1;
  if (!run_one("evex512",evex512,[] {
    return check_width<8>(evaluate_evex512<0,1,16,17>,
      []<unsigned... I>(inputs<8> const& in, outputs<8,sizeof...(I)>& out, std::integer_sequence<unsigned,I...> seq) {
        evaluate_evex512(in,out,seq);
      });
  })) return 1;
  return executed ? 0 : 77;
}
}
