// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/x86/vpopcntdq.h>

#if defined(__AVX__) || defined(__AVX2__) || defined(__AVX512F__) || \
    defined(__AVX512VL__) || defined(__AVX512DQ__) || defined(__AVX512BW__) || \
    defined(__AVX512CD__) || defined(__AVX512VPOPCNTDQ__) || \
    defined(__AVX512BITALG__) || defined(__POPCNT__) || defined(__FMA__)
#error Optional instructions must come from function targets, not translation-unit flags
#endif

constexpr auto count512 = native::x86_feature::avx512f &
                          native::x86_feature::avx512vpopcntdq;
constexpr auto countvl = count512 & native::x86_feature::avx512vl;

#define NATIVE_COUNT_FIXTURES(width, reg, dmask, requirement, features) \
  extern "C" native_noinline native_target(features) \
  reg native_vpopcntd_##width##_plain(reg value) noexcept { \
    return native::vpopcntd<requirement>(value); \
  } \
  extern "C" native_noinline native_target(features) \
  reg native_vpopcntd_##width##_merge(reg source, dmask mask, reg value) noexcept { \
    return native::mask_vpopcntd<requirement>(source, mask, value); \
  } \
  extern "C" native_noinline native_target(features) \
  reg native_vpopcntd_##width##_zero(dmask mask, reg value) noexcept { \
    return native::maskz_vpopcntd<requirement>(mask, value); \
  } \
  extern "C" native_noinline native_target(features) \
  reg native_vpopcntq_##width##_plain(reg value) noexcept { \
    return native::vpopcntq<requirement>(value); \
  } \
  extern "C" native_noinline native_target(features) \
  reg native_vpopcntq_##width##_merge(reg source, __mmask8 mask, reg value) noexcept { \
    return native::mask_vpopcntq<requirement>(source, mask, value); \
  } \
  extern "C" native_noinline native_target(features) \
  reg native_vpopcntq_##width##_zero(__mmask8 mask, reg value) noexcept { \
    return native::maskz_vpopcntq<requirement>(mask, value); \
  }

NATIVE_COUNT_FIXTURES(128, __m128i, __mmask8, countvl,
                      "avx512f,avx512vpopcntdq,avx512vl")
NATIVE_COUNT_FIXTURES(256, __m256i, __mmask8, countvl,
                      "avx512f,avx512vpopcntdq,avx512vl")
NATIVE_COUNT_FIXTURES(512, __m512i, __mmask16, count512,
                      "avx512f,avx512vpopcntdq")
#undef NATIVE_COUNT_FIXTURES

// An ordinary function in the same translation unit retains its baseline.
extern "C" native_noinline unsigned long long
native_vpopcntdq_baseline(unsigned long long value) noexcept {
  return (value >> 3) ^ (value + 17);
}
