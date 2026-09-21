// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/isa.h>
#include <native/targets.h>
#ifndef __ARM_FEATURE_MATMUL_INT8
#error This test needs the explicit compiler I8MM target
#endif
static_assert(NATIVE_TARGET_MINIMUM.has(native::arm_feature::i8mm));
static_assert(NATIVE_TARGET_MINIMUM.has(native::arm_feature::neon));
static_assert(NATIVE_TARGET_MINIMUM<=native::detail::arm_features);
static_assert(!NATIVE_TARGET_MINIMUM.has(native::arm_feature::dotprod));
static_assert(!NATIVE_TARGET_MINIMUM.has(native::arm_feature::neon_fp16));
static_assert(!NATIVE_TARGET_MINIMUM.has(native::arm_feature::neon_bf16));
