// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "simd_adapter.h"
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <concepts>
#include <utility>
namespace fcma_simd_contract {
  constexpr auto arch_0 = native::feature_closure(native::isa<native::arm>(native::arm_feature::complxnum));
  template<class... A> concept accepts_0 = requires(A... a) { native::fcadd<arch_0, 90>(a...); };
  static_assert(accepts_0<native::simd<float, 2, arch_0>, native::simd<float, 2, arch_0>>);
  static_assert(!accepts_0<native::simd<std::uint32_t, 2, arch_0>, native::simd<float, 2, arch_0>>);
  static_assert(std::same_as<decltype(native::fcadd<arch_0, 90>(std::declval<native::simd<float, 2, arch_0>>(), std::declval<native::simd<float, 2, arch_0>>())), native::simd<float, 2, arch_0>>);
  static_assert(!accepts_0<float32x2_t, float32x2_t>);
  static_assert(!accepts_0<native::simd<float, 2, (arch_0 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<float, 2, arch_0>>);
  constexpr auto arch_1 = native::feature_closure(native::isa<native::arm>(native::arm_feature::complxnum));
  template<class... A> concept accepts_1 = requires(A... a) { native::fcmla<arch_1, 90>(a...); };
  static_assert(accepts_1<native::simd<float, 2, arch_1>, native::simd<float, 2, arch_1>, native::simd<float, 2, arch_1>>);
  static_assert(!accepts_1<native::simd<std::uint32_t, 2, arch_1>, native::simd<float, 2, arch_1>, native::simd<float, 2, arch_1>>);
  static_assert(std::same_as<decltype(native::fcmla<arch_1, 90>(std::declval<native::simd<float, 2, arch_1>>(), std::declval<native::simd<float, 2, arch_1>>(), std::declval<native::simd<float, 2, arch_1>>())), native::simd<float, 2, arch_1>>);
  static_assert(!accepts_1<float32x2_t, float32x2_t, float32x2_t>);
  static_assert(!accepts_1<native::simd<float, 2, (arch_1 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<float, 2, arch_1>, native::simd<float, 2, arch_1>>);
  constexpr auto arch_2 = native::feature_closure(native::isa<native::arm>(native::arm_feature::complxnum));
  template<class... A> concept accepts_2 = requires(A... a) { native::fcmla_lane<arch_2, 90, 0>(a...); };
  static_assert(accepts_2<native::simd<float, 2, arch_2>, native::simd<float, 2, arch_2>, native::simd<float, 2, arch_2>>);
  static_assert(!accepts_2<native::simd<std::uint32_t, 2, arch_2>, native::simd<float, 2, arch_2>, native::simd<float, 2, arch_2>>);
  static_assert(std::same_as<decltype(native::fcmla_lane<arch_2, 90, 0>(std::declval<native::simd<float, 2, arch_2>>(), std::declval<native::simd<float, 2, arch_2>>(), std::declval<native::simd<float, 2, arch_2>>())), native::simd<float, 2, arch_2>>);
  static_assert(!accepts_2<float32x2_t, float32x2_t, float32x2_t>);
  static_assert(!accepts_2<native::simd<float, 2, (arch_2 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<float, 2, arch_2>, native::simd<float, 2, arch_2>>);
  constexpr auto arch_3 = native::feature_closure(native::isa<native::arm>(native::arm_feature::complxnum));
  template<class... A> concept accepts_3 = requires(A... a) { native::fcmla_lane<arch_3, 90, 0>(a...); };
  static_assert(accepts_3<native::simd<float, 2, arch_3>, native::simd<float, 2, arch_3>, native::simd<float, 4, arch_3>>);
  static_assert(!accepts_3<native::simd<std::uint32_t, 2, arch_3>, native::simd<float, 2, arch_3>, native::simd<float, 4, arch_3>>);
  static_assert(std::same_as<decltype(native::fcmla_lane<arch_3, 90, 0>(std::declval<native::simd<float, 2, arch_3>>(), std::declval<native::simd<float, 2, arch_3>>(), std::declval<native::simd<float, 4, arch_3>>())), native::simd<float, 2, arch_3>>);
  static_assert(!accepts_3<float32x2_t, float32x2_t, float32x4_t>);
  static_assert(!accepts_3<native::simd<float, 2, (arch_3 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<float, 2, arch_3>, native::simd<float, 4, arch_3>>);
  constexpr auto arch_4 = native::feature_closure(native::isa<native::arm>(native::arm_feature::complxnum));
  template<class... A> concept accepts_4 = requires(A... a) { native::fcadd<arch_4, 90>(a...); };
  static_assert(accepts_4<native::simd<float, 4, arch_4>, native::simd<float, 4, arch_4>>);
  static_assert(!accepts_4<native::simd<std::uint32_t, 4, arch_4>, native::simd<float, 4, arch_4>>);
  static_assert(std::same_as<decltype(native::fcadd<arch_4, 90>(std::declval<native::simd<float, 4, arch_4>>(), std::declval<native::simd<float, 4, arch_4>>())), native::simd<float, 4, arch_4>>);
  static_assert(!accepts_4<float32x4_t, float32x4_t>);
  static_assert(!accepts_4<native::simd<float, 4, (arch_4 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<float, 4, arch_4>>);
  constexpr auto arch_5 = native::feature_closure(native::isa<native::arm>(native::arm_feature::complxnum));
  template<class... A> concept accepts_5 = requires(A... a) { native::fcmla<arch_5, 90>(a...); };
  static_assert(accepts_5<native::simd<float, 4, arch_5>, native::simd<float, 4, arch_5>, native::simd<float, 4, arch_5>>);
  static_assert(!accepts_5<native::simd<std::uint32_t, 4, arch_5>, native::simd<float, 4, arch_5>, native::simd<float, 4, arch_5>>);
  static_assert(std::same_as<decltype(native::fcmla<arch_5, 90>(std::declval<native::simd<float, 4, arch_5>>(), std::declval<native::simd<float, 4, arch_5>>(), std::declval<native::simd<float, 4, arch_5>>())), native::simd<float, 4, arch_5>>);
  static_assert(!accepts_5<float32x4_t, float32x4_t, float32x4_t>);
  static_assert(!accepts_5<native::simd<float, 4, (arch_5 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<float, 4, arch_5>, native::simd<float, 4, arch_5>>);
  constexpr auto arch_6 = native::feature_closure(native::isa<native::arm>(native::arm_feature::complxnum));
  template<class... A> concept accepts_6 = requires(A... a) { native::fcmla_lane<arch_6, 90, 0>(a...); };
  static_assert(accepts_6<native::simd<float, 4, arch_6>, native::simd<float, 4, arch_6>, native::simd<float, 2, arch_6>>);
  static_assert(!accepts_6<native::simd<std::uint32_t, 4, arch_6>, native::simd<float, 4, arch_6>, native::simd<float, 2, arch_6>>);
  static_assert(std::same_as<decltype(native::fcmla_lane<arch_6, 90, 0>(std::declval<native::simd<float, 4, arch_6>>(), std::declval<native::simd<float, 4, arch_6>>(), std::declval<native::simd<float, 2, arch_6>>())), native::simd<float, 4, arch_6>>);
  static_assert(!accepts_6<float32x4_t, float32x4_t, float32x2_t>);
  static_assert(!accepts_6<native::simd<float, 4, (arch_6 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<float, 4, arch_6>, native::simd<float, 2, arch_6>>);
  constexpr auto arch_7 = native::feature_closure(native::isa<native::arm>(native::arm_feature::complxnum));
  template<class... A> concept accepts_7 = requires(A... a) { native::fcmla_lane<arch_7, 90, 0>(a...); };
  static_assert(accepts_7<native::simd<float, 4, arch_7>, native::simd<float, 4, arch_7>, native::simd<float, 4, arch_7>>);
  static_assert(!accepts_7<native::simd<std::uint32_t, 4, arch_7>, native::simd<float, 4, arch_7>, native::simd<float, 4, arch_7>>);
  static_assert(std::same_as<decltype(native::fcmla_lane<arch_7, 90, 0>(std::declval<native::simd<float, 4, arch_7>>(), std::declval<native::simd<float, 4, arch_7>>(), std::declval<native::simd<float, 4, arch_7>>())), native::simd<float, 4, arch_7>>);
  static_assert(!accepts_7<float32x4_t, float32x4_t, float32x4_t>);
  static_assert(!accepts_7<native::simd<float, 4, (arch_7 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<float, 4, arch_7>, native::simd<float, 4, arch_7>>);
  constexpr auto arch_8 = native::feature_closure(native::isa<native::arm>(native::arm_feature::complxnum));
  template<class... A> concept accepts_8 = requires(A... a) { native::fcadd<arch_8, 90>(a...); };
  static_assert(accepts_8<native::simd<double, 2, arch_8>, native::simd<double, 2, arch_8>>);
  static_assert(!accepts_8<native::simd<std::uint64_t, 2, arch_8>, native::simd<double, 2, arch_8>>);
  static_assert(std::same_as<decltype(native::fcadd<arch_8, 90>(std::declval<native::simd<double, 2, arch_8>>(), std::declval<native::simd<double, 2, arch_8>>())), native::simd<double, 2, arch_8>>);
  static_assert(!accepts_8<float64x2_t, float64x2_t>);
  static_assert(!accepts_8<native::simd<double, 2, (arch_8 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<double, 2, arch_8>>);
  constexpr auto arch_9 = native::feature_closure(native::isa<native::arm>(native::arm_feature::complxnum));
  template<class... A> concept accepts_9 = requires(A... a) { native::fcmla<arch_9, 90>(a...); };
  static_assert(accepts_9<native::simd<double, 2, arch_9>, native::simd<double, 2, arch_9>, native::simd<double, 2, arch_9>>);
  static_assert(!accepts_9<native::simd<std::uint64_t, 2, arch_9>, native::simd<double, 2, arch_9>, native::simd<double, 2, arch_9>>);
  static_assert(std::same_as<decltype(native::fcmla<arch_9, 90>(std::declval<native::simd<double, 2, arch_9>>(), std::declval<native::simd<double, 2, arch_9>>(), std::declval<native::simd<double, 2, arch_9>>())), native::simd<double, 2, arch_9>>);
  static_assert(!accepts_9<float64x2_t, float64x2_t, float64x2_t>);
  static_assert(!accepts_9<native::simd<double, 2, (arch_9 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<double, 2, arch_9>, native::simd<double, 2, arch_9>>);
  constexpr auto arch_10 = (native::feature_closure(native::isa<native::arm>(native::arm_feature::complxnum)) & native::neon_fp16);
  template<class... A> concept accepts_10 = requires(A... a) { native::fcadd<arch_10, 90>(a...); };
  static_assert(accepts_10<native::simd<native::fp16, 4, arch_10>, native::simd<native::fp16, 4, arch_10>>);
  static_assert(!accepts_10<native::simd<std::uint16_t, 4, arch_10>, native::simd<native::fp16, 4, arch_10>>);
  static_assert(std::same_as<decltype(native::fcadd<arch_10, 90>(std::declval<native::simd<native::fp16, 4, arch_10>>(), std::declval<native::simd<native::fp16, 4, arch_10>>())), native::simd<native::fp16, 4, arch_10>>);
  static_assert(!accepts_10<float16x4_t, float16x4_t>);
  static_assert(!accepts_10<native::simd<native::fp16, 4, (arch_10 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 4, arch_10>>);
  constexpr auto arch_11 = (native::feature_closure(native::isa<native::arm>(native::arm_feature::complxnum)) & native::neon_fp16);
  template<class... A> concept accepts_11 = requires(A... a) { native::fcmla<arch_11, 90>(a...); };
  static_assert(accepts_11<native::simd<native::fp16, 4, arch_11>, native::simd<native::fp16, 4, arch_11>, native::simd<native::fp16, 4, arch_11>>);
  static_assert(!accepts_11<native::simd<std::uint16_t, 4, arch_11>, native::simd<native::fp16, 4, arch_11>, native::simd<native::fp16, 4, arch_11>>);
  static_assert(std::same_as<decltype(native::fcmla<arch_11, 90>(std::declval<native::simd<native::fp16, 4, arch_11>>(), std::declval<native::simd<native::fp16, 4, arch_11>>(), std::declval<native::simd<native::fp16, 4, arch_11>>())), native::simd<native::fp16, 4, arch_11>>);
  static_assert(!accepts_11<float16x4_t, float16x4_t, float16x4_t>);
  static_assert(!accepts_11<native::simd<native::fp16, 4, (arch_11 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 4, arch_11>, native::simd<native::fp16, 4, arch_11>>);
  constexpr auto arch_12 = (native::feature_closure(native::isa<native::arm>(native::arm_feature::complxnum)) & native::neon_fp16);
  template<class... A> concept accepts_12 = requires(A... a) { native::fcmla_lane<arch_12, 90, 0>(a...); };
  static_assert(accepts_12<native::simd<native::fp16, 4, arch_12>, native::simd<native::fp16, 4, arch_12>, native::simd<native::fp16, 4, arch_12>>);
  static_assert(!accepts_12<native::simd<std::uint16_t, 4, arch_12>, native::simd<native::fp16, 4, arch_12>, native::simd<native::fp16, 4, arch_12>>);
  static_assert(std::same_as<decltype(native::fcmla_lane<arch_12, 90, 0>(std::declval<native::simd<native::fp16, 4, arch_12>>(), std::declval<native::simd<native::fp16, 4, arch_12>>(), std::declval<native::simd<native::fp16, 4, arch_12>>())), native::simd<native::fp16, 4, arch_12>>);
  static_assert(!accepts_12<float16x4_t, float16x4_t, float16x4_t>);
  static_assert(!accepts_12<native::simd<native::fp16, 4, (arch_12 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 4, arch_12>, native::simd<native::fp16, 4, arch_12>>);
  constexpr auto arch_13 = (native::feature_closure(native::isa<native::arm>(native::arm_feature::complxnum)) & native::neon_fp16);
  template<class... A> concept accepts_13 = requires(A... a) { native::fcmla_lane<arch_13, 90, 0>(a...); };
  static_assert(accepts_13<native::simd<native::fp16, 4, arch_13>, native::simd<native::fp16, 4, arch_13>, native::simd<native::fp16, 8, arch_13>>);
  static_assert(!accepts_13<native::simd<std::uint16_t, 4, arch_13>, native::simd<native::fp16, 4, arch_13>, native::simd<native::fp16, 8, arch_13>>);
  static_assert(std::same_as<decltype(native::fcmla_lane<arch_13, 90, 0>(std::declval<native::simd<native::fp16, 4, arch_13>>(), std::declval<native::simd<native::fp16, 4, arch_13>>(), std::declval<native::simd<native::fp16, 8, arch_13>>())), native::simd<native::fp16, 4, arch_13>>);
  static_assert(!accepts_13<float16x4_t, float16x4_t, float16x8_t>);
  static_assert(!accepts_13<native::simd<native::fp16, 4, (arch_13 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 4, arch_13>, native::simd<native::fp16, 8, arch_13>>);
  constexpr auto arch_14 = (native::feature_closure(native::isa<native::arm>(native::arm_feature::complxnum)) & native::neon_fp16);
  template<class... A> concept accepts_14 = requires(A... a) { native::fcadd<arch_14, 90>(a...); };
  static_assert(accepts_14<native::simd<native::fp16, 8, arch_14>, native::simd<native::fp16, 8, arch_14>>);
  static_assert(!accepts_14<native::simd<std::uint16_t, 8, arch_14>, native::simd<native::fp16, 8, arch_14>>);
  static_assert(std::same_as<decltype(native::fcadd<arch_14, 90>(std::declval<native::simd<native::fp16, 8, arch_14>>(), std::declval<native::simd<native::fp16, 8, arch_14>>())), native::simd<native::fp16, 8, arch_14>>);
  static_assert(!accepts_14<float16x8_t, float16x8_t>);
  static_assert(!accepts_14<native::simd<native::fp16, 8, (arch_14 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 8, arch_14>>);
  constexpr auto arch_15 = (native::feature_closure(native::isa<native::arm>(native::arm_feature::complxnum)) & native::neon_fp16);
  template<class... A> concept accepts_15 = requires(A... a) { native::fcmla<arch_15, 90>(a...); };
  static_assert(accepts_15<native::simd<native::fp16, 8, arch_15>, native::simd<native::fp16, 8, arch_15>, native::simd<native::fp16, 8, arch_15>>);
  static_assert(!accepts_15<native::simd<std::uint16_t, 8, arch_15>, native::simd<native::fp16, 8, arch_15>, native::simd<native::fp16, 8, arch_15>>);
  static_assert(std::same_as<decltype(native::fcmla<arch_15, 90>(std::declval<native::simd<native::fp16, 8, arch_15>>(), std::declval<native::simd<native::fp16, 8, arch_15>>(), std::declval<native::simd<native::fp16, 8, arch_15>>())), native::simd<native::fp16, 8, arch_15>>);
  static_assert(!accepts_15<float16x8_t, float16x8_t, float16x8_t>);
  static_assert(!accepts_15<native::simd<native::fp16, 8, (arch_15 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 8, arch_15>, native::simd<native::fp16, 8, arch_15>>);
  constexpr auto arch_16 = (native::feature_closure(native::isa<native::arm>(native::arm_feature::complxnum)) & native::neon_fp16);
  template<class... A> concept accepts_16 = requires(A... a) { native::fcmla_lane<arch_16, 90, 0>(a...); };
  static_assert(accepts_16<native::simd<native::fp16, 8, arch_16>, native::simd<native::fp16, 8, arch_16>, native::simd<native::fp16, 4, arch_16>>);
  static_assert(!accepts_16<native::simd<std::uint16_t, 8, arch_16>, native::simd<native::fp16, 8, arch_16>, native::simd<native::fp16, 4, arch_16>>);
  static_assert(std::same_as<decltype(native::fcmla_lane<arch_16, 90, 0>(std::declval<native::simd<native::fp16, 8, arch_16>>(), std::declval<native::simd<native::fp16, 8, arch_16>>(), std::declval<native::simd<native::fp16, 4, arch_16>>())), native::simd<native::fp16, 8, arch_16>>);
  static_assert(!accepts_16<float16x8_t, float16x8_t, float16x4_t>);
  static_assert(!accepts_16<native::simd<native::fp16, 8, (arch_16 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 8, arch_16>, native::simd<native::fp16, 4, arch_16>>);
  constexpr auto arch_17 = (native::feature_closure(native::isa<native::arm>(native::arm_feature::complxnum)) & native::neon_fp16);
  template<class... A> concept accepts_17 = requires(A... a) { native::fcmla_lane<arch_17, 90, 0>(a...); };
  static_assert(accepts_17<native::simd<native::fp16, 8, arch_17>, native::simd<native::fp16, 8, arch_17>, native::simd<native::fp16, 8, arch_17>>);
  static_assert(!accepts_17<native::simd<std::uint16_t, 8, arch_17>, native::simd<native::fp16, 8, arch_17>, native::simd<native::fp16, 8, arch_17>>);
  static_assert(std::same_as<decltype(native::fcmla_lane<arch_17, 90, 0>(std::declval<native::simd<native::fp16, 8, arch_17>>(), std::declval<native::simd<native::fp16, 8, arch_17>>(), std::declval<native::simd<native::fp16, 8, arch_17>>())), native::simd<native::fp16, 8, arch_17>>);
  static_assert(!accepts_17<float16x8_t, float16x8_t, float16x8_t>);
  static_assert(!accepts_17<native::simd<native::fp16, 8, (arch_17 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 8, arch_17>, native::simd<native::fp16, 8, arch_17>>);
}
namespace fcma_fixture {
  constexpr native::isa<native::arm> arch{native::arm_feature::complxnum};
  constexpr native::isa<native::arm> half_arch = [] {
    auto a = arch; a.set(native::arm_feature::neon_fp16, true); return a;
  }();
  template<native::isa<native::arm> A, unsigned R, class V> concept add_available = requires(V v) {
    { fcma_api::fcadd<A, R>(v, v) } noexcept -> std::same_as<V>;
  };
  template<native::isa<native::arm> A, unsigned R, class V> concept mla_available = requires(V v) {
    { fcma_api::fcmla<A, R>(v, v, v) } noexcept -> std::same_as<V>;
  };
  template<native::isa<native::arm> A, unsigned R, unsigned L, class V, class B>
  concept lane_available = requires(V v, B b) { fcma_api::fcmla_lane<A, R, L>(v, v, b); };
  // Weak feature tags participate only through consteval overloads; runtime inputs are rejected.
  static_assert(add_available<arch, 90, float32x2_t>);
  static_assert(add_available<arch, 270, float32x2_t>);
  static_assert(mla_available<arch, 0, float32x2_t>);
  static_assert(mla_available<arch, 90, float32x2_t>);
  static_assert(mla_available<arch, 180, float32x2_t>);
  static_assert(mla_available<arch, 270, float32x2_t>);
  static_assert(!add_available<arch, 0, float32x2_t>);
  static_assert(!add_available<arch, 180, float32x2_t>);
  static_assert(!add_available<arch, 360, float32x2_t>);
  static_assert(!add_available<arch, 1, float32x2_t>);
  static_assert(!mla_available<arch, 360, float32x2_t>);
  static_assert(!add_available<native::scalar, 90, float32x2_t> && !mla_available<native::scalar, 0, float32x2_t>);
  static_assert(add_available<native::neon, 90, float32x2_t> && mla_available<native::neon, 0, float32x2_t>);
  static_assert(add_available<native::neon_fp16, 90, float32x2_t> && mla_available<native::neon_fp16, 0, float32x2_t>);
  static_assert(add_available<native::isa<native::arm>{native::arm_feature::fp16fml}, 90, float32x2_t> && mla_available<native::isa<native::arm>{native::arm_feature::fp16fml}, 0, float32x2_t>);
  static_assert(lane_available<arch, 270, 0, float32x2_t, float32x2_t>);
  static_assert(lane_available<arch, 270, 0, float32x2_t, float32x2_t>);
  static_assert(!lane_available<arch, 0, 1, float32x2_t, float32x2_t>);
  static_assert(lane_available<arch, 270, 0, float32x2_t, float32x4_t>);
  static_assert(lane_available<arch, 270, 1, float32x2_t, float32x4_t>);
  static_assert(!lane_available<arch, 0, 2, float32x2_t, float32x4_t>);
  static_assert(add_available<arch, 90, float32x4_t>);
  static_assert(add_available<arch, 270, float32x4_t>);
  static_assert(mla_available<arch, 0, float32x4_t>);
  static_assert(mla_available<arch, 90, float32x4_t>);
  static_assert(mla_available<arch, 180, float32x4_t>);
  static_assert(mla_available<arch, 270, float32x4_t>);
  static_assert(!add_available<arch, 0, float32x4_t>);
  static_assert(!add_available<arch, 180, float32x4_t>);
  static_assert(!add_available<arch, 360, float32x4_t>);
  static_assert(!add_available<arch, 1, float32x4_t>);
  static_assert(!mla_available<arch, 360, float32x4_t>);
  static_assert(!add_available<native::scalar, 90, float32x4_t> && !mla_available<native::scalar, 0, float32x4_t>);
  static_assert(add_available<native::neon, 90, float32x4_t> && mla_available<native::neon, 0, float32x4_t>);
  static_assert(add_available<native::neon_fp16, 90, float32x4_t> && mla_available<native::neon_fp16, 0, float32x4_t>);
  static_assert(add_available<native::isa<native::arm>{native::arm_feature::fp16fml}, 90, float32x4_t> && mla_available<native::isa<native::arm>{native::arm_feature::fp16fml}, 0, float32x4_t>);
  static_assert(lane_available<arch, 270, 0, float32x4_t, float32x2_t>);
  static_assert(lane_available<arch, 270, 0, float32x4_t, float32x2_t>);
  static_assert(!lane_available<arch, 0, 1, float32x4_t, float32x2_t>);
  static_assert(lane_available<arch, 270, 0, float32x4_t, float32x4_t>);
  static_assert(lane_available<arch, 270, 1, float32x4_t, float32x4_t>);
  static_assert(!lane_available<arch, 0, 2, float32x4_t, float32x4_t>);
  static_assert(add_available<arch, 90, float64x2_t>);
  static_assert(add_available<arch, 270, float64x2_t>);
  static_assert(mla_available<arch, 0, float64x2_t>);
  static_assert(mla_available<arch, 90, float64x2_t>);
  static_assert(mla_available<arch, 180, float64x2_t>);
  static_assert(mla_available<arch, 270, float64x2_t>);
  static_assert(!add_available<arch, 0, float64x2_t>);
  static_assert(!add_available<arch, 180, float64x2_t>);
  static_assert(!add_available<arch, 360, float64x2_t>);
  static_assert(!add_available<arch, 1, float64x2_t>);
  static_assert(!mla_available<arch, 360, float64x2_t>);
  static_assert(!add_available<native::scalar, 90, float64x2_t> && !mla_available<native::scalar, 0, float64x2_t>);
  static_assert(add_available<native::neon, 90, float64x2_t> && mla_available<native::neon, 0, float64x2_t>);
  static_assert(add_available<native::neon_fp16, 90, float64x2_t> && mla_available<native::neon_fp16, 0, float64x2_t>);
  static_assert(add_available<native::isa<native::arm>{native::arm_feature::fp16fml}, 90, float64x2_t> && mla_available<native::isa<native::arm>{native::arm_feature::fp16fml}, 0, float64x2_t>);
  static_assert(add_available<half_arch, 90, float16x4_t>);
  static_assert(add_available<half_arch, 270, float16x4_t>);
  static_assert(mla_available<half_arch, 0, float16x4_t>);
  static_assert(mla_available<half_arch, 90, float16x4_t>);
  static_assert(mla_available<half_arch, 180, float16x4_t>);
  static_assert(mla_available<half_arch, 270, float16x4_t>);
  static_assert(!add_available<half_arch, 0, float16x4_t>);
  static_assert(!add_available<half_arch, 180, float16x4_t>);
  static_assert(!add_available<half_arch, 360, float16x4_t>);
  static_assert(!add_available<half_arch, 1, float16x4_t>);
  static_assert(!mla_available<half_arch, 360, float16x4_t>);
  static_assert(!add_available<native::scalar, 90, float16x4_t> && !mla_available<native::scalar, 0, float16x4_t>);
  static_assert(add_available<native::neon, 90, float16x4_t> && mla_available<native::neon, 0, float16x4_t>);
  static_assert(add_available<native::neon_fp16, 90, float16x4_t> && mla_available<native::neon_fp16, 0, float16x4_t>);
  static_assert(add_available<native::isa<native::arm>{native::arm_feature::fp16fml}, 90, float16x4_t> && mla_available<native::isa<native::arm>{native::arm_feature::fp16fml}, 0, float16x4_t>);
  static_assert(add_available<arch, 90, float16x4_t> && mla_available<arch, 0, float16x4_t>);
  static_assert(lane_available<half_arch, 270, 0, float16x4_t, float16x4_t>);
  static_assert(lane_available<half_arch, 270, 1, float16x4_t, float16x4_t>);
  static_assert(!lane_available<half_arch, 0, 2, float16x4_t, float16x4_t>);
  static_assert(lane_available<half_arch, 270, 0, float16x4_t, float16x8_t>);
  static_assert(lane_available<half_arch, 270, 3, float16x4_t, float16x8_t>);
  static_assert(!lane_available<half_arch, 0, 4, float16x4_t, float16x8_t>);
  static_assert(add_available<half_arch, 90, float16x8_t>);
  static_assert(add_available<half_arch, 270, float16x8_t>);
  static_assert(mla_available<half_arch, 0, float16x8_t>);
  static_assert(mla_available<half_arch, 90, float16x8_t>);
  static_assert(mla_available<half_arch, 180, float16x8_t>);
  static_assert(mla_available<half_arch, 270, float16x8_t>);
  static_assert(!add_available<half_arch, 0, float16x8_t>);
  static_assert(!add_available<half_arch, 180, float16x8_t>);
  static_assert(!add_available<half_arch, 360, float16x8_t>);
  static_assert(!add_available<half_arch, 1, float16x8_t>);
  static_assert(!mla_available<half_arch, 360, float16x8_t>);
  static_assert(!add_available<native::scalar, 90, float16x8_t> && !mla_available<native::scalar, 0, float16x8_t>);
  static_assert(add_available<native::neon, 90, float16x8_t> && mla_available<native::neon, 0, float16x8_t>);
  static_assert(add_available<native::neon_fp16, 90, float16x8_t> && mla_available<native::neon_fp16, 0, float16x8_t>);
  static_assert(add_available<native::isa<native::arm>{native::arm_feature::fp16fml}, 90, float16x8_t> && mla_available<native::isa<native::arm>{native::arm_feature::fp16fml}, 0, float16x8_t>);
  static_assert(add_available<arch, 90, float16x8_t> && mla_available<arch, 0, float16x8_t>);
  static_assert(lane_available<half_arch, 270, 0, float16x8_t, float16x4_t>);
  static_assert(lane_available<half_arch, 270, 1, float16x8_t, float16x4_t>);
  static_assert(!lane_available<half_arch, 0, 2, float16x8_t, float16x4_t>);
  static_assert(lane_available<half_arch, 270, 0, float16x8_t, float16x8_t>);
  static_assert(lane_available<half_arch, 270, 3, float16x8_t, float16x8_t>);
  static_assert(!lane_available<half_arch, 0, 4, float16x8_t, float16x8_t>);

  static_assert(!lane_available<native::scalar, 0, 0, float32x2_t, float32x2_t>);
  static_assert(lane_available<native::neon, 0, 0, float32x2_t, float32x2_t>);
  static_assert(lane_available<native::neon_fp16, 0, 0, float32x2_t, float32x2_t>);
  static_assert(lane_available<native::isa<native::arm>{native::arm_feature::fp16fml}, 0, 0, float32x2_t, float32x2_t>);
  static_assert(!lane_available<arch, 45, 0, float32x2_t, float32x2_t>);
  static_assert(!lane_available<native::scalar, 0, 0, float32x2_t, float32x4_t>);
  static_assert(lane_available<native::neon, 0, 0, float32x2_t, float32x4_t>);
  static_assert(lane_available<native::neon_fp16, 0, 0, float32x2_t, float32x4_t>);
  static_assert(lane_available<native::isa<native::arm>{native::arm_feature::fp16fml}, 0, 0, float32x2_t, float32x4_t>);
  static_assert(!lane_available<arch, 45, 0, float32x2_t, float32x4_t>);
  static_assert(!lane_available<native::scalar, 0, 0, float32x4_t, float32x2_t>);
  static_assert(lane_available<native::neon, 0, 0, float32x4_t, float32x2_t>);
  static_assert(lane_available<native::neon_fp16, 0, 0, float32x4_t, float32x2_t>);
  static_assert(lane_available<native::isa<native::arm>{native::arm_feature::fp16fml}, 0, 0, float32x4_t, float32x2_t>);
  static_assert(!lane_available<arch, 45, 0, float32x4_t, float32x2_t>);
  static_assert(!lane_available<native::scalar, 0, 0, float32x4_t, float32x4_t>);
  static_assert(lane_available<native::neon, 0, 0, float32x4_t, float32x4_t>);
  static_assert(lane_available<native::neon_fp16, 0, 0, float32x4_t, float32x4_t>);
  static_assert(lane_available<native::isa<native::arm>{native::arm_feature::fp16fml}, 0, 0, float32x4_t, float32x4_t>);
  static_assert(!lane_available<arch, 45, 0, float32x4_t, float32x4_t>);
  static_assert(!lane_available<native::scalar, 0, 0, float16x4_t, float16x4_t>);
  static_assert(lane_available<native::neon, 0, 0, float16x4_t, float16x4_t>);
  static_assert(lane_available<native::neon_fp16, 0, 0, float16x4_t, float16x4_t>);
  static_assert(lane_available<native::isa<native::arm>{native::arm_feature::fp16fml}, 0, 0, float16x4_t, float16x4_t>);
  static_assert(lane_available<arch, 0, 0, float16x4_t, float16x4_t>);
  static_assert(!lane_available<half_arch, 45, 0, float16x4_t, float16x4_t>);
  static_assert(!lane_available<native::scalar, 0, 0, float16x4_t, float16x8_t>);
  static_assert(lane_available<native::neon, 0, 0, float16x4_t, float16x8_t>);
  static_assert(lane_available<native::neon_fp16, 0, 0, float16x4_t, float16x8_t>);
  static_assert(lane_available<native::isa<native::arm>{native::arm_feature::fp16fml}, 0, 0, float16x4_t, float16x8_t>);
  static_assert(lane_available<arch, 0, 0, float16x4_t, float16x8_t>);
  static_assert(!lane_available<half_arch, 45, 0, float16x4_t, float16x8_t>);
  static_assert(!lane_available<native::scalar, 0, 0, float16x8_t, float16x4_t>);
  static_assert(lane_available<native::neon, 0, 0, float16x8_t, float16x4_t>);
  static_assert(lane_available<native::neon_fp16, 0, 0, float16x8_t, float16x4_t>);
  static_assert(lane_available<native::isa<native::arm>{native::arm_feature::fp16fml}, 0, 0, float16x8_t, float16x4_t>);
  static_assert(lane_available<arch, 0, 0, float16x8_t, float16x4_t>);
  static_assert(!lane_available<half_arch, 45, 0, float16x8_t, float16x4_t>);
  static_assert(!lane_available<native::scalar, 0, 0, float16x8_t, float16x8_t>);
  static_assert(lane_available<native::neon, 0, 0, float16x8_t, float16x8_t>);
  static_assert(lane_available<native::neon_fp16, 0, 0, float16x8_t, float16x8_t>);
  static_assert(lane_available<native::isa<native::arm>{native::arm_feature::fp16fml}, 0, 0, float16x8_t, float16x8_t>);
  static_assert(lane_available<arch, 0, 0, float16x8_t, float16x8_t>);
  static_assert(!lane_available<half_arch, 45, 0, float16x8_t, float16x8_t>);

  static_assert(!lane_available<half_arch, 0, 1, float32x2_t, float32x2_t>);
  static_assert(!lane_available<half_arch, 0, 2, float32x2_t, float32x4_t>);
  static_assert(!lane_available<half_arch, 0, 1, float32x4_t, float32x2_t>);
  static_assert(!lane_available<half_arch, 0, 2, float32x4_t, float32x4_t>);

  consteval bool admission_contract() {
    native::arm_capabilities cpu{};
    cpu.present.set(native::arm_feature::neon, true);
    cpu.observed.set(native::arm_feature::neon, true);
    cpu.present.set(native::arm_feature::complxnum, true);
    cpu.observed.set(native::arm_feature::complxnum, true);
    if(!native::classify_isa(cpu, arch).admitted()) return false;
    if(native::classify_isa(cpu, half_arch).admitted()) return false;
    cpu.present.set(native::arm_feature::neon_fp16, true);
    cpu.observed.set(native::arm_feature::neon_fp16, true);
    if(!native::classify_isa(cpu, half_arch).admitted()) return false;
    for(auto feature : {native::arm_feature::neon, native::arm_feature::complxnum}) {
      auto missing = cpu;
      missing.present.set(feature, false);
      if(native::classify_isa(missing, arch).admitted()) return false;
      missing = cpu;
      missing.observed.set(feature, false);
      if(native::classify_isa(missing, arch).admitted()) return false;
    }
    return true;
  }
  static_assert(admission_contract());

  inline std::uint64_t fpcr() {
    std::uint64_t value; asm volatile("mrs %0, fpcr" : "=r"(value)); return value;
  }
  template<class V> bool equal(V a, V b) {
    for(unsigned i = 0; i != sizeof(V) / sizeof(a[0]); ++i) {
      // The fixture's half bank is exactly representable. For all formats,
      // compare zero signs and accept NaNs without asserting payload selection.
      if(a[i] != b[i] && !(std::isnan(double(a[i])) && std::isnan(double(b[i])))) return false;
      if(a[i] == 0 && std::signbit(double(a[i])) != std::signbit(double(b[i]))) return false;
    }
    return true;
  }
  template<unsigned R, class V> V reference(V acc, V a, V b) {
    V out{};
    for(unsigned i = 0; i != sizeof(V) / sizeof(a[0]); i += 2) {
      // FCMLA selects a.real for 0/180 and a.imag for 90/270; each
      // destination component has exactly one fused product and one rounding.
      auto x = a[i + ((R == 90 || R == 270) ? 1 : 0)];
      auto br = b[i + ((R == 90 || R == 270) ? 1 : 0)];
      auto bi = b[i + ((R == 90 || R == 270) ? 0 : 1)];
      if constexpr(R == 90 || R == 180) br = -br;
      if constexpr(R == 180 || R == 270) bi = -bi;
      if constexpr(sizeof(a[0]) == 2) {
        // Binary64 is exact for this bounded dyadic half test bank. One final
        // conversion supplies the half rounding, independently of FCMLA.
        out[i] = double(x) * double(br) + double(acc[i]);
        out[i+1] = double(x) * double(bi) + double(acc[i+1]);
      } else {
        out[i] = std::fma(x, br, acc[i]);
        out[i+1] = std::fma(x, bi, acc[i+1]);
      }
    }
    return out;
  }
  template<class V>
  __attribute__((target("complxnum,fullfp16"), noinline))
  bool check(V acc, V a, V b) {
    constexpr auto required = sizeof(a[0]) == 2 ? half_arch : arch;
    auto r90 = fcma_api::fcadd<required, 90>(a, b);
    auto r270 = fcma_api::fcadd<required, 270>(a, b);
    V ref90{}, ref270{};
    for(unsigned i = 0; i != sizeof(V) / sizeof(a[0]); i += 2) {
      ref90[i] = a[i] - b[i+1]; ref90[i+1] = a[i+1] + b[i];
      ref270[i] = a[i] + b[i+1]; ref270[i+1] = a[i+1] - b[i];
    }
    return equal(r90, ref90) && equal(r270, ref270) &&
      equal(fcma_api::fcmla<required, 0>(acc, a, b), reference<0>(acc, a, b)) &&
      equal(fcma_api::fcmla<required, 90>(acc, a, b), reference<90>(acc, a, b)) &&
      equal(fcma_api::fcmla<required, 180>(acc, a, b), reference<180>(acc, a, b)) &&
      equal(fcma_api::fcmla<required, 270>(acc, a, b), reference<270>(acc, a, b));
  }
  template<unsigned Lane, class V, class B>
  __attribute__((target("complxnum,fullfp16"), noinline))
  bool check_lane(V acc, V a, B b) {
    constexpr auto required = sizeof(a[0]) == 2 ? half_arch : arch;
    V pair{};
    for(unsigned i = 0; i != sizeof(V) / sizeof(a[0]); i += 2) {
      pair[i] = b[2*Lane]; pair[i+1] = b[2*Lane+1];
    }
    return equal(fcma_api::fcmla_lane<required, 0, Lane>(acc, a, b), reference<0>(acc, a, pair)) &&
      equal(fcma_api::fcmla_lane<required, 90, Lane>(acc, a, b), reference<90>(acc, a, pair)) &&
      equal(fcma_api::fcmla_lane<required, 180, Lane>(acc, a, b), reference<180>(acc, a, pair)) &&
      equal(fcma_api::fcmla_lane<required, 270, Lane>(acc, a, b), reference<270>(acc, a, pair));
  }
  __attribute__((target("complxnum,fullfp16"), noinline))
  bool exercise(unsigned seed) {
    auto control = fpcr();
    for(unsigned k = 0; k != 128; ++k) {
      float32x4_t a{}, b{}, acc{}; float16x8_t ha{}, hb{}, hc{}; float64x2_t da{}, db{}, dc{};
      for(unsigned i = 0; i != 8; ++i) {
        seed = seed * 1664525u + 1013904223u;
        ha[i] = (int(seed % 4096u) - 2048) * 0.125;
        hb[i] = (int((seed >> 12) % 256u) - 128) * 0.0625;
        hc[i] = (int((seed >> 20) % 64u) - 32) * 0.25;
        if(i < 4) {
          a[i] = std::bit_cast<float>((seed & 0x807fffffu) | 0x3f000000u);
          b[i] = std::bit_cast<float>(((seed >> 1) & 0x807fffffu) | 0x40000000u);
          acc[i] = std::bit_cast<float>(((seed >> 2) & 0x807fffffu) | 0x3f800000u);
        }
        if(i < 2) {
          da[i] = double(a[i]); db[i] = double(b[i]); dc[i] = double(acc[i]) + 0x1p-40;
        }
      }
      auto a2 = vget_low_f32(a), b2 = vget_low_f32(b), c2 = vget_low_f32(acc);
      auto ha4 = vget_low_f16(ha), hb4 = vget_low_f16(hb), hc4 = vget_low_f16(hc);
      if(!check(acc, a, b) || !check(c2, a2, b2) || !check(dc, da, db) ||
         !check(hc, ha, hb) || !check(hc4, ha4, hb4)) return false;
      if(!check_lane<0>(c2, a2, b2)) return false;
      if(!check_lane<0>(c2, a2, b)) return false;
      if(!check_lane<1>(c2, a2, b)) return false;
      if(!check_lane<0>(acc, a, b2)) return false;
      if(!check_lane<0>(acc, a, b)) return false;
      if(!check_lane<1>(acc, a, b)) return false;
      if(!check_lane<0>(hc4, ha4, hb4)) return false;
      if(!check_lane<1>(hc4, ha4, hb4)) return false;
      if(!check_lane<0>(hc4, ha4, hb)) return false;
      if(!check_lane<1>(hc4, ha4, hb)) return false;
      if(!check_lane<2>(hc4, ha4, hb)) return false;
      if(!check_lane<3>(hc4, ha4, hb)) return false;
      if(!check_lane<0>(hc, ha, hb4)) return false;
      if(!check_lane<1>(hc, ha, hb4)) return false;
      if(!check_lane<0>(hc, ha, hb)) return false;
      if(!check_lane<1>(hc, ha, hb)) return false;
      if(!check_lane<2>(hc, ha, hb)) return false;
      if(!check_lane<3>(hc, ha, hb)) return false;
    }
    float32x4_t a{0, -0.0f, 1, -1}, b{1, 1, 2, -2}, acc{0, -0.0f, 0, 0};
    if(!check(acc, a, b)) return false;
    b[0] = std::numeric_limits<float>::infinity();
    std::feclearexcept(FE_ALL_EXCEPT);
    volatile auto invalid = fcma_api::fcmla<arch, 0>(acc, a, b);
    (void)invalid;
    if(!(std::fetestexcept(FE_INVALID) & FE_INVALID)) return false;
    std::feclearexcept(FE_ALL_EXCEPT);
    (void)fcma_api::fcmla<arch, 0>(acc, a, b);
    if(!(std::fetestexcept(FE_INVALID) & FE_INVALID)) return false;
    if(std::feraiseexcept(FE_DIVBYZERO) != 0) return false;
    b[0] = 1;
    (void)fcma_api::fcmla<arch, 0>(acc, a, b);
    if(!(std::fetestexcept(FE_DIVBYZERO) & FE_DIVBYZERO)) return false;
    return fpcr() == control;
  }
  inline int run(unsigned seed) {
    auto admission = native::classify_isa(native::observe_arm_capabilities(), half_arch);
    if(!admission.admitted()) {
      std::printf("Skipped: %s\n", admission.reason());
      return 77;
    }
    std::fenv_t saved;
    if(std::fegetenv(&saved) != 0) return 1;
    bool ok = true;
    for(int mode : {FE_TONEAREST, FE_DOWNWARD, FE_UPWARD, FE_TOWARDZERO}) {
      if(std::fesetround(mode) != 0 || !exercise(seed)) { ok = false; break; }
    }
    if(std::fesetenv(&saved) != 0) ok = false;
    if(!ok) std::puts("FCMA operation graph or FP environment check failed");
    return ok ? 0 : 1;
  }
}
