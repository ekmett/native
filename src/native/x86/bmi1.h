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

namespace native::detail {
  template<class U>
  constexpr U bmi1_extract(U value, unsigned start, unsigned length) noexcept {
    constexpr unsigned width = sizeof(U) * 8;
    start &= 255u;
    length &= 255u;
    if (start >= width) return 0;
    auto shifted = U(value >> start);
    return length >= width - start ? shifted : U(shifted & ((U{1} << length) - U{1}));
  }
}

namespace native {
  /// \defgroup x86_bmi1 BMI1
  /// Integer bit operations; runtime calls require x86_feature::bmi1 and target "bmi".
  /// Constant evaluation supports every x86 Arch; weak tags select consteval overloads.
  /// \{

  /// ANDN: complement the first operand, then AND with the second.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::bmi1))
  native_nodiscard native_inline native_const __attribute__((target("bmi")))
  constexpr std::uint32_t andn(std::uint32_t first, std::uint32_t second) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return (~first) & second;
    } else {
      return _andn_u32(first, second);
    }
  }
  /// ANDN, with 64-bit operands.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::bmi1))
  native_nodiscard native_inline native_const __attribute__((target("bmi")))
  constexpr std::uint64_t andn(std::uint64_t first, std::uint64_t second) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return (~first) & second;
    } else {
      return _andn_u64(first, second);
    }
  }

  /// BEXTR: control[7:0] is start; control[15:8] is length; higher bits are ignored.
  /// Extraction stops at the operand width; an out-of-range start or zero length yields zero.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::bmi1))
  native_nodiscard native_inline native_const __attribute__((target("bmi")))
  constexpr std::uint32_t bextr(std::uint32_t value, std::uint32_t control) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return detail::bmi1_extract(value, control & 255u, (control >> 8) & 255u);
    } else {
      return _bextr2_u32(value, control);
    }
  }
  /// BEXTR, with a 64-bit value and the same packed control format.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::bmi1))
  native_nodiscard native_inline native_const __attribute__((target("bmi")))
  constexpr std::uint64_t bextr(std::uint64_t value, std::uint32_t control) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return detail::bmi1_extract(value, control & 255u, (control >> 8) & 255u);
    } else {
      return _bextr2_u64(value, control);
    }
  }
  /// BEXTR with separate controls; only the low eight bits of each are used.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::bmi1))
  native_nodiscard native_inline native_const __attribute__((target("bmi")))
  constexpr std::uint32_t bextr(std::uint32_t value, unsigned start, unsigned length) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return detail::bmi1_extract(value, start, length);
    } else {
      return _bextr_u32(value, start, length);
    }
  }
  /// BEXTR with a 64-bit value and separate low-eight-bit controls.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::bmi1))
  native_nodiscard native_inline native_const __attribute__((target("bmi")))
  constexpr std::uint64_t bextr(std::uint64_t value, unsigned start, unsigned length) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return detail::bmi1_extract(value, start, length);
    } else {
      return _bextr_u64(value, start, length);
    }
  }

  /// BLSI: retain only the lowest set bit; zero remains zero.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::bmi1))
  native_nodiscard native_inline native_const __attribute__((target("bmi")))
  constexpr std::uint32_t blsi(std::uint32_t value) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return value & (std::uint32_t{0} - value);
    } else {
      return _blsi_u32(value);
    }
  }
  /// BLSI, with a 64-bit operand.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::bmi1))
  native_nodiscard native_inline native_const __attribute__((target("bmi")))
  constexpr std::uint64_t blsi(std::uint64_t value) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return value & (std::uint64_t{0} - value);
    } else {
      return _blsi_u64(value);
    }
  }

  /// BLSMSK: set every bit through the lowest set bit; zero produces all ones.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::bmi1))
  native_nodiscard native_inline native_const __attribute__((target("bmi")))
  constexpr std::uint32_t blsmsk(std::uint32_t value) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return value ^ (value - std::uint32_t{1});
    } else {
      return _blsmsk_u32(value);
    }
  }
  /// BLSMSK, with a 64-bit operand.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::bmi1))
  native_nodiscard native_inline native_const __attribute__((target("bmi")))
  constexpr std::uint64_t blsmsk(std::uint64_t value) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return value ^ (value - std::uint64_t{1});
    } else {
      return _blsmsk_u64(value);
    }
  }

  /// BLSR: clear the lowest set bit; zero remains zero.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::bmi1))
  native_nodiscard native_inline native_const __attribute__((target("bmi")))
  constexpr std::uint32_t blsr(std::uint32_t value) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return value & (value - std::uint32_t{1});
    } else {
      return _blsr_u32(value);
    }
  }
  /// BLSR, with a 64-bit operand.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::bmi1))
  native_nodiscard native_inline native_const __attribute__((target("bmi")))
  constexpr std::uint64_t blsr(std::uint64_t value) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return value & (value - std::uint64_t{1});
    } else {
      return _blsr_u64(value);
    }
  }

  /// TZCNT: count low zero bits; zero returns the operand width, including 16-bit operands.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::bmi1))
  native_nodiscard native_inline native_const __attribute__((target("bmi")))
  constexpr std::uint16_t tzcnt(std::uint16_t value) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return static_cast<std::uint16_t>(std::countr_zero(value));
    } else {
      return _tzcnt_u16(value);
    }
  }
  /// TZCNT, with a 32-bit operand and result; zero returns 32.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::bmi1))
  native_nodiscard native_inline native_const __attribute__((target("bmi")))
  constexpr std::uint32_t tzcnt(std::uint32_t value) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return static_cast<std::uint32_t>(std::countr_zero(value));
    } else {
      return _tzcnt_u32(value);
    }
  }
  /// TZCNT, with a 64-bit operand and result; zero returns 64.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::bmi1))
  native_nodiscard native_inline native_const __attribute__((target("bmi")))
  constexpr std::uint64_t tzcnt(std::uint64_t value) noexcept {
    if (__builtin_is_constant_evaluated()) {
      return static_cast<std::uint64_t>(std::countr_zero(value));
    } else {
      return _tzcnt_u64(value);
    }
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::bmi1))
  native_nodiscard
  consteval std::uint32_t andn(std::uint32_t first, std::uint32_t second) noexcept {
    return andn<isa<x86>{x86_feature::bmi1}>(first, second);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::bmi1))
  native_nodiscard
  consteval std::uint64_t andn(std::uint64_t first, std::uint64_t second) noexcept {
    return andn<isa<x86>{x86_feature::bmi1}>(first, second);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::bmi1))
  native_nodiscard
  consteval std::uint32_t bextr(std::uint32_t value, std::uint32_t control) noexcept {
    return bextr<isa<x86>{x86_feature::bmi1}>(value, control);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::bmi1))
  native_nodiscard
  consteval std::uint64_t bextr(std::uint64_t value, std::uint32_t control) noexcept {
    return bextr<isa<x86>{x86_feature::bmi1}>(value, control);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::bmi1))
  native_nodiscard
  consteval std::uint32_t bextr(std::uint32_t value, unsigned start, unsigned length) noexcept {
    return bextr<isa<x86>{x86_feature::bmi1}>(value, start, length);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::bmi1))
  native_nodiscard
  consteval std::uint64_t bextr(std::uint64_t value, unsigned start, unsigned length) noexcept {
    return bextr<isa<x86>{x86_feature::bmi1}>(value, start, length);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::bmi1))
  native_nodiscard
  consteval std::uint32_t blsi(std::uint32_t value) noexcept {
    return blsi<isa<x86>{x86_feature::bmi1}>(value);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::bmi1))
  native_nodiscard
  consteval std::uint64_t blsi(std::uint64_t value) noexcept {
    return blsi<isa<x86>{x86_feature::bmi1}>(value);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::bmi1))
  native_nodiscard
  consteval std::uint32_t blsmsk(std::uint32_t value) noexcept {
    return blsmsk<isa<x86>{x86_feature::bmi1}>(value);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::bmi1))
  native_nodiscard
  consteval std::uint64_t blsmsk(std::uint64_t value) noexcept {
    return blsmsk<isa<x86>{x86_feature::bmi1}>(value);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::bmi1))
  native_nodiscard
  consteval std::uint32_t blsr(std::uint32_t value) noexcept {
    return blsr<isa<x86>{x86_feature::bmi1}>(value);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::bmi1))
  native_nodiscard
  consteval std::uint64_t blsr(std::uint64_t value) noexcept {
    return blsr<isa<x86>{x86_feature::bmi1}>(value);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::bmi1))
  native_nodiscard
  consteval std::uint16_t tzcnt(std::uint16_t value) noexcept {
    return tzcnt<isa<x86>{x86_feature::bmi1}>(value);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::bmi1))
  native_nodiscard
  consteval std::uint32_t tzcnt(std::uint32_t value) noexcept {
    return tzcnt<isa<x86>{x86_feature::bmi1}>(value);
  }

  /// Evaluate the same operation at compile time when Arch lacks the feature.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::bmi1))
  native_nodiscard
  consteval std::uint64_t tzcnt(std::uint64_t value) noexcept {
    return tzcnt<isa<x86>{x86_feature::bmi1}>(value);
  }

  /// \}
}
#endif
