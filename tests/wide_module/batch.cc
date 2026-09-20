// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>
import native.wide;

namespace sample {
  inline int element_calls = 0;
  inline int batch_calls = 0;
  struct value { int n; };
  value operator+(value a,value b) noexcept { ++element_calls; return {a.n+b.n}; }
  value operator-(value a,value b) noexcept { ++element_calls; return {a.n-b.n}; }
  value operator*(value a,value b) noexcept { ++element_calls; return {a.n*b.n}; }
  value fma(value a,value b,value c) noexcept { ++element_calls; return {a.n*b.n+c.n}; }
  value atan2(value y,value x) noexcept { ++element_calls; return {y.n-x.n}; }
  value exp(value a) noexcept { ++element_calls; return a; }
  std::pair<value,value> sincos(value a) noexcept { ++element_calls; return {a,a}; }
  template<std::size_t N> auto add(std::array<value,N> const & a,std::array<value,N> const & b) noexcept {
    ++batch_calls;
    auto const & [...x] = a; auto const & [...y] = b;
    return std::array<value,N>{value{x.n+y.n}...};
  }
  template<std::size_t N> auto sub(std::array<value,N> const & a,std::array<value,N> const & b) noexcept {
    ++batch_calls;
    auto const & [...x] = a; auto const & [...y] = b;
    return std::array<value,N>{value{x.n-y.n}...};
  }
  template<std::size_t N> auto mul(std::array<value,N> const & a,std::array<value,N> const & b) noexcept {
    ++batch_calls;
    auto const & [...x] = a; auto const & [...y] = b;
    return std::array<value,N>{value{x.n*y.n}...};
  }
  template<std::size_t N> auto fma(std::array<value,N> const & a,
      std::array<value,N> const & b,std::array<value,N> const & c) noexcept {
    ++batch_calls;
    auto const & [...x] = a; auto const & [...y] = b; auto const & [...z] = c;
    return std::array<value,N>{value{x.n*y.n+z.n}...};
  }
  template<std::size_t N> auto atan2(std::array<value,N> const & y,std::array<value,N> const & x) noexcept {
    ++batch_calls;
    auto const & [...a] = y; auto const & [...b] = x;
    return std::array<value,N>{value{a.n-b.n}...};
  }
  template<std::size_t N> auto exp(std::array<value,N> const & a) noexcept { ++batch_calls; return a; }
  template<std::size_t N> auto sincos(std::array<value,N> const & a) noexcept {
    ++batch_calls; return std::pair{a,a};
  }
  struct noncopy {
    int n;
    noncopy(noncopy const &) = delete;
    explicit noncopy(int x) : n(x) {}
  };
  template<std::size_t N> auto add(std::array<value,N> const & a,noncopy const & b) noexcept {
    ++batch_calls;
    auto const & [...x] = a;
    return std::array<value,N>{value{x.n+b.n}...};
  }
  struct lvalue_assignment {
    lvalue_assignment() noexcept = default;
    lvalue_assignment(lvalue_assignment const &) noexcept = default;
    lvalue_assignment & operator=(lvalue_assignment const &) noexcept(false);
    lvalue_assignment & operator=(lvalue_assignment &&) noexcept = default;
    friend auto sincos(lvalue_assignment const &) noexcept {
      return std::pair<lvalue_assignment,lvalue_assignment>{};
    }
  };
}
static_assert(noexcept(native::sincos(std::declval<native::wide<sample::value,3> const &>())));
static_assert(!noexcept(native::sincos(std::declval<native::wide<sample::lvalue_assignment,3> const &>())));
static_assert(noexcept(std::declval<native::wide<sample::value,3> &>() += std::declval<sample::noncopy const &>()));

template<std::size_t N> bool check() {
  using W = native::wide<sample::value,N>;
  W a(sample::value{2}), b(sample::value{3});
  sample::batch_calls = sample::element_calls = 0;
  auto sum = a+b;
  auto difference = b-a;
  auto product = a*b;
  auto fused = native::fma(a,b,a);
  auto exponential = native::exp(a);
  auto angle = native::atan2(a,b);
  auto [s,c] = native::sincos(a);
  a += b; a -= b; a *= b;
  sample::noncopy increment(4);
  a += increment;
  if (sample::batch_calls != 11 || sample::element_calls != 0) return false;
  for (std::size_t i=0; i<N; ++i)
    if (sum.registers[i].n!=5 || difference.registers[i].n!=1 || product.registers[i].n!=6 ||
        fused.registers[i].n!=8 || exponential.registers[i].n!=2 || s.registers[i].n!=2 ||
        c.registers[i].n!=2 || a.registers[i].n!=10 || angle.registers[i].n!=-1) return false;
  return true;
}
int main() { return check<0>() && check<1>() && check<3>() && check<12>() ? 0 : 1; }
