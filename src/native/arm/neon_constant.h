// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/arm/detail/constant_lanes.h"
#include <limits>

namespace native::detail::arm_neon_constant {
  template <class T> constexpr T add(T a, T b) noexcept {
    constexpr auto lo = std::numeric_limits<T>::min();
    constexpr auto hi = std::numeric_limits<T>::max();
    if constexpr (std::is_signed_v<T>) {
      if (b > 0 && a > hi - b)
        return hi;
      if (b < 0 && a < lo - b)
        return lo;
    } else if (a > hi - b) {
      return hi;
    }
    return T(a + b);
  }

  template <class T> constexpr T sub(T a, T b) noexcept {
    constexpr auto lo = std::numeric_limits<T>::min();
    constexpr auto hi = std::numeric_limits<T>::max();
    if constexpr (std::is_signed_v<T>) {
      if (b < 0 && a > hi + b)
        return hi;
      if (b > 0 && a < lo + b)
        return lo;
    } else if (a < b) {
      return 0;
    }
    return T(a - b);
  }

  template <class To, class From> constexpr To narrow(From a) noexcept {
    if (a < From(std::numeric_limits<To>::min()))
      return std::numeric_limits<To>::min();
    if (a > From(std::numeric_limits<To>::max()))
      return std::numeric_limits<To>::max();
    return To(a);
  }

  template <bool Round, class T> constexpr T multiply_high(T a, T b) noexcept {
    constexpr unsigned width = sizeof(T) * 8;
    auto product = std::int64_t(a) * b;
    if constexpr (Round)
      product += std::int64_t{1} << (width - 2);
    return narrow<T>(product >> (width - 1));
  }

  template <bool Round, bool Saturate, class T, class S>
  constexpr T shift(T value, S count) noexcept {
    using U = std::make_unsigned_t<T>;
    constexpr unsigned width = sizeof(T) * 8;
    // The architecture consumes only the signed low byte of each count lane.
    int amount = std::bit_cast<std::int8_t>(std::uint8_t(count));
    if (amount >= 0) {
      unsigned left = unsigned(amount);
      if (left >= width) {
        if constexpr (Saturate) {
          if (value != 0)
            return value < 0 ? std::numeric_limits<T>::min() : std::numeric_limits<T>::max();
        }
        return 0;
      }
      if constexpr (Saturate) {
        if (value > (std::numeric_limits<T>::max() >> left))
          return std::numeric_limits<T>::max();
        if constexpr (std::is_signed_v<T>)
          if (value < (std::numeric_limits<T>::min() >> left))
            return std::numeric_limits<T>::min();
      }
      return std::bit_cast<T>(U(std::uint64_t(U(value)) << left));
    }
    unsigned right = unsigned(-amount);
    if (right >= width) {
      if constexpr (Round) {
        if constexpr (std::is_unsigned_v<T>)
          if (right == width)
            return T(value >> (width - 1));
        return 0;
      } else {
        return value < 0 ? T(-1) : T(0);
      }
    }
    T result = T(value >> right);
    if constexpr (Round)
      result = T(result + T((U(value) >> (right - 1)) & 1));
    return result;
  }

  template <class V, class W, class F> constexpr V binary(V a, W b, F operation) noexcept {
    auto left = arm_constant::lanes(a);
    auto right = arm_constant::lanes(b);
    for (unsigned i = 0; i < V::lanes; ++i)
      left[i] = operation(left[i], right[i]);
    return arm_constant::pack<V>(left);
  }

  template <class Result, class V> constexpr Result narrow_low(V input) noexcept {
    auto source = arm_constant::lanes(input);
    std::array<typename Result::value_type, Result::lanes> result{};
    for (unsigned i = 0; i < Result::lanes; ++i)
      result[i] = narrow<typename Result::value_type>(source[i]);
    return arm_constant::pack<Result>(result);
  }

  template <class Result, class Low, class V>
  constexpr Result narrow_high(Low low, V input) noexcept {
    auto bottom = arm_constant::lanes(low);
    auto source = arm_constant::lanes(input);
    std::array<typename Result::value_type, Result::lanes> result{};
    for (unsigned i = 0; i < Low::lanes; ++i) {
      result[i] = bottom[i];
      result[i + Low::lanes] = narrow<typename Result::value_type>(source[i]);
    }
    return arm_constant::pack<Result>(result);
  }
} // namespace native::detail::arm_neon_constant
