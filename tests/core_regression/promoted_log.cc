// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <tuple>
#include <type_traits>
#include <vector>
#if !defined(__wasm__)
#include "support/fp_environment.h"
#include "support/profile.h"
#elif NATIVE_TEST_IMPORT
import native;
#else
#include <native/simd.h>
#include <native/simd/math/exp.h>
#endif
#if NATIVE_TEST_IMPORT
import native.wide;
import native.math;
#else
#include <native/wide.h>
#include <native/wide_math.h>
#endif

namespace {
  using word=std::uint32_t;
#if defined(__wasm__)
  constexpr auto architecture=native::feature_closure(native::wasm_feature::simd128);
  // The baseline Wasm polynomial intentionally rounds multiply and add separately.
  constexpr std::uint64_t accuracy_ulp=2;
#else
  constexpr auto architecture=test_arch;
  constexpr std::uint64_t accuracy_ulp=2;
#endif
  using scalar=native::simd<float,1,native::scalar>;
  template<unsigned N> using vector=native::simd<float,N,architecture>;
  constexpr word magnitude(word x) {return x&0x7fffffffu;}
  constexpr bool nan(word x) {return magnitude(x)>0x7f800000u;}
  constexpr word ordered(word x) {return x>>31 ? ~x : x^0x80000000u;}
  constexpr std::uint64_t distance(word a,word b) {
    auto x=std::uint64_t(ordered(a)),y=std::uint64_t(ordered(b));
    return x>y ? x-y : y-x;
  }
  void require(bool value,char const * message) {
    if(!value) {std::fprintf(stderr,"transcendentals: %s\n",message);std::abort();}
  }
  struct row {word input,log,log1p;};
  // Mathematical boundary references, rounded to binary32 independently of
  // the production coefficients. Zero/subnormal/domain rows follow the public
  // native contract. Ordinary finite rows permit the sampled accuracy budget.
  constexpr std::array fixed{
    row{0x00000000u,0xff800000u,0x00000000u},
    row{0x80000000u,0xff800000u,0x80000000u},
    row{0x00000001u,0xff800000u,0x00000001u},
    row{0x80000001u,0xff800000u,0x80000001u},
    row{0x007fffffu,0xff800000u,0x007fffffu},
    row{0x807fffffu,0xff800000u,0x807fffffu},
    row{0x00800000u,0xc2aeac50u,0x00800000u},
    row{0x33000000u,0xc18aa123u,0x33000000u},
    row{0x33000001u,0xc18aa123u,0x33000001u},
    row{0x3effffffu,0xbf317219u,0x3ecf991fu},
    row{0x3f000000u,0xbf317218u,0x3ecf991fu},
    row{0x3f000001u,0xbf317216u,0x3ecf9921u},
    row{0x3f3fffffu,0xbe934b13u,0x3f0f42fau},
    row{0x3f400000u,0xbe934b11u,0x3f0f42fbu},
    row{0x3f7fffffu,0xb3800000u,0x3f317217u},
    row{0x3f800000u,0x00000000u,0x3f317218u},
    row{0x3f800001u,0x33ffffffu,0x3f317219u},
    row{0x3fbfffffu,0x3ecf991du,0x3f6a9207u},
    row{0x3fc00000u,0x3ecf991fu,0x3f6a9208u},
    row{0x3fc00001u,0x3ecf9922u,0x3f6a9208u},
    row{0x40000000u,0x3f317218u,0x3f8c9f54u},
    row{0x7f7fffffu,0x42b17218u,0x42b17218u},
    row{0xbf000000u,0x7fc00000u,0xbf317218u},
    row{0xbf000001u,0x7fc00000u,0xbf31721au},
    row{0xbf7fffffu,0x7fc00000u,0xc1851592u},
    row{0xbf800000u,0x7fc00000u,0xff800000u},
    row{0xbf800001u,0x7fc00000u,0x7fc00000u},
    row{0x7f800000u,0x7f800000u,0x7f800000u},
    row{0xff800000u,0x7fc00000u,0x7fc00000u},
    row{0x7fc12345u,0x7fc00000u,0x7fc00000u},
    row{0xff812345u,0x7fc00000u,0x7fc00000u}};
  static_assert(std::same_as<decltype(math::log(1.f)),float>);
  static_assert(std::same_as<decltype(math::log1p(scalar{})),scalar>);
  static_assert(std::same_as<decltype(math::log(std::array<float,3>{})),std::array<float,3>>);
  static_assert(std::same_as<decltype(math::log1p(native::wide<scalar,3>{})),native::wide<scalar,3>>);
  static_assert(std::same_as<decltype(math::log(std::array<float,0>{})),std::array<float,0>>);
  template<class T> concept logarithm_argument=requires(T const & x) {math::log(x);math::log1p(x);};
  static_assert(!logarithm_argument<std::tuple<float,float>>);
  static_assert(std::bit_cast<word>(math::log(1.f))==0);
  static_assert(std::bit_cast<word>(math::log(0.f))==0xff800000u);
  static_assert(std::bit_cast<word>(math::log(-1.f))==0x7fc00000u);
  static_assert(std::bit_cast<word>(math::log1p(-0.f))==0x80000000u);
  static_assert(std::bit_cast<word>(math::log1p(-1.f))==0xff800000u);
  static_assert(std::bit_cast<word>(math::log1p(std::bit_cast<float>(1u)))==1u);
  static_assert(distance(std::bit_cast<word>(math::log(0.5f)),0xbf317218u)<=accuracy_ulp);
  static_assert(distance(std::bit_cast<word>(math::log1p(0.5f)),0x3ecf991fu)<=accuracy_ulp);

