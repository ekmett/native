// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#include <cstdint>
#if NATIVE_HOST_NEON
#include <arm_acle.h>
#endif

#if NATIVE_HOST_NEON || defined(NATIVE_DOXYGEN)
namespace native {
/** \defgroup arm_crc ARM CRC updates
 * Raw CRC32 and CRC32C updates, consuming operand bits from least to most
 * significant. CRC32 uses reflected polynomial 0xedb88320; CRC32C uses
 * 0x82f63b78. Neither applies an initial or final complement. Both require
 * arm_feature::crc; the caller must enable and admit the crc target.
 * The accumulator and result are always 32 bits, including 64-bit operands.
 * These integer operations do not change floating-point state.
 * \{ */
  /// Update the CRC32 accumulator with exactly 8 input bits.
  template<isa Arch> requires(Arch.has(arm_feature::crc))
  native_nodiscard native_inline native_const native_target("crc")
  std::uint32_t crc32(std::uint32_t accumulator, std::uint8_t value) noexcept {
    return __crc32b(accumulator,value);
  }

  /// Update the CRC32 accumulator with exactly 16 input bits.
  template<isa Arch> requires(Arch.has(arm_feature::crc))
  native_nodiscard native_inline native_const native_target("crc")
  std::uint32_t crc32(std::uint32_t accumulator, std::uint16_t value) noexcept {
    return __crc32h(accumulator,value);
  }

  /// Update the CRC32 accumulator with exactly 32 input bits.
  template<isa Arch> requires(Arch.has(arm_feature::crc))
  native_nodiscard native_inline native_const native_target("crc")
  std::uint32_t crc32(std::uint32_t accumulator, std::uint32_t value) noexcept {
    return __crc32w(accumulator,value);
  }

  /// Update the CRC32 accumulator with exactly 64 input bits.
  template<isa Arch> requires(Arch.has(arm_feature::crc))
  native_nodiscard native_inline native_const native_target("crc")
  std::uint32_t crc32(std::uint32_t accumulator, std::uint64_t value) noexcept {
    return __crc32d(accumulator,value);
  }

  /// Reject implicit narrowing or conversion to a different operand width.
  template<isa Arch, class A, class B>
  void crc32(A, B) = delete;

  /// Update the CRC32C accumulator with exactly 8 input bits.
  template<isa Arch> requires(Arch.has(arm_feature::crc))
  native_nodiscard native_inline native_const native_target("crc")
  std::uint32_t crc32c(std::uint32_t accumulator, std::uint8_t value) noexcept {
    return __crc32cb(accumulator,value);
  }

  /// Update the CRC32C accumulator with exactly 16 input bits.
  template<isa Arch> requires(Arch.has(arm_feature::crc))
  native_nodiscard native_inline native_const native_target("crc")
  std::uint32_t crc32c(std::uint32_t accumulator, std::uint16_t value) noexcept {
    return __crc32ch(accumulator,value);
  }

  /// Update the CRC32C accumulator with exactly 32 input bits.
  template<isa Arch> requires(Arch.has(arm_feature::crc))
  native_nodiscard native_inline native_const native_target("crc")
  std::uint32_t crc32c(std::uint32_t accumulator, std::uint32_t value) noexcept {
    return __crc32cw(accumulator,value);
  }

  /// Update the CRC32C accumulator with exactly 64 input bits.
  template<isa Arch> requires(Arch.has(arm_feature::crc))
  native_nodiscard native_inline native_const native_target("crc")
  std::uint32_t crc32c(std::uint32_t accumulator, std::uint64_t value) noexcept {
    return __crc32cd(accumulator,value);
  }

  /// Reject implicit narrowing or conversion to a different operand width.
  template<isa Arch, class A, class B>
  void crc32c(A, B) = delete;

/// \}
}
#endif
