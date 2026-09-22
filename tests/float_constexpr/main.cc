// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/config.h>
#include <native/attributes.h>
#include "../property_check.h"
#include "../core_regression/support/fp_environment.h"
#include <cmath>
#include <cstdio>
#if NATIVE_FLOAT_HEADERS
#include <native/simd/math/exp.h>
#include <native/simd/math/bits.h>
#else
import native.math;
#endif
import native.features;

using namespace native;
namespace fixture {
  template<std::size_t N> using words=std::array<std::uint32_t,N>;
  template<std::size_t N> struct input {words<N> a,b,c;};
  template<std::size_t N> struct output {
    std::array<words<N>,12> values;
    std::array<std::uint64_t,6> masks;
    constexpr bool operator==(output const&) const = default;
  };
#define NATIVE_FLOAT_ARCH scalar
#define NATIVE_FLOAT_TARGET
#include "calculate.h"
#undef NATIVE_FLOAT_ARCH
#undef NATIVE_FLOAT_TARGET
#if NATIVE_HOST_NEON
#define NATIVE_FLOAT_ARCH neon
#define NATIVE_FLOAT_TARGET native_target("neon")
#include "calculate.h"
#undef NATIVE_FLOAT_ARCH
#undef NATIVE_FLOAT_TARGET
#elif NATIVE_HOST_X86
#define NATIVE_FLOAT_ARCH avx2
#define NATIVE_FLOAT_TARGET native_target("avx2,fma")
#include "calculate.h"
#undef NATIVE_FLOAT_ARCH
#undef NATIVE_FLOAT_TARGET
#define NATIVE_FLOAT_ARCH avx512
#define NATIVE_FLOAT_TARGET native_target("avx512f,avx512dq,avx512bw,avx512vl,avx2,fma")
#include "calculate.h"
#undef NATIVE_FLOAT_ARCH
#undef NATIVE_FLOAT_TARGET
#endif
  template<std::size_t N> constexpr auto make_input(native_test::property_rng& rng) {
    input<N> result{};
    for(auto* values:{&result.a,&result.b,&result.c}) for(auto& x:*values) {
      x=rng.bits<std::uint32_t>();
      if((x&0x7f800000u)==0x7f800000u) x^=0x00800000u;
    }
    for(auto& x:result.b) if((x&0x7fffffffu)==0) x|=1;
    return result;
  }
  template<std::size_t N> constexpr auto inputs=native_test::property_cases<16>(native_test::property_seed,make_input<N>);
  template<class V> constexpr auto expected=[] consteval {
    std::array<output<V::lanes>,16> result{};
    for(std::size_t i=0;i<result.size();++i) result[i]=calculate<V>(inputs<V::lanes>[i]);
    return result;
  }();
  template<std::size_t N> constexpr auto math_inputs=native_test::property_cases<4>(native_test::property_seed,[](auto& rng) {
    words<N> result{};
    for(auto& x:result) x=std::bit_cast<std::uint32_t>((float(rng.index(16385))-8192.f)/1024.f);
    return result;
  });
  template<class V,std::size_t I> constexpr auto math_expected_case=math_calculate<V>(math_inputs<V::lanes>[I]);
  template<class V> constexpr auto math_expected=std::array{
    math_expected_case<V,0>,math_expected_case<V,1>,math_expected_case<V,2>,math_expected_case<V,3>};
  template<class V> consteval bool edges() {
    auto zero=V::from_bits(0),minus_zero=V::from_bits(0x80000000u);
    auto infinity=V::from_bits(0x7f800000u),nan=V::from_bits(0x7f812345u);
    if(bits(V(1.f)/zero)[0]!=0x7f800000u || bits(zero/zero)[0]!=0x7fc00000u) return false;
    if(bits(sqrt(V(-1.f)))[0]!=0x7fc00000u || bits(sqrt(minus_zero))[0]!=0x80000000u) return false;
    if(bits(nan+V(1.f))[0]!=0x7fc12345u || any(nan==nan) || !all(nan!=nan)) return false;
    if(!all(zero==minus_zero) || any(infinity<nan)) return false;
    if(bits(round_even(V(-.5f)))[0]!=0x80000000u || bits(round_even(V(2.5f)))[0]!=0x40000000u) return false;
    if(bits(V::from_bits(1)/V(2.f))[0]!=0 || bits(V::from_bits(3)/V(2.f))[0]!=2) return false;
    // One fused rounding retains the low product that separate operations lose.
    if(bits(fma(V::from_bits(0x3f800001u),V::from_bits(0x3f7ffffeu),V(-1.f)))[0]!=0xa8800000u) return false;
    auto x=V(2.f);x+=3.f;x*=2.f;x-=1.f;x/=3.f;
    if(!all(x==3.f) || !all(3.f==x) || !all(fma(2.f,x,-1.f)==5.f)) return false;
    if(!all(convert<float>(convert<std::int32_t>(V(-2.75f)))==-2.f)) return false;
    if constexpr(requires(V a) { scaleb(a,a); }) {
      if(bits(masked_scaleb(typename V::mask_type(false),nan,infinity,infinity))[0]!=0x7f812345u) return false;
      if(bits(scaleb(V::from_bits(1),V(1.f)))[0]!=2) return false;
      if(bits(scaleb(V(1.f),V(-.5f)))[0]!=0x3f000000u) return false;
      if(bits(scaleb(nan,infinity))[0]!=0x7fc12345u) return false;
    }
    std::array<float,V::lanes+1> storage{};storage.back()=99.f;
    auto filled=load_simd_partial<V>(static_cast<float const*>(nullptr),0,-2.f);
    store_simd_partial(storage.data(),filled,V::lanes);
    if(storage[0]!=-2.f || storage.back()!=99.f) return false;
    if(!all(load_simd<V>(storage.data())==-2.f)) return false;
    store_simd_partial(static_cast<float*>(nullptr),filled,0);
    auto empty=::math::sincos(std::array<V,0>{});
    if(!empty.first.empty() || !::math::exp(std::array<V,0>{}).empty()) return false;
    // Exp's exponent conversion must remain defined even though every lane
    // reaches it before the final range selection.
    for(auto word:{0x7f800001u,0x7fc12345u,0xff800001u,0xffc12345u})
      if((bits(::math::exp(V::from_bits(word)))[0]&0x7fffffffu)<=0x7f800000u) return false;
    for(auto word:{0x7f800000u,0x7f7fffffu})
      if(bits(::math::exp(V::from_bits(word)))[0]!=0x7f800000u) return false;
    for(auto word:{0xff800000u,0xff7fffffu})
      if(bits(::math::exp(V::from_bits(word)))[0]!=0) return false;
    return expected<V>.size()==16 && math_expected<V>.size()==4;
  }
  template<std::size_t N> auto oracle(input<N> const& in) {
    output<N> out{};
    for(std::size_t i=0;i<N;++i) {
      volatile float a=std::bit_cast<float>(in.a[i]),b=std::bit_cast<float>(in.b[i]),c=std::bit_cast<float>(in.c[i]);
      std::array<float,12> values{a+b,a-b,a*b,a/b,std::fma(float(a),float(b),float(c)),
        std::sqrt(std::abs(float(a))),std::floor(float(a)),std::ceil(float(a)),std::trunc(float(a)),
        std::nearbyint(float(a)),-a,a<b?a:b};
      for(std::size_t j=0;j<12;++j) out.values[j][i]=std::bit_cast<std::uint32_t>(values[j]);
      std::array<bool,6> masks{a<b,a>b,a==b,a!=b,a<=b,a>=b};
      for(std::size_t j=0;j<6;++j) out.masks[j]|=std::uint64_t(masks[j])<<i;
    }
    return out;
  }
  template<class V> native_inline bool run() {
    static_assert(edges<V>());
    for(std::size_t i=0;i<16;++i) {
      auto actual=calculate<V>(inputs<V::lanes>[i]);
      if(!native_test::property_equal("float constexpr/native",native_test::property_seed,i,
          expected<V>[i].values,actual.values,inputs<V::lanes>[i].a,inputs<V::lanes>[i].b,inputs<V::lanes>[i].c) ||
        !native_test::property_equal("float comparison constexpr/native",native_test::property_seed,i,expected<V>[i].masks,actual.masks)) return false;
    }
    for(std::size_t i=0;i<4;++i) {
      auto actual=math_calculate<V>(math_inputs<V::lanes>[i]);
      if(!native_test::property_equal("float math constexpr/native",native_test::property_seed,i,
          math_expected<V>[i],actual,math_inputs<V::lanes>[i])) return false;
    }
    if constexpr(V::lanes==2 || V::lanes==3) {
      // Zero physical padding must never execute 0/0 during logical division.
      input<V::lanes> in{};
      volatile std::uint32_t six=0x40c00000u,two=0x40000000u;
      in.a.fill(std::uint32_t(six));in.b.fill(std::uint32_t(two));in.c.fill(std::uint32_t(two));
      std::feclearexcept(FE_ALL_EXCEPT);
      auto divided=calculate<V>(in).values[3];
      if(std::fetestexcept(FE_INVALID|FE_DIVBYZERO) || divided[0]!=0x40400000u) return false;
    }
    auto settings=native_test::property_config();native_test::property_rng rng{settings.seed};
    for(std::size_t i=0;i<settings.cases;++i) {
      auto in=make_input<V::lanes>(rng);auto want=oracle(in),actual=calculate<V>(in);
      if(!native_test::property_equal("float scalar oracle",settings.seed,i,want.values,actual.values,in.a,in.b,in.c) ||
         !native_test::property_equal("float comparison oracle",settings.seed,i,want.masks,actual.masks,in.a,in.b)) return false;
    }
    return true;
  }
}
using S=simd<float,1,scalar>;
static_assert(fixture::edges<S>());
static_assert(::math::exp(0.f)==1.f && ::math::sin(0.f)==0.f && ::math::cos(0.f)==1.f);
#if NATIVE_HOST_NEON
static_assert(fixture::edges<simd<float,2,neon>>());
static_assert(fixture::edges<simd<float,3,neon>>());
static_assert(fixture::edges<simd<float,4,neon>>());
native_target("neon") bool native_run() {
  return fixture::run<simd<float,2,neon>>() && fixture::run<simd<float,3,neon>>() && fixture::run<simd<float,4,neon>>();
}
#elif NATIVE_HOST_X86
static_assert(fixture::edges<simd<float,2,avx2>>());
static_assert(fixture::edges<simd<float,3,avx2>>());
static_assert(fixture::edges<simd<float,4,avx2>>());
static_assert(fixture::edges<simd<float,8,avx2>>());
static_assert(fixture::edges<simd<float,16,avx512>>());
native_target("avx2,fma") bool avx2_run() {
  return fixture::run<simd<float,2,avx2>>() && fixture::run<simd<float,3,avx2>>() && fixture::run<simd<float,4,avx2>>() && fixture::run<simd<float,8,avx2>>();
}
native_target("avx512f,avx512dq,avx512bw,avx512vl,avx2,fma") bool avx512_run() {
  return fixture::run<simd<float,4,avx512>>() && fixture::run<simd<float,8,avx512>>() && fixture::run<simd<float,16,avx512>>();
}
#endif
int main() {
  native::test::fp_scope environment(native::test::fp_mode::gradual);
  if(!fixture::run<S>()) return 1;
#if NATIVE_HOST_NEON
  auto admission=classify_isa(observe_arm_capabilities(),neon);
  if(admission.admitted()) { if(!native_run()) return 1; }
  else if(admission.invalid_features) return 1;
  else std::puts("SKIP NEON runtime: unsupported or unknown capabilities");
#elif NATIVE_HOST_X86
  auto cpu=observe_x86_capabilities();
  auto a=classify_isa(cpu,avx2),b=classify_isa(cpu,avx512);
  if(a.invalid_features || b.invalid_features) return 1;
  if(a.admitted()) {if(!avx2_run()) return 1;} else std::puts("SKIP AVX2 runtime: unsupported or unknown capabilities");
  if(b.admitted()) {if(!avx512_run()) return 1;} else std::puts("SKIP AVX-512 runtime: unsupported or unknown capabilities");
#endif
  return environment.controls_match()?0:1;
}
