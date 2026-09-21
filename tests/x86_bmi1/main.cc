// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <concepts>
#include <cstdint>
#include <cstdio>
#include <limits>
#if defined(NATIVE_BMI1_TEST_HEADER)
#include <native/x86/bmi1.h>
import native.x86.features;
#elif defined(NATIVE_BMI1_TEST_HUB)
import native;
#else
import native.x86.bmi1;
#endif

#if defined(__BMI__) || defined(__BMI2__) || defined(__AVX__) || defined(__AVX2__) || defined(__FMA__)
#error BMI1 admission must run in a baseline translation unit
#endif

constexpr native::isa<native::x86> bmi1{native::x86_feature::bmi1};
constexpr native::isa<native::x86> bmi2{native::x86_feature::bmi2};

template<native::isa<native::x86> A, class T> concept has_all_bmi1 = requires(T a, T b, unsigned c) {
  { native::andn<A>(a, b) } noexcept -> std::same_as<T>;
  { native::bextr<A>(a, c) } noexcept -> std::same_as<T>;
  { native::bextr<A>(a, c, c) } noexcept -> std::same_as<T>;
  { native::blsi<A>(a) } noexcept -> std::same_as<T>;
  { native::blsmsk<A>(a) } noexcept -> std::same_as<T>;
  { native::blsr<A>(a) } noexcept -> std::same_as<T>;
  { native::tzcnt<A>(a) } noexcept -> std::same_as<T>;
};
template<native::isa<native::x86> A> concept has_tzcnt16 = requires(std::uint16_t a) {
  { native::tzcnt<A>(a) } noexcept -> std::same_as<std::uint16_t>;
};
// Weak tags participate for constant evaluation; runtime calls are rejected separately.
static_assert(has_all_bmi1<bmi1, std::uint32_t> && has_all_bmi1<bmi1, std::uint64_t>);
static_assert(has_tzcnt16<bmi1>);
static_assert(has_all_bmi1<native::isa<native::x86>{}, std::uint32_t>);
static_assert(has_all_bmi1<native::isa<native::x86>{}, std::uint64_t>);
static_assert(has_all_bmi1<bmi2, std::uint32_t> && has_all_bmi1<bmi2, std::uint64_t>);
static_assert(has_tzcnt16<native::isa<native::x86>{}> && has_tzcnt16<bmi2>);

// BMI1 does not require the AVX OS state or the independent BMI2 flag.
static_assert([] {
  native::x86_capabilities cpu;
  cpu.present.set(native::x86_feature::bmi1, true);
  cpu.observed = cpu.present;
  auto result = native::classify_isa(cpu, bmi1);
  if (!result.admitted() || result.missing_xcr0 || result.missing_xcr0_observation) return false;
  cpu.observed.set(native::x86_feature::bmi1, false);
  if (native::classify_isa(cpu, bmi1).admitted()) return false;
  cpu.observed.set(native::x86_feature::bmi1, true);
  cpu.present.set(native::x86_feature::bmi1, false);
  return !native::classify_isa(cpu, bmi1).admitted();
}());

template<class T> constexpr unsigned width = std::numeric_limits<T>::digits;

// Bit-by-bit reference functions stay in the baseline part of the fixture.
template<class T> __attribute__((noinline)) unsigned count_low_zeroes(T value) {
  unsigned n = 0;
  while (n < width<T> && ((value >> n) & T{1}) == 0) ++n;
  return n;
}
template<class T> __attribute__((noinline)) T extract(T value, unsigned start, unsigned length) {
  start &= 255u;
  length &= 255u;
  T result = 0;
  for (unsigned i = 0; i < length && start + i < width<T>; ++i)
    if ((value >> (start + i)) & T{1}) result |= T{1} << i;
  return result;
}
template<class T> __attribute__((target("bmi"))) bool check_value(T value, T second) {
  auto n = count_low_zeroes(value);
  T isolated = 0, mask = 0, and_not = 0;
  for (unsigned i = 0; i < width<T>; ++i) {
    if (i == n) isolated |= T{1} << i;
    if (i <= n) mask |= T{1} << i;
    if (!((value >> i) & T{1}) && ((second >> i) & T{1})) and_not |= T{1} << i;
  }
  if (native::andn<bmi1>(value, second) != and_not ||
      native::blsi<bmi1>(value) != isolated ||
      native::blsmsk<bmi1>(value) != mask ||
      native::blsr<bmi1>(value) != (value ^ isolated) ||
      native::tzcnt<bmi1>(value) != n) return false;
  constexpr std::array<unsigned, 15> controls{0, 1, 15, 16, 31, 32, 33, 63, 64, 65, 255, 256, 257, 511, 0xffffffffu};
  for (auto start : controls) for (auto length : controls) {
    auto expected = extract(value, start, length);
    auto packed = (start & 255u) | ((length & 255u) << 8) | 0xface0000u;
    if (native::bextr<bmi1>(value, start, length) != expected ||
        native::bextr<bmi1>(value, packed) != expected) return false;
  }
  return true;
}

template<class T> __attribute__((target("bmi"))) bool check_width() {
  constexpr T all = std::numeric_limits<T>::max();
  for (T value : std::array<T, 8>{0, 1, 2, 3, all, T(all - 1), T(all / 3), T(all / 3 * 2)})
    if (!check_value(value, T(~value))) return false;
  for (unsigned bit = 0; bit < width<T>; ++bit)
    if (!check_value(T{1} << bit, all) || !check_value(T(~(T{1} << bit)), all)) return false;
  std::uint64_t state = 0x8ca28942b764f091ull;
  for (unsigned i = 0; i < 1024; ++i) {
    state ^= state << 13; state ^= state >> 7; state ^= state << 17;
    if (!check_value(T(state), T(state >> 11))) return false;
  }
  // Cover every hardware control pair, including lengths crossing the width.
  for (std::uint32_t control = 0; control != 65536; ++control)
    if (native::bextr<bmi1>(all, control) != extract(all, control & 255u, control >> 8)) return false;
  return true;
}
__attribute__((target("bmi"), noinline)) bool check_bmi1() {
  for (unsigned value = 0; value != 65536; ++value)
    if (native::tzcnt<bmi1>(std::uint16_t(value)) != count_low_zeroes(std::uint16_t(value))) return false;
  return check_width<std::uint32_t>() && check_width<std::uint64_t>();
}

int main() {
  if (!native::classify_isa(native::observe_x86_capabilities(), bmi1).admitted()) {
    std::puts("BMI1 unavailable; runtime operations skipped");
    return 77;
  }
  if (!check_bmi1()) {
    std::fputs("BMI1 result differs from scalar reference\n", stderr);
    return 1;
  }
}
