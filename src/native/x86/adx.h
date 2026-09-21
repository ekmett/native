// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#include <cstdint>
#if NATIVE_HOST_X86
#include <immintrin.h>

namespace native::detail::x86_adx {
  template<class U>
  constexpr std::uint8_t add(std::uint8_t carry, U a, U b, U * result) noexcept {
    auto first = static_cast<U>(a + b);
    auto overflow = first < a;
    auto sum = static_cast<U>(first + (carry != 0));
    *result = sum;
    return static_cast<std::uint8_t>(overflow || sum < first);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::adx))
  native_nodiscard native_inline native_target("adx")
  std::uint8_t addcarryx(std::uint8_t carry, std::uint32_t a, std::uint32_t b, std::uint32_t * result) noexcept {
    return _addcarryx_u32(carry, a, b, result);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::adx))
  native_nodiscard native_inline native_target("adx")
  std::uint8_t addcarryx(std::uint8_t carry, std::uint64_t a, std::uint64_t b, std::uint64_t * result) noexcept {
    // The intrinsic output uses unsigned long long even on LP64 platforms.
    unsigned long long sum;
    auto next = _addcarryx_u64(carry, a, b, &sum);
    *result = sum;
    return next;
  }

  template<isa<x86> Arch, class... Args>
  void addcarryx(Args...) = delete;
}

namespace native {
  /// \defgroup x86_adx Unsigned addition with carry
  /// Add two 32/64-bit values and one if the input carry is nonzero. Write the
  /// modular sum through a valid output pointer and return carry-out as 0 or 1.
  /// Strong tags require ADX and a compatible caller target; weak tags support
  /// constant evaluation only. Module defaults use the provider's baseline.
  /// Clang 23 may lower its addcarryx intrinsic to ADD/ADC. These value-level
  /// operations do not expose independent ADCX/ADOX flag chains.
  /// \{

  /// Add 32-bit operands with carry, writing the modular result.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::adx))
  native_nodiscard native_inline native_target("adx")
  constexpr std::uint8_t addcarryx(
    std::uint8_t carry, std::uint32_t a, std::uint32_t b, std::uint32_t * result) noexcept {
    if consteval {
      return detail::x86_adx::add(carry, a, b, result);
    } else {
      return detail::x86_adx::addcarryx<Arch>(carry, a, b, result);
    }
  }

  /// Add 64-bit operands with carry, writing the modular result.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::adx))
  native_nodiscard native_inline native_target("adx")
  constexpr std::uint8_t addcarryx(
    std::uint8_t carry, std::uint64_t a, std::uint64_t b, std::uint64_t * result) noexcept {
    if consteval {
      return detail::x86_adx::add(carry, a, b, result);
    } else {
      return detail::x86_adx::addcarryx<Arch>(carry, a, b, result);
    }
  }

  /// Constant 32-bit addition with carry without ADX.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::adx))
  native_nodiscard consteval std::uint8_t addcarryx(
    std::uint8_t carry, std::uint32_t a, std::uint32_t b, std::uint32_t * result) noexcept {
    return detail::x86_adx::add(carry, a, b, result);
  }

  /// Constant 64-bit addition with carry without ADX.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::adx))
  native_nodiscard consteval std::uint8_t addcarryx(
    std::uint8_t carry, std::uint64_t a, std::uint64_t b, std::uint64_t * result) noexcept {
    return detail::x86_adx::add(carry, a, b, result);
  }

  /// \}
}
#endif
