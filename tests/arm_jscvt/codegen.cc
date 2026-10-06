// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.arm.jscvt;
constexpr auto arch = native::target_features("jsconv");
extern "C" __attribute__((target("jsconv"), noinline))
std::int32_t native_jcvt(double x) { return native::jcvt<arch>(x); }
// The builtin requires v8.3a; the public wrapper admits just jsconv.
extern "C" __attribute__((target("arch=armv8.3-a"), noinline))
std::int32_t raw_jcvt(double x) { return __builtin_arm_jcvt(x); }
