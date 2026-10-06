// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
/** \file
 * \brief Shared AES byte operations for constant evaluation on ARM and x86.
 *
 * Field arithmetic uses the AES polynomial 0x11b. A 16-byte state is stored
 * by column, with four row bytes per column. These transforms do not add a
 * round key: each instruction adapter applies it at the architecture's step.
 */
#pragma once

#include <array>
#include <bit>
#include <cstdint>

namespace native::detail::aes_constant {
  constexpr std::uint8_t field_product(std::uint8_t a, std::uint8_t b) noexcept {
    unsigned x = a, y = b, result = 0;
    for (unsigned bit = 0; bit < 8; ++bit) {
      if (y & 1) result ^= x;
      y >>= 1;
      x = (x << 1) ^ ((x & 0x80) ? 0x11b : 0);
    }
    return static_cast<std::uint8_t>(result);
  }

  constexpr std::uint8_t field_inverse(std::uint8_t value) noexcept {
    // In GF(256), x^254 is x^-1 for nonzero x; zero remains zero.
    std::uint8_t result = 1;
    for (unsigned exponent = 254; exponent; exponent >>= 1) {
      if (exponent & 1) result = field_product(result, value);
      value = field_product(value, value);
    }
    return result;
  }

  template<bool Inverse>
  constexpr std::uint8_t substitute(std::uint8_t value) noexcept {
    if constexpr (Inverse) {
      return field_inverse(static_cast<std::uint8_t>(
        std::rotl(value, 1) ^ std::rotl(value, 3) ^ std::rotl(value, 6) ^ 5));
    } else {
      auto x = field_inverse(value);
      return static_cast<std::uint8_t>(x ^ std::rotl(x, 1) ^ std::rotl(x, 2) ^
        std::rotl(x, 3) ^ std::rotl(x, 4) ^ 0x63);
    }
  }

  template<bool Inverse>
  constexpr auto shift_substitute(std::array<std::uint8_t, 16> const & input) noexcept {
    std::array<std::uint8_t, 16> result{};
    for (unsigned column = 0; column < 4; ++column) {
      for (unsigned row = 0; row < 4; ++row) {
        auto source = 4 * ((column + (Inverse ? 4 - row : row)) % 4) + row;
        result[4 * column + row] = substitute<Inverse>(input[source]);
      }
    }
    return result;
  }

  template<bool Inverse>
  constexpr auto mix(std::array<std::uint8_t, 16> const & input) noexcept {
    std::array<std::uint8_t, 16> result{};
    constexpr std::uint8_t coefficients[2][4]{{2, 3, 1, 1}, {14, 11, 13, 9}};
    for (unsigned column = 0; column < 4; ++column) {
      for (unsigned row = 0; row < 4; ++row) {
        for (unsigned term = 0; term < 4; ++term) {
          result[4 * column + row] ^= field_product(input[4 * column + term],
            coefficients[Inverse][(term + 4 - row) % 4]);
        }
      }
    }
    return result;
  }
}
