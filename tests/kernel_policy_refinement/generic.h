// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
namespace generic_test {
  inline unsigned arrays=0,elements=0,cleanups=0;
  struct value {
    int word{};
    friend value operator+(value a,value b) noexcept { ++elements;return {a.word+b.word}; }
    friend value abs(value a) noexcept { ++elements;return {a.word<0?-a.word:a.word}; }
#define ELEMENT(name) friend value name(value a) noexcept { ++elements;return {a.word+1}; }
    ELEMENT(sqrt) ELEMENT(floor) ELEMENT(ceil) ELEMENT(trunc)
    ELEMENT(sin) ELEMENT(cos) ELEMENT(exp) ELEMENT(expm1)
    ELEMENT(tanh) ELEMENT(log) ELEMENT(log1p)
#undef ELEMENT
    friend value atan2(value a,value b) noexcept {++elements;return {a.word-b.word};}
    friend value fma(value a,value b,value c) noexcept {++elements;return {a.word*b.word+c.word};}
    friend bool isfinite(value a) noexcept {++elements;return a.word!=99;}
    friend bool isinf(value a) noexcept {++elements;return a.word==99;}
    friend bool isnan(value a) noexcept {++elements;return a.word==100;}
    friend bool signbit(value a) noexcept {++elements;return a.word<0;}
    friend value copysign(value a,value b) noexcept {
      ++elements;auto magnitude=a.word<0?-a.word:a.word;
      return {b.word<0?-magnitude:magnitude};
    }
    friend std::pair<value,value> sincos(value a) noexcept {++elements;return {value{a.word+1},value{a.word+2}};}
  };
  template<std::size_t N> auto add(std::array<value,N> a,std::array<value,N> const & b) noexcept {
    ++arrays;
    for(std::size_t i=0;i<N;++i) a[i].word+=b[i].word;
    return a;
  }
  template<std::size_t N> auto abs(std::array<value,N> a) noexcept {
    ++arrays;
    for(auto & x:a) if(x.word<0) x.word=-x.word;
    return a;
  }
#define ARRAY(name) template<std::size_t N> auto name(std::array<value,N> a) noexcept { \
    ++arrays;for(auto & x:a) ++x.word;return a; }
  ARRAY(sqrt) ARRAY(floor) ARRAY(ceil) ARRAY(trunc)
  ARRAY(sin) ARRAY(cos) ARRAY(exp) ARRAY(expm1)
  ARRAY(tanh) ARRAY(log) ARRAY(log1p)
#undef ARRAY
  template<std::size_t N> auto atan2(std::array<value,N> a,std::array<value,N> const & b) noexcept {
    ++arrays;for(std::size_t i=0;i<N;++i) a[i].word-=b[i].word;return a;
  }
  template<std::size_t N> auto fma(std::array<value,N> a,std::array<value,N> const & b,std::array<value,N> const & c) noexcept {
    ++arrays;for(std::size_t i=0;i<N;++i) a[i].word=a[i].word*b[i].word+c[i].word;return a;
  }
  template<std::size_t N> auto sincos(std::array<value,N> a) noexcept {
    ++arrays;auto b=a;for(auto & x:a) ++x.word;for(auto & x:b) x.word+=2;return std::pair{a,b};
  }
  struct failure {};
  struct cleanup { ~cleanup() noexcept {++cleanups;} };
  struct throwing {
    int word{};
    friend throwing abs(throwing a) noexcept(false) {
      cleanup guard;
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
      throw failure{};
#else
      return a;
#endif
    }
  };
  struct converted {
    int word{};
    converted(value const & x) noexcept :word(x.word) {}
  };
  struct throwing_conversion {
    throwing_conversion(value const &) noexcept(false) {}
  };
  static_assert(noexcept(abs(simd::wide<value,3>{})));
  static_assert(!noexcept(abs(simd::wide<throwing,3>{})));
  static_assert(!std::is_nothrow_constructible_v<simd::wide<throwing_conversion,3>,simd::wide<value,3> const &>);
  bool check() {
    using W=simd::wide<value,3>;
    W a{value{-1},value{2},value{-3}};
    arrays=elements=0;
    auto b=a+a;
    auto c=abs(a);
    if(arrays!=2 || elements!=0 || b.registers[2].word!=-6 || c.registers[0].word!=1) return false;
    simd::wide<converted,3> d{a};
    if(d.registers[2].word!=-3) return false;
    simd::wide<value,0> empty{};
    if(empty.apply([]{return 17;})!=17) return false;
    auto e=abs(empty);
    if(!e.registers.empty() || arrays!=3 || elements!=0) return false;
#define CHECK(name) {auto before=arrays;auto result=simd::name(a); \
    if(arrays!=before+1 || elements!=0 || result.registers[0].word!=0) return false;}
    CHECK(sqrt)
    CHECK(sin) CHECK(cos) CHECK(exp) CHECK(expm1)
    CHECK(tanh) CHECK(log) CHECK(log1p)
#undef CHECK
#define CHECK_ELEMENT(name) {auto before=arrays;auto count=elements;auto result=simd::name(a); \
    if(arrays!=before || elements!=count+3 || result.registers[0].word!=0) return false;}
    CHECK_ELEMENT(floor) CHECK_ELEMENT(ceil) CHECK_ELEMENT(trunc)
#undef CHECK_ELEMENT
    elements=0;
    auto before=arrays;
    auto angle=simd::atan2(a,a);auto fused=simd::fma(a,a,a);auto pair=simd::sincos(a);
    if(arrays!=before+3 || elements!=0 || angle.registers[0].word!=0 ||
       fused.registers[0].word!=0 || pair.first.registers[0].word!=0 ||
       pair.second.registers[0].word!=1) return false;
    before=elements;
    auto finite=simd::isfinite(a);auto infinite=simd::isinf(a);
    auto nan=simd::isnan(a);auto sign=simd::signbit(a);
    auto signed_copy=simd::copysign(c,a);
    if(elements!=before+15 || !finite.registers[0] || infinite.registers[0] ||
       nan.registers[0] || !sign.registers[0] || signed_copy.registers[0].word!=-1) return false;
    auto scalar=simd::sqrt(simd::wide<float,2>{4.f,9.f});
    if(scalar.registers[0]!=2.f || scalar.registers[1]!=3.f) return false;
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
    cleanups=0;
    try { auto unused=abs(simd::wide<throwing,1>{throwing{4}});(void)unused;return false; }
    catch(failure const &) {}
    if(cleanups!=1) return false;
#endif
    return true;
  }
}
