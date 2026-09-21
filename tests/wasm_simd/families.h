// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <algorithm>
#include <bit>
#include <cmath>
#include <limits>

inline std::uint64_t random_state = 0x65396746840bc671ull;
inline std::uint64_t random_word() {
  random_state ^= random_state << 13;
  random_state ^= random_state >> 7;
  random_state ^= random_state << 17;
  return random_state;
}
template <class Vector, class T, std::size_t N>
__attribute__((target("simd128"))) constexpr bool equals(Vector value,
                                                         std::array<T, N> const &expected) {
  std::array<T, N> actual{};
  value.store(actual.data());
  return actual == expected;
}
template <class T> constexpr T limit(std::int64_t x) {
  if (x < std::numeric_limits<T>::min())
    return std::numeric_limits<T>::min();
  if (x > std::numeric_limits<T>::max())
    return std::numeric_limits<T>::max();
  return T(x);
}
template <class T> __attribute__((target("simd128"))) bool integers() {
  constexpr auto n = V<T>::lanes;
  using U = std::make_unsigned_t<T>;
  for (unsigned k = 0; k < 128; ++k) {
    std::array<T, n> x{}, y{}, r{};
    for (unsigned i = 0; i < n; ++i) {
      x[i] = std::bit_cast<T>(U(random_word()));
      y[i] = std::bit_cast<T>(U(random_word()));
    }
    V<T> p(x), q(y);
    for (unsigned i = 0; i < n; ++i)
      r[i] = std::min(x[i], y[i]);
    if (!equals(native::min(p, q), r))
      return false;
    for (unsigned i = 0; i < n; ++i)
      r[i] = std::max(x[i], y[i]);
    if (!equals(native::max(p, q), r))
      return false;
    std::uint32_t bits = 0;
    for (unsigned i = 0; i < n; ++i)
      bits |= std::uint32_t(U(x[i]) >> (sizeof(T) * 8 - 1)) << i;
    if (native::bitmask(p) != bits)
      return false;
    bool any = false, all = true;
    for (auto v : x) {
      any |= v != 0;
      all &= v != 0;
    }
    if (native::any(p) != any || native::all(p) != all)
      return false;
    if constexpr (std::is_signed_v<T>) {
      for (unsigned i = 0; i < n; ++i)
        r[i] = x[i] < 0 ? std::bit_cast<T>(U(0 - U(x[i]))) : x[i];
      if (!equals(native::abs(p), r))
        return false;
    }
    if constexpr (sizeof(T) <= 2) {
      for (unsigned i = 0; i < n; ++i)
        r[i] = limit<T>(std::int64_t(x[i]) + y[i]);
      if (!equals(native::add_sat(p, q), r))
        return false;
      for (unsigned i = 0; i < n; ++i)
        r[i] = limit<T>(std::int64_t(x[i]) - y[i]);
      if (!equals(native::sub_sat(p, q), r))
        return false;
      if constexpr (std::is_unsigned_v<T>) {
        for (unsigned i = 0; i < n; ++i)
          r[i] = T((unsigned(x[i]) + y[i] + 1) / 2);
        if (!equals(native::average_round(p, q), r))
          return false;
      }
    }
    if constexpr (sizeof(T) <= 4) {
      using W = typename decltype(native::extend_low(p))::value_type;
      std::array<W, n / 2> lo{}, hi{}, mullo{}, mulhi{};
      for (unsigned i = 0; i < n / 2; ++i) {
        lo[i] = W(x[i]);
        hi[i] = W(x[i + n / 2]);
        mullo[i] = W(x[i]) * W(y[i]);
        mulhi[i] = W(x[i + n / 2]) * W(y[i + n / 2]);
      }
      if (!equals(native::extend_low(p), lo) || !equals(native::extend_high(p), hi) ||
          !equals(native::multiply_widened_low(p, q), mullo) ||
          !equals(native::multiply_widened_high(p, q), mulhi))
        return false;
      if (!equals(native::load_widened<W, T, a>(x.data()), lo))
        return false;
      if constexpr (sizeof(T) <= 2 || std::is_unsigned_v<T>) {
        for (unsigned i = 0; i < n / 2; ++i)
          lo[i] = W(x[2 * i]) + W(x[2 * i + 1]);
        if (!equals(native::pairwise_add_widened(p), lo))
          return false;
      }
    }
    if constexpr (std::is_unsigned_v<T>) {
      for (unsigned i = 0; i < n; ++i)
        r[i] = T(std::popcount(x[i]));
      if (!equals(native::popcount(p), r))
        return false;
      if constexpr (sizeof(T) <= 4) {
        std::uint64_t sum = 0;
        for (auto v : x)
          sum += v;
        if (native::reduce_add_widened(p) != sum)
          return false;
      }
      if constexpr (sizeof(T) > 1) {
        using Small =
            std::conditional_t<sizeof(T) == 2, std::uint8_t,
                               std::conditional_t<sizeof(T) == 4, std::uint16_t, std::uint32_t>>;
        std::array<Small, 2 * n> expected{};
        for (unsigned i = 0; i < n; ++i) {
          expected[i] = Small(x[i]);
          expected[i + n] = Small(y[i]);
        }
        if (!equals(native::narrow_concat<Small>(p, q), expected))
          return false;
      }
    }
    if constexpr (std::is_signed_v<T> && (sizeof(T) == 2 || sizeof(T) == 4)) {
      using S = std::conditional_t<sizeof(T) == 2, std::int8_t, std::int16_t>;
      using US = std::make_unsigned_t<S>;
      std::array<S, 2 * n> sr{};
      std::array<US, 2 * n> ur{};
      for (unsigned i = 0; i < n; ++i) {
        sr[i] = limit<S>(x[i]);
        sr[i + n] = limit<S>(y[i]);
        ur[i] = limit<US>(x[i]);
        ur[i + n] = limit<US>(y[i]);
      }
      if (!equals(native::narrow_sat<S>(p, q), sr) || !equals(native::narrow_sat<US>(p, q), ur))
        return false;
    }
  }
  return true;
}
__attribute__((target("simd128"))) constexpr bool mixed_families() {
  V<std::int16_t> x(std::int16_t(-32768)), y(std::int16_t(-32768));
  if (!all(native::q15mulr_sat(x, y) == V<std::int16_t>(std::int16_t(32767))))
    return false;
  if (!all(native::dot(x, y) == V<std::int32_t>(INT32_MIN)))
    return false;
  V<std::uint32_t> p{0u, 1u, 2u, 3u};
  if (!equals(native::shuffle<3, 5, 1, 7>(p, p), std::array{3u, 1u, 1u, 3u}))
    return false;
  V<std::uint8_t> indices{std::uint8_t(0), std::uint8_t(15), std::uint8_t(16), std::uint8_t(255),
                          std::uint8_t(1), std::uint8_t(2),  std::uint8_t(3),  std::uint8_t(4),
                          std::uint8_t(5), std::uint8_t(6),  std::uint8_t(7),  std::uint8_t(8),
                          std::uint8_t(9), std::uint8_t(10), std::uint8_t(11), std::uint8_t(12)};
  std::array<std::uint8_t, 16> b{};
  for (unsigned i = 0; i < 16; ++i)
    b[i] = i;
  auto expected = b;
  indices.store(expected.data());
  for (auto &v : expected)
    if (v >= 16)
      v = 0;
  if (!equals(native::swizzle(V<std::uint8_t>(b), indices), expected))
    return false;
  std::uint32_t scalar = 77;
  if (native::load_lane<3>(&scalar, p).template get<3>() != 77)
    return false;
  native::store_lane<2>(&scalar, p);
  if (scalar != 2)
    return false;
  if (!equals(native::load_zero<V<std::uint32_t>>(&scalar), std::array{2u, 0u, 0u, 0u}))
    return false;
  if (!all(native::load_splat<V<std::uint32_t>>(&scalar) == V<std::uint32_t>(2u)))
    return false;
  std::array<std::uint32_t, 6> out{9u, 9u, 9u, 9u, 9u, 9u};
  native::store_simd_partial(out.data() + 1, p, 2);
  if (out != std::array{9u, 0u, 1u, 9u, 9u, 9u})
    return false;
  auto partial = native::load_simd_partial<V<std::uint32_t>>(out.data() + 1, 2, 8u);
  if (!equals(partial, std::array{0u, 1u, 8u, 8u}))
    return false;
  return true;
}
static_assert(mixed_families());
__attribute__((target("simd128"))) constexpr bool conversions() {
  using F = V<float>;
  using D = V<double>;
  using I = V<std::int32_t>;
  using U = V<std::uint32_t>;
  auto nan = std::bit_cast<float>(std::uint32_t(0x7fc00000));
  F x{nan, -1.9f, 2147483648.f, 4294967296.f};
  if (!equals(native::trunc_sat<std::int32_t>(x), std::array{0, -1, INT32_MAX, INT32_MAX}))
    return false;
  if (!equals(native::trunc_sat<std::uint32_t>(x), std::array{0u, 0u, 2147483648u, UINT32_MAX}))
    return false;
  D d{-2147483649.0, 4294967296.0};
  if (!equals(native::trunc_sat<std::int32_t>(d), std::array{INT32_MIN, INT32_MAX, 0, 0}))
    return false;
  if (!equals(native::trunc_sat<std::uint32_t>(d), std::array{0u, UINT32_MAX, 0u, 0u}))
    return false;
  I i{-3, 4, INT32_MIN, INT32_MAX};
  U u{0u, UINT32_MAX, 3u, 4u};
  if (!equals(native::convert<float>(i), std::array{-3.f, 4.f, float(INT32_MIN), float(INT32_MAX)}))
    return false;
  if (!equals(native::convert<double>(i), std::array{-3., 4.}))
    return false;
  if (!equals(native::convert<float>(u), std::array{0.f, float(UINT32_MAX), 3.f, 4.f}))
    return false;
  if (!equals(native::convert<double>(u), std::array{0., double(UINT32_MAX)}))
    return false;
  if (!equals(native::convert<double>(F{-3.f, 4.f, 5.f, 6.f}), std::array{-3., 4.}))
    return false;
  if (!equals(native::convert<float>(D{-3., 4.}), std::array{-3.f, 4.f, 0.f, 0.f}))
    return false;
  return true;
}
static_assert(conversions());

