// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/detail/constexpr_float.h>
#include "../property_check.h"
#include <cfenv>
#include <cmath>
#include <limits>

#pragma STDC FENV_ACCESS ON
namespace cf=native::detail::constexpr_float;
using cf::binary16;
using cf::bfloat16;
using cf::binary32;
using cf::binary64;
using cf::rounding;

static_assert(cf::div_bits<binary32>(0x3f800000,0x40400000)==0x3eaaaaab);
static_assert(cf::div_bits<binary32>(0x3f800000,0x40400000,rounding::downward)==0x3eaaaaaa);
static_assert(cf::div_bits<binary32>(0xbf800000,0x40400000,rounding::downward)==0xbeaaaaab);
static_assert(cf::div_bits<binary32>(1,0x40000000)==0);
static_assert(cf::div_bits<binary32>(1,0x40000000,rounding::upward)==1);
static_assert(cf::div_bits<binary32>(1,0x40000000,rounding::to_odd)==1);
static_assert(cf::div_bits<binary32>(0x80000000,0x40000000)==0x80000000);
static_assert(cf::div_bits<binary32>(0x3f800000,0x80000000)==0xff800000);
static_assert(cf::div_bits<binary32>(0x3f800000,0xff800000)==0x80000000);
static_assert(cf::div_bits<binary32>(0x7f800000,0x7f800000)==0x7fc00000);
static_assert(cf::div_bits<binary32>(0,0)==0x7fc00000);
static_assert(cf::div_bits<binary32>(0x7fc12345,0x7f812345)==0x7fc12345);
static_assert(cf::div_bits<binary64>(0x3ff0000000000000ULL,0x4008000000000000ULL)==0x3fd5555555555555ULL);
static_assert(cf::sqrt_bits<binary32>(0x40000000)==0x3fb504f3);
static_assert(cf::sqrt_bits<binary32>(0x40000000,rounding::upward)==0x3fb504f4);
static_assert(cf::sqrt_bits<binary32>(0x80000000)==0x80000000);
static_assert(cf::sqrt_bits<binary32>(0xbf800000)==0x7fc00000);
static_assert(cf::sqrt_bits<binary32>(0x7f800000)==0x7f800000);
static_assert(cf::sqrt_bits<binary32>(0xff812345)==0xffc12345);
static_assert(cf::sqrt_bits<binary64>(0x4000000000000000ULL)==0x3ff6a09e667f3bcdULL);
static_assert(cf::sqrt_bits<binary64>(1)==0x1e60000000000000ULL);
static_assert(cf::sqrt_bits<binary16>(0x4000)==0x3da8);
static_assert(cf::sqrt_bits<bfloat16>(0x4000)==0x3fb5);
static_assert(cf::round_integral_bits<binary32>(0x3f000000)==0);
static_assert(cf::round_integral_bits<binary32>(0xbf000000)==0x80000000);
static_assert(cf::round_integral_bits<binary32>(0x3fc00000)==0x40000000);
static_assert(cf::round_integral_bits<binary32>(0x40200000)==0x40000000);
static_assert(cf::round_integral_bits<binary32>(0x80000001,rounding::downward)==0xbf800000);
static_assert(cf::round_integral_bits<binary32>(1,rounding::upward)==0x3f800000);
static_assert(cf::round_integral_bits<binary32>(0x3f400000,rounding::to_odd)==0x3f800000);
static_assert(cf::round_integral_bits<binary64>(0x8000000000000001ULL)==0x8000000000000000ULL);
static_assert(cf::sub_bits<binary32>(0x3f800000,0x3f800000,rounding::downward)==0x80000000);
static_assert(cf::sub_bits<binary32>(0x3f800000,0xffc12345)==0xffc12345);
static_assert(cf::equal_bits<binary32>(0,0x80000000));
static_assert(!cf::less_bits<binary32>(0x80000000,0));
static_assert(cf::less_bits<binary32>(0xff800000,0xff7fffff));
static_assert(!cf::less_bits<binary32>(0x7f812345,0x7f800000));

