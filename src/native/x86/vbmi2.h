// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#if NATIVE_HOST_X86
#include <immintrin.h>
#endif
#if NATIVE_HOST_X86 || defined(NATIVE_DOXYGEN)
namespace native::detail::x86_vbmi2 {
  // Internal register and memory helpers for native.x86.vbmi2.

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i mask_vpcompressb(
    __m128i source,
    __mmask16 mask,
    __m128i value) noexcept {
    return _mm_mask_compress_epi8(source, mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i maskz_vpcompressb(
    __mmask16 mask,
    __m128i value) noexcept {
    return _mm_maskz_compress_epi8(mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_inline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  void mask_vpcompressb(
    void * destination,
    __mmask16 mask,
    __m128i value) noexcept {
    _mm_mask_compressstoreu_epi8(destination, mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i mask_vpexpandb(
    __m128i source,
    __mmask16 mask,
    __m128i value) noexcept {
    return _mm_mask_expand_epi8(source, mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i maskz_vpexpandb(
    __mmask16 mask,
    __m128i value) noexcept {
    return _mm_maskz_expand_epi8(mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_pure native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i mask_vpexpandb(
    __m128i source,
    __mmask16 mask,
    void const * memory) noexcept {
    return _mm_mask_expandloadu_epi8(source, mask, memory);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_pure native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i maskz_vpexpandb_128(
    __mmask16 mask,
    void const * memory) noexcept {
    return _mm_maskz_expandloadu_epi8(mask, memory);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i mask_vpcompressw(
    __m128i source,
    __mmask8 mask,
    __m128i value) noexcept {
    return _mm_mask_compress_epi16(source, mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i maskz_vpcompressw(
    __mmask8 mask,
    __m128i value) noexcept {
    return _mm_maskz_compress_epi16(mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_inline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  void mask_vpcompressw(
    void * destination,
    __mmask8 mask,
    __m128i value) noexcept {
    _mm_mask_compressstoreu_epi16(destination, mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i mask_vpexpandw(
    __m128i source,
    __mmask8 mask,
    __m128i value) noexcept {
    return _mm_mask_expand_epi16(source, mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i maskz_vpexpandw(
    __mmask8 mask,
    __m128i value) noexcept {
    return _mm_maskz_expand_epi16(mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_pure native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i mask_vpexpandw(
    __m128i source,
    __mmask8 mask,
    void const * memory) noexcept {
    return _mm_mask_expandloadu_epi16(source, mask, memory);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_pure native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i maskz_vpexpandw_128(
    __mmask8 mask,
    void const * memory) noexcept {
    return _mm_maskz_expandloadu_epi16(mask, memory);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i vpshldw(
    __m128i a,
    __m128i b) noexcept {
    return _mm_shldi_epi16(a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i mask_vpshldw(
    __m128i source,
    __mmask8 mask,
    __m128i a,
    __m128i b) noexcept {
    return _mm_mask_shldi_epi16(source, mask, a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i maskz_vpshldw(
    __mmask8 mask,
    __m128i a,
    __m128i b) noexcept {
    return _mm_maskz_shldi_epi16(mask, a, b, Imm8);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i vpshldvw(
    __m128i a,
    __m128i b,
    __m128i counts) noexcept {
    return _mm_shldv_epi16(a, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i mask_vpshldvw(
    __m128i a,
    __mmask8 mask,
    __m128i b,
    __m128i counts) noexcept {
    return _mm_mask_shldv_epi16(a, mask, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i maskz_vpshldvw(
    __mmask8 mask,
    __m128i a,
    __m128i b,
    __m128i counts) noexcept {
    return _mm_maskz_shldv_epi16(mask, a, b, counts);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i vpshrdw(
    __m128i a,
    __m128i b) noexcept {
    return _mm_shrdi_epi16(a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i mask_vpshrdw(
    __m128i source,
    __mmask8 mask,
    __m128i a,
    __m128i b) noexcept {
    return _mm_mask_shrdi_epi16(source, mask, a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i maskz_vpshrdw(
    __mmask8 mask,
    __m128i a,
    __m128i b) noexcept {
    return _mm_maskz_shrdi_epi16(mask, a, b, Imm8);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i vpshrdvw(
    __m128i a,
    __m128i b,
    __m128i counts) noexcept {
    return _mm_shrdv_epi16(a, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i mask_vpshrdvw(
    __m128i a,
    __mmask8 mask,
    __m128i b,
    __m128i counts) noexcept {
    return _mm_mask_shrdv_epi16(a, mask, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i maskz_vpshrdvw(
    __mmask8 mask,
    __m128i a,
    __m128i b,
    __m128i counts) noexcept {
    return _mm_maskz_shrdv_epi16(mask, a, b, counts);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i vpshldd(
    __m128i a,
    __m128i b) noexcept {
    return _mm_shldi_epi32(a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i mask_vpshldd(
    __m128i source,
    __mmask8 mask,
    __m128i a,
    __m128i b) noexcept {
    return _mm_mask_shldi_epi32(source, mask, a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i maskz_vpshldd(
    __mmask8 mask,
    __m128i a,
    __m128i b) noexcept {
    return _mm_maskz_shldi_epi32(mask, a, b, Imm8);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i vpshldvd(
    __m128i a,
    __m128i b,
    __m128i counts) noexcept {
    return _mm_shldv_epi32(a, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i mask_vpshldvd(
    __m128i a,
    __mmask8 mask,
    __m128i b,
    __m128i counts) noexcept {
    return _mm_mask_shldv_epi32(a, mask, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i maskz_vpshldvd(
    __mmask8 mask,
    __m128i a,
    __m128i b,
    __m128i counts) noexcept {
    return _mm_maskz_shldv_epi32(mask, a, b, counts);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i vpshrdd(
    __m128i a,
    __m128i b) noexcept {
    return _mm_shrdi_epi32(a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i mask_vpshrdd(
    __m128i source,
    __mmask8 mask,
    __m128i a,
    __m128i b) noexcept {
    return _mm_mask_shrdi_epi32(source, mask, a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i maskz_vpshrdd(
    __mmask8 mask,
    __m128i a,
    __m128i b) noexcept {
    return _mm_maskz_shrdi_epi32(mask, a, b, Imm8);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i vpshrdvd(
    __m128i a,
    __m128i b,
    __m128i counts) noexcept {
    return _mm_shrdv_epi32(a, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i mask_vpshrdvd(
    __m128i a,
    __mmask8 mask,
    __m128i b,
    __m128i counts) noexcept {
    return _mm_mask_shrdv_epi32(a, mask, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i maskz_vpshrdvd(
    __mmask8 mask,
    __m128i a,
    __m128i b,
    __m128i counts) noexcept {
    return _mm_maskz_shrdv_epi32(mask, a, b, counts);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i vpshldq(
    __m128i a,
    __m128i b) noexcept {
    return _mm_shldi_epi64(a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i mask_vpshldq(
    __m128i source,
    __mmask8 mask,
    __m128i a,
    __m128i b) noexcept {
    return _mm_mask_shldi_epi64(source, mask, a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i maskz_vpshldq(
    __mmask8 mask,
    __m128i a,
    __m128i b) noexcept {
    return _mm_maskz_shldi_epi64(mask, a, b, Imm8);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i vpshldvq(
    __m128i a,
    __m128i b,
    __m128i counts) noexcept {
    return _mm_shldv_epi64(a, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i mask_vpshldvq(
    __m128i a,
    __mmask8 mask,
    __m128i b,
    __m128i counts) noexcept {
    return _mm_mask_shldv_epi64(a, mask, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i maskz_vpshldvq(
    __mmask8 mask,
    __m128i a,
    __m128i b,
    __m128i counts) noexcept {
    return _mm_maskz_shldv_epi64(mask, a, b, counts);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i vpshrdq(
    __m128i a,
    __m128i b) noexcept {
    return _mm_shrdi_epi64(a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i mask_vpshrdq(
    __m128i source,
    __mmask8 mask,
    __m128i a,
    __m128i b) noexcept {
    return _mm_mask_shrdi_epi64(source, mask, a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i maskz_vpshrdq(
    __mmask8 mask,
    __m128i a,
    __m128i b) noexcept {
    return _mm_maskz_shrdi_epi64(mask, a, b, Imm8);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i vpshrdvq(
    __m128i a,
    __m128i b,
    __m128i counts) noexcept {
    return _mm_shrdv_epi64(a, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i mask_vpshrdvq(
    __m128i a,
    __mmask8 mask,
    __m128i b,
    __m128i counts) noexcept {
    return _mm_mask_shrdv_epi64(a, mask, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m128i maskz_vpshrdvq(
    __mmask8 mask,
    __m128i a,
    __m128i b,
    __m128i counts) noexcept {
    return _mm_maskz_shrdv_epi64(mask, a, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i mask_vpcompressb(
    __m256i source,
    __mmask32 mask,
    __m256i value) noexcept {
    return _mm256_mask_compress_epi8(source, mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i maskz_vpcompressb(
    __mmask32 mask,
    __m256i value) noexcept {
    return _mm256_maskz_compress_epi8(mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_inline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  void mask_vpcompressb(
    void * destination,
    __mmask32 mask,
    __m256i value) noexcept {
    _mm256_mask_compressstoreu_epi8(destination, mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i mask_vpexpandb(
    __m256i source,
    __mmask32 mask,
    __m256i value) noexcept {
    return _mm256_mask_expand_epi8(source, mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i maskz_vpexpandb(
    __mmask32 mask,
    __m256i value) noexcept {
    return _mm256_maskz_expand_epi8(mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_pure native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i mask_vpexpandb(
    __m256i source,
    __mmask32 mask,
    void const * memory) noexcept {
    return _mm256_mask_expandloadu_epi8(source, mask, memory);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_pure native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i maskz_vpexpandb_256(
    __mmask32 mask,
    void const * memory) noexcept {
    return _mm256_maskz_expandloadu_epi8(mask, memory);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i mask_vpcompressw(
    __m256i source,
    __mmask16 mask,
    __m256i value) noexcept {
    return _mm256_mask_compress_epi16(source, mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i maskz_vpcompressw(
    __mmask16 mask,
    __m256i value) noexcept {
    return _mm256_maskz_compress_epi16(mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_inline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  void mask_vpcompressw(
    void * destination,
    __mmask16 mask,
    __m256i value) noexcept {
    _mm256_mask_compressstoreu_epi16(destination, mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i mask_vpexpandw(
    __m256i source,
    __mmask16 mask,
    __m256i value) noexcept {
    return _mm256_mask_expand_epi16(source, mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i maskz_vpexpandw(
    __mmask16 mask,
    __m256i value) noexcept {
    return _mm256_maskz_expand_epi16(mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_pure native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i mask_vpexpandw(
    __m256i source,
    __mmask16 mask,
    void const * memory) noexcept {
    return _mm256_mask_expandloadu_epi16(source, mask, memory);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_pure native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i maskz_vpexpandw_256(
    __mmask16 mask,
    void const * memory) noexcept {
    return _mm256_maskz_expandloadu_epi16(mask, memory);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i vpshldw(
    __m256i a,
    __m256i b) noexcept {
    return _mm256_shldi_epi16(a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i mask_vpshldw(
    __m256i source,
    __mmask16 mask,
    __m256i a,
    __m256i b) noexcept {
    return _mm256_mask_shldi_epi16(source, mask, a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i maskz_vpshldw(
    __mmask16 mask,
    __m256i a,
    __m256i b) noexcept {
    return _mm256_maskz_shldi_epi16(mask, a, b, Imm8);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i vpshldvw(
    __m256i a,
    __m256i b,
    __m256i counts) noexcept {
    return _mm256_shldv_epi16(a, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i mask_vpshldvw(
    __m256i a,
    __mmask16 mask,
    __m256i b,
    __m256i counts) noexcept {
    return _mm256_mask_shldv_epi16(a, mask, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i maskz_vpshldvw(
    __mmask16 mask,
    __m256i a,
    __m256i b,
    __m256i counts) noexcept {
    return _mm256_maskz_shldv_epi16(mask, a, b, counts);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i vpshrdw(
    __m256i a,
    __m256i b) noexcept {
    return _mm256_shrdi_epi16(a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i mask_vpshrdw(
    __m256i source,
    __mmask16 mask,
    __m256i a,
    __m256i b) noexcept {
    return _mm256_mask_shrdi_epi16(source, mask, a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i maskz_vpshrdw(
    __mmask16 mask,
    __m256i a,
    __m256i b) noexcept {
    return _mm256_maskz_shrdi_epi16(mask, a, b, Imm8);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i vpshrdvw(
    __m256i a,
    __m256i b,
    __m256i counts) noexcept {
    return _mm256_shrdv_epi16(a, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i mask_vpshrdvw(
    __m256i a,
    __mmask16 mask,
    __m256i b,
    __m256i counts) noexcept {
    return _mm256_mask_shrdv_epi16(a, mask, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i maskz_vpshrdvw(
    __mmask16 mask,
    __m256i a,
    __m256i b,
    __m256i counts) noexcept {
    return _mm256_maskz_shrdv_epi16(mask, a, b, counts);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i vpshldd(
    __m256i a,
    __m256i b) noexcept {
    return _mm256_shldi_epi32(a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i mask_vpshldd(
    __m256i source,
    __mmask8 mask,
    __m256i a,
    __m256i b) noexcept {
    return _mm256_mask_shldi_epi32(source, mask, a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i maskz_vpshldd(
    __mmask8 mask,
    __m256i a,
    __m256i b) noexcept {
    return _mm256_maskz_shldi_epi32(mask, a, b, Imm8);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i vpshldvd(
    __m256i a,
    __m256i b,
    __m256i counts) noexcept {
    return _mm256_shldv_epi32(a, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i mask_vpshldvd(
    __m256i a,
    __mmask8 mask,
    __m256i b,
    __m256i counts) noexcept {
    return _mm256_mask_shldv_epi32(a, mask, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i maskz_vpshldvd(
    __mmask8 mask,
    __m256i a,
    __m256i b,
    __m256i counts) noexcept {
    return _mm256_maskz_shldv_epi32(mask, a, b, counts);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i vpshrdd(
    __m256i a,
    __m256i b) noexcept {
    return _mm256_shrdi_epi32(a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i mask_vpshrdd(
    __m256i source,
    __mmask8 mask,
    __m256i a,
    __m256i b) noexcept {
    return _mm256_mask_shrdi_epi32(source, mask, a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i maskz_vpshrdd(
    __mmask8 mask,
    __m256i a,
    __m256i b) noexcept {
    return _mm256_maskz_shrdi_epi32(mask, a, b, Imm8);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i vpshrdvd(
    __m256i a,
    __m256i b,
    __m256i counts) noexcept {
    return _mm256_shrdv_epi32(a, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i mask_vpshrdvd(
    __m256i a,
    __mmask8 mask,
    __m256i b,
    __m256i counts) noexcept {
    return _mm256_mask_shrdv_epi32(a, mask, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i maskz_vpshrdvd(
    __mmask8 mask,
    __m256i a,
    __m256i b,
    __m256i counts) noexcept {
    return _mm256_maskz_shrdv_epi32(mask, a, b, counts);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i vpshldq(
    __m256i a,
    __m256i b) noexcept {
    return _mm256_shldi_epi64(a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i mask_vpshldq(
    __m256i source,
    __mmask8 mask,
    __m256i a,
    __m256i b) noexcept {
    return _mm256_mask_shldi_epi64(source, mask, a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i maskz_vpshldq(
    __mmask8 mask,
    __m256i a,
    __m256i b) noexcept {
    return _mm256_maskz_shldi_epi64(mask, a, b, Imm8);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i vpshldvq(
    __m256i a,
    __m256i b,
    __m256i counts) noexcept {
    return _mm256_shldv_epi64(a, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i mask_vpshldvq(
    __m256i a,
    __mmask8 mask,
    __m256i b,
    __m256i counts) noexcept {
    return _mm256_mask_shldv_epi64(a, mask, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i maskz_vpshldvq(
    __mmask8 mask,
    __m256i a,
    __m256i b,
    __m256i counts) noexcept {
    return _mm256_maskz_shldv_epi64(mask, a, b, counts);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i vpshrdq(
    __m256i a,
    __m256i b) noexcept {
    return _mm256_shrdi_epi64(a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i mask_vpshrdq(
    __m256i source,
    __mmask8 mask,
    __m256i a,
    __m256i b) noexcept {
    return _mm256_mask_shrdi_epi64(source, mask, a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i maskz_vpshrdq(
    __mmask8 mask,
    __m256i a,
    __m256i b) noexcept {
    return _mm256_maskz_shrdi_epi64(mask, a, b, Imm8);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i vpshrdvq(
    __m256i a,
    __m256i b,
    __m256i counts) noexcept {
    return _mm256_shrdv_epi64(a, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i mask_vpshrdvq(
    __m256i a,
    __mmask8 mask,
    __m256i b,
    __m256i counts) noexcept {
    return _mm256_mask_shrdv_epi64(a, mask, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
  __m256i maskz_vpshrdvq(
    __mmask8 mask,
    __m256i a,
    __m256i b,
    __m256i counts) noexcept {
    return _mm256_maskz_shrdv_epi64(mask, a, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i mask_vpcompressb(
    __m512i source,
    __mmask64 mask,
    __m512i value) noexcept {
    return _mm512_mask_compress_epi8(source, mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i maskz_vpcompressb(
    __mmask64 mask,
    __m512i value) noexcept {
    return _mm512_maskz_compress_epi8(mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_inline native_target("avx512f,avx512bw,avx512vbmi2")
  void mask_vpcompressb(
    void * destination,
    __mmask64 mask,
    __m512i value) noexcept {
    _mm512_mask_compressstoreu_epi8(destination, mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i mask_vpexpandb(
    __m512i source,
    __mmask64 mask,
    __m512i value) noexcept {
    return _mm512_mask_expand_epi8(source, mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i maskz_vpexpandb(
    __mmask64 mask,
    __m512i value) noexcept {
    return _mm512_maskz_expand_epi8(mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_pure native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i mask_vpexpandb(
    __m512i source,
    __mmask64 mask,
    void const * memory) noexcept {
    return _mm512_mask_expandloadu_epi8(source, mask, memory);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_pure native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i maskz_vpexpandb_512(
    __mmask64 mask,
    void const * memory) noexcept {
    return _mm512_maskz_expandloadu_epi8(mask, memory);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i mask_vpcompressw(
    __m512i source,
    __mmask32 mask,
    __m512i value) noexcept {
    return _mm512_mask_compress_epi16(source, mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i maskz_vpcompressw(
    __mmask32 mask,
    __m512i value) noexcept {
    return _mm512_maskz_compress_epi16(mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_inline native_target("avx512f,avx512bw,avx512vbmi2")
  void mask_vpcompressw(
    void * destination,
    __mmask32 mask,
    __m512i value) noexcept {
    _mm512_mask_compressstoreu_epi16(destination, mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i mask_vpexpandw(
    __m512i source,
    __mmask32 mask,
    __m512i value) noexcept {
    return _mm512_mask_expand_epi16(source, mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i maskz_vpexpandw(
    __mmask32 mask,
    __m512i value) noexcept {
    return _mm512_maskz_expand_epi16(mask, value);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_pure native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i mask_vpexpandw(
    __m512i source,
    __mmask32 mask,
    void const * memory) noexcept {
    return _mm512_mask_expandloadu_epi16(source, mask, memory);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_pure native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i maskz_vpexpandw_512(
    __mmask32 mask,
    void const * memory) noexcept {
    return _mm512_maskz_expandloadu_epi16(mask, memory);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i vpshldw(
    __m512i a,
    __m512i b) noexcept {
    return _mm512_shldi_epi16(a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i mask_vpshldw(
    __m512i source,
    __mmask32 mask,
    __m512i a,
    __m512i b) noexcept {
    return _mm512_mask_shldi_epi16(source, mask, a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i maskz_vpshldw(
    __mmask32 mask,
    __m512i a,
    __m512i b) noexcept {
    return _mm512_maskz_shldi_epi16(mask, a, b, Imm8);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i vpshldvw(
    __m512i a,
    __m512i b,
    __m512i counts) noexcept {
    return _mm512_shldv_epi16(a, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i mask_vpshldvw(
    __m512i a,
    __mmask32 mask,
    __m512i b,
    __m512i counts) noexcept {
    return _mm512_mask_shldv_epi16(a, mask, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i maskz_vpshldvw(
    __mmask32 mask,
    __m512i a,
    __m512i b,
    __m512i counts) noexcept {
    return _mm512_maskz_shldv_epi16(mask, a, b, counts);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i vpshrdw(
    __m512i a,
    __m512i b) noexcept {
    return _mm512_shrdi_epi16(a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i mask_vpshrdw(
    __m512i source,
    __mmask32 mask,
    __m512i a,
    __m512i b) noexcept {
    return _mm512_mask_shrdi_epi16(source, mask, a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i maskz_vpshrdw(
    __mmask32 mask,
    __m512i a,
    __m512i b) noexcept {
    return _mm512_maskz_shrdi_epi16(mask, a, b, Imm8);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i vpshrdvw(
    __m512i a,
    __m512i b,
    __m512i counts) noexcept {
    return _mm512_shrdv_epi16(a, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i mask_vpshrdvw(
    __m512i a,
    __mmask32 mask,
    __m512i b,
    __m512i counts) noexcept {
    return _mm512_mask_shrdv_epi16(a, mask, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i maskz_vpshrdvw(
    __mmask32 mask,
    __m512i a,
    __m512i b,
    __m512i counts) noexcept {
    return _mm512_maskz_shrdv_epi16(mask, a, b, counts);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i vpshldd(
    __m512i a,
    __m512i b) noexcept {
    return _mm512_shldi_epi32(a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i mask_vpshldd(
    __m512i source,
    __mmask16 mask,
    __m512i a,
    __m512i b) noexcept {
    return _mm512_mask_shldi_epi32(source, mask, a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i maskz_vpshldd(
    __mmask16 mask,
    __m512i a,
    __m512i b) noexcept {
    return _mm512_maskz_shldi_epi32(mask, a, b, Imm8);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i vpshldvd(
    __m512i a,
    __m512i b,
    __m512i counts) noexcept {
    return _mm512_shldv_epi32(a, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i mask_vpshldvd(
    __m512i a,
    __mmask16 mask,
    __m512i b,
    __m512i counts) noexcept {
    return _mm512_mask_shldv_epi32(a, mask, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i maskz_vpshldvd(
    __mmask16 mask,
    __m512i a,
    __m512i b,
    __m512i counts) noexcept {
    return _mm512_maskz_shldv_epi32(mask, a, b, counts);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i vpshrdd(
    __m512i a,
    __m512i b) noexcept {
    return _mm512_shrdi_epi32(a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i mask_vpshrdd(
    __m512i source,
    __mmask16 mask,
    __m512i a,
    __m512i b) noexcept {
    return _mm512_mask_shrdi_epi32(source, mask, a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i maskz_vpshrdd(
    __mmask16 mask,
    __m512i a,
    __m512i b) noexcept {
    return _mm512_maskz_shrdi_epi32(mask, a, b, Imm8);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i vpshrdvd(
    __m512i a,
    __m512i b,
    __m512i counts) noexcept {
    return _mm512_shrdv_epi32(a, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i mask_vpshrdvd(
    __m512i a,
    __mmask16 mask,
    __m512i b,
    __m512i counts) noexcept {
    return _mm512_mask_shrdv_epi32(a, mask, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i maskz_vpshrdvd(
    __mmask16 mask,
    __m512i a,
    __m512i b,
    __m512i counts) noexcept {
    return _mm512_maskz_shrdv_epi32(mask, a, b, counts);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i vpshldq(
    __m512i a,
    __m512i b) noexcept {
    return _mm512_shldi_epi64(a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i mask_vpshldq(
    __m512i source,
    __mmask8 mask,
    __m512i a,
    __m512i b) noexcept {
    return _mm512_mask_shldi_epi64(source, mask, a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i maskz_vpshldq(
    __mmask8 mask,
    __m512i a,
    __m512i b) noexcept {
    return _mm512_maskz_shldi_epi64(mask, a, b, Imm8);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i vpshldvq(
    __m512i a,
    __m512i b,
    __m512i counts) noexcept {
    return _mm512_shldv_epi64(a, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i mask_vpshldvq(
    __m512i a,
    __mmask8 mask,
    __m512i b,
    __m512i counts) noexcept {
    return _mm512_mask_shldv_epi64(a, mask, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i maskz_vpshldvq(
    __mmask8 mask,
    __m512i a,
    __m512i b,
    __m512i counts) noexcept {
    return _mm512_maskz_shldv_epi64(mask, a, b, counts);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i vpshrdq(
    __m512i a,
    __m512i b) noexcept {
    return _mm512_shrdi_epi64(a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i mask_vpshrdq(
    __m512i source,
    __mmask8 mask,
    __m512i a,
    __m512i b) noexcept {
    return _mm512_mask_shrdi_epi64(source, mask, a, b, Imm8);
  }

  template<isa<x86> Arch, unsigned Imm8>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i maskz_vpshrdq(
    __mmask8 mask,
    __m512i a,
    __m512i b) noexcept {
    return _mm512_maskz_shrdi_epi64(mask, a, b, Imm8);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i vpshrdvq(
    __m512i a,
    __m512i b,
    __m512i counts) noexcept {
    return _mm512_shrdv_epi64(a, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i mask_vpshrdvq(
    __m512i a,
    __mmask8 mask,
    __m512i b,
    __m512i counts) noexcept {
    return _mm512_mask_shrdv_epi64(a, mask, b, counts);
  }

  template<isa<x86> Arch>
    requires(Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) &&
      Arch.has(x86_feature::avx512vbmi2))
  native_nodiscard native_inline native_const native_target("avx512f,avx512bw,avx512vbmi2")
  __m512i maskz_vpshrdvq(
    __mmask8 mask,
    __m512i a,
    __m512i b,
    __m512i counts) noexcept {
    return _mm512_maskz_shrdv_epi64(mask, a, b, counts);
  }
}
#endif

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace native::detail::x86_vbmi2_constant {
  template<bool Expand, class V>
  constexpr V compact(V source, std::uint64_t mask, V value) noexcept {
    std::array<typename V::value_type, V::lanes> input{}, result{};
    value.store(input.data());
    source.store(result.data());
    std::size_t packed = 0;
    for (std::size_t lane = 0; lane < V::lanes; ++lane) {
      if ((mask >> lane) & 1) {
        if constexpr (Expand) {
          result[lane] = input[packed++];
        } else {
          result[packed++] = input[lane];
        }
      }
    }
    return V::load(result.data());
  }

  template<class T, class V>
  constexpr void compress_store(T * destination, std::uint64_t mask, V value) noexcept {
    std::array<T, V::lanes> input{};
    value.store(input.data());
    std::size_t packed = 0;
    for (std::size_t lane = 0; lane < V::lanes; ++lane) {
      if ((mask >> lane) & 1) {
        destination[packed++] = input[lane];
      }
    }
  }

  template<class V, class T>
  constexpr V expand_load(V source, std::uint64_t mask, T * memory) noexcept {
    std::array<typename V::value_type, V::lanes> result{};
    source.store(result.data());
    std::size_t packed = 0;
    for (std::size_t lane = 0; lane < V::lanes; ++lane) {
      if ((mask >> lane) & 1) {
        result[lane] = memory[packed++];
      }
    }
    return V::load(result.data());
  }

  template<bool Right, class T>
  constexpr T shift_lane(T a, T b, unsigned count) noexcept {
    constexpr unsigned bits = sizeof(T) * 8;
    count &= bits - 1;
    if (!count) {
      return a;
    }
    // Widen narrow unsigned lanes before shifting to avoid signed promotions.
    auto first = static_cast<std::uint64_t>(a);
    auto second = static_cast<std::uint64_t>(b);
    if constexpr (Right) {
      return static_cast<T>((first >> count) | (second << (bits - count)));
    } else {
      return static_cast<T>((first << count) | (second >> (bits - count)));
    }
  }

  template<bool Right, class V, class C>
  constexpr V shift(V a, V b, C counts, V source, std::uint64_t mask) noexcept {
    using lane_type = typename V::value_type;
    std::array<lane_type, V::lanes> first{}, second{}, selectors{}, result{};
    a.store(first.data());
    b.store(second.data());
    source.store(result.data());
    if constexpr (!std::is_integral_v<C>) {
      counts.store(selectors.data());
    }
    for (std::size_t lane = 0; lane < V::lanes; ++lane) {
      if ((mask >> lane) & 1) {
        unsigned count;
        if constexpr (std::is_integral_v<C>) {
          count = counts;
        } else {
          count = static_cast<unsigned>(selectors[lane]);
        }
        result[lane] = shift_lane<Right>(first[lane], second[lane], count);
      }
    }
    return V::load(result.data());
  }
}
