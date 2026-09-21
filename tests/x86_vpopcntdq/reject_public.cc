// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <native/attributes.h>
import native.x86.vpopcntdq;

#if NATIVE_PUBLIC_REJECT == 1
constexpr auto arch=native::target_features<native::x86>("avx512f,avx512vl,avx512vpopcntdq");
using vector=native::simd<std::uint32_t,4,arch>;
native_target("avx512f,avx512vl,no-avx512vpopcntdq") auto rejected(vector a) { return native::vpopcntd<arch>(a); }
#elif NATIVE_PUBLIC_REJECT == 2
constexpr auto arch=native::target_features<native::x86>("avx512f,avx512vl,avx512vpopcntdq");
using vector=native::simd<std::uint32_t,8,arch>;
native_target("avx512f,avx512vl,no-avx512vpopcntdq") auto rejected(vector a) { return native::vpopcntd<arch>(a); }
#elif NATIVE_PUBLIC_REJECT == 3
constexpr auto arch=native::target_features<native::x86>("avx512f,avx512vpopcntdq");
using vector=native::simd<std::uint32_t,16,arch>;
native_target("avx512f,no-avx512vpopcntdq") auto rejected(vector a) { return native::vpopcntd<arch>(a); }
#else
#error Select a public target rejection
#endif
