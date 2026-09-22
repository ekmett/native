#include "support/failure.h"
#include "support/profile.h"
#include "support/fp_environment.h"
#include "math_contract.h"
#include <array>
#include <bit>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// This fixture executes native VSCALEF only; other profiles test rejection.
#if defined(__AVX512F__)
namespace {
  using namespace test_simd;
  using word = std::uint32_t;
  constexpr word sign_bit=0x80000000u, infinity=0x7f800000u, quiet_nan=0x7fc00000u;
  void require(bool ok,char const * message) { if (!ok) native::test::fail(std::runtime_error(message)); }
  constexpr word magnitude(word x) { return x&0x7fffffffu; }
  constexpr bool is_nan(word x) { return magnitude(x)>infinity; }
  constexpr word flush(word x) { return magnitude(x)<0x00800000u ? x&sign_bit : x; }

  // This reference manipulates the exact dyadic significand. It calls neither
  // the production scaling helper nor scalbn/ldexp or floating arithmetic.
  constexpr word scale_integer(word x,int n,bool flush_result,bool before_rounding) {
    word a=magnitude(x),sign=x&sign_bit;
    if (a>=infinity || a==0) return x;
    int encoded=int(a>>23);
    word significand=(a&0x007fffffu)|(encoded ? 0x00800000u : 0u);
    int power=(encoded ? encoded-150 : -149)+n;
    int high=int(std::bit_width(significand))-1;
    int exponent=power+high;
    if (exponent>127) return sign|infinity;
    if (exponent>=-126)
      return sign|(word(exponent+127)<<23)|((significand<<(23-high))&0x007fffffu);
    if (flush_result && before_rounding) return sign;
    int shift=power+149;
    word rounded;
    if (shift>=0) rounded=significand<<shift;
    else if (shift < -24) rounded=0;
    else {
      unsigned right=unsigned(-shift);
      word quotient=significand>>right;
      word remainder=significand&((word(1)<<right)-1u);
      word half=word(1)<<(right-1);
      rounded=quotient+word(remainder>half || (remainder==half && (quotient&1u)));
    }
    if (flush_result && rounded<0x00800000u) return sign;
    return sign|rounded; // RNE can promote the largest tiny result to minnormal.
  }
  constexpr int floor_exponent(word x) {
    word a=magnitude(x);
    if (a==0) return 0;
    int e=int(a>>23)-127;
    bool negative=(x&sign_bit)!=0;
    if (e<0) return negative ? -1 : 0;
    if (e>=10) return negative ? -4096 : 4096; // Enough to settle every finite binary32 base.
    word significand=(a&0x007fffffu)|0x00800000u;
    unsigned shift=unsigned(23-e);
    int integral=int(significand>>shift);
    bool fractional=(significand&((word(1)<<shift)-1u))!=0;
    return negative ? -integral-int(fractional) : integral;
  }
  constexpr word reference(word x,word y,bool daz,bool output_flush,bool raw_before_rounding) {
    if (daz) { x=flush(x);y=flush(y); }
    word a=magnitude(x),b=magnitude(y);
    bool signaling=is_nan(x) && (x&0x00400000u)==0;
    if (signaling) return quiet_nan;
    if (is_nan(x)) {
      if (y==infinity) return infinity;
      if (y==(sign_bit|infinity)) return 0;
      return quiet_nan;
    }
    if (b>infinity) return quiet_nan;
    if (y==infinity) return a==0 ? quiet_nan : (x&sign_bit)|infinity;
    if (y==(sign_bit|infinity)) return a==infinity ? quiet_nan : x&sign_bit;
    return scale_integer(x,floor_exponent(y),output_flush,raw_before_rounding);
  }
  void check_reference() {
    static_assert(scale_integer(0x3fffffffu,-127,false,false)==0x00800000u);
    static_assert(scale_integer(0xbfffffffu,-127,true,false)==0x80800000u);
    static_assert(scale_integer(0x00fffffeu,-1,false,false)==0x007fffffu);
    static_assert(scale_integer(0x00800001u,-1,false,false)==0x00400000u);
    static_assert(scale_integer(0x00800003u,-1,false,false)==0x00400002u);
    static_assert(scale_integer(1,149,false,false)==0x3f800000u);
    static_assert(scale_integer(0x007fffffu,1,false,false)==0x00fffffeu);
    static_assert(scale_integer(0x7f7fffffu,1,false,false)==infinity);
    static_assert(floor_exponent(0xbf000000u)==-1 && floor_exponent(0x3fe00000u)==1);
    static_assert(floor_exponent(0x80000001u)==-1 && floor_exponent(0x80000000u)==0);
  }
  struct row { word value,exponent,prior; };
  std::vector<row> bank() {
    constexpr word values[]={0,sign_bit,1,0x80000001u,0x003fffffu,0x803fffffu,
      0x007fffffu,0x807fffffu,0x00800000u,0x80800000u,0x00800001u,0x00800003u,
      0x00fffffeu,0x00ffffffu,0x80ffffffu,0x3f000000u,0xbf000000u,
      0x3f800000u,0xbf800000u,0x3fffffffu,0xbfffffffu,0x40000000u,
      0x7f7fffffu,0xff7fffffu,infinity,sign_bit|infinity,
      quiet_nan,0xffc12345u,0x7f800001u,0xffa12345u};
    constexpr word special_exponents[]={0,sign_bit,1,0x80000001u,0x007fffffu,0x807fffffu,
      0x00800000u,0x80800000u,0x00800001u,0x80800001u,
      0x3f000000u,0xbf000000u,0x3fffffffu,0xbfffffffu,0x3f800000u,0xbf800000u,
      0x7f7fffffu,0xff7fffffu,infinity,sign_bit|infinity,quiet_nan,0x7f800001u,0xffa12345u};
    std::vector<row> rows;
    for (int exponent=-512;exponent<=512;++exponent)
      for (word x:values) rows.push_back({x,std::bit_cast<word>(float(exponent)),values[rows.size()%std::size(values)]});
    for (word x:values) for (word y:special_exponents)
      rows.push_back({x,y,values[rows.size()%std::size(values)]});
    auto below=[](word x) { return magnitude(x)==0 ? 0x80000001u : (x&sign_bit ? x+1u : x-1u); };
    auto above=[](word x) { return magnitude(x)==0 ? 1u : (x&sign_bit ? x-1u : x+1u); };
    // A maximum significand rounds up at the scaling interval [-E,1-E).
    // Exercise both neighboring float exponents and a fractional interior for
    // every normal biased exponent, with a nonpromoting significand control.
    for (unsigned e=1;e<255;++e) {
      word lower=std::bit_cast<word>(float(-int(e))),upper=std::bit_cast<word>(float(1-int(e)));
      word exponents[]={below(lower),lower,above(lower),std::bit_cast<word>(float(-int(e))+0.5f),below(upper),upper,above(upper)};
      for (word sign:{0u,sign_bit}) for (word fraction:{0x007ffffeu,0x007fffffu})
        for (word y:exponents) rows.push_back({sign|(e<<23)|fraction,y,values[rows.size()%std::size(values)]});
    }
    word state=0x5343414cu;
    auto next=[&] { state^=state<<13;state^=state>>17;state^=state<<5;return state; };
    for (unsigned i=0;i<2048;++i) {
      word x=next(),y=next(),prior=next();rows.push_back({x,y,prior});
    }
    return rows;
  }
  template<class V,class M> std::size_t check_mask(std::vector<row> const & rows,bool daz,bool output_flush,bool raw_before_rounding) {
    constexpr auto N=V::lanes;
    using E=test_vec<float,N>;
    static_assert(std::same_as<decltype(scaleb(std::declval<V>(),std::declval<E>())),V>);
    static_assert(std::same_as<decltype(masked_scaleb(std::declval<M>(),std::declval<V>(),std::declval<V>(),std::declval<E>())),V>);
    static_assert(std::same_as<decltype(masked_scaleb_zero(std::declval<M>(),std::declval<V>(),std::declval<E>())),V>);
    std::size_t checked=0;
    for (std::size_t start=0;start<rows.size();start+=N) {
      std::array<word,N> x{},y{},prior{};
      for (std::size_t lane=0;lane<N;++lane) {
        auto const & r=rows[(start+lane)%rows.size()];
        x[lane]=r.value;y[lane]=r.exponent;prior[lane]=r.prior;
      }
      auto vx=V::load_bits(x.data()),vp=V::load_bits(prior.data());auto vy=E::load_bits(y.data());
      auto inspect=[&](V result,std::uint64_t bits,bool zero) {
        std::array<word,N> actual{};result.store_bits(actual.data());
        for (std::size_t lane=0;lane<N;++lane) {
          bool active=(bits&(std::uint64_t(1)<<lane))!=0;
          word want=active?reference(x[lane],y[lane],daz,output_flush,raw_before_rounding):(zero?0:prior[lane]);
          bool equal=active?native::math_test::equivalent_fp32(actual[lane],want):actual[lane]==want;
          if (!equal) native::test::fail(std::runtime_error(std::string("raw")+" scaleb N="+std::to_string(N)+
            " row="+std::to_string(start+lane)+" active="+std::to_string(active)+" actual="+
            std::to_string(actual[lane])+" expected="+std::to_string(want)));
          ++checked;
        }
      };
      constexpr std::uint64_t all_bits=(std::uint64_t(1)<<N)-1;
      // None/all, complementary alternating masks and a moving single lane
      // exercise inactive raw copies, numeric-zero output, and lane ordering.
      std::uint64_t masks[]={0,all_bits,0xaaaaaaaaaaaaaaaaull&all_bits,
        0x5555555555555555ull&all_bits,std::uint64_t(1)<<((start/N)%N)};
      for (auto bits:masks) {
        auto mask=M::from_bitset(bits);
        inspect(masked_scaleb(mask,vp,vx,vy),bits,false);
        inspect(masked_scaleb_zero(mask,vx,vy),bits,true);
      }
      inspect(scaleb(vx,vy),all_bits,false);
    }
    return checked;
  }
  template<std::size_t N> std::size_t check_width(std::vector<row> const & rows,bool daz,bool output_flush,bool raw_before_rounding) {
    using F=test_vec<float,N>;using M=test_vec<mask32,N>;
    auto checked=check_mask<F,M>(rows,daz,output_flush,raw_before_rounding);

    if constexpr(!std::same_as<typename F::mask,M>) {
      checked+=check_mask<F,typename F::mask>(rows,daz,output_flush,raw_before_rounding);

    }
    return checked;
  }
}
int main(int argc,char ** argv) {
  {
    require(argc==3,"usage: scaleb gradual|daz|ftz|flush before-rounding|after-rounding");
    std::string mode=argv[1],profile=argv[2];require(mode=="gradual"||mode=="daz"||mode=="ftz"||mode=="flush","mode");
    require(profile=="before-rounding"||profile=="after-rounding","raw profile");
    bool daz=mode=="daz"||mode=="flush",output_flush=mode=="ftz"||mode=="flush";
    bool raw_before=profile=="before-rounding";
    check_reference();auto rows=bank();std::size_t checked=0;
    auto before=native::test::read_fp_state();

    {
      native::test::fp_scope scope(mode=="flush"?native::test::fp_mode::flush:native::test::fp_mode::gradual);
#if defined(_M_X64) || defined(__x86_64__)
      // The owner scope still restores the complete original state. The two
      // intermediate MXCSR modes exercise raw arithmetic only.
      _mm_setcsr((_mm_getcsr()&~0x8040u)|(daz?0x40u:0u)|(output_flush?0x8000u:0u));
#else
      require(mode=="gradual"||mode=="flush","Separate DAZ/FTZ modes require x86 MXCSR");
#endif
      auto requested=native::test::read_fp_state().control;
      checked+=check_width<1>(rows,daz,output_flush,raw_before);
#if defined(__AVX512VL__)
      checked+=check_width<2>(rows,daz,output_flush,raw_before);
      checked+=check_width<3>(rows,daz,output_flush,raw_before);
      checked+=check_width<4>(rows,daz,output_flush,raw_before);
#endif
#if defined(__AVX512VL__)
      checked+=check_width<8>(rows,daz,output_flush,raw_before);
#endif
#if defined(__AVX512F__) && defined(__AVX512DQ__)
      checked+=check_width<16>(rows,daz,output_flush,raw_before);
#endif
      require(native::test::read_fp_state().control==requested,"FP controls changed");
    }
    require(before==native::test::read_fp_state(),"FP environment not restored");
    std::cout<<"{\"passed\":true,\"mode\":\""<<mode<<"\",\"raw_profile\":\""<<profile
      <<"\",\"rows\":"<<rows.size()<<",\"checked_words\":"<<checked
      <<",\"raw_daz\":"<<(daz?"true":"false")<<",\"raw_ftz\":"<<(output_flush?"true":"false")
      <<",\"environment_restored\":true}\n";
  }
}

#else
int main() {
  std::cout<<"Native VSCALEF unavailable in the selected test profile.\n";
  return 77;
}
#endif

/**
 * \file
 * \license
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
 * SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
 * \endlicense
 * \author Edward Kmett <ekmett@gmail.com>
 * \brief Checks masked binary scaling against an independent integer reference.
 */
