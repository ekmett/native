// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.arm.jscvt;
#if NATIVE_TEST_WEAK
__attribute__((target("jsconv")))
std::int32_t rejected(double x) { return native::jcvt<native::isa<native::arm>{}>(x); }
#else
__attribute__((target("no-jsconv")))
std::int32_t rejected(double x) { return native::jcvt<native::target_features("jsconv")>(x); }
#endif
