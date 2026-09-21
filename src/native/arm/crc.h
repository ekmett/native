// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#include "native/detail/crc.h"
#include <cstdint>
#if NATIVE_HOST_NEON
#include <arm_acle.h>
#endif

#if NATIVE_HOST_NEON || defined(NATIVE_DOXYGEN)
namespace native {
/** \defgroup arm_crc ARM CRC updates
 * Raw CRC32 and CRC32C updates, consuming operand bits from least to most
 * significant. CRC32 uses reflected polynomial 0xedb88320; CRC32C uses
 * 0x82f63b78. Neither applies an initial or final complement. Runtime calls
 * require arm_feature::crc and an enabled, admitted crc target. Constant
 * evaluation also accepts ISA tags without CRC; it supplies no runtime fallback.
 * The accumulator and result are always 32 bits, including 64-bit operands.
 * These integer operations do not change floating-point state.
 * \{ */
  /// Update the CRC32 accumulator with exactly 8 input bits.
  template<isa Arch> requires(Arch.has(arm_feature::crc))
  native_nodiscard native_inline native_const native_target("crc")
  constexpr std::uint32_t crc32(std::uint32_t accumulator, std::uint8_t value) noexcept {
    if(__builtin_is_constant_evaluated()) return detail::crc_update(accumulator,value,8,0xedb88320u);
    return __crc32b(accumulator,value);
  }

  /// Evaluate the 8-bit CRC32 update at compile time without requiring CRC instructions.
  template<isa Arch> requires(!Arch.has(arm_feature::crc))
  native_nodiscard consteval std::uint32_t crc32(std::uint32_t accumulator, std::uint8_t value) noexcept {
    return detail::crc_update(accumulator,value,8,0xedb88320u);
  }

  /// Update the CRC32 accumulator with exactly 16 input bits.
  template<isa Arch> requires(Arch.has(arm_feature::crc))
  native_nodiscard native_inline native_const native_target("crc")
  constexpr std::uint32_t crc32(std::uint32_t accumulator, std::uint16_t value) noexcept {
    if(__builtin_is_constant_evaluated()) return detail::crc_update(accumulator,value,16,0xedb88320u);
    return __crc32h(accumulator,value);
  }

  /// Evaluate the 16-bit CRC32 update at compile time without requiring CRC instructions.
  template<isa Arch> requires(!Arch.has(arm_feature::crc))
  native_nodiscard consteval std::uint32_t crc32(std::uint32_t accumulator, std::uint16_t value) noexcept {
    return detail::crc_update(accumulator,value,16,0xedb88320u);
  }

  /// Update the CRC32 accumulator with exactly 32 input bits.
  template<isa Arch> requires(Arch.has(arm_feature::crc))
  native_nodiscard native_inline native_const native_target("crc")
  constexpr std::uint32_t crc32(std::uint32_t accumulator, std::uint32_t value) noexcept {
    if(__builtin_is_constant_evaluated()) return detail::crc_update(accumulator,value,32,0xedb88320u);
    return __crc32w(accumulator,value);
  }

  /// Evaluate the 32-bit CRC32 update at compile time without requiring CRC instructions.
  template<isa Arch> requires(!Arch.has(arm_feature::crc))
  native_nodiscard consteval std::uint32_t crc32(std::uint32_t accumulator, std::uint32_t value) noexcept {
    return detail::crc_update(accumulator,value,32,0xedb88320u);
  }

  /// Update the CRC32 accumulator with exactly 64 input bits.
  template<isa Arch> requires(Arch.has(arm_feature::crc))
  native_nodiscard native_inline native_const native_target("crc")
  constexpr std::uint32_t crc32(std::uint32_t accumulator, std::uint64_t value) noexcept {
    if(__builtin_is_constant_evaluated()) return detail::crc_update(accumulator,value,64,0xedb88320u);
    return __crc32d(accumulator,value);
  }

