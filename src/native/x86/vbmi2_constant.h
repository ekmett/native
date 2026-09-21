// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace native::detail::x86_vbmi2_constant {
  template<bool Expand, class V>
  constexpr V compact(V source, std::uint64_t mask, V value) noexcept {
    std::array<typename V::value_type, V::lanes> input{}, result{};
    value.store(input.data());
    source.store(result.data());
    std::size_t packed = 0;
    for (std::size_t lane = 0; lane < V::lanes; ++lane) {
      if ((mask >> lane) & 1) {
        if constexpr (Expand) {
          result[lane] = input[packed++];
        } else {
          result[packed++] = input[lane];
        }
      }
    }
    return V::load(result.data());
  }

  template<class T, class V>
  constexpr void compress_store(T * destination, std::uint64_t mask, V value) noexcept {
    std::array<T, V::lanes> input{};
    value.store(input.data());
    std::size_t packed = 0;
    for (std::size_t lane = 0; lane < V::lanes; ++lane) {
      if ((mask >> lane) & 1) {
        destination[packed++] = input[lane];
      }
    }
  }

  template<class V, class T>
  constexpr V expand_load(V source, std::uint64_t mask, T * memory) noexcept {
    std::array<typename V::value_type, V::lanes> result{};
    source.store(result.data());
    std::size_t packed = 0;
    for (std::size_t lane = 0; lane < V::lanes; ++lane) {
      if ((mask >> lane) & 1) {
        result[lane] = memory[packed++];
      }
    }
    return V::load(result.data());
  }

  template<bool Right, class T>
  constexpr T shift_lane(T a, T b, unsigned count) noexcept {
    constexpr unsigned bits = sizeof(T) * 8;
    count &= bits - 1;
    if (!count) {
      return a;
    }
    // Widen narrow unsigned lanes before shifting to avoid signed promotions.
    auto first = static_cast<std::uint64_t>(a);
    auto second = static_cast<std::uint64_t>(b);
    if constexpr (Right) {
      return static_cast<T>((first >> count) | (second << (bits - count)));
    } else {
      return static_cast<T>((first << count) | (second >> (bits - count)));
    }
  }

  template<bool Right, class V, class C>
  constexpr V shift(V a, V b, C counts, V source, std::uint64_t mask) noexcept {
    using lane_type = typename V::value_type;
    std::array<lane_type, V::lanes> first{}, second{}, selectors{}, result{};
    a.store(first.data());
    b.store(second.data());
    source.store(result.data());
    if constexpr (!std::is_integral_v<C>) {
      counts.store(selectors.data());
    }
    for (std::size_t lane = 0; lane < V::lanes; ++lane) {
      if ((mask >> lane) & 1) {
        unsigned count;
        if constexpr (std::is_integral_v<C>) {
          count = counts;
        } else {
          count = static_cast<unsigned>(selectors[lane]);
        }
        result[lane] = shift_lane<Right>(first[lane], second[lane], count);
      }
    }
    return V::load(result.data());
  }
}
