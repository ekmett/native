// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/neon.h>
import native.arm.neon;
constexpr auto architecture = native::feature_closure(native::arm_feature::neon);
template <class T, std::size_t N> using V = native::simd<T, N, architecture>;

extern "C" __attribute__((noinline, used, target("neon"))) int8x8_t raw_sqadd_8_8(int8x8_t a,
                                                                                  int8x8_t b) {
  return native::detail::arm_neon::sqadd(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x8_t native_sqadd_8_8(int8x8_t a,
                                                                                     int8x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int8_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int8_t, 8>>(b);
  return native::detail::arm_neon::to_register<int8x8_t>(native::sqadd(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x16_t raw_sqadd_8_16(int8x16_t a,
                                                                                    int8x16_t b) {
  return native::detail::arm_neon::sqadd(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x16_t
native_sqadd_8_16(int8x16_t a, int8x16_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int8_t, 16>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int8_t, 16>>(b);
  return native::detail::arm_neon::to_register<int8x16_t>(native::sqadd(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x4_t raw_sqadd_16_4(int16x4_t a,
                                                                                    int16x4_t b) {
  return native::detail::arm_neon::sqadd(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x4_t
native_sqadd_16_4(int16x4_t a, int16x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int16_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int16_t, 4>>(b);
  return native::detail::arm_neon::to_register<int16x4_t>(native::sqadd(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x8_t raw_sqadd_16_8(int16x8_t a,
                                                                                    int16x8_t b) {
  return native::detail::arm_neon::sqadd(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x8_t
native_sqadd_16_8(int16x8_t a, int16x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int16_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int16_t, 8>>(b);
  return native::detail::arm_neon::to_register<int16x8_t>(native::sqadd(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x2_t raw_sqadd_32_2(int32x2_t a,
                                                                                    int32x2_t b) {
  return native::detail::arm_neon::sqadd(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x2_t
native_sqadd_32_2(int32x2_t a, int32x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int32_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int32_t, 2>>(b);
  return native::detail::arm_neon::to_register<int32x2_t>(native::sqadd(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x4_t raw_sqadd_32_4(int32x4_t a,
                                                                                    int32x4_t b) {
  return native::detail::arm_neon::sqadd(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x4_t
native_sqadd_32_4(int32x4_t a, int32x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int32_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int32_t, 4>>(b);
  return native::detail::arm_neon::to_register<int32x4_t>(native::sqadd(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int64x1_t raw_sqadd_64_1(int64x1_t a,
                                                                                    int64x1_t b) {
  return native::detail::arm_neon::sqadd(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int64x1_t
native_sqadd_64_1(int64x1_t a, int64x1_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int64_t, 1>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int64_t, 1>>(b);
  return native::detail::arm_neon::to_register<int64x1_t>(native::sqadd(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int64x2_t raw_sqadd_64_2(int64x2_t a,
                                                                                    int64x2_t b) {
  return native::detail::arm_neon::sqadd(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int64x2_t
native_sqadd_64_2(int64x2_t a, int64x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int64_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int64_t, 2>>(b);
  return native::detail::arm_neon::to_register<int64x2_t>(native::sqadd(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x8_t raw_uqadd_8_8(uint8x8_t a,
                                                                                   uint8x8_t b) {
  return native::detail::arm_neon::uqadd(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x8_t native_uqadd_8_8(uint8x8_t a,
                                                                                      uint8x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint8_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::uint8_t, 8>>(b);
  return native::detail::arm_neon::to_register<uint8x8_t>(native::uqadd(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x16_t raw_uqadd_8_16(uint8x16_t a,
                                                                                     uint8x16_t b) {
  return native::detail::arm_neon::uqadd(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x16_t
native_uqadd_8_16(uint8x16_t a, uint8x16_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint8_t, 16>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::uint8_t, 16>>(b);
  return native::detail::arm_neon::to_register<uint8x16_t>(native::uqadd(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x4_t raw_uqadd_16_4(uint16x4_t a,
                                                                                     uint16x4_t b) {
  return native::detail::arm_neon::uqadd(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x4_t
native_uqadd_16_4(uint16x4_t a, uint16x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint16_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::uint16_t, 4>>(b);
  return native::detail::arm_neon::to_register<uint16x4_t>(native::uqadd(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x8_t raw_uqadd_16_8(uint16x8_t a,
                                                                                     uint16x8_t b) {
  return native::detail::arm_neon::uqadd(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x8_t
native_uqadd_16_8(uint16x8_t a, uint16x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint16_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::uint16_t, 8>>(b);
  return native::detail::arm_neon::to_register<uint16x8_t>(native::uqadd(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x2_t raw_uqadd_32_2(uint32x2_t a,
                                                                                     uint32x2_t b) {
  return native::detail::arm_neon::uqadd(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x2_t
native_uqadd_32_2(uint32x2_t a, uint32x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint32_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::uint32_t, 2>>(b);
  return native::detail::arm_neon::to_register<uint32x2_t>(native::uqadd(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x4_t raw_uqadd_32_4(uint32x4_t a,
                                                                                     uint32x4_t b) {
  return native::detail::arm_neon::uqadd(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x4_t
native_uqadd_32_4(uint32x4_t a, uint32x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint32_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::uint32_t, 4>>(b);
  return native::detail::arm_neon::to_register<uint32x4_t>(native::uqadd(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint64x1_t raw_uqadd_64_1(uint64x1_t a,
                                                                                     uint64x1_t b) {
  return native::detail::arm_neon::uqadd(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint64x1_t
native_uqadd_64_1(uint64x1_t a, uint64x1_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint64_t, 1>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::uint64_t, 1>>(b);
  return native::detail::arm_neon::to_register<uint64x1_t>(native::uqadd(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint64x2_t raw_uqadd_64_2(uint64x2_t a,
                                                                                     uint64x2_t b) {
  return native::detail::arm_neon::uqadd(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint64x2_t
native_uqadd_64_2(uint64x2_t a, uint64x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint64_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::uint64_t, 2>>(b);
  return native::detail::arm_neon::to_register<uint64x2_t>(native::uqadd(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x8_t raw_sqsub_8_8(int8x8_t a,
                                                                                  int8x8_t b) {
  return native::detail::arm_neon::sqsub(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x8_t native_sqsub_8_8(int8x8_t a,
                                                                                     int8x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int8_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int8_t, 8>>(b);
  return native::detail::arm_neon::to_register<int8x8_t>(native::sqsub(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x16_t raw_sqsub_8_16(int8x16_t a,
                                                                                    int8x16_t b) {
  return native::detail::arm_neon::sqsub(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x16_t
native_sqsub_8_16(int8x16_t a, int8x16_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int8_t, 16>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int8_t, 16>>(b);
  return native::detail::arm_neon::to_register<int8x16_t>(native::sqsub(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x4_t raw_sqsub_16_4(int16x4_t a,
                                                                                    int16x4_t b) {
  return native::detail::arm_neon::sqsub(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x4_t
native_sqsub_16_4(int16x4_t a, int16x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int16_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int16_t, 4>>(b);
  return native::detail::arm_neon::to_register<int16x4_t>(native::sqsub(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x8_t raw_sqsub_16_8(int16x8_t a,
                                                                                    int16x8_t b) {
  return native::detail::arm_neon::sqsub(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x8_t
native_sqsub_16_8(int16x8_t a, int16x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int16_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int16_t, 8>>(b);
  return native::detail::arm_neon::to_register<int16x8_t>(native::sqsub(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x2_t raw_sqsub_32_2(int32x2_t a,
                                                                                    int32x2_t b) {
  return native::detail::arm_neon::sqsub(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x2_t
native_sqsub_32_2(int32x2_t a, int32x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int32_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int32_t, 2>>(b);
  return native::detail::arm_neon::to_register<int32x2_t>(native::sqsub(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x4_t raw_sqsub_32_4(int32x4_t a,
                                                                                    int32x4_t b) {
  return native::detail::arm_neon::sqsub(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x4_t
native_sqsub_32_4(int32x4_t a, int32x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int32_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int32_t, 4>>(b);
  return native::detail::arm_neon::to_register<int32x4_t>(native::sqsub(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int64x1_t raw_sqsub_64_1(int64x1_t a,
                                                                                    int64x1_t b) {
  return native::detail::arm_neon::sqsub(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int64x1_t
native_sqsub_64_1(int64x1_t a, int64x1_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int64_t, 1>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int64_t, 1>>(b);
  return native::detail::arm_neon::to_register<int64x1_t>(native::sqsub(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int64x2_t raw_sqsub_64_2(int64x2_t a,
                                                                                    int64x2_t b) {
  return native::detail::arm_neon::sqsub(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int64x2_t
native_sqsub_64_2(int64x2_t a, int64x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int64_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int64_t, 2>>(b);
  return native::detail::arm_neon::to_register<int64x2_t>(native::sqsub(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x8_t raw_uqsub_8_8(uint8x8_t a,
                                                                                   uint8x8_t b) {
  return native::detail::arm_neon::uqsub(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x8_t native_uqsub_8_8(uint8x8_t a,
                                                                                      uint8x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint8_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::uint8_t, 8>>(b);
  return native::detail::arm_neon::to_register<uint8x8_t>(native::uqsub(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x16_t raw_uqsub_8_16(uint8x16_t a,
                                                                                     uint8x16_t b) {
  return native::detail::arm_neon::uqsub(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x16_t
native_uqsub_8_16(uint8x16_t a, uint8x16_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint8_t, 16>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::uint8_t, 16>>(b);
  return native::detail::arm_neon::to_register<uint8x16_t>(native::uqsub(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x4_t raw_uqsub_16_4(uint16x4_t a,
                                                                                     uint16x4_t b) {
  return native::detail::arm_neon::uqsub(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x4_t
native_uqsub_16_4(uint16x4_t a, uint16x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint16_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::uint16_t, 4>>(b);
  return native::detail::arm_neon::to_register<uint16x4_t>(native::uqsub(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x8_t raw_uqsub_16_8(uint16x8_t a,
                                                                                     uint16x8_t b) {
  return native::detail::arm_neon::uqsub(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x8_t
native_uqsub_16_8(uint16x8_t a, uint16x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint16_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::uint16_t, 8>>(b);
  return native::detail::arm_neon::to_register<uint16x8_t>(native::uqsub(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x2_t raw_uqsub_32_2(uint32x2_t a,
                                                                                     uint32x2_t b) {
  return native::detail::arm_neon::uqsub(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x2_t
native_uqsub_32_2(uint32x2_t a, uint32x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint32_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::uint32_t, 2>>(b);
  return native::detail::arm_neon::to_register<uint32x2_t>(native::uqsub(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x4_t raw_uqsub_32_4(uint32x4_t a,
                                                                                     uint32x4_t b) {
  return native::detail::arm_neon::uqsub(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x4_t
native_uqsub_32_4(uint32x4_t a, uint32x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint32_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::uint32_t, 4>>(b);
  return native::detail::arm_neon::to_register<uint32x4_t>(native::uqsub(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint64x1_t raw_uqsub_64_1(uint64x1_t a,
                                                                                     uint64x1_t b) {
  return native::detail::arm_neon::uqsub(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint64x1_t
native_uqsub_64_1(uint64x1_t a, uint64x1_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint64_t, 1>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::uint64_t, 1>>(b);
  return native::detail::arm_neon::to_register<uint64x1_t>(native::uqsub(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint64x2_t raw_uqsub_64_2(uint64x2_t a,
                                                                                     uint64x2_t b) {
  return native::detail::arm_neon::uqsub(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint64x2_t
native_uqsub_64_2(uint64x2_t a, uint64x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint64_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::uint64_t, 2>>(b);
  return native::detail::arm_neon::to_register<uint64x2_t>(native::uqsub(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x4_t raw_sqdmulh_16_4(int16x4_t a,
                                                                                      int16x4_t b) {
  return native::detail::arm_neon::sqdmulh(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x4_t
native_sqdmulh_16_4(int16x4_t a, int16x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int16_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int16_t, 4>>(b);
  return native::detail::arm_neon::to_register<int16x4_t>(native::sqdmulh(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x8_t raw_sqdmulh_16_8(int16x8_t a,
                                                                                      int16x8_t b) {
  return native::detail::arm_neon::sqdmulh(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x8_t
native_sqdmulh_16_8(int16x8_t a, int16x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int16_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int16_t, 8>>(b);
  return native::detail::arm_neon::to_register<int16x8_t>(native::sqdmulh(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x2_t raw_sqdmulh_32_2(int32x2_t a,
                                                                                      int32x2_t b) {
  return native::detail::arm_neon::sqdmulh(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x2_t
native_sqdmulh_32_2(int32x2_t a, int32x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int32_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int32_t, 2>>(b);
  return native::detail::arm_neon::to_register<int32x2_t>(native::sqdmulh(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x4_t raw_sqdmulh_32_4(int32x4_t a,
                                                                                      int32x4_t b) {
  return native::detail::arm_neon::sqdmulh(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x4_t
native_sqdmulh_32_4(int32x4_t a, int32x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int32_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int32_t, 4>>(b);
  return native::detail::arm_neon::to_register<int32x4_t>(native::sqdmulh(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x4_t
raw_sqrdmulh_16_4(int16x4_t a, int16x4_t b) {
  return native::detail::arm_neon::sqrdmulh(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x4_t
native_sqrdmulh_16_4(int16x4_t a, int16x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int16_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int16_t, 4>>(b);
  return native::detail::arm_neon::to_register<int16x4_t>(native::sqrdmulh(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x8_t
raw_sqrdmulh_16_8(int16x8_t a, int16x8_t b) {
  return native::detail::arm_neon::sqrdmulh(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x8_t
native_sqrdmulh_16_8(int16x8_t a, int16x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int16_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int16_t, 8>>(b);
  return native::detail::arm_neon::to_register<int16x8_t>(native::sqrdmulh(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x2_t
raw_sqrdmulh_32_2(int32x2_t a, int32x2_t b) {
  return native::detail::arm_neon::sqrdmulh(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x2_t
native_sqrdmulh_32_2(int32x2_t a, int32x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int32_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int32_t, 2>>(b);
  return native::detail::arm_neon::to_register<int32x2_t>(native::sqrdmulh(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x4_t
raw_sqrdmulh_32_4(int32x4_t a, int32x4_t b) {
  return native::detail::arm_neon::sqrdmulh(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x4_t
native_sqrdmulh_32_4(int32x4_t a, int32x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int32_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int32_t, 4>>(b);
  return native::detail::arm_neon::to_register<int32x4_t>(native::sqrdmulh(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x8_t raw_sshl_8_8(int8x8_t a,
                                                                                 int8x8_t b) {
  return native::detail::arm_neon::sshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x8_t native_sshl_8_8(int8x8_t a,
                                                                                    int8x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int8_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int8_t, 8>>(b);
  return native::detail::arm_neon::to_register<int8x8_t>(native::sshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x16_t raw_sshl_8_16(int8x16_t a,
                                                                                   int8x16_t b) {
  return native::detail::arm_neon::sshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x16_t native_sshl_8_16(int8x16_t a,
                                                                                      int8x16_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int8_t, 16>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int8_t, 16>>(b);
  return native::detail::arm_neon::to_register<int8x16_t>(native::sshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x4_t raw_sshl_16_4(int16x4_t a,
                                                                                   int16x4_t b) {
  return native::detail::arm_neon::sshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x4_t native_sshl_16_4(int16x4_t a,
                                                                                      int16x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int16_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int16_t, 4>>(b);
  return native::detail::arm_neon::to_register<int16x4_t>(native::sshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x8_t raw_sshl_16_8(int16x8_t a,
                                                                                   int16x8_t b) {
  return native::detail::arm_neon::sshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x8_t native_sshl_16_8(int16x8_t a,
                                                                                      int16x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int16_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int16_t, 8>>(b);
  return native::detail::arm_neon::to_register<int16x8_t>(native::sshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x2_t raw_sshl_32_2(int32x2_t a,
                                                                                   int32x2_t b) {
  return native::detail::arm_neon::sshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x2_t native_sshl_32_2(int32x2_t a,
                                                                                      int32x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int32_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int32_t, 2>>(b);
  return native::detail::arm_neon::to_register<int32x2_t>(native::sshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x4_t raw_sshl_32_4(int32x4_t a,
                                                                                   int32x4_t b) {
  return native::detail::arm_neon::sshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x4_t native_sshl_32_4(int32x4_t a,
                                                                                      int32x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int32_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int32_t, 4>>(b);
  return native::detail::arm_neon::to_register<int32x4_t>(native::sshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int64x1_t raw_sshl_64_1(int64x1_t a,
                                                                                   int64x1_t b) {
  return native::detail::arm_neon::sshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int64x1_t native_sshl_64_1(int64x1_t a,
                                                                                      int64x1_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int64_t, 1>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int64_t, 1>>(b);
  return native::detail::arm_neon::to_register<int64x1_t>(native::sshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int64x2_t raw_sshl_64_2(int64x2_t a,
                                                                                   int64x2_t b) {
  return native::detail::arm_neon::sshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int64x2_t native_sshl_64_2(int64x2_t a,
                                                                                      int64x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int64_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int64_t, 2>>(b);
  return native::detail::arm_neon::to_register<int64x2_t>(native::sshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x8_t raw_srshl_8_8(int8x8_t a,
                                                                                  int8x8_t b) {
  return native::detail::arm_neon::srshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x8_t native_srshl_8_8(int8x8_t a,
                                                                                     int8x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int8_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int8_t, 8>>(b);
  return native::detail::arm_neon::to_register<int8x8_t>(native::srshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x16_t raw_srshl_8_16(int8x16_t a,
                                                                                    int8x16_t b) {
  return native::detail::arm_neon::srshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x16_t
native_srshl_8_16(int8x16_t a, int8x16_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int8_t, 16>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int8_t, 16>>(b);
  return native::detail::arm_neon::to_register<int8x16_t>(native::srshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x4_t raw_srshl_16_4(int16x4_t a,
                                                                                    int16x4_t b) {
  return native::detail::arm_neon::srshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x4_t
native_srshl_16_4(int16x4_t a, int16x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int16_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int16_t, 4>>(b);
  return native::detail::arm_neon::to_register<int16x4_t>(native::srshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x8_t raw_srshl_16_8(int16x8_t a,
                                                                                    int16x8_t b) {
  return native::detail::arm_neon::srshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x8_t
native_srshl_16_8(int16x8_t a, int16x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int16_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int16_t, 8>>(b);
  return native::detail::arm_neon::to_register<int16x8_t>(native::srshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x2_t raw_srshl_32_2(int32x2_t a,
                                                                                    int32x2_t b) {
  return native::detail::arm_neon::srshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x2_t
native_srshl_32_2(int32x2_t a, int32x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int32_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int32_t, 2>>(b);
  return native::detail::arm_neon::to_register<int32x2_t>(native::srshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x4_t raw_srshl_32_4(int32x4_t a,
                                                                                    int32x4_t b) {
  return native::detail::arm_neon::srshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x4_t
native_srshl_32_4(int32x4_t a, int32x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int32_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int32_t, 4>>(b);
  return native::detail::arm_neon::to_register<int32x4_t>(native::srshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int64x1_t raw_srshl_64_1(int64x1_t a,
                                                                                    int64x1_t b) {
  return native::detail::arm_neon::srshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int64x1_t
native_srshl_64_1(int64x1_t a, int64x1_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int64_t, 1>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int64_t, 1>>(b);
  return native::detail::arm_neon::to_register<int64x1_t>(native::srshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int64x2_t raw_srshl_64_2(int64x2_t a,
                                                                                    int64x2_t b) {
  return native::detail::arm_neon::srshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int64x2_t
native_srshl_64_2(int64x2_t a, int64x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int64_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int64_t, 2>>(b);
  return native::detail::arm_neon::to_register<int64x2_t>(native::srshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x8_t raw_sqshl_8_8(int8x8_t a,
                                                                                  int8x8_t b) {
  return native::detail::arm_neon::sqshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x8_t native_sqshl_8_8(int8x8_t a,
                                                                                     int8x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int8_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int8_t, 8>>(b);
  return native::detail::arm_neon::to_register<int8x8_t>(native::sqshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x16_t raw_sqshl_8_16(int8x16_t a,
                                                                                    int8x16_t b) {
  return native::detail::arm_neon::sqshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x16_t
native_sqshl_8_16(int8x16_t a, int8x16_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int8_t, 16>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int8_t, 16>>(b);
  return native::detail::arm_neon::to_register<int8x16_t>(native::sqshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x4_t raw_sqshl_16_4(int16x4_t a,
                                                                                    int16x4_t b) {
  return native::detail::arm_neon::sqshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x4_t
native_sqshl_16_4(int16x4_t a, int16x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int16_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int16_t, 4>>(b);
  return native::detail::arm_neon::to_register<int16x4_t>(native::sqshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x8_t raw_sqshl_16_8(int16x8_t a,
                                                                                    int16x8_t b) {
  return native::detail::arm_neon::sqshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x8_t
native_sqshl_16_8(int16x8_t a, int16x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int16_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int16_t, 8>>(b);
  return native::detail::arm_neon::to_register<int16x8_t>(native::sqshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x2_t raw_sqshl_32_2(int32x2_t a,
                                                                                    int32x2_t b) {
  return native::detail::arm_neon::sqshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x2_t
native_sqshl_32_2(int32x2_t a, int32x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int32_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int32_t, 2>>(b);
  return native::detail::arm_neon::to_register<int32x2_t>(native::sqshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x4_t raw_sqshl_32_4(int32x4_t a,
                                                                                    int32x4_t b) {
  return native::detail::arm_neon::sqshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x4_t
native_sqshl_32_4(int32x4_t a, int32x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int32_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int32_t, 4>>(b);
  return native::detail::arm_neon::to_register<int32x4_t>(native::sqshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int64x1_t raw_sqshl_64_1(int64x1_t a,
                                                                                    int64x1_t b) {
  return native::detail::arm_neon::sqshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int64x1_t
native_sqshl_64_1(int64x1_t a, int64x1_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int64_t, 1>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int64_t, 1>>(b);
  return native::detail::arm_neon::to_register<int64x1_t>(native::sqshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int64x2_t raw_sqshl_64_2(int64x2_t a,
                                                                                    int64x2_t b) {
  return native::detail::arm_neon::sqshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int64x2_t
native_sqshl_64_2(int64x2_t a, int64x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int64_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int64_t, 2>>(b);
  return native::detail::arm_neon::to_register<int64x2_t>(native::sqshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x8_t raw_sqrshl_8_8(int8x8_t a,
                                                                                   int8x8_t b) {
  return native::detail::arm_neon::sqrshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x8_t native_sqrshl_8_8(int8x8_t a,
                                                                                      int8x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int8_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int8_t, 8>>(b);
  return native::detail::arm_neon::to_register<int8x8_t>(native::sqrshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x16_t raw_sqrshl_8_16(int8x16_t a,
                                                                                     int8x16_t b) {
  return native::detail::arm_neon::sqrshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x16_t
native_sqrshl_8_16(int8x16_t a, int8x16_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int8_t, 16>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int8_t, 16>>(b);
  return native::detail::arm_neon::to_register<int8x16_t>(native::sqrshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x4_t raw_sqrshl_16_4(int16x4_t a,
                                                                                     int16x4_t b) {
  return native::detail::arm_neon::sqrshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x4_t
native_sqrshl_16_4(int16x4_t a, int16x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int16_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int16_t, 4>>(b);
  return native::detail::arm_neon::to_register<int16x4_t>(native::sqrshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x8_t raw_sqrshl_16_8(int16x8_t a,
                                                                                     int16x8_t b) {
  return native::detail::arm_neon::sqrshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x8_t
native_sqrshl_16_8(int16x8_t a, int16x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int16_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int16_t, 8>>(b);
  return native::detail::arm_neon::to_register<int16x8_t>(native::sqrshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x2_t raw_sqrshl_32_2(int32x2_t a,
                                                                                     int32x2_t b) {
  return native::detail::arm_neon::sqrshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x2_t
native_sqrshl_32_2(int32x2_t a, int32x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int32_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int32_t, 2>>(b);
  return native::detail::arm_neon::to_register<int32x2_t>(native::sqrshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x4_t raw_sqrshl_32_4(int32x4_t a,
                                                                                     int32x4_t b) {
  return native::detail::arm_neon::sqrshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x4_t
native_sqrshl_32_4(int32x4_t a, int32x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int32_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int32_t, 4>>(b);
  return native::detail::arm_neon::to_register<int32x4_t>(native::sqrshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int64x1_t raw_sqrshl_64_1(int64x1_t a,
                                                                                     int64x1_t b) {
  return native::detail::arm_neon::sqrshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int64x1_t
native_sqrshl_64_1(int64x1_t a, int64x1_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int64_t, 1>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int64_t, 1>>(b);
  return native::detail::arm_neon::to_register<int64x1_t>(native::sqrshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int64x2_t raw_sqrshl_64_2(int64x2_t a,
                                                                                     int64x2_t b) {
  return native::detail::arm_neon::sqrshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) int64x2_t
native_sqrshl_64_2(int64x2_t a, int64x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::int64_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int64_t, 2>>(b);
  return native::detail::arm_neon::to_register<int64x2_t>(native::sqrshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x8_t raw_ushl_8_8(uint8x8_t a,
                                                                                  int8x8_t b) {
  return native::detail::arm_neon::ushl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x8_t native_ushl_8_8(uint8x8_t a,
                                                                                     int8x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint8_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int8_t, 8>>(b);
  return native::detail::arm_neon::to_register<uint8x8_t>(native::ushl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x16_t raw_ushl_8_16(uint8x16_t a,
                                                                                    int8x16_t b) {
  return native::detail::arm_neon::ushl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x16_t
native_ushl_8_16(uint8x16_t a, int8x16_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint8_t, 16>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int8_t, 16>>(b);
  return native::detail::arm_neon::to_register<uint8x16_t>(native::ushl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x4_t raw_ushl_16_4(uint16x4_t a,
                                                                                    int16x4_t b) {
  return native::detail::arm_neon::ushl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x4_t
native_ushl_16_4(uint16x4_t a, int16x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint16_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int16_t, 4>>(b);
  return native::detail::arm_neon::to_register<uint16x4_t>(native::ushl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x8_t raw_ushl_16_8(uint16x8_t a,
                                                                                    int16x8_t b) {
  return native::detail::arm_neon::ushl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x8_t
native_ushl_16_8(uint16x8_t a, int16x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint16_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int16_t, 8>>(b);
  return native::detail::arm_neon::to_register<uint16x8_t>(native::ushl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x2_t raw_ushl_32_2(uint32x2_t a,
                                                                                    int32x2_t b) {
  return native::detail::arm_neon::ushl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x2_t
native_ushl_32_2(uint32x2_t a, int32x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint32_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int32_t, 2>>(b);
  return native::detail::arm_neon::to_register<uint32x2_t>(native::ushl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x4_t raw_ushl_32_4(uint32x4_t a,
                                                                                    int32x4_t b) {
  return native::detail::arm_neon::ushl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x4_t
native_ushl_32_4(uint32x4_t a, int32x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint32_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int32_t, 4>>(b);
  return native::detail::arm_neon::to_register<uint32x4_t>(native::ushl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint64x1_t raw_ushl_64_1(uint64x1_t a,
                                                                                    int64x1_t b) {
  return native::detail::arm_neon::ushl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint64x1_t
native_ushl_64_1(uint64x1_t a, int64x1_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint64_t, 1>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int64_t, 1>>(b);
  return native::detail::arm_neon::to_register<uint64x1_t>(native::ushl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint64x2_t raw_ushl_64_2(uint64x2_t a,
                                                                                    int64x2_t b) {
  return native::detail::arm_neon::ushl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint64x2_t
native_ushl_64_2(uint64x2_t a, int64x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint64_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int64_t, 2>>(b);
  return native::detail::arm_neon::to_register<uint64x2_t>(native::ushl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x8_t raw_urshl_8_8(uint8x8_t a,
                                                                                   int8x8_t b) {
  return native::detail::arm_neon::urshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x8_t native_urshl_8_8(uint8x8_t a,
                                                                                      int8x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint8_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int8_t, 8>>(b);
  return native::detail::arm_neon::to_register<uint8x8_t>(native::urshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x16_t raw_urshl_8_16(uint8x16_t a,
                                                                                     int8x16_t b) {
  return native::detail::arm_neon::urshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x16_t
native_urshl_8_16(uint8x16_t a, int8x16_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint8_t, 16>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int8_t, 16>>(b);
  return native::detail::arm_neon::to_register<uint8x16_t>(native::urshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x4_t raw_urshl_16_4(uint16x4_t a,
                                                                                     int16x4_t b) {
  return native::detail::arm_neon::urshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x4_t
native_urshl_16_4(uint16x4_t a, int16x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint16_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int16_t, 4>>(b);
  return native::detail::arm_neon::to_register<uint16x4_t>(native::urshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x8_t raw_urshl_16_8(uint16x8_t a,
                                                                                     int16x8_t b) {
  return native::detail::arm_neon::urshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x8_t
native_urshl_16_8(uint16x8_t a, int16x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint16_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int16_t, 8>>(b);
  return native::detail::arm_neon::to_register<uint16x8_t>(native::urshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x2_t raw_urshl_32_2(uint32x2_t a,
                                                                                     int32x2_t b) {
  return native::detail::arm_neon::urshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x2_t
native_urshl_32_2(uint32x2_t a, int32x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint32_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int32_t, 2>>(b);
  return native::detail::arm_neon::to_register<uint32x2_t>(native::urshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x4_t raw_urshl_32_4(uint32x4_t a,
                                                                                     int32x4_t b) {
  return native::detail::arm_neon::urshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x4_t
native_urshl_32_4(uint32x4_t a, int32x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint32_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int32_t, 4>>(b);
  return native::detail::arm_neon::to_register<uint32x4_t>(native::urshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint64x1_t raw_urshl_64_1(uint64x1_t a,
                                                                                     int64x1_t b) {
  return native::detail::arm_neon::urshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint64x1_t
native_urshl_64_1(uint64x1_t a, int64x1_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint64_t, 1>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int64_t, 1>>(b);
  return native::detail::arm_neon::to_register<uint64x1_t>(native::urshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint64x2_t raw_urshl_64_2(uint64x2_t a,
                                                                                     int64x2_t b) {
  return native::detail::arm_neon::urshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint64x2_t
native_urshl_64_2(uint64x2_t a, int64x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint64_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int64_t, 2>>(b);
  return native::detail::arm_neon::to_register<uint64x2_t>(native::urshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x8_t raw_uqshl_8_8(uint8x8_t a,
                                                                                   int8x8_t b) {
  return native::detail::arm_neon::uqshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x8_t native_uqshl_8_8(uint8x8_t a,
                                                                                      int8x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint8_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int8_t, 8>>(b);
  return native::detail::arm_neon::to_register<uint8x8_t>(native::uqshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x16_t raw_uqshl_8_16(uint8x16_t a,
                                                                                     int8x16_t b) {
  return native::detail::arm_neon::uqshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x16_t
native_uqshl_8_16(uint8x16_t a, int8x16_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint8_t, 16>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int8_t, 16>>(b);
  return native::detail::arm_neon::to_register<uint8x16_t>(native::uqshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x4_t raw_uqshl_16_4(uint16x4_t a,
                                                                                     int16x4_t b) {
  return native::detail::arm_neon::uqshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x4_t
native_uqshl_16_4(uint16x4_t a, int16x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint16_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int16_t, 4>>(b);
  return native::detail::arm_neon::to_register<uint16x4_t>(native::uqshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x8_t raw_uqshl_16_8(uint16x8_t a,
                                                                                     int16x8_t b) {
  return native::detail::arm_neon::uqshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x8_t
native_uqshl_16_8(uint16x8_t a, int16x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint16_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int16_t, 8>>(b);
  return native::detail::arm_neon::to_register<uint16x8_t>(native::uqshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x2_t raw_uqshl_32_2(uint32x2_t a,
                                                                                     int32x2_t b) {
  return native::detail::arm_neon::uqshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x2_t
native_uqshl_32_2(uint32x2_t a, int32x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint32_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int32_t, 2>>(b);
  return native::detail::arm_neon::to_register<uint32x2_t>(native::uqshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x4_t raw_uqshl_32_4(uint32x4_t a,
                                                                                     int32x4_t b) {
  return native::detail::arm_neon::uqshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x4_t
native_uqshl_32_4(uint32x4_t a, int32x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint32_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int32_t, 4>>(b);
  return native::detail::arm_neon::to_register<uint32x4_t>(native::uqshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint64x1_t raw_uqshl_64_1(uint64x1_t a,
                                                                                     int64x1_t b) {
  return native::detail::arm_neon::uqshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint64x1_t
native_uqshl_64_1(uint64x1_t a, int64x1_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint64_t, 1>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int64_t, 1>>(b);
  return native::detail::arm_neon::to_register<uint64x1_t>(native::uqshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint64x2_t raw_uqshl_64_2(uint64x2_t a,
                                                                                     int64x2_t b) {
  return native::detail::arm_neon::uqshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint64x2_t
native_uqshl_64_2(uint64x2_t a, int64x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint64_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int64_t, 2>>(b);
  return native::detail::arm_neon::to_register<uint64x2_t>(native::uqshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x8_t raw_uqrshl_8_8(uint8x8_t a,
                                                                                    int8x8_t b) {
  return native::detail::arm_neon::uqrshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x8_t native_uqrshl_8_8(uint8x8_t a,
                                                                                       int8x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint8_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int8_t, 8>>(b);
  return native::detail::arm_neon::to_register<uint8x8_t>(native::uqrshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x16_t raw_uqrshl_8_16(uint8x16_t a,
                                                                                      int8x16_t b) {
  return native::detail::arm_neon::uqrshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x16_t
native_uqrshl_8_16(uint8x16_t a, int8x16_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint8_t, 16>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int8_t, 16>>(b);
  return native::detail::arm_neon::to_register<uint8x16_t>(native::uqrshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x4_t raw_uqrshl_16_4(uint16x4_t a,
                                                                                      int16x4_t b) {
  return native::detail::arm_neon::uqrshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x4_t
native_uqrshl_16_4(uint16x4_t a, int16x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint16_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int16_t, 4>>(b);
  return native::detail::arm_neon::to_register<uint16x4_t>(native::uqrshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x8_t raw_uqrshl_16_8(uint16x8_t a,
                                                                                      int16x8_t b) {
  return native::detail::arm_neon::uqrshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x8_t
native_uqrshl_16_8(uint16x8_t a, int16x8_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint16_t, 8>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int16_t, 8>>(b);
  return native::detail::arm_neon::to_register<uint16x8_t>(native::uqrshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x2_t raw_uqrshl_32_2(uint32x2_t a,
                                                                                      int32x2_t b) {
  return native::detail::arm_neon::uqrshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x2_t
native_uqrshl_32_2(uint32x2_t a, int32x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint32_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int32_t, 2>>(b);
  return native::detail::arm_neon::to_register<uint32x2_t>(native::uqrshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x4_t raw_uqrshl_32_4(uint32x4_t a,
                                                                                      int32x4_t b) {
  return native::detail::arm_neon::uqrshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x4_t
native_uqrshl_32_4(uint32x4_t a, int32x4_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint32_t, 4>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int32_t, 4>>(b);
  return native::detail::arm_neon::to_register<uint32x4_t>(native::uqrshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint64x1_t raw_uqrshl_64_1(uint64x1_t a,
                                                                                      int64x1_t b) {
  return native::detail::arm_neon::uqrshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint64x1_t
native_uqrshl_64_1(uint64x1_t a, int64x1_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint64_t, 1>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int64_t, 1>>(b);
  return native::detail::arm_neon::to_register<uint64x1_t>(native::uqrshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint64x2_t raw_uqrshl_64_2(uint64x2_t a,
                                                                                      int64x2_t b) {
  return native::detail::arm_neon::uqrshl(a, b);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint64x2_t
native_uqrshl_64_2(uint64x2_t a, int64x2_t b) {
  auto x = native::detail::arm_neon::from_register<V<std::uint64_t, 2>>(a);
  auto y = native::detail::arm_neon::from_register<V<std::int64_t, 2>>(b);
  return native::detail::arm_neon::to_register<uint64x2_t>(native::uqrshl(x, y));
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x8_t raw_sqxtn_16(int16x8_t a) {
  return native::detail::arm_neon::sqxtn(a);
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x8_t native_sqxtn_16(int16x8_t a) {
  auto x = native::detail::arm_neon::from_register<V<std::int16_t, 8>>(a);
  return native::detail::arm_neon::to_register<int8x8_t>(native::sqxtn(x));
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x16_t
raw_sqxtn_high_16(int8x8_t low, int16x8_t a) {
  return native::detail::arm_neon::sqxtn_high(low, a);
}

extern "C" __attribute__((noinline, used, target("neon"))) int8x16_t
native_sqxtn_high_16(int8x8_t low, int16x8_t a) {
  auto x = native::detail::arm_neon::from_register<V<std::int16_t, 8>>(a);
  auto bottom = native::detail::arm_neon::from_register<V<std::int8_t, 8>>(low);
  return native::detail::arm_neon::to_register<int8x16_t>(native::sqxtn_high(bottom, x));
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x4_t raw_sqxtn_32(int32x4_t a) {
  return native::detail::arm_neon::sqxtn(a);
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x4_t native_sqxtn_32(int32x4_t a) {
  auto x = native::detail::arm_neon::from_register<V<std::int32_t, 4>>(a);
  return native::detail::arm_neon::to_register<int16x4_t>(native::sqxtn(x));
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x8_t
raw_sqxtn_high_32(int16x4_t low, int32x4_t a) {
  return native::detail::arm_neon::sqxtn_high(low, a);
}

extern "C" __attribute__((noinline, used, target("neon"))) int16x8_t
native_sqxtn_high_32(int16x4_t low, int32x4_t a) {
  auto x = native::detail::arm_neon::from_register<V<std::int32_t, 4>>(a);
  auto bottom = native::detail::arm_neon::from_register<V<std::int16_t, 4>>(low);
  return native::detail::arm_neon::to_register<int16x8_t>(native::sqxtn_high(bottom, x));
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x2_t raw_sqxtn_64(int64x2_t a) {
  return native::detail::arm_neon::sqxtn(a);
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x2_t native_sqxtn_64(int64x2_t a) {
  auto x = native::detail::arm_neon::from_register<V<std::int64_t, 2>>(a);
  return native::detail::arm_neon::to_register<int32x2_t>(native::sqxtn(x));
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x4_t
raw_sqxtn_high_64(int32x2_t low, int64x2_t a) {
  return native::detail::arm_neon::sqxtn_high(low, a);
}

extern "C" __attribute__((noinline, used, target("neon"))) int32x4_t
native_sqxtn_high_64(int32x2_t low, int64x2_t a) {
  auto x = native::detail::arm_neon::from_register<V<std::int64_t, 2>>(a);
  auto bottom = native::detail::arm_neon::from_register<V<std::int32_t, 2>>(low);
  return native::detail::arm_neon::to_register<int32x4_t>(native::sqxtn_high(bottom, x));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x8_t raw_uqxtn_16(uint16x8_t a) {
  return native::detail::arm_neon::uqxtn(a);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x8_t native_uqxtn_16(uint16x8_t a) {
  auto x = native::detail::arm_neon::from_register<V<std::uint16_t, 8>>(a);
  return native::detail::arm_neon::to_register<uint8x8_t>(native::uqxtn(x));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x16_t
raw_uqxtn_high_16(uint8x8_t low, uint16x8_t a) {
  return native::detail::arm_neon::uqxtn_high(low, a);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x16_t
native_uqxtn_high_16(uint8x8_t low, uint16x8_t a) {
  auto x = native::detail::arm_neon::from_register<V<std::uint16_t, 8>>(a);
  auto bottom = native::detail::arm_neon::from_register<V<std::uint8_t, 8>>(low);
  return native::detail::arm_neon::to_register<uint8x16_t>(native::uqxtn_high(bottom, x));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x4_t raw_uqxtn_32(uint32x4_t a) {
  return native::detail::arm_neon::uqxtn(a);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x4_t
native_uqxtn_32(uint32x4_t a) {
  auto x = native::detail::arm_neon::from_register<V<std::uint32_t, 4>>(a);
  return native::detail::arm_neon::to_register<uint16x4_t>(native::uqxtn(x));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x8_t
raw_uqxtn_high_32(uint16x4_t low, uint32x4_t a) {
  return native::detail::arm_neon::uqxtn_high(low, a);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x8_t
native_uqxtn_high_32(uint16x4_t low, uint32x4_t a) {
  auto x = native::detail::arm_neon::from_register<V<std::uint32_t, 4>>(a);
  auto bottom = native::detail::arm_neon::from_register<V<std::uint16_t, 4>>(low);
  return native::detail::arm_neon::to_register<uint16x8_t>(native::uqxtn_high(bottom, x));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x2_t raw_uqxtn_64(uint64x2_t a) {
  return native::detail::arm_neon::uqxtn(a);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x2_t
native_uqxtn_64(uint64x2_t a) {
  auto x = native::detail::arm_neon::from_register<V<std::uint64_t, 2>>(a);
  return native::detail::arm_neon::to_register<uint32x2_t>(native::uqxtn(x));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x4_t
raw_uqxtn_high_64(uint32x2_t low, uint64x2_t a) {
  return native::detail::arm_neon::uqxtn_high(low, a);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x4_t
native_uqxtn_high_64(uint32x2_t low, uint64x2_t a) {
  auto x = native::detail::arm_neon::from_register<V<std::uint64_t, 2>>(a);
  auto bottom = native::detail::arm_neon::from_register<V<std::uint32_t, 2>>(low);
  return native::detail::arm_neon::to_register<uint32x4_t>(native::uqxtn_high(bottom, x));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x8_t raw_sqxtun_16(int16x8_t a) {
  return native::detail::arm_neon::sqxtun(a);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x8_t native_sqxtun_16(int16x8_t a) {
  auto x = native::detail::arm_neon::from_register<V<std::int16_t, 8>>(a);
  return native::detail::arm_neon::to_register<uint8x8_t>(native::sqxtun(x));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x16_t
raw_sqxtun_high_16(uint8x8_t low, int16x8_t a) {
  return native::detail::arm_neon::sqxtun_high(low, a);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint8x16_t
native_sqxtun_high_16(uint8x8_t low, int16x8_t a) {
  auto x = native::detail::arm_neon::from_register<V<std::int16_t, 8>>(a);
  auto bottom = native::detail::arm_neon::from_register<V<std::uint8_t, 8>>(low);
  return native::detail::arm_neon::to_register<uint8x16_t>(native::sqxtun_high(bottom, x));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x4_t raw_sqxtun_32(int32x4_t a) {
  return native::detail::arm_neon::sqxtun(a);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x4_t
native_sqxtun_32(int32x4_t a) {
  auto x = native::detail::arm_neon::from_register<V<std::int32_t, 4>>(a);
  return native::detail::arm_neon::to_register<uint16x4_t>(native::sqxtun(x));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x8_t
raw_sqxtun_high_32(uint16x4_t low, int32x4_t a) {
  return native::detail::arm_neon::sqxtun_high(low, a);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint16x8_t
native_sqxtun_high_32(uint16x4_t low, int32x4_t a) {
  auto x = native::detail::arm_neon::from_register<V<std::int32_t, 4>>(a);
  auto bottom = native::detail::arm_neon::from_register<V<std::uint16_t, 4>>(low);
  return native::detail::arm_neon::to_register<uint16x8_t>(native::sqxtun_high(bottom, x));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x2_t raw_sqxtun_64(int64x2_t a) {
  return native::detail::arm_neon::sqxtun(a);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x2_t
native_sqxtun_64(int64x2_t a) {
  auto x = native::detail::arm_neon::from_register<V<std::int64_t, 2>>(a);
  return native::detail::arm_neon::to_register<uint32x2_t>(native::sqxtun(x));
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x4_t
raw_sqxtun_high_64(uint32x2_t low, int64x2_t a) {
  return native::detail::arm_neon::sqxtun_high(low, a);
}

extern "C" __attribute__((noinline, used, target("neon"))) uint32x4_t
native_sqxtun_high_64(uint32x2_t low, int64x2_t a) {
  auto x = native::detail::arm_neon::from_register<V<std::int64_t, 2>>(a);
  auto bottom = native::detail::arm_neon::from_register<V<std::uint32_t, 2>>(low);
  return native::detail::arm_neon::to_register<uint32x4_t>(native::sqxtun_high(bottom, x));
}
