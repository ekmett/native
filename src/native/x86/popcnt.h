// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#include <bit>
#include <cstdint>
#if NATIVE_HOST_X86
#include <immintrin.h>

namespace native {
/** \defgroup x86_popcnt POPCNT
 * Scalar bit counts requiring only CPUID leaf 1 ECX bit 23.
 * Runtime calls require x86_feature::popcnt in the supplied ISA.
 * Admit that feature before entering a matching target scope. No vector OS state is required.
 * Constant evaluation supports every Arch; weak tags select consteval overloads.
 * \{ */
  /// Count the set bits of a 16-bit value; zero returns zero.
  /// Arch must contain POPCNT; runtime callers must enable and admit that feature.
  template<isa Arch> requires(Arch.has(x86_feature::popcnt))
  native_nodiscard native_inline native_const __attribute__((target("popcnt")))
  constexpr std::uint16_t popcnt(std::uint16_t value) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return static_cast<std::uint16_t>(std::popcount(value));
    } else {
      return static_cast<std::uint16_t>(_mm_popcnt_u32(value));
    }
  }

  /// Count the set bits of a 32-bit value; zero returns zero.
  /// Arch must contain POPCNT; runtime callers must enable and admit that feature.
  template<isa Arch> requires(Arch.has(x86_feature::popcnt))
  native_nodiscard native_inline native_const __attribute__((target("popcnt")))
  constexpr std::uint32_t popcnt(std::uint32_t value) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return static_cast<std::uint32_t>(std::popcount(value));
    } else {
      return static_cast<std::uint32_t>(_mm_popcnt_u32(value));
    }
  }

  /// Count the set bits of a 64-bit value; zero returns zero.
  /// Arch must contain POPCNT; runtime callers must enable and admit that feature.
  template<isa Arch> requires(Arch.has(x86_feature::popcnt))
  native_nodiscard native_inline native_const __attribute__((target("popcnt")))
  constexpr std::uint64_t popcnt(std::uint64_t value) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return static_cast<std::uint64_t>(std::popcount(value));
    } else {
      return static_cast<std::uint64_t>(_mm_popcnt_u64(value));
    }
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa Arch> requires(!Arch.has(x86_feature::popcnt))
  native_nodiscard
  consteval std::uint16_t popcnt(std::uint16_t value) noexcept {
    return popcnt<isa{x86_feature::popcnt}>(value);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa Arch> requires(!Arch.has(x86_feature::popcnt))
  native_nodiscard
  consteval std::uint32_t popcnt(std::uint32_t value) noexcept {
    return popcnt<isa{x86_feature::popcnt}>(value);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa Arch> requires(!Arch.has(x86_feature::popcnt))
  native_nodiscard
  consteval std::uint64_t popcnt(std::uint64_t value) noexcept {
    return popcnt<isa{x86_feature::popcnt}>(value);
  }

/// \}
}
#endif
