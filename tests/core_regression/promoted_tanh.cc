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
#if defined(NATIVE_TEST_MPFR)
#include <mpfr.h>
#endif
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

#if !defined(__wasm__)
// Frozen scalar interval/operation graph from FTZ 125a617e64b6b0808b6bf0d48520d3c9508f341e,
// src/shared/ftz/math/tanh.h. Only the named arithmetic wrappers are replaced by
// native multiply/FMA and bit casts. Compare on FTZ's admitted domain (no raw
// subnormals); this checks graph equality, independently of the MPFR accuracy test.
namespace frozen_ftz {
  inline float fp32_decode(unsigned int word){return std::bit_cast<float>(word);}
  inline unsigned int fp32_encode(float x){return std::bit_cast<unsigned int>(x);}
  inline float multiply(float a,float b){return a*b;}
  inline unsigned int tanh_words(unsigned int word) {
    unsigned int magnitude = word & 0x7fffffffu;
    unsigned int sign = word & 0x80000000u;
    if (magnitude > 0x7f800000u) {
      return 0x7fc00000u;
    }
    if (magnitude < 0x00800000u) {
      return sign;
    }
    // tanh(x) rounds to x throughout this small interval. Preserve raw -0
    // above and normal bits here without evaluating an underflowing product.
    if (magnitude <= 0x39800000u) {
      return word;
    }
    // tanh(10) differs from one by less than half a binary32 ULP below one.
    // This also handles either infinity before any arithmetic.
    if (magnitude >= 0x41200000u) {
      return sign | 0x3f800000u;
    }
    float x = fp32_decode(magnitude);
    float z = multiply(x, x);
    float t, h;
    if (magnitude <= 0x3f800000u) {
      t = std::fma(z, fp32_decode(0x40000000u), fp32_decode(0xbf800000u));
      h = fp32_decode(0x34facb37u);
      h = std::fma(h, t, fp32_decode(0xb63a0d2du));
      h = std::fma(h, t, fp32_decode(0x37813497u));
      h = std::fma(h, t, fp32_decode(0xb8bfb3f8u));
      h = std::fma(h, t, fp32_decode(0x3a0e6d24u));
      h = std::fma(h, t, fp32_decode(0xbb535f6cu));
      h = std::fma(h, t, fp32_decode(0x3c9d20e4u));
      h = std::fma(h, t, fp32_decode(0xbded544du));
      h = std::fma(h, t, fp32_decode(0x3f5c6e3eu));
    }
    else if (magnitude <= 0x40000000u) {
      t = std::fma(z, fp32_decode(0x3f000000u), fp32_decode(0xbfa00000u));
      h = fp32_decode(0x38752140u);
      h = std::fma(h, t, fp32_decode(0xb9183513u));
      h = std::fma(h, t, fp32_decode(0x398d9ee0u));
      h = std::fma(h, t, fp32_decode(0xba2fdf3au));
      h = std::fma(h, t, fp32_decode(0x3ae1130du));
      h = std::fma(h, t, fp32_decode(0xbb8bc302u));
      h = std::fma(h, t, fp32_decode(0x3c2d6773u));
      h = std::fma(h, t, fp32_decode(0xbcd7a178u));
      h = std::fma(h, t, fp32_decode(0x3d86d45du));
      h = std::fma(h, t, fp32_decode(0xbe2e2df9u));
      h = std::fma(h, t, fp32_decode(0x3f14c222u));
    }
    else if (magnitude <= 0x40400000u) {
      t = std::fma(z, fp32_decode(0x3e800000u), fp32_decode(0xbfd00000u));
      h = fp32_decode(0xb947bd66u);
      h = std::fma(h, t, fp32_decode(0x39dfe5a4u));
      h = std::fma(h, t, fp32_decode(0xba4a3864u));
      h = std::fma(h, t, fp32_decode(0x3ae2b84bu));
      h = std::fma(h, t, fp32_decode(0xbb813c08u));
      h = std::fma(h, t, fp32_decode(0x3c1129ceu));
      h = std::fma(h, t, fp32_decode(0xbca3c0b8u));
      h = std::fma(h, t, fp32_decode(0x3d3bcdf4u));
      h = std::fma(h, t, fp32_decode(0xbde4f8bcu));
      h = std::fma(h, t, fp32_decode(0x3ec662fcu));
    }
    else if (magnitude <= 0x40800000u) {
      t = std::fma(z, fp32_decode(0x3e800000u), fp32_decode(0xc0480000u));
      h = fp32_decode(0xb58f6d10u);
      h = std::fma(h, t, fp32_decode(0x36863471u));
      h = std::fma(h, t, fp32_decode(0xb758ec9du));
      h = std::fma(h, t, fp32_decode(0x384b3fb7u));
      h = std::fma(h, t, fp32_decode(0xb9404721u));
      h = std::fma(h, t, fp32_decode(0x3a356f7bu));
      h = std::fma(h, t, fp32_decode(0xbb2d3d49u));
      h = std::fma(h, t, fp32_decode(0x3c2a7e14u));
      h = std::fma(h, t, fp32_decode(0xbd36d397u));
      h = std::fma(h, t, fp32_decode(0x3e9091d7u));
    }
    else if (magnitude <= 0x40c00000u) {
      t = std::fma(z, fp32_decode(0x3d800000u), fp32_decode(0xbfd00000u));
      h = fp32_decode(0xb9405fbfu);
      h = std::fma(h, t, fp32_decode(0x39ab4d5fu));
      h = std::fma(h, t, fp32_decode(0xb9c08feeu));
      h = std::fma(h, t, fp32_decode(0x3a2bf5fau));
      h = std::fma(h, t, fp32_decode(0xbaa65b2au));
      h = std::fma(h, t, fp32_decode(0x3b1584c1u));
      h = std::fma(h, t, fp32_decode(0xbb86bc79u));
      h = std::fma(h, t, fp32_decode(0x3bf71905u));
      h = std::fma(h, t, fp32_decode(0xbc67da1fu));
      h = std::fma(h, t, fp32_decode(0x3ce35ff2u));
      h = std::fma(h, t, fp32_decode(0xbd76f5e5u));
      h = std::fma(h, t, fp32_decode(0x3e48ced7u));
    }
    else if (magnitude <= 0x41000000u) {
      t = std::fma(z, fp32_decode(0x3d800000u), fp32_decode(0xc0480000u));
      h = fp32_decode(0xb59171b1u);
      h = std::fma(h, t, fp32_decode(0x3671d749u));
      h = std::fma(h, t, fp32_decode(0xb72757e1u));
      h = std::fma(h, t, fp32_decode(0x380d4deeu));
      h = std::fma(h, t, fp32_decode(0xb8f4646bu));
      h = std::fma(h, t, fp32_decode(0x39d46b54u));
      h = std::fma(h, t, fp32_decode(0xbabdbc02u));
      h = std::fma(h, t, fp32_decode(0x3bb1ed8au));
      h = std::fma(h, t, fp32_decode(0xbcb95c19u));
      h = std::fma(h, t, fp32_decode(0x3e10d0b5u));
    }
    else {
      t = std::fma(z, fp32_decode(0x3d000000u), fp32_decode(0xc0240000u));
      h = fp32_decode(0xb811c415u);
      h = std::fma(h, t, fp32_decode(0x38c8e73cu));
      h = std::fma(h, t, fp32_decode(0xb980a2b8u));
      h = std::fma(h, t, fp32_decode(0x3a37296bu));
      h = std::fma(h, t, fp32_decode(0xbb06690eu));
      h = std::fma(h, t, fp32_decode(0x3bcea86au));
      h = std::fma(h, t, fp32_decode(0xbcb08499u));
      h = std::fma(h, t, fp32_decode(0x3de229ecu));
    }
    // Keep the mathematical range despite the last multiply's rounding error.
    unsigned int rounded_magnitude = fp32_encode(multiply(x, h));
    return (rounded_magnitude > 0x3f800000u ? 0x3f800000u : rounded_magnitude) | sign;
  }
}
#endif

