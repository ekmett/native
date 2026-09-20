// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#define NATIVE_TARGET_test_popcnt "popcnt"
inline constexpr auto requirements = NATIVE_TARGET_ISA(test_popcnt);
static_assert(requirements == native::isa(native::x86_feature::popcnt));
static_assert(native::feature_closure(requirements) == requirements);

template<native::isa A, class T>
concept has_popcnt = requires(T value) {
  { native::popcnt<A>(value) } noexcept -> std::same_as<T>;
};
static_assert(has_popcnt<requirements, std::uint16_t>);
static_assert(has_popcnt<requirements, std::uint32_t>);
static_assert(has_popcnt<requirements, std::uint64_t>);
static_assert(!has_popcnt<native::scalar, std::uint16_t>);
static_assert(!has_popcnt<native::scalar, std::uint32_t>);
static_assert(!has_popcnt<native::scalar, std::uint64_t>);
static_assert(!has_popcnt<native::isa(native::x86_feature::bmi1), std::uint32_t>);
static_assert(!has_popcnt<native::isa(native::x86_feature::bmi2), std::uint64_t>);
static_assert(!has_popcnt<native::isa(native::x86_feature::lzcnt), std::uint32_t>);

consteval bool admission_contract() {
  native::x86_capabilities cpu{};
  cpu.present.set(native::x86_feature::popcnt, true);
  cpu.observed.set(native::x86_feature::popcnt, true);
  // A single scalar instruction feature needs no XCR0 observation or SIMD bits.
  if (!native::classify_isa(cpu, requirements).admitted()) return false;
  cpu.present.set(native::x86_feature::popcnt, false);
  if (native::classify_isa(cpu, requirements).admitted()) return false;
  cpu.present.set(native::x86_feature::popcnt, true);
  cpu.observed.set(native::x86_feature::popcnt, false);
  return !native::classify_isa(cpu, requirements).admitted();
}
static_assert(admission_contract());

NATIVE_TARGET_PUSH(test_popcnt)
template<class T>
[[gnu::noinline]] T invoke_popcnt(T value) noexcept {
  return native::popcnt<requirements>(value);
}
NATIVE_TARGET_POP()

template<class T> bool check_width(std::uint64_t& state) {
  constexpr unsigned width = std::numeric_limits<T>::digits;
  constexpr T all = std::numeric_limits<T>::max();
  auto check = [](T value) {
    return invoke_popcnt(value) == static_cast<T>(std::popcount(value));
  };
  if (invoke_popcnt(T{0}) != T{0} ||
      invoke_popcnt(all) != T{width}) return false;
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
