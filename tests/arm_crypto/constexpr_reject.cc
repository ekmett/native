// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.arm.aes;
import native.arm.pmull;
import native.arm.sha;
constexpr auto arch = native::neon;

#if NATIVE_ARM_CRYPTO_REJECT == 0
auto rejected(native::simd<std::uint8_t, 16, arch> state, native::simd<std::uint8_t, 16, arch> round_key) { return native::aese<arch>(state, round_key); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 1
auto rejected(native::simd<std::uint8_t, 16, arch> state, native::simd<std::uint8_t, 16, arch> round_key) { return native::aesd<arch>(state, round_key); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 2
auto rejected(native::simd<std::uint8_t, 16, arch> state) { return native::aesmc<arch>(state); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 3
auto rejected(native::simd<std::uint8_t, 16, arch> state) { return native::aesimc<arch>(state); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 4
auto rejected(native::simd<std::uint64_t, 2, arch> a, native::simd<std::uint64_t, 2, arch> b) { return native::pmull2<arch>(a, b); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 5
auto rejected(native::simd<std::uint32_t, 4, arch> abcd, std::uint32_t e, native::simd<std::uint32_t, 4, arch> wk) { return native::sha1c<arch>(abcd, e, wk); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 6
auto rejected(native::simd<std::uint32_t, 4, arch> abcd, std::uint32_t e, native::simd<std::uint32_t, 4, arch> wk) { return native::sha1p<arch>(abcd, e, wk); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 7
auto rejected(native::simd<std::uint32_t, 4, arch> abcd, std::uint32_t e, native::simd<std::uint32_t, 4, arch> wk) { return native::sha1m<arch>(abcd, e, wk); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 8
auto rejected(native::simd<std::uint32_t, 4, arch> w0_3, native::simd<std::uint32_t, 4, arch> w4_7, native::simd<std::uint32_t, 4, arch> w8_11) { return native::sha1su0<arch>(w0_3, w4_7, w8_11); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 9
auto rejected(native::simd<std::uint32_t, 4, arch> partial, native::simd<std::uint32_t, 4, arch> w12_15) { return native::sha1su1<arch>(partial, w12_15); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 10
auto rejected(native::simd<std::uint32_t, 4, arch> abcd, native::simd<std::uint32_t, 4, arch> efgh, native::simd<std::uint32_t, 4, arch> wk) { return native::sha256h<arch>(abcd, efgh, wk); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 11
auto rejected(native::simd<std::uint32_t, 4, arch> efgh, native::simd<std::uint32_t, 4, arch> abcd, native::simd<std::uint32_t, 4, arch> wk) { return native::sha256h2<arch>(efgh, abcd, wk); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 12
auto rejected(native::simd<std::uint32_t, 4, arch> w0_3, native::simd<std::uint32_t, 4, arch> w4_7) { return native::sha256su0<arch>(w0_3, w4_7); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 13
auto rejected(native::simd<std::uint32_t, 4, arch> partial, native::simd<std::uint32_t, 4, arch> w8_11, native::simd<std::uint32_t, 4, arch> w12_15) { return native::sha256su1<arch>(partial, w8_11, w12_15); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 14
auto rejected(native::simd<std::uint64_t, 2, arch> sum, native::simd<std::uint64_t, 2, arch> fg, native::simd<std::uint64_t, 2, arch> de) { return native::sha512h<arch>(sum, fg, de); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 15
auto rejected(native::simd<std::uint64_t, 2, arch> sum_ab, native::simd<std::uint64_t, 2, arch> c_, native::simd<std::uint64_t, 2, arch> ab) { return native::sha512h2<arch>(sum_ab, c_, ab); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 16
auto rejected(native::simd<std::uint64_t, 2, arch> w0_1, native::simd<std::uint64_t, 2, arch> w2_) { return native::sha512su0<arch>(w0_1, w2_); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 17
auto rejected(native::simd<std::uint64_t, 2, arch> partial, native::simd<std::uint64_t, 2, arch> w14_15, native::simd<std::uint64_t, 2, arch> w9_10) { return native::sha512su1<arch>(partial, w14_15, w9_10); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 18
auto rejected(native::simd<std::uint8_t, 16, arch> a, native::simd<std::uint8_t, 16, arch> b, native::simd<std::uint8_t, 16, arch> c) { return native::eor3<arch>(a, b, c); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 19
auto rejected(native::simd<std::uint16_t, 8, arch> a, native::simd<std::uint16_t, 8, arch> b, native::simd<std::uint16_t, 8, arch> c) { return native::eor3<arch>(a, b, c); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 20
auto rejected(native::simd<std::uint32_t, 4, arch> a, native::simd<std::uint32_t, 4, arch> b, native::simd<std::uint32_t, 4, arch> c) { return native::eor3<arch>(a, b, c); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 21
auto rejected(native::simd<std::uint64_t, 2, arch> a, native::simd<std::uint64_t, 2, arch> b, native::simd<std::uint64_t, 2, arch> c) { return native::eor3<arch>(a, b, c); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 22
auto rejected(native::simd<std::int8_t, 16, arch> a, native::simd<std::int8_t, 16, arch> b, native::simd<std::int8_t, 16, arch> c) { return native::eor3<arch>(a, b, c); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 23
auto rejected(native::simd<std::int16_t, 8, arch> a, native::simd<std::int16_t, 8, arch> b, native::simd<std::int16_t, 8, arch> c) { return native::eor3<arch>(a, b, c); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 24
auto rejected(native::simd<std::int32_t, 4, arch> a, native::simd<std::int32_t, 4, arch> b, native::simd<std::int32_t, 4, arch> c) { return native::eor3<arch>(a, b, c); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 25
auto rejected(native::simd<std::int64_t, 2, arch> a, native::simd<std::int64_t, 2, arch> b, native::simd<std::int64_t, 2, arch> c) { return native::eor3<arch>(a, b, c); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 26
auto rejected(native::simd<std::uint8_t, 16, arch> a, native::simd<std::uint8_t, 16, arch> b, native::simd<std::uint8_t, 16, arch> c) { return native::bcax<arch>(a, b, c); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 27
auto rejected(native::simd<std::uint16_t, 8, arch> a, native::simd<std::uint16_t, 8, arch> b, native::simd<std::uint16_t, 8, arch> c) { return native::bcax<arch>(a, b, c); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 28
auto rejected(native::simd<std::uint32_t, 4, arch> a, native::simd<std::uint32_t, 4, arch> b, native::simd<std::uint32_t, 4, arch> c) { return native::bcax<arch>(a, b, c); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 29
auto rejected(native::simd<std::uint64_t, 2, arch> a, native::simd<std::uint64_t, 2, arch> b, native::simd<std::uint64_t, 2, arch> c) { return native::bcax<arch>(a, b, c); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 30
auto rejected(native::simd<std::int8_t, 16, arch> a, native::simd<std::int8_t, 16, arch> b, native::simd<std::int8_t, 16, arch> c) { return native::bcax<arch>(a, b, c); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 31
auto rejected(native::simd<std::int16_t, 8, arch> a, native::simd<std::int16_t, 8, arch> b, native::simd<std::int16_t, 8, arch> c) { return native::bcax<arch>(a, b, c); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 32
auto rejected(native::simd<std::int32_t, 4, arch> a, native::simd<std::int32_t, 4, arch> b, native::simd<std::int32_t, 4, arch> c) { return native::bcax<arch>(a, b, c); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 33
auto rejected(native::simd<std::int64_t, 2, arch> a, native::simd<std::int64_t, 2, arch> b, native::simd<std::int64_t, 2, arch> c) { return native::bcax<arch>(a, b, c); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 34
auto rejected(native::simd<std::uint64_t, 2, arch> a, native::simd<std::uint64_t, 2, arch> b) { return native::rax1<arch>(a, b); }
#endif

#if NATIVE_ARM_CRYPTO_REJECT == 35
auto rejected(native::simd<std::uint64_t, 2, arch> a, native::simd<std::uint64_t, 2, arch> b) { return native::xar<arch, 63>(a, b); }
#endif
