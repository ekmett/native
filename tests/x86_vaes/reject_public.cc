// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <immintrin.h>
#include <native/attributes.h>
import native.x86.vaes;
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
#if NATIVE_TEST_SHAPE == 1
using vector = register_type;
#elif NATIVE_TEST_SHAPE == 2
using vector = native::simd<std::int8_t, NATIVE_TEST_WIDTH / 8, arch>;
#elif NATIVE_TEST_SHAPE == 3
using vector = native::simd<std::uint8_t, NATIVE_TEST_WIDTH / 8, arch & native::x86_feature::gfni>;
#elif NATIVE_TEST_SHAPE == 4
using vector = native::simd<std::uint32_t, NATIVE_TEST_WIDTH / 32, arch>;
#else
using vector = native::simd<std::uint8_t, NATIVE_TEST_WIDTH / 8, arch>;
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 0
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vaesenc(vector a, vector b) noexcept {
  return native::vaesenc<arch>(a, b);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 1
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vaesenclast(vector a, vector b) noexcept {
  return native::vaesenclast<arch>(a, b);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 2
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vaesdec(vector a, vector b) noexcept {
  return native::vaesdec<arch>(a, b);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 3
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vaesdeclast(vector a, vector b) noexcept {
  return native::vaesdeclast<arch>(a, b);
}
#endif
