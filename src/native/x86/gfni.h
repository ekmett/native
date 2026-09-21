// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#if NATIVE_HOST_X86
#include <immintrin.h>
#endif

#if NATIVE_HOST_X86 || defined(NATIVE_DOXYGEN)
namespace native {
/** \defgroup x86_gfni GFNI
 * Byte arithmetic modulo x^8 + x^4 + x^3 + x + 1 and binary affine maps.
 * Arch records instruction requirements; callers must separately enable and
 * admit a matching target. The raw register width selects the overload and
 * minimum target requirements.
 *
 * Each matrix operand contains one 8 by 8 binary matrix per 64-bit lane.
 * Output bit i uses matrix byte 7-i within that lane, with input bit j
 * multiplying bit j of that byte. Imm8 is XORed into every result byte.
 * Inverse-affine first takes the field inverse of each input byte (zero maps
 * to zero), then applies the matrix and Imm8; it does not invert the matrix.
 *
 * Mask bit i selects byte i. Merge forms retain src in inactive bytes;
 * zero forms clear them. LLVM's byte-mask intrinsics require AVX512BW;
 * narrower masked forms also require AVX512VL.
 * All operations depend only on their register arguments and have no side effects.
 * \{ */

  // 128-bit registers.
  /// Multiply corresponding bytes in GF(2^8).
  template<isa Arch> requires(Arch.has(x86_feature::gfni))
  native_nodiscard native_inline native_const native_target("gfni")
  __m128i gf2p8mulb(__m128i a, __m128i b) noexcept {
    return _mm_gf2p8mul_epi8(a, b);
  }

  /// Apply the binary matrix in each 64-bit lane and XOR Imm8.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("gfni")
  __m128i gf2p8affineqb(__m128i a, __m128i matrix) noexcept {
    return _mm_gf2p8affine_epi64_epi8(a, matrix, Imm8);
  }

  /// Invert each field byte, apply its lane's binary matrix, and XOR Imm8.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("gfni")
  __m128i gf2p8affineinvqb(__m128i a, __m128i matrix) noexcept {
    return _mm_gf2p8affineinv_epi64_epi8(a, matrix, Imm8);
  }

  /// Merge inactive bytes from src after gf2p8mulb.
  template<isa Arch> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512bw,avx512vl,gfni")
  __m128i gf2p8mulb_mask(__m128i src, __mmask16 k, __m128i a, __m128i b) noexcept {
    return _mm_mask_gf2p8mul_epi8(src, k, a, b);
  }

  /// Zero inactive bytes after gf2p8mulb.
  template<isa Arch> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512bw,avx512vl,gfni")
  __m128i gf2p8mulb_maskz(__mmask16 k, __m128i a, __m128i b) noexcept {
    return _mm_maskz_gf2p8mul_epi8(k, a, b);
  }

  /// Merge inactive bytes from src after gf2p8affineqb.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512bw,avx512vl,gfni")
  __m128i gf2p8affineqb_mask(__m128i src, __mmask16 k, __m128i a, __m128i matrix) noexcept {
    return _mm_mask_gf2p8affine_epi64_epi8(src, k, a, matrix, Imm8);
  }

  /// Zero inactive bytes after gf2p8affineqb.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512bw,avx512vl,gfni")
  __m128i gf2p8affineqb_maskz(__mmask16 k, __m128i a, __m128i matrix) noexcept {
    return _mm_maskz_gf2p8affine_epi64_epi8(k, a, matrix, Imm8);
  }

  /// Merge inactive bytes from src after gf2p8affineinvqb.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512bw,avx512vl,gfni")
  __m128i gf2p8affineinvqb_mask(__m128i src, __mmask16 k, __m128i a, __m128i matrix) noexcept {
    return _mm_mask_gf2p8affineinv_epi64_epi8(src, k, a, matrix, Imm8);
  }

  /// Zero inactive bytes after gf2p8affineinvqb.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512bw,avx512vl,gfni")
  __m128i gf2p8affineinvqb_maskz(__mmask16 k, __m128i a, __m128i matrix) noexcept {
    return _mm_maskz_gf2p8affineinv_epi64_epi8(k, a, matrix, Imm8);
  }


