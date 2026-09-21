// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/x86/vbmi.h>

#if NATIVE_TEST_WIDTH == 512
using register_type = __m512i;
using bmask = __mmask64;
#define NATIVE_TEST_CALLER_TARGET "avx512f,avx512bw,avx512vbmi"
#elif NATIVE_TEST_WIDTH == 256
using register_type = __m256i;
using bmask = __mmask32;
#define NATIVE_TEST_CALLER_TARGET "avx512f,avx512bw,avx512vbmi,avx512vl"
#elif NATIVE_TEST_WIDTH == 128
using register_type = __m128i;
using bmask = __mmask16;
#define NATIVE_TEST_CALLER_TARGET "avx512f,avx512bw,avx512vbmi,avx512vl"
#else
#error NATIVE_TEST_WIDTH must be 128, 256 or 512
#endif

// Structural feature omissions never change the caller's full compiler target.
constexpr auto requirements = [] {
  auto value = native::x86_feature::avx512f & native::x86_feature::avx512bw &
    native::x86_feature::avx512vbmi;
#if NATIVE_TEST_WIDTH != 512
  value.set(native::x86_feature::avx512vl, true);
#endif
#if NATIVE_TEST_FEATURE == 1
  value.set(native::x86_feature::avx512f, false);
#elif NATIVE_TEST_FEATURE == 2
  value.set(native::x86_feature::avx512bw, false);
#elif NATIVE_TEST_FEATURE == 3
  value.set(native::x86_feature::avx512vbmi, false);
#elif NATIVE_TEST_FEATURE == 4
  value.set(native::x86_feature::avx512vl, false);
#endif
  return value;
}();

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_vpermb(register_type indices, register_type value) noexcept {
  return native::detail::x86_vbmi::vpermb<requirements>(indices, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpermb(
  register_type source,
  bmask mask,
  register_type indices,
  register_type value) noexcept {
  return native::detail::x86_vbmi::mask_vpermb<requirements>(source, mask, indices, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpermb(bmask mask, register_type indices, register_type value) noexcept {
  return native::detail::x86_vbmi::maskz_vpermb<requirements>(mask, indices, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_vpermt2b(register_type a, register_type indices, register_type b) noexcept {
  return native::detail::x86_vbmi::vpermt2b<requirements>(a, indices, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpermt2b(
  register_type a,
  bmask mask,
  register_type indices,
  register_type b) noexcept {
  return native::detail::x86_vbmi::mask_vpermt2b<requirements>(a, mask, indices, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpermt2b(
  bmask mask,
  register_type a,
  register_type indices,
  register_type b) noexcept {
  return native::detail::x86_vbmi::maskz_vpermt2b<requirements>(mask, a, indices, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_vpermi2b(register_type indices, register_type a, register_type b) noexcept {
  return native::detail::x86_vbmi::vpermi2b<requirements>(indices, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpermi2b(
  register_type indices,
  bmask mask,
  register_type a,
  register_type b) noexcept {
  return native::detail::x86_vbmi::mask_vpermi2b<requirements>(indices, mask, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpermi2b(
  bmask mask,
  register_type indices,
  register_type a,
  register_type b) noexcept {
  return native::detail::x86_vbmi::maskz_vpermi2b<requirements>(mask, indices, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_vpmultishiftqb(register_type control, register_type value) noexcept {
  return native::detail::x86_vbmi::vpmultishiftqb<requirements>(control, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpmultishiftqb(
  register_type source,
  bmask mask,
  register_type control,
  register_type value) noexcept {
  return native::detail::x86_vbmi::mask_vpmultishiftqb<requirements>(source, mask, control, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpmultishiftqb(bmask mask, register_type control, register_type value) noexcept {
  return native::detail::x86_vbmi::maskz_vpmultishiftqb<requirements>(mask, control, value);
}