template <class T> bool fp_equal(T a, T b) {
  using U = std::conditional_t<sizeof(T) == 4, std::uint32_t, std::uint64_t>;
  return (a != a && b != b) || std::bit_cast<U>(a) == std::bit_cast<U>(b);
}
template <class T> __attribute__((target("simd128"))) bool floating_families() {
  constexpr auto n = V<T>::lanes;
  using U = std::conditional_t<sizeof(T) == 4, std::uint32_t, std::uint64_t>;
  auto sign = U{1} << (sizeof(T) * 8 - 1);
  T values[]{T(0),
             std::bit_cast<T>(sign),
             T(1.5),
             T(-2.5),
             T(4),
             T(0.25),
             std::numeric_limits<T>::infinity(),
             -std::numeric_limits<T>::infinity(),
             std::numeric_limits<T>::quiet_NaN()};
  for (unsigned k = 0; k < std::size(values); ++k) {
    std::array<T, n> x{}, y{}, result{};
    for (unsigned i = 0; i < n; ++i) {
      x[i] = values[(k + i) % std::size(values)];
      y[i] = values[(k + i + 1) % std::size(values)];
    }
    V<T> p(x), q(y);
    native::abs(p).store(result.data());
    for (unsigned i = 0; i < n; ++i)
      if (!fp_equal(result[i], std::bit_cast<T>(U(std::bit_cast<U>(x[i]) & ~sign))))
        return false;
    native::min(p, q).store(result.data());
    for (unsigned i = 0; i < n; ++i) {
      auto expected = (x[i] != x[i] || y[i] != y[i]) ? std::numeric_limits<T>::quiet_NaN()
                      : (x[i] == 0 && y[i] == 0)
                          ? std::bit_cast<T>(U(std::bit_cast<U>(x[i]) | std::bit_cast<U>(y[i])))
                          : (x[i] < y[i] ? x[i] : y[i]);
      if (!fp_equal(result[i], expected))
        return false;
    }
    native::max(p, q).store(result.data());
    for (unsigned i = 0; i < n; ++i) {
      auto expected = (x[i] != x[i] || y[i] != y[i]) ? std::numeric_limits<T>::quiet_NaN()
                      : (x[i] == 0 && y[i] == 0)
                          ? std::bit_cast<T>(U(std::bit_cast<U>(x[i]) & std::bit_cast<U>(y[i])))
                          : (x[i] > y[i] ? x[i] : y[i]);
      if (!fp_equal(result[i], expected))
        return false;
    }
    native::pmin(p, q).store(result.data());
    for (unsigned i = 0; i < n; ++i)
      if (!fp_equal(result[i], y[i] < x[i] ? y[i] : x[i]))
        return false;
    native::pmax(p, q).store(result.data());
    for (unsigned i = 0; i < n; ++i)
      if (!fp_equal(result[i], y[i] > x[i] ? y[i] : x[i]))
        return false;
    native::sqrt(p).store(result.data());
    for (unsigned i = 0; i < n; ++i)
      if (!fp_equal(result[i], std::sqrt(x[i])))
        return false;
    native::floor(p).store(result.data());
    for (unsigned i = 0; i < n; ++i)
      if (!fp_equal(result[i], std::floor(x[i])))
        return false;
    native::ceil(p).store(result.data());
    for (unsigned i = 0; i < n; ++i)
      if (!fp_equal(result[i], std::ceil(x[i])))
        return false;
    native::trunc(p).store(result.data());
    for (unsigned i = 0; i < n; ++i)
      if (!fp_equal(result[i], std::trunc(x[i])))
        return false;
    native::round_even(p).store(result.data());
    for (unsigned i = 0; i < n; ++i)
      if (!fp_equal(result[i], std::nearbyint(x[i])))
        return false;
  }
  return true;
}
