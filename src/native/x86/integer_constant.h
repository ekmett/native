// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#include "native/detail/aes_constant.h"
#include "native/x86/detail/constant_lanes.h"
#include <array>
#include <bit>
#include <cstdint>
#include <limits>
#include <type_traits>

// Integer instruction semantics used only by the constant-evaluation branches.
// Runtime wrappers continue to call their target-specific native helpers.
namespace native::detail::x86_instruction_constant {
  using x86_constant::lanes;

  template<unsigned Imm8, class V>
  constexpr V carryless(V a, V b) noexcept {
    auto aa = lanes(a), bb = lanes(b);
    decltype(aa) result{};
    for (std::size_t lane = 0; lane < V::lanes; lane += 2) {
      auto x = aa[lane + (Imm8 & 1)];
      auto y = bb[lane + ((Imm8 >> 4) & 1)];
      for (unsigned bit = 0; bit < 64; ++bit) if ((y >> bit) & 1) {
        result[lane] ^= x << bit;
        if (bit) result[lane + 1] ^= x >> (64 - bit);
      }
    }
    return V::load(result.data());
  }

  template<class V>
  constexpr V population(V value, V source, std::uint64_t mask) noexcept {
    auto input = lanes(value), result = lanes(source);
    for (std::size_t lane = 0; lane < V::lanes; ++lane)
      if ((mask >> lane) & 1)
        result[lane] = static_cast<typename V::value_type>(std::popcount(input[lane]));
    return V::load(result.data());
  }

  using aes_constant::field_product;
  using aes_constant::field_inverse;

  template<class V>
  constexpr V field_multiply(V a, V b, V source, std::uint64_t mask) noexcept {
    auto aa = lanes(a), bb = lanes(b), result = lanes(source);
    for (std::size_t lane = 0; lane < V::lanes; ++lane)
      if ((mask >> lane) & 1) result[lane] = field_product(aa[lane], bb[lane]);
    return V::load(result.data());
  }

  template<unsigned Imm8, bool Inverse, class V, class M>
  constexpr V field_affine(V a, M matrix, V source, std::uint64_t mask) noexcept {
    auto aa = lanes(a), result = lanes(source);
    auto rows = lanes(matrix);
    for (std::size_t lane = 0; lane < V::lanes; ++lane) if ((mask >> lane) & 1) {
      auto value = Inverse ? field_inverse(aa[lane]) : aa[lane];
      unsigned byte = Imm8;
      for (unsigned bit = 0; bit < 8; ++bit) {
        auto row = static_cast<unsigned>(rows[lane / 8] >> (8 * (7 - bit))) & 255;
        byte ^= (std::popcount(row & value) & 1u) << bit;
      }
      result[lane] = static_cast<std::uint8_t>(byte);
    }
    return V::load(result.data());
  }

  template<bool Saturate, class V, class A, class B>
  constexpr V dot(V accumulator, A a, B b, std::uint64_t mask, bool zero) noexcept {
    using T = typename V::value_type;
    auto result = lanes(accumulator);
    auto aa = lanes(a);
    auto bb = lanes(b);
    constexpr auto group = A::lanes / V::lanes;
    for (std::size_t lane = 0; lane < V::lanes; ++lane) {
      if (!((mask >> lane) & 1)) {
        if (zero) result[lane] = 0;
        continue;
      }
      // Even two unsigned 16-bit products plus an unsigned accumulator fit
      // in int64_t. Saturate the complete sum, never an intermediate pair.
      std::int64_t sum = result[lane];
      for (std::size_t part = 0; part < group; ++part)
        sum += std::int64_t(aa[group * lane + part]) * std::int64_t(bb[group * lane + part]);
      if constexpr (Saturate) {
        if (sum < std::numeric_limits<T>::min()) sum = std::numeric_limits<T>::min();
        if (sum > std::numeric_limits<T>::max()) sum = std::numeric_limits<T>::max();
      }
      result[lane] = std::bit_cast<T>(static_cast<std::uint32_t>(sum));
    }
    return V::load(result.data());
  }
}
