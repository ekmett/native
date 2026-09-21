// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/x86/gfni.h>

#if defined(__GFNI__) || defined(__AVX__) || defined(__AVX2__) || defined(__AVX512F__) || defined(__AVX512BW__) || defined(__AVX512VL__)
#error GFNI codegen probes require a baseline translation unit
#endif

constexpr native::isa gfni128{native::x86_feature::gfni};
constexpr auto gfni256 = gfni128 & native::x86_feature::avx;
constexpr auto gfni512 = gfni128 & native::x86_feature::avx512f;
constexpr auto gfni_mask512 = gfni512 & native::x86_feature::avx512bw;
constexpr auto gfni_maskvl = gfni_mask512 & native::x86_feature::avx512vl;
static_assert(!gfni256.has(native::x86_feature::avx2));

// Keep arguments unknown so each instruction, immediate and byte predicate is
// visible independently of constant folding. Target strings establish the exact
// LLVM requirements even when every optional global compiler feature is disabled.
#define GFNI_UNMASKED(width, type, arch, target_string) \
  extern "C" native_noinline native_target(target_string) \
  type native_gfni_mul##width(type a, type b) noexcept { \
    return native::detail::x86_gfni::gf2p8mulb<arch>(a, b); \
  } \
  extern "C" native_noinline native_target(target_string) \
  type native_gfni_affine##width(type a, type matrix) noexcept { \
    return native::detail::x86_gfni::gf2p8affineqb<arch, 0x63>(a, matrix); \
  } \
  extern "C" native_noinline native_target(target_string) \
  type native_gfni_inverse##width(type a, type matrix) noexcept { \
    return native::detail::x86_gfni::gf2p8affineinvqb<arch, 0xa5>(a, matrix); \
  }

GFNI_UNMASKED(128, __m128i, gfni128, "gfni,no-avx")
GFNI_UNMASKED(256, __m256i, gfni256, "avx,gfni,no-avx2")
GFNI_UNMASKED(512, __m512i, gfni512, "avx512f,gfni")
#undef GFNI_UNMASKED

#define GFNI_MASKED(width, type, mask_type, arch, target_string) \
  extern "C" native_noinline native_target(target_string) \
  type native_gfni_mul_mask##width(type src, mask_type k, type a, type b) noexcept { \
    return native::detail::x86_gfni::gf2p8mulb_mask<arch>(src, k, a, b); \
  } \
  extern "C" native_noinline native_target(target_string) \
  type native_gfni_mul_maskz##width(mask_type k, type a, type b) noexcept { \
    return native::detail::x86_gfni::gf2p8mulb_maskz<arch>(k, a, b); \
  } \
  extern "C" native_noinline native_target(target_string) \
  type native_gfni_affine_mask##width(type src, mask_type k, type a, type matrix) noexcept { \
    return native::detail::x86_gfni::gf2p8affineqb_mask<arch, 0x63>(src, k, a, matrix); \
  } \
  extern "C" native_noinline native_target(target_string) \
  type native_gfni_affine_maskz##width(mask_type k, type a, type matrix) noexcept { \
    return native::detail::x86_gfni::gf2p8affineqb_maskz<arch, 0x63>(k, a, matrix); \
  } \
  extern "C" native_noinline native_target(target_string) \
  type native_gfni_inverse_mask##width(type src, mask_type k, type a, type matrix) noexcept { \
    return native::detail::x86_gfni::gf2p8affineinvqb_mask<arch, 0xa5>(src, k, a, matrix); \
  } \
  extern "C" native_noinline native_target(target_string) \
  type native_gfni_inverse_maskz##width(mask_type k, type a, type matrix) noexcept { \
    return native::detail::x86_gfni::gf2p8affineinvqb_maskz<arch, 0xa5>(k, a, matrix); \
  }

GFNI_MASKED(128, __m128i, __mmask16, gfni_maskvl, "avx512bw,avx512vl,gfni")
GFNI_MASKED(256, __m256i, __mmask32, gfni_maskvl, "avx512bw,avx512vl,gfni")
GFNI_MASKED(512, __m512i, __mmask64, gfni_mask512, "avx512bw,gfni")
#undef GFNI_MASKED
