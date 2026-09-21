// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <array>
#include <bit>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <type_traits>
#include <utility>

namespace native_test {
  inline constexpr std::uint64_t property_seed=0x243f6a8885a308d3ULL;

  // SplitMix64 gives the same corpus on every host and in constant evaluation.
  struct property_rng {
    std::uint64_t state=property_seed;
    constexpr std::uint64_t next() noexcept {
      auto x=(state+=0x9e3779b97f4a7c15ULL);
      x=(x^(x>>30))*0xbf58476d1ce4e5b9ULL;
      x=(x^(x>>27))*0x94d049bb133111ebULL;
      return x^(x>>31);
    }
    template<class T> constexpr T bits() noexcept {
      static_assert(std::is_integral_v<T> && sizeof(T)<=8);
      if constexpr(std::is_same_v<T,bool>) return (next()&1)!=0;
      else return std::bit_cast<T>(static_cast<std::make_unsigned_t<T>>(next()));
    }
    constexpr std::size_t index(std::size_t bound) noexcept {
      return bound ? std::size_t(next()%bound) : 0;
    }
  };

  template<std::size_t N,class Make>
  consteval auto property_cases(std::uint64_t seed,Make make) {
    using value=decltype(make(std::declval<property_rng &>()));
    std::array<value,N> result{};
    property_rng random{seed};
    for(auto & entry:result) entry=make(random);
    return result;
  }

  struct property_settings {
    std::uint64_t seed;
    std::size_t cases;
  };

  inline property_settings property_config(std::size_t default_cases=1024) {
    auto number=[](char const * name,std::uint64_t fallback) {
      auto text=std::getenv(name);
      if(!text) return fallback;
      char * end=nullptr;
      errno=0;
      auto value=std::strtoull(text,&end,0);
      if(errno || text==end || *end || *text=='-') {
        std::fprintf(stderr,"Invalid %s: %s\n",name,text);
        std::abort();
      }
      return static_cast<std::uint64_t>(value);
    };
    auto seed=number("NATIVE_TEST_SEED",property_seed);
    auto count=number("NATIVE_TEST_CASES",default_cases);
    if(!count || count>std::numeric_limits<std::size_t>::max()) {
      std::fputs("NATIVE_TEST_CASES must be a positive size_t value\n",stderr);
      std::abort();
    }
    return {seed,static_cast<std::size_t>(count)};
  }

  namespace property_detail {
    template<class T> void print(T const & value) {
      if constexpr(std::is_same_v<T,bool>) std::fputs(value?"true":"false",stderr);
      else if constexpr(std::is_integral_v<T> && sizeof(T)<=8)
        std::fprintf(stderr,"0x%0*llx",int(2*sizeof(T)),
          static_cast<unsigned long long>(static_cast<std::make_unsigned_t<T>>(value)));
      else if constexpr(std::is_enum_v<T>) print(static_cast<std::underlying_type_t<T>>(value));
      else if constexpr(std::is_same_v<T,float>) print(std::bit_cast<std::uint32_t>(value));
      else if constexpr(std::is_same_v<T,double>) print(std::bit_cast<std::uint64_t>(value));
      else if constexpr(requires { value.begin(); value.end(); }) {
        std::fputc('[',stderr);
        bool first=true;
        for(auto const & lane:value) {
          if(!first) std::fputs(", ",stderr);
          print(lane); first=false;
        }
        std::fputc(']',stderr);
      } else static_assert(sizeof(T)==0,"Print the case's scalar or array fields explicitly");
    }
  }

  template<class... Inputs>
  bool property_check(char const * name,std::uint64_t seed,std::size_t index,
      bool condition,Inputs const &... inputs) {
    if(condition) return true;
    std::fprintf(stderr,"Property %s failed: seed=0x%016llx case=%zu\n",
      name,static_cast<unsigned long long>(seed),index);
    std::size_t field=0;
    ([&] {
      std::fprintf(stderr,"  value[%zu]=",field++);
      property_detail::print(inputs);
      std::fputc('\n',stderr);
    }(),...);
    return false;
  }

  // The first two reported values are expected and actual; the rest are inputs.
  template<class Expected,class Actual,class... Inputs>
  bool property_equal(char const * name,std::uint64_t seed,std::size_t index,
      Expected const & expected,Actual const & actual,Inputs const &... inputs) {
    return property_check(name,seed,index,expected==actual,expected,actual,inputs...);
  }
}