  // 256-bit registers.
  /// Multiply corresponding bytes in GF(2^8).
  template<isa Arch> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx))
  native_nodiscard native_inline native_const native_target("avx,gfni")
  __m256i gf2p8mulb(__m256i a, __m256i b) noexcept {
    return _mm256_gf2p8mul_epi8(a, b);
  }

  /// Apply the binary matrix in each 64-bit lane and XOR Imm8.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx,gfni")
  __m256i gf2p8affineqb(__m256i a, __m256i matrix) noexcept {
    return _mm256_gf2p8affine_epi64_epi8(a, matrix, Imm8);
  }

  /// Invert each field byte, apply its lane's binary matrix, and XOR Imm8.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx,gfni")
  __m256i gf2p8affineinvqb(__m256i a, __m256i matrix) noexcept {
    return _mm256_gf2p8affineinv_epi64_epi8(a, matrix, Imm8);
  }

  /// Merge inactive bytes from src after gf2p8mulb.
  template<isa Arch> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512bw,avx512vl,gfni")
  __m256i gf2p8mulb_mask(__m256i src, __mmask32 k, __m256i a, __m256i b) noexcept {
    return _mm256_mask_gf2p8mul_epi8(src, k, a, b);
  }

  /// Zero inactive bytes after gf2p8mulb.
  template<isa Arch> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512bw,avx512vl,gfni")
  __m256i gf2p8mulb_maskz(__mmask32 k, __m256i a, __m256i b) noexcept {
    return _mm256_maskz_gf2p8mul_epi8(k, a, b);
  }

  /// Merge inactive bytes from src after gf2p8affineqb.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512bw,avx512vl,gfni")
  __m256i gf2p8affineqb_mask(__m256i src, __mmask32 k, __m256i a, __m256i matrix) noexcept {
    return _mm256_mask_gf2p8affine_epi64_epi8(src, k, a, matrix, Imm8);
  }

  /// Zero inactive bytes after gf2p8affineqb.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512bw,avx512vl,gfni")
  __m256i gf2p8affineqb_maskz(__mmask32 k, __m256i a, __m256i matrix) noexcept {
    return _mm256_maskz_gf2p8affine_epi64_epi8(k, a, matrix, Imm8);
  }

  /// Merge inactive bytes from src after gf2p8affineinvqb.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512bw,avx512vl,gfni")
  __m256i gf2p8affineinvqb_mask(__m256i src, __mmask32 k, __m256i a, __m256i matrix) noexcept {
    return _mm256_mask_gf2p8affineinv_epi64_epi8(src, k, a, matrix, Imm8);
  }

  /// Zero inactive bytes after gf2p8affineinvqb.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512bw,avx512vl,gfni")
  __m256i gf2p8affineinvqb_maskz(__mmask32 k, __m256i a, __m256i matrix) noexcept {
    return _mm256_maskz_gf2p8affineinv_epi64_epi8(k, a, matrix, Imm8);
  }


  // 512-bit registers.
  /// Multiply corresponding bytes in GF(2^8).
  template<isa Arch> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f))
  native_nodiscard native_inline native_const native_target("avx512f,gfni")
  __m512i gf2p8mulb(__m512i a, __m512i b) noexcept {
    return _mm512_gf2p8mul_epi8(a, b);
  }

  /// Apply the binary matrix in each 64-bit lane and XOR Imm8.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,gfni")
  __m512i gf2p8affineqb(__m512i a, __m512i matrix) noexcept {
    return _mm512_gf2p8affine_epi64_epi8(a, matrix, Imm8);
  }

  /// Invert each field byte, apply its lane's binary matrix, and XOR Imm8.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,gfni")
  __m512i gf2p8affineinvqb(__m512i a, __m512i matrix) noexcept {
    return _mm512_gf2p8affineinv_epi64_epi8(a, matrix, Imm8);
  }

  /// Merge inactive bytes from src after gf2p8mulb.
  template<isa Arch> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw))
  native_nodiscard native_inline native_const native_target("avx512bw,gfni")
  __m512i gf2p8mulb_mask(__m512i src, __mmask64 k, __m512i a, __m512i b) noexcept {
    return _mm512_mask_gf2p8mul_epi8(src, k, a, b);
  }

  /// Zero inactive bytes after gf2p8mulb.
  template<isa Arch> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw))
  native_nodiscard native_inline native_const native_target("avx512bw,gfni")
  __m512i gf2p8mulb_maskz(__mmask64 k, __m512i a, __m512i b) noexcept {
    return _mm512_maskz_gf2p8mul_epi8(k, a, b);
  }

  /// Merge inactive bytes from src after gf2p8affineqb.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512bw,gfni")
  __m512i gf2p8affineqb_mask(__m512i src, __mmask64 k, __m512i a, __m512i matrix) noexcept {
    return _mm512_mask_gf2p8affine_epi64_epi8(src, k, a, matrix, Imm8);
  }

  /// Zero inactive bytes after gf2p8affineqb.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512bw,gfni")
  __m512i gf2p8affineqb_maskz(__mmask64 k, __m512i a, __m512i matrix) noexcept {
    return _mm512_maskz_gf2p8affine_epi64_epi8(k, a, matrix, Imm8);
  }

  /// Merge inactive bytes from src after gf2p8affineinvqb.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512bw,gfni")
  __m512i gf2p8affineinvqb_mask(__m512i src, __mmask64 k, __m512i a, __m512i matrix) noexcept {
    return _mm512_mask_gf2p8affineinv_epi64_epi8(src, k, a, matrix, Imm8);
  }

  /// Zero inactive bytes after gf2p8affineinvqb.
  template<isa Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512bw,gfni")
  __m512i gf2p8affineinvqb_maskz(__mmask64 k, __m512i a, __m512i matrix) noexcept {
    return _mm512_maskz_gf2p8affineinv_epi64_epi8(k, a, matrix, Imm8);
  }

/// \}
}
#endif
