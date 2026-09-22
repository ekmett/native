// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "simd_bridge.h"
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
namespace crypto_fixture {
  template<native::isa<native::arm> A, class T0, class T1> concept available_aes_0 = requires(T0 a0, T1 a1) { native::aese<A>(a0, a1); };
  static_assert(available_aes_0<native::feature_closure(native::isa<native::arm>{native::arm_feature::aes}), native::simd<std::uint8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::aes})>, native::simd<std::uint8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::aes})>>);
  static_assert(available_aes_0<native::neon, native::simd<std::uint8_t, 16, native::neon>, native::simd<std::uint8_t, 16, native::neon>>);
  static_assert(!available_aes_0<native::feature_closure(native::isa<native::arm>{native::arm_feature::aes}), double, native::simd<std::uint8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::aes})>>);
  static_assert(!available_aes_0<native::feature_closure(native::isa<native::arm>{native::arm_feature::aes}), uint8x16_t, uint8x16_t>);
  template<native::isa<native::arm> A, class T0, class T1> concept available_aes_1 = requires(T0 a0, T1 a1) { native::aesd<A>(a0, a1); };
  static_assert(available_aes_1<native::feature_closure(native::isa<native::arm>{native::arm_feature::aes}), native::simd<std::uint8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::aes})>, native::simd<std::uint8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::aes})>>);
  static_assert(available_aes_1<native::neon, native::simd<std::uint8_t, 16, native::neon>, native::simd<std::uint8_t, 16, native::neon>>);
  static_assert(!available_aes_1<native::feature_closure(native::isa<native::arm>{native::arm_feature::aes}), double, native::simd<std::uint8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::aes})>>);
  static_assert(!available_aes_1<native::feature_closure(native::isa<native::arm>{native::arm_feature::aes}), uint8x16_t, uint8x16_t>);
  template<native::isa<native::arm> A, class T0> concept available_aes_2 = requires(T0 a0) { native::aesmc<A>(a0); };
  static_assert(available_aes_2<native::feature_closure(native::isa<native::arm>{native::arm_feature::aes}), native::simd<std::uint8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::aes})>>);
  static_assert(available_aes_2<native::neon, native::simd<std::uint8_t, 16, native::neon>>);
  static_assert(!available_aes_2<native::feature_closure(native::isa<native::arm>{native::arm_feature::aes}), double>);
  static_assert(!available_aes_2<native::feature_closure(native::isa<native::arm>{native::arm_feature::aes}), uint8x16_t>);
  template<native::isa<native::arm> A, class T0> concept available_aes_3 = requires(T0 a0) { native::aesimc<A>(a0); };
  static_assert(available_aes_3<native::feature_closure(native::isa<native::arm>{native::arm_feature::aes}), native::simd<std::uint8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::aes})>>);
  static_assert(available_aes_3<native::neon, native::simd<std::uint8_t, 16, native::neon>>);
  static_assert(!available_aes_3<native::feature_closure(native::isa<native::arm>{native::arm_feature::aes}), double>);
  static_assert(!available_aes_3<native::feature_closure(native::isa<native::arm>{native::arm_feature::aes}), uint8x16_t>);
  template<native::isa<native::arm> A, class T0, class T1> concept available_pmull_0 = requires(T0 a0, T1 a1) { native::pmull<A>(a0, a1); };
  static_assert(available_pmull_0<native::feature_closure(native::isa<native::arm>{native::arm_feature::pmull}), std::uint64_t, std::uint64_t>);
  static_assert(available_pmull_0<native::neon, std::uint64_t, std::uint64_t>);
  static_assert(!available_pmull_0<native::scalar, std::uint64_t, std::uint64_t>);
  static_assert(!available_pmull_0<native::feature_closure(native::isa<native::arm>{native::arm_feature::pmull}), double, std::uint64_t>);
  template<native::isa<native::arm> A, class T0, class T1> concept available_pmull_1 = requires(T0 a0, T1 a1) { native::pmull2<A>(a0, a1); };
  static_assert(available_pmull_1<native::feature_closure(native::isa<native::arm>{native::arm_feature::pmull}), native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::pmull})>, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::pmull})>>);
  static_assert(available_pmull_1<native::neon, native::simd<std::uint64_t, 2, native::neon>, native::simd<std::uint64_t, 2, native::neon>>);
  static_assert(!available_pmull_1<native::feature_closure(native::isa<native::arm>{native::arm_feature::pmull}), double, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::pmull})>>);
  static_assert(!available_pmull_1<native::feature_closure(native::isa<native::arm>{native::arm_feature::pmull}), poly64x2_t, poly64x2_t>);
  template<native::isa<native::arm> A, class T0, class T1> concept available_pmull_2 = requires(T0 a0, T1 a1) { native::pmull<A>(a0, a1); };
  static_assert(available_pmull_2<native::feature_closure(native::isa<native::arm>{native::arm_feature::neon}), native::simd<std::uint8_t, 8, native::feature_closure(native::isa<native::arm>{native::arm_feature::neon})>, native::simd<std::uint8_t, 8, native::feature_closure(native::isa<native::arm>{native::arm_feature::neon})>>);
  static_assert(!available_pmull_2<native::scalar, native::simd<std::uint8_t, 8, native::scalar>, native::simd<std::uint8_t, 8, native::scalar>>);
  static_assert(!available_pmull_2<native::feature_closure(native::isa<native::arm>{native::arm_feature::neon}), double, native::simd<std::uint8_t, 8, native::feature_closure(native::isa<native::arm>{native::arm_feature::neon})>>);
  static_assert(!available_pmull_2<native::feature_closure(native::isa<native::arm>{native::arm_feature::neon}), poly8x8_t, poly8x8_t>);
  template<native::isa<native::arm> A, class T0, class T1> concept available_pmull_3 = requires(T0 a0, T1 a1) { native::pmull2<A>(a0, a1); };
  static_assert(available_pmull_3<native::feature_closure(native::isa<native::arm>{native::arm_feature::neon}), native::simd<std::uint8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::neon})>, native::simd<std::uint8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::neon})>>);
  static_assert(!available_pmull_3<native::scalar, native::simd<std::uint8_t, 16, native::scalar>, native::simd<std::uint8_t, 16, native::scalar>>);
  static_assert(!available_pmull_3<native::feature_closure(native::isa<native::arm>{native::arm_feature::neon}), double, native::simd<std::uint8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::neon})>>);
  static_assert(!available_pmull_3<native::feature_closure(native::isa<native::arm>{native::arm_feature::neon}), poly8x16_t, poly8x16_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_0 = requires(T0 a0, T1 a1, T2 a2) { native::sha1c<A>(a0, a1, a2); };
  static_assert(available_sha_0<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1}), native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1})>, std::uint32_t, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1})>>);
  static_assert(available_sha_0<native::neon, native::simd<std::uint32_t, 4, native::neon>, std::uint32_t, native::simd<std::uint32_t, 4, native::neon>>);
  static_assert(!available_sha_0<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1}), double, std::uint32_t, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1})>>);
  static_assert(!available_sha_0<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1}), uint32x4_t, std::uint32_t, uint32x4_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_1 = requires(T0 a0, T1 a1, T2 a2) { native::sha1p<A>(a0, a1, a2); };
  static_assert(available_sha_1<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1}), native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1})>, std::uint32_t, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1})>>);
  static_assert(available_sha_1<native::neon, native::simd<std::uint32_t, 4, native::neon>, std::uint32_t, native::simd<std::uint32_t, 4, native::neon>>);
  static_assert(!available_sha_1<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1}), double, std::uint32_t, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1})>>);
  static_assert(!available_sha_1<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1}), uint32x4_t, std::uint32_t, uint32x4_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_2 = requires(T0 a0, T1 a1, T2 a2) { native::sha1m<A>(a0, a1, a2); };
  static_assert(available_sha_2<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1}), native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1})>, std::uint32_t, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1})>>);
  static_assert(available_sha_2<native::neon, native::simd<std::uint32_t, 4, native::neon>, std::uint32_t, native::simd<std::uint32_t, 4, native::neon>>);
  static_assert(!available_sha_2<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1}), double, std::uint32_t, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1})>>);
  static_assert(!available_sha_2<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1}), uint32x4_t, std::uint32_t, uint32x4_t>);
  template<native::isa<native::arm> A, class T0> concept available_sha_3 = requires(T0 a0) { native::sha1h<A>(a0); };
  static_assert(available_sha_3<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1}), std::uint32_t>);
  // The weak overload is immediate-only; an actual runtime rejection is tested separately.
  static_assert(available_sha_3<native::neon, std::uint32_t>);
  static_assert(!available_sha_3<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1}), double>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_4 = requires(T0 a0, T1 a1, T2 a2) { native::sha1su0<A>(a0, a1, a2); };
  static_assert(available_sha_4<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1}), native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1})>, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1})>, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1})>>);
  static_assert(available_sha_4<native::neon, native::simd<std::uint32_t, 4, native::neon>, native::simd<std::uint32_t, 4, native::neon>, native::simd<std::uint32_t, 4, native::neon>>);
  static_assert(!available_sha_4<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1}), double, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1})>, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1})>>);
  static_assert(!available_sha_4<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1}), uint32x4_t, uint32x4_t, uint32x4_t>);
  template<native::isa<native::arm> A, class T0, class T1> concept available_sha_5 = requires(T0 a0, T1 a1) { native::sha1su1<A>(a0, a1); };
  static_assert(available_sha_5<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1}), native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1})>, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1})>>);
  static_assert(available_sha_5<native::neon, native::simd<std::uint32_t, 4, native::neon>, native::simd<std::uint32_t, 4, native::neon>>);
  static_assert(!available_sha_5<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1}), double, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1})>>);
  static_assert(!available_sha_5<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1}), uint32x4_t, uint32x4_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_6 = requires(T0 a0, T1 a1, T2 a2) { native::sha256h<A>(a0, a1, a2); };
  static_assert(available_sha_6<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2}), native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2})>, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2})>, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2})>>);
  static_assert(available_sha_6<native::neon, native::simd<std::uint32_t, 4, native::neon>, native::simd<std::uint32_t, 4, native::neon>, native::simd<std::uint32_t, 4, native::neon>>);
  static_assert(!available_sha_6<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2}), double, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2})>, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2})>>);
  static_assert(!available_sha_6<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2}), uint32x4_t, uint32x4_t, uint32x4_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_7 = requires(T0 a0, T1 a1, T2 a2) { native::sha256h2<A>(a0, a1, a2); };
  static_assert(available_sha_7<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2}), native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2})>, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2})>, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2})>>);
  static_assert(available_sha_7<native::neon, native::simd<std::uint32_t, 4, native::neon>, native::simd<std::uint32_t, 4, native::neon>, native::simd<std::uint32_t, 4, native::neon>>);
  static_assert(!available_sha_7<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2}), double, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2})>, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2})>>);
  static_assert(!available_sha_7<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2}), uint32x4_t, uint32x4_t, uint32x4_t>);
  template<native::isa<native::arm> A, class T0, class T1> concept available_sha_8 = requires(T0 a0, T1 a1) { native::sha256su0<A>(a0, a1); };
  static_assert(available_sha_8<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2}), native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2})>, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2})>>);
  static_assert(available_sha_8<native::neon, native::simd<std::uint32_t, 4, native::neon>, native::simd<std::uint32_t, 4, native::neon>>);
  static_assert(!available_sha_8<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2}), double, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2})>>);
  static_assert(!available_sha_8<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2}), uint32x4_t, uint32x4_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_9 = requires(T0 a0, T1 a1, T2 a2) { native::sha256su1<A>(a0, a1, a2); };
  static_assert(available_sha_9<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2}), native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2})>, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2})>, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2})>>);
  static_assert(available_sha_9<native::neon, native::simd<std::uint32_t, 4, native::neon>, native::simd<std::uint32_t, 4, native::neon>, native::simd<std::uint32_t, 4, native::neon>>);
  static_assert(!available_sha_9<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2}), double, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2})>, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2})>>);
  static_assert(!available_sha_9<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2}), uint32x4_t, uint32x4_t, uint32x4_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_10 = requires(T0 a0, T1 a1, T2 a2) { native::sha512h<A>(a0, a1, a2); };
  static_assert(available_sha_10<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512}), native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512})>, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512})>, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512})>>);
  static_assert(available_sha_10<native::neon, native::simd<std::uint64_t, 2, native::neon>, native::simd<std::uint64_t, 2, native::neon>, native::simd<std::uint64_t, 2, native::neon>>);
  static_assert(!available_sha_10<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512}), double, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512})>, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512})>>);
  static_assert(!available_sha_10<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512}), uint64x2_t, uint64x2_t, uint64x2_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_11 = requires(T0 a0, T1 a1, T2 a2) { native::sha512h2<A>(a0, a1, a2); };
  static_assert(available_sha_11<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512}), native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512})>, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512})>, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512})>>);
  static_assert(available_sha_11<native::neon, native::simd<std::uint64_t, 2, native::neon>, native::simd<std::uint64_t, 2, native::neon>, native::simd<std::uint64_t, 2, native::neon>>);
  static_assert(!available_sha_11<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512}), double, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512})>, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512})>>);
  static_assert(!available_sha_11<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512}), uint64x2_t, uint64x2_t, uint64x2_t>);
  template<native::isa<native::arm> A, class T0, class T1> concept available_sha_12 = requires(T0 a0, T1 a1) { native::sha512su0<A>(a0, a1); };
  static_assert(available_sha_12<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512}), native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512})>, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512})>>);
  static_assert(available_sha_12<native::neon, native::simd<std::uint64_t, 2, native::neon>, native::simd<std::uint64_t, 2, native::neon>>);
  static_assert(!available_sha_12<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512}), double, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512})>>);
  static_assert(!available_sha_12<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512}), uint64x2_t, uint64x2_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_13 = requires(T0 a0, T1 a1, T2 a2) { native::sha512su1<A>(a0, a1, a2); };
  static_assert(available_sha_13<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512}), native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512})>, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512})>, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512})>>);
  static_assert(available_sha_13<native::neon, native::simd<std::uint64_t, 2, native::neon>, native::simd<std::uint64_t, 2, native::neon>, native::simd<std::uint64_t, 2, native::neon>>);
  static_assert(!available_sha_13<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512}), double, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512})>, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512})>>);
  static_assert(!available_sha_13<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512}), uint64x2_t, uint64x2_t, uint64x2_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_14 = requires(T0 a0, T1 a1, T2 a2) { native::eor3<A>(a0, a1, a2); };
  static_assert(available_sha_14<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), native::simd<std::uint8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(available_sha_14<native::neon, native::simd<std::uint8_t, 16, native::neon>, native::simd<std::uint8_t, 16, native::neon>, native::simd<std::uint8_t, 16, native::neon>>);
  static_assert(!available_sha_14<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), double, native::simd<std::uint8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(!available_sha_14<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), uint8x16_t, uint8x16_t, uint8x16_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_15 = requires(T0 a0, T1 a1, T2 a2) { native::eor3<A>(a0, a1, a2); };
  static_assert(available_sha_15<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), native::simd<std::uint16_t, 8, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint16_t, 8, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint16_t, 8, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(available_sha_15<native::neon, native::simd<std::uint16_t, 8, native::neon>, native::simd<std::uint16_t, 8, native::neon>, native::simd<std::uint16_t, 8, native::neon>>);
  static_assert(!available_sha_15<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), double, native::simd<std::uint16_t, 8, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint16_t, 8, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(!available_sha_15<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), uint16x8_t, uint16x8_t, uint16x8_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_16 = requires(T0 a0, T1 a1, T2 a2) { native::eor3<A>(a0, a1, a2); };
  static_assert(available_sha_16<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(available_sha_16<native::neon, native::simd<std::uint32_t, 4, native::neon>, native::simd<std::uint32_t, 4, native::neon>, native::simd<std::uint32_t, 4, native::neon>>);
  static_assert(!available_sha_16<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), double, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(!available_sha_16<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), uint32x4_t, uint32x4_t, uint32x4_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_17 = requires(T0 a0, T1 a1, T2 a2) { native::eor3<A>(a0, a1, a2); };
  static_assert(available_sha_17<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(available_sha_17<native::neon, native::simd<std::uint64_t, 2, native::neon>, native::simd<std::uint64_t, 2, native::neon>, native::simd<std::uint64_t, 2, native::neon>>);
  static_assert(!available_sha_17<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), double, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(!available_sha_17<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), uint64x2_t, uint64x2_t, uint64x2_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_18 = requires(T0 a0, T1 a1, T2 a2) { native::eor3<A>(a0, a1, a2); };
  static_assert(available_sha_18<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), native::simd<std::int8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::int8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::int8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(available_sha_18<native::neon, native::simd<std::int8_t, 16, native::neon>, native::simd<std::int8_t, 16, native::neon>, native::simd<std::int8_t, 16, native::neon>>);
  static_assert(!available_sha_18<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), double, native::simd<std::int8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::int8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(!available_sha_18<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), int8x16_t, int8x16_t, int8x16_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_19 = requires(T0 a0, T1 a1, T2 a2) { native::eor3<A>(a0, a1, a2); };
  static_assert(available_sha_19<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), native::simd<std::int16_t, 8, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::int16_t, 8, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::int16_t, 8, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(available_sha_19<native::neon, native::simd<std::int16_t, 8, native::neon>, native::simd<std::int16_t, 8, native::neon>, native::simd<std::int16_t, 8, native::neon>>);
  static_assert(!available_sha_19<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), double, native::simd<std::int16_t, 8, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::int16_t, 8, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(!available_sha_19<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), int16x8_t, int16x8_t, int16x8_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_20 = requires(T0 a0, T1 a1, T2 a2) { native::eor3<A>(a0, a1, a2); };
  static_assert(available_sha_20<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), native::simd<std::int32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::int32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::int32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(available_sha_20<native::neon, native::simd<std::int32_t, 4, native::neon>, native::simd<std::int32_t, 4, native::neon>, native::simd<std::int32_t, 4, native::neon>>);
  static_assert(!available_sha_20<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), double, native::simd<std::int32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::int32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(!available_sha_20<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), int32x4_t, int32x4_t, int32x4_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_21 = requires(T0 a0, T1 a1, T2 a2) { native::eor3<A>(a0, a1, a2); };
  static_assert(available_sha_21<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), native::simd<std::int64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::int64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::int64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(available_sha_21<native::neon, native::simd<std::int64_t, 2, native::neon>, native::simd<std::int64_t, 2, native::neon>, native::simd<std::int64_t, 2, native::neon>>);
  static_assert(!available_sha_21<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), double, native::simd<std::int64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::int64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(!available_sha_21<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), int64x2_t, int64x2_t, int64x2_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_22 = requires(T0 a0, T1 a1, T2 a2) { native::bcax<A>(a0, a1, a2); };
  static_assert(available_sha_22<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), native::simd<std::uint8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(available_sha_22<native::neon, native::simd<std::uint8_t, 16, native::neon>, native::simd<std::uint8_t, 16, native::neon>, native::simd<std::uint8_t, 16, native::neon>>);
  static_assert(!available_sha_22<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), double, native::simd<std::uint8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(!available_sha_22<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), uint8x16_t, uint8x16_t, uint8x16_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_23 = requires(T0 a0, T1 a1, T2 a2) { native::bcax<A>(a0, a1, a2); };
  static_assert(available_sha_23<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), native::simd<std::uint16_t, 8, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint16_t, 8, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint16_t, 8, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(available_sha_23<native::neon, native::simd<std::uint16_t, 8, native::neon>, native::simd<std::uint16_t, 8, native::neon>, native::simd<std::uint16_t, 8, native::neon>>);
  static_assert(!available_sha_23<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), double, native::simd<std::uint16_t, 8, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint16_t, 8, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(!available_sha_23<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), uint16x8_t, uint16x8_t, uint16x8_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_24 = requires(T0 a0, T1 a1, T2 a2) { native::bcax<A>(a0, a1, a2); };
  static_assert(available_sha_24<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(available_sha_24<native::neon, native::simd<std::uint32_t, 4, native::neon>, native::simd<std::uint32_t, 4, native::neon>, native::simd<std::uint32_t, 4, native::neon>>);
  static_assert(!available_sha_24<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), double, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(!available_sha_24<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), uint32x4_t, uint32x4_t, uint32x4_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_25 = requires(T0 a0, T1 a1, T2 a2) { native::bcax<A>(a0, a1, a2); };
  static_assert(available_sha_25<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(available_sha_25<native::neon, native::simd<std::uint64_t, 2, native::neon>, native::simd<std::uint64_t, 2, native::neon>, native::simd<std::uint64_t, 2, native::neon>>);
  static_assert(!available_sha_25<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), double, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(!available_sha_25<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), uint64x2_t, uint64x2_t, uint64x2_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_26 = requires(T0 a0, T1 a1, T2 a2) { native::bcax<A>(a0, a1, a2); };
  static_assert(available_sha_26<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), native::simd<std::int8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::int8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::int8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(available_sha_26<native::neon, native::simd<std::int8_t, 16, native::neon>, native::simd<std::int8_t, 16, native::neon>, native::simd<std::int8_t, 16, native::neon>>);
  static_assert(!available_sha_26<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), double, native::simd<std::int8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::int8_t, 16, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(!available_sha_26<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), int8x16_t, int8x16_t, int8x16_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_27 = requires(T0 a0, T1 a1, T2 a2) { native::bcax<A>(a0, a1, a2); };
  static_assert(available_sha_27<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), native::simd<std::int16_t, 8, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::int16_t, 8, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::int16_t, 8, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(available_sha_27<native::neon, native::simd<std::int16_t, 8, native::neon>, native::simd<std::int16_t, 8, native::neon>, native::simd<std::int16_t, 8, native::neon>>);
  static_assert(!available_sha_27<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), double, native::simd<std::int16_t, 8, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::int16_t, 8, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(!available_sha_27<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), int16x8_t, int16x8_t, int16x8_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_28 = requires(T0 a0, T1 a1, T2 a2) { native::bcax<A>(a0, a1, a2); };
  static_assert(available_sha_28<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), native::simd<std::int32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::int32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::int32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(available_sha_28<native::neon, native::simd<std::int32_t, 4, native::neon>, native::simd<std::int32_t, 4, native::neon>, native::simd<std::int32_t, 4, native::neon>>);
  static_assert(!available_sha_28<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), double, native::simd<std::int32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::int32_t, 4, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(!available_sha_28<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), int32x4_t, int32x4_t, int32x4_t>);
  template<native::isa<native::arm> A, class T0, class T1, class T2> concept available_sha_29 = requires(T0 a0, T1 a1, T2 a2) { native::bcax<A>(a0, a1, a2); };
  static_assert(available_sha_29<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), native::simd<std::int64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::int64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::int64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(available_sha_29<native::neon, native::simd<std::int64_t, 2, native::neon>, native::simd<std::int64_t, 2, native::neon>, native::simd<std::int64_t, 2, native::neon>>);
  static_assert(!available_sha_29<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), double, native::simd<std::int64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::int64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(!available_sha_29<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), int64x2_t, int64x2_t, int64x2_t>);
  template<native::isa<native::arm> A, class T0, class T1> concept available_sha_30 = requires(T0 a0, T1 a1) { native::rax1<A>(a0, a1); };
  static_assert(available_sha_30<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(available_sha_30<native::neon, native::simd<std::uint64_t, 2, native::neon>, native::simd<std::uint64_t, 2, native::neon>>);
  static_assert(!available_sha_30<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), double, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(!available_sha_30<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), uint64x2_t, uint64x2_t>);
  template<native::isa<native::arm> A, class T0, class T1> concept available_sha_31 = requires(T0 a0, T1 a1) { native::xar<A, 0>(a0, a1); };
  static_assert(available_sha_31<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(available_sha_31<native::neon, native::simd<std::uint64_t, 2, native::neon>, native::simd<std::uint64_t, 2, native::neon>>);
  static_assert(!available_sha_31<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), double, native::simd<std::uint64_t, 2, native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>>);
  static_assert(!available_sha_31<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), uint64x2_t, uint64x2_t>);
}
namespace crypto_fixture {
  constexpr auto aes = native::feature_closure(native::isa<native::arm>{native::arm_feature::aes});
  constexpr auto pmull = native::feature_closure(native::isa<native::arm>{native::arm_feature::pmull});
  constexpr auto sha1 = native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1});
  constexpr auto sha2 = native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2});
  constexpr auto sha512 = native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512});
  constexpr auto sha3 = native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3});

  inline std::uint64_t random_word(std::uint64_t &s) {
    s ^= s << 13; s ^= s >> 7; s ^= s << 17; return s;
  }
  template<class V> V random_vector(std::uint64_t &s) {
    using T = std::remove_cvref_t<decltype(V{}[0])>;
    V v{};
    for(unsigned i = 0; i != sizeof(V) / sizeof(T); ++i) v[i] = T(random_word(s));
    return v;
  }
  template<class V, class T, std::size_t N>
  bool equal(V actual, std::array<T,N> const &expected, char const *name) {
    for(unsigned i = 0; i != N; ++i) if(actual[i] != expected[i]) {
      std::printf("%s lane %u mismatch\n", name, i); return false;
    }
    return true;
  }
  // Baseline noinline oracles prevent crypto-leaf optimization from replacing
  // the scalar reference with the instruction under test.
  template<class T> __attribute__((noinline)) T choose(T a, T b, T c) { return (a & b) ^ (~a & c); }
  template<class T> __attribute__((noinline)) T majority(T a, T b, T c) { return (a & b) ^ (a & c) ^ (b & c); }
  __attribute__((noinline)) inline std::uint32_t sigma0(std::uint32_t a) { return std::rotr(a,7) ^ std::rotr(a,18) ^ (a >> 3); }
  __attribute__((noinline)) inline std::uint32_t sigma1(std::uint32_t a) { return std::rotr(a,17) ^ std::rotr(a,19) ^ (a >> 10); }
  __attribute__((noinline)) inline std::uint64_t sigma0(std::uint64_t a) { return std::rotr(a,1) ^ std::rotr(a,8) ^ (a >> 7); }
  __attribute__((noinline)) inline std::uint64_t sigma1(std::uint64_t a) { return std::rotr(a,19) ^ std::rotr(a,61) ^ (a >> 6); }
  __attribute__((noinline)) inline std::uint32_t sum0(std::uint32_t a) { return std::rotr(a,2) ^ std::rotr(a,13) ^ std::rotr(a,22); }
  __attribute__((noinline)) inline std::uint32_t sum1(std::uint32_t a) { return std::rotr(a,6) ^ std::rotr(a,11) ^ std::rotr(a,25); }
  __attribute__((noinline)) inline std::uint64_t sum0(std::uint64_t a) { return std::rotr(a,28) ^ std::rotr(a,34) ^ std::rotr(a,39); }
  __attribute__((noinline)) inline std::uint64_t sum1(std::uint64_t a) { return std::rotr(a,14) ^ std::rotr(a,18) ^ std::rotr(a,41); }

  // Independent GF(2^8) arithmetic generates the substitution and inverse maps.
  __attribute__((noinline)) inline std::uint8_t multiply_byte(std::uint8_t a, std::uint8_t b) {
    unsigned product = 0;
    for(unsigned i = 0; i != 8; ++i) {
      if(b & 1) product ^= a;
      a = std::uint8_t((unsigned(a) << 1) ^ ((a & 128) ? 0x11b : 0)); b >>= 1;
    }
    return std::uint8_t(product);
  }
  struct substitutions {
    std::array<std::uint8_t,256> forward{}, inverse{};
    substitutions() {
      for(unsigned i = 0; i != 256; ++i) {
        auto base = std::uint8_t(i), inv = std::uint8_t(1);
        for(unsigned power = 254; power; power >>= 1) {
          if(power & 1) inv = multiply_byte(inv, base);
          base = multiply_byte(base,base);
        }
        auto y = std::uint8_t(inv ^ std::rotl(inv,1) ^ std::rotl(inv,2) ^
          std::rotl(inv,3) ^ std::rotl(inv,4) ^ 0x63);
        forward[i] = y; inverse[y] = std::uint8_t(i);
      }
    }
  };
  __attribute__((target("aes"), noinline)) bool aes_checks(std::uint64_t seed) {
    substitutions table;
    if(table.forward[0] != 0x63 || table.forward[0x53] != 0xed) return false;
    for(unsigned test = 0; test != 512; ++test) {
      auto a = random_vector<uint8x16_t>(seed), k = random_vector<uint8x16_t>(seed);
      // Exercise every S-box input, including zero, independently of the RNG.
      a[0] = std::uint8_t(test); k[0] = 0;
      std::array<std::uint8_t,16> enc{}, dec{}, mix{}, invmix{};
      for(unsigned col = 0; col != 4; ++col) for(unsigned row = 0; row != 4; ++row) {
        auto e = 4*((col+row)%4)+row, d = 4*((col+4-row)%4)+row;
        enc[4*col+row] = table.forward[a[e]^k[e]];
        dec[4*col+row] = table.inverse[a[d]^k[d]];
        constexpr unsigned factors[4]{2,3,1,1}, inverse[4]{14,11,13,9};
        for(unsigned j=0; j!=4; ++j) {
          mix[4*col+row] ^= multiply_byte(a[4*col+j], std::uint8_t(factors[(j+4-row)%4]));
          invmix[4*col+row] ^= multiply_byte(a[4*col+j], std::uint8_t(inverse[(j+4-row)%4]));
        }
      }
      if(!equal(instruction_fixture::aese<aes>(a,k),enc,"AESE") || !equal(instruction_fixture::aesd<aes>(a,k),dec,"AESD") ||
         !equal(instruction_fixture::aesmc<aes>(a),mix,"AESMC") || !equal(instruction_fixture::aesimc<aes>(a),invmix,"AESIMC")) return false;
    }
    return true;
  }
  __attribute__((noinline)) inline poly128_t polynomial(std::uint64_t a, std::uint64_t b) {
    poly128_t result = 0;
    for(unsigned bit=0; bit!=64; ++bit) if((b >> bit) & 1) result ^= poly128_t(a) << bit;
    return result;
  }
  __attribute__((target("aes"), noinline)) bool polynomial_checks(std::uint64_t seed) {
    for(unsigned test=0; test!=512; ++test) {
      auto a=random_word(seed), b=random_word(seed);
      if(test < 64) a = std::uint64_t(1) << test;
      if(test == 64) a = b = ~std::uint64_t(0);
      if(test == 65) a = b = 0;
      if(instruction_fixture::pmull<pmull>(poly64_t(a),poly64_t(b)) != polynomial(a,b) ||
         instruction_fixture::pmull2<pmull>(poly64x2_t{poly64_t(random_word(seed)),poly64_t(a)},
                              poly64x2_t{poly64_t(random_word(seed)),poly64_t(b)}) != polynomial(a,b)) return false;
      // Clang's MSVC ABI gives uint8x16_t and poly8x16_t the same template
      // mangling. Reinterpret unsigned bytes to preserve the polynomial lane bits.
      auto x=vreinterpretq_p8_u8(random_vector<uint8x16_t>(seed));
      auto y=vreinterpretq_p8_u8(random_vector<uint8x16_t>(seed));
      std::array<poly16_t,8> lo{},hi{};
      for(unsigned i=0; i!=8; ++i) {
        lo[i]=poly16_t(polynomial(x[i],y[i])); hi[i]=poly16_t(polynomial(x[i+8],y[i+8]));
      }
      if(!equal(instruction_fixture::pmull<native::neon>(vget_low_p8(x),vget_low_p8(y)),lo,"PMULL8") ||
         !equal(instruction_fixture::pmull2<native::neon>(x,y),hi,"PMULL2-8")) return false;
    }
    return true;
  }
  __attribute__((target("sha2"), noinline)) bool sha2_checks(std::uint64_t seed) {
    for(unsigned test=0; test!=512; ++test) {
      auto x=random_vector<uint32x4_t>(seed), y=random_vector<uint32x4_t>(seed), z=random_vector<uint32x4_t>(seed);
      auto e=std::uint32_t(random_word(seed));
      for(unsigned mode=0; mode!=3; ++mode) {
        std::uint32_t a=x[0],b=x[1],c=x[2],d=x[3],h=e;
        for(unsigned i=0; i!=4; ++i) {
          auto f=mode==0 ? choose(b,c,d) : mode==1 ? (b^c^d) : majority(b,c,d);
          auto next=std::rotl(a,5)+f+h+z[i]; h=d; d=c; c=std::rotl(b,30); b=a; a=next;
        }
        auto actual=mode==0 ? instruction_fixture::sha1c<sha1>(x,e,z) : mode==1 ? instruction_fixture::sha1p<sha1>(x,e,z) : instruction_fixture::sha1m<sha1>(x,e,z);
        if(!equal(actual,std::array{a,b,c,d},"SHA1 rounds")) return false;
      }
      if(instruction_fixture::sha1h<sha1>(e) != std::rotr(e,2)) return false;
      std::array<std::uint32_t,4> su0{},su1{},s2560{},s2561{};
      for(unsigned i=0; i!=4; ++i) {
        su0[i]=x[i]^z[i]^(i<2 ? x[i+2] : y[i-2]);
        su1[i]=std::rotl(x[i]^(i<3 ? y[i+1] : 0),1);
        s2560[i]=x[i]+sigma0(i<3 ? x[i+1] : y[0]);
        s2561[i]=x[i]+(i<3 ? y[i+1] : z[0])+sigma1(i<2 ? z[i+2] : s2561[i-2]);
      }
      su1[3] ^= std::rotl(x[0]^y[1],2);
      if(!equal(instruction_fixture::sha1su0<sha1>(x,y,z),su0,"SHA1SU0") ||
         !equal(instruction_fixture::sha1su1<sha1>(x,y),su1,"SHA1SU1") ||
         !equal(instruction_fixture::sha256su0<sha2>(x,y),s2560,"SHA256SU0") ||
         !equal(instruction_fixture::sha256su1<sha2>(x,y,z),s2561,"SHA256SU1")) return false;
      std::uint32_t a=x[0],b=x[1],c=x[2],d=x[3],f=y[1],g=y[2],h=y[3]; e=y[0];
      for(unsigned i=0; i!=4; ++i) {
        auto t1=h+sum1(e)+choose(e,f,g)+z[i], t2=sum0(a)+majority(a,b,c);
        h=g;g=f;f=e;e=d+t1;d=c;c=b;b=a;a=t1+t2;
      }
      if(!equal(instruction_fixture::sha256h<sha2>(x,y,z),std::array{a,b,c,d},"SHA256H") ||
         !equal(instruction_fixture::sha256h2<sha2>(y,x,z),std::array{e,f,g,h},"SHA256H2")) return false;
    }
    return true;
  }
  __attribute__((noinline)) inline std::uint64_t xor3(std::uint64_t a, std::uint64_t b, std::uint64_t c) {
    return a ^ b ^ c;
  }
  __attribute__((noinline)) inline std::uint64_t clear_xor(std::uint64_t a, std::uint64_t b, std::uint64_t c) {
    return a ^ (b & ~c);
  }
  __attribute__((noinline)) inline std::uint64_t xor_rotate(std::uint64_t a, std::uint64_t b, unsigned r) {
    return std::rotr(a ^ b,r);
  }
  __attribute__((noinline)) inline std::uint64_t rotate_xor(std::uint64_t a, std::uint64_t b) {
    return a ^ std::rotl(b,1);
  }
  template<class V>
  __attribute__((target("sha3"))) bool logical_shapes(uint64x2_t x,uint64x2_t y,uint64x2_t z) {
    auto a=std::bit_cast<V>(x),b=std::bit_cast<V>(y),c=std::bit_cast<V>(z);
    auto e=std::bit_cast<uint64x2_t>(instruction_fixture::eor3<sha3>(a,b,c));
    auto bc=std::bit_cast<uint64x2_t>(instruction_fixture::bcax<sha3>(a,b,c));
    for(unsigned i=0;i!=2;++i) if(e[i]!=xor3(x[i],y[i],z[i]) || bc[i]!=clear_xor(x[i],y[i],z[i])) return false;
    return true;
  }
  template<unsigned R>
  __attribute__((target("sha3"))) bool rotate(uint64x2_t x,uint64x2_t y) {
    return equal(instruction_fixture::xar<sha3,R>(x,y),std::array{xor_rotate(x[0],y[0],R),xor_rotate(x[1],y[1],R)},"XAR");
  }
  template<unsigned... R>
  __attribute__((target("sha3"))) bool rotations(uint64x2_t x,uint64x2_t y,std::integer_sequence<unsigned,R...>) {
    return (rotate<R>(x,y) && ...);
  }
  __attribute__((target("sha3"), noinline)) bool sha3_checks(std::uint64_t seed) {
    for(unsigned test=0; test!=512; ++test) {
      auto x=random_vector<uint64x2_t>(seed),y=random_vector<uint64x2_t>(seed),z=random_vector<uint64x2_t>(seed);
      std::array<std::uint64_t,2> h{},h2{},su0{},su1{};
      // The SHA512 instructions update two dependent rounds in high/low order.
      h[1]=x[1]+sum1(z[1])+choose(z[1],y[0],y[1]);
      auto next=h[1]+z[0]; h[0]=x[0]+sum1(next)+choose(next,z[1],y[0]);
      h2[1]=x[1]+sum0(z[0])+majority(z[0],z[1],y[0]);
      h2[0]=x[0]+sum0(h2[1])+majority(h2[1],z[0],z[1]);
      su0={x[0]+sigma0(x[1]),x[1]+sigma0(y[0])};
      su1={x[0]+sigma1(y[0])+z[0],x[1]+sigma1(y[1])+z[1]};
      if(!equal(instruction_fixture::sha512h<sha512>(x,y,z),h,"SHA512H") ||
         !equal(instruction_fixture::sha512h2<sha512>(x,y,z),h2,"SHA512H2") ||
         !equal(instruction_fixture::sha512su0<sha512>(x,y),su0,"SHA512SU0") ||
         !equal(instruction_fixture::sha512su1<sha512>(x,y,z),su1,"SHA512SU1")) return false;
      if(!logical_shapes<uint8x16_t>(x,y,z) || !logical_shapes<uint16x8_t>(x,y,z) ||
         !logical_shapes<uint32x4_t>(x,y,z) || !logical_shapes<uint64x2_t>(x,y,z) ||
         !logical_shapes<int8x16_t>(x,y,z) || !logical_shapes<int16x8_t>(x,y,z) ||
         !logical_shapes<int32x4_t>(x,y,z) || !logical_shapes<int64x2_t>(x,y,z)) return false;
      if(!equal(instruction_fixture::rax1<sha3>(x,y),std::array{rotate_xor(x[0],y[0]),rotate_xor(x[1],y[1])},"RAX1") ||
         !rotations(x,y,std::make_integer_sequence<unsigned,64>{})) return false;
    }
    return true;
  }
  constexpr native::arm_capabilities capabilities(native::isa<native::arm> features) {
    native::arm_capabilities cpu{};
    for(auto f : {native::arm_feature::neon,native::arm_feature::aes,native::arm_feature::pmull,
                  native::arm_feature::sha1,native::arm_feature::sha2,
                  native::arm_feature::sha512,native::arm_feature::sha3}) {
      cpu.present.set(f,features.has(f)); cpu.observed.set(f,features.has(f));
    }
    return cpu;
  }
  consteval bool admission_contract() {
    for(auto f : {native::arm_feature::aes,native::arm_feature::pmull,native::arm_feature::sha1,
                  native::arm_feature::sha2,native::arm_feature::sha512,native::arm_feature::sha3}) {
      auto cpu=capabilities(native::feature_closure(native::isa<native::arm>{f}));
      if(!native::classify_isa(cpu,native::isa<native::arm>{f}).admitted()) return false;
      cpu.observed.set(f,false);
      if(native::classify_isa(cpu,native::isa<native::arm>{f}).admitted()) return false;
    }
    for(auto target : {"aes","sha2","sha3"}) {
      auto features=native::target_features<native::arm>(target);
      for(auto f : {native::arm_feature::aes,native::arm_feature::pmull,native::arm_feature::sha1,
                    native::arm_feature::sha2,native::arm_feature::sha512,native::arm_feature::sha3}) {
        if(!features.has(f)) continue;
        auto cpu=capabilities(features);
        cpu.present.set(f,false);
        if(native::classify_isa(cpu,features).admitted()) return false;
      }
    }
    return true;
  }
  static_assert(admission_contract());
  inline std::uint64_t fpcr() { std::uint64_t r; asm volatile("mrs %0, fpcr" : "=r"(r)); return r; }
  inline std::uint64_t fpsr() { std::uint64_t r; asm volatile("mrs %0, fpsr" : "=r"(r)); return r; }
  inline void fpsr(std::uint64_t value) { asm volatile("msr fpsr, %0" : : "r"(value) : "memory"); }
  inline int run(unsigned seed) {
    auto cpu=native::observe_arm_capabilities();
    for(auto target : {"aes","sha2","sha3"}) {
      auto result=native::classify_isa(cpu,native::target_features<native::arm>(target));
      if(!result.admitted()) { std::printf("Skipped %s: %s\n",target,result.reason()); return 77; }
    }
    auto control=fpcr(),saved_status=fpsr();
    // Retain both a sticky FP exception and the saturating-integer QC flag.
    auto status=saved_status | 1u | (1u << 27);
    fpsr(status);
    bool ok=aes_checks(seed) && polynomial_checks(seed) && sha2_checks(seed) && sha3_checks(seed);
    ok=ok && control==fpcr() && status==fpsr();
    fpsr(saved_status);
    if(!ok) std::puts("ARM crypto reference or FP environment check failed");
    return ok ? 0 : 1;
  }
}
