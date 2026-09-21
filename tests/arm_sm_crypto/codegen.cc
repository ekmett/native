// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <arm_neon.h>
#include <cstdint>
import native.arm.sm3;
import native.arm.sm4;
constexpr auto architecture = native::target_features<native::arm>("sm4");
using V = native::simd<std::uint32_t, 4, architecture>;

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t raw_sm3ss1(uint32x4_t a,
                                                                                uint32x4_t b,
                                                                                uint32x4_t c) {
  return vsm3ss1q_u32(a, b, c);
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t native_sm3ss1(uint32x4_t a,
                                                                                   uint32x4_t b,
                                                                                   uint32x4_t c) {
  return __builtin_bit_cast(uint32x4_t,
                            native::sm3ss1(V::from_native(__builtin_bit_cast(V::native_type, a)),
                                           V::from_native(__builtin_bit_cast(V::native_type, b)),
                                           V::from_native(__builtin_bit_cast(V::native_type, c)))
                                .to_native());
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t raw_sm3tt1a0(uint32x4_t a,
                                                                                  uint32x4_t b,
                                                                                  uint32x4_t c) {
  return vsm3tt1aq_u32(a, b, c, 0);
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t native_sm3tt1a0(uint32x4_t a,
                                                                                     uint32x4_t b,
                                                                                     uint32x4_t c) {
  return __builtin_bit_cast(uint32x4_t, native::sm3tt1a<architecture, 0>(
                                            V::from_native(__builtin_bit_cast(V::native_type, a)),
                                            V::from_native(__builtin_bit_cast(V::native_type, b)),
                                            V::from_native(__builtin_bit_cast(V::native_type, c)))
                                            .to_native());
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t raw_sm3tt1a1(uint32x4_t a,
                                                                                  uint32x4_t b,
                                                                                  uint32x4_t c) {
  return vsm3tt1aq_u32(a, b, c, 1);
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t native_sm3tt1a1(uint32x4_t a,
                                                                                     uint32x4_t b,
                                                                                     uint32x4_t c) {
  return __builtin_bit_cast(uint32x4_t, native::sm3tt1a<architecture, 1>(
                                            V::from_native(__builtin_bit_cast(V::native_type, a)),
                                            V::from_native(__builtin_bit_cast(V::native_type, b)),
                                            V::from_native(__builtin_bit_cast(V::native_type, c)))
                                            .to_native());
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t raw_sm3tt1a2(uint32x4_t a,
                                                                                  uint32x4_t b,
                                                                                  uint32x4_t c) {
  return vsm3tt1aq_u32(a, b, c, 2);
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t native_sm3tt1a2(uint32x4_t a,
                                                                                     uint32x4_t b,
                                                                                     uint32x4_t c) {
  return __builtin_bit_cast(uint32x4_t, native::sm3tt1a<architecture, 2>(
                                            V::from_native(__builtin_bit_cast(V::native_type, a)),
                                            V::from_native(__builtin_bit_cast(V::native_type, b)),
                                            V::from_native(__builtin_bit_cast(V::native_type, c)))
                                            .to_native());
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t raw_sm3tt1a3(uint32x4_t a,
                                                                                  uint32x4_t b,
                                                                                  uint32x4_t c) {
  return vsm3tt1aq_u32(a, b, c, 3);
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t native_sm3tt1a3(uint32x4_t a,
                                                                                     uint32x4_t b,
                                                                                     uint32x4_t c) {
  return __builtin_bit_cast(uint32x4_t, native::sm3tt1a<architecture, 3>(
                                            V::from_native(__builtin_bit_cast(V::native_type, a)),
                                            V::from_native(__builtin_bit_cast(V::native_type, b)),
                                            V::from_native(__builtin_bit_cast(V::native_type, c)))
                                            .to_native());
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t raw_sm3tt1b0(uint32x4_t a,
                                                                                  uint32x4_t b,
                                                                                  uint32x4_t c) {
  return vsm3tt1bq_u32(a, b, c, 0);
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t native_sm3tt1b0(uint32x4_t a,
                                                                                     uint32x4_t b,
                                                                                     uint32x4_t c) {
  return __builtin_bit_cast(uint32x4_t, native::sm3tt1b<architecture, 0>(
                                            V::from_native(__builtin_bit_cast(V::native_type, a)),
                                            V::from_native(__builtin_bit_cast(V::native_type, b)),
                                            V::from_native(__builtin_bit_cast(V::native_type, c)))
                                            .to_native());
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t raw_sm3tt1b1(uint32x4_t a,
                                                                                  uint32x4_t b,
                                                                                  uint32x4_t c) {
  return vsm3tt1bq_u32(a, b, c, 1);
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t native_sm3tt1b1(uint32x4_t a,
                                                                                     uint32x4_t b,
                                                                                     uint32x4_t c) {
  return __builtin_bit_cast(uint32x4_t, native::sm3tt1b<architecture, 1>(
                                            V::from_native(__builtin_bit_cast(V::native_type, a)),
                                            V::from_native(__builtin_bit_cast(V::native_type, b)),
                                            V::from_native(__builtin_bit_cast(V::native_type, c)))
                                            .to_native());
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t raw_sm3tt1b2(uint32x4_t a,
                                                                                  uint32x4_t b,
                                                                                  uint32x4_t c) {
  return vsm3tt1bq_u32(a, b, c, 2);
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t native_sm3tt1b2(uint32x4_t a,
                                                                                     uint32x4_t b,
                                                                                     uint32x4_t c) {
  return __builtin_bit_cast(uint32x4_t, native::sm3tt1b<architecture, 2>(
                                            V::from_native(__builtin_bit_cast(V::native_type, a)),
                                            V::from_native(__builtin_bit_cast(V::native_type, b)),
                                            V::from_native(__builtin_bit_cast(V::native_type, c)))
                                            .to_native());
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t raw_sm3tt1b3(uint32x4_t a,
                                                                                  uint32x4_t b,
                                                                                  uint32x4_t c) {
  return vsm3tt1bq_u32(a, b, c, 3);
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t native_sm3tt1b3(uint32x4_t a,
                                                                                     uint32x4_t b,
                                                                                     uint32x4_t c) {
  return __builtin_bit_cast(uint32x4_t, native::sm3tt1b<architecture, 3>(
                                            V::from_native(__builtin_bit_cast(V::native_type, a)),
                                            V::from_native(__builtin_bit_cast(V::native_type, b)),
                                            V::from_native(__builtin_bit_cast(V::native_type, c)))
                                            .to_native());
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t raw_sm3tt2a0(uint32x4_t a,
                                                                                  uint32x4_t b,
                                                                                  uint32x4_t c) {
  return vsm3tt2aq_u32(a, b, c, 0);
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t native_sm3tt2a0(uint32x4_t a,
                                                                                     uint32x4_t b,
                                                                                     uint32x4_t c) {
  return __builtin_bit_cast(uint32x4_t, native::sm3tt2a<architecture, 0>(
                                            V::from_native(__builtin_bit_cast(V::native_type, a)),
                                            V::from_native(__builtin_bit_cast(V::native_type, b)),
                                            V::from_native(__builtin_bit_cast(V::native_type, c)))
                                            .to_native());
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t raw_sm3tt2a1(uint32x4_t a,
                                                                                  uint32x4_t b,
                                                                                  uint32x4_t c) {
  return vsm3tt2aq_u32(a, b, c, 1);
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t native_sm3tt2a1(uint32x4_t a,
                                                                                     uint32x4_t b,
                                                                                     uint32x4_t c) {
  return __builtin_bit_cast(uint32x4_t, native::sm3tt2a<architecture, 1>(
                                            V::from_native(__builtin_bit_cast(V::native_type, a)),
                                            V::from_native(__builtin_bit_cast(V::native_type, b)),
                                            V::from_native(__builtin_bit_cast(V::native_type, c)))
                                            .to_native());
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t raw_sm3tt2a2(uint32x4_t a,
                                                                                  uint32x4_t b,
                                                                                  uint32x4_t c) {
  return vsm3tt2aq_u32(a, b, c, 2);
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t native_sm3tt2a2(uint32x4_t a,
                                                                                     uint32x4_t b,
                                                                                     uint32x4_t c) {
  return __builtin_bit_cast(uint32x4_t, native::sm3tt2a<architecture, 2>(
                                            V::from_native(__builtin_bit_cast(V::native_type, a)),
                                            V::from_native(__builtin_bit_cast(V::native_type, b)),
                                            V::from_native(__builtin_bit_cast(V::native_type, c)))
                                            .to_native());
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t raw_sm3tt2a3(uint32x4_t a,
                                                                                  uint32x4_t b,
                                                                                  uint32x4_t c) {
  return vsm3tt2aq_u32(a, b, c, 3);
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t native_sm3tt2a3(uint32x4_t a,
                                                                                     uint32x4_t b,
                                                                                     uint32x4_t c) {
  return __builtin_bit_cast(uint32x4_t, native::sm3tt2a<architecture, 3>(
                                            V::from_native(__builtin_bit_cast(V::native_type, a)),
                                            V::from_native(__builtin_bit_cast(V::native_type, b)),
                                            V::from_native(__builtin_bit_cast(V::native_type, c)))
                                            .to_native());
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t raw_sm3tt2b0(uint32x4_t a,
                                                                                  uint32x4_t b,
                                                                                  uint32x4_t c) {
  return vsm3tt2bq_u32(a, b, c, 0);
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t native_sm3tt2b0(uint32x4_t a,
                                                                                     uint32x4_t b,
                                                                                     uint32x4_t c) {
  return __builtin_bit_cast(uint32x4_t, native::sm3tt2b<architecture, 0>(
                                            V::from_native(__builtin_bit_cast(V::native_type, a)),
                                            V::from_native(__builtin_bit_cast(V::native_type, b)),
                                            V::from_native(__builtin_bit_cast(V::native_type, c)))
                                            .to_native());
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t raw_sm3tt2b1(uint32x4_t a,
                                                                                  uint32x4_t b,
                                                                                  uint32x4_t c) {
  return vsm3tt2bq_u32(a, b, c, 1);
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t native_sm3tt2b1(uint32x4_t a,
                                                                                     uint32x4_t b,
                                                                                     uint32x4_t c) {
  return __builtin_bit_cast(uint32x4_t, native::sm3tt2b<architecture, 1>(
                                            V::from_native(__builtin_bit_cast(V::native_type, a)),
                                            V::from_native(__builtin_bit_cast(V::native_type, b)),
                                            V::from_native(__builtin_bit_cast(V::native_type, c)))
                                            .to_native());
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t raw_sm3tt2b2(uint32x4_t a,
                                                                                  uint32x4_t b,
                                                                                  uint32x4_t c) {
  return vsm3tt2bq_u32(a, b, c, 2);
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t native_sm3tt2b2(uint32x4_t a,
                                                                                     uint32x4_t b,
                                                                                     uint32x4_t c) {
  return __builtin_bit_cast(uint32x4_t, native::sm3tt2b<architecture, 2>(
                                            V::from_native(__builtin_bit_cast(V::native_type, a)),
                                            V::from_native(__builtin_bit_cast(V::native_type, b)),
                                            V::from_native(__builtin_bit_cast(V::native_type, c)))
                                            .to_native());
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t raw_sm3tt2b3(uint32x4_t a,
                                                                                  uint32x4_t b,
                                                                                  uint32x4_t c) {
  return vsm3tt2bq_u32(a, b, c, 3);
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t native_sm3tt2b3(uint32x4_t a,
                                                                                     uint32x4_t b,
                                                                                     uint32x4_t c) {
  return __builtin_bit_cast(uint32x4_t, native::sm3tt2b<architecture, 3>(
                                            V::from_native(__builtin_bit_cast(V::native_type, a)),
                                            V::from_native(__builtin_bit_cast(V::native_type, b)),
                                            V::from_native(__builtin_bit_cast(V::native_type, c)))
                                            .to_native());
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t raw_sm3partw1(uint32x4_t a,
                                                                                   uint32x4_t b,
                                                                                   uint32x4_t c) {
  return vsm3partw1q_u32(a, b, c);
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t
native_sm3partw1(uint32x4_t a, uint32x4_t b, uint32x4_t c) {
  return __builtin_bit_cast(uint32x4_t,
                            native::sm3partw1(V::from_native(__builtin_bit_cast(V::native_type, a)),
                                              V::from_native(__builtin_bit_cast(V::native_type, b)),
                                              V::from_native(__builtin_bit_cast(V::native_type, c)))
                                .to_native());
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t raw_sm3partw2(uint32x4_t a,
                                                                                   uint32x4_t b,
                                                                                   uint32x4_t c) {
  return vsm3partw2q_u32(a, b, c);
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t
native_sm3partw2(uint32x4_t a, uint32x4_t b, uint32x4_t c) {
  return __builtin_bit_cast(uint32x4_t,
                            native::sm3partw2(V::from_native(__builtin_bit_cast(V::native_type, a)),
                                              V::from_native(__builtin_bit_cast(V::native_type, b)),
                                              V::from_native(__builtin_bit_cast(V::native_type, c)))
                                .to_native());
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t raw_sm4e(uint32x4_t a,
                                                                              uint32x4_t b) {
  return vsm4eq_u32(a, b);
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t native_sm4e(uint32x4_t a,
                                                                                 uint32x4_t b) {
  return __builtin_bit_cast(uint32x4_t,
                            native::sm4e(V::from_native(__builtin_bit_cast(V::native_type, a)),
                                         V::from_native(__builtin_bit_cast(V::native_type, b)))
                                .to_native());
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t raw_sm4ekey(uint32x4_t a,
                                                                                 uint32x4_t b) {
  return vsm4ekeyq_u32(a, b);
}

extern "C" __attribute__((noinline, used, target("sm4"))) uint32x4_t native_sm4ekey(uint32x4_t a,
                                                                                    uint32x4_t b) {
  return __builtin_bit_cast(uint32x4_t,
                            native::sm4ekey(V::from_native(__builtin_bit_cast(V::native_type, a)),
                                            V::from_native(__builtin_bit_cast(V::native_type, b)))
                                .to_native());
}
