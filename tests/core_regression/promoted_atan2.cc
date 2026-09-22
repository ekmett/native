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

namespace {
  using word=std::uint32_t;
#if defined(__wasm__)
  constexpr auto architecture=native::feature_closure(native::wasm_feature::simd128);
#else
  constexpr auto architecture=test_arch;
#endif
  constexpr std::uint64_t accuracy_ulp=2;
  using scalar=native::simd<float,1,native::scalar>;
  template<unsigned N> using vector=native::simd<float,N,architecture>;
  constexpr word magnitude(word x) {return x&0x7fffffffu;}
  constexpr bool nan(word x) {return magnitude(x)>0x7f800000u;}
  constexpr word canonical(word x) {return magnitude(x)<0x00800000u ? x&0x80000000u : x;}
  constexpr word ordered(word x) {return x>>31 ? ~x : x^0x80000000u;}
  constexpr std::uint64_t distance(word a,word b) {
    auto x=std::uint64_t(ordered(a)),y=std::uint64_t(ordered(b));
    return x>y ? x-y : y-x;
  }
  void require(bool value,char const * message) {
    if(!value) {std::fprintf(stderr,"atan2: %s\n",message);std::abort();}
  }
  struct row {word y,x,angle;};
  constexpr std::array fixed{
    row{0x00000000u,0x00000000u,0x00000000u},
    row{0x80000000u,0x00000000u,0x80000000u},
    row{0x00000000u,0x80000000u,0x40490fdbu},
    row{0x80000000u,0x80000000u,0xc0490fdbu},
    row{0x3f800000u,0x00000000u,0x3fc90fdbu},
    row{0xbf800000u,0x80000000u,0xbfc90fdbu},
    row{0x3f800000u,0x3f800000u,0x3f490fdbu},
    row{0x3f800000u,0xbf800000u,0x4016cbe4u},
    row{0xbf800000u,0xbf800000u,0xc016cbe4u},
    row{0xbf800000u,0x3f800000u,0xbf490fdbu},
    row{0x7f800000u,0x7f800000u,0x3f490fdbu},
    row{0x7f800000u,0xff800000u,0x4016cbe4u},
    row{0xff800000u,0xff800000u,0xc016cbe4u},
    row{0xff800000u,0x7f800000u,0xbf490fdbu},
    row{0x3f800000u,0x7f800000u,0x00000000u},
    row{0xbf800000u,0x7f800000u,0x80000000u},
    row{0x3f800000u,0xff800000u,0x40490fdbu},
    row{0xbf800000u,0xff800000u,0xc0490fdbu},
    row{0x7f800000u,0x3f800000u,0x3fc90fdbu},
    row{0xff800000u,0xbf800000u,0xbfc90fdbu},
    row{0x00000001u,0x80000001u,0x40490fdbu},
    row{0x807fffffu,0x007fffffu,0x80000000u},
    row{0x00800000u,0x40000000u,0x00400000u},
    row{0x80800000u,0x40000000u,0x80400000u},
    row{0x7fc12345u,0x3f800000u,0x7fc00000u},
    row{0x3f800000u,0xff812345u,0x7fc00000u}};
  template<class Y,class X=Y> concept argument=requires(Y const & y,X const & x) {math::atan2(y,x);};
  static_assert(argument<float> && argument<scalar>);
  static_assert(!argument<std::tuple<float,float>> && !argument<double>);
  static_assert(!argument<float,scalar> && !argument<std::array<float,2>,std::array<float,3>>);
  static_assert(std::same_as<decltype(math::atan2(1.f,1.f)),float>);
  static_assert(std::same_as<decltype(math::atan2(std::array<float,0>{},std::array<float,0>{})),std::array<float,0>>);
  static_assert(std::bit_cast<word>(math::atan2(-0.f,0.f))==0x80000000u);
  static_assert(std::bit_cast<word>(math::atan2(-0.f,-0.f))==0xc0490fdbu);
  static_assert(std::bit_cast<word>(math::atan2(std::bit_cast<float>(0x00000001u),-1.f))==0x40490fdbu);
  static_assert(std::bit_cast<word>(math::atan2(std::bit_cast<float>(0x7f800000u),std::bit_cast<float>(0xff800000u)))==0x4016cbe4u);
  static_assert(distance(std::bit_cast<word>(math::atan2(1.f,1.f)),0x3f490fdbu)<=accuracy_ulp);

