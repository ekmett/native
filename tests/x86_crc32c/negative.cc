// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <native/targets.h>
#if NATIVE_TEST_INTERFACE == 0
#include <native/x86/crc32c.h>
#elif NATIVE_TEST_INTERFACE == 1
import native.x86.crc32c;
#elif NATIVE_TEST_INTERFACE == 2
import native;
#elif NATIVE_TEST_INTERFACE == 3
import native.x86;
#else
#error Select the header, direct module or main hub test interface.
#endif

#if NATIVE_TEST_NEGATIVE_FEATURE
#define NATIVE_TARGET_negative_crc32c "crc32"
NATIVE_TARGET_PUSH(negative_crc32c)
constexpr auto negative_requirements = native::isa(native::x86_feature::popcnt);
#else
#ifdef __CRC32__
#error NATIVE_TARGET_NEGATIVE_UNAVAILABLE
#endif
constexpr auto negative_requirements = native::isa(native::x86_feature::crc32);
#endif

#if NATIVE_TEST_WIDTH == 8
using operand = std::uint8_t;
#elif NATIVE_TEST_WIDTH == 16
using operand = std::uint16_t;
#elif NATIVE_TEST_WIDTH == 32
using operand = std::uint32_t;
#elif NATIVE_TEST_WIDTH == 64 && (defined(__x86_64__) || defined(_M_X64))
using operand = std::uint64_t;
#else
#error Select an available CRC32C operand width.
#endif

std::uint32_t rejected(std::uint32_t accumulator, operand value) {
  return native::crc32c<negative_requirements>(accumulator, value);
}
#if NATIVE_TEST_NEGATIVE_FEATURE
NATIVE_TARGET_POP()
#endif
