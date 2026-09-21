// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <native/attributes.h>
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

#define NATIVE_TARGET_codegen_crc32c "crc32"
constexpr auto codegen_requirements = NATIVE_TARGET_ISA(codegen_crc32c);
static_assert(codegen_requirements == native::isa(native::x86_feature::crc32));
NATIVE_TARGET_PUSH(codegen_crc32c)
extern "C" native_noinline std::uint32_t native_crc32c_u8(
    std::uint32_t accumulator, std::uint8_t value) noexcept {
  return native::crc32c<codegen_requirements>(accumulator, value);
}
extern "C" native_noinline std::uint32_t native_crc32c_u16(
    std::uint32_t accumulator, std::uint16_t value) noexcept {
  return native::crc32c<codegen_requirements>(accumulator, value);
}
extern "C" native_noinline std::uint32_t native_crc32c_u32(
    std::uint32_t accumulator, std::uint32_t value) noexcept {
  return native::crc32c<codegen_requirements>(accumulator, value);
}
#if defined(__x86_64__) || defined(_M_X64)
extern "C" native_noinline std::uint32_t native_crc32c_u64(
    std::uint32_t accumulator, std::uint64_t value) noexcept {
  return native::crc32c<codegen_requirements>(accumulator, value);
}
#endif
NATIVE_TARGET_POP()

// An ordinary importer must retain its baseline after the attributed functions.
extern "C" native_noinline std::uint32_t native_crc32c_baseline(
    std::uint32_t accumulator, std::uint32_t value) noexcept {
  return accumulator ^ value;
}
