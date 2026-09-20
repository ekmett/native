// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/x86/lzcnt.h>
#include <native/targets.h>
#define NATIVE_TARGET_codegen_lzcnt "lzcnt"
constexpr auto codegen_requirements = NATIVE_TARGET_ISA(codegen_lzcnt);
NATIVE_TARGET_PUSH(codegen_lzcnt)
extern "C" native_noinline std::uint16_t native_lzcnt_u16(std::uint16_t value) noexcept {
  return native::lzcnt<codegen_requirements>(value);
}
extern "C" native_noinline std::uint32_t native_lzcnt_u32(std::uint32_t value) noexcept {
  return native::lzcnt<codegen_requirements>(value);
}
extern "C" native_noinline std::uint64_t native_lzcnt_u64(std::uint64_t value) noexcept {
  return native::lzcnt<codegen_requirements>(value);
}
NATIVE_TARGET_POP()
