// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#include <cstdint>
#if NATIVE_HOST_X86
#include <immintrin.h>

namespace native {
/** \defgroup x86_crc32c CRC32C
 * Raw Castagnoli CRC updates requiring CPUID leaf 1 ECX bit 20.
 * Arch must contain x86_feature::crc32; the caller must enable and admit
 * that feature. No vector OS state is required. Each update consumes the
 * operand's bits from least to most significant, using reflected polynomial
 * 0x82f63b78. No initial or final complement is applied.
 * \{ */
  /// Update a 32-bit CRC32C accumulator with exactly eight bits.
  template<isa Arch> requires(Arch.has(x86_feature::crc32))
  native_nodiscard native_inline native_const native_target("crc32")
  std::uint32_t crc32c(std::uint32_t accumulator, std::uint8_t value) noexcept {
    return _mm_crc32_u8(accumulator, value);
  }

  /// Update a 32-bit CRC32C accumulator with exactly sixteen bits.
  template<isa Arch> requires(Arch.has(x86_feature::crc32))
  native_nodiscard native_inline native_const native_target("crc32")
  std::uint32_t crc32c(std::uint32_t accumulator, std::uint16_t value) noexcept {
    return _mm_crc32_u16(accumulator, value);
  }

  /// Update a 32-bit CRC32C accumulator with exactly thirty-two bits.
  template<isa Arch> requires(Arch.has(x86_feature::crc32))
  native_nodiscard native_inline native_const native_target("crc32")
  std::uint32_t crc32c(std::uint32_t accumulator, std::uint32_t value) noexcept {
    return _mm_crc32_u32(accumulator, value);
  }

#if defined(__x86_64__) || defined(_M_X64)
  /// Update with sixty-four bits in x86-64 mode; the CRC remains 32 bits.
  /// The instruction zeroes the high half of its 64-bit destination register.
  template<isa Arch> requires(Arch.has(x86_feature::crc32))
  native_nodiscard native_inline native_const native_target("crc32")
  std::uint32_t crc32c(std::uint32_t accumulator, std::uint64_t value) noexcept {
    return static_cast<std::uint32_t>(_mm_crc32_u64(accumulator, value));
  }
#endif
/// \}
}
#endif
