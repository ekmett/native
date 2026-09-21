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
/** \defgroup x86_vnni VNNI integer dot products
 * Each 32-bit accumulator lane receives the sum of four byte products or two
 * word products from the corresponding adjacent input group. Non-saturating
 * forms wrap modulo 2^32. Saturating forms clamp the complete sum, including
 * the accumulator, without first wrapping or clamping the product sum.
 * All saturation is signed except dpbuuds and dpwuuds, which use unsigned
 * accumulators and unsigned saturation. These register operations do not
 * change integer flags or floating-point status.
 *
 * Unmasked 128/256-bit core operations use AVX-VNNI when Arch contains
 * avxvnni; otherwise they require AVX512F, AVX512VL and AVX512VNNI.
 * 512-bit core operations require AVX512F and AVX512VNNI. Masked forms always
 * require those EVEX features, with AVX512VL for 128/256 bits.
 * INT8/INT16 extensions have only unmasked 128/256-bit forms and require
 * their own independent feature. Compiler prerequisite closure and CPU/OS
 * admission are separate from the exact instruction constraints below.
 *
 * Mask bit i selects 32-bit result lane i. Merge forms retain accumulator
 * lanes; zero forms clear inactive lanes. Bits above the lane count are
 * ignored. Callers must enable a compatible target and admit its CPU and OS
 * state requirements before execution. There is no runtime dispatch.
 * \{ */

  // 128-bit core operations.
  /// Accumulate products of four unsigned bytes from a and signed bytes from b, modulo 2^32. Uses AVX-VNNI.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnni))
  native_nodiscard native_inline native_const native_target("avxvnni")
  __m128i dpbusd(__m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_dpbusd_avx_epi32(accumulator, a, b);
  }

  /// Accumulate products of four unsigned bytes from a and signed bytes from b, modulo 2^32. Uses AVX512VNNI.
  template<isa Arch> requires(!Arch.has(x86_feature::avxvnni) &&
      Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  __m128i dpbusd(__m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_dpbusd_epi32(accumulator, a, b);
  }

  /// dpbusd in active lanes; inactive lanes retain accumulator.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  __m128i mask_dpbusd(__m128i accumulator, __mmask8 mask, __m128i a, __m128i b) noexcept {
    return _mm_mask_dpbusd_epi32(accumulator, mask, a, b);
  }

  /// dpbusd in active lanes; inactive lanes become zero.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  __m128i maskz_dpbusd(__mmask8 mask, __m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_maskz_dpbusd_epi32(mask, accumulator, a, b);
  }

  /// Accumulate products of four unsigned bytes from a and signed bytes from b, with signed 32-bit saturation. Uses AVX-VNNI.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnni))
  native_nodiscard native_inline native_const native_target("avxvnni")
  __m128i dpbusds(__m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_dpbusds_avx_epi32(accumulator, a, b);
  }

  /// Accumulate products of four unsigned bytes from a and signed bytes from b, with signed 32-bit saturation. Uses AVX512VNNI.
  template<isa Arch> requires(!Arch.has(x86_feature::avxvnni) &&
      Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  __m128i dpbusds(__m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_dpbusds_epi32(accumulator, a, b);
  }

  /// dpbusds in active lanes; inactive lanes retain accumulator.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  __m128i mask_dpbusds(__m128i accumulator, __mmask8 mask, __m128i a, __m128i b) noexcept {
    return _mm_mask_dpbusds_epi32(accumulator, mask, a, b);
  }

  /// dpbusds in active lanes; inactive lanes become zero.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  __m128i maskz_dpbusds(__mmask8 mask, __m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_maskz_dpbusds_epi32(mask, accumulator, a, b);
  }

  /// Accumulate products of two signed words from a and b, modulo 2^32. Uses AVX-VNNI.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnni))
  native_nodiscard native_inline native_const native_target("avxvnni")
  __m128i dpwssd(__m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_dpwssd_avx_epi32(accumulator, a, b);
  }

  /// Accumulate products of two signed words from a and b, modulo 2^32. Uses AVX512VNNI.
  template<isa Arch> requires(!Arch.has(x86_feature::avxvnni) &&
      Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  __m128i dpwssd(__m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_dpwssd_epi32(accumulator, a, b);
  }

  /// dpwssd in active lanes; inactive lanes retain accumulator.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  __m128i mask_dpwssd(__m128i accumulator, __mmask8 mask, __m128i a, __m128i b) noexcept {
    return _mm_mask_dpwssd_epi32(accumulator, mask, a, b);
  }

  /// dpwssd in active lanes; inactive lanes become zero.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  __m128i maskz_dpwssd(__mmask8 mask, __m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_maskz_dpwssd_epi32(mask, accumulator, a, b);
  }

  /// Accumulate products of two signed words from a and b, with signed 32-bit saturation. Uses AVX-VNNI.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnni))
  native_nodiscard native_inline native_const native_target("avxvnni")
  __m128i dpwssds(__m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_dpwssds_avx_epi32(accumulator, a, b);
  }

  /// Accumulate products of two signed words from a and b, with signed 32-bit saturation. Uses AVX512VNNI.
  template<isa Arch> requires(!Arch.has(x86_feature::avxvnni) &&
      Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  __m128i dpwssds(__m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_dpwssds_epi32(accumulator, a, b);
  }

  /// dpwssds in active lanes; inactive lanes retain accumulator.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  __m128i mask_dpwssds(__m128i accumulator, __mmask8 mask, __m128i a, __m128i b) noexcept {
    return _mm_mask_dpwssds_epi32(accumulator, mask, a, b);
  }

  /// dpwssds in active lanes; inactive lanes become zero.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  __m128i maskz_dpwssds(__mmask8 mask, __m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_maskz_dpwssds_epi32(mask, accumulator, a, b);
  }

  // 256-bit core operations.
  /// Accumulate products of four unsigned bytes from a and signed bytes from b, modulo 2^32. Uses AVX-VNNI.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnni))
  native_nodiscard native_inline native_const native_target("avxvnni")
  __m256i dpbusd(__m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_dpbusd_avx_epi32(accumulator, a, b);
  }

  /// Accumulate products of four unsigned bytes from a and signed bytes from b, modulo 2^32. Uses AVX512VNNI.
  template<isa Arch> requires(!Arch.has(x86_feature::avxvnni) &&
      Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  __m256i dpbusd(__m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_dpbusd_epi32(accumulator, a, b);
  }

  /// dpbusd in active lanes; inactive lanes retain accumulator.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  __m256i mask_dpbusd(__m256i accumulator, __mmask8 mask, __m256i a, __m256i b) noexcept {
    return _mm256_mask_dpbusd_epi32(accumulator, mask, a, b);
  }

  /// dpbusd in active lanes; inactive lanes become zero.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  __m256i maskz_dpbusd(__mmask8 mask, __m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_maskz_dpbusd_epi32(mask, accumulator, a, b);
  }

  /// Accumulate products of four unsigned bytes from a and signed bytes from b, with signed 32-bit saturation. Uses AVX-VNNI.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnni))
  native_nodiscard native_inline native_const native_target("avxvnni")
  __m256i dpbusds(__m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_dpbusds_avx_epi32(accumulator, a, b);
  }

  /// Accumulate products of four unsigned bytes from a and signed bytes from b, with signed 32-bit saturation. Uses AVX512VNNI.
  template<isa Arch> requires(!Arch.has(x86_feature::avxvnni) &&
      Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  __m256i dpbusds(__m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_dpbusds_epi32(accumulator, a, b);
  }

  /// dpbusds in active lanes; inactive lanes retain accumulator.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  __m256i mask_dpbusds(__m256i accumulator, __mmask8 mask, __m256i a, __m256i b) noexcept {
    return _mm256_mask_dpbusds_epi32(accumulator, mask, a, b);
  }

  /// dpbusds in active lanes; inactive lanes become zero.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  __m256i maskz_dpbusds(__mmask8 mask, __m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_maskz_dpbusds_epi32(mask, accumulator, a, b);
  }

  /// Accumulate products of two signed words from a and b, modulo 2^32. Uses AVX-VNNI.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnni))
  native_nodiscard native_inline native_const native_target("avxvnni")
  __m256i dpwssd(__m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_dpwssd_avx_epi32(accumulator, a, b);
  }

  /// Accumulate products of two signed words from a and b, modulo 2^32. Uses AVX512VNNI.
  template<isa Arch> requires(!Arch.has(x86_feature::avxvnni) &&
      Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  __m256i dpwssd(__m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_dpwssd_epi32(accumulator, a, b);
  }

  /// dpwssd in active lanes; inactive lanes retain accumulator.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  __m256i mask_dpwssd(__m256i accumulator, __mmask8 mask, __m256i a, __m256i b) noexcept {
    return _mm256_mask_dpwssd_epi32(accumulator, mask, a, b);
  }

  /// dpwssd in active lanes; inactive lanes become zero.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  __m256i maskz_dpwssd(__mmask8 mask, __m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_maskz_dpwssd_epi32(mask, accumulator, a, b);
  }

  /// Accumulate products of two signed words from a and b, with signed 32-bit saturation. Uses AVX-VNNI.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnni))
  native_nodiscard native_inline native_const native_target("avxvnni")
  __m256i dpwssds(__m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_dpwssds_avx_epi32(accumulator, a, b);
  }

  /// Accumulate products of two signed words from a and b, with signed 32-bit saturation. Uses AVX512VNNI.
  template<isa Arch> requires(!Arch.has(x86_feature::avxvnni) &&
      Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  __m256i dpwssds(__m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_dpwssds_epi32(accumulator, a, b);
  }

  /// dpwssds in active lanes; inactive lanes retain accumulator.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  __m256i mask_dpwssds(__m256i accumulator, __mmask8 mask, __m256i a, __m256i b) noexcept {
    return _mm256_mask_dpwssds_epi32(accumulator, mask, a, b);
  }

  /// dpwssds in active lanes; inactive lanes become zero.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  __m256i maskz_dpwssds(__mmask8 mask, __m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_maskz_dpwssds_epi32(mask, accumulator, a, b);
  }

  // 512-bit core operations.
  /// Accumulate products of four unsigned bytes from a and signed bytes from b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni")
  __m512i dpbusd(__m512i accumulator, __m512i a, __m512i b) noexcept {
    return _mm512_dpbusd_epi32(accumulator, a, b);
  }

  /// dpbusd in active lanes; inactive lanes retain accumulator.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni")
  __m512i mask_dpbusd(__m512i accumulator, __mmask16 mask, __m512i a, __m512i b) noexcept {
    return _mm512_mask_dpbusd_epi32(accumulator, mask, a, b);
  }

  /// dpbusd in active lanes; inactive lanes become zero.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni")
  __m512i maskz_dpbusd(__mmask16 mask, __m512i accumulator, __m512i a, __m512i b) noexcept {
    return _mm512_maskz_dpbusd_epi32(mask, accumulator, a, b);
  }

  /// Accumulate products of four unsigned bytes from a and signed bytes from b, with signed 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni")
  __m512i dpbusds(__m512i accumulator, __m512i a, __m512i b) noexcept {
    return _mm512_dpbusds_epi32(accumulator, a, b);
  }

  /// dpbusds in active lanes; inactive lanes retain accumulator.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni")
  __m512i mask_dpbusds(__m512i accumulator, __mmask16 mask, __m512i a, __m512i b) noexcept {
    return _mm512_mask_dpbusds_epi32(accumulator, mask, a, b);
  }

  /// dpbusds in active lanes; inactive lanes become zero.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni")
  __m512i maskz_dpbusds(__mmask16 mask, __m512i accumulator, __m512i a, __m512i b) noexcept {
    return _mm512_maskz_dpbusds_epi32(mask, accumulator, a, b);
  }

  /// Accumulate products of two signed words from a and b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni")
  __m512i dpwssd(__m512i accumulator, __m512i a, __m512i b) noexcept {
    return _mm512_dpwssd_epi32(accumulator, a, b);
  }

  /// dpwssd in active lanes; inactive lanes retain accumulator.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni")
  __m512i mask_dpwssd(__m512i accumulator, __mmask16 mask, __m512i a, __m512i b) noexcept {
    return _mm512_mask_dpwssd_epi32(accumulator, mask, a, b);
  }

  /// dpwssd in active lanes; inactive lanes become zero.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni")
  __m512i maskz_dpwssd(__mmask16 mask, __m512i accumulator, __m512i a, __m512i b) noexcept {
    return _mm512_maskz_dpwssd_epi32(mask, accumulator, a, b);
  }

  /// Accumulate products of two signed words from a and b, with signed 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni")
  __m512i dpwssds(__m512i accumulator, __m512i a, __m512i b) noexcept {
    return _mm512_dpwssds_epi32(accumulator, a, b);
  }

  /// dpwssds in active lanes; inactive lanes retain accumulator.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni")
  __m512i mask_dpwssds(__m512i accumulator, __mmask16 mask, __m512i a, __m512i b) noexcept {
    return _mm512_mask_dpwssds_epi32(accumulator, mask, a, b);
  }

  /// dpwssds in active lanes; inactive lanes become zero.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni")
  __m512i maskz_dpwssds(__mmask16 mask, __m512i accumulator, __m512i a, __m512i b) noexcept {
    return _mm512_maskz_dpwssds_epi32(mask, accumulator, a, b);
  }

  /// Accumulate products of four signed bytes from a and b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint8))
  native_nodiscard native_inline native_const native_target("avxvnniint8")
  __m128i dpbssd(__m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_dpbssd_epi32(accumulator, a, b);
  }

  /// Accumulate products of four signed bytes from a and b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint8))
  native_nodiscard native_inline native_const native_target("avxvnniint8")
  __m256i dpbssd(__m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_dpbssd_epi32(accumulator, a, b);
  }

  /// Accumulate products of four signed bytes from a and b, with signed 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint8))
  native_nodiscard native_inline native_const native_target("avxvnniint8")
  __m128i dpbssds(__m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_dpbssds_epi32(accumulator, a, b);
  }

  /// Accumulate products of four signed bytes from a and b, with signed 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint8))
  native_nodiscard native_inline native_const native_target("avxvnniint8")
  __m256i dpbssds(__m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_dpbssds_epi32(accumulator, a, b);
  }

  /// Accumulate products of four signed bytes from a and unsigned bytes from b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint8))
  native_nodiscard native_inline native_const native_target("avxvnniint8")
  __m128i dpbsud(__m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_dpbsud_epi32(accumulator, a, b);
  }

  /// Accumulate products of four signed bytes from a and unsigned bytes from b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint8))
  native_nodiscard native_inline native_const native_target("avxvnniint8")
  __m256i dpbsud(__m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_dpbsud_epi32(accumulator, a, b);
  }

  /// Accumulate products of four signed bytes from a and unsigned bytes from b, with signed 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint8))
  native_nodiscard native_inline native_const native_target("avxvnniint8")
  __m128i dpbsuds(__m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_dpbsuds_epi32(accumulator, a, b);
  }

  /// Accumulate products of four signed bytes from a and unsigned bytes from b, with signed 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint8))
  native_nodiscard native_inline native_const native_target("avxvnniint8")
  __m256i dpbsuds(__m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_dpbsuds_epi32(accumulator, a, b);
  }

  /// Accumulate products of four unsigned bytes from a and b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint8))
  native_nodiscard native_inline native_const native_target("avxvnniint8")
  __m128i dpbuud(__m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_dpbuud_epi32(accumulator, a, b);
  }

  /// Accumulate products of four unsigned bytes from a and b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint8))
  native_nodiscard native_inline native_const native_target("avxvnniint8")
  __m256i dpbuud(__m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_dpbuud_epi32(accumulator, a, b);
  }

  /// Accumulate products of four unsigned bytes from a and b, with unsigned 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint8))
  native_nodiscard native_inline native_const native_target("avxvnniint8")
  __m128i dpbuuds(__m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_dpbuuds_epi32(accumulator, a, b);
  }

  /// Accumulate products of four unsigned bytes from a and b, with unsigned 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint8))
  native_nodiscard native_inline native_const native_target("avxvnniint8")
  __m256i dpbuuds(__m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_dpbuuds_epi32(accumulator, a, b);
  }

  /// Accumulate products of two signed words from a and unsigned words from b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint16))
  native_nodiscard native_inline native_const native_target("avxvnniint16")
  __m128i dpwsud(__m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_dpwsud_epi32(accumulator, a, b);
  }

  /// Accumulate products of two signed words from a and unsigned words from b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint16))
  native_nodiscard native_inline native_const native_target("avxvnniint16")
  __m256i dpwsud(__m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_dpwsud_epi32(accumulator, a, b);
  }

  /// Accumulate products of two signed words from a and unsigned words from b, with signed 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint16))
  native_nodiscard native_inline native_const native_target("avxvnniint16")
  __m128i dpwsuds(__m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_dpwsuds_epi32(accumulator, a, b);
  }

  /// Accumulate products of two signed words from a and unsigned words from b, with signed 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint16))
  native_nodiscard native_inline native_const native_target("avxvnniint16")
  __m256i dpwsuds(__m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_dpwsuds_epi32(accumulator, a, b);
  }

  /// Accumulate products of two unsigned words from a and signed words from b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint16))
  native_nodiscard native_inline native_const native_target("avxvnniint16")
  __m128i dpwusd(__m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_dpwusd_epi32(accumulator, a, b);
  }

  /// Accumulate products of two unsigned words from a and signed words from b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint16))
  native_nodiscard native_inline native_const native_target("avxvnniint16")
  __m256i dpwusd(__m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_dpwusd_epi32(accumulator, a, b);
  }

  /// Accumulate products of two unsigned words from a and signed words from b, with signed 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint16))
  native_nodiscard native_inline native_const native_target("avxvnniint16")
  __m128i dpwusds(__m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_dpwusds_epi32(accumulator, a, b);
  }

  /// Accumulate products of two unsigned words from a and signed words from b, with signed 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint16))
  native_nodiscard native_inline native_const native_target("avxvnniint16")
  __m256i dpwusds(__m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_dpwusds_epi32(accumulator, a, b);
  }

  /// Accumulate products of two unsigned words from a and b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint16))
  native_nodiscard native_inline native_const native_target("avxvnniint16")
  __m128i dpwuud(__m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_dpwuud_epi32(accumulator, a, b);
  }

  /// Accumulate products of two unsigned words from a and b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint16))
  native_nodiscard native_inline native_const native_target("avxvnniint16")
  __m256i dpwuud(__m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_dpwuud_epi32(accumulator, a, b);
  }

  /// Accumulate products of two unsigned words from a and b, with unsigned 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint16))
  native_nodiscard native_inline native_const native_target("avxvnniint16")
  __m128i dpwuuds(__m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_dpwuuds_epi32(accumulator, a, b);
  }

  /// Accumulate products of two unsigned words from a and b, with unsigned 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint16))
  native_nodiscard native_inline native_const native_target("avxvnniint16")
  __m256i dpwuuds(__m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_dpwuuds_epi32(accumulator, a, b);
  }

  // Exact vector operands prevent Clang's permissive vector conversions from
  // selecting an instruction for floating-point or mixed register shapes.
  /// \cond
  namespace detail {
    template<class S, class A, class B>
    concept vnni_registers = __is_same(S, A) && __is_same(A, B) &&
      (__is_same(S, __m128i) || __is_same(S, __m256i) || __is_same(S, __m512i));
  }
  template<isa Arch, class S, class A, class B>
  void dpbusd(S, A, B) = delete;
  template<isa Arch, class S, class A, class B>
  void dpbusds(S, A, B) = delete;
  template<isa Arch, class S, class A, class B>
  void dpwssd(S, A, B) = delete;
  template<isa Arch, class S, class A, class B>
  void dpwssds(S, A, B) = delete;
  template<isa Arch, class S, class A, class B>
  void dpbssd(S, A, B) = delete;
  template<isa Arch, class S, class A, class B>
  void dpbssds(S, A, B) = delete;
  template<isa Arch, class S, class A, class B>
  void dpbsud(S, A, B) = delete;
  template<isa Arch, class S, class A, class B>
  void dpbsuds(S, A, B) = delete;
  template<isa Arch, class S, class A, class B>
  void dpbuud(S, A, B) = delete;
  template<isa Arch, class S, class A, class B>
  void dpbuuds(S, A, B) = delete;
  template<isa Arch, class S, class A, class B>
  void dpwsud(S, A, B) = delete;
  template<isa Arch, class S, class A, class B>
  void dpwsuds(S, A, B) = delete;
  template<isa Arch, class S, class A, class B>
  void dpwusd(S, A, B) = delete;
  template<isa Arch, class S, class A, class B>
  void dpwusds(S, A, B) = delete;
  template<isa Arch, class S, class A, class B>
  void dpwuud(S, A, B) = delete;
  template<isa Arch, class S, class A, class B>
  void dpwuuds(S, A, B) = delete;
  template<isa Arch, class S, class M, class A, class B>
    requires(!detail::vnni_registers<S, A, B>)
  void mask_dpbusd(S, M, A, B) = delete;
  template<isa Arch, class S, class M, class A, class B>
    requires(!detail::vnni_registers<S, A, B>)
  void maskz_dpbusd(M, S, A, B) = delete;
  template<isa Arch, class S, class M, class A, class B>
    requires(!detail::vnni_registers<S, A, B>)
  void mask_dpbusds(S, M, A, B) = delete;
  template<isa Arch, class S, class M, class A, class B>
    requires(!detail::vnni_registers<S, A, B>)
  void maskz_dpbusds(M, S, A, B) = delete;
  template<isa Arch, class S, class M, class A, class B>
    requires(!detail::vnni_registers<S, A, B>)
  void mask_dpwssd(S, M, A, B) = delete;
  template<isa Arch, class S, class M, class A, class B>
    requires(!detail::vnni_registers<S, A, B>)
  void maskz_dpwssd(M, S, A, B) = delete;
  template<isa Arch, class S, class M, class A, class B>
    requires(!detail::vnni_registers<S, A, B>)
  void mask_dpwssds(S, M, A, B) = delete;
  template<isa Arch, class S, class M, class A, class B>
    requires(!detail::vnni_registers<S, A, B>)
  void maskz_dpwssds(M, S, A, B) = delete;
  /// \endcond
/// \}
}
#endif
