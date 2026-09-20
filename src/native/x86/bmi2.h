// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#include <bit>
#include <cstdint>
#if NATIVE_HOST_X86
#include <immintrin.h>
#endif
#if NATIVE_HOST_X86
namespace native {
  /// \defgroup x86_bmi2 BMI2
  /// Integer bit manipulation requiring exactly x86_feature::bmi2.
  /// \{

  /// Zero bits at and above index's low eight bits; an index at least the width leaves value unchanged.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  std::uint32_t bzhi(std::uint32_t value, unsigned index) noexcept {
    return _bzhi_u32(value, index);
  }
  /// BZHI with a 64-bit value and the same low-eight-bit index semantics.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  std::uint64_t bzhi(std::uint64_t value, unsigned index) noexcept {
    return _bzhi_u64(value, index);
  }

  /// Return the low product and write the high product to a valid output pointer.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline __attribute__((target("bmi2")))
  std::uint32_t mulx(std::uint32_t a, std::uint32_t b, std::uint32_t* high) noexcept {
    unsigned int upper;
    auto lower = _mulx_u32(a, b, &upper);
    *high = upper;
    return lower;
  }
  /// Return the low 64 bits and write the high 64 bits of the unsigned 128-bit product.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline __attribute__((target("bmi2")))
  std::uint64_t mulx(std::uint64_t a, std::uint64_t b, std::uint64_t* high) noexcept {
    // uint64_t can be unsigned long on LP64; the intrinsic requires unsigned long long*.
    unsigned long long upper;
    auto lower = _mulx_u64(a, b, &upper);
    *high = upper;
    return lower;
  }

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

  /// Logical left shift using the low five bits of count.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  std::uint32_t shlx(std::uint32_t value, unsigned count) noexcept {
    return value << (count & 31u);
  }
  /// Logical left shift using the low six bits of count.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  std::uint64_t shlx(std::uint64_t value, unsigned count) noexcept {
    return value << (count & 63u);
  }

  /// Logical right shift using the low five bits of count.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  std::uint32_t shrx(std::uint32_t value, unsigned count) noexcept {
    return value >> (count & 31u);
  }
  /// Logical right shift using the low six bits of count.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  std::uint64_t shrx(std::uint64_t value, unsigned count) noexcept {
    return value >> (count & 63u);
  }

  /// Arithmetic right shift using the low five bits of count, propagating the sign bit.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  std::int32_t sarx(std::int32_t value, unsigned count) noexcept {
    return value >> (count & 31u);
  }
  /// Arithmetic right shift using the low six bits of count, propagating the sign bit.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  std::int64_t sarx(std::int64_t value, unsigned count) noexcept {
    return value >> (count & 63u);
  }

  /// Rotate right by an immediate byte; the low five bits select the 32-bit rotation.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::bmi2) && Imm8 <= 255)
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  std::uint32_t rorx(std::uint32_t value) noexcept {
    return std::rotr(value, int(Imm8));
  }
  /// Rotate right by an immediate byte; the low six bits select the 64-bit rotation.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::bmi2) && Imm8 <= 255)
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  std::uint64_t rorx(std::uint64_t value) noexcept {
    return std::rotr(value, int(Imm8));
  }

  /// \}
}
#endif
