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
  { native::bzhi<A>(value, 9u) } noexcept -> std::same_as<T>;
  { native::mulx<A>(value, mask, &mask) } noexcept -> std::same_as<T>;
  { native::rorx<A, 13>(value) } noexcept -> std::same_as<T>;
};
static_assert(has_bmi2<requirements, std::uint32_t>);
static_assert(has_bmi2<requirements, std::uint64_t>);
static_assert(has_bmi2<native::scalar, std::uint32_t>);
static_assert(has_bmi2<native::scalar, std::uint64_t>);
// Below-feature calls are immediate-only; runtime-input rejection has its own fixtures.
static_assert(native::pdep<native::scalar>(std::uint32_t{5}, std::uint32_t{0x52}) == 0x42);
static_assert(native::pext<native::scalar>(std::uint64_t{0x100000002},
                                         std::uint64_t{0x100000012}) == 5);

#define NATIVE_TARGET_platform_lzcnt "lzcnt"
constexpr auto lzcnt_requirements = NATIVE_TARGET_ISA(platform_lzcnt);
static_assert(lzcnt_requirements == native::isa(native::x86_feature::lzcnt));

NATIVE_TARGET_PUSH(platform_lzcnt)
static bool check_lzcnt(std::uint64_t value) {
  return native::lzcnt<lzcnt_requirements>(value | (std::uint64_t(1) << 63)) == 0 &&
         native::lzcnt<lzcnt_requirements>(std::uint16_t(0)) == 16 &&
         native::lzcnt<lzcnt_requirements>(std::uint32_t(0)) == 32 &&
         native::lzcnt<lzcnt_requirements>(std::uint64_t(0)) == 64;
}
NATIVE_TARGET_POP()

#define NATIVE_TARGET_platform_popcnt "popcnt"
constexpr auto popcnt_requirements = NATIVE_TARGET_ISA(platform_popcnt);
static_assert(popcnt_requirements == native::isa(native::x86_feature::popcnt));

NATIVE_TARGET_PUSH(platform_popcnt)
static bool check_popcnt(std::uint64_t value) {
  return native::popcnt<popcnt_requirements>(value) +
         native::popcnt<popcnt_requirements>(~value) == 64 &&
         native::popcnt<popcnt_requirements>(std::uint16_t(0xffff)) == 16;
}
NATIVE_TARGET_POP()

#define NATIVE_TARGET_platform_bmi1 "bmi"
constexpr auto bmi1_requirements = NATIVE_TARGET_ISA(platform_bmi1);
static_assert(bmi1_requirements == native::isa(native::x86_feature::bmi1));

NATIVE_TARGET_PUSH(platform_bmi1)
static bool check_bmi1(std::uint64_t value) {
  auto low = native::blsi<bmi1_requirements>(value);
  return native::blsr<bmi1_requirements>(value) == (value ^ low) &&
         native::tzcnt<bmi1_requirements>(std::uint16_t(0)) == 16;
}
NATIVE_TARGET_POP()

NATIVE_TARGET_PUSH(platform_bmi2)
static bool round_trip(std::uint32_t source32, std::uint64_t source64) {
  constexpr std::uint32_t mask32 = 0x80010102;
  constexpr std::uint64_t mask64 = 0x8000000100010102;
  auto deposited32 = native::pdep<requirements>(source32, mask32);
  auto deposited64 = native::pdep<requirements>(source64, mask64);
  std::uint64_t high;
  auto product = native::mulx<requirements>(source64, std::uint64_t(3), &high);
  return native::pext<requirements>(deposited32, mask32) == (source32 & 0xf) &&
         native::pext<requirements>(deposited64, mask64) == (source64 & 0x1f) &&
         native::bzhi<requirements>(source32, 256u) == 0 &&
         native::shrx<requirements>(source64, 64u) == source64 &&
         native::rorx<requirements, 0>(source64) == source64 &&
         product == source64 * 3 && high == 0;
}
NATIVE_TARGET_POP()

int main(int argc, char **) {
  auto cpu = native::observe_x86_capabilities();
  if (!native::classify_isa(cpu, native::scalar).admitted()) return 1;
  // Runtime inputs prevent the entire operation check becoming a constant.
  auto seed = static_cast<std::uint32_t>(argc);
  if (native::classify_isa(cpu, requirements).admitted() &&
      !round_trip(seed ^ 0x95u, std::uint64_t(seed) ^ 0x100000016ull)) return 2;
  if (native::classify_isa(cpu, bmi1_requirements).admitted() &&
      !check_bmi1(std::uint64_t(seed) ^ 0x100000016ull)) return 3;
  if (native::classify_isa(cpu, popcnt_requirements).admitted() &&
      !check_popcnt(std::uint64_t(seed) ^ 0x100000016ull)) return 4;
  if (native::classify_isa(cpu, lzcnt_requirements).admitted() &&
      !check_lzcnt(std::uint64_t(seed) ^ 0x100000016ull)) return 5;
  return 0;
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
