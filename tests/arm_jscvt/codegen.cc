// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native_test.jscvt;
extern "C" __attribute__((target("jsconv"), noinline))
std::int32_t native_jcvt(double x) { return native_test::jcvt(x); }
// The builtin requires v8.3a; the public wrapper admits just jsconv.
extern "C" __attribute__((target("arch=armv8.3-a"), noinline))
std::int32_t raw_jcvt(double x) { return __builtin_arm_jcvt(x); }
