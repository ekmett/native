// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <array>
#include <cstdint>

namespace reference {
  using words = std::array<std::uint32_t, 4>;
  using input = std::array<words, 3>;

  constexpr std::uint32_t rotate(std::uint32_t x, unsigned distance) {
    auto n = distance % 32;
    return std::uint32_t((std::uint64_t(x) << n) | (std::uint64_t(x) >> ((32 - n) % 32)));
  }

  // Bit truth tables give parity, majority and choice independently of the
  // production word-level expressions. The high argument is the selector.
  constexpr std::uint32_t boolean(std::uint32_t a, std::uint32_t b, std::uint32_t c,
                                  unsigned table) {
    std::uint32_t result = 0;
    for (unsigned bit = 0; bit < 32; ++bit) {
      auto index = (((a >> bit) & 1) << 2) | (((b >> bit) & 1) << 1) | ((c >> bit) & 1);
      result |= ((table >> index) & 1) << bit;
    }
    return result;
  }

  constexpr words sm3ss1(input const &in) {
    return {0, 0, 0, rotate(rotate(in[0][3], 12) + in[1][3] + in[2][3], 7)};
  }

  constexpr words tt(input const &in, unsigned lane, bool second, bool late) {
    auto const &x = in[0];
    auto f = boolean(x[3], x[2], x[1], !late ? 0x96 : second ? 0xca : 0xe8);
    std::uint64_t sum = f;
    sum += x[0];
    sum += second ? in[1][3] : in[1][3] ^ rotate(x[3], 12);
    sum += in[2][lane];
    auto next = std::uint32_t(sum);
    if (second)
      next = next ^ rotate(next, 9) ^ rotate(next, 17);
    return {x[1], rotate(x[2], second ? 19 : 9), x[3], next};
  }

  constexpr words part1(input const &in) {
    words out{};
    for (unsigned i = 0; i < 4; ++i) {
      auto feedback = i == 3 ? out[0] : in[2][i + 1];
      auto base = in[0][i] ^ in[1][i] ^ rotate(feedback, 15);
      out[i] = base ^ rotate(base, 15) ^ rotate(base, 23);
    }
    return out;
  }

  constexpr words part2(input const &in) {
    words out{};
    for (unsigned i = 0; i < 4; ++i)
      out[i] = in[0][i] ^ in[1][i] ^ rotate(in[2][i], 7);
    auto feedback = in[1][0] ^ rotate(in[2][0], 7);
    out[3] ^= rotate(feedback, 15) ^ rotate(feedback, 30) ^ rotate(feedback, 38);
    return out;
  }

  // Fixed algorithm data in a separately decoded representation.
  inline constexpr char substitution[] = "d690e9fecce13db716b614c228fb2c05"
                                         "2b679a762abe04c3aa44132649860699"
                                         "9c4250f491ef987a33540b43edcfac62"
                                         "e4b31ca9c908e89580df94fa758f3fa6"
                                         "4707a7fcf37317ba83593c19e6854fa8"
                                         "686b81b27164da8bf8eb0f4b70569d35"
                                         "1e240e5e6358d1a225227c3b01217887"
                                         "d40046579fd327524c3602e7a0c4c89e"
                                         "eabf8ad240c738b5a3f7f2cef96115a1"
                                         "e0ae5da49b341a55ad933230f58cb1e3"
                                         "1df6e22e8266ca60c02923ab0d534e6f"
                                         "d5db3745defd8e2f03ff6a726d6c5b51"
                                         "8d1baf92bbddbc7f11d95c411f105ad8"
                                         "0ac13188a5cd7bbd2d74d012b8e5b4b0"
                                         "8969974a0c96777e65b9f109c56ec684"
                                         "18f07dec3adc4d2079ee5f3ed7cb3948";

  constexpr unsigned digit(char x) {
    return x <= '9' ? unsigned(x - '0') : unsigned(x - 'a' + 10);
  }

  constexpr std::uint32_t tau(std::uint32_t x) {
    std::uint32_t out = 0;
    for (unsigned i = 0; i < 4; ++i) {
      auto index = ((x >> (24 - i * 8)) & 255) * 2;
      out = (out << 8) | (digit(substitution[index]) * 16 + digit(substitution[index + 1]));
    }
    return out;
  }

  constexpr words sm4(input const &in, bool key) {
    std::array<std::uint32_t, 8> sequence{};
    for (unsigned i = 0; i < 4; ++i)
      sequence[i] = in[0][i];
    for (unsigned i = 0; i < 4; ++i) {
      auto nonlinear = tau(sequence[i + 1] ^ sequence[i + 2] ^ sequence[i + 3] ^ in[1][i]);
      auto linear = key ? nonlinear ^ rotate(nonlinear, 13) ^ rotate(nonlinear, 23)
                        : nonlinear ^ rotate(nonlinear, 2) ^ rotate(nonlinear, 10) ^
                              rotate(nonlinear, 18) ^ rotate(nonlinear, 24);
      sequence[i + 4] = sequence[i] ^ linear;
    }
    return {sequence[4], sequence[5], sequence[6], sequence[7]};
  }
} // namespace reference
