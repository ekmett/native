// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#include <concepts>
#include <cstdint>
#if NATIVE_HOST_X86
#include <immintrin.h>
#endif

#if NATIVE_HOST_X86 || defined(NATIVE_DOXYGEN)
namespace native {
/** \defgroup x86_f16c F16C
 * Binary32 / IEEE binary16 conversions using VEX VCVTPS2PH and VCVTPH2PS.
 * Arch must contain f16c; its compiler prerequisite is AVX. Callers must enable
 * a matching target and admit CPU support and XMM/YMM OS state separately.
 * No AVX2 or AVX512FP16 instructions are required.
 *
 * Half operands and results contain representation bits, compatible with
 * fp16::to_bits()/from_bits(). Packed forms use intrinsic registers and can
 * cross the simd boundary with to_native()/from_native(). This family has no
 * dependency on native.simd or native.numerics.
 *
 * Imm8 accepts every byte, 0..255. Bit 2 selects MXCSR.RC; otherwise bits 1:0
 * select nearest-even (0), down (1), up (2), or toward zero (3). Bits 7:3 are
 * ignored by the instruction: in particular, bit 3 does NOT suppress exceptions.
 * Narrowing ignores FTZ and honors DAZ for binary32 subnormal inputs. Widening
 * ignores DAZ and does not raise a denormal exception for binary16 subnormals.
 * Signs of zero and infinity are preserved. NaNs retain their sign and high
 * payload bits and are quieted; signaling NaNs raise invalid.
 *
 * These operations retain architectural MXCSR status updates and unmasked
 * exceptions even if their result is discarded. They do not modify control
 * bits or masks. They are neither const nor pure, and are not constexpr.
 * \{ */

  /// Convert four binary32 lanes to the low four half words, clearing the high four.
  template<isa Arch, unsigned Imm8, class V>
    requires(Arch.has(x86_feature::f16c) && Imm8 <= 255 && std::same_as<V, __m128>)
  native_nodiscard native_inline native_target("f16c")
  __m128i cvtps_ph(V a) noexcept {
    __m128i result;
    // LLVM's conversion intrinsics can disappear when their output is unused.
    // Volatile asm retains the instruction; the memory clobber orders it with
    // MXCSR loads/stores. Dialect alternatives support both assembler syntaxes.
    __asm__ volatile("vcvtps2ph {%2, %1, %0|%0, %1, %2}"
      : "=x"(result) : "x"(a), "i"(Imm8) : "memory");
    return result;
  }

  /// Convert eight binary32 lanes to eight half words in a 128-bit register.
  template<isa Arch, unsigned Imm8, class V>
    requires(Arch.has(x86_feature::f16c) && Imm8 <= 255 && std::same_as<V, __m256>)
  native_nodiscard native_inline native_target("f16c")
  __m128i cvtps_ph(V a) noexcept {
    __m128i result;
    __asm__ volatile("vcvtps2ph {%2, %1, %0|%0, %1, %2}"
      : "=x"(result) : "x"(a), "i"(Imm8) : "memory");
    return result;
  }

  /// Widen the low four half words; Lanes=4 selects the __m128 result explicitly.
  template<isa Arch, unsigned Lanes, class V>
    requires(Arch.has(x86_feature::f16c) && Lanes == 4 && std::same_as<V, __m128i>)
  native_nodiscard native_inline native_target("f16c")
  __m128 cvtph_ps(V a) noexcept {
    __m128 result;
    __asm__ volatile("vcvtph2ps {%1, %0|%0, %1}"
      : "=x"(result) : "x"(a) : "memory");
    return result;
  }

  /// Widen all eight half words; Lanes=8 selects the __m256 result explicitly.
  template<isa Arch, unsigned Lanes, class V>
    requires(Arch.has(x86_feature::f16c) && Lanes == 8 && std::same_as<V, __m128i>)
  native_nodiscard native_inline native_target("f16c")
  __m256 cvtph_ps(V a) noexcept {
    __m256 result;
    __asm__ volatile("vcvtph2ps {%1, %0|%0, %1}"
      : "=x"(result) : "x"(a) : "memory");
    return result;
  }

  /// Convert one binary32 value to half bits, using zero for unused instruction lanes.
  template<isa Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::f16c) && Imm8 <= 255)
  native_nodiscard native_inline native_target("f16c")
  std::uint16_t cvtss_sh(float a) noexcept {
    return static_cast<std::uint16_t>(_mm_cvtsi128_si32(cvtps_ph<Arch, Imm8>(_mm_set_ss(a))));
  }

  /// Widen one half bit pattern, using zero for unused instruction lanes.
  template<isa Arch> requires(Arch.has(x86_feature::f16c))
  native_nodiscard native_inline native_target("f16c")
  float cvtsh_ss(std::uint16_t a) noexcept {
    return _mm_cvtss_f32(cvtph_ps<Arch, 4>(_mm_cvtsi32_si128(a)));
  }
  /// \}
}
#endif
