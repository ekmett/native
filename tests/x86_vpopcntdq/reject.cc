// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/x86/vpopcntdq.h>

// Each negative has a successful build of this source with identical width
// and complete feature requirements as its positive control.
#if NATIVE_TEST_WIDTH == 512
using register_type = __m512i;
using dmask_type = __mmask16;
#define NATIVE_TEST_FULL_TARGET "avx512f,avx512vpopcntdq"
#elif NATIVE_TEST_WIDTH == 256
using register_type = __m256i;
using dmask_type = __mmask8;
#define NATIVE_TEST_FULL_TARGET "avx512f,avx512vpopcntdq,avx512vl"
#elif NATIVE_TEST_WIDTH == 128
using register_type = __m128i;
using dmask_type = __mmask8;
#define NATIVE_TEST_FULL_TARGET "avx512f,avx512vpopcntdq,avx512vl"
#else
#error NATIVE_TEST_WIDTH must be 128, 256 or 512
#endif

constexpr auto requirements = [] {
  auto value = native::x86_feature::avx512f & native::x86_feature::avx512vpopcntdq;
#if NATIVE_TEST_WIDTH != 512
  value.set(native::x86_feature::avx512vl, true);
#endif
#if NATIVE_TEST_FEATURE == 1
  value.set(native::x86_feature::avx512f, false);
#elif NATIVE_TEST_FEATURE == 2
  value.set(native::x86_feature::avx512vpopcntdq, false);
#elif NATIVE_TEST_FEATURE == 3
  value.set(native::x86_feature::avx512vl, false);
#endif
  return value;
}();

#if NATIVE_TEST_TARGET == 1
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512f"
#elif NATIVE_TEST_TARGET == 2
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512vpopcntdq"
#elif NATIVE_TEST_TARGET == 3
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512vl"
#elif NATIVE_TEST_TARGET == 4
#define NATIVE_TEST_CALLER_TARGET "sse2,no-avx,no-avx512f,no-avx512vpopcntdq,no-avx512vl"
#else
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 0
extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_d(register_type value) noexcept {
  return native::vpopcntd<requirements>(value);
}
#endif
#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 1
extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_d(register_type source, dmask_type mask,
                            register_type value) noexcept {
  return native::mask_vpopcntd<requirements>(source, mask, value);
}
#endif
#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 2
extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_d(dmask_type mask, register_type value) noexcept {
  return native::maskz_vpopcntd<requirements>(mask, value);
}
#endif
#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 3
extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_q(register_type value) noexcept {
  return native::vpopcntq<requirements>(value);
}
#endif
#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 4
extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_q(register_type source, __mmask8 mask,
                            register_type value) noexcept {
  return native::mask_vpopcntq<requirements>(source, mask, value);
}
#endif
#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 5
extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_q(__mmask8 mask, register_type value) noexcept {
  return native::maskz_vpopcntq<requirements>(mask, value);
}
#endif
