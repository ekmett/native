// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <type_traits>
import native.arm.neon;
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace reference {
  enum class operation { add, sub, multiply, shift };

  template <class T> struct result {
    T value;
    bool saturated;
  };

  template <class T> constexpr result<T> clamp(__int128 value) {
    auto lo = __int128(std::numeric_limits<T>::min());
    auto hi = __int128(std::numeric_limits<T>::max());
    return {T(value < lo ? lo : value > hi ? hi : value), value < lo || value > hi};
  }

  template <class T, class S>
  constexpr result<T> binary(operation op, T a, S b, bool round, bool saturate) {
    constexpr unsigned width = sizeof(T) * 8;
    __int128 value = a;
    if (op == operation::add)
      return clamp<T>(value + __int128(b));
    if (op == operation::sub)
      return clamp<T>(value - __int128(b));
    if (op == operation::multiply) {
      auto doubled = 2 * value * __int128(b);
      if (round)
        doubled += __int128{1} << (width - 1);
      return clamp<T>(doubled >> width);
    }
    int shift = int(std::uint8_t(b));
    if (shift >= 128)
      shift -= 256;
    if (shift >= 0) {
      if (unsigned(shift) >= width) {
        if (saturate && a != 0)
          return {a < 0 ? std::numeric_limits<T>::min() : std::numeric_limits<T>::max(), true};
        return {T(0), false};
      }
      value *= __int128{1} << shift;
    } else {
      unsigned right = unsigned(-shift);
      if (right > width + 1)
        right = width + 1;
      if (round)
        value += __int128{1} << (right - 1);
      value >>= right;
    }
    if (saturate)
      return clamp<T>(value);
    using U = std::make_unsigned_t<T>;
    return {std::bit_cast<T>(U(value)), false};
  }

  template <class T> constexpr auto boundaries() {
    return std::array<T, 8>{std::numeric_limits<T>::min(),
                            std::numeric_limits<T>::max(),
                            T(0),
                            T(1),
                            T(-1),
                            T(2),
                            T(3),
                            T(std::numeric_limits<T>::max() / 2)};
  }
} // namespace reference

constexpr auto architecture = native::feature_closure(native::arm_feature::neon);
template <class T, std::size_t N> using V = native::simd<T, N, architecture>;
#pragma clang attribute push(__attribute__((target("neon"))), apply_to = function)

// Logical lane packing also covers the established padded int32x2 shape.
template <class Vector> constexpr auto lanes(Vector value) {
  std::array<typename Vector::value_type, Vector::lanes> result{};
  value.store(result.data());
  return result;
}

template <class T, class S, std::size_t N, class F>
constexpr bool constant_binary(F operation, reference::operation kind, bool round, bool saturate) {
  auto values = reference::boundaries<T>();
  for (unsigned trial = 0; trial < values.size(); ++trial) {
    std::array<T, N> a{}, expected{};
    std::array<S, N> b{};
    for (unsigned i = 0; i < N; ++i) {
      a[i] = values[(trial + i) % values.size()];
      b[i] = S(values[(trial + i + 1) % values.size()]);
      expected[i] = reference::binary(kind, a[i], b[i], round, saturate).value;
    }
    if (lanes(operation(V<T, N>::load(a.data()), V<S, N>::load(b.data()))) != expected)
      return false;
  }
  return true;
}

template <class T, class D, std::size_t N, class Low, class High>
constexpr bool constant_narrow(Low low_operation, High high_operation) {
  auto values = reference::boundaries<T>();
  for (unsigned trial = 0; trial < values.size(); ++trial) {
    std::array<T, N> a{};
    std::array<D, N> bottom{}, expected{};
    std::array<D, 2 * N> combined{};
    for (unsigned i = 0; i < N; ++i) {
      a[i] = values[(trial + i) % values.size()];
      bottom[i] = D(i + 7);
      expected[i] = reference::clamp<D>(a[i]).value;
      combined[i] = bottom[i];
      combined[i + N] = expected[i];
    }
    auto input = V<T, N>::load(a.data());
    if (lanes(low_operation(input)) != expected ||
        lanes(high_operation(V<D, N>::load(bottom.data()), input)) != combined)
      return false;
  }
  return true;
}

// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
static_assert(constant_binary<std::int8_t, std::int8_t, 8>([](V<std::int8_t, 8> a, V<std::int8_t, 8> b) { return native::sqadd(a, b); }, reference::operation::add, false, true));
static_assert(constant_binary<std::int8_t, std::int8_t, 16>([](V<std::int8_t, 16> a, V<std::int8_t, 16> b) { return native::sqadd(a, b); }, reference::operation::add, false, true));
static_assert(constant_binary<std::int16_t, std::int16_t, 4>([](V<std::int16_t, 4> a, V<std::int16_t, 4> b) { return native::sqadd(a, b); }, reference::operation::add, false, true));
static_assert(constant_binary<std::int16_t, std::int16_t, 8>([](V<std::int16_t, 8> a, V<std::int16_t, 8> b) { return native::sqadd(a, b); }, reference::operation::add, false, true));
static_assert(constant_binary<std::int32_t, std::int32_t, 2>([](V<std::int32_t, 2> a, V<std::int32_t, 2> b) { return native::sqadd(a, b); }, reference::operation::add, false, true));
static_assert(constant_binary<std::int32_t, std::int32_t, 4>([](V<std::int32_t, 4> a, V<std::int32_t, 4> b) { return native::sqadd(a, b); }, reference::operation::add, false, true));
static_assert(constant_binary<std::int64_t, std::int64_t, 1>([](V<std::int64_t, 1> a, V<std::int64_t, 1> b) { return native::sqadd(a, b); }, reference::operation::add, false, true));
static_assert(constant_binary<std::int64_t, std::int64_t, 2>([](V<std::int64_t, 2> a, V<std::int64_t, 2> b) { return native::sqadd(a, b); }, reference::operation::add, false, true));
static_assert(constant_binary<std::uint8_t, std::uint8_t, 8>([](V<std::uint8_t, 8> a, V<std::uint8_t, 8> b) { return native::uqadd(a, b); }, reference::operation::add, false, true));
static_assert(constant_binary<std::uint8_t, std::uint8_t, 16>([](V<std::uint8_t, 16> a, V<std::uint8_t, 16> b) { return native::uqadd(a, b); }, reference::operation::add, false, true));
static_assert(constant_binary<std::uint16_t, std::uint16_t, 4>([](V<std::uint16_t, 4> a, V<std::uint16_t, 4> b) { return native::uqadd(a, b); }, reference::operation::add, false, true));
static_assert(constant_binary<std::uint16_t, std::uint16_t, 8>([](V<std::uint16_t, 8> a, V<std::uint16_t, 8> b) { return native::uqadd(a, b); }, reference::operation::add, false, true));
static_assert(constant_binary<std::uint32_t, std::uint32_t, 2>([](V<std::uint32_t, 2> a, V<std::uint32_t, 2> b) { return native::uqadd(a, b); }, reference::operation::add, false, true));
static_assert(constant_binary<std::uint32_t, std::uint32_t, 4>([](V<std::uint32_t, 4> a, V<std::uint32_t, 4> b) { return native::uqadd(a, b); }, reference::operation::add, false, true));
static_assert(constant_binary<std::uint64_t, std::uint64_t, 1>([](V<std::uint64_t, 1> a, V<std::uint64_t, 1> b) { return native::uqadd(a, b); }, reference::operation::add, false, true));
static_assert(constant_binary<std::uint64_t, std::uint64_t, 2>([](V<std::uint64_t, 2> a, V<std::uint64_t, 2> b) { return native::uqadd(a, b); }, reference::operation::add, false, true));
static_assert(constant_binary<std::int8_t, std::int8_t, 8>([](V<std::int8_t, 8> a, V<std::int8_t, 8> b) { return native::sqsub(a, b); }, reference::operation::sub, false, true));
static_assert(constant_binary<std::int8_t, std::int8_t, 16>([](V<std::int8_t, 16> a, V<std::int8_t, 16> b) { return native::sqsub(a, b); }, reference::operation::sub, false, true));
static_assert(constant_binary<std::int16_t, std::int16_t, 4>([](V<std::int16_t, 4> a, V<std::int16_t, 4> b) { return native::sqsub(a, b); }, reference::operation::sub, false, true));
static_assert(constant_binary<std::int16_t, std::int16_t, 8>([](V<std::int16_t, 8> a, V<std::int16_t, 8> b) { return native::sqsub(a, b); }, reference::operation::sub, false, true));
static_assert(constant_binary<std::int32_t, std::int32_t, 2>([](V<std::int32_t, 2> a, V<std::int32_t, 2> b) { return native::sqsub(a, b); }, reference::operation::sub, false, true));
static_assert(constant_binary<std::int32_t, std::int32_t, 4>([](V<std::int32_t, 4> a, V<std::int32_t, 4> b) { return native::sqsub(a, b); }, reference::operation::sub, false, true));
static_assert(constant_binary<std::int64_t, std::int64_t, 1>([](V<std::int64_t, 1> a, V<std::int64_t, 1> b) { return native::sqsub(a, b); }, reference::operation::sub, false, true));
static_assert(constant_binary<std::int64_t, std::int64_t, 2>([](V<std::int64_t, 2> a, V<std::int64_t, 2> b) { return native::sqsub(a, b); }, reference::operation::sub, false, true));
static_assert(constant_binary<std::uint8_t, std::uint8_t, 8>([](V<std::uint8_t, 8> a, V<std::uint8_t, 8> b) { return native::uqsub(a, b); }, reference::operation::sub, false, true));
static_assert(constant_binary<std::uint8_t, std::uint8_t, 16>([](V<std::uint8_t, 16> a, V<std::uint8_t, 16> b) { return native::uqsub(a, b); }, reference::operation::sub, false, true));
static_assert(constant_binary<std::uint16_t, std::uint16_t, 4>([](V<std::uint16_t, 4> a, V<std::uint16_t, 4> b) { return native::uqsub(a, b); }, reference::operation::sub, false, true));
static_assert(constant_binary<std::uint16_t, std::uint16_t, 8>([](V<std::uint16_t, 8> a, V<std::uint16_t, 8> b) { return native::uqsub(a, b); }, reference::operation::sub, false, true));
static_assert(constant_binary<std::uint32_t, std::uint32_t, 2>([](V<std::uint32_t, 2> a, V<std::uint32_t, 2> b) { return native::uqsub(a, b); }, reference::operation::sub, false, true));
static_assert(constant_binary<std::uint32_t, std::uint32_t, 4>([](V<std::uint32_t, 4> a, V<std::uint32_t, 4> b) { return native::uqsub(a, b); }, reference::operation::sub, false, true));
static_assert(constant_binary<std::uint64_t, std::uint64_t, 1>([](V<std::uint64_t, 1> a, V<std::uint64_t, 1> b) { return native::uqsub(a, b); }, reference::operation::sub, false, true));
static_assert(constant_binary<std::uint64_t, std::uint64_t, 2>([](V<std::uint64_t, 2> a, V<std::uint64_t, 2> b) { return native::uqsub(a, b); }, reference::operation::sub, false, true));
static_assert(constant_binary<std::int16_t, std::int16_t, 4>([](V<std::int16_t, 4> a, V<std::int16_t, 4> b) { return native::sqdmulh(a, b); }, reference::operation::multiply, false, true));
static_assert(constant_binary<std::int16_t, std::int16_t, 8>([](V<std::int16_t, 8> a, V<std::int16_t, 8> b) { return native::sqdmulh(a, b); }, reference::operation::multiply, false, true));
static_assert(constant_binary<std::int32_t, std::int32_t, 2>([](V<std::int32_t, 2> a, V<std::int32_t, 2> b) { return native::sqdmulh(a, b); }, reference::operation::multiply, false, true));
static_assert(constant_binary<std::int32_t, std::int32_t, 4>([](V<std::int32_t, 4> a, V<std::int32_t, 4> b) { return native::sqdmulh(a, b); }, reference::operation::multiply, false, true));
static_assert(constant_binary<std::int16_t, std::int16_t, 4>([](V<std::int16_t, 4> a, V<std::int16_t, 4> b) { return native::sqrdmulh(a, b); }, reference::operation::multiply, true, true));
static_assert(constant_binary<std::int16_t, std::int16_t, 8>([](V<std::int16_t, 8> a, V<std::int16_t, 8> b) { return native::sqrdmulh(a, b); }, reference::operation::multiply, true, true));
static_assert(constant_binary<std::int32_t, std::int32_t, 2>([](V<std::int32_t, 2> a, V<std::int32_t, 2> b) { return native::sqrdmulh(a, b); }, reference::operation::multiply, true, true));
static_assert(constant_binary<std::int32_t, std::int32_t, 4>([](V<std::int32_t, 4> a, V<std::int32_t, 4> b) { return native::sqrdmulh(a, b); }, reference::operation::multiply, true, true));
static_assert(constant_binary<std::int8_t, std::int8_t, 8>([](V<std::int8_t, 8> a, V<std::int8_t, 8> b) { return native::sshl(a, b); }, reference::operation::shift, false, false));
static_assert(constant_binary<std::int8_t, std::int8_t, 16>([](V<std::int8_t, 16> a, V<std::int8_t, 16> b) { return native::sshl(a, b); }, reference::operation::shift, false, false));
static_assert(constant_binary<std::int16_t, std::int16_t, 4>([](V<std::int16_t, 4> a, V<std::int16_t, 4> b) { return native::sshl(a, b); }, reference::operation::shift, false, false));
static_assert(constant_binary<std::int16_t, std::int16_t, 8>([](V<std::int16_t, 8> a, V<std::int16_t, 8> b) { return native::sshl(a, b); }, reference::operation::shift, false, false));
static_assert(constant_binary<std::int32_t, std::int32_t, 2>([](V<std::int32_t, 2> a, V<std::int32_t, 2> b) { return native::sshl(a, b); }, reference::operation::shift, false, false));
static_assert(constant_binary<std::int32_t, std::int32_t, 4>([](V<std::int32_t, 4> a, V<std::int32_t, 4> b) { return native::sshl(a, b); }, reference::operation::shift, false, false));
static_assert(constant_binary<std::int64_t, std::int64_t, 1>([](V<std::int64_t, 1> a, V<std::int64_t, 1> b) { return native::sshl(a, b); }, reference::operation::shift, false, false));
static_assert(constant_binary<std::int64_t, std::int64_t, 2>([](V<std::int64_t, 2> a, V<std::int64_t, 2> b) { return native::sshl(a, b); }, reference::operation::shift, false, false));
static_assert(constant_binary<std::int8_t, std::int8_t, 8>([](V<std::int8_t, 8> a, V<std::int8_t, 8> b) { return native::srshl(a, b); }, reference::operation::shift, true, false));
static_assert(constant_binary<std::int8_t, std::int8_t, 16>([](V<std::int8_t, 16> a, V<std::int8_t, 16> b) { return native::srshl(a, b); }, reference::operation::shift, true, false));
static_assert(constant_binary<std::int16_t, std::int16_t, 4>([](V<std::int16_t, 4> a, V<std::int16_t, 4> b) { return native::srshl(a, b); }, reference::operation::shift, true, false));
static_assert(constant_binary<std::int16_t, std::int16_t, 8>([](V<std::int16_t, 8> a, V<std::int16_t, 8> b) { return native::srshl(a, b); }, reference::operation::shift, true, false));
static_assert(constant_binary<std::int32_t, std::int32_t, 2>([](V<std::int32_t, 2> a, V<std::int32_t, 2> b) { return native::srshl(a, b); }, reference::operation::shift, true, false));
static_assert(constant_binary<std::int32_t, std::int32_t, 4>([](V<std::int32_t, 4> a, V<std::int32_t, 4> b) { return native::srshl(a, b); }, reference::operation::shift, true, false));
static_assert(constant_binary<std::int64_t, std::int64_t, 1>([](V<std::int64_t, 1> a, V<std::int64_t, 1> b) { return native::srshl(a, b); }, reference::operation::shift, true, false));
static_assert(constant_binary<std::int64_t, std::int64_t, 2>([](V<std::int64_t, 2> a, V<std::int64_t, 2> b) { return native::srshl(a, b); }, reference::operation::shift, true, false));
static_assert(constant_binary<std::int8_t, std::int8_t, 8>([](V<std::int8_t, 8> a, V<std::int8_t, 8> b) { return native::sqshl(a, b); }, reference::operation::shift, false, true));
static_assert(constant_binary<std::int8_t, std::int8_t, 16>([](V<std::int8_t, 16> a, V<std::int8_t, 16> b) { return native::sqshl(a, b); }, reference::operation::shift, false, true));
static_assert(constant_binary<std::int16_t, std::int16_t, 4>([](V<std::int16_t, 4> a, V<std::int16_t, 4> b) { return native::sqshl(a, b); }, reference::operation::shift, false, true));
static_assert(constant_binary<std::int16_t, std::int16_t, 8>([](V<std::int16_t, 8> a, V<std::int16_t, 8> b) { return native::sqshl(a, b); }, reference::operation::shift, false, true));
static_assert(constant_binary<std::int32_t, std::int32_t, 2>([](V<std::int32_t, 2> a, V<std::int32_t, 2> b) { return native::sqshl(a, b); }, reference::operation::shift, false, true));
static_assert(constant_binary<std::int32_t, std::int32_t, 4>([](V<std::int32_t, 4> a, V<std::int32_t, 4> b) { return native::sqshl(a, b); }, reference::operation::shift, false, true));
static_assert(constant_binary<std::int64_t, std::int64_t, 1>([](V<std::int64_t, 1> a, V<std::int64_t, 1> b) { return native::sqshl(a, b); }, reference::operation::shift, false, true));
static_assert(constant_binary<std::int64_t, std::int64_t, 2>([](V<std::int64_t, 2> a, V<std::int64_t, 2> b) { return native::sqshl(a, b); }, reference::operation::shift, false, true));
static_assert(constant_binary<std::int8_t, std::int8_t, 8>([](V<std::int8_t, 8> a, V<std::int8_t, 8> b) { return native::sqrshl(a, b); }, reference::operation::shift, true, true));
static_assert(constant_binary<std::int8_t, std::int8_t, 16>([](V<std::int8_t, 16> a, V<std::int8_t, 16> b) { return native::sqrshl(a, b); }, reference::operation::shift, true, true));
static_assert(constant_binary<std::int16_t, std::int16_t, 4>([](V<std::int16_t, 4> a, V<std::int16_t, 4> b) { return native::sqrshl(a, b); }, reference::operation::shift, true, true));
static_assert(constant_binary<std::int16_t, std::int16_t, 8>([](V<std::int16_t, 8> a, V<std::int16_t, 8> b) { return native::sqrshl(a, b); }, reference::operation::shift, true, true));
static_assert(constant_binary<std::int32_t, std::int32_t, 2>([](V<std::int32_t, 2> a, V<std::int32_t, 2> b) { return native::sqrshl(a, b); }, reference::operation::shift, true, true));
static_assert(constant_binary<std::int32_t, std::int32_t, 4>([](V<std::int32_t, 4> a, V<std::int32_t, 4> b) { return native::sqrshl(a, b); }, reference::operation::shift, true, true));
static_assert(constant_binary<std::int64_t, std::int64_t, 1>([](V<std::int64_t, 1> a, V<std::int64_t, 1> b) { return native::sqrshl(a, b); }, reference::operation::shift, true, true));
static_assert(constant_binary<std::int64_t, std::int64_t, 2>([](V<std::int64_t, 2> a, V<std::int64_t, 2> b) { return native::sqrshl(a, b); }, reference::operation::shift, true, true));
static_assert(constant_binary<std::uint8_t, std::int8_t, 8>([](V<std::uint8_t, 8> a, V<std::int8_t, 8> b) { return native::ushl(a, b); }, reference::operation::shift, false, false));
static_assert(constant_binary<std::uint8_t, std::int8_t, 16>([](V<std::uint8_t, 16> a, V<std::int8_t, 16> b) { return native::ushl(a, b); }, reference::operation::shift, false, false));
static_assert(constant_binary<std::uint16_t, std::int16_t, 4>([](V<std::uint16_t, 4> a, V<std::int16_t, 4> b) { return native::ushl(a, b); }, reference::operation::shift, false, false));
static_assert(constant_binary<std::uint16_t, std::int16_t, 8>([](V<std::uint16_t, 8> a, V<std::int16_t, 8> b) { return native::ushl(a, b); }, reference::operation::shift, false, false));
static_assert(constant_binary<std::uint32_t, std::int32_t, 2>([](V<std::uint32_t, 2> a, V<std::int32_t, 2> b) { return native::ushl(a, b); }, reference::operation::shift, false, false));
static_assert(constant_binary<std::uint32_t, std::int32_t, 4>([](V<std::uint32_t, 4> a, V<std::int32_t, 4> b) { return native::ushl(a, b); }, reference::operation::shift, false, false));
static_assert(constant_binary<std::uint64_t, std::int64_t, 1>([](V<std::uint64_t, 1> a, V<std::int64_t, 1> b) { return native::ushl(a, b); }, reference::operation::shift, false, false));
static_assert(constant_binary<std::uint64_t, std::int64_t, 2>([](V<std::uint64_t, 2> a, V<std::int64_t, 2> b) { return native::ushl(a, b); }, reference::operation::shift, false, false));
static_assert(constant_binary<std::uint8_t, std::int8_t, 8>([](V<std::uint8_t, 8> a, V<std::int8_t, 8> b) { return native::urshl(a, b); }, reference::operation::shift, true, false));
static_assert(constant_binary<std::uint8_t, std::int8_t, 16>([](V<std::uint8_t, 16> a, V<std::int8_t, 16> b) { return native::urshl(a, b); }, reference::operation::shift, true, false));
static_assert(constant_binary<std::uint16_t, std::int16_t, 4>([](V<std::uint16_t, 4> a, V<std::int16_t, 4> b) { return native::urshl(a, b); }, reference::operation::shift, true, false));
static_assert(constant_binary<std::uint16_t, std::int16_t, 8>([](V<std::uint16_t, 8> a, V<std::int16_t, 8> b) { return native::urshl(a, b); }, reference::operation::shift, true, false));
static_assert(constant_binary<std::uint32_t, std::int32_t, 2>([](V<std::uint32_t, 2> a, V<std::int32_t, 2> b) { return native::urshl(a, b); }, reference::operation::shift, true, false));
static_assert(constant_binary<std::uint32_t, std::int32_t, 4>([](V<std::uint32_t, 4> a, V<std::int32_t, 4> b) { return native::urshl(a, b); }, reference::operation::shift, true, false));
static_assert(constant_binary<std::uint64_t, std::int64_t, 1>([](V<std::uint64_t, 1> a, V<std::int64_t, 1> b) { return native::urshl(a, b); }, reference::operation::shift, true, false));
static_assert(constant_binary<std::uint64_t, std::int64_t, 2>([](V<std::uint64_t, 2> a, V<std::int64_t, 2> b) { return native::urshl(a, b); }, reference::operation::shift, true, false));
static_assert(constant_binary<std::uint8_t, std::int8_t, 8>([](V<std::uint8_t, 8> a, V<std::int8_t, 8> b) { return native::uqshl(a, b); }, reference::operation::shift, false, true));
static_assert(constant_binary<std::uint8_t, std::int8_t, 16>([](V<std::uint8_t, 16> a, V<std::int8_t, 16> b) { return native::uqshl(a, b); }, reference::operation::shift, false, true));
static_assert(constant_binary<std::uint16_t, std::int16_t, 4>([](V<std::uint16_t, 4> a, V<std::int16_t, 4> b) { return native::uqshl(a, b); }, reference::operation::shift, false, true));
static_assert(constant_binary<std::uint16_t, std::int16_t, 8>([](V<std::uint16_t, 8> a, V<std::int16_t, 8> b) { return native::uqshl(a, b); }, reference::operation::shift, false, true));
static_assert(constant_binary<std::uint32_t, std::int32_t, 2>([](V<std::uint32_t, 2> a, V<std::int32_t, 2> b) { return native::uqshl(a, b); }, reference::operation::shift, false, true));
static_assert(constant_binary<std::uint32_t, std::int32_t, 4>([](V<std::uint32_t, 4> a, V<std::int32_t, 4> b) { return native::uqshl(a, b); }, reference::operation::shift, false, true));
static_assert(constant_binary<std::uint64_t, std::int64_t, 1>([](V<std::uint64_t, 1> a, V<std::int64_t, 1> b) { return native::uqshl(a, b); }, reference::operation::shift, false, true));
static_assert(constant_binary<std::uint64_t, std::int64_t, 2>([](V<std::uint64_t, 2> a, V<std::int64_t, 2> b) { return native::uqshl(a, b); }, reference::operation::shift, false, true));
static_assert(constant_binary<std::uint8_t, std::int8_t, 8>([](V<std::uint8_t, 8> a, V<std::int8_t, 8> b) { return native::uqrshl(a, b); }, reference::operation::shift, true, true));
static_assert(constant_binary<std::uint8_t, std::int8_t, 16>([](V<std::uint8_t, 16> a, V<std::int8_t, 16> b) { return native::uqrshl(a, b); }, reference::operation::shift, true, true));
static_assert(constant_binary<std::uint16_t, std::int16_t, 4>([](V<std::uint16_t, 4> a, V<std::int16_t, 4> b) { return native::uqrshl(a, b); }, reference::operation::shift, true, true));
static_assert(constant_binary<std::uint16_t, std::int16_t, 8>([](V<std::uint16_t, 8> a, V<std::int16_t, 8> b) { return native::uqrshl(a, b); }, reference::operation::shift, true, true));
static_assert(constant_binary<std::uint32_t, std::int32_t, 2>([](V<std::uint32_t, 2> a, V<std::int32_t, 2> b) { return native::uqrshl(a, b); }, reference::operation::shift, true, true));
static_assert(constant_binary<std::uint32_t, std::int32_t, 4>([](V<std::uint32_t, 4> a, V<std::int32_t, 4> b) { return native::uqrshl(a, b); }, reference::operation::shift, true, true));
static_assert(constant_binary<std::uint64_t, std::int64_t, 1>([](V<std::uint64_t, 1> a, V<std::int64_t, 1> b) { return native::uqrshl(a, b); }, reference::operation::shift, true, true));
static_assert(constant_binary<std::uint64_t, std::int64_t, 2>([](V<std::uint64_t, 2> a, V<std::int64_t, 2> b) { return native::uqrshl(a, b); }, reference::operation::shift, true, true));
static_assert(constant_narrow<std::int16_t, std::int8_t, 8>([](V<std::int16_t, 8> a) { return native::sqxtn(a); }, [](V<std::int8_t, 8> low, V<std::int16_t, 8> a) { return native::sqxtn_high(low, a); }));
static_assert(constant_narrow<std::int32_t, std::int16_t, 4>([](V<std::int32_t, 4> a) { return native::sqxtn(a); }, [](V<std::int16_t, 4> low, V<std::int32_t, 4> a) { return native::sqxtn_high(low, a); }));
static_assert(constant_narrow<std::int64_t, std::int32_t, 2>([](V<std::int64_t, 2> a) { return native::sqxtn(a); }, [](V<std::int32_t, 2> low, V<std::int64_t, 2> a) { return native::sqxtn_high(low, a); }));
static_assert(constant_narrow<std::uint16_t, std::uint8_t, 8>([](V<std::uint16_t, 8> a) { return native::uqxtn(a); }, [](V<std::uint8_t, 8> low, V<std::uint16_t, 8> a) { return native::uqxtn_high(low, a); }));
static_assert(constant_narrow<std::uint32_t, std::uint16_t, 4>([](V<std::uint32_t, 4> a) { return native::uqxtn(a); }, [](V<std::uint16_t, 4> low, V<std::uint32_t, 4> a) { return native::uqxtn_high(low, a); }));
static_assert(constant_narrow<std::uint64_t, std::uint32_t, 2>([](V<std::uint64_t, 2> a) { return native::uqxtn(a); }, [](V<std::uint32_t, 2> low, V<std::uint64_t, 2> a) { return native::uqxtn_high(low, a); }));
static_assert(constant_narrow<std::int16_t, std::uint8_t, 8>([](V<std::int16_t, 8> a) { return native::sqxtun(a); }, [](V<std::uint8_t, 8> low, V<std::int16_t, 8> a) { return native::sqxtun_high(low, a); }));
static_assert(constant_narrow<std::int32_t, std::uint16_t, 4>([](V<std::int32_t, 4> a) { return native::sqxtun(a); }, [](V<std::uint16_t, 4> low, V<std::int32_t, 4> a) { return native::sqxtun_high(low, a); }));
static_assert(constant_narrow<std::int64_t, std::uint32_t, 2>([](V<std::int64_t, 2> a) { return native::sqxtun(a); }, [](V<std::uint32_t, 2> low, V<std::int64_t, 2> a) { return native::sqxtun_high(low, a); }));

