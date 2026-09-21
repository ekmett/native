// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/detail/constexpr_float.h>
#include "../property_check.h"
#include "../x86_f16c/oracle.h"
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

static_assert(cf::convert_bits<binary32,binary16>(0x0001)==0x33800000);
static_assert(cf::convert_bits<binary16,binary32>(0x33000000)==0);
static_assert(cf::convert_bits<binary16,binary32>(0x33000001)==1);
static_assert(cf::convert_bits<binary16,binary32>(0x33000000,rounding::upward)==1);
static_assert(cf::convert_bits<binary16,binary32>(0xb3000000,rounding::downward)==0x8001);
static_assert(cf::convert_bits<binary16,binary32>(0xb3000000)==0x8000);
static_assert(cf::convert_bits<binary16,binary32>(0x387fe000)==0x0400);
static_assert(cf::convert_bits<binary16,binary32>(0x477ff000)==0x7c00);
static_assert(cf::convert_bits<binary16,binary32>(0x477ff000,rounding::toward_zero)==0x7bff);
static_assert(cf::convert_bits<binary16,binary32>(0xff800001)==0xfe00);
static_assert(cf::convert_bits<binary32,bfloat16>(0xff81)==0xffc10000);
static_assert(cf::convert_bits<bfloat16,binary32>(0x3f808000)==0x3f80);
static_assert(cf::convert_bits<bfloat16,binary32>(0x3f818000)==0x3f82);
static_assert(cf::convert_bits<binary64,binary32>(1)==0x36a0000000000000ULL);
static_assert(cf::convert_bits<binary32,binary64>(0x36a0000000000000ULL)==1);
static_assert(cf::resize_nan<binary32,binary16>(0x7c01,false)==0x7f802000);
static_assert(cf::resize_nan<binary16,binary64>(0x7ff0000000000001ULL,false)==0x7c01);
static_assert(cf::multiply(UINT64_MAX,UINT64_MAX).low==1);
static_assert(cf::multiply(UINT64_MAX,UINT64_MAX).high==UINT64_MAX-1);
static_assert(cf::add_bits<binary32>(0x3f800000,0xbf800000)==0);
static_assert(cf::add_bits<binary32>(0x3f800000,0xbf800000,rounding::downward)==0x80000000);
static_assert(cf::add_bits<binary32>(0x80000000,0x80000000)==0x80000000);
static_assert(cf::mul_bits<binary32>(0x80000000,0x40000000)==0x80000000);
static_assert(cf::mul_bits<binary32>(0x7f800000,0)==0x7fc00000);
static_assert(cf::mul_bits<binary32>(0x7f7fffff,0x40000000,rounding::to_odd)==0x7f7fffff);
static_assert(cf::mul_bits<binary32>(0x7f7fffff,0x40000000,rounding::to_odd,
  {.odd_overflow_infinity=true})==0x7f800000);
static_assert(cf::mul_bits<binary32>(1,0x3f000000,rounding::to_odd)==1);
static_assert(cf::fma_bits<binary32>(0x3f800001,0x3f7ffffe,0xbf800000)==0xa8800000);
static_assert(cf::fma_bits<binary64>(0x3ff0000000000001ULL,0x3feffffffffffffeULL,
  0xbff0000000000000ULL)==0xb970000000000000ULL);
static_assert(cf::fma_bits<binary32>(0,0x7f800000,0x7fc12345)==0x7fc00000);
static_assert(cf::fma_bits<binary32>(0,0x7f800000,0x7f812345)==0x7fc12345);
static_assert(cf::fma_bits<binary32>(0x7f812345,0x7fc00002,0x7fc00003)==0x7fc12345);
static_assert(cf::fma_bits<binary32>(0x7fc00001,0x7fc00002,0x7fc00003)==0x7fc00003);
static_assert(cf::fma_bits<binary32>(0x7fc00001,0x7fc00002,0x7fc00003,
  rounding::nearest_even,{.nan=cf::nan_propagation::first,
  .fma_order=cf::fma_nan_order::multiplicands_first})==0x7fc00001);
static_assert(cf::add_bits<binary32>(0x007fffff,0x33000000,
  rounding::nearest_even,{.flush_inputs=true})==0x33000000);
static_assert(cf::mul_bits<binary32>(0x00800000,0x3f7fffff,
  rounding::nearest_even,{.flush_outputs=true})==0);
static_assert(cf::mul_bits<binary32>(0x00800000,0x3f7fffff)==0x00800000);
static_assert(cf::mul_bits<binary64>(1,0x3fe0000000000000ULL,rounding::upward)==1);
static_assert(cf::fma_bits<binary64>(1,1,0x7fefffffffffffffULL)==0x7fefffffffffffffULL);
static_assert(cf::fma_bits<binary64>(0x7fefffffffffffffULL,0x7fefffffffffffffULL,1)==
  0x7ff0000000000000ULL);

constexpr std::array modes{rounding::nearest_even,rounding::downward,
  rounding::upward,rounding::toward_zero};
constexpr std::array host_modes{FE_TONEAREST,FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO};

