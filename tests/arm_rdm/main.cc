// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <concepts>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <utility>
#include <arm_neon.h>
#include <native/targets.h>
#if NATIVE_TEST_INTERFACE == 0
import native.arm;
#elif NATIVE_TEST_INTERFACE == 1
import native.arm.rdm;
#elif NATIVE_TEST_INTERFACE == 2
import native;
#else
#error Select header, granular module or omnibus interface.
#endif
#include "simd_adapter.h"
#include "simd_contract.h"
constexpr auto requirements = native::feature_closure(native::arm_feature::rdm);
consteval bool admission_contract() {
  native::arm_capabilities cpu{};
  for (auto f : {native::arm_feature::neon, native::arm_feature::rdm}) {
    cpu.present.set(f, true);
    cpu.observed.set(f, true);
  }
  if (!native::classify_isa(cpu, requirements).admitted()) return false;
  cpu.present.set(native::arm_feature::rdm, false);
  if (native::classify_isa(cpu, requirements).admitted()) return false;
  cpu.present.set(native::arm_feature::rdm, true);
  cpu.observed.set(native::arm_feature::rdm, false);
  return !native::classify_isa(cpu, requirements).admitted();
}
static_assert(admission_contract());
template<native::isa<native::arm> A, class R, class X>
concept has_sqrdmlah = requires(R r, X x) {
  { rdm_api::sqrdmlah<A>(r, x, x) } noexcept -> std::same_as<R>;
};
template<native::isa<native::arm> A, int Lane, class R, class X, class Y>
concept has_sqrdmlah_lane = requires(R r, X x, Y y) {
  { rdm_api::sqrdmlah_lane<A, Lane>(r, x, y) } noexcept -> std::same_as<R>;
};
template<native::isa<native::arm> A, class R, class X>
concept has_sqrdmlsh = requires(R r, X x) {
  { rdm_api::sqrdmlsh<A>(r, x, x) } noexcept -> std::same_as<R>;
};
template<native::isa<native::arm> A, int Lane, class R, class X, class Y>
concept has_sqrdmlsh_lane = requires(R r, X x, Y y) {
  { rdm_api::sqrdmlsh_lane<A, Lane>(r, x, y) } noexcept -> std::same_as<R>;
};
// Check ordinary call participation: a wrong return type alone must not hide
// an invalid 32-bit lane falling through to a 16-bit instruction overload.
template<int Lane, class R, class Y>
concept calls_sqrdmlah_lane = requires(R a, Y c) {
  rdm_api::sqrdmlah_lane<requirements, Lane>(a, a, c);
};
template<int Lane, class R, class Y>
concept calls_sqrdmlsh_lane = requires(R a, Y c) {
  rdm_api::sqrdmlsh_lane<requirements, Lane>(a, a, c);
};
static_assert(!calls_sqrdmlah_lane<2, int32_t, int32x2_t>);
static_assert(!calls_sqrdmlah_lane<3, int32_t, int32x2_t>);
static_assert(!calls_sqrdmlah_lane<4, int32_t, int32x4_t>);
static_assert(!calls_sqrdmlah_lane<5, int32_t, int32x4_t>);
static_assert(!calls_sqrdmlah_lane<6, int32_t, int32x4_t>);
static_assert(!calls_sqrdmlah_lane<7, int32_t, int32x4_t>);
static_assert(!calls_sqrdmlah_lane<2, int32x2_t, int32x2_t>);
static_assert(!calls_sqrdmlah_lane<3, int32x2_t, int32x2_t>);
static_assert(!calls_sqrdmlah_lane<4, int32x2_t, int32x4_t>);
static_assert(!calls_sqrdmlah_lane<5, int32x2_t, int32x4_t>);
static_assert(!calls_sqrdmlah_lane<6, int32x2_t, int32x4_t>);
static_assert(!calls_sqrdmlah_lane<7, int32x2_t, int32x4_t>);
static_assert(!calls_sqrdmlah_lane<2, int32x4_t, int32x2_t>);
static_assert(!calls_sqrdmlah_lane<3, int32x4_t, int32x2_t>);
static_assert(!calls_sqrdmlah_lane<4, int32x4_t, int32x4_t>);
static_assert(!calls_sqrdmlah_lane<5, int32x4_t, int32x4_t>);
static_assert(!calls_sqrdmlah_lane<6, int32x4_t, int32x4_t>);
static_assert(!calls_sqrdmlah_lane<7, int32x4_t, int32x4_t>);
static_assert(!calls_sqrdmlsh_lane<2, int32_t, int32x2_t>);
static_assert(!calls_sqrdmlsh_lane<3, int32_t, int32x2_t>);
static_assert(!calls_sqrdmlsh_lane<4, int32_t, int32x4_t>);
static_assert(!calls_sqrdmlsh_lane<5, int32_t, int32x4_t>);
static_assert(!calls_sqrdmlsh_lane<6, int32_t, int32x4_t>);
static_assert(!calls_sqrdmlsh_lane<7, int32_t, int32x4_t>);
static_assert(!calls_sqrdmlsh_lane<2, int32x2_t, int32x2_t>);
static_assert(!calls_sqrdmlsh_lane<3, int32x2_t, int32x2_t>);
static_assert(!calls_sqrdmlsh_lane<4, int32x2_t, int32x4_t>);
static_assert(!calls_sqrdmlsh_lane<5, int32x2_t, int32x4_t>);
static_assert(!calls_sqrdmlsh_lane<6, int32x2_t, int32x4_t>);
static_assert(!calls_sqrdmlsh_lane<7, int32x2_t, int32x4_t>);
static_assert(!calls_sqrdmlsh_lane<2, int32x4_t, int32x2_t>);
static_assert(!calls_sqrdmlsh_lane<3, int32x4_t, int32x2_t>);
static_assert(!calls_sqrdmlsh_lane<4, int32x4_t, int32x4_t>);
static_assert(!calls_sqrdmlsh_lane<5, int32x4_t, int32x4_t>);
static_assert(!calls_sqrdmlsh_lane<6, int32x4_t, int32x4_t>);
static_assert(!calls_sqrdmlsh_lane<7, int32x4_t, int32x4_t>);
template<class T, std::size_t N>
std::array<T, N> sample(std::uint64_t & state, unsigned iteration, unsigned operand) {
  constexpr auto lo = std::numeric_limits<T>::min();
  constexpr auto hi = std::numeric_limits<T>::max();
  constexpr T edges[]{lo, hi, T{0}, T{1}, static_cast<T>(-1),
    static_cast<T>(lo / 2), static_cast<T>(hi / 2)};
  // The first cases reach every shape and every selected lane deterministically.
  constexpr T half = T{1} << (sizeof(T) * 8 - 2);
  constexpr T directed[][3] = {
    {lo, lo, lo}, {hi, lo, lo}, {-1, lo, lo}, {0, lo, lo},
    {lo, hi, hi}, {hi, hi, hi}, {0, 1, half}, {0, -1, half},
    {7, 1, half}, {-7, -1, half}, {0, hi, hi}, {lo, lo, hi},
    {hi, lo, hi}, {lo, 0, hi}, {hi, 0, lo}, {0, lo, hi}
  };
  std::array<T, N> result;
  for (auto & x : result) {
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    x = iteration < std::size(directed) ? directed[iteration][operand] :
      iteration < 64 ? edges[state % std::size(edges)] : static_cast<T>(state);
  }
  return result;
}
// Wide arithmetic keeps the full product and accumulator until the single
// architectural rounding shift. Signed right shift is arithmetic in C++26.
template<class T> constexpr T reference(T a, T b, T c, bool subtract) {
  constexpr int width = sizeof(T) * 8;
  auto product = __int128{2} * b * c;
  auto wide = __int128{a} * (__int128{1} << width) +
    (subtract ? -product : product) + (__int128{1} << (width - 1));
  auto rounded = wide >> width;
  if (rounded > std::numeric_limits<T>::max()) return std::numeric_limits<T>::max();
  if (rounded < std::numeric_limits<T>::min()) return std::numeric_limits<T>::min();
  return static_cast<T>(rounded);
}
static_assert(reference<std::int16_t>(-32768, -32768, -32768, false) == 0);
static_assert(reference<std::int16_t>(32767, -32768, -32768, true) == -1);
static_assert(reference<std::int16_t>(0, 1, 16384, false) == 1);
static_assert(reference<std::int16_t>(0, 1, 16384, true) == 0);
static_assert(reference<std::int32_t>(-2147483647 - 1, -2147483647 - 1,
  -2147483647 - 1, false) == 0);

