// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <immintrin.h>
#include <native/attributes.h>
import native.x86.sha;
#if NATIVE_TEST_WIDTH == 128
#define NATIVE_TEST_FULL_TARGET "sha"
constexpr auto full_arch = native::target_features<native::x86>(NATIVE_TEST_FULL_TARGET);
constexpr auto storage_arch = native::target_features<native::x86>("sse2");
using register_type = __m128i;
constexpr auto requirements = [] {
  auto value = full_arch;
#if NATIVE_TEST_FEATURE == 1
  value.set(native::x86_feature::sha, false);
#endif
  return value;
}();
#if NATIVE_TEST_TARGET == 1
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-sha"
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
using vector = native::simd<std::int32_t, 4, arch>;
#elif NATIVE_TEST_SHAPE == 3
using vector = native::simd<std::uint32_t, 4, arch & native::x86_feature::gfni>;
#elif NATIVE_TEST_SHAPE == 4
using vector = native::simd<std::uint8_t, 16, arch>;
#else
using vector = native::simd<std::uint32_t, 4, arch>;
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 0
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_sha1rnds4_0(vector a, vector b) noexcept {
  return native::sha1rnds4<arch, 0>(a, b);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 1
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_sha1rnds4_1(vector a, vector b) noexcept {
  return native::sha1rnds4<arch, 1>(a, b);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 2
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_sha1rnds4_2(vector a, vector b) noexcept {
  return native::sha1rnds4<arch, 2>(a, b);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 3
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_sha1rnds4_3(vector a, vector b) noexcept {
  return native::sha1rnds4<arch, 3>(a, b);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 4
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_sha1nexte(vector a, vector b) noexcept {
  return native::sha1nexte<arch>(a, b);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 5
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_sha1msg1(vector a, vector b) noexcept {
  return native::sha1msg1<arch>(a, b);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 6
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_sha1msg2(vector a, vector b) noexcept {
  return native::sha1msg2<arch>(a, b);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 7
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_sha256rnds2(vector a, vector b, vector c) noexcept {
  return native::sha256rnds2<arch>(a, b, c);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 8
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_sha256msg1(vector a, vector b) noexcept {
  return native::sha256msg1<arch>(a, b);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 9
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_sha256msg2(vector a, vector b) noexcept {
  return native::sha256msg2<arch>(a, b);
}
#endif
