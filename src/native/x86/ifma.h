// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#if NATIVE_HOST_X86
#include <immintrin.h>

namespace native::detail::x86_ifma {

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avxifma) && Arch.has(x86_feature::avx))
  native_nodiscard native_inline native_const native_target("avxifma")
  __m128i madd52lo(__m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_madd52lo_avx_epu64(accumulator, a, b);
  }

  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avxifma) && Arch.has(x86_feature::avx)) &&
      Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma,avx512vl")
  __m128i madd52lo(__m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_madd52lo_epu64(accumulator, a, b);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avxifma) && Arch.has(x86_feature::avx))
  native_nodiscard native_inline native_const native_target("avxifma")
  __m256i madd52lo(__m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_madd52lo_avx_epu64(accumulator, a, b);
  }

  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avxifma) && Arch.has(x86_feature::avx)) &&
      Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma,avx512vl")
  __m256i madd52lo(__m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_madd52lo_epu64(accumulator, a, b);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma")
  __m512i madd52lo(__m512i accumulator, __m512i a, __m512i b) noexcept {
    return _mm512_madd52lo_epu64(accumulator, a, b);
  }

  template<isa<x86> Arch, class... Args>
  void madd52lo(Args...) = delete;

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma,avx512vl")
  __m128i mask_madd52lo(__m128i accumulator, __mmask8 mask, __m128i a, __m128i b) noexcept {
    return _mm_mask_madd52lo_epu64(accumulator, mask, a, b);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma,avx512vl")
  __m256i mask_madd52lo(__m256i accumulator, __mmask8 mask, __m256i a, __m256i b) noexcept {
    return _mm256_mask_madd52lo_epu64(accumulator, mask, a, b);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma")
  __m512i mask_madd52lo(__m512i accumulator, __mmask8 mask, __m512i a, __m512i b) noexcept {
    return _mm512_mask_madd52lo_epu64(accumulator, mask, a, b);
  }

  template<isa<x86> Arch, class... Args>
  void mask_madd52lo(Args...) = delete;

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma,avx512vl")
  __m128i maskz_madd52lo(__mmask8 mask, __m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_maskz_madd52lo_epu64(mask, accumulator, a, b);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma,avx512vl")
  __m256i maskz_madd52lo(__mmask8 mask, __m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_maskz_madd52lo_epu64(mask, accumulator, a, b);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma")
  __m512i maskz_madd52lo(__mmask8 mask, __m512i accumulator, __m512i a, __m512i b) noexcept {
    return _mm512_maskz_madd52lo_epu64(mask, accumulator, a, b);
  }

  template<isa<x86> Arch, class... Args>
  void maskz_madd52lo(Args...) = delete;

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avxifma) && Arch.has(x86_feature::avx))
  native_nodiscard native_inline native_const native_target("avxifma")
  __m128i madd52hi(__m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_madd52hi_avx_epu64(accumulator, a, b);
  }

  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avxifma) && Arch.has(x86_feature::avx)) &&
      Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma,avx512vl")
  __m128i madd52hi(__m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_madd52hi_epu64(accumulator, a, b);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avxifma) && Arch.has(x86_feature::avx))
  native_nodiscard native_inline native_const native_target("avxifma")
  __m256i madd52hi(__m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_madd52hi_avx_epu64(accumulator, a, b);
  }

  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avxifma) && Arch.has(x86_feature::avx)) &&
      Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma,avx512vl")
  __m256i madd52hi(__m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_madd52hi_epu64(accumulator, a, b);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma")
  __m512i madd52hi(__m512i accumulator, __m512i a, __m512i b) noexcept {
    return _mm512_madd52hi_epu64(accumulator, a, b);
  }

  template<isa<x86> Arch, class... Args>
  void madd52hi(Args...) = delete;

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma,avx512vl")
  __m128i mask_madd52hi(__m128i accumulator, __mmask8 mask, __m128i a, __m128i b) noexcept {
    return _mm_mask_madd52hi_epu64(accumulator, mask, a, b);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma,avx512vl")
  __m256i mask_madd52hi(__m256i accumulator, __mmask8 mask, __m256i a, __m256i b) noexcept {
    return _mm256_mask_madd52hi_epu64(accumulator, mask, a, b);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma")
  __m512i mask_madd52hi(__m512i accumulator, __mmask8 mask, __m512i a, __m512i b) noexcept {
    return _mm512_mask_madd52hi_epu64(accumulator, mask, a, b);
  }

  template<isa<x86> Arch, class... Args>
  void mask_madd52hi(Args...) = delete;

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma,avx512vl")
  __m128i maskz_madd52hi(__mmask8 mask, __m128i accumulator, __m128i a, __m128i b) noexcept {
    return _mm_maskz_madd52hi_epu64(mask, accumulator, a, b);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma,avx512vl")
  __m256i maskz_madd52hi(__mmask8 mask, __m256i accumulator, __m256i a, __m256i b) noexcept {
    return _mm256_maskz_madd52hi_epu64(mask, accumulator, a, b);
  }

  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma")
  __m512i maskz_madd52hi(__mmask8 mask, __m512i accumulator, __m512i a, __m512i b) noexcept {
    return _mm512_maskz_madd52hi_epu64(mask, accumulator, a, b);
  }

  template<isa<x86> Arch, class... Args>
  void maskz_madd52hi(Args...) = delete;
}
#endif
