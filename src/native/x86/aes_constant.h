// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#include "native/detail/aes_constant.h"
#include "native/x86/detail/constant_lanes.h"

// Intel AES round ordering: the round key is added after substitution,
// shifting and (except in the last round) mixing. ARM AESE/AESD add it first.
namespace native::detail::x86_aes_constant {
  using aes_constant::field_inverse;
  using aes_constant::field_product;
  using aes_constant::substitute;
  using aes_constant::mix;
  using x86_constant::lanes;

  template<bool Inverse, bool Last, class V>
  constexpr V round(V state, V key) noexcept {
    // Every 128-bit lane is a separate AES state with its own round key.
    static_assert(V::lanes % 16 == 0);
    auto input = lanes(state);
    auto result = input;
    auto round_key = lanes(key);
    for (unsigned base = 0; base < V::lanes; base += 16) {
      std::array<std::uint8_t, 16> block{};
      for (unsigned i = 0; i < 16; ++i) block[i] = input[base + i];
      block = aes_constant::shift_substitute<Inverse>(block);
      if constexpr (!Last) block = mix<Inverse>(block);
      for (unsigned i = 0; i < 16; ++i) result[base + i] = block[i] ^ round_key[base + i];
    }
    return V::load(result.data());
  }

  template<class V>
  constexpr V inverse_mix(V state) noexcept {
    auto result = mix<true>(lanes(state));
    return V::load(result.data());
  }

  template<unsigned Imm8, class V>
  constexpr V keygen(V state) noexcept {
    auto input = lanes(state);
    auto result = input;
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
