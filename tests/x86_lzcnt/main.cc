// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <bit>
#include <concepts>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <native/targets.h>
#if NATIVE_TEST_INTERFACE == 0
#include <native/x86/lzcnt.h>
import native.x86.features;
#elif NATIVE_TEST_INTERFACE == 1
import native.x86.lzcnt;
#elif NATIVE_TEST_INTERFACE == 2
import native;
#else
#error Select the header, direct module or main hub test interface.
#endif
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#define NATIVE_TARGET_test_lzcnt "lzcnt"
inline constexpr auto requirements = NATIVE_TARGET_ISA(test_lzcnt);
static_assert(requirements == native::isa<native::x86>(native::x86_feature::lzcnt));
static_assert(native::feature_closure(requirements) == requirements);

// Weak tags expose immediate-only calls; actual runtime rejection is tested separately.
template<native::isa<native::x86> A, class T>
concept has_lzcnt = requires(T value) {
  { native::lzcnt<A>(value) } noexcept -> std::same_as<T>;
};
static_assert(has_lzcnt<requirements, std::uint16_t>);
static_assert(has_lzcnt<requirements, std::uint32_t>);
static_assert(has_lzcnt<requirements, std::uint64_t>);
static_assert(has_lzcnt<native::isa<native::x86>{}, std::uint16_t>);
static_assert(has_lzcnt<native::isa<native::x86>{}, std::uint32_t>);
static_assert(has_lzcnt<native::isa<native::x86>{}, std::uint64_t>);
static_assert(has_lzcnt<native::isa<native::x86>(native::x86_feature::bmi1), std::uint32_t>);
static_assert(has_lzcnt<native::isa<native::x86>(native::x86_feature::bmi2), std::uint64_t>);
static_assert(has_lzcnt<native::isa<native::x86>(native::x86_feature::popcnt), std::uint32_t>);

consteval bool admission_contract() {
  native::x86_capabilities cpu{};
  cpu.present.set(native::x86_feature::lzcnt, true);
  cpu.observed.set(native::x86_feature::lzcnt, true);
  // A single scalar instruction feature needs no XCR0 observation or SIMD bits.
  if (!native::classify_isa(cpu, requirements).admitted()) return false;
  cpu.present.set(native::x86_feature::lzcnt, false);
  if (native::classify_isa(cpu, requirements).admitted()) return false;
  cpu.present.set(native::x86_feature::lzcnt, true);
  cpu.observed.set(native::x86_feature::lzcnt, false);
  return !native::classify_isa(cpu, requirements).admitted();
}
static_assert(admission_contract());

NATIVE_TARGET_PUSH(test_lzcnt)
template<class T>
[[gnu::noinline]] T invoke_lzcnt(T value) noexcept {
  return native::lzcnt<requirements>(value);
}
NATIVE_TARGET_POP()

template<class T> bool check_width(std::uint64_t& state) {
  constexpr unsigned width = std::numeric_limits<T>::digits;
  constexpr T all = std::numeric_limits<T>::max();
  auto check = [](T value) {
    return invoke_lzcnt(value) == static_cast<T>(std::countl_zero(value));
  };
  if (invoke_lzcnt(T{0}) != T{width} ||
      invoke_lzcnt(all) != T{0}) return false;
  for (unsigned bit = 0; bit != width; ++bit) {
    T single = static_cast<T>(T{1} << bit);
    if (!check(single) || !check(static_cast<T>(all ^ single))) return false;
  }
  for (unsigned i = 0; i != 4096; ++i) {
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    if (!check(static_cast<T>(state))) return false;
  }
  return true;
}

int main(int argc, char**) {
  auto cpu = native::observe_x86_capabilities();
  if (!native::classify_isa(cpu, requirements, NATIVE_TARGET_MINIMUM).admitted()) {
    std::puts("LZCNT unavailable; compile-time and synthetic admission checks passed.");
    return 77;
  }
  auto state = std::uint64_t{0x9e3779b97f4a7c15ull} ^ static_cast<unsigned>(argc);
  if (!check_width<std::uint16_t>(state)) return 1;
  if (!check_width<std::uint32_t>(state)) return 2;
  if (!check_width<std::uint64_t>(state)) return 3;
  return 0;
}
