// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <limits>
#include "support/fp_environment.h"
import native.numerics;

template<class T> using word_t = native::uint_t<T>;
template<class T> consteval bool fixed_boundaries() {
  using U = word_t<T>;
  constexpr int p = std::numeric_limits<T>::digits - 1;
  constexpr int bias = std::numeric_limits<T>::max_exponent - 1;
  constexpr U hidden = U(1) << p;
  constexpr U sign = U(1) << (sizeof(T) * 8 - 1);
  constexpr U infinity = U(2 * bias + 1) << p;
  auto check = [](U x, T y, U expected) {
    return std::bit_cast<U>(native::scalef(std::bit_cast<T>(x), y)) == expected;
  };
  for (U s : {U(0), sign}) {
    if (!check(s | (U(bias) << p), T(1 - bias), s | hidden)) return false;
    if (!check(s | (U(bias) << p), T(bias + 2), s | infinity)) return false;
    if (!check(s | (U(bias) << p), T(-bias - p), s)) return false;
    if (!check(s | hidden, T(-1), s | (hidden >> 1))) return false;
    if (!check(s | (hidden + 1), T(-1), s | (hidden >> 1))) return false;
    if (!check(s | (hidden + 3), T(-1), s | ((hidden >> 1) + 2))) return false;
    if (!check(s | (2 * hidden - 1), T(-1), s | hidden)) return false;
    if (!check(s | (hidden - 1), T(1), s | (2 * hidden - 2))) return false;
    if (!check(s | U(1), T(p), s | hidden)) return false;
    if (!check(s | U(1), T(-1), s)) return false;
    if (!check(s | U(3), T(-1), s | U(2))) return false;
    if (!check(s | (infinity - 1), T(1), s | infinity)) return false;
    if (!check(s | (U(bias) << p), T(1.75), s | (U(bias + 1) << p))) return false;
    if (!check(s | (U(bias) << p), T(-1.75), s | (U(bias - 1) << p))) return false;
  }
  return true;
}
static_assert(fixed_boundaries<float>());
static_assert(fixed_boundaries<double>());

template<class T> struct sample { T x, y; word_t<T> expected; };
template<class T> consteval auto make_samples() {
  using U = word_t<T>;
  constexpr int p = std::numeric_limits<T>::digits - 1;
  constexpr int bias = std::numeric_limits<T>::max_exponent - 1;
  constexpr U h = U(1) << p;
  constexpr U infinity = U(2 * bias + 1) << p;
  constexpr U sign = U(1) << (sizeof(T) * 8 - 1);
  constexpr std::array words{U(0), U(1), U(2), U(3), h - 1, h, h + 1, h + 3,
    2 * h - 1, U(bias) << p, (U(bias) << p) + 1, infinity - 1, infinity,
    infinity | (h >> 1) | U(7)};
  constexpr std::array scales{T(-2147483648.0), T(-bias - p - 1), T(-bias - p),
    T(1 - bias - p), T(-bias), T(1 - bias), T(-p), T(-2), T(-1.75), T(-1),
    T(-0.75), T(0), T(0.75), T(1), T(1.75), T(2), T(p), T(bias),
    T(bias + 1), T(bias + p), T(2147483520.0)};
  std::array<sample<T>, words.size() * scales.size() * 2> result{};
  std::size_t i = 0;
  for (U s : {U(0), sign}) for (auto word : words) for (auto y : scales) {
    T x = std::bit_cast<T>(s | word);
    result[i++] = {x, y, std::bit_cast<U>(native::scalef(x, y))};
  }
  return result;
}

template<class T> bool runtime_agrees() {
  constexpr auto samples = make_samples<T>();
  for (auto const &sample : samples) {
    volatile T x = sample.x;
    volatile T y = sample.y;
    T actual = native::scalef(x, y);
    bool same = std::isnan(sample.x) ? std::isnan(actual) :
      std::bit_cast<word_t<T>>(actual) == sample.expected;
    if (!same) {
      std::fprintf(stderr, "scalef(%a, %a), width %zu: constexpr %llx runtime %llx\n",
        double(sample.x), double(sample.y), sizeof(T),
        static_cast<unsigned long long>(sample.expected),
        static_cast<unsigned long long>(std::bit_cast<word_t<T>>(actual)));
      return false;
    }
  }
  return true;
}
int main() {
  native::test::fp_scope nearest_gradual(native::test::fp_mode::gradual);
  if (!runtime_agrees<float>() || !runtime_agrees<double>()) return 1;
  std::puts("1176 public-module constexpr/runtime scalef cases agree in nearest-even gradual mode.");
}