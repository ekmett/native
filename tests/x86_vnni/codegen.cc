// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/x86/vnni.h>

#if defined(__AVX__) || defined(__AVX2__) || defined(__AVX512F__) || \
    defined(__AVX512VL__) || defined(__AVXVNNI__) || defined(__AVX512VNNI__) || \
    defined(__AVXVNNIINT8__) || defined(__AVXVNNIINT16__)
#error VNNI codegen probes require a baseline translation unit
#endif

constexpr native::isa vex{native::x86_feature::avxvnni};
constexpr auto evex = native::x86_feature::avx512f & native::x86_feature::avx512vnni;
constexpr auto evexvl = evex & native::x86_feature::avx512vl;
constexpr auto both = vex & evexvl;
constexpr native::isa int8{native::x86_feature::avxvnniint8};
constexpr native::isa int16{native::x86_feature::avxvnniint16};

#define NATIVE_VNNI_PLAIN(operation, width, reg, family, requirement, features) \
  extern "C" native_noinline native_target(features) \
  reg native_vnni_##family##_##operation##_##width##_plain( \
      reg acc, reg a, reg b) noexcept { \
    return native::operation<requirement>(acc, a, b); \
  }

#define NATIVE_VNNI_MASKED(operation, width, reg, mask, requirement, features) \
  NATIVE_VNNI_PLAIN(operation, width, reg, evex, requirement, features) \
  extern "C" native_noinline native_target(features) \
  reg native_vnni_evex_##operation##_##width##_merge( \
      reg acc, unsigned k, reg a, reg b) noexcept { \
    return native::mask_##operation<requirement>(acc, k, a, b); \
  } \
  extern "C" native_noinline native_target(features) \
  reg native_vnni_evex_##operation##_##width##_zero( \
      unsigned k, reg acc, reg a, reg b) noexcept { \
    return native::maskz_##operation<requirement>(k, acc, a, b); \
  }

#define NATIVE_VNNI_CORE(operation) \
  NATIVE_VNNI_PLAIN(operation, 128, __m128i, vex, vex, "avxvnni,no-avx512f") \
  NATIVE_VNNI_PLAIN(operation, 256, __m256i, vex, vex, "avxvnni,no-avx512f") \
  NATIVE_VNNI_PLAIN(operation, 128, __m128i, priority, both, "avxvnni,no-avx512f") \
  NATIVE_VNNI_PLAIN(operation, 256, __m256i, priority, both, "avxvnni,no-avx512f") \
  NATIVE_VNNI_MASKED(operation, 128, __m128i, __mmask8, evexvl, \
                     "avx512f,avx512vnni,avx512vl,no-avxvnni") \
  NATIVE_VNNI_MASKED(operation, 256, __m256i, __mmask8, evexvl, \
                     "avx512f,avx512vnni,avx512vl,no-avxvnni") \
  NATIVE_VNNI_MASKED(operation, 512, __m512i, __mmask16, evex, \
                     "avx512f,avx512vnni,no-avxvnni")

NATIVE_VNNI_CORE(dpbusd)
NATIVE_VNNI_CORE(dpbusds)
NATIVE_VNNI_CORE(dpwssd)
NATIVE_VNNI_CORE(dpwssds)

#define NATIVE_VNNI_EXTENSION(operation, family, features) \
  NATIVE_VNNI_PLAIN(operation, 128, __m128i, family, family, features) \
  NATIVE_VNNI_PLAIN(operation, 256, __m256i, family, family, features)

NATIVE_VNNI_EXTENSION(dpbssd, int8, "avxvnniint8,no-avx512f")
NATIVE_VNNI_EXTENSION(dpbssds, int8, "avxvnniint8,no-avx512f")
NATIVE_VNNI_EXTENSION(dpbsud, int8, "avxvnniint8,no-avx512f")
NATIVE_VNNI_EXTENSION(dpbsuds, int8, "avxvnniint8,no-avx512f")
NATIVE_VNNI_EXTENSION(dpbuud, int8, "avxvnniint8,no-avx512f")
NATIVE_VNNI_EXTENSION(dpbuuds, int8, "avxvnniint8,no-avx512f")
NATIVE_VNNI_EXTENSION(dpwsud, int16, "avxvnniint16,no-avx512f")
NATIVE_VNNI_EXTENSION(dpwsuds, int16, "avxvnniint16,no-avx512f")
NATIVE_VNNI_EXTENSION(dpwusd, int16, "avxvnniint16,no-avx512f")
NATIVE_VNNI_EXTENSION(dpwusds, int16, "avxvnniint16,no-avx512f")
NATIVE_VNNI_EXTENSION(dpwuud, int16, "avxvnniint16,no-avx512f")
NATIVE_VNNI_EXTENSION(dpwuuds, int16, "avxvnniint16,no-avx512f")

#undef NATIVE_VNNI_EXTENSION
#undef NATIVE_VNNI_CORE
#undef NATIVE_VNNI_MASKED
#undef NATIVE_VNNI_PLAIN

extern "C" native_noinline unsigned long long
native_vnni_baseline(unsigned long long value) noexcept {
  return (value >> 3) ^ (value + 17);
}
