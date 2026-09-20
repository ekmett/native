// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#include <cstdint>
#if NATIVE_HOST_X86
#include <immintrin.h>
#endif
#if NATIVE_HOST_X86
namespace native {
  /// Parallel bit deposit (PDEP): place the low popcount(mask) bits into mask's set positions, in order.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  std::uint32_t pdep(std::uint32_t value, std::uint32_t mask) noexcept {
    return _pdep_u32(value, mask);
  }
  /// Parallel bit deposit (PDEP), with 64-bit operands.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  std::uint64_t pdep(std::uint64_t value, std::uint64_t mask) noexcept {
    return _pdep_u64(value, mask);
  }

  /// Parallel bit extract (PEXT): gather mask's selected bits into the low bits, in order.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  std::uint32_t pext(std::uint32_t value, std::uint32_t mask) noexcept {
    return _pext_u32(value, mask);
  }
  /// Parallel bit extract (PEXT), with 64-bit operands.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  std::uint64_t pext(std::uint64_t value, std::uint64_t mask) noexcept {
    return _pext_u64(value, mask);
  }
}
#endif
