// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <arm_neon.h>
#include <native/arm/rdm.h>
import native.arm.rdm;
#include "simd_adapter.h"
#if defined(__ARM_FEATURE_DOTPROD) || defined(__ARM_FEATURE_QRDMX) || \
    defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC) || defined(__ARM_FEATURE_FP16_FML) || \
    defined(__ARM_FEATURE_COMPLEX) || defined(__ARM_FEATURE_MATMUL_INT8) || \
    defined(__ARM_FEATURE_SVE) || defined(__ARM_FEATURE_ATOMICS)
#error The code-generation fixture must start from baseline AArch64.
#endif
constexpr auto requirements = native::isa<native::arm>(native::arm_feature::rdm);
extern "C" __attribute__((target("rdm"), noinline))
int16_t native_sqrdmlah_int16(int16_t a, int16_t b, int16_t c) noexcept {
  return rdm_api::sqrdmlah<requirements>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16_t native_sqrdmlah_int16_lane4(int16_t a, int16_t b, int16x4_t c) noexcept {
  return rdm_api::sqrdmlah_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16_t native_sqrdmlah_int16_lane8(int16_t a, int16_t b, int16x8_t c) noexcept {
  return rdm_api::sqrdmlah_lane<requirements, 7>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16x4_t native_sqrdmlah_int16x4(int16x4_t a, int16x4_t b, int16x4_t c) noexcept {
  return rdm_api::sqrdmlah<requirements>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16x4_t native_sqrdmlah_int16x4_lane4(int16x4_t a, int16x4_t b, int16x4_t c) noexcept {
  return rdm_api::sqrdmlah_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16x4_t native_sqrdmlah_int16x4_lane8(int16x4_t a, int16x4_t b, int16x8_t c) noexcept {
  return rdm_api::sqrdmlah_lane<requirements, 7>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16x8_t native_sqrdmlah_int16x8(int16x8_t a, int16x8_t b, int16x8_t c) noexcept {
  return rdm_api::sqrdmlah<requirements>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16x8_t native_sqrdmlah_int16x8_lane4(int16x8_t a, int16x8_t b, int16x4_t c) noexcept {
  return rdm_api::sqrdmlah_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16x8_t native_sqrdmlah_int16x8_lane8(int16x8_t a, int16x8_t b, int16x8_t c) noexcept {
  return rdm_api::sqrdmlah_lane<requirements, 7>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32_t native_sqrdmlah_int32(int32_t a, int32_t b, int32_t c) noexcept {
  return rdm_api::sqrdmlah<requirements>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32_t native_sqrdmlah_int32_lane2(int32_t a, int32_t b, int32x2_t c) noexcept {
  return rdm_api::sqrdmlah_lane<requirements, 1>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32_t native_sqrdmlah_int32_lane4(int32_t a, int32_t b, int32x4_t c) noexcept {
  return rdm_api::sqrdmlah_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32x2_t native_sqrdmlah_int32x2(int32x2_t a, int32x2_t b, int32x2_t c) noexcept {
  return rdm_api::sqrdmlah<requirements>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32x2_t native_sqrdmlah_int32x2_lane2(int32x2_t a, int32x2_t b, int32x2_t c) noexcept {
  return rdm_api::sqrdmlah_lane<requirements, 1>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32x2_t native_sqrdmlah_int32x2_lane4(int32x2_t a, int32x2_t b, int32x4_t c) noexcept {
  return rdm_api::sqrdmlah_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32x4_t native_sqrdmlah_int32x4(int32x4_t a, int32x4_t b, int32x4_t c) noexcept {
  return rdm_api::sqrdmlah<requirements>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32x4_t native_sqrdmlah_int32x4_lane2(int32x4_t a, int32x4_t b, int32x2_t c) noexcept {
  return rdm_api::sqrdmlah_lane<requirements, 1>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32x4_t native_sqrdmlah_int32x4_lane4(int32x4_t a, int32x4_t b, int32x4_t c) noexcept {
  return rdm_api::sqrdmlah_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16_t native_sqrdmlsh_int16(int16_t a, int16_t b, int16_t c) noexcept {
  return rdm_api::sqrdmlsh<requirements>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16_t native_sqrdmlsh_int16_lane4(int16_t a, int16_t b, int16x4_t c) noexcept {
  return rdm_api::sqrdmlsh_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16_t native_sqrdmlsh_int16_lane8(int16_t a, int16_t b, int16x8_t c) noexcept {
  return rdm_api::sqrdmlsh_lane<requirements, 7>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16x4_t native_sqrdmlsh_int16x4(int16x4_t a, int16x4_t b, int16x4_t c) noexcept {
  return rdm_api::sqrdmlsh<requirements>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16x4_t native_sqrdmlsh_int16x4_lane4(int16x4_t a, int16x4_t b, int16x4_t c) noexcept {
  return rdm_api::sqrdmlsh_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16x4_t native_sqrdmlsh_int16x4_lane8(int16x4_t a, int16x4_t b, int16x8_t c) noexcept {
  return rdm_api::sqrdmlsh_lane<requirements, 7>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16x8_t native_sqrdmlsh_int16x8(int16x8_t a, int16x8_t b, int16x8_t c) noexcept {
  return rdm_api::sqrdmlsh<requirements>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16x8_t native_sqrdmlsh_int16x8_lane4(int16x8_t a, int16x8_t b, int16x4_t c) noexcept {
  return rdm_api::sqrdmlsh_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16x8_t native_sqrdmlsh_int16x8_lane8(int16x8_t a, int16x8_t b, int16x8_t c) noexcept {
  return rdm_api::sqrdmlsh_lane<requirements, 7>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32_t native_sqrdmlsh_int32(int32_t a, int32_t b, int32_t c) noexcept {
  return rdm_api::sqrdmlsh<requirements>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32_t native_sqrdmlsh_int32_lane2(int32_t a, int32_t b, int32x2_t c) noexcept {
  return rdm_api::sqrdmlsh_lane<requirements, 1>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32_t native_sqrdmlsh_int32_lane4(int32_t a, int32_t b, int32x4_t c) noexcept {
  return rdm_api::sqrdmlsh_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32x2_t native_sqrdmlsh_int32x2(int32x2_t a, int32x2_t b, int32x2_t c) noexcept {
  return rdm_api::sqrdmlsh<requirements>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32x2_t native_sqrdmlsh_int32x2_lane2(int32x2_t a, int32x2_t b, int32x2_t c) noexcept {
  return rdm_api::sqrdmlsh_lane<requirements, 1>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32x2_t native_sqrdmlsh_int32x2_lane4(int32x2_t a, int32x2_t b, int32x4_t c) noexcept {
  return rdm_api::sqrdmlsh_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32x4_t native_sqrdmlsh_int32x4(int32x4_t a, int32x4_t b, int32x4_t c) noexcept {
  return rdm_api::sqrdmlsh<requirements>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32x4_t native_sqrdmlsh_int32x4_lane2(int32x4_t a, int32x4_t b, int32x2_t c) noexcept {
  return rdm_api::sqrdmlsh_lane<requirements, 1>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32x4_t native_sqrdmlsh_int32x4_lane4(int32x4_t a, int32x4_t b, int32x4_t c) noexcept {
  return rdm_api::sqrdmlsh_lane<requirements, 3>(a, b, c);
}

// Same signatures and target attributes, with only the public simd layer removed.
extern "C" __attribute__((target("rdm"), noinline))
int16_t raw_sqrdmlah_int16(int16_t a, int16_t b, int16_t c) noexcept {
  return native::detail::sqrdmlah<requirements>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16_t raw_sqrdmlah_int16_lane4(int16_t a, int16_t b, int16x4_t c) noexcept {
  return native::detail::sqrdmlah_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16_t raw_sqrdmlah_int16_lane8(int16_t a, int16_t b, int16x8_t c) noexcept {
  return native::detail::sqrdmlah_lane<requirements, 7>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16x4_t raw_sqrdmlah_int16x4(int16x4_t a, int16x4_t b, int16x4_t c) noexcept {
  return native::detail::sqrdmlah<requirements>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16x4_t raw_sqrdmlah_int16x4_lane4(int16x4_t a, int16x4_t b, int16x4_t c) noexcept {
  return native::detail::sqrdmlah_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16x4_t raw_sqrdmlah_int16x4_lane8(int16x4_t a, int16x4_t b, int16x8_t c) noexcept {
  return native::detail::sqrdmlah_lane<requirements, 7>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16x8_t raw_sqrdmlah_int16x8(int16x8_t a, int16x8_t b, int16x8_t c) noexcept {
  return native::detail::sqrdmlah<requirements>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16x8_t raw_sqrdmlah_int16x8_lane4(int16x8_t a, int16x8_t b, int16x4_t c) noexcept {
  return native::detail::sqrdmlah_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16x8_t raw_sqrdmlah_int16x8_lane8(int16x8_t a, int16x8_t b, int16x8_t c) noexcept {
  return native::detail::sqrdmlah_lane<requirements, 7>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32_t raw_sqrdmlah_int32(int32_t a, int32_t b, int32_t c) noexcept {
  return native::detail::sqrdmlah<requirements>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32_t raw_sqrdmlah_int32_lane2(int32_t a, int32_t b, int32x2_t c) noexcept {
  return native::detail::sqrdmlah_lane<requirements, 1>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32_t raw_sqrdmlah_int32_lane4(int32_t a, int32_t b, int32x4_t c) noexcept {
  return native::detail::sqrdmlah_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32x2_t raw_sqrdmlah_int32x2(int32x2_t a, int32x2_t b, int32x2_t c) noexcept {
  return native::detail::sqrdmlah<requirements>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32x2_t raw_sqrdmlah_int32x2_lane2(int32x2_t a, int32x2_t b, int32x2_t c) noexcept {
  return native::detail::sqrdmlah_lane<requirements, 1>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32x2_t raw_sqrdmlah_int32x2_lane4(int32x2_t a, int32x2_t b, int32x4_t c) noexcept {
  return native::detail::sqrdmlah_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32x4_t raw_sqrdmlah_int32x4(int32x4_t a, int32x4_t b, int32x4_t c) noexcept {
  return native::detail::sqrdmlah<requirements>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32x4_t raw_sqrdmlah_int32x4_lane2(int32x4_t a, int32x4_t b, int32x2_t c) noexcept {
  return native::detail::sqrdmlah_lane<requirements, 1>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32x4_t raw_sqrdmlah_int32x4_lane4(int32x4_t a, int32x4_t b, int32x4_t c) noexcept {
  return native::detail::sqrdmlah_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16_t raw_sqrdmlsh_int16(int16_t a, int16_t b, int16_t c) noexcept {
  return native::detail::sqrdmlsh<requirements>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16_t raw_sqrdmlsh_int16_lane4(int16_t a, int16_t b, int16x4_t c) noexcept {
  return native::detail::sqrdmlsh_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16_t raw_sqrdmlsh_int16_lane8(int16_t a, int16_t b, int16x8_t c) noexcept {
  return native::detail::sqrdmlsh_lane<requirements, 7>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16x4_t raw_sqrdmlsh_int16x4(int16x4_t a, int16x4_t b, int16x4_t c) noexcept {
  return native::detail::sqrdmlsh<requirements>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16x4_t raw_sqrdmlsh_int16x4_lane4(int16x4_t a, int16x4_t b, int16x4_t c) noexcept {
  return native::detail::sqrdmlsh_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16x4_t raw_sqrdmlsh_int16x4_lane8(int16x4_t a, int16x4_t b, int16x8_t c) noexcept {
  return native::detail::sqrdmlsh_lane<requirements, 7>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16x8_t raw_sqrdmlsh_int16x8(int16x8_t a, int16x8_t b, int16x8_t c) noexcept {
  return native::detail::sqrdmlsh<requirements>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16x8_t raw_sqrdmlsh_int16x8_lane4(int16x8_t a, int16x8_t b, int16x4_t c) noexcept {
  return native::detail::sqrdmlsh_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int16x8_t raw_sqrdmlsh_int16x8_lane8(int16x8_t a, int16x8_t b, int16x8_t c) noexcept {
  return native::detail::sqrdmlsh_lane<requirements, 7>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32_t raw_sqrdmlsh_int32(int32_t a, int32_t b, int32_t c) noexcept {
  return native::detail::sqrdmlsh<requirements>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32_t raw_sqrdmlsh_int32_lane2(int32_t a, int32_t b, int32x2_t c) noexcept {
  return native::detail::sqrdmlsh_lane<requirements, 1>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32_t raw_sqrdmlsh_int32_lane4(int32_t a, int32_t b, int32x4_t c) noexcept {
  return native::detail::sqrdmlsh_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32x2_t raw_sqrdmlsh_int32x2(int32x2_t a, int32x2_t b, int32x2_t c) noexcept {
  return native::detail::sqrdmlsh<requirements>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32x2_t raw_sqrdmlsh_int32x2_lane2(int32x2_t a, int32x2_t b, int32x2_t c) noexcept {
  return native::detail::sqrdmlsh_lane<requirements, 1>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32x2_t raw_sqrdmlsh_int32x2_lane4(int32x2_t a, int32x2_t b, int32x4_t c) noexcept {
  return native::detail::sqrdmlsh_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32x4_t raw_sqrdmlsh_int32x4(int32x4_t a, int32x4_t b, int32x4_t c) noexcept {
  return native::detail::sqrdmlsh<requirements>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32x4_t raw_sqrdmlsh_int32x4_lane2(int32x4_t a, int32x4_t b, int32x2_t c) noexcept {
  return native::detail::sqrdmlsh_lane<requirements, 1>(a, b, c);
}
extern "C" __attribute__((target("rdm"), noinline))
int32x4_t raw_sqrdmlsh_int32x4_lane4(int32x4_t a, int32x4_t b, int32x4_t c) noexcept {
  return native::detail::sqrdmlsh_lane<requirements, 3>(a, b, c);
}
