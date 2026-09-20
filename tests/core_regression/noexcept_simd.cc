// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>
#include "support/profile.h"
import native.wide;

namespace {
  struct converted {
    operator float() const noexcept { return 2.0f; }
  };
  struct may_throw {
    operator float() const noexcept(false);
  };
  // Parameters are named lvalues inside the SIMD constructor, even when
  // construction receives prvalues. Check the operation actually performed.
  struct lvalue_may_throw {
    operator float() & noexcept(false);
    operator float() && noexcept { return 3.0f; }
  };
  template<class X, std::size_t... I>
  constexpr bool pack_nothrow(std::index_sequence<I...>) {
    return noexcept(test_vec<float, sizeof...(I)>(((void)I, X{})...));
  }
  template<std::size_t N>
  bool check() {
    using indices = std::make_index_sequence<N>;
    static_assert(pack_nothrow<float>(indices{}));
    static_assert(pack_nothrow<converted>(indices{}));
    static_assert(!pack_nothrow<may_throw>(indices{}));
    static_assert(!pack_nothrow<lvalue_may_throw>(indices{}));
    auto values = []<std::size_t... I>(std::index_sequence<I...>) {
      return test_vec<float, N>(((void)I, converted{})...);
    }(indices{});
    std::array<float, N> output;
    values.storeu(output.data());
    for (float x : output) if (x != 2.0f) return false;
    return true;
  }
  int conversions = 0;
  int cleanups = 0;
  struct failure {};
  struct guard { ~guard() { ++cleanups; } };

  template<class V, bool Throws>
  struct vector_conversion {
    operator V() & noexcept(!Throws) {
      ++conversions;
      guard cleanup;
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
      if constexpr (Throws) throw failure{};
#endif
      return V(2.f);
    }
    // Opposite promises expose specifications which inspect an rvalue even
    // though the adapter actually converts its named lvalue parameter.
    operator V() && noexcept(Throws) { return V(3.f); }
  };

  template<class V, class F>
  bool check_conversion(F f, int count = 1) {
    using good = vector_conversion<V, false>;
    using bad = vector_conversion<V, true>;
    static_assert(noexcept(f(good{})));
    static_assert(!noexcept(f(bad{})));
    conversions = cleanups = 0;
    // Ignoring the numerical result must not discard user conversion effects.
    (void)f(good{});
    if (conversions != count || cleanups != count) return false;
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
    conversions = cleanups = 0;
    bool caught = false;
    try {
      guard caller_cleanup;
      (void)f(bad{});
    } catch (failure const &) {
      caught = true;
    }
    if (!caught || conversions != 1 || cleanups != 2) return false;
#endif
    return true;
  }

  template<std::size_t N>
  bool check_mixed() {
    using V = test_vec<float, N>;
    V value(1.f);
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(value + x)) { return value + x; })) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(x + value)) { return x + value; })) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(value - x)) { return value - x; })) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(x - value)) { return x - value; })) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(value * x)) { return value * x; })) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(x * value)) { return x * value; })) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(value / x)) { return value / x; })) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(x / value)) { return x / value; })) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(value < x)) { return value < x; })) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(x < value)) { return x < value; })) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(value > x)) { return value > x; })) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(x > value)) { return x > value; })) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(value <= x)) { return value <= x; })) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(x <= value)) { return x <= value; })) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(value >= x)) { return value >= x; })) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(x >= value)) { return x >= value; })) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(value == x)) { return value == x; })) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(x == value)) { return x == value; })) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(value != x)) { return value != x; })) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(x != value)) { return x != value; })) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(fma(value,x,value))) { return fma(value,x,value); }, 1)) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(fma(x,value,value))) { return fma(x,value,value); }, 1)) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(fma(value,value,x))) { return fma(value,value,x); }, 1)) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(fma(value,x,x))) { return fma(value,x,x); }, 2)) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(fma(x,value,x))) { return fma(x,value,x); }, 2)) return false;
    if (!check_conversion<V>([&](auto x) noexcept(noexcept(fma(x,x,value))) { return fma(x,x,value); }, 2)) return false;
    return true;
  }
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
  struct throwing_float {
    operator float() const {
      ++conversions;
      throw failure{};
    }
  };
  template<std::size_t N>
  bool check_unwind() {
    conversions = cleanups = 0;
    try {
      guard g;
      auto values = []<std::size_t... I>(std::index_sequence<I...>) {
        return test_vec<float, N>(((void)I, throwing_float{})...);
      }(std::make_index_sequence<N>{});
      std::array<float, N> output;
      values.storeu(output.data());
    } catch (failure const &) {
      return conversions == 1 && cleanups == 1;
    }
    return false;
  }
#endif
}
int main() {
  if (!check_mixed<1>()) return 7;
#if defined(__AVX2__) || defined(__ARM_NEON)
  if (!check<4>() || !check_mixed<4>()) return 1;
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
  if (!check_unwind<4>()) return 2;
#endif
#endif
#if defined(__AVX2__)
  if (!check<8>() || !check_mixed<8>()) return 3;
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
  if (!check_unwind<8>()) return 4;
#endif
#endif
#if defined(__AVX512F__)
  if (!check<16>() || !check_mixed<16>()) return 5;
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
  if (!check_unwind<16>()) return 6;
#endif
#endif
}
