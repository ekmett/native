// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <native/attributes.h>
#if NATIVE_SCALAR_NEGATIVE < 6
#include "weak_imports.h"
#else
#include "strong_imports.h"
#endif

#if NATIVE_SCALAR_NEGATIVE == 0
native_target("bmi")
std::uint32_t rejected(std::uint32_t a) { return native::andn(a, a); }
#endif

#if NATIVE_SCALAR_NEGATIVE == 1
native_target("bmi2")
std::uint32_t rejected(std::uint32_t a) { return native::rorx<7>(a); }
#endif

#if NATIVE_SCALAR_NEGATIVE == 2
native_target("popcnt")
std::uint32_t rejected(std::uint32_t a) { return native::popcnt(a); }
#endif

#if NATIVE_SCALAR_NEGATIVE == 3
native_target("lzcnt")
std::uint32_t rejected(std::uint32_t a) { return native::lzcnt(a); }
#endif

#if NATIVE_SCALAR_NEGATIVE == 4
native_target("crc32")
std::uint32_t rejected(std::uint32_t a) { return native::crc32c(a, a); }
#endif

#if NATIVE_SCALAR_NEGATIVE == 5
native_target("f16c")
std::uint16_t rejected(float a) { return native::cvtss_sh<0>(a); }
#endif

#if NATIVE_SCALAR_NEGATIVE == 6
native_target("no-bmi")
std::uint32_t rejected(std::uint32_t a) { return native::andn(a, a); }
#endif

#if NATIVE_SCALAR_NEGATIVE == 7
native_target("no-bmi2")
std::uint32_t rejected(std::uint32_t a) { return native::rorx<7>(a); }
#endif

#if NATIVE_SCALAR_NEGATIVE == 8
native_target("no-popcnt")
std::uint32_t rejected(std::uint32_t a) { return native::popcnt(a); }
#endif

#if NATIVE_SCALAR_NEGATIVE == 9
native_target("no-lzcnt")
std::uint32_t rejected(std::uint32_t a) { return native::lzcnt(a); }
#endif

#if NATIVE_SCALAR_NEGATIVE == 10
native_target("no-crc32")
std::uint32_t rejected(std::uint32_t a) { return native::crc32c(a, a); }
#endif

#if NATIVE_SCALAR_NEGATIVE == 11
native_target("no-f16c")
std::uint16_t rejected(float a) { return native::cvtss_sh<0>(a); }
#endif

#if NATIVE_SCALAR_NEGATIVE == 12
std::uint32_t rejected(std::uint32_t a) { return native::rorx<256>(a); }
#endif

#if NATIVE_SCALAR_NEGATIVE == 13
std::uint32_t rejected(std::uint32_t a, unsigned immediate) { return native::rorx<immediate>(a); }
#endif

#if NATIVE_SCALAR_NEGATIVE == 14
std::uint16_t rejected(float a) { return native::cvtss_sh<256>(a); }
#endif

#if NATIVE_SCALAR_NEGATIVE == 15
std::uint16_t rejected(float a, unsigned immediate) { return native::cvtss_sh<immediate>(a); }
#endif
