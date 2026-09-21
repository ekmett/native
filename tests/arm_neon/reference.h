// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
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