  /// Evaluate the 64-bit CRC32 update at compile time without requiring CRC instructions.
  template<isa Arch> requires(!Arch.has(arm_feature::crc))
  native_nodiscard consteval std::uint32_t crc32(std::uint32_t accumulator, std::uint64_t value) noexcept {
    return detail::crc_update(accumulator,value,64,0xedb88320u);
  }

  /// Reject implicit narrowing or conversion to a different operand width.
  template<isa Arch, class A, class B>
  void crc32(A, B) = delete;

  /// Update the CRC32C accumulator with exactly 8 input bits.
  template<isa Arch> requires(Arch.has(arm_feature::crc))
  native_nodiscard native_inline native_const native_target("crc")
  constexpr std::uint32_t crc32c(std::uint32_t accumulator, std::uint8_t value) noexcept {
    if(__builtin_is_constant_evaluated()) return detail::crc_update(accumulator,value,8,0x82f63b78u);
    return __crc32cb(accumulator,value);
  }

  /// Evaluate the 8-bit CRC32C update at compile time without requiring CRC instructions.
  template<isa Arch> requires(!Arch.has(arm_feature::crc))
  native_nodiscard consteval std::uint32_t crc32c(std::uint32_t accumulator, std::uint8_t value) noexcept {
    return detail::crc_update(accumulator,value,8,0x82f63b78u);
  }

  /// Update the CRC32C accumulator with exactly 16 input bits.
  template<isa Arch> requires(Arch.has(arm_feature::crc))
  native_nodiscard native_inline native_const native_target("crc")
  constexpr std::uint32_t crc32c(std::uint32_t accumulator, std::uint16_t value) noexcept {
    if(__builtin_is_constant_evaluated()) return detail::crc_update(accumulator,value,16,0x82f63b78u);
    return __crc32ch(accumulator,value);
  }

  /// Evaluate the 16-bit CRC32C update at compile time without requiring CRC instructions.
  template<isa Arch> requires(!Arch.has(arm_feature::crc))
  native_nodiscard consteval std::uint32_t crc32c(std::uint32_t accumulator, std::uint16_t value) noexcept {
    return detail::crc_update(accumulator,value,16,0x82f63b78u);
  }

  /// Update the CRC32C accumulator with exactly 32 input bits.
  template<isa Arch> requires(Arch.has(arm_feature::crc))
  native_nodiscard native_inline native_const native_target("crc")
  constexpr std::uint32_t crc32c(std::uint32_t accumulator, std::uint32_t value) noexcept {
    if(__builtin_is_constant_evaluated()) return detail::crc_update(accumulator,value,32,0x82f63b78u);
    return __crc32cw(accumulator,value);
  }

  /// Evaluate the 32-bit CRC32C update at compile time without requiring CRC instructions.
  template<isa Arch> requires(!Arch.has(arm_feature::crc))
  native_nodiscard consteval std::uint32_t crc32c(std::uint32_t accumulator, std::uint32_t value) noexcept {
    return detail::crc_update(accumulator,value,32,0x82f63b78u);
  }

  /// Update the CRC32C accumulator with exactly 64 input bits.
  template<isa Arch> requires(Arch.has(arm_feature::crc))
  native_nodiscard native_inline native_const native_target("crc")
  constexpr std::uint32_t crc32c(std::uint32_t accumulator, std::uint64_t value) noexcept {
    if(__builtin_is_constant_evaluated()) return detail::crc_update(accumulator,value,64,0x82f63b78u);
    return __crc32cd(accumulator,value);
  }

  /// Evaluate the 64-bit CRC32C update at compile time without requiring CRC instructions.
  template<isa Arch> requires(!Arch.has(arm_feature::crc))
  native_nodiscard consteval std::uint32_t crc32c(std::uint32_t accumulator, std::uint64_t value) noexcept {
    return detail::crc_update(accumulator,value,64,0x82f63b78u);
  }

  /// Reject implicit narrowing or conversion to a different operand width.
  template<isa Arch, class A, class B>
  void crc32c(A, B) = delete;

/// \}
}
#endif
