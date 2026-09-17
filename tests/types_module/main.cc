// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "contract.h"
#include <string_view>
#if SIMD_TEST_NUMERICS
import simd.numerics;
#else
import simd.types;
#endif

struct string {};
static_assert(sizeof(string) > 0);

static_assert(simd::one_of_t<int, float, int>);
static_assert(!simd::one_of_t<int>);
static_assert(simd::not_one_of_t<int>);
static_assert(!simd::one_of_t<int const, int>);
static_assert(simd::one_of_t<int const, int const>);
static_assert(!simd::one_of_t<int &, int>);
static_assert(simd::one_of_t<int &, int &, float>);
static_assert(simd::one_of_t<int[3], int[3]>);
static_assert(!simd::one_of_t<int[3], int[4]>);
static_assert(!noexcept(simd::type_of(1)));

template<class T> requires simd::one_of_t<T, int, double>
constexpr int admitted(T) { return 1; }
static_assert(admitted(1) == 1 && admitted(1.0) == 1);

#if SIMD_TEST_NUMERICS
static_assert(simd::one_of<2, 1, 2>);
static_assert(simd::cmpint<simd::CMPINT::LT>(-1, 0));
static_assert(simd::fp16::from_bits(0x3c00).to_bits() == 0x3c00);
#endif

int main() {
  if (simd::type<int>.empty() || simd::type<double>.empty()) return 1;
  if (simd::type<int> == simd::type<double>) return 2;
  if (simd::type<fixture::sample>.empty() || simd::type<int *>.empty()) return 3;
  fixture::sample const value{};
  if (simd::type_of(value) != simd::type<fixture::sample>) return 4;
  if (simd::type<fixture::sample>.data() != fixture::name_storage()) return 5;
  if (simd::type_of(1) != simd::type<int>) return 6;
  if (!fixture::peer_checks()) return 7;
  return 0;
}
