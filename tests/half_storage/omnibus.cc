// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import simd;
std::uint32_t omnibus_half_bits() {
  return (std::uint32_t(simd::fp16(1.0f).to_bits()) << 16) | simd::bf16(1.0f).to_bits();
}