// Keep FPSR reads/writes ordered and restore the caller's status on every exit.
std::uint64_t read_fpsr() {
  std::uint64_t result;
  asm volatile("mrs %0, fpsr" : "=r"(result) : : "memory");
  return result;
}
void write_fpsr(std::uint64_t value) {
  asm volatile("msr fpsr, %0" : : "r"(value) : "memory");
}
__attribute__((target("rdm"), noinline))
bool check_saturation_flag() {
  auto saved = read_fpsr();
  constexpr std::uint64_t qc = 1ull << 27;
  auto clear = saved & ~qc;
  write_fpsr(clear);
  // An unused arithmetic result must still produce the architectural QC effect.
  (void)rdm_api::sqrdmlah<requirements>(std::int16_t{0}, std::int16_t{-32768},
    std::int16_t{-32768});
  bool passed = (read_fpsr() & qc) != 0;
  (void)rdm_api::sqrdmlsh<requirements>(std::int16_t{0}, std::int16_t{0},
    std::int16_t{0});
  passed &= (read_fpsr() & qc) != 0;
  write_fpsr(clear);
  (void)rdm_api::sqrdmlah<requirements>(std::int16_t{-32768}, std::int16_t{-32768},
    std::int16_t{-32768});
  passed &= (read_fpsr() & qc) == 0;
  (void)rdm_api::sqrdmlsh_lane<requirements, 3>(vdupq_n_s32(-2147483647 - 1),
    vdupq_n_s32(-2147483647 - 1), vdupq_n_s32(-2147483647 - 1));
  passed &= (read_fpsr() & qc) != 0;
  write_fpsr(saved);
  return passed;
}
static_assert(has_sqrdmlah<requirements, int16_t, int16_t>);
static_assert(has_sqrdmlah<native::isa<native::arm>(native::arm_feature::rdm), int16_t, int16_t>);
static_assert(!has_sqrdmlah<native::scalar, int16_t, int16_t>);
static_assert(!has_sqrdmlah<native::neon, int16_t, int16_t>);
static_assert(!has_sqrdmlah<native::isa<native::arm>(native::arm_feature::dotprod), int16_t, int16_t>);
static_assert(has_sqrdmlah_lane<requirements, 3, int16_t, int16_t, int16x4_t>);
static_assert(!has_sqrdmlah_lane<requirements, -1, int16_t, int16_t, int16x4_t>);
static_assert(!has_sqrdmlah_lane<requirements, 4, int16_t, int16_t, int16x4_t>);
static_assert(!has_sqrdmlah_lane<native::neon, 0, int16_t, int16_t, int16x4_t>);
static_assert(has_sqrdmlah_lane<requirements, 7, int16_t, int16_t, int16x8_t>);
static_assert(!has_sqrdmlah_lane<requirements, -1, int16_t, int16_t, int16x8_t>);
static_assert(!has_sqrdmlah_lane<requirements, 8, int16_t, int16_t, int16x8_t>);
static_assert(!has_sqrdmlah_lane<native::neon, 0, int16_t, int16_t, int16x8_t>);
static_assert(has_sqrdmlah<requirements, int16x4_t, int16x4_t>);
static_assert(has_sqrdmlah<native::isa<native::arm>(native::arm_feature::rdm), int16x4_t, int16x4_t>);
static_assert(!has_sqrdmlah<native::scalar, int16x4_t, int16x4_t>);
static_assert(!has_sqrdmlah<native::neon, int16x4_t, int16x4_t>);
static_assert(!has_sqrdmlah<native::isa<native::arm>(native::arm_feature::dotprod), int16x4_t, int16x4_t>);
static_assert(has_sqrdmlah_lane<requirements, 3, int16x4_t, int16x4_t, int16x4_t>);
static_assert(!has_sqrdmlah_lane<requirements, -1, int16x4_t, int16x4_t, int16x4_t>);
static_assert(!has_sqrdmlah_lane<requirements, 4, int16x4_t, int16x4_t, int16x4_t>);
static_assert(!has_sqrdmlah_lane<native::neon, 0, int16x4_t, int16x4_t, int16x4_t>);
static_assert(has_sqrdmlah_lane<requirements, 7, int16x4_t, int16x4_t, int16x8_t>);
static_assert(!has_sqrdmlah_lane<requirements, -1, int16x4_t, int16x4_t, int16x8_t>);
static_assert(!has_sqrdmlah_lane<requirements, 8, int16x4_t, int16x4_t, int16x8_t>);
static_assert(!has_sqrdmlah_lane<native::neon, 0, int16x4_t, int16x4_t, int16x8_t>);
static_assert(has_sqrdmlah<requirements, int16x8_t, int16x8_t>);
static_assert(has_sqrdmlah<native::isa<native::arm>(native::arm_feature::rdm), int16x8_t, int16x8_t>);
static_assert(!has_sqrdmlah<native::scalar, int16x8_t, int16x8_t>);
static_assert(!has_sqrdmlah<native::neon, int16x8_t, int16x8_t>);
static_assert(!has_sqrdmlah<native::isa<native::arm>(native::arm_feature::dotprod), int16x8_t, int16x8_t>);
static_assert(has_sqrdmlah_lane<requirements, 3, int16x8_t, int16x8_t, int16x4_t>);
static_assert(!has_sqrdmlah_lane<requirements, -1, int16x8_t, int16x8_t, int16x4_t>);
static_assert(!has_sqrdmlah_lane<requirements, 4, int16x8_t, int16x8_t, int16x4_t>);
static_assert(!has_sqrdmlah_lane<native::neon, 0, int16x8_t, int16x8_t, int16x4_t>);
static_assert(has_sqrdmlah_lane<requirements, 7, int16x8_t, int16x8_t, int16x8_t>);
static_assert(!has_sqrdmlah_lane<requirements, -1, int16x8_t, int16x8_t, int16x8_t>);
static_assert(!has_sqrdmlah_lane<requirements, 8, int16x8_t, int16x8_t, int16x8_t>);
static_assert(!has_sqrdmlah_lane<native::neon, 0, int16x8_t, int16x8_t, int16x8_t>);
static_assert(has_sqrdmlah<requirements, int32_t, int32_t>);
static_assert(has_sqrdmlah<native::isa<native::arm>(native::arm_feature::rdm), int32_t, int32_t>);
static_assert(!has_sqrdmlah<native::scalar, int32_t, int32_t>);
static_assert(!has_sqrdmlah<native::neon, int32_t, int32_t>);
static_assert(!has_sqrdmlah<native::isa<native::arm>(native::arm_feature::dotprod), int32_t, int32_t>);
static_assert(has_sqrdmlah_lane<requirements, 1, int32_t, int32_t, int32x2_t>);
static_assert(!has_sqrdmlah_lane<requirements, -1, int32_t, int32_t, int32x2_t>);
static_assert(!has_sqrdmlah_lane<requirements, 2, int32_t, int32_t, int32x2_t>);
static_assert(!has_sqrdmlah_lane<native::neon, 0, int32_t, int32_t, int32x2_t>);
static_assert(has_sqrdmlah_lane<requirements, 3, int32_t, int32_t, int32x4_t>);
static_assert(!has_sqrdmlah_lane<requirements, -1, int32_t, int32_t, int32x4_t>);
static_assert(!has_sqrdmlah_lane<requirements, 4, int32_t, int32_t, int32x4_t>);
static_assert(!has_sqrdmlah_lane<native::neon, 0, int32_t, int32_t, int32x4_t>);
static_assert(has_sqrdmlah<requirements, int32x2_t, int32x2_t>);
static_assert(has_sqrdmlah<native::isa<native::arm>(native::arm_feature::rdm), int32x2_t, int32x2_t>);
static_assert(!has_sqrdmlah<native::scalar, int32x2_t, int32x2_t>);
static_assert(!has_sqrdmlah<native::neon, int32x2_t, int32x2_t>);
static_assert(!has_sqrdmlah<native::isa<native::arm>(native::arm_feature::dotprod), int32x2_t, int32x2_t>);
static_assert(has_sqrdmlah_lane<requirements, 1, int32x2_t, int32x2_t, int32x2_t>);
static_assert(!has_sqrdmlah_lane<requirements, -1, int32x2_t, int32x2_t, int32x2_t>);
static_assert(!has_sqrdmlah_lane<requirements, 2, int32x2_t, int32x2_t, int32x2_t>);
static_assert(!has_sqrdmlah_lane<native::neon, 0, int32x2_t, int32x2_t, int32x2_t>);
static_assert(has_sqrdmlah_lane<requirements, 3, int32x2_t, int32x2_t, int32x4_t>);
static_assert(!has_sqrdmlah_lane<requirements, -1, int32x2_t, int32x2_t, int32x4_t>);
static_assert(!has_sqrdmlah_lane<requirements, 4, int32x2_t, int32x2_t, int32x4_t>);
static_assert(!has_sqrdmlah_lane<native::neon, 0, int32x2_t, int32x2_t, int32x4_t>);
static_assert(has_sqrdmlah<requirements, int32x4_t, int32x4_t>);
static_assert(has_sqrdmlah<native::isa<native::arm>(native::arm_feature::rdm), int32x4_t, int32x4_t>);
static_assert(!has_sqrdmlah<native::scalar, int32x4_t, int32x4_t>);
static_assert(!has_sqrdmlah<native::neon, int32x4_t, int32x4_t>);
static_assert(!has_sqrdmlah<native::isa<native::arm>(native::arm_feature::dotprod), int32x4_t, int32x4_t>);
static_assert(has_sqrdmlah_lane<requirements, 1, int32x4_t, int32x4_t, int32x2_t>);
static_assert(!has_sqrdmlah_lane<requirements, -1, int32x4_t, int32x4_t, int32x2_t>);
static_assert(!has_sqrdmlah_lane<requirements, 2, int32x4_t, int32x4_t, int32x2_t>);
static_assert(!has_sqrdmlah_lane<native::neon, 0, int32x4_t, int32x4_t, int32x2_t>);
static_assert(has_sqrdmlah_lane<requirements, 3, int32x4_t, int32x4_t, int32x4_t>);
static_assert(!has_sqrdmlah_lane<requirements, -1, int32x4_t, int32x4_t, int32x4_t>);
static_assert(!has_sqrdmlah_lane<requirements, 4, int32x4_t, int32x4_t, int32x4_t>);
static_assert(!has_sqrdmlah_lane<native::neon, 0, int32x4_t, int32x4_t, int32x4_t>);
static_assert(has_sqrdmlsh<requirements, int16_t, int16_t>);
static_assert(has_sqrdmlsh<native::isa<native::arm>(native::arm_feature::rdm), int16_t, int16_t>);
static_assert(!has_sqrdmlsh<native::scalar, int16_t, int16_t>);
static_assert(!has_sqrdmlsh<native::neon, int16_t, int16_t>);
static_assert(!has_sqrdmlsh<native::isa<native::arm>(native::arm_feature::dotprod), int16_t, int16_t>);
static_assert(has_sqrdmlsh_lane<requirements, 3, int16_t, int16_t, int16x4_t>);
static_assert(!has_sqrdmlsh_lane<requirements, -1, int16_t, int16_t, int16x4_t>);
static_assert(!has_sqrdmlsh_lane<requirements, 4, int16_t, int16_t, int16x4_t>);
static_assert(!has_sqrdmlsh_lane<native::neon, 0, int16_t, int16_t, int16x4_t>);
static_assert(has_sqrdmlsh_lane<requirements, 7, int16_t, int16_t, int16x8_t>);
static_assert(!has_sqrdmlsh_lane<requirements, -1, int16_t, int16_t, int16x8_t>);
static_assert(!has_sqrdmlsh_lane<requirements, 8, int16_t, int16_t, int16x8_t>);
static_assert(!has_sqrdmlsh_lane<native::neon, 0, int16_t, int16_t, int16x8_t>);
static_assert(has_sqrdmlsh<requirements, int16x4_t, int16x4_t>);
static_assert(has_sqrdmlsh<native::isa<native::arm>(native::arm_feature::rdm), int16x4_t, int16x4_t>);
static_assert(!has_sqrdmlsh<native::scalar, int16x4_t, int16x4_t>);
static_assert(!has_sqrdmlsh<native::neon, int16x4_t, int16x4_t>);
static_assert(!has_sqrdmlsh<native::isa<native::arm>(native::arm_feature::dotprod), int16x4_t, int16x4_t>);
static_assert(has_sqrdmlsh_lane<requirements, 3, int16x4_t, int16x4_t, int16x4_t>);
static_assert(!has_sqrdmlsh_lane<requirements, -1, int16x4_t, int16x4_t, int16x4_t>);
static_assert(!has_sqrdmlsh_lane<requirements, 4, int16x4_t, int16x4_t, int16x4_t>);
static_assert(!has_sqrdmlsh_lane<native::neon, 0, int16x4_t, int16x4_t, int16x4_t>);
static_assert(has_sqrdmlsh_lane<requirements, 7, int16x4_t, int16x4_t, int16x8_t>);
static_assert(!has_sqrdmlsh_lane<requirements, -1, int16x4_t, int16x4_t, int16x8_t>);
static_assert(!has_sqrdmlsh_lane<requirements, 8, int16x4_t, int16x4_t, int16x8_t>);
static_assert(!has_sqrdmlsh_lane<native::neon, 0, int16x4_t, int16x4_t, int16x8_t>);
static_assert(has_sqrdmlsh<requirements, int16x8_t, int16x8_t>);
static_assert(has_sqrdmlsh<native::isa<native::arm>(native::arm_feature::rdm), int16x8_t, int16x8_t>);
static_assert(!has_sqrdmlsh<native::scalar, int16x8_t, int16x8_t>);
static_assert(!has_sqrdmlsh<native::neon, int16x8_t, int16x8_t>);
static_assert(!has_sqrdmlsh<native::isa<native::arm>(native::arm_feature::dotprod), int16x8_t, int16x8_t>);
static_assert(has_sqrdmlsh_lane<requirements, 3, int16x8_t, int16x8_t, int16x4_t>);
static_assert(!has_sqrdmlsh_lane<requirements, -1, int16x8_t, int16x8_t, int16x4_t>);
static_assert(!has_sqrdmlsh_lane<requirements, 4, int16x8_t, int16x8_t, int16x4_t>);
static_assert(!has_sqrdmlsh_lane<native::neon, 0, int16x8_t, int16x8_t, int16x4_t>);
static_assert(has_sqrdmlsh_lane<requirements, 7, int16x8_t, int16x8_t, int16x8_t>);
static_assert(!has_sqrdmlsh_lane<requirements, -1, int16x8_t, int16x8_t, int16x8_t>);
static_assert(!has_sqrdmlsh_lane<requirements, 8, int16x8_t, int16x8_t, int16x8_t>);
static_assert(!has_sqrdmlsh_lane<native::neon, 0, int16x8_t, int16x8_t, int16x8_t>);
static_assert(has_sqrdmlsh<requirements, int32_t, int32_t>);
static_assert(has_sqrdmlsh<native::isa<native::arm>(native::arm_feature::rdm), int32_t, int32_t>);
static_assert(!has_sqrdmlsh<native::scalar, int32_t, int32_t>);
static_assert(!has_sqrdmlsh<native::neon, int32_t, int32_t>);
static_assert(!has_sqrdmlsh<native::isa<native::arm>(native::arm_feature::dotprod), int32_t, int32_t>);
static_assert(has_sqrdmlsh_lane<requirements, 1, int32_t, int32_t, int32x2_t>);
static_assert(!has_sqrdmlsh_lane<requirements, -1, int32_t, int32_t, int32x2_t>);
static_assert(!has_sqrdmlsh_lane<requirements, 2, int32_t, int32_t, int32x2_t>);
static_assert(!has_sqrdmlsh_lane<native::neon, 0, int32_t, int32_t, int32x2_t>);
static_assert(has_sqrdmlsh_lane<requirements, 3, int32_t, int32_t, int32x4_t>);
static_assert(!has_sqrdmlsh_lane<requirements, -1, int32_t, int32_t, int32x4_t>);
static_assert(!has_sqrdmlsh_lane<requirements, 4, int32_t, int32_t, int32x4_t>);
static_assert(!has_sqrdmlsh_lane<native::neon, 0, int32_t, int32_t, int32x4_t>);
static_assert(has_sqrdmlsh<requirements, int32x2_t, int32x2_t>);
static_assert(has_sqrdmlsh<native::isa<native::arm>(native::arm_feature::rdm), int32x2_t, int32x2_t>);
static_assert(!has_sqrdmlsh<native::scalar, int32x2_t, int32x2_t>);
static_assert(!has_sqrdmlsh<native::neon, int32x2_t, int32x2_t>);
static_assert(!has_sqrdmlsh<native::isa<native::arm>(native::arm_feature::dotprod), int32x2_t, int32x2_t>);
static_assert(has_sqrdmlsh_lane<requirements, 1, int32x2_t, int32x2_t, int32x2_t>);
static_assert(!has_sqrdmlsh_lane<requirements, -1, int32x2_t, int32x2_t, int32x2_t>);
static_assert(!has_sqrdmlsh_lane<requirements, 2, int32x2_t, int32x2_t, int32x2_t>);
static_assert(!has_sqrdmlsh_lane<native::neon, 0, int32x2_t, int32x2_t, int32x2_t>);
static_assert(has_sqrdmlsh_lane<requirements, 3, int32x2_t, int32x2_t, int32x4_t>);
static_assert(!has_sqrdmlsh_lane<requirements, -1, int32x2_t, int32x2_t, int32x4_t>);
static_assert(!has_sqrdmlsh_lane<requirements, 4, int32x2_t, int32x2_t, int32x4_t>);
static_assert(!has_sqrdmlsh_lane<native::neon, 0, int32x2_t, int32x2_t, int32x4_t>);
static_assert(has_sqrdmlsh<requirements, int32x4_t, int32x4_t>);
static_assert(has_sqrdmlsh<native::isa<native::arm>(native::arm_feature::rdm), int32x4_t, int32x4_t>);
static_assert(!has_sqrdmlsh<native::scalar, int32x4_t, int32x4_t>);
static_assert(!has_sqrdmlsh<native::neon, int32x4_t, int32x4_t>);
static_assert(!has_sqrdmlsh<native::isa<native::arm>(native::arm_feature::dotprod), int32x4_t, int32x4_t>);
static_assert(has_sqrdmlsh_lane<requirements, 1, int32x4_t, int32x4_t, int32x2_t>);
static_assert(!has_sqrdmlsh_lane<requirements, -1, int32x4_t, int32x4_t, int32x2_t>);
static_assert(!has_sqrdmlsh_lane<requirements, 2, int32x4_t, int32x4_t, int32x2_t>);
static_assert(!has_sqrdmlsh_lane<native::neon, 0, int32x4_t, int32x4_t, int32x2_t>);
static_assert(has_sqrdmlsh_lane<requirements, 3, int32x4_t, int32x4_t, int32x4_t>);
static_assert(!has_sqrdmlsh_lane<requirements, -1, int32x4_t, int32x4_t, int32x4_t>);
static_assert(!has_sqrdmlsh_lane<requirements, 4, int32x4_t, int32x4_t, int32x4_t>);
static_assert(!has_sqrdmlsh_lane<native::neon, 0, int32x4_t, int32x4_t, int32x4_t>);
__attribute__((target("rdm"), noinline))
bool check_sqrdmlah_int16(std::uint64_t & state) {
  for (unsigned iteration = 0; iteration < 2048; ++iteration) {
    auto a = sample<std::int16_t, 1>(state, iteration, 0);
    auto b = sample<std::int16_t, 1>(state, iteration, 1);
    auto c = sample<std::int16_t, 1>(state, iteration, 2);
    auto av = std::bit_cast<int16_t>(a);
    auto bv = std::bit_cast<int16_t>(b);
    auto cv = std::bit_cast<int16_t>(c);
    auto result = std::bit_cast<std::array<std::int16_t, 1>>(rdm_api::sqrdmlah<requirements>(av, bv, cv));
    for (unsigned i = 0; i < 1; ++i)
      if (result[i] != reference(a[i], b[i], c[i], false)) return false;
    {
      auto c = sample<std::int16_t, 4>(state, iteration, 2);
      auto cv = std::bit_cast<int16x4_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("rdm"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("rdm"))) {
          auto result = std::bit_cast<std::array<std::int16_t, 1>>(rdm_api::sqrdmlah_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 1; ++i)
            if (result[i] != reference(a[i], b[i], c[L], false)) passed = false;
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<4>{});
      if (!passed) return false;
    }
    {
      auto c = sample<std::int16_t, 8>(state, iteration, 2);
      auto cv = std::bit_cast<int16x8_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("rdm"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("rdm"))) {
          auto result = std::bit_cast<std::array<std::int16_t, 1>>(rdm_api::sqrdmlah_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 1; ++i)
            if (result[i] != reference(a[i], b[i], c[L], false)) passed = false;
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<8>{});
      if (!passed) return false;
    }
  }
  return true;
}
__attribute__((target("rdm"), noinline))
bool check_sqrdmlah_int16x4(std::uint64_t & state) {
  for (unsigned iteration = 0; iteration < 2048; ++iteration) {
    auto a = sample<std::int16_t, 4>(state, iteration, 0);
    auto b = sample<std::int16_t, 4>(state, iteration, 1);
    auto c = sample<std::int16_t, 4>(state, iteration, 2);
    auto av = std::bit_cast<int16x4_t>(a);
    auto bv = std::bit_cast<int16x4_t>(b);
    auto cv = std::bit_cast<int16x4_t>(c);
    auto result = std::bit_cast<std::array<std::int16_t, 4>>(rdm_api::sqrdmlah<requirements>(av, bv, cv));
    for (unsigned i = 0; i < 4; ++i)
      if (result[i] != reference(a[i], b[i], c[i], false)) return false;
    {
      auto c = sample<std::int16_t, 4>(state, iteration, 2);
      auto cv = std::bit_cast<int16x4_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("rdm"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("rdm"))) {
          auto result = std::bit_cast<std::array<std::int16_t, 4>>(rdm_api::sqrdmlah_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 4; ++i)
            if (result[i] != reference(a[i], b[i], c[L], false)) passed = false;
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<4>{});
      if (!passed) return false;
    }
    {
      auto c = sample<std::int16_t, 8>(state, iteration, 2);
      auto cv = std::bit_cast<int16x8_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("rdm"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("rdm"))) {
          auto result = std::bit_cast<std::array<std::int16_t, 4>>(rdm_api::sqrdmlah_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 4; ++i)
            if (result[i] != reference(a[i], b[i], c[L], false)) passed = false;
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<8>{});
      if (!passed) return false;
    }
  }
  return true;
}
__attribute__((target("rdm"), noinline))
bool check_sqrdmlah_int16x8(std::uint64_t & state) {
  for (unsigned iteration = 0; iteration < 2048; ++iteration) {
    auto a = sample<std::int16_t, 8>(state, iteration, 0);
    auto b = sample<std::int16_t, 8>(state, iteration, 1);
    auto c = sample<std::int16_t, 8>(state, iteration, 2);
    auto av = std::bit_cast<int16x8_t>(a);
    auto bv = std::bit_cast<int16x8_t>(b);
    auto cv = std::bit_cast<int16x8_t>(c);
    auto result = std::bit_cast<std::array<std::int16_t, 8>>(rdm_api::sqrdmlah<requirements>(av, bv, cv));
    for (unsigned i = 0; i < 8; ++i)
      if (result[i] != reference(a[i], b[i], c[i], false)) return false;
    {
      auto c = sample<std::int16_t, 4>(state, iteration, 2);
      auto cv = std::bit_cast<int16x4_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("rdm"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("rdm"))) {
          auto result = std::bit_cast<std::array<std::int16_t, 8>>(rdm_api::sqrdmlah_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 8; ++i)
            if (result[i] != reference(a[i], b[i], c[L], false)) passed = false;
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<4>{});
      if (!passed) return false;
    }
    {
      auto c = sample<std::int16_t, 8>(state, iteration, 2);
      auto cv = std::bit_cast<int16x8_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("rdm"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("rdm"))) {
          auto result = std::bit_cast<std::array<std::int16_t, 8>>(rdm_api::sqrdmlah_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 8; ++i)
            if (result[i] != reference(a[i], b[i], c[L], false)) passed = false;
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<8>{});
      if (!passed) return false;
    }
  }
  return true;
}
__attribute__((target("rdm"), noinline))
bool check_sqrdmlah_int32(std::uint64_t & state) {
  for (unsigned iteration = 0; iteration < 2048; ++iteration) {
    auto a = sample<std::int32_t, 1>(state, iteration, 0);
    auto b = sample<std::int32_t, 1>(state, iteration, 1);
    auto c = sample<std::int32_t, 1>(state, iteration, 2);
    auto av = std::bit_cast<int32_t>(a);
    auto bv = std::bit_cast<int32_t>(b);
    auto cv = std::bit_cast<int32_t>(c);
    auto result = std::bit_cast<std::array<std::int32_t, 1>>(rdm_api::sqrdmlah<requirements>(av, bv, cv));
    for (unsigned i = 0; i < 1; ++i)
      if (result[i] != reference(a[i], b[i], c[i], false)) return false;
    {
      auto c = sample<std::int32_t, 2>(state, iteration, 2);
      auto cv = std::bit_cast<int32x2_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("rdm"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("rdm"))) {
          auto result = std::bit_cast<std::array<std::int32_t, 1>>(rdm_api::sqrdmlah_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 1; ++i)
            if (result[i] != reference(a[i], b[i], c[L], false)) passed = false;
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<2>{});
      if (!passed) return false;
    }
    {
      auto c = sample<std::int32_t, 4>(state, iteration, 2);
      auto cv = std::bit_cast<int32x4_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("rdm"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("rdm"))) {
          auto result = std::bit_cast<std::array<std::int32_t, 1>>(rdm_api::sqrdmlah_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 1; ++i)
            if (result[i] != reference(a[i], b[i], c[L], false)) passed = false;
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<4>{});
      if (!passed) return false;
    }
  }
  return true;
}
__attribute__((target("rdm"), noinline))
bool check_sqrdmlah_int32x2(std::uint64_t & state) {
  for (unsigned iteration = 0; iteration < 2048; ++iteration) {
    auto a = sample<std::int32_t, 2>(state, iteration, 0);
    auto b = sample<std::int32_t, 2>(state, iteration, 1);
    auto c = sample<std::int32_t, 2>(state, iteration, 2);
    auto av = std::bit_cast<int32x2_t>(a);
    auto bv = std::bit_cast<int32x2_t>(b);
    auto cv = std::bit_cast<int32x2_t>(c);
    auto result = std::bit_cast<std::array<std::int32_t, 2>>(rdm_api::sqrdmlah<requirements>(av, bv, cv));
    for (unsigned i = 0; i < 2; ++i)
      if (result[i] != reference(a[i], b[i], c[i], false)) return false;
    {
      auto c = sample<std::int32_t, 2>(state, iteration, 2);
      auto cv = std::bit_cast<int32x2_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("rdm"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("rdm"))) {
          auto result = std::bit_cast<std::array<std::int32_t, 2>>(rdm_api::sqrdmlah_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 2; ++i)
            if (result[i] != reference(a[i], b[i], c[L], false)) passed = false;
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<2>{});
      if (!passed) return false;
    }
    {
      auto c = sample<std::int32_t, 4>(state, iteration, 2);
      auto cv = std::bit_cast<int32x4_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("rdm"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("rdm"))) {
          auto result = std::bit_cast<std::array<std::int32_t, 2>>(rdm_api::sqrdmlah_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 2; ++i)
            if (result[i] != reference(a[i], b[i], c[L], false)) passed = false;
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<4>{});
      if (!passed) return false;
    }
  }
  return true;
}
__attribute__((target("rdm"), noinline))
bool check_sqrdmlah_int32x4(std::uint64_t & state) {
  for (unsigned iteration = 0; iteration < 2048; ++iteration) {
    auto a = sample<std::int32_t, 4>(state, iteration, 0);
    auto b = sample<std::int32_t, 4>(state, iteration, 1);
    auto c = sample<std::int32_t, 4>(state, iteration, 2);
    auto av = std::bit_cast<int32x4_t>(a);
    auto bv = std::bit_cast<int32x4_t>(b);
    auto cv = std::bit_cast<int32x4_t>(c);
    auto result = std::bit_cast<std::array<std::int32_t, 4>>(rdm_api::sqrdmlah<requirements>(av, bv, cv));
    for (unsigned i = 0; i < 4; ++i)
      if (result[i] != reference(a[i], b[i], c[i], false)) return false;
    {
      auto c = sample<std::int32_t, 2>(state, iteration, 2);
      auto cv = std::bit_cast<int32x2_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("rdm"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("rdm"))) {
          auto result = std::bit_cast<std::array<std::int32_t, 4>>(rdm_api::sqrdmlah_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 4; ++i)
            if (result[i] != reference(a[i], b[i], c[L], false)) passed = false;
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<2>{});
      if (!passed) return false;
    }
    {
      auto c = sample<std::int32_t, 4>(state, iteration, 2);
      auto cv = std::bit_cast<int32x4_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("rdm"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("rdm"))) {
          auto result = std::bit_cast<std::array<std::int32_t, 4>>(rdm_api::sqrdmlah_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 4; ++i)
            if (result[i] != reference(a[i], b[i], c[L], false)) passed = false;
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<4>{});
      if (!passed) return false;
    }
  }
  return true;
}
__attribute__((target("rdm"), noinline))
bool check_sqrdmlsh_int16(std::uint64_t & state) {
  for (unsigned iteration = 0; iteration < 2048; ++iteration) {
    auto a = sample<std::int16_t, 1>(state, iteration, 0);
    auto b = sample<std::int16_t, 1>(state, iteration, 1);
    auto c = sample<std::int16_t, 1>(state, iteration, 2);
    auto av = std::bit_cast<int16_t>(a);
    auto bv = std::bit_cast<int16_t>(b);
    auto cv = std::bit_cast<int16_t>(c);
    auto result = std::bit_cast<std::array<std::int16_t, 1>>(rdm_api::sqrdmlsh<requirements>(av, bv, cv));
    for (unsigned i = 0; i < 1; ++i)
      if (result[i] != reference(a[i], b[i], c[i], true)) return false;
    {
      auto c = sample<std::int16_t, 4>(state, iteration, 2);
      auto cv = std::bit_cast<int16x4_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("rdm"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("rdm"))) {
          auto result = std::bit_cast<std::array<std::int16_t, 1>>(rdm_api::sqrdmlsh_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 1; ++i)
            if (result[i] != reference(a[i], b[i], c[L], true)) passed = false;
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<4>{});
      if (!passed) return false;
    }
    {
      auto c = sample<std::int16_t, 8>(state, iteration, 2);
      auto cv = std::bit_cast<int16x8_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("rdm"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("rdm"))) {
          auto result = std::bit_cast<std::array<std::int16_t, 1>>(rdm_api::sqrdmlsh_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 1; ++i)
            if (result[i] != reference(a[i], b[i], c[L], true)) passed = false;
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<8>{});
      if (!passed) return false;
    }
  }
  return true;
}
__attribute__((target("rdm"), noinline))
bool check_sqrdmlsh_int16x4(std::uint64_t & state) {
  for (unsigned iteration = 0; iteration < 2048; ++iteration) {
    auto a = sample<std::int16_t, 4>(state, iteration, 0);
    auto b = sample<std::int16_t, 4>(state, iteration, 1);
    auto c = sample<std::int16_t, 4>(state, iteration, 2);
    auto av = std::bit_cast<int16x4_t>(a);
    auto bv = std::bit_cast<int16x4_t>(b);
    auto cv = std::bit_cast<int16x4_t>(c);
    auto result = std::bit_cast<std::array<std::int16_t, 4>>(rdm_api::sqrdmlsh<requirements>(av, bv, cv));
    for (unsigned i = 0; i < 4; ++i)
      if (result[i] != reference(a[i], b[i], c[i], true)) return false;
    {
      auto c = sample<std::int16_t, 4>(state, iteration, 2);
      auto cv = std::bit_cast<int16x4_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("rdm"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("rdm"))) {
          auto result = std::bit_cast<std::array<std::int16_t, 4>>(rdm_api::sqrdmlsh_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 4; ++i)
            if (result[i] != reference(a[i], b[i], c[L], true)) passed = false;
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<4>{});
      if (!passed) return false;
    }
    {
      auto c = sample<std::int16_t, 8>(state, iteration, 2);
      auto cv = std::bit_cast<int16x8_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("rdm"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("rdm"))) {
          auto result = std::bit_cast<std::array<std::int16_t, 4>>(rdm_api::sqrdmlsh_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 4; ++i)
            if (result[i] != reference(a[i], b[i], c[L], true)) passed = false;
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<8>{});
      if (!passed) return false;
    }
  }
  return true;
}
__attribute__((target("rdm"), noinline))
bool check_sqrdmlsh_int16x8(std::uint64_t & state) {
  for (unsigned iteration = 0; iteration < 2048; ++iteration) {
    auto a = sample<std::int16_t, 8>(state, iteration, 0);
    auto b = sample<std::int16_t, 8>(state, iteration, 1);
    auto c = sample<std::int16_t, 8>(state, iteration, 2);
    auto av = std::bit_cast<int16x8_t>(a);
    auto bv = std::bit_cast<int16x8_t>(b);
    auto cv = std::bit_cast<int16x8_t>(c);
    auto result = std::bit_cast<std::array<std::int16_t, 8>>(rdm_api::sqrdmlsh<requirements>(av, bv, cv));
    for (unsigned i = 0; i < 8; ++i)
      if (result[i] != reference(a[i], b[i], c[i], true)) return false;
    {
      auto c = sample<std::int16_t, 4>(state, iteration, 2);
      auto cv = std::bit_cast<int16x4_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("rdm"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("rdm"))) {
          auto result = std::bit_cast<std::array<std::int16_t, 8>>(rdm_api::sqrdmlsh_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 8; ++i)
            if (result[i] != reference(a[i], b[i], c[L], true)) passed = false;
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<4>{});
      if (!passed) return false;
    }
    {
      auto c = sample<std::int16_t, 8>(state, iteration, 2);
      auto cv = std::bit_cast<int16x8_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("rdm"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("rdm"))) {
          auto result = std::bit_cast<std::array<std::int16_t, 8>>(rdm_api::sqrdmlsh_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 8; ++i)
            if (result[i] != reference(a[i], b[i], c[L], true)) passed = false;
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<8>{});
      if (!passed) return false;
    }
  }
  return true;
}
__attribute__((target("rdm"), noinline))
bool check_sqrdmlsh_int32(std::uint64_t & state) {
  for (unsigned iteration = 0; iteration < 2048; ++iteration) {
    auto a = sample<std::int32_t, 1>(state, iteration, 0);
    auto b = sample<std::int32_t, 1>(state, iteration, 1);
    auto c = sample<std::int32_t, 1>(state, iteration, 2);
    auto av = std::bit_cast<int32_t>(a);
    auto bv = std::bit_cast<int32_t>(b);
    auto cv = std::bit_cast<int32_t>(c);
    auto result = std::bit_cast<std::array<std::int32_t, 1>>(rdm_api::sqrdmlsh<requirements>(av, bv, cv));
    for (unsigned i = 0; i < 1; ++i)
      if (result[i] != reference(a[i], b[i], c[i], true)) return false;
    {
      auto c = sample<std::int32_t, 2>(state, iteration, 2);
      auto cv = std::bit_cast<int32x2_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("rdm"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("rdm"))) {
          auto result = std::bit_cast<std::array<std::int32_t, 1>>(rdm_api::sqrdmlsh_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 1; ++i)
            if (result[i] != reference(a[i], b[i], c[L], true)) passed = false;
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<2>{});
      if (!passed) return false;
    }
    {
      auto c = sample<std::int32_t, 4>(state, iteration, 2);
      auto cv = std::bit_cast<int32x4_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("rdm"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("rdm"))) {
          auto result = std::bit_cast<std::array<std::int32_t, 1>>(rdm_api::sqrdmlsh_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 1; ++i)
            if (result[i] != reference(a[i], b[i], c[L], true)) passed = false;
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<4>{});
      if (!passed) return false;
    }
  }
  return true;
}
__attribute__((target("rdm"), noinline))
bool check_sqrdmlsh_int32x2(std::uint64_t & state) {
  for (unsigned iteration = 0; iteration < 2048; ++iteration) {
    auto a = sample<std::int32_t, 2>(state, iteration, 0);
    auto b = sample<std::int32_t, 2>(state, iteration, 1);
    auto c = sample<std::int32_t, 2>(state, iteration, 2);
    auto av = std::bit_cast<int32x2_t>(a);
    auto bv = std::bit_cast<int32x2_t>(b);
    auto cv = std::bit_cast<int32x2_t>(c);
    auto result = std::bit_cast<std::array<std::int32_t, 2>>(rdm_api::sqrdmlsh<requirements>(av, bv, cv));
    for (unsigned i = 0; i < 2; ++i)
      if (result[i] != reference(a[i], b[i], c[i], true)) return false;
    {
      auto c = sample<std::int32_t, 2>(state, iteration, 2);
      auto cv = std::bit_cast<int32x2_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("rdm"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("rdm"))) {
          auto result = std::bit_cast<std::array<std::int32_t, 2>>(rdm_api::sqrdmlsh_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 2; ++i)
            if (result[i] != reference(a[i], b[i], c[L], true)) passed = false;
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<2>{});
      if (!passed) return false;
    }
    {
      auto c = sample<std::int32_t, 4>(state, iteration, 2);
      auto cv = std::bit_cast<int32x4_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("rdm"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("rdm"))) {
          auto result = std::bit_cast<std::array<std::int32_t, 2>>(rdm_api::sqrdmlsh_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 2; ++i)
            if (result[i] != reference(a[i], b[i], c[L], true)) passed = false;
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<4>{});
      if (!passed) return false;
    }
  }
  return true;
}
__attribute__((target("rdm"), noinline))
bool check_sqrdmlsh_int32x4(std::uint64_t & state) {
  for (unsigned iteration = 0; iteration < 2048; ++iteration) {
    auto a = sample<std::int32_t, 4>(state, iteration, 0);
    auto b = sample<std::int32_t, 4>(state, iteration, 1);
    auto c = sample<std::int32_t, 4>(state, iteration, 2);
    auto av = std::bit_cast<int32x4_t>(a);
    auto bv = std::bit_cast<int32x4_t>(b);
    auto cv = std::bit_cast<int32x4_t>(c);
    auto result = std::bit_cast<std::array<std::int32_t, 4>>(rdm_api::sqrdmlsh<requirements>(av, bv, cv));
    for (unsigned i = 0; i < 4; ++i)
      if (result[i] != reference(a[i], b[i], c[i], true)) return false;
    {
      auto c = sample<std::int32_t, 2>(state, iteration, 2);
      auto cv = std::bit_cast<int32x2_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("rdm"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("rdm"))) {
          auto result = std::bit_cast<std::array<std::int32_t, 4>>(rdm_api::sqrdmlsh_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 4; ++i)
            if (result[i] != reference(a[i], b[i], c[L], true)) passed = false;
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<2>{});
      if (!passed) return false;
    }
    {
      auto c = sample<std::int32_t, 4>(state, iteration, 2);
      auto cv = std::bit_cast<int32x4_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("rdm"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("rdm"))) {
          auto result = std::bit_cast<std::array<std::int32_t, 4>>(rdm_api::sqrdmlsh_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 4; ++i)
            if (result[i] != reference(a[i], b[i], c[L], true)) passed = false;
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<4>{});
      if (!passed) return false;
    }
  }
  return true;
}
int main(int argc, char **) {
  auto cpu = native::observe_arm_capabilities();
  if (!native::classify_isa(cpu, requirements, NATIVE_TARGET_MINIMUM).admitted()) {
    std::puts("RDM unavailable; static admission checks passed.");
    return 77;
  }
  if (!check_saturation_flag()) { std::puts("FPSR.QC failed"); return 1; }
  std::uint64_t state = 0x9e3779b97f4a7c15ull ^ static_cast<unsigned>(argc);
  if (!check_sqrdmlah_int16(state)) { std::puts("sqrdmlah_int16 failed"); return 1; }
  if (!check_sqrdmlah_int16x4(state)) { std::puts("sqrdmlah_int16x4 failed"); return 1; }
  if (!check_sqrdmlah_int16x8(state)) { std::puts("sqrdmlah_int16x8 failed"); return 1; }
  if (!check_sqrdmlah_int32(state)) { std::puts("sqrdmlah_int32 failed"); return 1; }
  if (!check_sqrdmlah_int32x2(state)) { std::puts("sqrdmlah_int32x2 failed"); return 1; }
  if (!check_sqrdmlah_int32x4(state)) { std::puts("sqrdmlah_int32x4 failed"); return 1; }
  if (!check_sqrdmlsh_int16(state)) { std::puts("sqrdmlsh_int16 failed"); return 1; }
  if (!check_sqrdmlsh_int16x4(state)) { std::puts("sqrdmlsh_int16x4 failed"); return 1; }
  if (!check_sqrdmlsh_int16x8(state)) { std::puts("sqrdmlsh_int16x8 failed"); return 1; }
  if (!check_sqrdmlsh_int32(state)) { std::puts("sqrdmlsh_int32 failed"); return 1; }
  if (!check_sqrdmlsh_int32x2(state)) { std::puts("sqrdmlsh_int32x2 failed"); return 1; }
  if (!check_sqrdmlsh_int32x4(state)) { std::puts("sqrdmlsh_int32x4 failed"); return 1; }
  return 0;
}
