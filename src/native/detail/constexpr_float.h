// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace native::detail::constexpr_float {
  // Integer encodings keep constant evaluation independent of the host FP
  // environment. This implementation does not model exception status or traps.
  template<unsigned ExponentBits,unsigned FractionBits>
  struct binary_format {
    static_assert(ExponentBits>=2 && ExponentBits<=11 && FractionBits>=1 &&
      FractionBits<=52 && 1+ExponentBits+FractionBits<=64);
    static constexpr unsigned exponent_bits=ExponentBits;
    static constexpr unsigned fraction_bits=FractionBits;
    static constexpr unsigned width=1+ExponentBits+FractionBits;
    using bits_type=std::conditional_t<(width<=16),std::uint16_t,
      std::conditional_t<(width<=32),std::uint32_t,std::uint64_t>>;
    static constexpr std::uint64_t fraction_mask=(std::uint64_t{1}<<FractionBits)-1;
    static constexpr std::uint64_t exponent_max=(std::uint64_t{1}<<ExponentBits)-1;
    static constexpr std::uint64_t exponent_mask=exponent_max<<FractionBits;
    static constexpr std::uint64_t sign_mask=std::uint64_t{1}<<(width-1);
    static constexpr std::uint64_t quiet_mask=std::uint64_t{1}<<(FractionBits-1);
    static constexpr int bias=(1<<(ExponentBits-1))-1;
    static constexpr int min_normal_exponent=1-bias;
    static constexpr int min_subnormal_exponent=min_normal_exponent-int(FractionBits);
    static constexpr int max_normal_exponent=bias;
  };
  using binary16=binary_format<5,10>;
  using bfloat16=binary_format<8,7>;
  using binary32=binary_format<8,23>;
  using binary64=binary_format<11,52>;

  enum class rounding { nearest_even, downward, upward, toward_zero, to_odd };
  enum class nan_propagation { signaling_first, first, default_nan };
  enum class fma_nan_order { addend_first, multiplicands_first };

  struct policy {
    nan_propagation nan=nan_propagation::signaling_first;
    fma_nan_order fma_order=fma_nan_order::addend_first;
    bool flush_inputs=false;
    // Flush before rounding, including tiny values that would round to normal.
    bool flush_outputs=false;
    // Legacy BFloat16 dot arithmetic overflows to infinity even with round-odd.
    bool odd_overflow_infinity=false;
    bool default_nan_negative=false;
    // Arm AH=0: a quiet addend does not mask an invalid 0 * infinity product.
    bool invalid_product_overrides_quiet_addend=true;
  };

  template<class F> constexpr bool is_nan(typename F::bits_type x) noexcept {
    return (x&F::exponent_mask)==F::exponent_mask && (x&F::fraction_mask)!=0;
  }
  template<class F> constexpr bool is_signaling_nan(typename F::bits_type x) noexcept {
    return is_nan<F>(x) && !(x&F::quiet_mask);
  }
  template<class F> constexpr bool is_infinite(typename F::bits_type x) noexcept {
    return (x&~F::sign_mask)==F::exponent_mask;
  }
  template<class F> constexpr bool is_zero(typename F::bits_type x) noexcept {
    return (x&~F::sign_mask)==0;
  }
  template<class F> constexpr typename F::bits_type quiet_nan(typename F::bits_type x) noexcept {
    return typename F::bits_type(x|F::quiet_mask);
  }
  template<class F> constexpr typename F::bits_type default_nan(policy p={}) noexcept {
    return typename F::bits_type(F::exponent_mask|F::quiet_mask|
      (p.default_nan_negative?F::sign_mask:0));
  }
  template<class F> constexpr typename F::bits_type flush_input(
      typename F::bits_type x,policy p) noexcept {
    return p.flush_inputs && !(x&F::exponent_mask)
      ?typename F::bits_type(x&F::sign_mask):x;
  }

  template<class F,std::size_t N>
  constexpr typename F::bits_type select_nan(
      std::array<typename F::bits_type,N> const & operands,policy p) noexcept {
    if(p.nan==nan_propagation::default_nan) return default_nan<F>(p);
    if(p.nan==nan_propagation::signaling_first)
      for(auto x:operands) if(is_signaling_nan<F>(x)) return quiet_nan<F>(x);
    for(auto x:operands) if(is_nan<F>(x)) return quiet_nan<F>(x);
    return default_nan<F>(p);
  }

  struct finite_parts { bool sign; std::uint64_t significand; int exponent; };
  template<class F> constexpr finite_parts unpack(typename F::bits_type x) noexcept {
    auto e=unsigned((x&F::exponent_mask)>>F::fraction_bits);
    return {(x&F::sign_mask)!=0,
      (x&F::fraction_mask)|(e?std::uint64_t{1}<<F::fraction_bits:0),
      e?int(e)-F::bias-int(F::fraction_bits):F::min_subnormal_exponent};
  }

  // Fixed-capacity unsigned magnitude. Arithmetic uses only uint64_t, including
  // on Windows targets without a language-level 128-bit integer type.
  template<std::size_t N> struct magnitude {
    std::array<std::uint64_t,N> words{};
    constexpr unsigned bit_width() const noexcept {
      for(std::size_t i=N;i>0;--i)
        if(words[i-1]) return unsigned((i-1)*64+std::bit_width(words[i-1]));
      return 0;
    }
    constexpr void insert(std::uint64_t x,unsigned shift) noexcept {
      auto i=shift/64;
      auto n=shift%64;
      if(i<N) words[i]|=x<<n;
      if(n && i+1<N) words[i+1]|=x>>(64-n);
    }
    constexpr std::uint64_t extract(unsigned shift) const noexcept {
      auto i=shift/64;
      auto n=shift%64;
      auto x=i<N?words[i]>>n:0;
      if(n && i+1<N) x|=words[i+1]<<(64-n);
      return x;
    }
    constexpr bool below(unsigned bit) const noexcept {
      auto full=bit/64;
      for(std::size_t i=0;i<N && i<full;++i) if(words[i]) return true;
      return full<N && bit%64 && (words[full]&((std::uint64_t{1}<<(bit%64))-1));
    }
    constexpr int compare(magnitude const & other) const noexcept {
      for(std::size_t i=N;i>0;--i)
        if(words[i-1]!=other.words[i-1]) return words[i-1]>other.words[i-1]?1:-1;
      return 0;
    }
    constexpr void add(magnitude const & other) noexcept {
      bool carry=false;
      for(std::size_t i=0;i<N;++i) {
        auto x=words[i],sum=x+other.words[i];
        auto next=sum+std::uint64_t(carry);
        carry=sum<x || next<sum;
        words[i]=next;
      }
    }
    // Precondition: *this >= other.
    constexpr void subtract(magnitude const & other) noexcept {
      bool borrow=false;
      for(std::size_t i=0;i<N;++i) {
        auto x=words[i],difference=x-other.words[i];
        auto next=difference-std::uint64_t(borrow);
        borrow=x<other.words[i] || difference<std::uint64_t(borrow);
        words[i]=next;
      }
    }
  };

  struct double_word { std::uint64_t low,high; };
  constexpr double_word multiply(std::uint64_t a,std::uint64_t b) noexcept {
    constexpr auto mask=std::uint64_t{0xffffffff};
    auto a0=a&mask,a1=a>>32,b0=b&mask,b1=b>>32;
    auto low=a0*b0;
    auto middle=a1*b0+(low>>32);
    auto high=middle>>32;
    middle=(middle&mask)+a0*b1;
    return {(middle<<32)|(low&mask),a1*b1+high+(middle>>32)};
  }

  template<class F> constexpr typename F::bits_type overflow(
      bool sign,rounding mode,policy p) noexcept {
    bool infinity=mode==rounding::nearest_even ||
      (mode==rounding::upward && !sign) || (mode==rounding::downward && sign) ||
      (mode==rounding::to_odd && p.odd_overflow_infinity);
    return typename F::bits_type((sign?F::sign_mask:0)|
      (infinity?F::exponent_mask:F::exponent_mask-1));
  }

  // Round sign * value * 2^exponent once to F. All discarded bits remain
  // available, even after cancellation of a product and a distant addend.
  template<class F,std::size_t N>
  constexpr typename F::bits_type round_pack(bool sign,magnitude<N> const & value,
      int exponent,rounding mode=rounding::nearest_even,policy p={}) noexcept {
    auto sign_bits=sign?F::sign_mask:0;
    auto width=value.bit_width();
    if(!width) return typename F::bits_type(sign_bits);
    auto highest=exponent+int(width)-1;
    if(p.flush_outputs && highest<F::min_normal_exponent)
      return typename F::bits_type(sign_bits);
    auto quantum=highest-int(F::fraction_bits);
    if(quantum<F::min_subnormal_exponent) quantum=F::min_subnormal_exponent;
    auto shift=quantum-exponent;
    auto mantissa=shift>=0?value.extract(unsigned(shift)):value.words[0]<<unsigned(-shift);
    bool guard=shift>0 && (value.extract(unsigned(shift-1))&1);
    bool sticky=shift>1 && value.below(unsigned(shift-1));
    bool inexact=guard || sticky;
    bool increment=(mode==rounding::nearest_even && guard && (sticky || (mantissa&1))) ||
      (mode==rounding::downward && sign && inexact) ||
      (mode==rounding::upward && !sign && inexact);
    if(mode==rounding::to_odd && inexact) mantissa|=1;
    else if(increment) ++mantissa;
    if(mantissa>=(std::uint64_t{1}<<(F::fraction_bits+1))) {mantissa>>=1;++quantum;}
    auto encoded_exponent=mantissa>=(std::uint64_t{1}<<F::fraction_bits)
      ?quantum+int(F::fraction_bits)+F::bias:0;
    if(encoded_exponent>=int(F::exponent_max)) return overflow<F>(sign,mode,p);
    return typename F::bits_type(sign_bits|(std::uint64_t(encoded_exponent)<<F::fraction_bits)|
      (mantissa&F::fraction_mask));
  }

  // Resize a NaN payload. Mixed-precision arithmetic can retain a signaling
  // operand until the operation selects among all its NaNs; FP conversions
  // themselves always request quieting. Precondition: x is a NaN encoding.
  template<class To,class From>
  constexpr typename To::bits_type resize_nan(typename From::bits_type x,
      bool quiet=true) noexcept {
    auto payload=std::uint64_t(x&From::fraction_mask);
    if constexpr(To::fraction_bits>=From::fraction_bits)
      payload<<=To::fraction_bits-From::fraction_bits;
    else payload>>=From::fraction_bits-To::fraction_bits;
    if(quiet) payload|=To::quiet_mask;
    else if(!payload) payload=1;
    return typename To::bits_type(((x&From::sign_mask)?To::sign_mask:0)|To::exponent_mask|payload);
  }

  template<class To,class From>
  constexpr typename To::bits_type convert_bits(typename From::bits_type x,
      rounding mode=rounding::nearest_even,policy p={}) noexcept {
    auto sign=(x&From::sign_mask)!=0;
    auto sign_bits=sign?To::sign_mask:0;
    if(is_nan<From>(x)) {
      if(p.nan==nan_propagation::default_nan) return default_nan<To>(p);
      return resize_nan<To,From>(x);
    }
    if(is_infinite<From>(x)) return typename To::bits_type(sign_bits|To::exponent_mask);
    auto parts=unpack<From>(flush_input<From>(x,p));
    magnitude<1> value{{parts.significand}};
    return round_pack<To>(sign,value,parts.exponent,mode,p);
  }

  // Capacity includes the full exponent span of two products plus a carry.
  template<class F> inline constexpr std::size_t arithmetic_words=
    (2*(F::max_normal_exponent-F::min_subnormal_exponent)+3+63)/64;

  template<class F,std::size_t N>
  constexpr typename F::bits_type sum_magnitudes(magnitude<N> a,bool sign_a,
      magnitude<N> b,bool sign_b,int exponent,rounding mode,policy p) noexcept {
    if(sign_a==sign_b) {
      a.add(b);
      return round_pack<F>(sign_a,a,exponent,mode,p);
    }
    auto order=a.compare(b);
    if(!order) return typename F::bits_type(mode==rounding::downward?F::sign_mask:0);
    if(order>0) {a.subtract(b);return round_pack<F>(sign_a,a,exponent,mode,p);}
    b.subtract(a);
    return round_pack<F>(sign_b,b,exponent,mode,p);
  }

  template<class F>
  constexpr typename F::bits_type add_bits(typename F::bits_type x,typename F::bits_type y,
      rounding mode=rounding::nearest_even,policy p={}) noexcept {
    x=flush_input<F>(x,p);y=flush_input<F>(y,p);
    if(is_nan<F>(x) || is_nan<F>(y)) return select_nan<F>(std::array{x,y},p);
    bool ix=is_infinite<F>(x),iy=is_infinite<F>(y);
    if(ix && iy && ((x^y)&F::sign_mask)) return default_nan<F>(p);
    if(ix || iy) return ix?x:y;
    auto a=unpack<F>(x),b=unpack<F>(y);
    auto exponent=a.exponent<b.exponent?a.exponent:b.exponent;
    magnitude<arithmetic_words<F>> av{},bv{};
    av.insert(a.significand,unsigned(a.exponent-exponent));
    bv.insert(b.significand,unsigned(b.exponent-exponent));
    return sum_magnitudes<F>(av,a.sign,bv,b.sign,exponent,mode,p);
  }

  template<class F>
  constexpr typename F::bits_type mul_bits(typename F::bits_type x,typename F::bits_type y,
      rounding mode=rounding::nearest_even,policy p={}) noexcept {
    x=flush_input<F>(x,p);y=flush_input<F>(y,p);
    if(is_nan<F>(x) || is_nan<F>(y)) return select_nan<F>(std::array{x,y},p);
    bool ix=is_infinite<F>(x),iy=is_infinite<F>(y);
    if((ix && is_zero<F>(y)) || (iy && is_zero<F>(x))) return default_nan<F>(p);
    auto a=unpack<F>(x),b=unpack<F>(y);
    bool sign=a.sign!=b.sign;
    if(ix || iy) return typename F::bits_type((sign?F::sign_mask:0)|F::exponent_mask);
    auto product=multiply(a.significand,b.significand);
    magnitude<2> value{{product.low,product.high}};
    return round_pack<F>(sign,value,a.exponent+b.exponent,mode,p);
  }

  // Computes x*y+addend with one rounding. NaN order is explicit: the default
  // examines addend,x,y for signaling NaNs, then the same order for quiet NaNs.
  template<class F>
  constexpr typename F::bits_type fma_bits(typename F::bits_type x,typename F::bits_type y,
      typename F::bits_type addend,rounding mode=rounding::nearest_even,policy p={}) noexcept {
    x=flush_input<F>(x,p);y=flush_input<F>(y,p);addend=flush_input<F>(addend,p);
    bool ix=is_infinite<F>(x),iy=is_infinite<F>(y),iz=is_infinite<F>(addend);
    bool invalid=(ix && is_zero<F>(y)) || (iy && is_zero<F>(x));
    if(is_nan<F>(x) || is_nan<F>(y) || is_nan<F>(addend)) {
      if(invalid && is_nan<F>(addend) && !is_signaling_nan<F>(addend) &&
          p.invalid_product_overrides_quiet_addend) return default_nan<F>(p);
      return select_nan<F>(p.fma_order==fma_nan_order::addend_first
        ?std::array{addend,x,y}:std::array{x,y,addend},p);
    }
    auto a=unpack<F>(x),b=unpack<F>(y),c=unpack<F>(addend);
    bool sign=a.sign!=b.sign;
    if(invalid || ((ix || iy) && iz && sign!=c.sign)) return default_nan<F>(p);
    if(ix || iy) return typename F::bits_type((sign?F::sign_mask:0)|F::exponent_mask);
    if(iz) return addend;
    auto exponent_product=a.exponent+b.exponent;
    auto exponent=exponent_product<c.exponent?exponent_product:c.exponent;
    auto product=multiply(a.significand,b.significand);
    magnitude<arithmetic_words<F>> pv{},cv{};
    pv.insert(product.low,unsigned(exponent_product-exponent));
    pv.insert(product.high,unsigned(exponent_product-exponent)+64);
    cv.insert(c.significand,unsigned(c.exponent-exponent));
    return sum_magnitudes<F>(pv,sign,cv,c.sign,exponent,mode,p);
  }
}
