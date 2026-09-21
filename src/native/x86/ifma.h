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

#include "native/x86/integer_constant.h"

namespace native::detail::x86_ifma_constant {
  template<bool High>
  constexpr std::uint64_t product(std::uint64_t a, std::uint64_t b) noexcept {
    constexpr std::uint64_t mask26 = (std::uint64_t{1} << 26) - 1;
    constexpr std::uint64_t mask52 = (std::uint64_t{1} << 52) - 1;
    a &= mask52;
    b &= mask52;
    auto low = (a & mask26) * (b & mask26);
    auto middle = (a >> 26) * (b & mask26) + (a & mask26) * (b >> 26);
    low += (middle & mask26) << 26;
    if constexpr (High) {
      return (a >> 26) * (b >> 26) + (middle >> 26) + (low >> 52);
    } else {
      return low & mask52;
    }
  }

  template<bool High, class V>
  constexpr V accumulate(V accumulator, V a, V b, std::uint64_t mask, bool zero) noexcept {
    auto result = x86_instruction_constant::lanes(accumulator);
    auto left = x86_instruction_constant::lanes(a);
    auto right = x86_instruction_constant::lanes(b);
    for (unsigned lane = 0; lane < V::lanes; ++lane) {
      if ((mask >> lane) & 1) {
        result[lane] += product<High>(left[lane], right[lane]);
      } else if (zero) {
        result[lane] = 0;
      }
    }
    return V::load(result.data());
  }
}
