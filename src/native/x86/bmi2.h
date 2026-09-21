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
namespace native::detail {
  template<class U>
  constexpr U bmi2_zero_high(U value, unsigned index) noexcept {
    index &= 255u;
    return index >= sizeof(U) * 8 ? value : U(value & ((U{1} << index) - U{1}));
  }

  template<class U>
  constexpr U bmi2_deposit(U value, U mask) noexcept {
    U result = 0;
    for (U bit = 1; mask; bit <<= 1, mask &= mask - U{1})
      if (value & bit) result |= mask & (U{0} - mask);
    return result;
  }

  template<class U>
  constexpr U bmi2_extract(U value, U mask) noexcept {
    U result = 0;
    for (U bit = 1; mask; bit <<= 1, mask &= mask - U{1})
      if (value & (mask & (U{0} - mask))) result |= bit;
    return result;
  }

  constexpr std::uint32_t bmi2_multiply(std::uint32_t a, std::uint32_t b,
                                      std::uint32_t* high) noexcept {
    auto product = std::uint64_t{a} * b;
    *high = static_cast<std::uint32_t>(product >> 32);
    return static_cast<std::uint32_t>(product);
  }

  constexpr std::uint64_t bmi2_multiply(std::uint64_t a, std::uint64_t b,
                                      std::uint64_t* high) noexcept {
    // Base-2^32 limbs avoid a nonstandard 128-bit integer requirement.
    auto a_low = std::uint64_t{static_cast<std::uint32_t>(a)};
    auto b_low = std::uint64_t{static_cast<std::uint32_t>(b)};
    auto a_high = a >> 32;
    auto b_high = b >> 32;
    auto low_product = a_low * b_low;
    auto middle = a_high * b_low + (low_product >> 32);
    auto upper = middle >> 32;
    middle = std::uint64_t{static_cast<std::uint32_t>(middle)} + a_low * b_high;
    *high = a_high * b_high + upper + (middle >> 32);
    return (middle << 32) | static_cast<std::uint32_t>(low_product);
  }
}

namespace native {
  /// \defgroup x86_bmi2 BMI2
  /// Integer bit manipulation; runtime calls require exactly x86_feature::bmi2.
  /// Constant evaluation supports every Arch; weak tags select consteval overloads.
  /// \{