constexpr std::array modes{rounding::nearest_even,rounding::downward,rounding::upward,rounding::toward_zero};
constexpr std::array host_modes{FE_TONEAREST,FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO};

template<class F> struct sample {
  using word=typename F::bits_type;
  std::array<word,2> inputs;
  std::array<word,4> outputs;
};
template<class F> constexpr auto calculate(typename F::bits_type a,typename F::bits_type b,rounding mode) {
  return std::array{cf::sub_bits<F>(a,b,mode),cf::div_bits<F>(a,b,mode),
    cf::sqrt_bits<F>(typename F::bits_type(a&~F::sign_mask),mode),cf::round_integral_bits<F>(a,mode)};
}
template<class F> constexpr sample<F> make_case(native_test::property_rng& rng) {
  using word=typename F::bits_type;
  auto a=rng.bits<word>(),b=rng.bits<word>();
  if(cf::is_nan<F>(a) || cf::is_infinite<F>(a)) a&=~F::exponent_mask;
  if(cf::is_nan<F>(b) || cf::is_infinite<F>(b)) b&=~F::exponent_mask;
  if(cf::is_zero<F>(b)) b=1;
  return {{a,b},calculate<F>(a,b,rounding::nearest_even)};
}
constexpr auto cases32=native_test::property_cases<64>(native_test::property_seed,make_case<binary32>);
constexpr auto cases64=native_test::property_cases<64>(native_test::property_seed,make_case<binary64>);

template<class F,class T> bool check(sample<F> const& input,std::uint64_t seed,std::size_t index,unsigned mode) {
  using word=typename F::bits_type;
  volatile T a=std::bit_cast<T>(input.inputs[0]),b=std::bit_cast<T>(input.inputs[1]);
  volatile T positive=std::bit_cast<T>(word(input.inputs[0]&~F::sign_mask));
  std::array<word,4> actual{std::bit_cast<word>(T(a-b)),std::bit_cast<word>(T(a/b)),
    std::bit_cast<word>(std::sqrt(T(positive))),std::bit_cast<word>(std::nearbyint(T(a)))};
  if(!native_test::property_equal("IEEE division/sqrt/rounding",seed,index,input.outputs,actual,input.inputs,mode)) return false;
  return native_test::property_check("IEEE ordered comparisons",seed,index,
    cf::less_bits<F>(input.inputs[0],input.inputs[1])==(a<b) &&
    cf::equal_bits<F>(input.inputs[0],input.inputs[1])==(a==b),input.inputs);
}

template<class F,class T> bool generated(native_test::property_settings cfg) {
  native_test::property_rng random{cfg.seed};
  for(unsigned mode=0;mode<4;++mode) {
    if(std::fesetround(host_modes[mode])) return false;
    for(std::size_t i=0;i<cfg.cases;++i) {
      auto input=make_case<F>(random);
      input.outputs=calculate<F>(input.inputs[0],input.inputs[1],modes[mode]);
      if(!check<F,T>(input,cfg.seed,i,mode)) return false;
    }
  }
  return true;
}
int main() {
  std::fenv_t saved;
  if(std::fegetenv(&saved) || std::fesetenv(FE_DFL_ENV)) return 1;
  bool ok=true;
  for(std::size_t i=0;i<cases32.size() && ok;++i) ok=check<binary32,float>(cases32[i],native_test::property_seed,i,0);
  for(std::size_t i=0;i<cases64.size() && ok;++i) ok=check<binary64,double>(cases64[i],native_test::property_seed,i,0);
  auto cfg=native_test::property_config(8192);
  ok=ok && generated<binary32,float>(cfg) && generated<binary64,double>(cfg);
  if(std::fesetenv(&saved)) return 1;
  return ok?0:1;
}
