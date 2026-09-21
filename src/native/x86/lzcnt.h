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
/** \defgroup x86_lzcnt LZCNT
 * Scalar bit counts requiring only extended CPUID leaf 0x80000001 ECX bit 5.
 * The supplied ISA must contain x86_feature::lzcnt; admit the feature
 * before entering a matching target scope. No vector OS state is required.
 * \{ */
  /// Count leading zero bits of a 16-bit value; zero returns 16.
  /// Arch must contain LZCNT; the caller must enable and admit that feature.
  template<isa Arch> requires(Arch.has(x86_feature::lzcnt))
  native_nodiscard native_inline native_const __attribute__((target("lzcnt")))
  std::uint16_t lzcnt(std::uint16_t value) noexcept {
    return __builtin_ia32_lzcnt_u16(value);
  }

  /// Count leading zero bits of a 32-bit value; zero returns 32.
  /// Arch must contain LZCNT; the caller must enable and admit that feature.
  template<isa Arch> requires(Arch.has(x86_feature::lzcnt))
  native_nodiscard native_inline native_const __attribute__((target("lzcnt")))
  std::uint32_t lzcnt(std::uint32_t value) noexcept {
    return _lzcnt_u32(value);
  }

  /// Count leading zero bits of a 64-bit value; zero returns 64.
  /// Arch must contain LZCNT; the caller must enable and admit that feature.
  template<isa Arch> requires(Arch.has(x86_feature::lzcnt))
  native_nodiscard native_inline native_const __attribute__((target("lzcnt")))
  std::uint64_t lzcnt(std::uint64_t value) noexcept {
    return _lzcnt_u64(value);
  }
/// \}
}
#endif
