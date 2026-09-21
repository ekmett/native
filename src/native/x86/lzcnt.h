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
/** \defgroup x86_lzcnt LZCNT
 * Scalar bit counts requiring only extended CPUID leaf 0x80000001 ECX bit 5.
 * Runtime calls require x86_feature::lzcnt in the supplied ISA.
 * Admit that feature before entering a matching target scope. No vector OS state is required.
 * Constant evaluation supports every x86 Arch; weak tags select consteval overloads.
 * \{ */
  /// Count leading zero bits of a 16-bit value; zero returns 16.
  /// Arch must contain LZCNT; runtime callers must enable and admit that feature.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::lzcnt))
  native_nodiscard native_inline native_const __attribute__((target("lzcnt")))
  constexpr std::uint16_t lzcnt(std::uint16_t value) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return static_cast<std::uint16_t>(std::countl_zero(value));
    } else {
      return __builtin_ia32_lzcnt_u16(value);
    }
  }

  /// Count leading zero bits of a 32-bit value; zero returns 32.
  /// Arch must contain LZCNT; runtime callers must enable and admit that feature.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::lzcnt))
  native_nodiscard native_inline native_const __attribute__((target("lzcnt")))
  constexpr std::uint32_t lzcnt(std::uint32_t value) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return static_cast<std::uint32_t>(std::countl_zero(value));
    } else {
      return _lzcnt_u32(value);
    }
  }

  /// Count leading zero bits of a 64-bit value; zero returns 64.
  /// Arch must contain LZCNT; runtime callers must enable and admit that feature.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::lzcnt))
  native_nodiscard native_inline native_const __attribute__((target("lzcnt")))
  constexpr std::uint64_t lzcnt(std::uint64_t value) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return static_cast<std::uint64_t>(std::countl_zero(value));
    } else {
      return _lzcnt_u64(value);
    }
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::lzcnt))
  native_nodiscard
  consteval std::uint16_t lzcnt(std::uint16_t value) noexcept {
    return lzcnt<isa<x86>{x86_feature::lzcnt}>(value);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::lzcnt))
  native_nodiscard
  consteval std::uint32_t lzcnt(std::uint32_t value) noexcept {
    return lzcnt<isa<x86>{x86_feature::lzcnt}>(value);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::lzcnt))
  native_nodiscard
  consteval std::uint64_t lzcnt(std::uint64_t value) noexcept {
    return lzcnt<isa<x86>{x86_feature::lzcnt}>(value);
  }

/// \}
}
#endif
