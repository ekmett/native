// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <cstdint>
#include <cstdio>
#include <type_traits>
import native;
constexpr auto a = native::feature_closure(native::wasm_feature::simd128);
template <class T> using V = native::simd<T, 16 / sizeof(T), a>;

template <class T>
__attribute__((target("simd128"))) constexpr bool arithmetic(T three = T(3), T two = T(2)) {
  V<T> x(three), y(two);
  auto sum = x + y;
  auto difference = x - y;
  std::array<T, V<T>::lanes> values{};
  sum.store(values.data());
  for (auto v : values)
    if (v != T(5))
      return false;
  if (!all(sum > difference) || any(sum == difference))
    return false;
  auto chosen = select(x > y, sum, difference);
  chosen.store(values.data());
  for (auto v : values)
    if (v != T(5))
      return false;
  if constexpr (sizeof(T) > 1) {
    auto p = x * y;
    p.store(values.data());
    for (auto v : values)
      if (v != T(6))
        return false;
  }
  if constexpr (std::is_floating_point_v<T>) {
    auto q = x / y;
    q.store(values.data());
    for (auto v : values)
      if (v != T(1.5))
        return false;
  } else {
    auto shifted = (x << unsigned(sizeof(T) * 8 + 1));
    shifted.store(values.data());
    for (auto v : values)
      if (v != T(6))
        return false;
  }
  return true;
}

static_assert(arithmetic<std::int8_t>() && arithmetic<std::uint8_t>());
static_assert(arithmetic<std::int16_t>() && arithmetic<std::uint16_t>());
static_assert(arithmetic<std::int32_t>() && arithmetic<std::uint32_t>());
static_assert(arithmetic<std::int64_t>() && arithmetic<std::uint64_t>());
static_assert(arithmetic<float>() && arithmetic<double>());
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
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
    auto check = [&](auto result, auto scalar) __attribute__((target("simd128"))) {
      for (unsigned i = 0; i < n; ++i)
        r[i] = scalar(x[i], y[i]);
      return equals(result, r);
    };
    if (!check(p + q, [](T a, T b) { return std::bit_cast<T>(U(std::uint64_t(U(a)) + U(b))); }) ||
        !check(p - q, [](T a, T b) { return std::bit_cast<T>(U(std::uint64_t(U(a)) - U(b))); }) ||
        !check(p & q, [](T a, T b) { return std::bit_cast<T>(U(U(a) & U(b))); }) ||
        !check(p | q, [](T a, T b) { return std::bit_cast<T>(U(U(a) | U(b))); }) ||
        !check(p ^ q, [](T a, T b) { return std::bit_cast<T>(U(U(a) ^ U(b))); }) ||
        !check(~p, [](T a, T) { return std::bit_cast<T>(U(~U(a))); }) ||
        !check(-p, [](T a, T) { return std::bit_cast<T>(U(0 - U(a))); }))
      return false;
    if constexpr (sizeof(T) > 1)
      if (!check(p * q, [](T a, T b) { return std::bit_cast<T>(U(std::uint64_t(U(a)) * U(b))); }))
        return false;
    unsigned count = unsigned(random_word());
    unsigned shift = count % (8 * sizeof(T));
    if (!check(p << count,
               [=](T a, T) { return std::bit_cast<T>(U(std::uint64_t(U(a)) << shift)); }) ||
        !check(p >> count, [=](T a, T) { return T(a >> shift); }))
      return false;
    auto compare = [&](auto mask, auto scalar) __attribute__((target("simd128"))) {
      std::uint64_t expected = 0;
      for (unsigned i = 0; i < n; ++i)
        expected |= std::uint64_t(scalar(x[i], y[i])) << i;
      return mask.to_bitset() == expected;
    };
    if (!compare(p == q, [](T a, T b) { return a == b; }) ||
        !compare(p != q, [](T a, T b) { return a != b; }) ||
        !compare(p < q, [](T a, T b) { return a < b; }) ||
        !compare(p <= q, [](T a, T b) { return a <= b; }) ||
        !compare(p > q, [](T a, T b) { return a > b; }) ||
        !compare(p >= q, [](T a, T b) { return a >= b; }))
      return false;
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
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