  row reference(word y,word x) {
    if(nan(y)||nan(x)) return {y,x,0x7fc00000u};
    auto cy=canonical(y),cx=canonical(x);
#if defined(NATIVE_TEST_MPFR)
    mpfr_t my,mx,ma;mpfr_inits2(256,my,mx,ma,static_cast<mpfr_ptr>(nullptr));
    mpfr_set_flt(my,std::bit_cast<float>(cy),MPFR_RNDN);
    mpfr_set_flt(mx,std::bit_cast<float>(cx),MPFR_RNDN);
    mpfr_atan2(ma,my,mx,MPFR_RNDN);
    auto angle=std::bit_cast<word>(mpfr_get_flt(ma,MPFR_RNDN));
    mpfr_clears(my,mx,ma,static_cast<mpfr_ptr>(nullptr));
    return {y,x,angle};
#else
    return {y,x,std::bit_cast<word>(float(std::atan2(double(std::bit_cast<float>(cy)),double(std::bit_cast<float>(cx)))))};
#endif
  }
  std::vector<row> make_bank() {
    std::vector<row> rows(fixed.begin(),fixed.end());
    constexpr std::array<word,20> endpoints{0,0x80000000u,1,0x80000001u,0x007fffffu,0x807fffffu,
      0x00800000u,0x80800000u,0x3f000000u,0xbf000000u,0x3f800000u,0xbf800000u,
      0x7f7fffffu,0xff7fffffu,0x7f800000u,0xff800000u,0x7fc12345u,0xffc12345u,0x7f812345u,0xff812345u};
    for(word y:endpoints) for(word x:endpoints) rows.push_back(reference(y,x));
    for(unsigned e=1;e<255;++e) for(int d=-2;d<=2;++d) {
      word y=(e<<23)+word(d);
      for(word sign:{0u,0x80000000u}) {
        rows.push_back(reference(y|sign,0x3f800000u));
        rows.push_back(reference(0x3f800000u,y|sign));
        rows.push_back(reference(y|sign,0x7f7fffffu));
        rows.push_back(reference(0x00800000u,y|sign));
      }
    }
    for(word t:{0x39800000u,0x3f800000u}) for(int d=-256;d<=256;++d)
      for(word sy:{0u,0x80000000u}) for(word sx:{0u,0x80000000u}) {
        auto y=(t+word(d))|sy,x=0x3f800000u|sx;
        rows.push_back(reference(y,x));rows.push_back(reference(x,y));
      }
    word state=0x724981abu;
    auto next=[&] {state^=state<<13;state^=state>>17;state^=state<<5;return state;};
    for(unsigned i=0;i<32768;++i) {auto y=next(),x=next();rows.push_back(reference(y,x));}
    // Uniform reduced ratios exercise the polynomial between the endpoint
    // neighborhoods; unrelated random exponents mostly approach an axis.
    for(unsigned i=0;i<32768;++i) {
      auto bits=next();
      auto y=std::bit_cast<word>(float(bits&0x00ffffffu)*0x1p-24f)|(bits&0x80000000u);
      auto x=0x3f800000u|((bits<<1)&0x80000000u);
      rows.push_back((bits&0x01000000u) ? reference(y,x) : reference(x,y));
    }
    return rows;
  }
  bool flush=false;
  std::uint64_t largest_ulp=0;
  std::size_t checks=0;
  void inspect(word actual,row const & r) {
    word expected=r.angle;
    if(flush && magnitude(expected)<0x00800000u) expected &= 0x80000000u;
    auto y=canonical(r.y),x=canonical(r.x);
    bool exact=nan(expected) || magnitude(y)==0 || magnitude(x)==0 || magnitude(y)>=0x7f800000u || magnitude(x)>=0x7f800000u;
    auto error=distance(actual,expected);
    bool good=nan(expected) ? nan(actual) : exact ? actual==expected : magnitude(actual)<0x7f800000u && error<=accuracy_ulp;
    if(!good) {
      std::fprintf(stderr,"atan2 y=%08x x=%08x actual=%08x reference=%08x flush=%d ulp=%llu\n",
        r.y,r.x,actual,expected,flush,static_cast<unsigned long long>(error));std::abort();
    }
    if(!exact) largest_ulp=std::max(largest_ulp,error);
    ++checks;
  }
  template<class V> void equal(V const & a,V const & b) {
    std::array<word,V::lanes> x{},y{};a.store_bits(x.data());b.store_bits(y.data());
    require(x==y,"public shapes differ");
  }
  template<class V> void check_shapes(std::vector<row> const & rows) {
    using pack=std::array<V,3>;
    using wide_pack=native::wide<V,3>;
    static_assert(std::same_as<decltype(math::atan2(V{},V{})),V>);
    static_assert(std::same_as<decltype(math::atan2(pack{},pack{})),pack>);
    static_assert(std::same_as<decltype(math::atan2(wide_pack{},wide_pack{})),wide_pack>);
    static_assert(std::same_as<decltype(native::atan2(V{},V{})),V>);
    static_assert(std::same_as<decltype(native::atan2(pack{},pack{})),pack>);
    static_assert(std::same_as<decltype(native::atan2(wide_pack{},wide_pack{})),wide_pack>);
    static_assert(noexcept(math::atan2(V{},V{})) && noexcept(native::atan2(pack{},pack{})));
    static_assert([] {constexpr auto a=math::atan2(V::from_bits(0x80000001u),V(-1.f));
      std::array<word,V::lanes> words{};a.store_bits(words.data());
      for(word x:words) if(x!=0xc0490fdbu) return false;return true;}());
    std::array<V,0> empty{};native::wide<V,0> empty_wide{};
    require(math::atan2(empty,empty).empty(),"empty array changed size");
    (void)math::atan2(empty_wide,empty_wide);
    require(native::atan2(empty,empty).empty(),"native empty array changed size");
    (void)native::atan2(empty_wide,empty_wide);
    for(std::size_t base=0;base<rows.size();base+=3*V::lanes) {
      pack y{},x{};std::array<word,V::lanes> words{};
      for(unsigned i=0;i<3;++i) {
        for(std::size_t lane=0;lane<V::lanes;++lane)words[lane]=rows[(base+i*V::lanes+lane)%rows.size()].y;
        y[i]=V::load_bits(words.data());
        for(std::size_t lane=0;lane<V::lanes;++lane)words[lane]=rows[(base+i*V::lanes+lane)%rows.size()].x;
        x[i]=V::load_bits(words.data());
      }
      auto a=math::atan2(y,x);auto wa=math::atan2(wide_pack{y},wide_pack{x});
      auto na=native::atan2(y,x);auto nwa=native::atan2(wide_pack{y},wide_pack{x});
      for(unsigned i=0;i<3;++i) {
        equal(a[i],wa.registers[i]);equal(a[i],math::atan2(y[i],x[i]));
        equal(a[i],na[i]);equal(a[i],nwa.registers[i]);equal(a[i],native::atan2(y[i],x[i]));
        a[i].store_bits(words.data());for(std::size_t lane=0;lane<V::lanes;++lane)inspect(words[lane],rows[(base+i*V::lanes+lane)%rows.size()]);
      }
    }
  }
  void run(std::vector<row> const & rows) {
    for(auto const & r:rows)inspect(std::bit_cast<word>(math::atan2(std::bit_cast<float>(r.y),std::bit_cast<float>(r.x))),r);
    for(std::size_t base=0;base<rows.size();base+=3) {
      std::array<float,3> y{},x{};
      for(unsigned i=0;i<3;++i) {auto r=rows[(base+i)%rows.size()];y[i]=std::bit_cast<float>(r.y);x[i]=std::bit_cast<float>(r.x);}
      auto a=math::atan2(y,x);auto wa=math::atan2(native::wide<float,3>{y},native::wide<float,3>{x});
      for(unsigned i=0;i<3;++i) {inspect(std::bit_cast<word>(a[i]),rows[(base+i)%rows.size()]);require(std::bit_cast<word>(a[i])==std::bit_cast<word>(wa.registers[i]),"scalar array/wide mismatch");}
    }
    check_shapes<scalar>(rows);
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
    native::test::fp_scope scope(mode);flush=mode==native::test::fp_mode::flush;run(rows);
    require(scope.controls_match(),"atan2 changed FP controls");
  }
  require(native::test::read_fp_state()==saved,"fixture failed to restore FP state");
#endif
#if defined(NATIVE_TEST_MPFR)
  constexpr char reference_name[]="MPFR256 RNDN";
#else
  constexpr char reference_name[]="double libm";
#endif
  std::printf("atan2: %zu sampled %s pairs; %zu value checks; maximum %llu ULP; shapes/constexpr passed\n",rows.size(),reference_name,checks,static_cast<unsigned long long>(largest_ulp));
}
