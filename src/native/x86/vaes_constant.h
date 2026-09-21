// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#include "native/x86/aes_constant.h"

namespace native::detail::x86_vaes_constant {
  // Every 128-bit lane is a separate AES state with its own round key. Reuse
  // the AES substitution and field-mixing primitives without cross-lane work.
  template<bool Inverse, bool Last, class V>
  constexpr V round(V state, V key) noexcept {
    auto input = x86_aes_constant::lanes(state);
    auto round_key = x86_aes_constant::lanes(key);
    auto result = input;
    for (unsigned base = 0; base < V::lanes; base += 16) {
      std::array<std::uint8_t, 16> block{};
      for (unsigned column = 0; column < 4; ++column) {
        for (unsigned row = 0; row < 4; ++row) {
          auto source = 4 * ((column + (Inverse ? 4 - row : row)) % 4) + row;
          block[4 * column + row] = x86_aes_constant::substitute<Inverse>(input[base + source]);
        }
      }
      if constexpr (!Last) {
        block = x86_aes_constant::mix<Inverse>(block);
      }
      for (unsigned i = 0; i < 16; ++i) {
        result[base + i] = block[i] ^ round_key[base + i];
      }
    }
    return V::load(result.data());
  }
}
