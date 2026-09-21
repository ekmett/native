// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "constant_lanes.h"
#include <limits>

namespace native::detail::arm_constant {
  template<bool Matrix, int Lane, class Acc, class Left, class Right>
  constexpr Acc dot(Acc accumulator, Left lhs, Right rhs) noexcept {
    using T = typename Acc::value_type;
    auto a = lanes(accumulator);
    auto b = lanes(lhs);
    auto c = lanes(rhs);
    for (unsigned i = 0; i < Acc::lanes; ++i) {
      auto sum = std::bit_cast<std::uint32_t>(a[i]);
      for (unsigned k = 0; k < (Matrix ? 8 : 4); ++k) {
        unsigned left = Matrix ? 8 * (i / 2) + k : 4 * i + k;
        unsigned right = Matrix ? 8 * (i % 2) + k : Lane < 0 ? 4 * i + k : 4 * Lane + k;
        // A byte product fits signed 32 bits for every signedness pairing;
        // unsigned accumulation then gives the architectural modulo-2^32 sum.
        sum += static_cast<std::uint32_t>(static_cast<std::int32_t>(b[left]) * static_cast<std::int32_t>(c[right]));
      }
      a[i] = std::bit_cast<T>(sum);
    }
    return pack<Acc>(a);
  }

  template<bool Subtract, class T>
  constexpr T rdm_scalar(T accumulator, T lhs, T rhs) noexcept {
    constexpr unsigned width = sizeof(T) * 8;
    // Keep the product unsaturated. Distributing the integral accumulator
    // across the rounding shift avoids a 65-bit doubled intermediate.
    // The product, rounding bias and final sum all fit signed 64 bits.
    auto product = static_cast<std::int64_t>(lhs) * static_cast<std::int64_t>(rhs);
    auto rounded = ((Subtract ? -product : product) + (std::int64_t{1} << (width - 2))) >> (width - 1);
    auto sum = static_cast<std::int64_t>(accumulator) + rounded;
    if (sum > std::numeric_limits<T>::max()) return std::numeric_limits<T>::max();
    if (sum < std::numeric_limits<T>::min()) return std::numeric_limits<T>::min();
    return static_cast<T>(sum);
  }

  template<bool Subtract, int Lane, class Acc, class Right>
  constexpr Acc rdm(Acc accumulator, Acc lhs, Right rhs) noexcept {
    if constexpr (std::is_integral_v<Acc>) {
      if constexpr (Lane < 0) return rdm_scalar<Subtract>(accumulator, lhs, rhs);
      else return rdm_scalar<Subtract>(accumulator, lhs, lanes(rhs)[Lane]);
    } else {
      auto a = lanes(accumulator);
      auto b = lanes(lhs);
      auto c = lanes(rhs);
      for (unsigned i = 0; i < Acc::lanes; ++i)
        a[i] = rdm_scalar<Subtract>(a[i], b[i], c[Lane < 0 ? i : Lane]);
      return pack<Acc>(a);
    }
  }
}