namespace {
  using word=std::uint32_t;
#if defined(__wasm__)
  constexpr auto architecture=native::feature_closure(native::wasm_feature::simd128);
#else
  constexpr auto architecture=test_arch;
#endif
  constexpr unsigned accuracy_ulp=2;
  using scalar=native::simd<float,1,native::scalar>;
  template<unsigned N> using vector=native::simd<float,N,architecture>;
  constexpr word magnitude(word x) {return x&0x7fffffffu;}
  constexpr bool nan(word x) {return magnitude(x)>0x7f800000u;}
  constexpr word distance(word a,word b) {
    a=magnitude(a);b=magnitude(b);return a>b?a-b:b-a;
  }
  constexpr bool equivalent(word a,word b) {return a==b || (nan(a)&&nan(b));}
  constexpr bool accurate(word actual,word input,word expected) {
    if(nan(input)) return nan(actual);
    if(magnitude(input)<=0x39800000u || magnitude(input)>=0x41200000u)
      return actual==expected;
    return (actual>>31)==(input>>31) && magnitude(actual)<=0x3f800000u &&
      distance(actual,expected)<=accuracy_ulp;
  }
  void require(bool value,char const * message) {
    if(!value) {std::fprintf(stderr,"tanh: %s\n",message);std::abort();}
  }
  struct row {word input,expected;};
  // Independently rounded MPFR256/RNDN references; NaN payloads are unrestricted.
  constexpr std::array fixed{
    row{0x00000000u,0x00000000u},
    row{0x80000000u,0x80000000u},
    row{0x00000001u,0x00000001u},
    row{0x80000001u,0x80000001u},
    row{0x007fffffu,0x007fffffu},
    row{0x807fffffu,0x807fffffu},
    row{0x00800000u,0x00800000u},
    row{0x80800000u,0x80800000u},
    row{0x39800000u,0x39800000u},
    row{0xb9800000u,0xb9800000u},
    row{0x39800001u,0x39800001u},
    row{0x3f000000u,0x3eec9a9fu},
    row{0xbf000000u,0xbeec9a9fu},
    row{0x3f7fffffu,0x3f42f7d5u},
    row{0x3f800000u,0x3f42f7d6u},
    row{0x3f800001u,0x3f42f7d6u},
    row{0x3fffffb5u,0x3f76ca78u},
    row{0x3fffffffu,0x3f76ca83u},
    row{0x40000000u,0x3f76ca83u},
    row{0x40000001u,0x3f76ca83u},
    row{0x40400000u,0x3f7ebbe9u},
    row{0x40800000u,0x3f7fd40cu},
    row{0x40c00000u,0x3f7fff32u},
    row{0x41000000u,0x3f7ffffcu},
    row{0x411fffffu,0x3f800000u},
    row{0x41200000u,0x3f800000u},
    row{0x7f7fffffu,0x3f800000u},
    row{0xff7fffffu,0xbf800000u},
    row{0x7f800000u,0x3f800000u},
    row{0xff800000u,0xbf800000u},
    row{0x7fc12345u,0x7fc00000u},
    row{0xff800001u,0x7fc00000u},
    row{0x40180000u,0x3f7b9bbbu},
    row{0x40680000u,0x3f7fa2fbu},
    row{0x40a80000u,0x3f7ffc64u},
    row{0x40e80000u,0x3f7fffefu},
  };
  static_assert(std::same_as<decltype(math::tanh(0.f)),float>);
  static_assert(std::same_as<decltype(math::tanh(scalar{})),scalar>);
  static_assert(std::same_as<decltype(math::tanh(std::array<float,3>{})),std::array<float,3>>);
  static_assert(std::same_as<decltype(math::tanh(native::wide<scalar,3>{})),native::wide<scalar,3>>);
  static_assert(std::same_as<decltype(math::tanh(std::array<float,0>{})),std::array<float,0>>);
  template<class T> concept tanh_argument=requires(T const & x) {math::tanh(x);};
  static_assert(!tanh_argument<std::tuple<float,float>>);
  static_assert(std::bit_cast<word>(math::tanh(-0.f))==0x80000000u);
  static_assert(std::bit_cast<word>(math::tanh(std::bit_cast<float>(1u)))==1u);
  static_assert(std::bit_cast<word>(math::tanh(std::bit_cast<float>(0xff800000u)))==0xbf800000u);
  static_assert([] {for(auto r:fixed)
    if(!accurate(std::bit_cast<word>(math::tanh(std::bit_cast<float>(r.input))),r.input,r.expected))return false;
    return true;}());