__attribute__((target("simd128"))) bool runtime_mixed() {
  for (unsigned trial = 0; trial < 128; ++trial) {
    std::array<std::int16_t, 8> a{}, b{}, product{};
    std::array<std::int32_t, 4> dot{};
    for (unsigned i = 0; i < 8; ++i) {
      a[i] = std::int16_t(random_word());
      b[i] = std::int16_t(random_word());
      product[i] = limit<std::int16_t>((std::int64_t(a[i]) * b[i] + 16384) >> 15);
    }
    // Include the one saturating multiplication case and the wrapping dot case.
    a[0] = a[1] = b[0] = b[1] = INT16_MIN;
    product[0] = product[1] = INT16_MAX;
    for (unsigned i = 0; i < 4; ++i) {
      auto sum = std::int64_t(a[2 * i]) * b[2 * i] + std::int64_t(a[2 * i + 1]) * b[2 * i + 1];
      dot[i] = std::bit_cast<std::int32_t>(std::uint32_t(sum));
    }
    if (!equals(native::q15mulr_sat(V<std::int16_t>(a), V<std::int16_t>(b)), product) ||
        !equals(native::dot(V<std::int16_t>(a), V<std::int16_t>(b)), dot))
      return false;
    std::array<std::uint8_t, 16> bytes{}, indices{}, expected{};
    for (unsigned i = 0; i < 16; ++i) {
      bytes[i] = std::uint8_t(random_word());
      indices[i] = std::uint8_t(random_word());
    }
    indices[0] = 0;
    indices[1] = 15;
    indices[2] = 16;
    indices[3] = 255;
    for (unsigned i = 0; i < 16; ++i)
      expected[i] = indices[i] < 16 ? bytes[indices[i]] : 0;
    if (!equals(native::swizzle(V<std::uint8_t>(bytes), V<std::uint8_t>(indices)), expected))
      return false;
    std::array<std::uint32_t, 4> words{};
    for (auto &word : words)
      word = std::uint32_t(random_word());
    V<std::uint32_t> p(words);
    if (!equals(native::shuffle<3, 5, 1, 7>(p, p),
                std::array{words[3], words[1], words[1], words[3]}))
      return false;
    auto scalar = std::uint32_t(random_word());
    if (native::load_lane<3>(&scalar, p).get<3>() != scalar)
      return false;
    native::store_lane<2>(&scalar, p);
    if (scalar != words[2] ||
        !equals(native::load_zero<V<std::uint32_t>>(&scalar), std::array{scalar, 0u, 0u, 0u}) ||
        !all(native::load_splat<V<std::uint32_t>>(&scalar) == V<std::uint32_t>(scalar)))
      return false;
    std::array<std::uint32_t, 6> out{9u, 9u, 9u, 9u, 9u, 9u};
    auto count = trial % 5;
    native::store_simd_partial(out.data() + 1, p, count);
    for (unsigned i = 0; i < 6; ++i)
      if (out[i] != (i > 0 && i <= count ? words[i - 1] : 9u))
        return false;
    auto partial = native::load_simd_partial<V<std::uint32_t>>(out.data() + 1, count, 8u);
    for (unsigned i = count; i < 4; ++i)
      words[i] = 8;
    if (!equals(partial, words))
      return false;
  }
  native::store_simd_partial(static_cast<std::uint32_t *>(nullptr), V<std::uint32_t>(1u), 0);
  return all(native::load_simd_partial<V<std::uint32_t>>(
                 static_cast<std::uint32_t const *>(nullptr), 0, 7u) == V<std::uint32_t>(7u));
}

template <class To, class From> To scalar_saturation(From value) {
  if (std::isnan(value))
    return 0;
  auto truncated = std::trunc(double(value));
  if (truncated <= double(std::numeric_limits<To>::min()))
    return std::numeric_limits<To>::min();
  if (truncated >= double(std::numeric_limits<To>::max()))
    return std::numeric_limits<To>::max();
  return To(truncated);
}

