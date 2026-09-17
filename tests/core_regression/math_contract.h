#pragma once
#include <cstdint>
namespace simd::math_test {
  constexpr bool equivalent_fp32(std::uint32_t a, std::uint32_t b) noexcept {
    return a == b || ((a & 0x7fffffffu) > 0x7f800000u && (b & 0x7fffffffu) > 0x7f800000u);
  }
  static_assert(equivalent_fp32(0xff800001u, 0x7fffffffu));
  static_assert(!equivalent_fp32(0x7fc00000u, 0x7f800000u));
  static_assert(!equivalent_fp32(0u, 0x80000000u));
  static_assert(!equivalent_fp32(0x3f800000u, 0x3f800001u));
}
/**
 * \file
 * \license
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
 * SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
 * \endlicense
 * \author Edward Kmett <ekmett@gmail.com>
 * \brief Compares reproducible FP32 results with NaN encodings outside contract.
 */
