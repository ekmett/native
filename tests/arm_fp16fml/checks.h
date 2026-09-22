// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "simd_adapter.h"
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <concepts>
#include <utility>
namespace fp16fml_simd_contract {
  constexpr auto arch_0 = native::feature_closure(native::isa<native::arm>(native::arm_feature::fp16fml));
  template<class... A> concept accepts_0 = requires(A... a) { native::fmlal<arch_0>(a...); };
  static_assert(accepts_0<native::simd<float, 2, arch_0>, native::simd<native::fp16, 4, arch_0>, native::simd<native::fp16, 4, arch_0>>);
  static_assert(!accepts_0<native::simd<std::uint32_t, 2, arch_0>, native::simd<native::fp16, 4, arch_0>, native::simd<native::fp16, 4, arch_0>>);
  static_assert(std::same_as<decltype(native::fmlal<arch_0>(std::declval<native::simd<float, 2, arch_0>>(), std::declval<native::simd<native::fp16, 4, arch_0>>(), std::declval<native::simd<native::fp16, 4, arch_0>>())), native::simd<float, 2, arch_0>>);
  static_assert(!accepts_0<float32x2_t, float16x4_t, float16x4_t>);
  static_assert(!accepts_0<native::simd<float, 2, (arch_0 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 4, arch_0>, native::simd<native::fp16, 4, arch_0>>);
  constexpr auto arch_1 = native::feature_closure(native::isa<native::arm>(native::arm_feature::fp16fml));
  template<class... A> concept accepts_1 = requires(A... a) { native::fmlal_lane<arch_1, 0>(a...); };
  static_assert(accepts_1<native::simd<float, 2, arch_1>, native::simd<native::fp16, 4, arch_1>, native::simd<native::fp16, 4, arch_1>>);
  static_assert(!accepts_1<native::simd<std::uint32_t, 2, arch_1>, native::simd<native::fp16, 4, arch_1>, native::simd<native::fp16, 4, arch_1>>);
  static_assert(std::same_as<decltype(native::fmlal_lane<arch_1, 0>(std::declval<native::simd<float, 2, arch_1>>(), std::declval<native::simd<native::fp16, 4, arch_1>>(), std::declval<native::simd<native::fp16, 4, arch_1>>())), native::simd<float, 2, arch_1>>);
  static_assert(!accepts_1<float32x2_t, float16x4_t, float16x4_t>);
  static_assert(!accepts_1<native::simd<float, 2, (arch_1 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 4, arch_1>, native::simd<native::fp16, 4, arch_1>>);
  constexpr auto arch_2 = native::feature_closure(native::isa<native::arm>(native::arm_feature::fp16fml));
  template<class... A> concept accepts_2 = requires(A... a) { native::fmlal_lane<arch_2, 0>(a...); };
  static_assert(accepts_2<native::simd<float, 2, arch_2>, native::simd<native::fp16, 4, arch_2>, native::simd<native::fp16, 8, arch_2>>);
  static_assert(!accepts_2<native::simd<std::uint32_t, 2, arch_2>, native::simd<native::fp16, 4, arch_2>, native::simd<native::fp16, 8, arch_2>>);
  static_assert(std::same_as<decltype(native::fmlal_lane<arch_2, 0>(std::declval<native::simd<float, 2, arch_2>>(), std::declval<native::simd<native::fp16, 4, arch_2>>(), std::declval<native::simd<native::fp16, 8, arch_2>>())), native::simd<float, 2, arch_2>>);
  static_assert(!accepts_2<float32x2_t, float16x4_t, float16x8_t>);
  static_assert(!accepts_2<native::simd<float, 2, (arch_2 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 4, arch_2>, native::simd<native::fp16, 8, arch_2>>);
  constexpr auto arch_3 = native::feature_closure(native::isa<native::arm>(native::arm_feature::fp16fml));
  template<class... A> concept accepts_3 = requires(A... a) { native::fmlal<arch_3>(a...); };
  static_assert(accepts_3<native::simd<float, 4, arch_3>, native::simd<native::fp16, 8, arch_3>, native::simd<native::fp16, 8, arch_3>>);
  static_assert(!accepts_3<native::simd<std::uint32_t, 4, arch_3>, native::simd<native::fp16, 8, arch_3>, native::simd<native::fp16, 8, arch_3>>);
  static_assert(std::same_as<decltype(native::fmlal<arch_3>(std::declval<native::simd<float, 4, arch_3>>(), std::declval<native::simd<native::fp16, 8, arch_3>>(), std::declval<native::simd<native::fp16, 8, arch_3>>())), native::simd<float, 4, arch_3>>);
  static_assert(!accepts_3<float32x4_t, float16x8_t, float16x8_t>);
  static_assert(!accepts_3<native::simd<float, 4, (arch_3 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 8, arch_3>, native::simd<native::fp16, 8, arch_3>>);
  constexpr auto arch_4 = native::feature_closure(native::isa<native::arm>(native::arm_feature::fp16fml));
  template<class... A> concept accepts_4 = requires(A... a) { native::fmlal_lane<arch_4, 0>(a...); };
  static_assert(accepts_4<native::simd<float, 4, arch_4>, native::simd<native::fp16, 8, arch_4>, native::simd<native::fp16, 4, arch_4>>);
  static_assert(!accepts_4<native::simd<std::uint32_t, 4, arch_4>, native::simd<native::fp16, 8, arch_4>, native::simd<native::fp16, 4, arch_4>>);
  static_assert(std::same_as<decltype(native::fmlal_lane<arch_4, 0>(std::declval<native::simd<float, 4, arch_4>>(), std::declval<native::simd<native::fp16, 8, arch_4>>(), std::declval<native::simd<native::fp16, 4, arch_4>>())), native::simd<float, 4, arch_4>>);
  static_assert(!accepts_4<float32x4_t, float16x8_t, float16x4_t>);
  static_assert(!accepts_4<native::simd<float, 4, (arch_4 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 8, arch_4>, native::simd<native::fp16, 4, arch_4>>);
  constexpr auto arch_5 = native::feature_closure(native::isa<native::arm>(native::arm_feature::fp16fml));
  template<class... A> concept accepts_5 = requires(A... a) { native::fmlal_lane<arch_5, 0>(a...); };
  static_assert(accepts_5<native::simd<float, 4, arch_5>, native::simd<native::fp16, 8, arch_5>, native::simd<native::fp16, 8, arch_5>>);
  static_assert(!accepts_5<native::simd<std::uint32_t, 4, arch_5>, native::simd<native::fp16, 8, arch_5>, native::simd<native::fp16, 8, arch_5>>);
  static_assert(std::same_as<decltype(native::fmlal_lane<arch_5, 0>(std::declval<native::simd<float, 4, arch_5>>(), std::declval<native::simd<native::fp16, 8, arch_5>>(), std::declval<native::simd<native::fp16, 8, arch_5>>())), native::simd<float, 4, arch_5>>);
  static_assert(!accepts_5<float32x4_t, float16x8_t, float16x8_t>);
  static_assert(!accepts_5<native::simd<float, 4, (arch_5 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 8, arch_5>, native::simd<native::fp16, 8, arch_5>>);
  constexpr auto arch_6 = native::feature_closure(native::isa<native::arm>(native::arm_feature::fp16fml));
  template<class... A> concept accepts_6 = requires(A... a) { native::fmlal2<arch_6>(a...); };
  static_assert(accepts_6<native::simd<float, 2, arch_6>, native::simd<native::fp16, 4, arch_6>, native::simd<native::fp16, 4, arch_6>>);
  static_assert(!accepts_6<native::simd<std::uint32_t, 2, arch_6>, native::simd<native::fp16, 4, arch_6>, native::simd<native::fp16, 4, arch_6>>);
  static_assert(std::same_as<decltype(native::fmlal2<arch_6>(std::declval<native::simd<float, 2, arch_6>>(), std::declval<native::simd<native::fp16, 4, arch_6>>(), std::declval<native::simd<native::fp16, 4, arch_6>>())), native::simd<float, 2, arch_6>>);
  static_assert(!accepts_6<float32x2_t, float16x4_t, float16x4_t>);
  static_assert(!accepts_6<native::simd<float, 2, (arch_6 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 4, arch_6>, native::simd<native::fp16, 4, arch_6>>);
  constexpr auto arch_7 = native::feature_closure(native::isa<native::arm>(native::arm_feature::fp16fml));
  template<class... A> concept accepts_7 = requires(A... a) { native::fmlal2_lane<arch_7, 0>(a...); };
  static_assert(accepts_7<native::simd<float, 2, arch_7>, native::simd<native::fp16, 4, arch_7>, native::simd<native::fp16, 4, arch_7>>);
  static_assert(!accepts_7<native::simd<std::uint32_t, 2, arch_7>, native::simd<native::fp16, 4, arch_7>, native::simd<native::fp16, 4, arch_7>>);
  static_assert(std::same_as<decltype(native::fmlal2_lane<arch_7, 0>(std::declval<native::simd<float, 2, arch_7>>(), std::declval<native::simd<native::fp16, 4, arch_7>>(), std::declval<native::simd<native::fp16, 4, arch_7>>())), native::simd<float, 2, arch_7>>);
  static_assert(!accepts_7<float32x2_t, float16x4_t, float16x4_t>);
  static_assert(!accepts_7<native::simd<float, 2, (arch_7 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 4, arch_7>, native::simd<native::fp16, 4, arch_7>>);
  constexpr auto arch_8 = native::feature_closure(native::isa<native::arm>(native::arm_feature::fp16fml));
  template<class... A> concept accepts_8 = requires(A... a) { native::fmlal2_lane<arch_8, 0>(a...); };
  static_assert(accepts_8<native::simd<float, 2, arch_8>, native::simd<native::fp16, 4, arch_8>, native::simd<native::fp16, 8, arch_8>>);
  static_assert(!accepts_8<native::simd<std::uint32_t, 2, arch_8>, native::simd<native::fp16, 4, arch_8>, native::simd<native::fp16, 8, arch_8>>);
  static_assert(std::same_as<decltype(native::fmlal2_lane<arch_8, 0>(std::declval<native::simd<float, 2, arch_8>>(), std::declval<native::simd<native::fp16, 4, arch_8>>(), std::declval<native::simd<native::fp16, 8, arch_8>>())), native::simd<float, 2, arch_8>>);
  static_assert(!accepts_8<float32x2_t, float16x4_t, float16x8_t>);
  static_assert(!accepts_8<native::simd<float, 2, (arch_8 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 4, arch_8>, native::simd<native::fp16, 8, arch_8>>);
  constexpr auto arch_9 = native::feature_closure(native::isa<native::arm>(native::arm_feature::fp16fml));
  template<class... A> concept accepts_9 = requires(A... a) { native::fmlal2<arch_9>(a...); };
  static_assert(accepts_9<native::simd<float, 4, arch_9>, native::simd<native::fp16, 8, arch_9>, native::simd<native::fp16, 8, arch_9>>);
  static_assert(!accepts_9<native::simd<std::uint32_t, 4, arch_9>, native::simd<native::fp16, 8, arch_9>, native::simd<native::fp16, 8, arch_9>>);
  static_assert(std::same_as<decltype(native::fmlal2<arch_9>(std::declval<native::simd<float, 4, arch_9>>(), std::declval<native::simd<native::fp16, 8, arch_9>>(), std::declval<native::simd<native::fp16, 8, arch_9>>())), native::simd<float, 4, arch_9>>);
  static_assert(!accepts_9<float32x4_t, float16x8_t, float16x8_t>);
  static_assert(!accepts_9<native::simd<float, 4, (arch_9 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 8, arch_9>, native::simd<native::fp16, 8, arch_9>>);
  constexpr auto arch_10 = native::feature_closure(native::isa<native::arm>(native::arm_feature::fp16fml));
  template<class... A> concept accepts_10 = requires(A... a) { native::fmlal2_lane<arch_10, 0>(a...); };
  static_assert(accepts_10<native::simd<float, 4, arch_10>, native::simd<native::fp16, 8, arch_10>, native::simd<native::fp16, 4, arch_10>>);
  static_assert(!accepts_10<native::simd<std::uint32_t, 4, arch_10>, native::simd<native::fp16, 8, arch_10>, native::simd<native::fp16, 4, arch_10>>);
  static_assert(std::same_as<decltype(native::fmlal2_lane<arch_10, 0>(std::declval<native::simd<float, 4, arch_10>>(), std::declval<native::simd<native::fp16, 8, arch_10>>(), std::declval<native::simd<native::fp16, 4, arch_10>>())), native::simd<float, 4, arch_10>>);
  static_assert(!accepts_10<float32x4_t, float16x8_t, float16x4_t>);
  static_assert(!accepts_10<native::simd<float, 4, (arch_10 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 8, arch_10>, native::simd<native::fp16, 4, arch_10>>);
  constexpr auto arch_11 = native::feature_closure(native::isa<native::arm>(native::arm_feature::fp16fml));
  template<class... A> concept accepts_11 = requires(A... a) { native::fmlal2_lane<arch_11, 0>(a...); };
  static_assert(accepts_11<native::simd<float, 4, arch_11>, native::simd<native::fp16, 8, arch_11>, native::simd<native::fp16, 8, arch_11>>);
  static_assert(!accepts_11<native::simd<std::uint32_t, 4, arch_11>, native::simd<native::fp16, 8, arch_11>, native::simd<native::fp16, 8, arch_11>>);
  static_assert(std::same_as<decltype(native::fmlal2_lane<arch_11, 0>(std::declval<native::simd<float, 4, arch_11>>(), std::declval<native::simd<native::fp16, 8, arch_11>>(), std::declval<native::simd<native::fp16, 8, arch_11>>())), native::simd<float, 4, arch_11>>);
  static_assert(!accepts_11<float32x4_t, float16x8_t, float16x8_t>);
  static_assert(!accepts_11<native::simd<float, 4, (arch_11 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 8, arch_11>, native::simd<native::fp16, 8, arch_11>>);
  constexpr auto arch_12 = native::feature_closure(native::isa<native::arm>(native::arm_feature::fp16fml));
  template<class... A> concept accepts_12 = requires(A... a) { native::fmlsl<arch_12>(a...); };
  static_assert(accepts_12<native::simd<float, 2, arch_12>, native::simd<native::fp16, 4, arch_12>, native::simd<native::fp16, 4, arch_12>>);
  static_assert(!accepts_12<native::simd<std::uint32_t, 2, arch_12>, native::simd<native::fp16, 4, arch_12>, native::simd<native::fp16, 4, arch_12>>);
  static_assert(std::same_as<decltype(native::fmlsl<arch_12>(std::declval<native::simd<float, 2, arch_12>>(), std::declval<native::simd<native::fp16, 4, arch_12>>(), std::declval<native::simd<native::fp16, 4, arch_12>>())), native::simd<float, 2, arch_12>>);
  static_assert(!accepts_12<float32x2_t, float16x4_t, float16x4_t>);
  static_assert(!accepts_12<native::simd<float, 2, (arch_12 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 4, arch_12>, native::simd<native::fp16, 4, arch_12>>);
  constexpr auto arch_13 = native::feature_closure(native::isa<native::arm>(native::arm_feature::fp16fml));
  template<class... A> concept accepts_13 = requires(A... a) { native::fmlsl_lane<arch_13, 0>(a...); };
  static_assert(accepts_13<native::simd<float, 2, arch_13>, native::simd<native::fp16, 4, arch_13>, native::simd<native::fp16, 4, arch_13>>);
  static_assert(!accepts_13<native::simd<std::uint32_t, 2, arch_13>, native::simd<native::fp16, 4, arch_13>, native::simd<native::fp16, 4, arch_13>>);
  static_assert(std::same_as<decltype(native::fmlsl_lane<arch_13, 0>(std::declval<native::simd<float, 2, arch_13>>(), std::declval<native::simd<native::fp16, 4, arch_13>>(), std::declval<native::simd<native::fp16, 4, arch_13>>())), native::simd<float, 2, arch_13>>);
  static_assert(!accepts_13<float32x2_t, float16x4_t, float16x4_t>);
  static_assert(!accepts_13<native::simd<float, 2, (arch_13 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 4, arch_13>, native::simd<native::fp16, 4, arch_13>>);
  constexpr auto arch_14 = native::feature_closure(native::isa<native::arm>(native::arm_feature::fp16fml));
  template<class... A> concept accepts_14 = requires(A... a) { native::fmlsl_lane<arch_14, 0>(a...); };
  static_assert(accepts_14<native::simd<float, 2, arch_14>, native::simd<native::fp16, 4, arch_14>, native::simd<native::fp16, 8, arch_14>>);
  static_assert(!accepts_14<native::simd<std::uint32_t, 2, arch_14>, native::simd<native::fp16, 4, arch_14>, native::simd<native::fp16, 8, arch_14>>);
  static_assert(std::same_as<decltype(native::fmlsl_lane<arch_14, 0>(std::declval<native::simd<float, 2, arch_14>>(), std::declval<native::simd<native::fp16, 4, arch_14>>(), std::declval<native::simd<native::fp16, 8, arch_14>>())), native::simd<float, 2, arch_14>>);
  static_assert(!accepts_14<float32x2_t, float16x4_t, float16x8_t>);
  static_assert(!accepts_14<native::simd<float, 2, (arch_14 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 4, arch_14>, native::simd<native::fp16, 8, arch_14>>);
  constexpr auto arch_15 = native::feature_closure(native::isa<native::arm>(native::arm_feature::fp16fml));
  template<class... A> concept accepts_15 = requires(A... a) { native::fmlsl<arch_15>(a...); };
  static_assert(accepts_15<native::simd<float, 4, arch_15>, native::simd<native::fp16, 8, arch_15>, native::simd<native::fp16, 8, arch_15>>);
  static_assert(!accepts_15<native::simd<std::uint32_t, 4, arch_15>, native::simd<native::fp16, 8, arch_15>, native::simd<native::fp16, 8, arch_15>>);
  static_assert(std::same_as<decltype(native::fmlsl<arch_15>(std::declval<native::simd<float, 4, arch_15>>(), std::declval<native::simd<native::fp16, 8, arch_15>>(), std::declval<native::simd<native::fp16, 8, arch_15>>())), native::simd<float, 4, arch_15>>);
  static_assert(!accepts_15<float32x4_t, float16x8_t, float16x8_t>);
  static_assert(!accepts_15<native::simd<float, 4, (arch_15 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 8, arch_15>, native::simd<native::fp16, 8, arch_15>>);
  constexpr auto arch_16 = native::feature_closure(native::isa<native::arm>(native::arm_feature::fp16fml));
  template<class... A> concept accepts_16 = requires(A... a) { native::fmlsl_lane<arch_16, 0>(a...); };
  static_assert(accepts_16<native::simd<float, 4, arch_16>, native::simd<native::fp16, 8, arch_16>, native::simd<native::fp16, 4, arch_16>>);
  static_assert(!accepts_16<native::simd<std::uint32_t, 4, arch_16>, native::simd<native::fp16, 8, arch_16>, native::simd<native::fp16, 4, arch_16>>);
  static_assert(std::same_as<decltype(native::fmlsl_lane<arch_16, 0>(std::declval<native::simd<float, 4, arch_16>>(), std::declval<native::simd<native::fp16, 8, arch_16>>(), std::declval<native::simd<native::fp16, 4, arch_16>>())), native::simd<float, 4, arch_16>>);
  static_assert(!accepts_16<float32x4_t, float16x8_t, float16x4_t>);
  static_assert(!accepts_16<native::simd<float, 4, (arch_16 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 8, arch_16>, native::simd<native::fp16, 4, arch_16>>);
  constexpr auto arch_17 = native::feature_closure(native::isa<native::arm>(native::arm_feature::fp16fml));
  template<class... A> concept accepts_17 = requires(A... a) { native::fmlsl_lane<arch_17, 0>(a...); };
  static_assert(accepts_17<native::simd<float, 4, arch_17>, native::simd<native::fp16, 8, arch_17>, native::simd<native::fp16, 8, arch_17>>);
  static_assert(!accepts_17<native::simd<std::uint32_t, 4, arch_17>, native::simd<native::fp16, 8, arch_17>, native::simd<native::fp16, 8, arch_17>>);
  static_assert(std::same_as<decltype(native::fmlsl_lane<arch_17, 0>(std::declval<native::simd<float, 4, arch_17>>(), std::declval<native::simd<native::fp16, 8, arch_17>>(), std::declval<native::simd<native::fp16, 8, arch_17>>())), native::simd<float, 4, arch_17>>);
  static_assert(!accepts_17<float32x4_t, float16x8_t, float16x8_t>);
  static_assert(!accepts_17<native::simd<float, 4, (arch_17 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 8, arch_17>, native::simd<native::fp16, 8, arch_17>>);
  constexpr auto arch_18 = native::feature_closure(native::isa<native::arm>(native::arm_feature::fp16fml));
  template<class... A> concept accepts_18 = requires(A... a) { native::fmlsl2<arch_18>(a...); };
  static_assert(accepts_18<native::simd<float, 2, arch_18>, native::simd<native::fp16, 4, arch_18>, native::simd<native::fp16, 4, arch_18>>);
  static_assert(!accepts_18<native::simd<std::uint32_t, 2, arch_18>, native::simd<native::fp16, 4, arch_18>, native::simd<native::fp16, 4, arch_18>>);
  static_assert(std::same_as<decltype(native::fmlsl2<arch_18>(std::declval<native::simd<float, 2, arch_18>>(), std::declval<native::simd<native::fp16, 4, arch_18>>(), std::declval<native::simd<native::fp16, 4, arch_18>>())), native::simd<float, 2, arch_18>>);
  static_assert(!accepts_18<float32x2_t, float16x4_t, float16x4_t>);
  static_assert(!accepts_18<native::simd<float, 2, (arch_18 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 4, arch_18>, native::simd<native::fp16, 4, arch_18>>);
  constexpr auto arch_19 = native::feature_closure(native::isa<native::arm>(native::arm_feature::fp16fml));
  template<class... A> concept accepts_19 = requires(A... a) { native::fmlsl2_lane<arch_19, 0>(a...); };
  static_assert(accepts_19<native::simd<float, 2, arch_19>, native::simd<native::fp16, 4, arch_19>, native::simd<native::fp16, 4, arch_19>>);
  static_assert(!accepts_19<native::simd<std::uint32_t, 2, arch_19>, native::simd<native::fp16, 4, arch_19>, native::simd<native::fp16, 4, arch_19>>);
  static_assert(std::same_as<decltype(native::fmlsl2_lane<arch_19, 0>(std::declval<native::simd<float, 2, arch_19>>(), std::declval<native::simd<native::fp16, 4, arch_19>>(), std::declval<native::simd<native::fp16, 4, arch_19>>())), native::simd<float, 2, arch_19>>);
  static_assert(!accepts_19<float32x2_t, float16x4_t, float16x4_t>);
  static_assert(!accepts_19<native::simd<float, 2, (arch_19 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 4, arch_19>, native::simd<native::fp16, 4, arch_19>>);
  constexpr auto arch_20 = native::feature_closure(native::isa<native::arm>(native::arm_feature::fp16fml));
  template<class... A> concept accepts_20 = requires(A... a) { native::fmlsl2_lane<arch_20, 0>(a...); };
  static_assert(accepts_20<native::simd<float, 2, arch_20>, native::simd<native::fp16, 4, arch_20>, native::simd<native::fp16, 8, arch_20>>);
  static_assert(!accepts_20<native::simd<std::uint32_t, 2, arch_20>, native::simd<native::fp16, 4, arch_20>, native::simd<native::fp16, 8, arch_20>>);
  static_assert(std::same_as<decltype(native::fmlsl2_lane<arch_20, 0>(std::declval<native::simd<float, 2, arch_20>>(), std::declval<native::simd<native::fp16, 4, arch_20>>(), std::declval<native::simd<native::fp16, 8, arch_20>>())), native::simd<float, 2, arch_20>>);
  static_assert(!accepts_20<float32x2_t, float16x4_t, float16x8_t>);
  static_assert(!accepts_20<native::simd<float, 2, (arch_20 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 4, arch_20>, native::simd<native::fp16, 8, arch_20>>);
  constexpr auto arch_21 = native::feature_closure(native::isa<native::arm>(native::arm_feature::fp16fml));
  template<class... A> concept accepts_21 = requires(A... a) { native::fmlsl2<arch_21>(a...); };
  static_assert(accepts_21<native::simd<float, 4, arch_21>, native::simd<native::fp16, 8, arch_21>, native::simd<native::fp16, 8, arch_21>>);
  static_assert(!accepts_21<native::simd<std::uint32_t, 4, arch_21>, native::simd<native::fp16, 8, arch_21>, native::simd<native::fp16, 8, arch_21>>);
  static_assert(std::same_as<decltype(native::fmlsl2<arch_21>(std::declval<native::simd<float, 4, arch_21>>(), std::declval<native::simd<native::fp16, 8, arch_21>>(), std::declval<native::simd<native::fp16, 8, arch_21>>())), native::simd<float, 4, arch_21>>);
  static_assert(!accepts_21<float32x4_t, float16x8_t, float16x8_t>);
  static_assert(!accepts_21<native::simd<float, 4, (arch_21 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 8, arch_21>, native::simd<native::fp16, 8, arch_21>>);
  constexpr auto arch_22 = native::feature_closure(native::isa<native::arm>(native::arm_feature::fp16fml));
  template<class... A> concept accepts_22 = requires(A... a) { native::fmlsl2_lane<arch_22, 0>(a...); };
  static_assert(accepts_22<native::simd<float, 4, arch_22>, native::simd<native::fp16, 8, arch_22>, native::simd<native::fp16, 4, arch_22>>);
  static_assert(!accepts_22<native::simd<std::uint32_t, 4, arch_22>, native::simd<native::fp16, 8, arch_22>, native::simd<native::fp16, 4, arch_22>>);
  static_assert(std::same_as<decltype(native::fmlsl2_lane<arch_22, 0>(std::declval<native::simd<float, 4, arch_22>>(), std::declval<native::simd<native::fp16, 8, arch_22>>(), std::declval<native::simd<native::fp16, 4, arch_22>>())), native::simd<float, 4, arch_22>>);
  static_assert(!accepts_22<float32x4_t, float16x8_t, float16x4_t>);
  static_assert(!accepts_22<native::simd<float, 4, (arch_22 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 8, arch_22>, native::simd<native::fp16, 4, arch_22>>);
  constexpr auto arch_23 = native::feature_closure(native::isa<native::arm>(native::arm_feature::fp16fml));
  template<class... A> concept accepts_23 = requires(A... a) { native::fmlsl2_lane<arch_23, 0>(a...); };
  static_assert(accepts_23<native::simd<float, 4, arch_23>, native::simd<native::fp16, 8, arch_23>, native::simd<native::fp16, 8, arch_23>>);
  static_assert(!accepts_23<native::simd<std::uint32_t, 4, arch_23>, native::simd<native::fp16, 8, arch_23>, native::simd<native::fp16, 8, arch_23>>);
  static_assert(std::same_as<decltype(native::fmlsl2_lane<arch_23, 0>(std::declval<native::simd<float, 4, arch_23>>(), std::declval<native::simd<native::fp16, 8, arch_23>>(), std::declval<native::simd<native::fp16, 8, arch_23>>())), native::simd<float, 4, arch_23>>);
  static_assert(!accepts_23<float32x4_t, float16x8_t, float16x8_t>);
  static_assert(!accepts_23<native::simd<float, 4, (arch_23 & native::isa<native::arm>(native::arm_feature::crc))>, native::simd<native::fp16, 8, arch_23>, native::simd<native::fp16, 8, arch_23>>);
}
namespace fp16fml_fixture {
  constexpr native::isa<native::arm> arch{native::arm_feature::fp16fml};
  template<native::isa<native::arm> A, class F, class H> concept available = requires(F f, H h) {
    { fp16fml_api::fmlal<A>(f, h, h) } noexcept -> std::same_as<F>;
    { fp16fml_api::fmlal2<A>(f, h, h) } noexcept -> std::same_as<F>;
    { fp16fml_api::fmlsl<A>(f, h, h) } noexcept -> std::same_as<F>;
    { fp16fml_api::fmlsl2<A>(f, h, h) } noexcept -> std::same_as<F>;
  };
  template<native::isa<native::arm> A, unsigned L, class F, class H, class B>
  concept lane_available = requires(F f, H h, B b) {
    fp16fml_api::fmlal_lane<A, L>(f, h, b);
    fp16fml_api::fmlal2_lane<A, L>(f, h, b);
    fp16fml_api::fmlsl_lane<A, L>(f, h, b);
    fp16fml_api::fmlsl2_lane<A, L>(f, h, b);
  };
  // Weak feature tags participate only through consteval overloads; runtime inputs are rejected.
  static_assert(available<arch, float32x2_t, float16x4_t>);
  static_assert(!available<native::scalar, float32x2_t, float16x4_t>);
  static_assert(available<native::neon, float32x2_t, float16x4_t>);
  static_assert(available<native::neon_fp16, float32x2_t, float16x4_t>);
  static_assert(available<native::isa<native::arm>{native::arm_feature::complxnum}, float32x2_t, float16x4_t>);
  static_assert(available<native::neon_bf16, float32x2_t, float16x4_t>);
  static_assert(lane_available<arch, 3, float32x2_t, float16x4_t, float16x4_t>);
  static_assert(!lane_available<arch, 4, float32x2_t, float16x4_t, float16x4_t>);
  static_assert(lane_available<arch, 7, float32x2_t, float16x4_t, float16x8_t>);
  static_assert(!lane_available<arch, 8, float32x2_t, float16x4_t, float16x8_t>);
  static_assert(available<arch, float32x4_t, float16x8_t>);
  static_assert(!available<native::scalar, float32x4_t, float16x8_t>);
  static_assert(available<native::neon, float32x4_t, float16x8_t>);
  static_assert(available<native::neon_fp16, float32x4_t, float16x8_t>);
  static_assert(available<native::isa<native::arm>{native::arm_feature::complxnum}, float32x4_t, float16x8_t>);
  static_assert(available<native::neon_bf16, float32x4_t, float16x8_t>);
  static_assert(lane_available<arch, 3, float32x4_t, float16x8_t, float16x4_t>);
  static_assert(!lane_available<arch, 4, float32x4_t, float16x8_t, float16x4_t>);
  static_assert(lane_available<arch, 7, float32x4_t, float16x8_t, float16x8_t>);
  static_assert(!lane_available<arch, 8, float32x4_t, float16x8_t, float16x8_t>);

  consteval bool admission_contract() {
    native::arm_capabilities cpu{};
    cpu.present.set(native::arm_feature::neon, true);
    cpu.observed.set(native::arm_feature::neon, true);
    cpu.present.set(native::arm_feature::neon_fp16, true);
    cpu.observed.set(native::arm_feature::neon_fp16, true);
    cpu.present.set(native::arm_feature::fp16fml, true);
    cpu.observed.set(native::arm_feature::fp16fml, true);
    if(!native::classify_isa(cpu, arch).admitted()) return false;
    for(auto feature : {native::arm_feature::neon, native::arm_feature::neon_fp16, native::arm_feature::fp16fml}) {
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
  inline bool same(float a, float b) {
    return std::bit_cast<std::uint32_t>(a) == std::bit_cast<std::uint32_t>(b)
      || (std::isnan(a) && std::isnan(b));
  }
  template<class F, class H, class B, unsigned Lane>
  __attribute__((target("fp16fml"), noinline))
  bool check(F acc, H a, B b) {
    constexpr unsigned n = sizeof(F) / sizeof(float);
    auto l = fp16fml_api::fmlal_lane<arch, Lane>(acc, a, b);
    auto h = fp16fml_api::fmlal2_lane<arch, Lane>(acc, a, b);
    auto sl = fp16fml_api::fmlsl_lane<arch, Lane>(acc, a, b);
    auto sh = fp16fml_api::fmlsl2_lane<arch, Lane>(acc, a, b);
    for(unsigned i = 0; i != n; ++i) {
      float bv = float(b[Lane]);
      if(!same(l[i], std::fma(float(a[i]), bv, acc[i])) ||
         !same(h[i], std::fma(float(a[i+n]), bv, acc[i])) ||
         !same(sl[i], std::fma(-float(a[i]), bv, acc[i])) ||
         !same(sh[i], std::fma(-float(a[i+n]), bv, acc[i]))) return false;
    }
    return true;
  }
  template<class F, class H>
  __attribute__((target("fp16fml"), noinline))
  bool check(F acc, H a, H b) {
    constexpr unsigned n = sizeof(F) / sizeof(float);
    auto l = fp16fml_api::fmlal<arch>(acc, a, b);
    auto h = fp16fml_api::fmlal2<arch>(acc, a, b);
    auto sl = fp16fml_api::fmlsl<arch>(acc, a, b);
    auto sh = fp16fml_api::fmlsl2<arch>(acc, a, b);
    for(unsigned i = 0; i != n; ++i) {
      if(!same(l[i], std::fma(float(a[i]), float(b[i]), acc[i])) ||
         !same(h[i], std::fma(float(a[i+n]), float(b[i+n]), acc[i])) ||
         !same(sl[i], std::fma(-float(a[i]), float(b[i]), acc[i])) ||
         !same(sh[i], std::fma(-float(a[i+n]), float(b[i+n]), acc[i]))) return false;
    }
    return true;
  }
  __attribute__((target("fp16fml"), noinline))
  bool exercise(unsigned seed) {
    auto control = fpcr();
    for(unsigned k = 0; k != 256; ++k) {
      float16x8_t a{}, b{}; float32x4_t acc{};
      for(unsigned i = 0; i != 8; ++i) {
        seed = seed * 1664525u + 1013904223u;
        a[i] = __builtin_bit_cast(__fp16, std::uint16_t((seed & 0x83ffu) | 0x3800u));
        seed = seed * 1664525u + 1013904223u;
        b[i] = __builtin_bit_cast(__fp16, std::uint16_t((seed & 0x83ffu) | 0x4000u));
        if(i < 4) acc[i] = std::bit_cast<float>((seed & 0x807fffffu) | 0x3f000000u);
      }
      auto a2 = vget_low_f16(a); auto b2 = vget_low_f16(b); auto acc2 = vget_low_f32(acc);
      if(!check(acc, a, b) || !check(acc2, a2, b2)) return false;
      if(!check<float32x2_t, float16x4_t, float16x4_t, 0>(acc2, a2, b2)) return false;
      if(!check<float32x2_t, float16x4_t, float16x4_t, 1>(acc2, a2, b2)) return false;
      if(!check<float32x2_t, float16x4_t, float16x4_t, 2>(acc2, a2, b2)) return false;
      if(!check<float32x2_t, float16x4_t, float16x4_t, 3>(acc2, a2, b2)) return false;
      if(!check<float32x2_t, float16x4_t, float16x8_t, 0>(acc2, a2, b)) return false;
      if(!check<float32x2_t, float16x4_t, float16x8_t, 1>(acc2, a2, b)) return false;
      if(!check<float32x2_t, float16x4_t, float16x8_t, 2>(acc2, a2, b)) return false;
      if(!check<float32x2_t, float16x4_t, float16x8_t, 3>(acc2, a2, b)) return false;
      if(!check<float32x2_t, float16x4_t, float16x8_t, 4>(acc2, a2, b)) return false;
      if(!check<float32x2_t, float16x4_t, float16x8_t, 5>(acc2, a2, b)) return false;
      if(!check<float32x2_t, float16x4_t, float16x8_t, 6>(acc2, a2, b)) return false;
      if(!check<float32x2_t, float16x4_t, float16x8_t, 7>(acc2, a2, b)) return false;
      if(!check<float32x4_t, float16x8_t, float16x4_t, 0>(acc, a, b2)) return false;
      if(!check<float32x4_t, float16x8_t, float16x4_t, 1>(acc, a, b2)) return false;
      if(!check<float32x4_t, float16x8_t, float16x4_t, 2>(acc, a, b2)) return false;
      if(!check<float32x4_t, float16x8_t, float16x4_t, 3>(acc, a, b2)) return false;
      if(!check<float32x4_t, float16x8_t, float16x8_t, 0>(acc, a, b)) return false;
      if(!check<float32x4_t, float16x8_t, float16x8_t, 1>(acc, a, b)) return false;
      if(!check<float32x4_t, float16x8_t, float16x8_t, 2>(acc, a, b)) return false;
      if(!check<float32x4_t, float16x8_t, float16x8_t, 3>(acc, a, b)) return false;
      if(!check<float32x4_t, float16x8_t, float16x8_t, 4>(acc, a, b)) return false;
      if(!check<float32x4_t, float16x8_t, float16x8_t, 5>(acc, a, b)) return false;
      if(!check<float32x4_t, float16x8_t, float16x8_t, 6>(acc, a, b)) return false;
      if(!check<float32x4_t, float16x8_t, float16x8_t, 7>(acc, a, b)) return false;
    }
    // Signed zero, subnormal inputs, and invalid 0*infinity are observed under
    // the caller's default FPCR; NaN payload identity is deliberately not assumed.
    float16x8_t a{0, -0.0, 1, -1, 2, -2, 0, 0};
    float16x8_t b{1, 1, 1, 1, 1, 1, 1, 1};
    float32x4_t acc{0, -0.0f, 0, 0};
    if(!check(acc, a, b)) return false;
    a[0] = __builtin_bit_cast(__fp16, std::uint16_t(1));
    if(!check(acc, a, b)) return false;
    a[0] = 0; b[0] = __builtin_bit_cast(__fp16, std::uint16_t(0x7c00));
    std::feclearexcept(FE_ALL_EXCEPT);
    volatile auto invalid = fp16fml_api::fmlal<arch>(acc, a, b);
    (void)invalid;
    if(!(std::fetestexcept(FE_INVALID) & FE_INVALID)) return false;
    std::feclearexcept(FE_ALL_EXCEPT);
    (void)fp16fml_api::fmlal<arch>(acc, a, b);
    if(!(std::fetestexcept(FE_INVALID) & FE_INVALID)) return false;
    if(std::feraiseexcept(FE_DIVBYZERO) != 0) return false;
    b[0] = 1;
    (void)fp16fml_api::fmlal<arch>(acc, a, b);
    if(!(std::fetestexcept(FE_DIVBYZERO) & FE_DIVBYZERO)) return false;
    return fpcr() == control;
  }
  inline int run(unsigned seed) {
    auto admission = native::classify_isa(native::observe_arm_capabilities(), arch);
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
    if(!ok) std::puts("FP16FML operation graph or FP environment check failed");
    return ok ? 0 : 1;
  }
}
