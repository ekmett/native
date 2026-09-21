// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <concepts>
#include <cstdlib>
#include <type_traits>
import native.wide;
namespace custom {
  inline int calls=0, live=0;
  struct value {
    int n;
    value()=delete;
    explicit value(int x) noexcept : n(x) { ++live; }
    value(value const & x) noexcept : n(x.n) { ++live; }
    value(value && x) noexcept : n(x.n) { ++live; }
    ~value() { --live; }
  };
  value floor(value const & x) { ++calls;
#if defined(__cpp_exceptions)
    if(x.n==3) throw 31;
#endif
    return value(x.n);
  }
  value ceil(value const & x) noexcept { ++calls;return value(x.n); }
  value trunc(value const & x) noexcept { ++calls;return value(x.n); }
  struct different {};
  int floor(different const &) noexcept { return 0; }
}
template<class T> concept has_floor = requires(T const & x) { native::floor(x); };
static_assert(!has_floor<native::wide<custom::different,1>>);
using W=native::wide<custom::value,3>;
static_assert(std::same_as<decltype(native::floor(std::declval<W const &>())),W>);
static_assert(!noexcept(native::floor(std::declval<W const &>())));
static_assert(noexcept(native::ceil(std::declval<W const &>())));
static_assert(noexcept(native::trunc(std::declval<W const &>())));
static_assert(noexcept(native::floor(std::declval<native::wide<custom::value,0> const &>())));
int main() {
  {
    native::wide<custom::value,0> empty{};
    (void)native::floor(empty);(void)native::ceil(empty);(void)native::trunc(empty);
    if(custom::calls || custom::live) std::abort();
    W x{std::array{custom::value(1),custom::value(2),custom::value(3)}};
    (void)native::ceil(x);(void)native::trunc(x);
    if(custom::calls!=6 || custom::live!=3) std::abort();
#if defined(__cpp_exceptions)
    bool caught=false;
    try { (void)native::floor(x); } catch(int e) { caught=e==31; }
    if(!caught || custom::live!=3 || custom::calls!=9) std::abort();
#else
    (void)native::floor(x);
    if(custom::live!=3 || custom::calls!=9) std::abort();
#endif
  }
  if(custom::live) std::abort();
}
