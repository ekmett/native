// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "contract.h"
#include <string_view>
#if NATIVE_TEST_NUMERICS
import native.numerics;
#else
import native.types;
#endif

struct string {};
static_assert(sizeof(string) > 0);

static_assert(native::one_of_t<int, float, int>);
static_assert(!native::one_of_t<int>);
static_assert(native::not_one_of_t<int>);
static_assert(!native::one_of_t<int const, int>);
static_assert(native::one_of_t<int const, int const>);
static_assert(!native::one_of_t<int &, int>);
static_assert(native::one_of_t<int &, int &, float>);
static_assert(native::one_of_t<int[3], int[3]>);
static_assert(!native::one_of_t<int[3], int[4]>);
static_assert(!noexcept(native::type_of(1)));

template<class T> requires native::one_of_t<T, int, double>
constexpr int admitted(T) { return 1; }
static_assert(admitted(1) == 1 && admitted(1.0) == 1);

#if NATIVE_TEST_NUMERICS
static_assert(native::one_of<2, 1, 2>);
static_assert(native::cmpint<native::CMPINT::LT>(-1, 0));
static_assert(native::fp16::from_bits(0x3c00).to_bits() == 0x3c00);
#endif

int main() {
  if (native::type<int>.empty() || native::type<double>.empty()) return 1;
  if (native::type<int> == native::type<double>) return 2;
  if (native::type<fixture::sample>.empty() || native::type<int *>.empty()) return 3;
  fixture::sample const value{};
  if (native::type_of(value) != native::type<fixture::sample>) return 4;
  if (native::type<fixture::sample>.data() != fixture::name_storage()) return 5;
  if (native::type_of(1) != native::type<int>) return 6;
  if (!fixture::peer_checks()) return 7;
  return 0;
}
