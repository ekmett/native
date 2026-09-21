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
import native.arm.dotprod;
#elif NATIVE_TEST_INTERFACE == 2
import native;
#else
#error Select header, granular module or omnibus interface.
#endif
#include "simd_adapter.h"
#include "simd_contract.h"
constexpr auto requirements = native::feature_closure(native::arm_feature::dotprod);
consteval bool admission_contract() {
  native::arm_capabilities cpu{};
  for (auto f : {native::arm_feature::neon, native::arm_feature::dotprod}) {
    cpu.present.set(f, true);
    cpu.observed.set(f, true);
  }
  if (!native::classify_isa(cpu, requirements).admitted()) return false;
  cpu.present.set(native::arm_feature::dotprod, false);
  if (native::classify_isa(cpu, requirements).admitted()) return false;
  cpu.present.set(native::arm_feature::dotprod, true);
  cpu.observed.set(native::arm_feature::dotprod, false);
  return !native::classify_isa(cpu, requirements).admitted();
}
static_assert(admission_contract());
template<native::isa<native::arm> A, class R, class X>
concept has_sdot = requires(R r, X x) {
  { dotprod_api::sdot<A>(r, x, x) } noexcept -> std::same_as<R>;
};
template<native::isa<native::arm> A, int Lane, class R, class X, class Y>
concept has_sdot_lane = requires(R r, X x, Y y) {
  { dotprod_api::sdot_lane<A, Lane>(r, x, y) } noexcept -> std::same_as<R>;
};
template<native::isa<native::arm> A, class R, class X>
concept has_udot = requires(R r, X x) {
  { dotprod_api::udot<A>(r, x, x) } noexcept -> std::same_as<R>;
};
template<native::isa<native::arm> A, int Lane, class R, class X, class Y>
concept has_udot_lane = requires(R r, X x, Y y) {
  { dotprod_api::udot_lane<A, Lane>(r, x, y) } noexcept -> std::same_as<R>;
};
template<class T, std::size_t N>
std::array<T, N> sample(std::uint64_t & state, unsigned iteration, unsigned operand) {
  constexpr auto lo = std::numeric_limits<T>::min();
  constexpr auto hi = std::numeric_limits<T>::max();
  constexpr T edges[]{lo, hi, T{0}, T{1}, static_cast<T>(-1),
    static_cast<T>(lo / 2), static_cast<T>(hi / 2)};
  std::array<T, N> result;
  for (auto & x : result) {
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    // Positive/negative signed overflow and unsigned carry are directed first.
    if (iteration < 2)
      x = iteration == 0 ? hi : operand == 2 ? hi : lo;
    else
      x = iteration < 64 ? edges[state % std::size(edges)] : static_cast<T>(state);
  }
  return result;
}
static_assert(has_sdot<requirements, int32x2_t, int8x8_t>);
static_assert(has_sdot<native::isa<native::arm>(native::arm_feature::dotprod), int32x2_t, int8x8_t>);
static_assert(!has_sdot<native::scalar, int32x2_t, int8x8_t>);
static_assert(!has_sdot<native::neon, int32x2_t, int8x8_t>);
static_assert(!has_sdot<native::isa<native::arm>(native::arm_feature::rdm), int32x2_t, int8x8_t>);
static_assert(has_sdot_lane<requirements, 1, int32x2_t, int8x8_t, int8x8_t>);
static_assert(!has_sdot_lane<requirements, -1, int32x2_t, int8x8_t, int8x8_t>);
static_assert(!has_sdot_lane<requirements, 2, int32x2_t, int8x8_t, int8x8_t>);
static_assert(!has_sdot_lane<native::neon, 0, int32x2_t, int8x8_t, int8x8_t>);
static_assert(has_sdot_lane<requirements, 3, int32x2_t, int8x8_t, int8x16_t>);
static_assert(!has_sdot_lane<requirements, -1, int32x2_t, int8x8_t, int8x16_t>);
static_assert(!has_sdot_lane<requirements, 4, int32x2_t, int8x8_t, int8x16_t>);
static_assert(!has_sdot_lane<native::neon, 0, int32x2_t, int8x8_t, int8x16_t>);
static_assert(has_sdot<requirements, int32x4_t, int8x16_t>);
static_assert(has_sdot<native::isa<native::arm>(native::arm_feature::dotprod), int32x4_t, int8x16_t>);
static_assert(!has_sdot<native::scalar, int32x4_t, int8x16_t>);
static_assert(!has_sdot<native::neon, int32x4_t, int8x16_t>);
static_assert(!has_sdot<native::isa<native::arm>(native::arm_feature::rdm), int32x4_t, int8x16_t>);
static_assert(has_sdot_lane<requirements, 1, int32x4_t, int8x16_t, int8x8_t>);
static_assert(!has_sdot_lane<requirements, -1, int32x4_t, int8x16_t, int8x8_t>);
static_assert(!has_sdot_lane<requirements, 2, int32x4_t, int8x16_t, int8x8_t>);
static_assert(!has_sdot_lane<native::neon, 0, int32x4_t, int8x16_t, int8x8_t>);
static_assert(has_sdot_lane<requirements, 3, int32x4_t, int8x16_t, int8x16_t>);
static_assert(!has_sdot_lane<requirements, -1, int32x4_t, int8x16_t, int8x16_t>);
static_assert(!has_sdot_lane<requirements, 4, int32x4_t, int8x16_t, int8x16_t>);
static_assert(!has_sdot_lane<native::neon, 0, int32x4_t, int8x16_t, int8x16_t>);
static_assert(has_udot<requirements, uint32x2_t, uint8x8_t>);
static_assert(has_udot<native::isa<native::arm>(native::arm_feature::dotprod), uint32x2_t, uint8x8_t>);
static_assert(!has_udot<native::scalar, uint32x2_t, uint8x8_t>);
static_assert(!has_udot<native::neon, uint32x2_t, uint8x8_t>);
static_assert(!has_udot<native::isa<native::arm>(native::arm_feature::rdm), uint32x2_t, uint8x8_t>);
static_assert(has_udot_lane<requirements, 1, uint32x2_t, uint8x8_t, uint8x8_t>);
static_assert(!has_udot_lane<requirements, -1, uint32x2_t, uint8x8_t, uint8x8_t>);
static_assert(!has_udot_lane<requirements, 2, uint32x2_t, uint8x8_t, uint8x8_t>);
static_assert(!has_udot_lane<native::neon, 0, uint32x2_t, uint8x8_t, uint8x8_t>);
static_assert(has_udot_lane<requirements, 3, uint32x2_t, uint8x8_t, uint8x16_t>);
static_assert(!has_udot_lane<requirements, -1, uint32x2_t, uint8x8_t, uint8x16_t>);
static_assert(!has_udot_lane<requirements, 4, uint32x2_t, uint8x8_t, uint8x16_t>);
static_assert(!has_udot_lane<native::neon, 0, uint32x2_t, uint8x8_t, uint8x16_t>);
static_assert(has_udot<requirements, uint32x4_t, uint8x16_t>);
static_assert(has_udot<native::isa<native::arm>(native::arm_feature::dotprod), uint32x4_t, uint8x16_t>);
static_assert(!has_udot<native::scalar, uint32x4_t, uint8x16_t>);
static_assert(!has_udot<native::neon, uint32x4_t, uint8x16_t>);
static_assert(!has_udot<native::isa<native::arm>(native::arm_feature::rdm), uint32x4_t, uint8x16_t>);
static_assert(has_udot_lane<requirements, 1, uint32x4_t, uint8x16_t, uint8x8_t>);
static_assert(!has_udot_lane<requirements, -1, uint32x4_t, uint8x16_t, uint8x8_t>);
static_assert(!has_udot_lane<requirements, 2, uint32x4_t, uint8x16_t, uint8x8_t>);
static_assert(!has_udot_lane<native::neon, 0, uint32x4_t, uint8x16_t, uint8x8_t>);
static_assert(has_udot_lane<requirements, 3, uint32x4_t, uint8x16_t, uint8x16_t>);
static_assert(!has_udot_lane<requirements, -1, uint32x4_t, uint8x16_t, uint8x16_t>);
static_assert(!has_udot_lane<requirements, 4, uint32x4_t, uint8x16_t, uint8x16_t>);
static_assert(!has_udot_lane<native::neon, 0, uint32x4_t, uint8x16_t, uint8x16_t>);
__attribute__((target("dotprod"), noinline))
bool check_sdot_int32x2(std::uint64_t & state) {
  for (unsigned iteration = 0; iteration < 2048; ++iteration) {
    auto a = sample<std::int32_t, 2>(state, iteration, 0);
    auto b = sample<std::int8_t, 8>(state, iteration, 1);
    auto c = sample<std::int8_t, 8>(state, iteration, 2);
    auto av = std::bit_cast<int32x2_t>(a);
    auto bv = std::bit_cast<int8x8_t>(b);
    auto cv = std::bit_cast<int8x8_t>(c);
    auto result = std::bit_cast<std::array<std::int32_t, 2>>(dotprod_api::sdot<requirements>(av, bv, cv));
    for (unsigned i = 0; i < 2; ++i) {
      auto expected = static_cast<std::uint32_t>(a[i]);
      for (unsigned j = 0; j < 4; ++j)
        expected += static_cast<std::uint32_t>(static_cast<std::int32_t>(b[4*i+j]) * c[4*i+j]);
      if (static_cast<std::uint32_t>(result[i]) != expected) return false;
    }
    {
      auto c = sample<std::int8_t, 8>(state, iteration, 2);
      auto cv = std::bit_cast<int8x8_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("dotprod"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("dotprod"))) {
          auto result = std::bit_cast<std::array<std::int32_t, 2>>(dotprod_api::sdot_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 2; ++i) {
            auto expected = static_cast<std::uint32_t>(a[i]);
            for (unsigned j = 0; j < 4; ++j)
              expected += static_cast<std::uint32_t>(static_cast<std::int32_t>(b[4*i+j]) * c[4*L+j]);
            if (static_cast<std::uint32_t>(result[i]) != expected) passed = false;
          }
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<2>{});
      if (!passed) return false;
    }
    {
      auto c = sample<std::int8_t, 16>(state, iteration, 2);
      auto cv = std::bit_cast<int8x16_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("dotprod"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("dotprod"))) {
          auto result = std::bit_cast<std::array<std::int32_t, 2>>(dotprod_api::sdot_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 2; ++i) {
            auto expected = static_cast<std::uint32_t>(a[i]);
            for (unsigned j = 0; j < 4; ++j)
              expected += static_cast<std::uint32_t>(static_cast<std::int32_t>(b[4*i+j]) * c[4*L+j]);
            if (static_cast<std::uint32_t>(result[i]) != expected) passed = false;
          }
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<4>{});
      if (!passed) return false;
    }
  }
  return true;
}
__attribute__((target("dotprod"), noinline))
bool check_sdot_int32x4(std::uint64_t & state) {
  for (unsigned iteration = 0; iteration < 2048; ++iteration) {
    auto a = sample<std::int32_t, 4>(state, iteration, 0);
    auto b = sample<std::int8_t, 16>(state, iteration, 1);
    auto c = sample<std::int8_t, 16>(state, iteration, 2);
    auto av = std::bit_cast<int32x4_t>(a);
    auto bv = std::bit_cast<int8x16_t>(b);
    auto cv = std::bit_cast<int8x16_t>(c);
    auto result = std::bit_cast<std::array<std::int32_t, 4>>(dotprod_api::sdot<requirements>(av, bv, cv));
    for (unsigned i = 0; i < 4; ++i) {
      auto expected = static_cast<std::uint32_t>(a[i]);
      for (unsigned j = 0; j < 4; ++j)
        expected += static_cast<std::uint32_t>(static_cast<std::int32_t>(b[4*i+j]) * c[4*i+j]);
      if (static_cast<std::uint32_t>(result[i]) != expected) return false;
    }
    {
      auto c = sample<std::int8_t, 8>(state, iteration, 2);
      auto cv = std::bit_cast<int8x8_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("dotprod"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("dotprod"))) {
          auto result = std::bit_cast<std::array<std::int32_t, 4>>(dotprod_api::sdot_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 4; ++i) {
            auto expected = static_cast<std::uint32_t>(a[i]);
            for (unsigned j = 0; j < 4; ++j)
              expected += static_cast<std::uint32_t>(static_cast<std::int32_t>(b[4*i+j]) * c[4*L+j]);
            if (static_cast<std::uint32_t>(result[i]) != expected) passed = false;
          }
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<2>{});
      if (!passed) return false;
    }
    {
      auto c = sample<std::int8_t, 16>(state, iteration, 2);
      auto cv = std::bit_cast<int8x16_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("dotprod"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("dotprod"))) {
          auto result = std::bit_cast<std::array<std::int32_t, 4>>(dotprod_api::sdot_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 4; ++i) {
            auto expected = static_cast<std::uint32_t>(a[i]);
            for (unsigned j = 0; j < 4; ++j)
              expected += static_cast<std::uint32_t>(static_cast<std::int32_t>(b[4*i+j]) * c[4*L+j]);
            if (static_cast<std::uint32_t>(result[i]) != expected) passed = false;
          }
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<4>{});
      if (!passed) return false;
    }
  }
  return true;
}
__attribute__((target("dotprod"), noinline))
bool check_udot_uint32x2(std::uint64_t & state) {
  for (unsigned iteration = 0; iteration < 2048; ++iteration) {
    auto a = sample<std::uint32_t, 2>(state, iteration, 0);
    auto b = sample<std::uint8_t, 8>(state, iteration, 1);
    auto c = sample<std::uint8_t, 8>(state, iteration, 2);
    auto av = std::bit_cast<uint32x2_t>(a);
    auto bv = std::bit_cast<uint8x8_t>(b);
    auto cv = std::bit_cast<uint8x8_t>(c);
    auto result = std::bit_cast<std::array<std::uint32_t, 2>>(dotprod_api::udot<requirements>(av, bv, cv));
    for (unsigned i = 0; i < 2; ++i) {
      auto expected = static_cast<std::uint32_t>(a[i]);
      for (unsigned j = 0; j < 4; ++j)
        expected += static_cast<std::uint32_t>(static_cast<std::int32_t>(b[4*i+j]) * c[4*i+j]);
      if (static_cast<std::uint32_t>(result[i]) != expected) return false;
    }
    {
      auto c = sample<std::uint8_t, 8>(state, iteration, 2);
      auto cv = std::bit_cast<uint8x8_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("dotprod"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("dotprod"))) {
          auto result = std::bit_cast<std::array<std::uint32_t, 2>>(dotprod_api::udot_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 2; ++i) {
            auto expected = static_cast<std::uint32_t>(a[i]);
            for (unsigned j = 0; j < 4; ++j)
              expected += static_cast<std::uint32_t>(static_cast<std::int32_t>(b[4*i+j]) * c[4*L+j]);
            if (static_cast<std::uint32_t>(result[i]) != expected) passed = false;
          }
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<2>{});
      if (!passed) return false;
    }
    {
      auto c = sample<std::uint8_t, 16>(state, iteration, 2);
      auto cv = std::bit_cast<uint8x16_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("dotprod"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("dotprod"))) {
          auto result = std::bit_cast<std::array<std::uint32_t, 2>>(dotprod_api::udot_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 2; ++i) {
            auto expected = static_cast<std::uint32_t>(a[i]);
            for (unsigned j = 0; j < 4; ++j)
              expected += static_cast<std::uint32_t>(static_cast<std::int32_t>(b[4*i+j]) * c[4*L+j]);
            if (static_cast<std::uint32_t>(result[i]) != expected) passed = false;
          }
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<4>{});
      if (!passed) return false;
    }
  }
  return true;
}
__attribute__((target("dotprod"), noinline))
bool check_udot_uint32x4(std::uint64_t & state) {
  for (unsigned iteration = 0; iteration < 2048; ++iteration) {
    auto a = sample<std::uint32_t, 4>(state, iteration, 0);
    auto b = sample<std::uint8_t, 16>(state, iteration, 1);
    auto c = sample<std::uint8_t, 16>(state, iteration, 2);
    auto av = std::bit_cast<uint32x4_t>(a);
    auto bv = std::bit_cast<uint8x16_t>(b);
    auto cv = std::bit_cast<uint8x16_t>(c);
    auto result = std::bit_cast<std::array<std::uint32_t, 4>>(dotprod_api::udot<requirements>(av, bv, cv));
    for (unsigned i = 0; i < 4; ++i) {
      auto expected = static_cast<std::uint32_t>(a[i]);
      for (unsigned j = 0; j < 4; ++j)
        expected += static_cast<std::uint32_t>(static_cast<std::int32_t>(b[4*i+j]) * c[4*i+j]);
      if (static_cast<std::uint32_t>(result[i]) != expected) return false;
    }
    {
      auto c = sample<std::uint8_t, 8>(state, iteration, 2);
      auto cv = std::bit_cast<uint8x8_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("dotprod"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("dotprod"))) {
          auto result = std::bit_cast<std::array<std::uint32_t, 4>>(dotprod_api::udot_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 4; ++i) {
            auto expected = static_cast<std::uint32_t>(a[i]);
            for (unsigned j = 0; j < 4; ++j)
              expected += static_cast<std::uint32_t>(static_cast<std::int32_t>(b[4*i+j]) * c[4*L+j]);
            if (static_cast<std::uint32_t>(result[i]) != expected) passed = false;
          }
        };
        (check.template operator()<Lane>(), ...);
      }(std::make_index_sequence<2>{});
      if (!passed) return false;
    }
    {
      auto c = sample<std::uint8_t, 16>(state, iteration, 2);
      auto cv = std::bit_cast<uint8x16_t>(c);
      bool passed = true;
      [&]<std::size_t... Lane>(std::index_sequence<Lane...>) __attribute__((target("dotprod"))) {
        auto check = [&]<std::size_t L>() __attribute__((target("dotprod"))) {
          auto result = std::bit_cast<std::array<std::uint32_t, 4>>(dotprod_api::udot_lane<requirements, L>(av, bv, cv));
          for (unsigned i = 0; i < 4; ++i) {
            auto expected = static_cast<std::uint32_t>(a[i]);
            for (unsigned j = 0; j < 4; ++j)
              expected += static_cast<std::uint32_t>(static_cast<std::int32_t>(b[4*i+j]) * c[4*L+j]);
            if (static_cast<std::uint32_t>(result[i]) != expected) passed = false;
          }
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
    std::puts("DOTPROD unavailable; static admission checks passed.");
    return 77;
  }
  std::uint64_t state = 0x9e3779b97f4a7c15ull ^ static_cast<unsigned>(argc);
  if (!check_sdot_int32x2(state)) { std::puts("sdot_int32x2 failed"); return 1; }
  if (!check_sdot_int32x4(state)) { std::puts("sdot_int32x4 failed"); return 1; }
  if (!check_udot_uint32x2(state)) { std::puts("udot_uint32x2 failed"); return 1; }
  if (!check_udot_uint32x4(state)) { std::puts("udot_uint32x4 failed"); return 1; }
  return 0;
}
