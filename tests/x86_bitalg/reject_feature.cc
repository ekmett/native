// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/x86/bitalg.h>

#if NATIVE_TEST_WIDTH == 512
using register_type = __m512i;
using bmask = __mmask64;
using wmask = __mmask32;
#define NATIVE_TEST_CALLER_TARGET "avx512f,avx512bw,avx512bitalg"
#elif NATIVE_TEST_WIDTH == 256
using register_type = __m256i;
using bmask = __mmask32;
using wmask = __mmask16;
#define NATIVE_TEST_CALLER_TARGET "avx512f,avx512bw,avx512bitalg,avx512vl"
#elif NATIVE_TEST_WIDTH == 128
using register_type = __m128i;
using bmask = __mmask16;
using wmask = __mmask8;
#define NATIVE_TEST_CALLER_TARGET "avx512f,avx512bw,avx512bitalg,avx512vl"
#else
#error NATIVE_TEST_WIDTH must be 128, 256 or 512
#endif

// Structural feature omissions never change the caller's full compiler target.
constexpr auto requirements = [] {
  auto value = native::x86_feature::avx512f & native::x86_feature::avx512bw &
    native::x86_feature::avx512bitalg;
#if NATIVE_TEST_WIDTH != 512
  value.set(native::x86_feature::avx512vl, true);
#endif
#if NATIVE_TEST_FEATURE == 1
  value.set(native::x86_feature::avx512f, false);
#elif NATIVE_TEST_FEATURE == 2
  value.set(native::x86_feature::avx512bw, false);
#elif NATIVE_TEST_FEATURE == 3
  value.set(native::x86_feature::avx512bitalg, false);
#elif NATIVE_TEST_FEATURE == 4
  value.set(native::x86_feature::avx512vl, false);
#endif
  return value;
}();

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_vpopcntb(register_type value) noexcept {
  return native::detail::x86_bitalg::vpopcntb<requirements>(value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpopcntb(register_type source, bmask mask, register_type value) noexcept {
  return native::detail::x86_bitalg::mask_vpopcntb<requirements>(source, mask, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpopcntb(bmask mask, register_type value) noexcept {
  return native::detail::x86_bitalg::maskz_vpopcntb<requirements>(mask, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_vpopcntw(register_type value) noexcept {
  return native::detail::x86_bitalg::vpopcntw<requirements>(value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpopcntw(register_type source, wmask mask, register_type value) noexcept {
  return native::detail::x86_bitalg::mask_vpopcntw<requirements>(source, mask, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpopcntw(wmask mask, register_type value) noexcept {
  return native::detail::x86_bitalg::maskz_vpopcntw<requirements>(mask, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
bmask reject_vpshufbitqmb(register_type value, register_type control) noexcept {
  return native::detail::x86_bitalg::vpshufbitqmb<requirements>(value, control);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
bmask reject_mask_vpshufbitqmb(bmask mask, register_type value, register_type control) noexcept {
  return native::detail::x86_bitalg::mask_vpshufbitqmb<requirements>(mask, value, control);
}