std::uint64_t state = 0x79842e0bb81df063ull;

std::uint64_t random_word() {
  state ^= state << 13;
  state ^= state >> 7;
  state ^= state << 17;
  return state;
}

std::uint64_t fpsr() {
  std::uint64_t value;
  asm volatile("mrs %0, FPSR" : "=r"(value));
  return value;
}

void set_fpsr(std::uint64_t value) {
  asm volatile("msr FPSR, %0" : : "r"(value));
}

constexpr std::uint64_t qc = 1ull << 27;

template <class T, class S, std::size_t N, class F>
bool check_binary(char const *name, F operation, reference::operation kind, bool round,
                  bool saturate) {
  auto values = reference::boundaries<T>();
  // Counts exercise low-byte decoding, both width boundaries and wrapping high bits.
  constexpr int counts[]{-128, -65, -64, -63, -33, -32, -31, -17, -16,  -15, -9,
                         -8,   -7,  -1,  0,   1,   7,   8,   15,  16,   31,  32,
                         63,   64,  65,  127, 128, 255, 256, 257, -256, -257};
  for (unsigned trial = 0; trial < 256; ++trial) {
    std::array<T, N> a{}, expected{};
    std::array<S, N> b{};
    bool saturated = false;
    for (unsigned i = 0; i < N; ++i) {
      a[i] = trial < 64 ? values[(trial / 8 + i) % values.size()] : T(random_word());
      b[i] = kind == reference::operation::shift ? S(counts[(trial + i) % std::size(counts)])
             : trial < 64                        ? S(values[(trial + i) % values.size()])
                                                 : S(random_word());
      auto result = reference::binary(kind, a[i], b[i], round, saturate);
      expected[i] = result.value;
      saturated |= result.saturated;
    }
    auto left = V<T, N>::load(a.data());
    auto right = V<S, N>::load(b.data());
    set_fpsr(fpsr() & ~qc);
    auto result = operation(left, right);
    auto status = fpsr();
    if (lanes(result) != expected || bool(status & qc) != saturated) {
      std::printf("%s trial %u: value or QC mismatch\n", name, trial);
      return false;
    }
    set_fpsr(status & ~qc);
    (void)operation(left, right);
    if (bool(fpsr() & qc) != saturated) {
      std::printf("%s discarded result lost QC effect\n", name);
      return false;
    }
    set_fpsr(status | qc);
    (void)operation(left, right);
    if (!(fpsr() & qc)) {
      std::printf("%s cleared sticky QC\n", name);
      return false;
    }
  }
  return true;
}