  std::vector<row> make_bank() {
    std::vector<word> inputs;
    auto neighbors=[&](word center,int radius) {
      for(int d=-radius;d<=radius;++d) if(std::int64_t(center)+d>=0)
        inputs.push_back(word(std::int64_t(center)+d));
    };
    for(auto r:fixed)neighbors(r.input,4);
    for(word center:std::array<word,8>{0x39800000u,0x3f800000u,0x40000000u,0x40400000u,
        0x40800000u,0x40c00000u,0x41000000u,0x41200000u}) {
      neighbors(center,128);neighbors(center|0x80000000u,128);
    }
    for(unsigned i=0;i<=32768;++i)inputs.push_back(std::bit_cast<word>(float(i)*(20.f/32768.f)-10.f));
    word state=0xfa728311u;
    for(unsigned i=0;i<8192;++i) {state^=state<<13;state^=state>>17;state^=state<<5;inputs.push_back(state);}
    std::sort(inputs.begin(),inputs.end());inputs.erase(std::unique(inputs.begin(),inputs.end()),inputs.end());
    std::vector<row> result;result.reserve(inputs.size());
#if defined(NATIVE_TEST_MPFR)
    mpfr_t value;mpfr_init2(value,256);
#endif
    for(word x:inputs) {
      word expected;
      if(nan(x))expected=0x7fc00000u;
#if defined(NATIVE_TEST_MPFR)
      else {
        mpfr_set_flt(value,std::bit_cast<float>(x),MPFR_RNDN);mpfr_tanh(value,value,MPFR_RNDN);
        expected=std::bit_cast<word>(mpfr_get_flt(value,MPFR_RNDN));
      }
#else
      else expected=std::bit_cast<word>(float(std::tanh(double(std::bit_cast<float>(x)))));
#endif
      result.push_back({x,expected});
    }
#if defined(NATIVE_TEST_MPFR)
    mpfr_clear(value);
#endif
    return result;
  }
  std::size_t checks=0;
#if !defined(__wasm__)
  std::size_t graph_checks=0;
#endif
  unsigned maximum_ulp=0;
  void inspect(word actual,row const & r) {
    if(!accurate(actual,r.input,r.expected)) {
      std::fprintf(stderr,"tanh input=%08x actual=%08x reference=%08x budget=%u\n",
        r.input,actual,r.expected,accuracy_ulp);std::abort();
    }
    if(!nan(r.input))maximum_ulp=std::max(maximum_ulp,unsigned(distance(actual,r.expected)));
    ++checks;
#if !defined(__wasm__)
    // FTZ's frozen graph admits zeros/normals/infinities/NaNs. Native preserves
    // raw subnormal inputs; Wasm intentionally uses a different nonfused graph.
    if(magnitude(r.input)>=0x00800000u || magnitude(r.input)==0) {
      require(equivalent(actual,frozen_ftz::tanh_words(r.input)),"frozen FTZ fused graph differs");
      ++graph_checks;
    }
#endif
  }
  template<class V> void equal(V const & a,V const & b) {
    std::array<word,V::lanes> x{},y{};a.store_bits(x.data());b.store_bits(y.data());
    for(std::size_t i=0;i<V::lanes;++i)require(equivalent(x[i],y[i]),"public shapes or aliases differ");
  }
  template<class V> void check_shapes(std::vector<row> const & rows) {
    using pack=std::array<V,3>;using wide_pack=native::wide<V,3>;
    static_assert(std::same_as<decltype(math::tanh(V{})),V>);
    static_assert(std::same_as<decltype(math::tanh(pack{})),pack>);
    static_assert(std::same_as<decltype(math::tanh(wide_pack{})),wide_pack>);
    static_assert(std::same_as<decltype(native::tanh(V{})),V>);
    static_assert(std::same_as<decltype(native::tanh(pack{})),pack>);
    static_assert(noexcept(math::tanh(V{})) && noexcept(math::tanh(pack{})));
    static_assert([] {auto const tiny=math::tanh(V::from_bits(0x80000001u));
      std::array<word,V::lanes> output{};tiny.store_bits(output.data());
      for(word x:output)if(x!=0x80000001u)return false;return true;}());
    static_assert([] {for(std::size_t base=0;base<fixed.size();base+=V::lanes) {
      std::array<word,V::lanes> input{},output{};
      for(std::size_t lane=0;lane<V::lanes;++lane)input[lane]=fixed[(base+lane)%fixed.size()].input;
      math::tanh(V::load_bits(input.data())).store_bits(output.data());
      for(std::size_t lane=0;lane<V::lanes;++lane)if(!accurate(output[lane],input[lane],fixed[(base+lane)%fixed.size()].expected))return false;
    }return true;}());
    std::array<V,0> empty{};native::wide<V,0> empty_wide{};
    require(math::tanh(empty).empty() && native::tanh(empty).empty(),"empty array changed size");
    require(math::tanh(empty_wide).registers.empty() && native::tanh(empty_wide).registers.empty(),"empty wide changed size");
    for(std::size_t base=0;base<rows.size();base+=3*V::lanes) {
      pack input{};std::array<word,V::lanes> words{};
      for(unsigned i=0;i<3;++i) {for(std::size_t lane=0;lane<V::lanes;++lane)words[lane]=rows[(base+i*V::lanes+lane)%rows.size()].input;input[i]=V::load_bits(words.data());}
      auto result=math::tanh(input),alias=native::tanh(input);
      wide_pack wide_input{input};auto legacy=math::tanh(wide_input),legacy_alias=native::tanh(wide_input);
      for(unsigned i=0;i<3;++i) {
        equal(result[i],alias[i]);equal(result[i],legacy.registers[i]);equal(result[i],legacy_alias.registers[i]);
        equal(result[i],math::tanh(input[i]));equal(result[i],wide::tanh(input[i]));
        equal(result[i],native::tanh(input[i]));
#if NATIVE_TEST_IMPORT
        equal(result[i],native::math::tanh(input[i]));
#endif
        result[i].store_bits(words.data());
        for(std::size_t lane=0;lane<V::lanes;++lane)inspect(words[lane],rows[(base+i*V::lanes+lane)%rows.size()]);
      }
    }
  }
  void scalar_shapes(std::vector<row> const & rows) {
    for(auto const & r:rows)inspect(std::bit_cast<word>(math::tanh(std::bit_cast<float>(r.input))),r);
    for(std::size_t base=0;base<fixed.size();base+=3) {
      std::array<float,3> x{};
      for(unsigned i=0;i<3;++i)x[i]=std::bit_cast<float>(fixed[(base+i)%fixed.size()].input);
      auto result=math::tanh(x);auto legacy=math::tanh(native::wide<float,3>{x});
      for(unsigned i=0;i<3;++i) {
        inspect(std::bit_cast<word>(result[i]),fixed[(base+i)%fixed.size()]);
        require(equivalent(std::bit_cast<word>(legacy.registers[i]),std::bit_cast<word>(result[i])),"scalar wide changed result");
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
  auto saved=native::test::read_fp_state();std::vector<row> rows;
  {native::test::fp_scope scope(native::test::fp_mode::gradual);rows=make_bank();}
  for(auto mode:{native::test::fp_mode::gradual,native::test::fp_mode::flush}) {
    native::test::fp_scope scope(mode);run(rows);require(scope.controls_match(),"changed FP controls");
  }
  require(native::test::read_fp_state()==saved,"fixture failed to restore FP state");
#endif
#if defined(NATIVE_TEST_MPFR)
  constexpr char reference_name[]="MPFR256 RNDN";
#else
  constexpr char reference_name[]="double libm";
#endif
  std::printf("tanh: %zu sampled %s inputs; %zu value checks, observed max %u ULP; shapes and constexpr passed\n",rows.size(),reference_name,checks,maximum_ulp);
#if !defined(__wasm__)
  std::printf("tanh: %zu exact frozen FTZ fused-graph checks; FP controls preserved in gradual/flushing modes\n",graph_checks);
#endif
}
