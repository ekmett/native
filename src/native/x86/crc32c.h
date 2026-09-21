// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#include "native/detail/crc.h"
#include <cstdint>
#if NATIVE_HOST_X86
#include <immintrin.h>
#endif

#if NATIVE_HOST_X86 || defined(NATIVE_DOXYGEN)
namespace native {
/** \defgroup x86_crc32c CRC32C
 * Raw Castagnoli CRC updates requiring CPUID leaf 1 ECX bit 20.
 * Runtime calls require Arch to contain x86_feature::crc32; the caller must enable and admit
 * that feature. No vector OS state is required. Each update consumes the
 * operand's bits from least to most significant, using reflected polynomial
 * 0x82f63b78. No initial or final complement is applied.
 * Constant evaluation supports every Arch; weak tags select consteval overloads.
 * \{ */
  /// Update a 32-bit CRC32C accumulator with exactly eight bits.
  template<isa Arch> requires(Arch.has(x86_feature::crc32))
  native_nodiscard native_inline native_const native_target("crc32")
  constexpr std::uint32_t crc32c(std::uint32_t accumulator, std::uint8_t value) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return detail::crc_update(accumulator, value, sizeof(value) * 8, 0x82f63b78u);
    } else {
      return _mm_crc32_u8(accumulator, value);
    }
  }

  /// Update a 32-bit CRC32C accumulator with exactly sixteen bits.
  template<isa Arch> requires(Arch.has(x86_feature::crc32))
  native_nodiscard native_inline native_const native_target("crc32")
  constexpr std::uint32_t crc32c(std::uint32_t accumulator, std::uint16_t value) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return detail::crc_update(accumulator, value, sizeof(value) * 8, 0x82f63b78u);
    } else {
      return _mm_crc32_u16(accumulator, value);
    }
  }

  /// Update a 32-bit CRC32C accumulator with exactly thirty-two bits.
  template<isa Arch> requires(Arch.has(x86_feature::crc32))
  native_nodiscard native_inline native_const native_target("crc32")
  constexpr std::uint32_t crc32c(std::uint32_t accumulator, std::uint32_t value) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return detail::crc_update(accumulator, value, sizeof(value) * 8, 0x82f63b78u);
    } else {
      return _mm_crc32_u32(accumulator, value);
    }
  }

#if defined(__x86_64__) || defined(_M_X64) || defined(NATIVE_DOXYGEN)
  /// Update with sixty-four bits in x86-64 mode; the CRC remains 32 bits.
  /// The instruction zeroes the high half of its 64-bit destination register.
  template<isa Arch> requires(Arch.has(x86_feature::crc32))
  native_nodiscard native_inline native_const native_target("crc32")
  constexpr std::uint32_t crc32c(std::uint32_t accumulator, std::uint64_t value) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return detail::crc_update(accumulator, value, sizeof(value) * 8, 0x82f63b78u);
    } else {
      return static_cast<std::uint32_t>(_mm_crc32_u64(accumulator, value));
    }
  }
#endif

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa Arch> requires(!Arch.has(x86_feature::crc32))
  native_nodiscard
  consteval std::uint32_t crc32c(std::uint32_t accumulator, std::uint8_t value) noexcept {
    return crc32c<isa{x86_feature::crc32}>(accumulator, value);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa Arch> requires(!Arch.has(x86_feature::crc32))
  native_nodiscard
  consteval std::uint32_t crc32c(std::uint32_t accumulator, std::uint16_t value) noexcept {
    return crc32c<isa{x86_feature::crc32}>(accumulator, value);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa Arch> requires(!Arch.has(x86_feature::crc32))
  native_nodiscard
  consteval std::uint32_t crc32c(std::uint32_t accumulator, std::uint32_t value) noexcept {
    return crc32c<isa{x86_feature::crc32}>(accumulator, value);
  }

#if defined(__x86_64__) || defined(_M_X64) || defined(NATIVE_DOXYGEN)
  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa Arch> requires(!Arch.has(x86_feature::crc32))
  native_nodiscard
  consteval std::uint32_t crc32c(std::uint32_t accumulator, std::uint64_t value) noexcept {
    return crc32c<isa{x86_feature::crc32}>(accumulator, value);
  }
#endif

/// \}
}
#endif