template <class T, class D, std::size_t N, class Low, class High>
bool check_narrow(char const *name, Low low_operation, High high_operation) {
  auto values = reference::boundaries<T>();
  for (unsigned trial = 0; trial < 128; ++trial) {
    std::array<T, N> a{};
    std::array<D, N> bottom{}, expected{};
    std::array<D, 2 * N> combined{};
    bool saturated = false;
    for (unsigned i = 0; i < N; ++i) {
      a[i] = trial < 8 ? values[(trial + i) % values.size()] : T(random_word());
      bottom[i] = D(random_word());
      auto result = reference::clamp<D>(a[i]);
      expected[i] = result.value;
      saturated |= result.saturated;
      combined[i] = bottom[i];
      combined[i + N] = expected[i];
    }
    auto input = V<T, N>::load(a.data());
    auto low = V<D, N>::load(bottom.data());
    set_fpsr(fpsr() & ~qc);
    auto small = low_operation(input);
    auto status = fpsr();
    if (lanes(small) != expected || bool(status & qc) != saturated) {
      std::printf("%s low trial %u mismatch\n", name, trial);
      return false;
    }
    set_fpsr(status & ~qc);
    auto full = high_operation(low, input);
    status = fpsr();
    if (lanes(full) != combined || bool(status & qc) != saturated) {
      std::printf("%s high trial %u mismatch\n", name, trial);
      return false;
    }
  }
  return true;
}

