// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <type_traits>
#include <utility>
import native.wide;

namespace sample {
  inline int calls=0,live=0,moves=0,fail_move=0;
  inline bool fail_call=false,fail_copy=false;
  struct failure {};
  struct mask {
    bool value;
    mask()=delete;
    explicit mask(bool x) noexcept : value(x) { ++live; }
    mask(mask const & x) noexcept(false) : value(x.value) {
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
      if(fail_copy)throw failure{};
#endif
      ++live;
    }
    mask(mask && x) noexcept(false) : value(x.value) {
      ++moves;
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
      if(moves==fail_move)throw failure{};
#endif
      ++live;
    }
    mask & operator=(mask const &)=delete;
    ~mask() { --live; }
  };
  struct value { int n; };
  mask classify(value const & x) {
    ++calls;
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
    if(fail_call)throw failure{};
#endif
    return mask(x.n!=0);
  }
  mask isfinite(value const & x) {return classify(x);}
  mask isinf(value const & x) {return classify(x);}
  mask isnan(value const & x) {return classify(x);}
  mask signbit(value const & x) {return classify(x);}
  value copysign(value const & a,value const & b) {
    ++calls;
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
    if(fail_call)throw failure{};
#endif
    return {b.n<0 ? -std::abs(a.n) : std::abs(a.n)};
  }
  struct reference {mask const * p;};
  mask const & isfinite(reference const & x) noexcept {return *x.p;}
  struct immobile {immobile()=delete;immobile(immobile const &)=delete;};
  immobile const & copysign(immobile const &,immobile const &) noexcept;
  struct empty_only {};
  immobile const & isfinite(empty_only const &) noexcept;
  struct no_operation {};
  struct void_operation {};
  void isfinite(void_operation const &) noexcept;
  struct trivial {bool value;};
  constexpr bool isfinite(trivial const & x) noexcept {return x.value;}
  constexpr trivial copysign(trivial const & a,trivial const &) noexcept {return a;}
}

template<class T>concept has_finite=requires(T const & x){native::isfinite(x);};
template<class T,class U=T>concept has_copysign=requires(T const & x,U const & y){native::copysign(x,y);};
using W=native::wide<sample::value,3>;
static_assert(std::same_as<decltype(native::isfinite(std::declval<W const &>())),native::wide<sample::mask,3>>);
static_assert(std::same_as<decltype(native::isinf(std::declval<W const &>())),native::wide<sample::mask,3>>);
static_assert(std::same_as<decltype(native::isnan(std::declval<W const &>())),native::wide<sample::mask,3>>);
static_assert(std::same_as<decltype(native::signbit(std::declval<W const &>())),native::wide<sample::mask,3>>);
static_assert(!has_finite<native::wide<sample::no_operation,3>>);
static_assert(!has_finite<native::wide<sample::void_operation,3>>);
static_assert(has_finite<native::wide<sample::empty_only,0>>);
static_assert(!has_finite<native::wide<sample::empty_only,1>>);
static_assert(!has_copysign<native::wide<float,2>,native::wide<double,2>>);
static_assert(!has_copysign<native::wide<float,2>,native::wide<float,3>>);
static_assert(!has_copysign<native::wide<int,2>>);
static_assert(has_copysign<native::wide<sample::immobile,0>>);
static_assert(!has_copysign<native::wide<sample::immobile,1>>);
static_assert(noexcept(native::copysign(std::declval<native::wide<sample::immobile,0> const &>(),std::declval<native::wide<sample::immobile,0> const &>())));
static_assert(!noexcept(native::isfinite(std::declval<W const &>())));
static_assert(!noexcept(native::isfinite(std::declval<native::wide<sample::reference,1> const &>())));
static_assert(noexcept(native::isfinite(std::declval<native::wide<sample::empty_only,0> const &>())));
static_assert(noexcept(native::isfinite(std::declval<native::wide<sample::trivial,3> const &>())));
static_assert(noexcept(native::copysign(std::declval<native::wide<sample::trivial,3> const &>(),std::declval<native::wide<sample::trivial,3> const &>())));
static_assert(!noexcept(native::copysign(std::declval<W const &>(),std::declval<W const &>())));
static_assert(native::isfinite(native::wide{sample::trivial{true}}).registers[0]);

