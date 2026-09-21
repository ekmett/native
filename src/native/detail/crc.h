// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <cstdint>

namespace native::detail {
  // Uncomplemented reflected CRC update, consuming exactly bits low input bits.
  constexpr std::uint32_t crc_update(std::uint32_t accumulator, std::uint64_t value,
      unsigned bits, std::uint32_t polynomial) noexcept {
    for(unsigned bit=0;bit<bits;++bit) {
      auto feedback=(accumulator^static_cast<std::uint32_t>(value))&1u;
      accumulator=(accumulator>>1)^(feedback?polynomial:0u);
      value>>=1;
    }
    return accumulator;
  }
}
