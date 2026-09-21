// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/x86/avx512cd.h>

// Missing-feature configurations change only the structural requirements.
// Every caller retains the full literal target and an identical positive control.
#if NATIVE_TEST_WIDTH == 512
using register_type = __m512i;
using dmask_type = __mmask16;
#define NATIVE_TEST_CALLER_TARGET "avx512f,avx512cd"
#elif NATIVE_TEST_WIDTH == 256
using register_type = __m256i;
using dmask_type = __mmask8;
#define NATIVE_TEST_CALLER_TARGET "avx512f,avx512cd,avx512vl"
#elif NATIVE_TEST_WIDTH == 128
using register_type = __m128i;
using dmask_type = __mmask8;
#define NATIVE_TEST_CALLER_TARGET "avx512f,avx512cd,avx512vl"
#else
#error NATIVE_TEST_WIDTH must be 128, 256 or 512
#endif

constexpr auto requirements = [] {
  auto value = native::x86_feature::avx512f & native::x86_feature::avx512cd;
#if NATIVE_TEST_WIDTH != 512
  value.set(native::x86_feature::avx512vl, true);
#endif
#if NATIVE_TEST_FEATURE == 1
  value.set(native::x86_feature::avx512f, false);
#elif NATIVE_TEST_FEATURE == 2
  value.set(native::x86_feature::avx512cd, false);
#elif NATIVE_TEST_FEATURE == 3
  value.set(native::x86_feature::avx512vl, false);
#endif
  return value;
}();

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_vpconflictd(register_type value) noexcept {
  return native::detail::x86_avx512cd::vpconflictd<requirements>(value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpconflictd(register_type source, dmask_type mask, register_type value) noexcept {
  return native::detail::x86_avx512cd::mask_vpconflictd<requirements>(source, mask, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpconflictd(dmask_type mask, register_type value) noexcept {
  return native::detail::x86_avx512cd::maskz_vpconflictd<requirements>(mask, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_vplzcntd(register_type value) noexcept {
  return native::detail::x86_avx512cd::vplzcntd<requirements>(value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vplzcntd(register_type source, dmask_type mask, register_type value) noexcept {
  return native::detail::x86_avx512cd::mask_vplzcntd<requirements>(source, mask, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vplzcntd(dmask_type mask, register_type value) noexcept {
  return native::detail::x86_avx512cd::maskz_vplzcntd<requirements>(mask, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_vpconflictq(register_type value) noexcept {
  return native::detail::x86_avx512cd::vpconflictq<requirements>(value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpconflictq(register_type source, __mmask8 mask, register_type value) noexcept {
  return native::detail::x86_avx512cd::mask_vpconflictq<requirements>(source, mask, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpconflictq(__mmask8 mask, register_type value) noexcept {
  return native::detail::x86_avx512cd::maskz_vpconflictq<requirements>(mask, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_vplzcntq(register_type value) noexcept {
  return native::detail::x86_avx512cd::vplzcntq<requirements>(value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vplzcntq(register_type source, __mmask8 mask, register_type value) noexcept {
  return native::detail::x86_avx512cd::mask_vplzcntq<requirements>(source, mask, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vplzcntq(__mmask8 mask, register_type value) noexcept {
  return native::detail::x86_avx512cd::maskz_vplzcntq<requirements>(mask, value);
}
