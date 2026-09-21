// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <native/attributes.h>
import native.x86.gfni;

#if NATIVE_PUBLIC_REJECT == 1
constexpr auto arch=native::target_features<native::x86>("gfni");
using vector=native::simd<std::uint8_t,16,arch>;
native_target("sse2,no-gfni") auto rejected(vector a,vector b) { return native::gf2p8mulb<arch>(a,b); }
#elif NATIVE_PUBLIC_REJECT == 2
constexpr auto arch=native::target_features<native::x86>("avx,gfni");
using vector=native::simd<std::uint8_t,32,arch>;
native_target("avx,no-gfni") auto rejected(vector a,vector b) { return native::gf2p8mulb<arch>(a,b); }
#elif NATIVE_PUBLIC_REJECT == 3
constexpr auto arch=native::target_features<native::x86>("avx512f,gfni");
using vector=native::simd<std::uint8_t,64,arch>;
native_target("avx512f,no-gfni") auto rejected(vector a,vector b) { return native::gf2p8mulb<arch>(a,b); }
#else
#error Select a public target rejection
#endif