template<class F> struct sample {
  using word=typename F::bits_type;
  std::array<word,3> inputs;
  std::array<word,3> outputs;
};
template<class F> constexpr sample<F> make_sample(native_test::property_rng & rng) {
  using word=typename F::bits_type;
  auto a=rng.bits<word>(),b=rng.bits<word>(),c=rng.bits<word>();
  // Keep generated hardware comparisons finite. Dedicated assertions cover
  // NaN payload precedence, which the C++ arithmetic operators do not specify.
  if(cf::is_nan<F>(a) || cf::is_infinite<F>(a)) a&=~F::exponent_mask;
  if(cf::is_nan<F>(b) || cf::is_infinite<F>(b)) b&=~F::exponent_mask;
  if(cf::is_nan<F>(c) || cf::is_infinite<F>(c)) c&=~F::exponent_mask;
  return {{a,b,c},{cf::add_bits<F>(a,b),cf::mul_bits<F>(a,b),cf::fma_bits<F>(a,b,c)}};
}
inline constexpr auto cases32=native_test::property_cases<64>(native_test::property_seed,make_sample<binary32>);
inline constexpr auto cases64=native_test::property_cases<32>(native_test::property_seed,make_sample<binary64>);

template<class F,class T> bool check_sample(sample<F> const & s,std::uint64_t seed,std::size_t index) {
  using word=typename F::bits_type;
  volatile T a=std::bit_cast<T>(s.inputs[0]),b=std::bit_cast<T>(s.inputs[1]);
  volatile T c=std::bit_cast<T>(s.inputs[2]);
  std::array<word,3> actual{std::bit_cast<word>(T(a+b)),std::bit_cast<word>(T(a*b)),
    std::bit_cast<word>(std::fma(T(a),T(b),T(c)))};
  return native_test::property_equal("integer IEEE arithmetic",seed,index,s.outputs,actual,s.inputs);
}

template<class F,class T> bool random_arithmetic(native_test::property_settings cfg) {
  native_test::property_rng rng{cfg.seed};
  for(unsigned mode=0;mode<4;++mode) {
    if(std::fesetround(host_modes[mode])) return false;
    for(std::size_t i=0;i<cfg.cases;++i) {
      auto s=make_sample<F>(rng);
      auto a=s.inputs[0],b=s.inputs[1],c=s.inputs[2];
      s.outputs={cf::add_bits<F>(a,b,modes[mode]),cf::mul_bits<F>(a,b,modes[mode]),
        cf::fma_bits<F>(a,b,c,modes[mode])};
      if(!check_sample<F,T>(s,cfg.seed,i)) return false;
    }
  }
  return true;
}

bool conversions(native_test::property_settings cfg) {
  native_test::property_rng rng{cfg.seed};
  for(unsigned mode=0;mode<4;++mode) {
    if(std::fesetround(host_modes[mode])) return false;
    for(std::size_t i=0;i<cfg.cases;++i) {
      auto bits=rng.bits<std::uint64_t>();
      if(cf::is_nan<binary64>(bits)) continue;
      volatile double input=std::bit_cast<double>(bits);
      auto actual=std::bit_cast<std::uint32_t>(float(input));
      if(!native_test::property_equal("binary64 to binary32 rounding",cfg.seed,i,
        actual,cf::convert_bits<binary32,binary64>(bits,modes[mode]),bits,mode)) return false;
    }
  }
  return std::fesetround(FE_TONEAREST)==0;
}

bool half_conversions(native_test::property_settings cfg) {
  for(unsigned h=0;h<=0xffff;++h)
    if(!native_test::property_equal("all binary16 encodings",cfg.seed,h,
      f16c_fixture::widen(std::uint16_t(h)),cf::convert_bits<binary32,binary16>(std::uint16_t(h)),h)) return false;
  native_test::property_rng rng{cfg.seed};
  for(std::size_t i=0;i<cfg.cases;++i) {
    auto bits=rng.bits<std::uint32_t>();
    for(unsigned mode=0;mode<4;++mode)
      if(!native_test::property_equal("binary32 to binary16 rounding",cfg.seed,i,
        f16c_fixture::narrow(bits,mode,false),cf::convert_bits<binary16,binary32>(bits,modes[mode]),
        bits,mode)) return false;
  }
  return true;
}

int main() {
  std::fenv_t saved;
  if(std::fegetenv(&saved) || std::fesetenv(FE_DFL_ENV)) return 1;
  auto cfg=native_test::property_config(8192);
  bool ok=half_conversions(cfg) && conversions(cfg);
  for(std::size_t i=0;i<cases32.size() && ok;++i) ok=check_sample<binary32,float>(cases32[i],native_test::property_seed,i);
  for(std::size_t i=0;i<cases64.size() && ok;++i) ok=check_sample<binary64,double>(cases64[i],native_test::property_seed,i);
  ok=ok && random_arithmetic<binary32,float>(cfg) && random_arithmetic<binary64,double>(cfg);
  if(std::fesetenv(&saved)) return 1;
  return ok?0:1;
}
