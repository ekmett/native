// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#include <array>
#include <bit>
#include <cstdint>

namespace sha_fixture {
  using words = std::array<std::uint32_t, 4>;
  using results = std::array<words, 10>;

  // Scalar FIPS 180-4 state transitions use algorithm order, independent of
  // the register packing used by the implementation's constant evaluator.
  constexpr void sha1_step(std::array<std::uint32_t, 5> & s, std::uint32_t w, unsigned phase) {
    constexpr std::uint32_t constants[]{0x5a827999, 0x6ed9eba1, 0x8f1bbcdc, 0xca62c1d6};
    auto f = s[1] ^ s[2] ^ s[3];
    if (phase == 0) {
      f = s[3] ^ (s[1] & (s[2] ^ s[3]));
    } else if (phase == 2) {
      f = (s[1] & s[2]) | (s[3] & (s[1] | s[2]));
    }
    auto next = std::rotl(s[0], 5) + f + s[4] + w + constants[phase];
    s = {next, s[0], std::rotr(s[1], 2), s[2], s[3]};
  }

  constexpr void sha256_step(std::array<std::uint32_t, 8> & s, std::uint32_t wk) {
    auto sum = s[7] + (std::rotr(s[4], 6) ^ std::rotr(s[4], 11) ^ std::rotr(s[4], 25)) +
      ((s[4] & s[5]) | (~s[4] & s[6])) + wk;
    auto next = sum + (std::rotr(s[0], 2) ^ std::rotr(s[0], 13) ^ std::rotr(s[0], 22)) +
      ((s[0] & s[1]) | (s[0] & s[2]) | (s[1] & s[2]));
    s = {next, s[0], s[1], s[2], s[3] + sum, s[4], s[5], s[6]};
  }

  constexpr std::uint32_t sigma(std::uint32_t x, bool last) {
    return last ? (std::rotr(x, 17) ^ std::rotr(x, 19) ^ (x >> 10)) :
      (std::rotr(x, 7) ^ std::rotr(x, 18) ^ (x >> 3));
  }

  constexpr results oracle(words a, words b, words c) {
    results out{};
    for (unsigned phase = 0; phase < 4; ++phase) {
      std::array<std::uint32_t, 5> state{a[3], a[2], a[1], a[0], 0};
      for (unsigned r = 0; r < 4; ++r) {
        sha1_step(state, b[3 - r], phase);
      }
      out[phase] = {state[3], state[2], state[1], state[0]};
    }
    out[4] = b;
    out[4][3] += std::rotr(a[3], 2);
    std::array<std::uint32_t, 8> packed{a[3], a[2], a[1], a[0], b[3], b[2], b[1], b[0]};
    for (unsigned i = 0; i < 4; ++i) {
      out[5][3 - i] = packed[i] ^ packed[i + 2];
      out[6][3 - i] = std::rotl(a[3 - i] ^ (i == 3 ? out[6][3] : b[2 - i]), 1);
    }
    std::array<std::uint32_t, 8> state{b[3], b[2], a[3], a[2], b[1], b[0], a[1], a[0]};
    sha256_step(state, c[0]);
    sha256_step(state, c[1]);
    out[7] = {state[5], state[4], state[1], state[0]};
    for (unsigned i = 0; i < 4; ++i) {
      out[8][i] = a[i] + sigma(i == 3 ? b[0] : a[i + 1], false);
      out[9][i] = a[i] + sigma(i < 2 ? b[i + 2] : out[9][i - 2], true);
    }
    return out;
  }
}
