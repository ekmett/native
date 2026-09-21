// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/arm/detail/constant_lanes.h"

namespace native::detail::arm_sm3_constant {
  constexpr std::uint32_t p1(std::uint32_t value) noexcept {
    return value ^ std::rotl(value, 15) ^ std::rotl(value, 23);
  }

  template<class V> constexpr V sm3ss1(V a, V b, V c) noexcept {
    auto x = arm_constant::lanes(a);
    auto y = arm_constant::lanes(b);
    auto z = arm_constant::lanes(c);
    std::array<std::uint32_t, 4> result{0, 0, 0, std::rotl(std::rotl(x[3], 12) + y[3] + z[3], 7)};
    return arm_constant::pack<V>(result);
  }

  template<int Lane, bool Second, bool Late, class V>
  constexpr V round(V state, V ss1, V words) noexcept {
    auto x = arm_constant::lanes(state);
    auto s = arm_constant::lanes(ss1)[3];
    auto word = arm_constant::lanes(words)[Lane];
    auto boolean = x[3] ^ x[2] ^ x[1];
    if constexpr (Late && Second)
      boolean = (x[3] & x[2]) | (~x[3] & x[1]);
    else if constexpr (Late)
      boolean = (x[3] & x[2]) | (x[3] & x[1]) | (x[2] & x[1]);
    if constexpr (!Second)
      s ^= std::rotl(x[3], 12);
    auto next = boolean + x[0] + s + word;
    if constexpr (Second)
      next ^= std::rotl(next, 9) ^ std::rotl(next, 17);
    std::array<std::uint32_t, 4> result{x[1], std::rotl(x[2], Second ? 19 : 9), x[3], next};
    return arm_constant::pack<V>(result);
  }

  template<int Lane, class V> constexpr V sm3tt1a(V a, V b, V c) noexcept {
    return round<Lane, false, false>(a, b, c);
  }

  template<int Lane, class V> constexpr V sm3tt1b(V a, V b, V c) noexcept {
    return round<Lane, false, true>(a, b, c);
  }

  template<int Lane, class V> constexpr V sm3tt2a(V a, V b, V c) noexcept {
    return round<Lane, true, false>(a, b, c);
  }

  template<int Lane, class V> constexpr V sm3tt2b(V a, V b, V c) noexcept {
    return round<Lane, true, true>(a, b, c);
  }

  template<class V> constexpr V sm3partw1(V a, V b, V c) noexcept {
    auto x = arm_constant::lanes(a);
    auto y = arm_constant::lanes(b);
    auto z = arm_constant::lanes(c);
    std::array<std::uint32_t, 4> result{};
    for (unsigned i = 0; i < 3; ++i)
      result[i] = p1(x[i] ^ y[i] ^ std::rotl(z[i + 1], 15));
    result[3] = p1(x[3] ^ y[3] ^ std::rotl(result[0], 15));
    return arm_constant::pack<V>(result);
  }

  template<class V> constexpr V sm3partw2(V a, V b, V c) noexcept {
    auto x = arm_constant::lanes(a);
    auto y = arm_constant::lanes(b);
    auto z = arm_constant::lanes(c);
    auto first = y[0] ^ std::rotl(z[0], 7);
    for (unsigned i = 0; i < 4; ++i)
      x[i] ^= y[i] ^ std::rotl(z[i], 7);
    x[3] ^= p1(std::rotl(first, 15));
    return arm_constant::pack<V>(x);
  }
} // namespace native::detail::arm_sm3_constant
