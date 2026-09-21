// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/targets.h>
import native.isa;
#if NATIVE_SM_EXPECT_ENABLED
#ifndef __ARM_FEATURE_SM3
#error expected SM3 compiler macro
#endif
#ifndef __ARM_FEATURE_SM4
#error expected SM4 compiler macro
#endif
static_assert(NATIVE_TARGET_MINIMUM.has(native::target_features<native::arm>("sm4")));
#else
static_assert(!NATIVE_TARGET_MINIMUM.has(native::arm_feature::sm3));
static_assert(!NATIVE_TARGET_MINIMUM.has(native::arm_feature::sm4));
#endif
int main() {}