void require(bool x){if(!x)std::abort();}
bool value_without_headers();

template<std::size_t N>void custom() {
  native::wide<sample::value,N> input(sample::value{1});
  sample::calls=sample::moves=0;
  {
    auto finite=native::isfinite(input);auto inf=native::isinf(input);
    auto nan=native::isnan(input);auto sign=native::signbit(input);
    for(std::size_t i=0;i<N;++i)require(finite.registers[i].value && inf.registers[i].value && nan.registers[i].value && sign.registers[i].value);
    require(sample::calls==int(4*N));
    (void)native::isfinite(input);require(sample::calls==int(5*N));
    auto signed_value=native::copysign(input,native::wide<sample::value,N>{sample::value{-1}});
    require(sample::calls==int(6*N));
    for(std::size_t i=0;i<N;++i)require(signed_value.registers[i].n==-1);
  }
  require(sample::live==0);
}
void scalar() {
  constexpr std::array<std::uint32_t,8> bits{0,0x80000000u,0x3f800000u,0xbf800000u,0x7f800000u,0xff800000u,0x7fc12345u,0xffc12345u};
  std::array<float,bits.size()> values{};
  for(std::size_t i=0;i<bits.size();++i)values[i]=std::bit_cast<float>(bits[i]);
  native::wide input(values);
  auto finite=native::isfinite(input);auto inf=native::isinf(input);auto nan=native::isnan(input);auto sign=native::signbit(input);
  static_assert(std::same_as<decltype(finite),native::wide<bool,bits.size()>>);
  auto signed_value=native::copysign(input,native::wide<float,bits.size()>{-1.0f});
  for(std::size_t i=0;i<bits.size();++i){
    require(finite.registers[i]==std::isfinite(values[i]));require(inf.registers[i]==std::isinf(values[i]));
    require(nan.registers[i]==std::isnan(values[i]));require(sign.registers[i]==std::signbit(values[i]));
    require(std::bit_cast<std::uint32_t>(signed_value.registers[i])==(bits[i]|0x80000000u));
  }
  auto empty=native::isfinite(native::wide<sample::empty_only,0>{});
  static_assert(std::same_as<decltype(empty),native::wide<sample::immobile,0>>);
  auto copied_empty=native::copysign(empty,empty);(void)copied_empty;
}
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
struct guard {bool & destroyed;~guard(){destroyed=true;}};
template<class F>void catches(F f){bool destroyed=false,caught=false;try{guard g{destroyed};f();}catch(sample::failure const &){caught=true;}require(destroyed&&caught);}
void exceptions(){
  W input(sample::value{1});sample::fail_call=true;
  catches([&]{(void)native::isfinite(input);});catches([&]{(void)native::isinf(input);});
  catches([&]{(void)native::isnan(input);});catches([&]{(void)native::signbit(input);});
  catches([&]{(void)native::copysign(input,input);});sample::fail_call=false;
  sample::moves=0;sample::fail_move=2;catches([&]{(void)native::isfinite(input);});sample::fail_move=0;require(sample::live==0);
  {sample::mask mask(true);sample::fail_copy=true;native::wide input_ref{sample::reference{&mask}};
   catches([&]{(void)native::isfinite(input_ref);});sample::fail_copy=false;require(sample::live==1);}
  require(sample::live==0);
  sample::fail_call=true;auto empty=native::isfinite(native::wide<sample::value,0>{});(void)empty;sample::fail_call=false;
}
#endif
int main(){require(value_without_headers());scalar();custom<0>();custom<1>();custom<3>();
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
  exceptions();
#endif
}
