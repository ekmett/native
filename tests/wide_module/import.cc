// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <concepts>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>
import native.wide;

namespace sample {
  struct value { int n; };
  constexpr value operator+(value a, value b) { return {a.n+b.n}; }
  constexpr value & operator+=(value & a, value b) { a.n+=b.n; return a; }
  struct move_only {
    int n;
    constexpr explicit move_only(int value) noexcept : n(value) {}
    constexpr move_only(move_only &&) noexcept = default;
    move_only(move_only const &) = delete;
  };
  struct copy_guard {
    int n;
    constexpr explicit copy_guard(int x) noexcept : n(x) {}
    constexpr copy_guard(copy_guard const &) noexcept = default;
    copy_guard(copy_guard &) noexcept(false);
  };
}
template<class T, std::size_t N>
constexpr int sum(native::wide<T,N> const & input) {
  auto const & [...values] = input;
  return (0 + ... + values);
}
template<class... T> concept deduces = requires(T... values) { native::wide{values...}; };
static_assert(!deduces<int,float>);
static_assert(std::same_as<decltype(native::wide{1}),native::wide<int,1>>);
static_assert(std::same_as<decltype(native::wide{1,2,3}),native::wide<int,3>>);
static_assert(std::tuple_size_v<native::wide<int,0>> == 0);
static_assert(std::tuple_size_v<native::wide<int,3>> == 3);
static_assert(std::same_as<std::tuple_element_t<1,native::wide<int,3>>,int>);
static_assert(std::is_convertible_v<native::wide<int,3>,native::wide<long,3>>);
static_assert(!std::is_convertible_v<native::wide<int,3>,native::wide<sample::move_only,3>>);
static_assert(!std::is_constructible_v<native::wide<int,2>,native::wide<int,3>>);
static_assert(std::is_nothrow_constructible_v<native::wide<sample::copy_guard,3>,int>);
static_assert(std::same_as<decltype(std::declval<native::wide<int,3> &>().get<0>()),int &>);
static_assert(std::same_as<decltype(std::declval<native::wide<int,3> const &>().get<0>()),int const &>);
static_assert(std::same_as<decltype(std::declval<native::wide<int,3> &&>().get<0>()),int &&>);
static_assert(std::same_as<decltype(std::declval<native::wide<int,3> const &&>().get<0>()),int const &&>);
constexpr bool check() {
  native::wide<int,0> empty;
  if (sum(empty) != 0 || sum(empty + empty) != 0) return false;
  native::wide single{9};
  native::wide<int,3> broadcast{4};
  native::wide from_array{std::array<int,3>{1,2,3}};
  static_assert(std::same_as<decltype(from_array),native::wide<int,3>>);
  if (sum(single)!=9 || sum(broadcast)!=12 || sum(from_array)!=6) return false;
  auto const added=from_array+2;
  if (sum(added)!=12 || sum(2+from_array)!=12) return false;
  from_array += from_array.get<0>(); // Aliased scalar is snapshotted.
  if (sum(from_array)!=9) return false;
  native::wide<long,3> converted=from_array;
  if (sum(converted)!=9) return false;
  native::wide<sample::move_only,3> moved{native::wide{1,2,3}};
  auto again=std::move(moved);
  if (again.get<0>().n!=1 || again.get<2>().n!=3) return false;
  native::wide<sample::copy_guard,3> guarded{5};
  if (guarded.get<2>().n!=5) return false;
  auto adl=native::wide{sample::value{1},sample::value{2}} + sample::value{3};
  adl += sample::value{4};
  if (adl.get<0>().n!=8 || adl.get<1>().n!=9) return false;
  return converted.apply([](auto... x) { return (0 + ... + x); }) == 9;
}
static_assert(check());
int main() { return check() ? 0 : 1; }
