// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.arm.neon;
#if defined(NATIVE_BITS_REJECT_FEATURE)
constexpr native::isa<native::arm> architecture{};
using vector = native::simd<std::uint32_t, 4, architecture>;
void rejected(vector const *value) { (void)native::clz(*value); }
#elif defined(NATIVE_BITS_REJECT_TARGET)
using vector = native::simd<std::uint32_t, 4, native::neon>;
__attribute__((target("cpu=generic,arch=armv8-a+nosimd")))
vector rejected(vector value) { return native::clz(value); }
#else
using vector = native::simd<std::uint32_t, 4, native::neon>;
vector rejected(vector value) { return native::rev16(value); }
#endif
