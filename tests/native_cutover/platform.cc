// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/targets.h>
#include <concepts>
#include <cstdint>

#if defined(__x86_64__) || defined(_M_X64)
import native.x86;

#define NATIVE_TARGET_platform_bmi2 "bmi2"
constexpr auto requirements = NATIVE_TARGET_ISA(platform_bmi2);
static_assert(requirements.has(native::x86_feature::bmi2));
static_assert(!requirements.has(native::x86_feature::avx2));

template<native::isa A, class T> concept has_bmi2 = requires(T value, T mask) {
  { native::pdep<A>(value, mask) } noexcept -> std::same_as<T>;
  { native::pext<A>(value, mask) } noexcept -> std::same_as<T>;
};
static_assert(has_bmi2<requirements, std::uint32_t>);
static_assert(has_bmi2<requirements, std::uint64_t>);
static_assert(!has_bmi2<native::scalar, std::uint32_t>);
static_assert(!has_bmi2<native::scalar, std::uint64_t>);

NATIVE_TARGET_PUSH(platform_bmi2)
static bool round_trip(std::uint32_t source32, std::uint64_t source64) {
  constexpr std::uint32_t mask32 = 0x80010102;
  constexpr std::uint64_t mask64 = 0x8000000100010102;
  auto deposited32 = native::pdep<requirements>(source32, mask32);
  auto deposited64 = native::pdep<requirements>(source64, mask64);
  return native::pext<requirements>(deposited32, mask32) == (source32 & 0xf) &&
         native::pext<requirements>(deposited64, mask64) == (source64 & 0x1f);
}
NATIVE_TARGET_POP()

int main(int argc, char **) {
  auto cpu = native::observe_x86_capabilities();
  if (!native::classify_isa(cpu, native::scalar).admitted()) return 1;
  if (!native::classify_isa(cpu, requirements).admitted()) return 0;
  // Runtime inputs prevent the entire operation check becoming a constant.
  auto seed = static_cast<std::uint32_t>(argc);
  return round_trip(seed ^ 0x95u, std::uint64_t(seed) ^ 0x100000016ull) ? 0 : 2;
}

#elif defined(__aarch64__) || defined(_M_ARM64)
import native.arm;

#define NATIVE_TARGET_platform_neon "neon"
constexpr auto requirements = NATIVE_TARGET_ISA(platform_neon);
static_assert(requirements.has(native::arm_feature::neon));
static_assert(native::scalar <= requirements);
static_assert(!requirements.has(native::x86_feature::bmi2));

int main() {
  auto cpu = native::observe_arm_capabilities();
  if (!native::classify_isa(cpu, native::scalar).admitted()) return 1;
  return native::classify_isa(cpu, requirements).admitted() ==
         (cpu.present.has(native::arm_feature::neon) &&
          cpu.observed.has(native::arm_feature::neon)) ? 0 : 2;
}
#else
#error The native platform smoke requires x86-64 or AArch64
#endif
