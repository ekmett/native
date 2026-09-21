// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <immintrin.h>
#include <native/attributes.h>
#include <native/x86/vaes.h>
#if NATIVE_TEST_WIDTH == 128
#define NATIVE_TEST_FULL_TARGET "avx,aes"
constexpr auto full_arch = native::target_features<native::x86>(NATIVE_TEST_FULL_TARGET);
constexpr auto storage_arch = native::target_features<native::x86>("sse2");
using register_type = __m128i;
constexpr auto requirements = [] {
  auto value = full_arch;
#if NATIVE_TEST_FEATURE == 1
  value.set(native::x86_feature::aes, false);
#elif NATIVE_TEST_FEATURE == 2
  value.set(native::x86_feature::avx, false);
#endif
  return value;
}();
#if NATIVE_TEST_TARGET == 1
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-aes"
#elif NATIVE_TEST_TARGET == 2
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx"
#else
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET
#endif
#elif NATIVE_TEST_WIDTH == 256
#define NATIVE_TEST_FULL_TARGET "avx,vaes"
constexpr auto full_arch = native::target_features<native::x86>(NATIVE_TEST_FULL_TARGET);
constexpr auto storage_arch = native::target_features<native::x86>("avx");
using register_type = __m256i;
constexpr auto requirements = [] {
  auto value = full_arch;
#if NATIVE_TEST_FEATURE == 1
  value.set(native::x86_feature::vaes, false);
#elif NATIVE_TEST_FEATURE == 2
  value.set(native::x86_feature::avx, false);
#endif
  return value;
}();
#if NATIVE_TEST_TARGET == 1
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-vaes"
#elif NATIVE_TEST_TARGET == 2
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx"
#else
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET
#endif
#elif NATIVE_TEST_WIDTH == 512
#define NATIVE_TEST_FULL_TARGET "avx512f,vaes"
constexpr auto full_arch = native::target_features<native::x86>(NATIVE_TEST_FULL_TARGET);
constexpr auto storage_arch = native::target_features<native::x86>("avx512f");
using register_type = __m512i;
constexpr auto requirements = [] {
  auto value = full_arch;
#if NATIVE_TEST_FEATURE == 1
  value.set(native::x86_feature::vaes, false);
#elif NATIVE_TEST_FEATURE == 2
  value.set(native::x86_feature::avx512f, false);
#endif
  return value;
}();
#if NATIVE_TEST_TARGET == 1
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-vaes"
#elif NATIVE_TEST_TARGET == 2
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512f"
#else
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET
#endif
#endif

#if NATIVE_TEST_WEAK
constexpr auto arch = storage_arch;
#else
constexpr auto arch = full_arch;
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 0
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vaesenc(register_type a, register_type b) noexcept {
  return native::detail::x86_vaes::vaesenc<requirements>(a, b);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 1
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vaesenclast(register_type a, register_type b) noexcept {
  return native::detail::x86_vaes::vaesenclast<requirements>(a, b);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 2
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vaesdec(register_type a, register_type b) noexcept {
  return native::detail::x86_vaes::vaesdec<requirements>(a, b);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 3
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vaesdeclast(register_type a, register_type b) noexcept {
  return native::detail::x86_vaes::vaesdeclast<requirements>(a, b);
}
#endif
