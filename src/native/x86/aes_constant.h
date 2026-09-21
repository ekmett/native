// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/x86/integer_constant.h"

// Intel AES round ordering: the round key is added after substitution,
// shifting and (except in the last round) mixing. ARM AESE/AESD add it first.
namespace native::detail::x86_aes_constant {
  using x86_instruction_constant::field_inverse;
  using x86_instruction_constant::field_product;
  using x86_instruction_constant::lanes;

  template<bool Inverse> constexpr std::uint8_t substitute(std::uint8_t value) noexcept {
    if constexpr (Inverse)
      return field_inverse(static_cast<std::uint8_t>(
        std::rotl(value, 1) ^ std::rotl(value, 3) ^ std::rotl(value, 6) ^ 5));
    else {
      auto x = field_inverse(value);
      return static_cast<std::uint8_t>(x ^ std::rotl(x, 1) ^ std::rotl(x, 2) ^
        std::rotl(x, 3) ^ std::rotl(x, 4) ^ 0x63);
    }
  }

  template<bool Inverse> constexpr auto mix(std::array<std::uint8_t, 16> input) noexcept {
    std::array<std::uint8_t, 16> result{};
    constexpr std::uint8_t coefficients[2][4]{{2, 3, 1, 1}, {14, 11, 13, 9}};
    for (unsigned column = 0; column < 4; ++column)
      for (unsigned row = 0; row < 4; ++row)
        for (unsigned term = 0; term < 4; ++term)
          result[4 * column + row] ^= field_product(input[4 * column + term],
            coefficients[Inverse][(term + 4 - row) % 4]);
    return result;
  }

  template<bool Inverse, bool Last, class V> constexpr V round(V state, V key) noexcept {
    auto input = lanes(state), result = input, round_key = lanes(key);
    for (unsigned column = 0; column < 4; ++column)
      for (unsigned row = 0; row < 4; ++row) {
        auto source = 4 * ((column + (Inverse ? 4 - row : row)) % 4) + row;
        result[4 * column + row] = substitute<Inverse>(input[source]);
      }
    if constexpr (!Last) result = mix<Inverse>(result);
    for (unsigned i = 0; i < 16; ++i) result[i] ^= round_key[i];
    return V::load(result.data());
  }

  template<class V> constexpr V inverse_mix(V state) noexcept {
    auto result = mix<true>(lanes(state));
    return V::load(result.data());
  }

  template<unsigned Imm8, class V> constexpr V keygen(V state) noexcept {
    auto input = lanes(state), result = input;
    for (unsigned half = 0; half < 2; ++half) {
      auto source = 8 * half + 4;
      for (unsigned i = 0; i < 4; ++i) {
        result[8 * half + i] = substitute<false>(input[source + i]);
        result[8 * half + 4 + i] = substitute<false>(input[source + (i + 1) % 4]);
      }
      result[8 * half + 4] ^= Imm8;
    }
    return V::load(result.data());
  }
}
