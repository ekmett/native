// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/x86/popcnt.h>
#include <native/targets.h>
#define NATIVE_TARGET_codegen_popcnt "popcnt"
constexpr auto codegen_requirements = NATIVE_TARGET_ISA(codegen_popcnt);
NATIVE_TARGET_PUSH(codegen_popcnt)
extern "C" native_noinline std::uint16_t native_popcnt_u16(std::uint16_t value) noexcept {
  return native::popcnt<codegen_requirements>(value);
}
extern "C" native_noinline std::uint32_t native_popcnt_u32(std::uint32_t value) noexcept {
  return native::popcnt<codegen_requirements>(value);
}
extern "C" native_noinline std::uint64_t native_popcnt_u64(std::uint64_t value) noexcept {
  return native::popcnt<codegen_requirements>(value);
}
NATIVE_TARGET_POP()