bool run() {
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
  if (!check_binary<std::int8_t, std::int8_t, 8>("sqadd_8_8", [](V<std::int8_t, 8> a, V<std::int8_t, 8> b) { return native::sqadd(a, b); }, reference::operation::add, false, true)) return false;
  if (!check_binary<std::int8_t, std::int8_t, 16>("sqadd_8_16", [](V<std::int8_t, 16> a, V<std::int8_t, 16> b) { return native::sqadd(a, b); }, reference::operation::add, false, true)) return false;
  if (!check_binary<std::int16_t, std::int16_t, 4>("sqadd_16_4", [](V<std::int16_t, 4> a, V<std::int16_t, 4> b) { return native::sqadd(a, b); }, reference::operation::add, false, true)) return false;
  if (!check_binary<std::int16_t, std::int16_t, 8>("sqadd_16_8", [](V<std::int16_t, 8> a, V<std::int16_t, 8> b) { return native::sqadd(a, b); }, reference::operation::add, false, true)) return false;
  if (!check_binary<std::int32_t, std::int32_t, 2>("sqadd_32_2", [](V<std::int32_t, 2> a, V<std::int32_t, 2> b) { return native::sqadd(a, b); }, reference::operation::add, false, true)) return false;
  if (!check_binary<std::int32_t, std::int32_t, 4>("sqadd_32_4", [](V<std::int32_t, 4> a, V<std::int32_t, 4> b) { return native::sqadd(a, b); }, reference::operation::add, false, true)) return false;
  if (!check_binary<std::int64_t, std::int64_t, 1>("sqadd_64_1", [](V<std::int64_t, 1> a, V<std::int64_t, 1> b) { return native::sqadd(a, b); }, reference::operation::add, false, true)) return false;
  if (!check_binary<std::int64_t, std::int64_t, 2>("sqadd_64_2", [](V<std::int64_t, 2> a, V<std::int64_t, 2> b) { return native::sqadd(a, b); }, reference::operation::add, false, true)) return false;
  if (!check_binary<std::uint8_t, std::uint8_t, 8>("uqadd_8_8", [](V<std::uint8_t, 8> a, V<std::uint8_t, 8> b) { return native::uqadd(a, b); }, reference::operation::add, false, true)) return false;
  if (!check_binary<std::uint8_t, std::uint8_t, 16>("uqadd_8_16", [](V<std::uint8_t, 16> a, V<std::uint8_t, 16> b) { return native::uqadd(a, b); }, reference::operation::add, false, true)) return false;
  if (!check_binary<std::uint16_t, std::uint16_t, 4>("uqadd_16_4", [](V<std::uint16_t, 4> a, V<std::uint16_t, 4> b) { return native::uqadd(a, b); }, reference::operation::add, false, true)) return false;
  if (!check_binary<std::uint16_t, std::uint16_t, 8>("uqadd_16_8", [](V<std::uint16_t, 8> a, V<std::uint16_t, 8> b) { return native::uqadd(a, b); }, reference::operation::add, false, true)) return false;
  if (!check_binary<std::uint32_t, std::uint32_t, 2>("uqadd_32_2", [](V<std::uint32_t, 2> a, V<std::uint32_t, 2> b) { return native::uqadd(a, b); }, reference::operation::add, false, true)) return false;
  if (!check_binary<std::uint32_t, std::uint32_t, 4>("uqadd_32_4", [](V<std::uint32_t, 4> a, V<std::uint32_t, 4> b) { return native::uqadd(a, b); }, reference::operation::add, false, true)) return false;
  if (!check_binary<std::uint64_t, std::uint64_t, 1>("uqadd_64_1", [](V<std::uint64_t, 1> a, V<std::uint64_t, 1> b) { return native::uqadd(a, b); }, reference::operation::add, false, true)) return false;
  if (!check_binary<std::uint64_t, std::uint64_t, 2>("uqadd_64_2", [](V<std::uint64_t, 2> a, V<std::uint64_t, 2> b) { return native::uqadd(a, b); }, reference::operation::add, false, true)) return false;
  if (!check_binary<std::int8_t, std::int8_t, 8>("sqsub_8_8", [](V<std::int8_t, 8> a, V<std::int8_t, 8> b) { return native::sqsub(a, b); }, reference::operation::sub, false, true)) return false;
  if (!check_binary<std::int8_t, std::int8_t, 16>("sqsub_8_16", [](V<std::int8_t, 16> a, V<std::int8_t, 16> b) { return native::sqsub(a, b); }, reference::operation::sub, false, true)) return false;
  if (!check_binary<std::int16_t, std::int16_t, 4>("sqsub_16_4", [](V<std::int16_t, 4> a, V<std::int16_t, 4> b) { return native::sqsub(a, b); }, reference::operation::sub, false, true)) return false;
  if (!check_binary<std::int16_t, std::int16_t, 8>("sqsub_16_8", [](V<std::int16_t, 8> a, V<std::int16_t, 8> b) { return native::sqsub(a, b); }, reference::operation::sub, false, true)) return false;
  if (!check_binary<std::int32_t, std::int32_t, 2>("sqsub_32_2", [](V<std::int32_t, 2> a, V<std::int32_t, 2> b) { return native::sqsub(a, b); }, reference::operation::sub, false, true)) return false;
  if (!check_binary<std::int32_t, std::int32_t, 4>("sqsub_32_4", [](V<std::int32_t, 4> a, V<std::int32_t, 4> b) { return native::sqsub(a, b); }, reference::operation::sub, false, true)) return false;
  if (!check_binary<std::int64_t, std::int64_t, 1>("sqsub_64_1", [](V<std::int64_t, 1> a, V<std::int64_t, 1> b) { return native::sqsub(a, b); }, reference::operation::sub, false, true)) return false;
  if (!check_binary<std::int64_t, std::int64_t, 2>("sqsub_64_2", [](V<std::int64_t, 2> a, V<std::int64_t, 2> b) { return native::sqsub(a, b); }, reference::operation::sub, false, true)) return false;
  if (!check_binary<std::uint8_t, std::uint8_t, 8>("uqsub_8_8", [](V<std::uint8_t, 8> a, V<std::uint8_t, 8> b) { return native::uqsub(a, b); }, reference::operation::sub, false, true)) return false;
  if (!check_binary<std::uint8_t, std::uint8_t, 16>("uqsub_8_16", [](V<std::uint8_t, 16> a, V<std::uint8_t, 16> b) { return native::uqsub(a, b); }, reference::operation::sub, false, true)) return false;
  if (!check_binary<std::uint16_t, std::uint16_t, 4>("uqsub_16_4", [](V<std::uint16_t, 4> a, V<std::uint16_t, 4> b) { return native::uqsub(a, b); }, reference::operation::sub, false, true)) return false;
  if (!check_binary<std::uint16_t, std::uint16_t, 8>("uqsub_16_8", [](V<std::uint16_t, 8> a, V<std::uint16_t, 8> b) { return native::uqsub(a, b); }, reference::operation::sub, false, true)) return false;
  if (!check_binary<std::uint32_t, std::uint32_t, 2>("uqsub_32_2", [](V<std::uint32_t, 2> a, V<std::uint32_t, 2> b) { return native::uqsub(a, b); }, reference::operation::sub, false, true)) return false;
  if (!check_binary<std::uint32_t, std::uint32_t, 4>("uqsub_32_4", [](V<std::uint32_t, 4> a, V<std::uint32_t, 4> b) { return native::uqsub(a, b); }, reference::operation::sub, false, true)) return false;
  if (!check_binary<std::uint64_t, std::uint64_t, 1>("uqsub_64_1", [](V<std::uint64_t, 1> a, V<std::uint64_t, 1> b) { return native::uqsub(a, b); }, reference::operation::sub, false, true)) return false;
  if (!check_binary<std::uint64_t, std::uint64_t, 2>("uqsub_64_2", [](V<std::uint64_t, 2> a, V<std::uint64_t, 2> b) { return native::uqsub(a, b); }, reference::operation::sub, false, true)) return false;
  if (!check_binary<std::int16_t, std::int16_t, 4>("sqdmulh_16_4", [](V<std::int16_t, 4> a, V<std::int16_t, 4> b) { return native::sqdmulh(a, b); }, reference::operation::multiply, false, true)) return false;
  if (!check_binary<std::int16_t, std::int16_t, 8>("sqdmulh_16_8", [](V<std::int16_t, 8> a, V<std::int16_t, 8> b) { return native::sqdmulh(a, b); }, reference::operation::multiply, false, true)) return false;
  if (!check_binary<std::int32_t, std::int32_t, 2>("sqdmulh_32_2", [](V<std::int32_t, 2> a, V<std::int32_t, 2> b) { return native::sqdmulh(a, b); }, reference::operation::multiply, false, true)) return false;
  if (!check_binary<std::int32_t, std::int32_t, 4>("sqdmulh_32_4", [](V<std::int32_t, 4> a, V<std::int32_t, 4> b) { return native::sqdmulh(a, b); }, reference::operation::multiply, false, true)) return false;
  if (!check_binary<std::int16_t, std::int16_t, 4>("sqrdmulh_16_4", [](V<std::int16_t, 4> a, V<std::int16_t, 4> b) { return native::sqrdmulh(a, b); }, reference::operation::multiply, true, true)) return false;
  if (!check_binary<std::int16_t, std::int16_t, 8>("sqrdmulh_16_8", [](V<std::int16_t, 8> a, V<std::int16_t, 8> b) { return native::sqrdmulh(a, b); }, reference::operation::multiply, true, true)) return false;
  if (!check_binary<std::int32_t, std::int32_t, 2>("sqrdmulh_32_2", [](V<std::int32_t, 2> a, V<std::int32_t, 2> b) { return native::sqrdmulh(a, b); }, reference::operation::multiply, true, true)) return false;
  if (!check_binary<std::int32_t, std::int32_t, 4>("sqrdmulh_32_4", [](V<std::int32_t, 4> a, V<std::int32_t, 4> b) { return native::sqrdmulh(a, b); }, reference::operation::multiply, true, true)) return false;
  if (!check_binary<std::int8_t, std::int8_t, 8>("sshl_8_8", [](V<std::int8_t, 8> a, V<std::int8_t, 8> b) { return native::sshl(a, b); }, reference::operation::shift, false, false)) return false;
  if (!check_binary<std::int8_t, std::int8_t, 16>("sshl_8_16", [](V<std::int8_t, 16> a, V<std::int8_t, 16> b) { return native::sshl(a, b); }, reference::operation::shift, false, false)) return false;
  if (!check_binary<std::int16_t, std::int16_t, 4>("sshl_16_4", [](V<std::int16_t, 4> a, V<std::int16_t, 4> b) { return native::sshl(a, b); }, reference::operation::shift, false, false)) return false;
  if (!check_binary<std::int16_t, std::int16_t, 8>("sshl_16_8", [](V<std::int16_t, 8> a, V<std::int16_t, 8> b) { return native::sshl(a, b); }, reference::operation::shift, false, false)) return false;
  if (!check_binary<std::int32_t, std::int32_t, 2>("sshl_32_2", [](V<std::int32_t, 2> a, V<std::int32_t, 2> b) { return native::sshl(a, b); }, reference::operation::shift, false, false)) return false;
  if (!check_binary<std::int32_t, std::int32_t, 4>("sshl_32_4", [](V<std::int32_t, 4> a, V<std::int32_t, 4> b) { return native::sshl(a, b); }, reference::operation::shift, false, false)) return false;
  if (!check_binary<std::int64_t, std::int64_t, 1>("sshl_64_1", [](V<std::int64_t, 1> a, V<std::int64_t, 1> b) { return native::sshl(a, b); }, reference::operation::shift, false, false)) return false;
  if (!check_binary<std::int64_t, std::int64_t, 2>("sshl_64_2", [](V<std::int64_t, 2> a, V<std::int64_t, 2> b) { return native::sshl(a, b); }, reference::operation::shift, false, false)) return false;
  if (!check_binary<std::int8_t, std::int8_t, 8>("srshl_8_8", [](V<std::int8_t, 8> a, V<std::int8_t, 8> b) { return native::srshl(a, b); }, reference::operation::shift, true, false)) return false;
  if (!check_binary<std::int8_t, std::int8_t, 16>("srshl_8_16", [](V<std::int8_t, 16> a, V<std::int8_t, 16> b) { return native::srshl(a, b); }, reference::operation::shift, true, false)) return false;
  if (!check_binary<std::int16_t, std::int16_t, 4>("srshl_16_4", [](V<std::int16_t, 4> a, V<std::int16_t, 4> b) { return native::srshl(a, b); }, reference::operation::shift, true, false)) return false;
  if (!check_binary<std::int16_t, std::int16_t, 8>("srshl_16_8", [](V<std::int16_t, 8> a, V<std::int16_t, 8> b) { return native::srshl(a, b); }, reference::operation::shift, true, false)) return false;
  if (!check_binary<std::int32_t, std::int32_t, 2>("srshl_32_2", [](V<std::int32_t, 2> a, V<std::int32_t, 2> b) { return native::srshl(a, b); }, reference::operation::shift, true, false)) return false;
  if (!check_binary<std::int32_t, std::int32_t, 4>("srshl_32_4", [](V<std::int32_t, 4> a, V<std::int32_t, 4> b) { return native::srshl(a, b); }, reference::operation::shift, true, false)) return false;
  if (!check_binary<std::int64_t, std::int64_t, 1>("srshl_64_1", [](V<std::int64_t, 1> a, V<std::int64_t, 1> b) { return native::srshl(a, b); }, reference::operation::shift, true, false)) return false;
  if (!check_binary<std::int64_t, std::int64_t, 2>("srshl_64_2", [](V<std::int64_t, 2> a, V<std::int64_t, 2> b) { return native::srshl(a, b); }, reference::operation::shift, true, false)) return false;
  if (!check_binary<std::int8_t, std::int8_t, 8>("sqshl_8_8", [](V<std::int8_t, 8> a, V<std::int8_t, 8> b) { return native::sqshl(a, b); }, reference::operation::shift, false, true)) return false;
  if (!check_binary<std::int8_t, std::int8_t, 16>("sqshl_8_16", [](V<std::int8_t, 16> a, V<std::int8_t, 16> b) { return native::sqshl(a, b); }, reference::operation::shift, false, true)) return false;
  if (!check_binary<std::int16_t, std::int16_t, 4>("sqshl_16_4", [](V<std::int16_t, 4> a, V<std::int16_t, 4> b) { return native::sqshl(a, b); }, reference::operation::shift, false, true)) return false;
  if (!check_binary<std::int16_t, std::int16_t, 8>("sqshl_16_8", [](V<std::int16_t, 8> a, V<std::int16_t, 8> b) { return native::sqshl(a, b); }, reference::operation::shift, false, true)) return false;
  if (!check_binary<std::int32_t, std::int32_t, 2>("sqshl_32_2", [](V<std::int32_t, 2> a, V<std::int32_t, 2> b) { return native::sqshl(a, b); }, reference::operation::shift, false, true)) return false;
  if (!check_binary<std::int32_t, std::int32_t, 4>("sqshl_32_4", [](V<std::int32_t, 4> a, V<std::int32_t, 4> b) { return native::sqshl(a, b); }, reference::operation::shift, false, true)) return false;
  if (!check_binary<std::int64_t, std::int64_t, 1>("sqshl_64_1", [](V<std::int64_t, 1> a, V<std::int64_t, 1> b) { return native::sqshl(a, b); }, reference::operation::shift, false, true)) return false;
  if (!check_binary<std::int64_t, std::int64_t, 2>("sqshl_64_2", [](V<std::int64_t, 2> a, V<std::int64_t, 2> b) { return native::sqshl(a, b); }, reference::operation::shift, false, true)) return false;
  if (!check_binary<std::int8_t, std::int8_t, 8>("sqrshl_8_8", [](V<std::int8_t, 8> a, V<std::int8_t, 8> b) { return native::sqrshl(a, b); }, reference::operation::shift, true, true)) return false;
  if (!check_binary<std::int8_t, std::int8_t, 16>("sqrshl_8_16", [](V<std::int8_t, 16> a, V<std::int8_t, 16> b) { return native::sqrshl(a, b); }, reference::operation::shift, true, true)) return false;
  if (!check_binary<std::int16_t, std::int16_t, 4>("sqrshl_16_4", [](V<std::int16_t, 4> a, V<std::int16_t, 4> b) { return native::sqrshl(a, b); }, reference::operation::shift, true, true)) return false;
  if (!check_binary<std::int16_t, std::int16_t, 8>("sqrshl_16_8", [](V<std::int16_t, 8> a, V<std::int16_t, 8> b) { return native::sqrshl(a, b); }, reference::operation::shift, true, true)) return false;
  if (!check_binary<std::int32_t, std::int32_t, 2>("sqrshl_32_2", [](V<std::int32_t, 2> a, V<std::int32_t, 2> b) { return native::sqrshl(a, b); }, reference::operation::shift, true, true)) return false;
  if (!check_binary<std::int32_t, std::int32_t, 4>("sqrshl_32_4", [](V<std::int32_t, 4> a, V<std::int32_t, 4> b) { return native::sqrshl(a, b); }, reference::operation::shift, true, true)) return false;
  if (!check_binary<std::int64_t, std::int64_t, 1>("sqrshl_64_1", [](V<std::int64_t, 1> a, V<std::int64_t, 1> b) { return native::sqrshl(a, b); }, reference::operation::shift, true, true)) return false;
  if (!check_binary<std::int64_t, std::int64_t, 2>("sqrshl_64_2", [](V<std::int64_t, 2> a, V<std::int64_t, 2> b) { return native::sqrshl(a, b); }, reference::operation::shift, true, true)) return false;
  if (!check_binary<std::uint8_t, std::int8_t, 8>("ushl_8_8", [](V<std::uint8_t, 8> a, V<std::int8_t, 8> b) { return native::ushl(a, b); }, reference::operation::shift, false, false)) return false;
  if (!check_binary<std::uint8_t, std::int8_t, 16>("ushl_8_16", [](V<std::uint8_t, 16> a, V<std::int8_t, 16> b) { return native::ushl(a, b); }, reference::operation::shift, false, false)) return false;
  if (!check_binary<std::uint16_t, std::int16_t, 4>("ushl_16_4", [](V<std::uint16_t, 4> a, V<std::int16_t, 4> b) { return native::ushl(a, b); }, reference::operation::shift, false, false)) return false;
  if (!check_binary<std::uint16_t, std::int16_t, 8>("ushl_16_8", [](V<std::uint16_t, 8> a, V<std::int16_t, 8> b) { return native::ushl(a, b); }, reference::operation::shift, false, false)) return false;
  if (!check_binary<std::uint32_t, std::int32_t, 2>("ushl_32_2", [](V<std::uint32_t, 2> a, V<std::int32_t, 2> b) { return native::ushl(a, b); }, reference::operation::shift, false, false)) return false;
  if (!check_binary<std::uint32_t, std::int32_t, 4>("ushl_32_4", [](V<std::uint32_t, 4> a, V<std::int32_t, 4> b) { return native::ushl(a, b); }, reference::operation::shift, false, false)) return false;
  if (!check_binary<std::uint64_t, std::int64_t, 1>("ushl_64_1", [](V<std::uint64_t, 1> a, V<std::int64_t, 1> b) { return native::ushl(a, b); }, reference::operation::shift, false, false)) return false;
  if (!check_binary<std::uint64_t, std::int64_t, 2>("ushl_64_2", [](V<std::uint64_t, 2> a, V<std::int64_t, 2> b) { return native::ushl(a, b); }, reference::operation::shift, false, false)) return false;
  if (!check_binary<std::uint8_t, std::int8_t, 8>("urshl_8_8", [](V<std::uint8_t, 8> a, V<std::int8_t, 8> b) { return native::urshl(a, b); }, reference::operation::shift, true, false)) return false;
  if (!check_binary<std::uint8_t, std::int8_t, 16>("urshl_8_16", [](V<std::uint8_t, 16> a, V<std::int8_t, 16> b) { return native::urshl(a, b); }, reference::operation::shift, true, false)) return false;
  if (!check_binary<std::uint16_t, std::int16_t, 4>("urshl_16_4", [](V<std::uint16_t, 4> a, V<std::int16_t, 4> b) { return native::urshl(a, b); }, reference::operation::shift, true, false)) return false;
  if (!check_binary<std::uint16_t, std::int16_t, 8>("urshl_16_8", [](V<std::uint16_t, 8> a, V<std::int16_t, 8> b) { return native::urshl(a, b); }, reference::operation::shift, true, false)) return false;
  if (!check_binary<std::uint32_t, std::int32_t, 2>("urshl_32_2", [](V<std::uint32_t, 2> a, V<std::int32_t, 2> b) { return native::urshl(a, b); }, reference::operation::shift, true, false)) return false;
  if (!check_binary<std::uint32_t, std::int32_t, 4>("urshl_32_4", [](V<std::uint32_t, 4> a, V<std::int32_t, 4> b) { return native::urshl(a, b); }, reference::operation::shift, true, false)) return false;
  if (!check_binary<std::uint64_t, std::int64_t, 1>("urshl_64_1", [](V<std::uint64_t, 1> a, V<std::int64_t, 1> b) { return native::urshl(a, b); }, reference::operation::shift, true, false)) return false;
  if (!check_binary<std::uint64_t, std::int64_t, 2>("urshl_64_2", [](V<std::uint64_t, 2> a, V<std::int64_t, 2> b) { return native::urshl(a, b); }, reference::operation::shift, true, false)) return false;
  if (!check_binary<std::uint8_t, std::int8_t, 8>("uqshl_8_8", [](V<std::uint8_t, 8> a, V<std::int8_t, 8> b) { return native::uqshl(a, b); }, reference::operation::shift, false, true)) return false;
  if (!check_binary<std::uint8_t, std::int8_t, 16>("uqshl_8_16", [](V<std::uint8_t, 16> a, V<std::int8_t, 16> b) { return native::uqshl(a, b); }, reference::operation::shift, false, true)) return false;
  if (!check_binary<std::uint16_t, std::int16_t, 4>("uqshl_16_4", [](V<std::uint16_t, 4> a, V<std::int16_t, 4> b) { return native::uqshl(a, b); }, reference::operation::shift, false, true)) return false;
  if (!check_binary<std::uint16_t, std::int16_t, 8>("uqshl_16_8", [](V<std::uint16_t, 8> a, V<std::int16_t, 8> b) { return native::uqshl(a, b); }, reference::operation::shift, false, true)) return false;
  if (!check_binary<std::uint32_t, std::int32_t, 2>("uqshl_32_2", [](V<std::uint32_t, 2> a, V<std::int32_t, 2> b) { return native::uqshl(a, b); }, reference::operation::shift, false, true)) return false;
  if (!check_binary<std::uint32_t, std::int32_t, 4>("uqshl_32_4", [](V<std::uint32_t, 4> a, V<std::int32_t, 4> b) { return native::uqshl(a, b); }, reference::operation::shift, false, true)) return false;
  if (!check_binary<std::uint64_t, std::int64_t, 1>("uqshl_64_1", [](V<std::uint64_t, 1> a, V<std::int64_t, 1> b) { return native::uqshl(a, b); }, reference::operation::shift, false, true)) return false;
  if (!check_binary<std::uint64_t, std::int64_t, 2>("uqshl_64_2", [](V<std::uint64_t, 2> a, V<std::int64_t, 2> b) { return native::uqshl(a, b); }, reference::operation::shift, false, true)) return false;
  if (!check_binary<std::uint8_t, std::int8_t, 8>("uqrshl_8_8", [](V<std::uint8_t, 8> a, V<std::int8_t, 8> b) { return native::uqrshl(a, b); }, reference::operation::shift, true, true)) return false;
  if (!check_binary<std::uint8_t, std::int8_t, 16>("uqrshl_8_16", [](V<std::uint8_t, 16> a, V<std::int8_t, 16> b) { return native::uqrshl(a, b); }, reference::operation::shift, true, true)) return false;
  if (!check_binary<std::uint16_t, std::int16_t, 4>("uqrshl_16_4", [](V<std::uint16_t, 4> a, V<std::int16_t, 4> b) { return native::uqrshl(a, b); }, reference::operation::shift, true, true)) return false;
  if (!check_binary<std::uint16_t, std::int16_t, 8>("uqrshl_16_8", [](V<std::uint16_t, 8> a, V<std::int16_t, 8> b) { return native::uqrshl(a, b); }, reference::operation::shift, true, true)) return false;
  if (!check_binary<std::uint32_t, std::int32_t, 2>("uqrshl_32_2", [](V<std::uint32_t, 2> a, V<std::int32_t, 2> b) { return native::uqrshl(a, b); }, reference::operation::shift, true, true)) return false;
  if (!check_binary<std::uint32_t, std::int32_t, 4>("uqrshl_32_4", [](V<std::uint32_t, 4> a, V<std::int32_t, 4> b) { return native::uqrshl(a, b); }, reference::operation::shift, true, true)) return false;
  if (!check_binary<std::uint64_t, std::int64_t, 1>("uqrshl_64_1", [](V<std::uint64_t, 1> a, V<std::int64_t, 1> b) { return native::uqrshl(a, b); }, reference::operation::shift, true, true)) return false;
  if (!check_binary<std::uint64_t, std::int64_t, 2>("uqrshl_64_2", [](V<std::uint64_t, 2> a, V<std::int64_t, 2> b) { return native::uqrshl(a, b); }, reference::operation::shift, true, true)) return false;
  if (!check_narrow<std::int16_t, std::int8_t, 8>("sqxtn_16", [](V<std::int16_t, 8> a) { return native::sqxtn(a); }, [](V<std::int8_t, 8> low, V<std::int16_t, 8> a) { return native::sqxtn_high(low, a); })) return false;
  if (!check_narrow<std::int32_t, std::int16_t, 4>("sqxtn_32", [](V<std::int32_t, 4> a) { return native::sqxtn(a); }, [](V<std::int16_t, 4> low, V<std::int32_t, 4> a) { return native::sqxtn_high(low, a); })) return false;
  if (!check_narrow<std::int64_t, std::int32_t, 2>("sqxtn_64", [](V<std::int64_t, 2> a) { return native::sqxtn(a); }, [](V<std::int32_t, 2> low, V<std::int64_t, 2> a) { return native::sqxtn_high(low, a); })) return false;
  if (!check_narrow<std::uint16_t, std::uint8_t, 8>("uqxtn_16", [](V<std::uint16_t, 8> a) { return native::uqxtn(a); }, [](V<std::uint8_t, 8> low, V<std::uint16_t, 8> a) { return native::uqxtn_high(low, a); })) return false;
  if (!check_narrow<std::uint32_t, std::uint16_t, 4>("uqxtn_32", [](V<std::uint32_t, 4> a) { return native::uqxtn(a); }, [](V<std::uint16_t, 4> low, V<std::uint32_t, 4> a) { return native::uqxtn_high(low, a); })) return false;
  if (!check_narrow<std::uint64_t, std::uint32_t, 2>("uqxtn_64", [](V<std::uint64_t, 2> a) { return native::uqxtn(a); }, [](V<std::uint32_t, 2> low, V<std::uint64_t, 2> a) { return native::uqxtn_high(low, a); })) return false;
  if (!check_narrow<std::int16_t, std::uint8_t, 8>("sqxtun_16", [](V<std::int16_t, 8> a) { return native::sqxtun(a); }, [](V<std::uint8_t, 8> low, V<std::int16_t, 8> a) { return native::sqxtun_high(low, a); })) return false;
  if (!check_narrow<std::int32_t, std::uint16_t, 4>("sqxtun_32", [](V<std::int32_t, 4> a) { return native::sqxtun(a); }, [](V<std::uint16_t, 4> low, V<std::int32_t, 4> a) { return native::sqxtun_high(low, a); })) return false;
  if (!check_narrow<std::int64_t, std::uint32_t, 2>("sqxtun_64", [](V<std::int64_t, 2> a) { return native::sqxtun(a); }, [](V<std::uint32_t, 2> low, V<std::int64_t, 2> a) { return native::sqxtun_high(low, a); })) return false;
  return true;
}

#pragma clang attribute pop

int main() {
  if (!native::classify_isa(native::observe_arm_capabilities(), architecture).admitted())
    return 77;
  auto old = fpsr();
  bool ok = run();
  set_fpsr(old);
  return ok ? 0 : 1;
}
