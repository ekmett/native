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
/** \defgroup x86_popcnt POPCNT
 * Scalar bit counts requiring only CPUID leaf 1 ECX bit 23.
 * The supplied ISA must contain x86_feature::popcnt; admit the feature
 * before entering a matching target scope. No vector OS state is required.
 * \{ */
  /// Count the set bits of a 16-bit value; zero returns zero.
  /// Arch must contain POPCNT; the caller must enable and admit that feature.
  template<isa Arch> requires(Arch.has(x86_feature::popcnt))
  native_nodiscard native_inline native_const __attribute__((target("popcnt")))
  std::uint16_t popcnt(std::uint16_t value) noexcept {
    return static_cast<std::uint16_t>(_mm_popcnt_u32(value));
  }

  /// Count the set bits of a 32-bit value; zero returns zero.
  /// Arch must contain POPCNT; the caller must enable and admit that feature.
  template<isa Arch> requires(Arch.has(x86_feature::popcnt))
  native_nodiscard native_inline native_const __attribute__((target("popcnt")))
  std::uint32_t popcnt(std::uint32_t value) noexcept {
    return static_cast<std::uint32_t>(_mm_popcnt_u32(value));
  }

  /// Count the set bits of a 64-bit value; zero returns zero.
  /// Arch must contain POPCNT; the caller must enable and admit that feature.
  template<isa Arch> requires(Arch.has(x86_feature::popcnt))
  native_nodiscard native_inline native_const __attribute__((target("popcnt")))
  std::uint64_t popcnt(std::uint64_t value) noexcept {
    return static_cast<std::uint64_t>(_mm_popcnt_u64(value));
  }
/// \}
}
#endif
