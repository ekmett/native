// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/attributes.h>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "../property_check.h"
#if defined(_MSC_VER)
#include <intrin.h>
#endif
#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#endif
import native;

#if defined(__aarch64__) || defined(_M_ARM64)
constexpr auto half_arch=native::neon_fp16;
constexpr auto bf_arch=native::neon_bf16;
constexpr std::size_t half_lanes=8;
#define half_target native_target("neon,fullfp16")
#define bf_target native_target("neon,bf16")
#else
constexpr auto half_arch=native::avx512_fp16;
constexpr auto bf_arch=native::avx512_bf16;
constexpr std::size_t half_lanes=32;
#define half_target native_target("avx2,fma,avx512f,avx512dq,avx512bw,avx512vl,avx512fp16")
#define bf_target native_target("avx2,fma,avx512f,avx512dq,avx512bw,avx512vl,avx512bf16")
#endif
using H=native::simd<native::fp16,half_lanes,half_arch>;
struct half_case { std::array<std::uint16_t,half_lanes> a{},b{},c{}; };
struct half_result {
  std::array<std::array<std::uint16_t,half_lanes>,8> words{};
  std::array<std::uint64_t,6> masks{};
};
constexpr std::array<std::uint16_t,18> boundaries{
  0,0x8000,1,0x8001,0x3ff,0x400,0x3bff,0x3c00,0x3c01,
  0x7bff,0x7c00,0xfc00,0x7c01,0x7e55,0xfe55,0xbc00,0x3555,0x8400};
constexpr auto half_cases=native_test::property_cases<64>(native_test::property_seed,[](auto & rng) {
  half_case c;
  for(std::size_t i=0;i<half_lanes;++i) {
    auto next=[&] { return rng.index(2) ? boundaries[rng.index(boundaries.size())] : rng.template bits<std::uint16_t>(); };
    c.a[i]=next(); c.b[i]=next(); c.c[i]=next();
  }
  return c;
});
#define HALF_EVALUATE \
  auto a=H::load_bits(c.a.data()),b=H::load_bits(c.b.data()),d=H::load_bits(c.c.data()); \
  half_result result; \
  (a+b).store_bits(result.words[0].data()); \
  (a-b).store_bits(result.words[1].data()); \
  (a*b).store_bits(result.words[2].data()); \
  (a/b).store_bits(result.words[3].data()); \
  sqrt(a).store_bits(result.words[4].data()); \
  native::fma(a,b,d).store_bits(result.words[5].data()); \
  (-a).store_bits(result.words[6].data()); \
  select(a<b,a,b).store_bits(result.words[7].data()); \
  result.masks={(a==b).to_bitset(),(a!=b).to_bitset(),(a<b).to_bitset(), \
    (a<=b).to_bitset(),(a>b).to_bitset(),(a>=b).to_bitset()}; \
  return result
consteval half_result constant_half(half_case const & c) { HALF_EVALUATE; }
half_target native_noinline half_result runtime_half(half_case const & c) { HALF_EVALUATE; }
#undef HALF_EVALUATE
constexpr auto half_expected=[] consteval {
  std::array<half_result,half_cases.size()> result{};
  for(std::size_t i=0;i<result.size();++i) result[i]=constant_half(half_cases[i]);
  return result;
}();
static_assert([] {
  half_case c; c.a.fill(0x3c00); c.b.fill(0x4000); c.c.fill(0x4200);
  auto r=constant_half(c);
  return r.words[0][0]==0x4200 && r.words[1][0]==0xbc00 && r.words[2][0]==0x4000 &&
    r.words[3][0]==0x3800 && r.words[4][0]==0x3c00 && r.words[5][0]==0x4500;
}());

template<std::size_t N> struct dot_case {
  std::array<std::uint16_t,N> a{},b{};
  std::array<std::uint32_t,N/2> c{};
};
template<std::size_t N> constexpr auto dot_cases=native_test::property_cases<64>(0x9216d5d98979fb1bull,[](auto & rng) {
  dot_case<N> c;
  constexpr std::array<std::uint16_t,12> special{0,0x8000,1,0x80,0x7f80,0xff80,0x7f81,0x7fc1,0x3f80,0x4000,0xbf80,0x7f7f};
  for(std::size_t i=0;i<N;++i) {
    c.a[i]=rng.index(2)?special[rng.index(special.size())]:rng.template bits<std::uint16_t>();
    c.b[i]=rng.index(2)?special[rng.index(special.size())]:rng.template bits<std::uint16_t>();
  }
  for(auto & x:c.c) x=rng.template bits<std::uint32_t>();
  return c;
});
#define DOT_EVALUATE \
  using B=native::simd<native::bf16,N,bf_arch>; \
  using V=native::simd<float,N/2,bf_arch>; \
  std::array<std::uint32_t,N/2> result{}; \
  native::dot2(B::load_bits(c.a.data()),B::load_bits(c.b.data()),V::load_bits(c.c.data())).store_bits(result.data()); \
  return result