__attribute__((target("simd128"))) bool runtime_conversions() {
  for (unsigned trial = 0; trial < 128; ++trial) {
    std::array<float, 4> f{}, fexpected{};
    std::array<double, 2> d{}, dexpected{};
    std::array<std::int32_t, 4> si{}, sexpected{};
    std::array<std::uint32_t, 4> ui{}, uexpected{};
    for (unsigned i = 0; i < 4; ++i) {
      f[i] = std::bit_cast<float>(std::uint32_t(random_word()));
      si[i] = std::int32_t(random_word());
      ui[i] = std::uint32_t(random_word());
      sexpected[i] = scalar_saturation<std::int32_t>(f[i]);
      uexpected[i] = scalar_saturation<std::uint32_t>(f[i]);
    }
    if (!equals(native::trunc_sat<std::int32_t>(V<float>(f)), sexpected) ||
        !equals(native::trunc_sat<std::uint32_t>(V<float>(f)), uexpected))
      return false;
    for (unsigned i = 0; i < 2; ++i) {
      d[i] = std::bit_cast<double>(random_word());
      sexpected[i] = scalar_saturation<std::int32_t>(d[i]);
      uexpected[i] = scalar_saturation<std::uint32_t>(d[i]);
    }
    sexpected[2] = sexpected[3] = 0;
    uexpected[2] = uexpected[3] = 0;
    if (!equals(native::trunc_sat<std::int32_t>(V<double>(d)), sexpected) ||
        !equals(native::trunc_sat<std::uint32_t>(V<double>(d)), uexpected))
      return false;
    for (unsigned i = 0; i < 4; ++i)
      fexpected[i] = float(si[i]);
    if (!equals(native::convert<float>(V<std::int32_t>(si)), fexpected))
      return false;
    for (unsigned i = 0; i < 4; ++i)
      fexpected[i] = float(ui[i]);
    if (!equals(native::convert<float>(V<std::uint32_t>(ui)), fexpected))
      return false;
    for (unsigned i = 0; i < 2; ++i)
      dexpected[i] = double(si[i]);
    if (!equals(native::convert<double>(V<std::int32_t>(si)), dexpected))
      return false;
    for (unsigned i = 0; i < 2; ++i)
      dexpected[i] = double(ui[i]);
    if (!equals(native::convert<double>(V<std::uint32_t>(ui)), dexpected))
      return false;
    auto promoted = native::convert<double>(V<float>(f));
    auto demoted = native::convert<float>(V<double>(d));
    promoted.store(dexpected.data());
    demoted.store(fexpected.data());
    for (unsigned i = 0; i < 2; ++i)
      if (!fp_equal(dexpected[i], double(f[i])) || !fp_equal(fexpected[i], float(d[i])))
        return false;
    if (!fp_equal(fexpected[2], 0.f) || !fp_equal(fexpected[3], 0.f))
      return false;
  }
  return true;
}
volatile int input_three = 3;
volatile int input_two = 2;

__attribute__((target("simd128"), noinline)) bool runtime() {
  return arithmetic<std::int8_t>(std::int8_t(input_three), std::int8_t(input_two)) &&
         arithmetic<std::uint8_t>(std::uint8_t(input_three), std::uint8_t(input_two)) &&
         arithmetic<std::int16_t>(std::int16_t(input_three), std::int16_t(input_two)) &&
         arithmetic<std::uint16_t>(std::uint16_t(input_three), std::uint16_t(input_two)) &&
         arithmetic<std::int32_t>(std::int32_t(input_three), std::int32_t(input_two)) &&
         arithmetic<std::uint32_t>(std::uint32_t(input_three), std::uint32_t(input_two)) &&
         arithmetic<std::int64_t>(std::int64_t(input_three), std::int64_t(input_two)) &&
         arithmetic<std::uint64_t>(std::uint64_t(input_three), std::uint64_t(input_two)) &&
         arithmetic<float>(float(input_three), float(input_two)) &&
         arithmetic<double>(double(input_three), double(input_two));
}

__attribute__((target("simd128"), noinline)) bool families() {
  random_state ^= std::uint64_t(input_three);
  return runtime_mixed() && runtime_conversions() && integers<std::int8_t>() &&
         integers<std::uint8_t>() && integers<std::int16_t>() && integers<std::uint16_t>() &&
         integers<std::int32_t>() && integers<std::uint32_t>() && integers<std::int64_t>() &&
         integers<std::uint64_t>() && mixed_families() && conversions() &&
         floating_families<float>() && floating_families<double>();
}

int main() {
  if (!runtime())
    return 1;
  if (!families())
    return 2;
  std::puts("Wasm SIMD128 arithmetic and constexpr contracts passed");
}
