// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <arm_neon.h>
#include <native/arm/fp16fml.h>
import native.arm.fp16fml;
#include "simd_adapter.h"
constexpr native::isa arch{native::arm_feature::fp16fml};
extern "C" __attribute__((target("fp16fml"), noinline)) float32x2_t native_fmlal_2(float32x2_t acc, float16x4_t a, float16x4_t b) { return fp16fml_api::fmlal<arch>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x2_t native_fmlal_2_lane(float32x2_t acc, float16x4_t a, float16x8_t b) { return fp16fml_api::fmlal_lane<arch, 7>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x4_t native_fmlal_4(float32x4_t acc, float16x8_t a, float16x8_t b) { return fp16fml_api::fmlal<arch>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x4_t native_fmlal_4_lane(float32x4_t acc, float16x8_t a, float16x8_t b) { return fp16fml_api::fmlal_lane<arch, 7>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x2_t native_fmlal2_2(float32x2_t acc, float16x4_t a, float16x4_t b) { return fp16fml_api::fmlal2<arch>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x2_t native_fmlal2_2_lane(float32x2_t acc, float16x4_t a, float16x8_t b) { return fp16fml_api::fmlal2_lane<arch, 7>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x4_t native_fmlal2_4(float32x4_t acc, float16x8_t a, float16x8_t b) { return fp16fml_api::fmlal2<arch>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x4_t native_fmlal2_4_lane(float32x4_t acc, float16x8_t a, float16x8_t b) { return fp16fml_api::fmlal2_lane<arch, 7>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x2_t native_fmlsl_2(float32x2_t acc, float16x4_t a, float16x4_t b) { return fp16fml_api::fmlsl<arch>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x2_t native_fmlsl_2_lane(float32x2_t acc, float16x4_t a, float16x8_t b) { return fp16fml_api::fmlsl_lane<arch, 7>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x4_t native_fmlsl_4(float32x4_t acc, float16x8_t a, float16x8_t b) { return fp16fml_api::fmlsl<arch>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x4_t native_fmlsl_4_lane(float32x4_t acc, float16x8_t a, float16x8_t b) { return fp16fml_api::fmlsl_lane<arch, 7>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x2_t native_fmlsl2_2(float32x2_t acc, float16x4_t a, float16x4_t b) { return fp16fml_api::fmlsl2<arch>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x2_t native_fmlsl2_2_lane(float32x2_t acc, float16x4_t a, float16x8_t b) { return fp16fml_api::fmlsl2_lane<arch, 7>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x4_t native_fmlsl2_4(float32x4_t acc, float16x8_t a, float16x8_t b) { return fp16fml_api::fmlsl2<arch>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x4_t native_fmlsl2_4_lane(float32x4_t acc, float16x8_t a, float16x8_t b) { return fp16fml_api::fmlsl2_lane<arch, 7>(acc, a, b); }
extern "C" __attribute__((noinline)) unsigned native_baseline(unsigned x) { return x + 1; }

// Same signatures and target attributes, with only the public simd layer removed.
extern "C" __attribute__((target("fp16fml"), noinline)) float32x2_t raw_fmlal_2(float32x2_t acc, float16x4_t a, float16x4_t b) { return native::detail::fmlal<arch>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x2_t raw_fmlal_2_lane(float32x2_t acc, float16x4_t a, float16x8_t b) { return native::detail::fmlal_lane<arch, 7>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x4_t raw_fmlal_4(float32x4_t acc, float16x8_t a, float16x8_t b) { return native::detail::fmlal<arch>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x4_t raw_fmlal_4_lane(float32x4_t acc, float16x8_t a, float16x8_t b) { return native::detail::fmlal_lane<arch, 7>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x2_t raw_fmlal2_2(float32x2_t acc, float16x4_t a, float16x4_t b) { return native::detail::fmlal2<arch>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x2_t raw_fmlal2_2_lane(float32x2_t acc, float16x4_t a, float16x8_t b) { return native::detail::fmlal2_lane<arch, 7>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x4_t raw_fmlal2_4(float32x4_t acc, float16x8_t a, float16x8_t b) { return native::detail::fmlal2<arch>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x4_t raw_fmlal2_4_lane(float32x4_t acc, float16x8_t a, float16x8_t b) { return native::detail::fmlal2_lane<arch, 7>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x2_t raw_fmlsl_2(float32x2_t acc, float16x4_t a, float16x4_t b) { return native::detail::fmlsl<arch>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x2_t raw_fmlsl_2_lane(float32x2_t acc, float16x4_t a, float16x8_t b) { return native::detail::fmlsl_lane<arch, 7>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x4_t raw_fmlsl_4(float32x4_t acc, float16x8_t a, float16x8_t b) { return native::detail::fmlsl<arch>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x4_t raw_fmlsl_4_lane(float32x4_t acc, float16x8_t a, float16x8_t b) { return native::detail::fmlsl_lane<arch, 7>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x2_t raw_fmlsl2_2(float32x2_t acc, float16x4_t a, float16x4_t b) { return native::detail::fmlsl2<arch>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x2_t raw_fmlsl2_2_lane(float32x2_t acc, float16x4_t a, float16x8_t b) { return native::detail::fmlsl2_lane<arch, 7>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x4_t raw_fmlsl2_4(float32x4_t acc, float16x8_t a, float16x8_t b) { return native::detail::fmlsl2<arch>(acc, a, b); }
extern "C" __attribute__((target("fp16fml"), noinline)) float32x4_t raw_fmlsl2_4_lane(float32x4_t acc, float16x8_t a, float16x8_t b) { return native::detail::fmlsl2_lane<arch, 7>(acc, a, b); }
extern "C" __attribute__((noinline)) unsigned raw_baseline(unsigned x) { return x + 1; }