template<std::size_t N> consteval auto constant_dot(dot_case<N> const & c) { DOT_EVALUATE; }
template<std::size_t N> bf_target native_noinline auto runtime_dot(dot_case<N> const & c) { DOT_EVALUATE; }
#undef DOT_EVALUATE
template<std::size_t N> constexpr auto dot_expected=[] consteval {
  std::array<std::array<std::uint32_t,N/2>,64> result{};
  for(std::size_t i=0;i<result.size();++i) result[i]=constant_dot(dot_cases<N>[i]);
  return result;
}();

bool nan(std::uint16_t x) { return (x&0x7c00)==0x7c00 && (x&0x3ff); }
bool half_properties() {
  for(std::size_t i=0;i<half_cases.size();++i) {
    auto actual=runtime_half(half_cases[i]); auto const & expected=half_expected[i];
    for(std::size_t op=0;op<8;++op) for(std::size_t lane=0;lane<half_lanes;++lane) {
      auto a=actual.words[op][lane],e=expected.words[op][lane];
      // Arithmetic NaN payload choice is instruction/compiler dependent; bit
      // transport, negation and selection are checked exactly.
      if(!native_test::property_check("half constexpr/native",native_test::property_seed,i,
          a==e || (op<6 && nan(a) && nan(e)),op,lane,e,a,
          half_cases[i].a[lane],half_cases[i].b[lane],half_cases[i].c[lane])) return false;
    }
    if(!native_test::property_equal("half comparisons",native_test::property_seed,i,expected.masks,actual.masks)) return false;
  }
  return true;
}
template<std::size_t N> bool dot_properties() {
  for(std::size_t i=0;i<dot_cases<N>.size();++i)
    if(!native_test::property_equal("bf16 dot constexpr/native",0x9216d5d98979fb1bull,i,
        dot_expected<N>[i],runtime_dot(dot_cases<N>[i]),dot_cases<N>[i].a,dot_cases<N>[i].b,dot_cases<N>[i].c)) return false;
  return true;
}
#if defined(__aarch64__) || defined(_M_ARM64)
std::uint64_t fpcr() {
#if defined(_MSC_VER)
  return _ReadStatusReg(ARM64_FPCR);
#else
  std::uint64_t x; __asm__ volatile("mrs %0, fpcr":"=r"(x)); return x;
#endif
}
void fpcr(std::uint64_t x) {
#if defined(_MSC_VER)
  _WriteStatusReg(ARM64_FPCR,x);
#else
  __asm__ volatile("msr fpcr, %0"::"r"(x):"memory");
#endif
}
#endif
int main(int argc,char ** argv) {
  if(argc!=2) return 2;
  if(!std::strcmp(argv[1],"static")) return 0;
  bool half=!std::strcmp(argv[1],"half");
  if(!half && std::strcmp(argv[1],"bf16")) return 2;
#if defined(__aarch64__) || defined(_M_ARM64)
  auto admission=native::classify_isa(native::observe_arm_capabilities(),half?half_arch:bf_arch);
#else
  auto admission=native::classify_isa(native::observe_x86_capabilities(),half?half_arch:bf_arch);
#endif
  if(!admission.admitted()) {std::puts(admission.reason());return admission.invalid_features?1:77;}
#if defined(__aarch64__) || defined(_M_ARM64)
  auto control=fpcr(); fpcr(0);
  bool ok=half?half_properties():dot_properties<8>();
  fpcr(control);
#else
  auto control=_mm_getcsr(); _mm_setcsr(0x1f80);
  bool ok=half?half_properties():(dot_properties<8>() && dot_properties<16>() && dot_properties<32>());
  _mm_setcsr(control);
#endif
  return ok?0:1;
}