  /// Zero bits at and above index's low eight bits; an index at least the width leaves value unchanged.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  constexpr std::uint32_t bzhi(std::uint32_t value, unsigned index) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return detail::bmi2_zero_high(value, index);
    } else {
      return _bzhi_u32(value, index);
    }
  }
  /// BZHI with a 64-bit value and the same low-eight-bit index semantics.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  constexpr std::uint64_t bzhi(std::uint64_t value, unsigned index) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return detail::bmi2_zero_high(value, index);
    } else {
      return _bzhi_u64(value, index);
    }
  }

  /// Return the low product and write the high product to a valid output pointer.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline __attribute__((target("bmi2")))
  constexpr std::uint32_t mulx(std::uint32_t a, std::uint32_t b, std::uint32_t* high) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return detail::bmi2_multiply(a, b, high);
    } else {
      unsigned int upper;
      auto lower = _mulx_u32(a, b, &upper);
      *high = upper;
      return lower;
    }
  }
  /// Return the low 64 bits and write the high 64 bits of the unsigned 128-bit product.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline __attribute__((target("bmi2")))
  constexpr std::uint64_t mulx(std::uint64_t a, std::uint64_t b, std::uint64_t* high) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return detail::bmi2_multiply(a, b, high);
    } else {
      // uint64_t can be unsigned long on LP64; the intrinsic requires unsigned long long*.
      unsigned long long upper;
      auto lower = _mulx_u64(a, b, &upper);
      *high = upper;
      return lower;
    }
  }

  /// Parallel bit deposit (PDEP): place the low popcount(mask) bits into mask's set positions, in order.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  constexpr std::uint32_t pdep(std::uint32_t value, std::uint32_t mask) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return detail::bmi2_deposit(value, mask);
    } else {
      return _pdep_u32(value, mask);
    }
  }
  /// Parallel bit deposit (PDEP), with 64-bit operands.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  constexpr std::uint64_t pdep(std::uint64_t value, std::uint64_t mask) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return detail::bmi2_deposit(value, mask);
    } else {
      return _pdep_u64(value, mask);
    }
  }

  /// Parallel bit extract (PEXT): gather mask's selected bits into the low bits, in order.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  constexpr std::uint32_t pext(std::uint32_t value, std::uint32_t mask) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return detail::bmi2_extract(value, mask);
    } else {
      return _pext_u32(value, mask);
    }
  }
  /// Parallel bit extract (PEXT), with 64-bit operands.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  constexpr std::uint64_t pext(std::uint64_t value, std::uint64_t mask) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return detail::bmi2_extract(value, mask);
    } else {
      return _pext_u64(value, mask);
    }
  }

  /// Logical left shift using the low five bits of count.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  constexpr std::uint32_t shlx(std::uint32_t value, unsigned count) noexcept {
    return value << (count & 31u);
  }
  /// Logical left shift using the low six bits of count.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  constexpr std::uint64_t shlx(std::uint64_t value, unsigned count) noexcept {
    return value << (count & 63u);
  }

  /// Logical right shift using the low five bits of count.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  constexpr std::uint32_t shrx(std::uint32_t value, unsigned count) noexcept {
    return value >> (count & 31u);
  }
  /// Logical right shift using the low six bits of count.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  constexpr std::uint64_t shrx(std::uint64_t value, unsigned count) noexcept {
    return value >> (count & 63u);
  }

  /// Arithmetic right shift using the low five bits of count, propagating the sign bit.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  constexpr std::int32_t sarx(std::int32_t value, unsigned count) noexcept {
    return value >> (count & 31u);
  }
  /// Arithmetic right shift using the low six bits of count, propagating the sign bit.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  constexpr std::int64_t sarx(std::int64_t value, unsigned count) noexcept {
    return value >> (count & 63u);
  }

  /// Rotate right by an immediate byte; the low five bits select the 32-bit rotation.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::bmi2) && Imm8 <= 255)
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  constexpr std::uint32_t rorx(std::uint32_t value) noexcept {
    return std::rotr(value, int(Imm8));
  }
  /// Rotate right by an immediate byte; the low six bits select the 64-bit rotation.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::bmi2) && Imm8 <= 255)
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  constexpr std::uint64_t rorx(std::uint64_t value) noexcept {
    return std::rotr(value, int(Imm8));
  }


  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa Arch> requires(!Arch.has(x86_feature::bmi2))
  native_nodiscard
  consteval std::uint32_t bzhi(std::uint32_t value, unsigned index) noexcept {
    return bzhi<isa{x86_feature::bmi2}>(value, index);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa Arch> requires(!Arch.has(x86_feature::bmi2))
  native_nodiscard
  consteval std::uint64_t bzhi(std::uint64_t value, unsigned index) noexcept {
    return bzhi<isa{x86_feature::bmi2}>(value, index);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa Arch> requires(!Arch.has(x86_feature::bmi2))
  native_nodiscard
  consteval std::uint32_t mulx(std::uint32_t a, std::uint32_t b, std::uint32_t* high) noexcept {
    return mulx<isa{x86_feature::bmi2}>(a, b, high);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa Arch> requires(!Arch.has(x86_feature::bmi2))
  native_nodiscard
  consteval std::uint64_t mulx(std::uint64_t a, std::uint64_t b, std::uint64_t* high) noexcept {
    return mulx<isa{x86_feature::bmi2}>(a, b, high);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa Arch> requires(!Arch.has(x86_feature::bmi2))
  native_nodiscard
  consteval std::uint32_t pdep(std::uint32_t value, std::uint32_t mask) noexcept {
    return pdep<isa{x86_feature::bmi2}>(value, mask);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa Arch> requires(!Arch.has(x86_feature::bmi2))
  native_nodiscard
  consteval std::uint64_t pdep(std::uint64_t value, std::uint64_t mask) noexcept {
    return pdep<isa{x86_feature::bmi2}>(value, mask);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa Arch> requires(!Arch.has(x86_feature::bmi2))
  native_nodiscard
  consteval std::uint32_t pext(std::uint32_t value, std::uint32_t mask) noexcept {
    return pext<isa{x86_feature::bmi2}>(value, mask);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa Arch> requires(!Arch.has(x86_feature::bmi2))
  native_nodiscard
  consteval std::uint64_t pext(std::uint64_t value, std::uint64_t mask) noexcept {
    return pext<isa{x86_feature::bmi2}>(value, mask);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa Arch> requires(!Arch.has(x86_feature::bmi2))
  native_nodiscard
  consteval std::uint32_t shlx(std::uint32_t value, unsigned count) noexcept {
    return shlx<isa{x86_feature::bmi2}>(value, count);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa Arch> requires(!Arch.has(x86_feature::bmi2))
  native_nodiscard
  consteval std::uint64_t shlx(std::uint64_t value, unsigned count) noexcept {
    return shlx<isa{x86_feature::bmi2}>(value, count);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa Arch> requires(!Arch.has(x86_feature::bmi2))
  native_nodiscard
  consteval std::uint32_t shrx(std::uint32_t value, unsigned count) noexcept {
    return shrx<isa{x86_feature::bmi2}>(value, count);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa Arch> requires(!Arch.has(x86_feature::bmi2))
  native_nodiscard
  consteval std::uint64_t shrx(std::uint64_t value, unsigned count) noexcept {
    return shrx<isa{x86_feature::bmi2}>(value, count);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa Arch> requires(!Arch.has(x86_feature::bmi2))
  native_nodiscard
  consteval std::int32_t sarx(std::int32_t value, unsigned count) noexcept {
    return sarx<isa{x86_feature::bmi2}>(value, count);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa Arch> requires(!Arch.has(x86_feature::bmi2))
  native_nodiscard
  consteval std::int64_t sarx(std::int64_t value, unsigned count) noexcept {
    return sarx<isa{x86_feature::bmi2}>(value, count);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa Arch, unsigned Imm8> requires(!Arch.has(x86_feature::bmi2) && Imm8 <= 255)
  native_nodiscard
  consteval std::uint32_t rorx(std::uint32_t value) noexcept {
    return rorx<isa{x86_feature::bmi2}, Imm8>(value);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa Arch, unsigned Imm8> requires(!Arch.has(x86_feature::bmi2) && Imm8 <= 255)
  native_nodiscard
  consteval std::uint64_t rorx(std::uint64_t value) noexcept {
    return rorx<isa{x86_feature::bmi2}, Imm8>(value);
  }

  /// \}
}
#endif
