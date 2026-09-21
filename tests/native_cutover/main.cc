// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/targets.h>
#include <concepts>
#include <cstdint>
import native;
using namespace native;

#if !NATIVE_MINIMAL_HAS_AVX2 && defined(__AVX2__)
#error Importing native must not raise the configured minimum to AVX2
#endif
#if !NATIVE_MINIMAL_HAS_AVX512 && (defined(__AVX512F__) || defined(__AVX512DQ__) || defined(__AVX512BW__) || defined(__AVX512VL__))
#error Importing native must not raise the configured minimum to AVX-512
#endif

static_assert(native::isa(native::x86_feature::bmi2).has(native::x86_feature::bmi2));
static_assert(native::isa(native::arm_feature::neon).has(native::arm_feature::neon));
// A public alias cannot be specialized. This checks that simd is the primary
// class template, including when reached through the omnibus module.
struct extension_element {};
template<> struct native::simd<extension_element, 1, native::scalar> {
  using value_type = extension_element;
};
static_assert(std::same_as<native::simd<extension_element, 1, native::scalar>::value_type,
                           extension_element>);

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_TARGET_cutover_bmi2 "bmi2"
constexpr auto bmi2 = NATIVE_TARGET_ISA(cutover_bmi2);
template<native::isa A, class T> concept has_bmi2 = requires(T value, T mask) {
  { native::pdep<A>(value, mask) } noexcept -> std::same_as<T>;
  { native::pext<A>(value, mask) } noexcept -> std::same_as<T>;
};
static_assert(has_bmi2<bmi2, std::uint32_t> && has_bmi2<bmi2, std::uint64_t>);
static_assert(!has_bmi2<native::scalar, std::uint32_t>);
static_assert(!has_bmi2<native::scalar, std::uint64_t>);

NATIVE_TARGET_PUSH(cutover_bmi2)
static bool check_bmi2() {
  return native::pdep<bmi2>(std::uint32_t(5), std::uint32_t(0x52)) == 0x42 &&
         native::pext<bmi2>(std::uint32_t(0x42), std::uint32_t(0x52)) == 5 &&
         native::pdep<bmi2>(std::uint64_t(5), std::uint64_t(0x100000012)) == 0x100000002 &&
         native::pext<bmi2>(std::uint64_t(0x100000002), std::uint64_t(0x100000012)) == 5;
}
NATIVE_TARGET_POP()
#endif

int main() {
  simd<float, 1, scalar> x(2.f);
  float result = 0.f;
  native::store_simd(&result, x + x);
  if (result != 4.f) return 1;
#if defined(__x86_64__) || defined(_M_X64)
  auto cpu = native::observe_cpu();
  if (native::classify_isa(cpu, bmi2).admitted() && !check_bmi2()) return 2;
#elif defined(__aarch64__) || defined(_M_ARM64)
  auto cpu = native::observe_cpu();
#else
  return 0;
#endif
#if defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__) || defined(_M_ARM64)
  unsigned calls = 0;
  auto selected = native::with_isa(native::isa_list<native::scalar>{}, cpu,
    [&]<native::isa A> { static_assert(A == native::scalar); ++calls; });
  return selected && calls == 1 ? 0 : 3;
#endif
}
