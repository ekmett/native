// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <immintrin.h>
#include <native/attributes.h>
#include <native/x86/sha.h>
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

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 0
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_sha1rnds4_0(register_type a, register_type b) noexcept {
  return native::detail::x86_sha::sha1rnds4<requirements, 0>(a, b);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 1
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_sha1rnds4_1(register_type a, register_type b) noexcept {
  return native::detail::x86_sha::sha1rnds4<requirements, 1>(a, b);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 2
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_sha1rnds4_2(register_type a, register_type b) noexcept {
  return native::detail::x86_sha::sha1rnds4<requirements, 2>(a, b);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 3
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_sha1rnds4_3(register_type a, register_type b) noexcept {
  return native::detail::x86_sha::sha1rnds4<requirements, 3>(a, b);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 4
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_sha1nexte(register_type a, register_type b) noexcept {
  return native::detail::x86_sha::sha1nexte<requirements>(a, b);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 5
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_sha1msg1(register_type a, register_type b) noexcept {
  return native::detail::x86_sha::sha1msg1<requirements>(a, b);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 6
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_sha1msg2(register_type a, register_type b) noexcept {
  return native::detail::x86_sha::sha1msg2<requirements>(a, b);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 7
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_sha256rnds2(register_type a, register_type b, register_type c) noexcept {
  return native::detail::x86_sha::sha256rnds2<requirements>(a, b, c);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 8
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_sha256msg1(register_type a, register_type b) noexcept {
  return native::detail::x86_sha::sha256msg1<requirements>(a, b);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 9
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_sha256msg2(register_type a, register_type b) noexcept {
  return native::detail::x86_sha::sha256msg2<requirements>(a, b);
}
#endif