  row reference(word input) {
    auto m=magnitude(input);
    word l,lp;
    if(nan(input)) return {input,0x7fc00000u,0x7fc00000u};
    if(m<0x00800000u) l=0xff800000u;
    else if(input>>31) l=0x7fc00000u;
    else if(m==0x7f800000u) l=input;
    else l=std::bit_cast<word>(float(std::log(double(std::bit_cast<float>(input)))));
    if((input>>31) && m>0x3f800000u) lp=0x7fc00000u;
    else if(input==0xbf800000u) lp=0xff800000u;
    else if(m==0x7f800000u || m<=0x33000000u) lp=input;
    else lp=std::bit_cast<word>(float(std::log1p(double(std::bit_cast<float>(input)))));
    return {input,l,lp};
  }
  std::vector<row> make_bank() {
    std::vector<word> inputs;
    auto neighbors=[&](word center,int radius) {for(int d=-radius;d<=radius;++d) inputs.push_back(center+word(d));};
    for(auto r:fixed) neighbors(r.input,4);
    neighbors(0x3f800000u,1024);neighbors(0xbf800000u,1024);
    for(unsigned e=1;e<255;++e) {neighbors(e<<23,2);neighbors((e<<23)|0x00400000u,2);}
    word state=0xfa728311u;
    for(unsigned i=0;i<8192;++i) {state^=state<<13;state^=state>>17;state^=state<<5;inputs.push_back(state);}
    std::sort(inputs.begin(),inputs.end());inputs.erase(std::unique(inputs.begin(),inputs.end()),inputs.end());
    std::vector<row> result;result.reserve(inputs.size());
    for(word x:inputs) result.push_back(reference(x));
    return result;
  }
  std::size_t checks=0;
  void inspect(word actual,row const & r,bool one_plus) {
    word expected=one_plus?r.log1p:r.log;
    bool exact=magnitude(expected)>=0x7f800000u || magnitude(expected)==0 ||
      (one_plus && magnitude(r.input)<=0x33000000u);
    bool good=exact ? actual==expected : magnitude(actual)<0x7f800000u && distance(actual,expected)<=accuracy_ulp;
    if(!good) {
      std::fprintf(stderr,"%s input=%08x actual=%08x reference=%08x budget=%llu\n",
        one_plus?"log1p":"log",r.input,actual,expected,static_cast<unsigned long long>(accuracy_ulp));std::abort();
    }
    ++checks;
  }
  template<class V> void equal(V const & a,V const & b) {
    std::array<word,V::lanes> x{},y{};a.store_bits(x.data());b.store_bits(y.data());
    require(x==y,"public shapes or aliases differ");
  }
  template<class V> void check_shapes(std::vector<row> const & rows) {
    using pack=std::array<V,3>;
    using wide_pack=native::wide<V,3>;
    static_assert(std::same_as<decltype(math::log(V{})),V>);
    static_assert(std::same_as<decltype(math::log1p(pack{})),pack>);
    static_assert(std::same_as<decltype(math::log(wide_pack{})),wide_pack>);
    static_assert(std::same_as<decltype(native::log(V{})),V>);
    static_assert(std::same_as<decltype(native::log1p(pack{})),pack>);
    static_assert(noexcept(math::log(V{})) && noexcept(math::log1p(pack{})));
    static_assert([] {constexpr auto tiny=math::log1p(V::from_bits(0x80000001u));
      std::array<word,V::lanes> result{};tiny.store_bits(result.data());
      for(word x:result) if(x!=0x80000001u) return false;return true;}());
    std::array<V,0> empty{};native::wide<V,0> empty_wide{};
    require(math::log(empty).empty() && math::log1p(empty).empty(),"empty array changed size");
    require(native::log(empty).empty() && native::log1p(empty).empty(),"empty native array changed size");
    (void)math::log(empty_wide);(void)math::log1p(empty_wide);
    (void)native::log(empty_wide);(void)native::log1p(empty_wide);
    for(std::size_t base=0;base<rows.size();base+=3*V::lanes) {
      pack input{};std::array<word,V::lanes> words{};
      for(unsigned i=0;i<3;++i) {for(std::size_t lane=0;lane<V::lanes;++lane) words[lane]=rows[(base+i*V::lanes+lane)%rows.size()].input;input[i]=V::load_bits(words.data());}
      auto l=math::log(input),lp=math::log1p(input);
      auto nl=native::log(input),nlp=native::log1p(input);
      wide_pack wide_input{input};auto wl=math::log(wide_input),wlp=math::log1p(wide_input);
      auto nwl=native::log(wide_input),nwlp=native::log1p(wide_input);
      for(unsigned i=0;i<3;++i) {
        equal(l[i],nl[i]);equal(lp[i],nlp[i]);equal(l[i],wl.registers[i]);equal(lp[i],wlp.registers[i]);
        equal(l[i],nwl.registers[i]);equal(lp[i],nwlp.registers[i]);
        equal(l[i],wide::log(input[i]));equal(lp[i],wide::log1p(input[i]));
        equal(l[i],native::log(input[i]));equal(lp[i],native::log1p(input[i]));
        l[i].store_bits(words.data());for(std::size_t lane=0;lane<V::lanes;++lane) inspect(words[lane],rows[(base+i*V::lanes+lane)%rows.size()],false);
        lp[i].store_bits(words.data());for(std::size_t lane=0;lane<V::lanes;++lane) inspect(words[lane],rows[(base+i*V::lanes+lane)%rows.size()],true);
      }
    }
  }
  void scalar_shapes(std::vector<row> const & rows) {
    for(auto const & r:rows) {
      float x=std::bit_cast<float>(r.input);
      inspect(std::bit_cast<word>(math::log(x)),r,false);inspect(std::bit_cast<word>(math::log1p(x)),r,true);
    }
    for(std::size_t base=0;base<fixed.size();base+=3) {
      std::array<float,3> x{};
      for(unsigned i=0;i<3;++i)x[i]=std::bit_cast<float>(fixed[(base+i)%fixed.size()].input);
      auto l=math::log(x),lp=math::log1p(x);native::wide<float,3> w{x};
      auto wl=math::log(w),wlp=math::log1p(w);
      for(unsigned i=0;i<3;++i) {
        inspect(std::bit_cast<word>(l[i]),fixed[(base+i)%fixed.size()],false);
        inspect(std::bit_cast<word>(lp[i]),fixed[(base+i)%fixed.size()],true);
        require(std::bit_cast<word>(wl.registers[i])==std::bit_cast<word>(l[i]) &&
          std::bit_cast<word>(wlp.registers[i])==std::bit_cast<word>(lp[i]),"scalar wide shape changed result");
      }
    }
  }
  void run(std::vector<row> const & rows) {
    scalar_shapes(rows);check_shapes<scalar>(rows);
#if defined(__wasm__)
    check_shapes<vector<4>>(rows);
#elif NATIVE_TEST_PROFILE != 0
    check_shapes<vector<1>>(rows);check_shapes<vector<2>>(rows);check_shapes<vector<3>>(rows);check_shapes<vector<4>>(rows);
#if NATIVE_TEST_PROFILE == 256 || NATIVE_TEST_PROFILE == 512
    check_shapes<vector<8>>(rows);
#endif
#if NATIVE_TEST_PROFILE == 512
    check_shapes<vector<16>>(rows);
#endif
#endif
  }
}
int main() {
#if defined(__wasm__)
  auto rows=make_bank();run(rows);
#else
  auto saved=native::test::read_fp_state();
  std::vector<row> rows;
  {native::test::fp_scope scope(native::test::fp_mode::gradual);rows=make_bank();}
  for(auto mode:{native::test::fp_mode::gradual,native::test::fp_mode::flush}) {
    native::test::fp_scope scope(mode);run(rows);
    require(scope.controls_match(),"log/log1p changed FP controls");
  }
  require(native::test::read_fp_state()==saved,"fixture failed to restore FP state");
#endif
  std::printf("log/log1p: %zu mathematical inputs; %zu value checks, shapes and constexpr passed\n",rows.size(),checks);
#if !defined(__wasm__)
  std::puts("log/log1p FP controls preserved in gradual and flushing modes");
#endif
}
