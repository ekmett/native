// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
// The oracle uses ACLE storage; every operation crosses the public semantic API.
namespace instruction_fixture {
  template<native::isa Arch> requires(Arch.has(native::arm_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  uint8x16_t aese(uint8x16_t state, uint8x16_t round_key) noexcept {
    auto result = native::aese<Arch>(native::simd<std::uint8_t, 16, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 16, Arch>::native_type, state)), native::simd<std::uint8_t, 16, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 16, Arch>::native_type, round_key)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint8_t, 16, Arch>>);
    return __builtin_bit_cast(uint8x16_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  uint8x16_t aesd(uint8x16_t state, uint8x16_t round_key) noexcept {
    auto result = native::aesd<Arch>(native::simd<std::uint8_t, 16, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 16, Arch>::native_type, state)), native::simd<std::uint8_t, 16, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 16, Arch>::native_type, round_key)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint8_t, 16, Arch>>);
    return __builtin_bit_cast(uint8x16_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  uint8x16_t aesmc(uint8x16_t state) noexcept {
    auto result = native::aesmc<Arch>(native::simd<std::uint8_t, 16, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 16, Arch>::native_type, state)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint8_t, 16, Arch>>);
    return __builtin_bit_cast(uint8x16_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::aes))
  native_nodiscard native_inline native_const native_target("aes")
  uint8x16_t aesimc(uint8x16_t state) noexcept {
    auto result = native::aesimc<Arch>(native::simd<std::uint8_t, 16, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 16, Arch>::native_type, state)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint8_t, 16, Arch>>);
    return __builtin_bit_cast(uint8x16_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::pmull))
  native_nodiscard native_inline native_const native_target("aes")
  poly128_t pmull(poly64_t a, poly64_t b) noexcept {
    auto result = native::pmull<Arch>(static_cast<std::uint64_t>(a), static_cast<std::uint64_t>(b));
    static_assert(std::same_as<decltype(result), native::simd<std::uint64_t, 2, Arch>>);
    return static_cast<poly128_t>(__builtin_bit_cast(uint64x2_t, result.to_native())[0]) | (static_cast<poly128_t>(__builtin_bit_cast(uint64x2_t, result.to_native())[1]) << 64);
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::pmull))
  native_nodiscard native_inline native_const native_target("aes")
  poly128_t pmull2(poly64x2_t a, poly64x2_t b) noexcept {
    auto result = native::pmull2<Arch>(native::simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint64_t, 2, Arch>::native_type, vreinterpretq_u64_p64(a))), native::simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint64_t, 2, Arch>::native_type, vreinterpretq_u64_p64(b))));
    static_assert(std::same_as<decltype(result), native::simd<std::uint64_t, 2, Arch>>);
    return static_cast<poly128_t>(__builtin_bit_cast(uint64x2_t, result.to_native())[0]) | (static_cast<poly128_t>(__builtin_bit_cast(uint64x2_t, result.to_native())[1]) << 64);
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::neon))
  native_nodiscard native_inline native_const
  poly16x8_t pmull(poly8x8_t a, poly8x8_t b) noexcept {
    auto result = native::pmull<Arch>(native::simd<std::uint8_t, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 8, Arch>::native_type, vreinterpret_u8_p8(a))), native::simd<std::uint8_t, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 8, Arch>::native_type, vreinterpret_u8_p8(b))));
    static_assert(std::same_as<decltype(result), native::simd<std::uint16_t, 8, Arch>>);
    return __builtin_bit_cast(poly16x8_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::neon))
  native_nodiscard native_inline native_const
  poly16x8_t pmull2(poly8x16_t a, poly8x16_t b) noexcept {
    auto result = native::pmull2<Arch>(native::simd<std::uint8_t, 16, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 16, Arch>::native_type, vreinterpretq_u8_p8(a))), native::simd<std::uint8_t, 16, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 16, Arch>::native_type, vreinterpretq_u8_p8(b))));
    static_assert(std::same_as<decltype(result), native::simd<std::uint16_t, 8, Arch>>);
    return __builtin_bit_cast(poly16x8_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha1))
  native_nodiscard native_inline native_const native_target("sha2")
  uint32x4_t sha1c(uint32x4_t abcd, std::uint32_t e, uint32x4_t wk) noexcept {
    auto result = native::sha1c<Arch>(native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, abcd)), e, native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, wk)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint32_t, 4, Arch>>);
    return __builtin_bit_cast(uint32x4_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha1))
  native_nodiscard native_inline native_const native_target("sha2")
  uint32x4_t sha1p(uint32x4_t abcd, std::uint32_t e, uint32x4_t wk) noexcept {
    auto result = native::sha1p<Arch>(native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, abcd)), e, native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, wk)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint32_t, 4, Arch>>);
    return __builtin_bit_cast(uint32x4_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha1))
  native_nodiscard native_inline native_const native_target("sha2")
  uint32x4_t sha1m(uint32x4_t abcd, std::uint32_t e, uint32x4_t wk) noexcept {
    auto result = native::sha1m<Arch>(native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, abcd)), e, native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, wk)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint32_t, 4, Arch>>);
    return __builtin_bit_cast(uint32x4_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha1))
  native_nodiscard native_inline native_const native_target("sha2")
  std::uint32_t sha1h(std::uint32_t a) noexcept {
    auto result = native::sha1h<Arch>(a);
    static_assert(std::same_as<decltype(result), std::uint32_t>);
    return result;
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha1))
  native_nodiscard native_inline native_const native_target("sha2")
  uint32x4_t sha1su0(uint32x4_t w0_3, uint32x4_t w4_7, uint32x4_t w8_11) noexcept {
    auto result = native::sha1su0<Arch>(native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, w0_3)), native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, w4_7)), native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, w8_11)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint32_t, 4, Arch>>);
    return __builtin_bit_cast(uint32x4_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha1))
  native_nodiscard native_inline native_const native_target("sha2")
  uint32x4_t sha1su1(uint32x4_t partial, uint32x4_t w12_15) noexcept {
    auto result = native::sha1su1<Arch>(native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, partial)), native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, w12_15)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint32_t, 4, Arch>>);
    return __builtin_bit_cast(uint32x4_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha2))
  native_nodiscard native_inline native_const native_target("sha2")
  uint32x4_t sha256h(uint32x4_t abcd, uint32x4_t efgh, uint32x4_t wk) noexcept {
    auto result = native::sha256h<Arch>(native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, abcd)), native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, efgh)), native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, wk)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint32_t, 4, Arch>>);
    return __builtin_bit_cast(uint32x4_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha2))
  native_nodiscard native_inline native_const native_target("sha2")
  uint32x4_t sha256h2(uint32x4_t efgh, uint32x4_t abcd, uint32x4_t wk) noexcept {
    auto result = native::sha256h2<Arch>(native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, efgh)), native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, abcd)), native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, wk)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint32_t, 4, Arch>>);
    return __builtin_bit_cast(uint32x4_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha2))
  native_nodiscard native_inline native_const native_target("sha2")
  uint32x4_t sha256su0(uint32x4_t w0_3, uint32x4_t w4_7) noexcept {
    auto result = native::sha256su0<Arch>(native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, w0_3)), native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, w4_7)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint32_t, 4, Arch>>);
    return __builtin_bit_cast(uint32x4_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha2))
  native_nodiscard native_inline native_const native_target("sha2")
  uint32x4_t sha256su1(uint32x4_t partial, uint32x4_t w8_11, uint32x4_t w12_15) noexcept {
    auto result = native::sha256su1<Arch>(native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, partial)), native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, w8_11)), native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, w12_15)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint32_t, 4, Arch>>);
    return __builtin_bit_cast(uint32x4_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha512))
  native_nodiscard native_inline native_const native_target("sha3")
  uint64x2_t sha512h(uint64x2_t sum, uint64x2_t fg, uint64x2_t de) noexcept {
    auto result = native::sha512h<Arch>(native::simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint64_t, 2, Arch>::native_type, sum)), native::simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint64_t, 2, Arch>::native_type, fg)), native::simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint64_t, 2, Arch>::native_type, de)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint64_t, 2, Arch>>);
    return __builtin_bit_cast(uint64x2_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha512))
  native_nodiscard native_inline native_const native_target("sha3")
  uint64x2_t sha512h2(uint64x2_t sum_ab, uint64x2_t c_, uint64x2_t ab) noexcept {
    auto result = native::sha512h2<Arch>(native::simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint64_t, 2, Arch>::native_type, sum_ab)), native::simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint64_t, 2, Arch>::native_type, c_)), native::simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint64_t, 2, Arch>::native_type, ab)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint64_t, 2, Arch>>);
    return __builtin_bit_cast(uint64x2_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha512))
  native_nodiscard native_inline native_const native_target("sha3")
  uint64x2_t sha512su0(uint64x2_t w0_1, uint64x2_t w2_) noexcept {
    auto result = native::sha512su0<Arch>(native::simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint64_t, 2, Arch>::native_type, w0_1)), native::simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint64_t, 2, Arch>::native_type, w2_)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint64_t, 2, Arch>>);
    return __builtin_bit_cast(uint64x2_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha512))
  native_nodiscard native_inline native_const native_target("sha3")
  uint64x2_t sha512su1(uint64x2_t partial, uint64x2_t w14_15, uint64x2_t w9_10) noexcept {
    auto result = native::sha512su1<Arch>(native::simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint64_t, 2, Arch>::native_type, partial)), native::simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint64_t, 2, Arch>::native_type, w14_15)), native::simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint64_t, 2, Arch>::native_type, w9_10)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint64_t, 2, Arch>>);
    return __builtin_bit_cast(uint64x2_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  uint8x16_t eor3(uint8x16_t a, uint8x16_t b, uint8x16_t c) noexcept {
    auto result = native::eor3<Arch>(native::simd<std::uint8_t, 16, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 16, Arch>::native_type, a)), native::simd<std::uint8_t, 16, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 16, Arch>::native_type, b)), native::simd<std::uint8_t, 16, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 16, Arch>::native_type, c)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint8_t, 16, Arch>>);
    return __builtin_bit_cast(uint8x16_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  uint16x8_t eor3(uint16x8_t a, uint16x8_t b, uint16x8_t c) noexcept {
    auto result = native::eor3<Arch>(native::simd<std::uint16_t, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint16_t, 8, Arch>::native_type, a)), native::simd<std::uint16_t, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint16_t, 8, Arch>::native_type, b)), native::simd<std::uint16_t, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint16_t, 8, Arch>::native_type, c)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint16_t, 8, Arch>>);
    return __builtin_bit_cast(uint16x8_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  uint32x4_t eor3(uint32x4_t a, uint32x4_t b, uint32x4_t c) noexcept {
    auto result = native::eor3<Arch>(native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, a)), native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, b)), native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, c)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint32_t, 4, Arch>>);
    return __builtin_bit_cast(uint32x4_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  uint64x2_t eor3(uint64x2_t a, uint64x2_t b, uint64x2_t c) noexcept {
    auto result = native::eor3<Arch>(native::simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint64_t, 2, Arch>::native_type, a)), native::simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint64_t, 2, Arch>::native_type, b)), native::simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint64_t, 2, Arch>::native_type, c)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint64_t, 2, Arch>>);
    return __builtin_bit_cast(uint64x2_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  int8x16_t eor3(int8x16_t a, int8x16_t b, int8x16_t c) noexcept {
    auto result = native::eor3<Arch>(native::simd<std::int8_t, 16, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::int8_t, 16, Arch>::native_type, a)), native::simd<std::int8_t, 16, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::int8_t, 16, Arch>::native_type, b)), native::simd<std::int8_t, 16, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::int8_t, 16, Arch>::native_type, c)));
    static_assert(std::same_as<decltype(result), native::simd<std::int8_t, 16, Arch>>);
    return __builtin_bit_cast(int8x16_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  int16x8_t eor3(int16x8_t a, int16x8_t b, int16x8_t c) noexcept {
    auto result = native::eor3<Arch>(native::simd<std::int16_t, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, Arch>::native_type, a)), native::simd<std::int16_t, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, Arch>::native_type, b)), native::simd<std::int16_t, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, Arch>::native_type, c)));
    static_assert(std::same_as<decltype(result), native::simd<std::int16_t, 8, Arch>>);
    return __builtin_bit_cast(int16x8_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  int32x4_t eor3(int32x4_t a, int32x4_t b, int32x4_t c) noexcept {
    auto result = native::eor3<Arch>(native::simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, Arch>::native_type, a)), native::simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, Arch>::native_type, b)), native::simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, Arch>::native_type, c)));
    static_assert(std::same_as<decltype(result), native::simd<std::int32_t, 4, Arch>>);
    return __builtin_bit_cast(int32x4_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  int64x2_t eor3(int64x2_t a, int64x2_t b, int64x2_t c) noexcept {
    auto result = native::eor3<Arch>(native::simd<std::int64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::int64_t, 2, Arch>::native_type, a)), native::simd<std::int64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::int64_t, 2, Arch>::native_type, b)), native::simd<std::int64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::int64_t, 2, Arch>::native_type, c)));
    static_assert(std::same_as<decltype(result), native::simd<std::int64_t, 2, Arch>>);
    return __builtin_bit_cast(int64x2_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  uint8x16_t bcax(uint8x16_t a, uint8x16_t b, uint8x16_t c) noexcept {
    auto result = native::bcax<Arch>(native::simd<std::uint8_t, 16, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 16, Arch>::native_type, a)), native::simd<std::uint8_t, 16, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 16, Arch>::native_type, b)), native::simd<std::uint8_t, 16, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint8_t, 16, Arch>::native_type, c)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint8_t, 16, Arch>>);
    return __builtin_bit_cast(uint8x16_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  uint16x8_t bcax(uint16x8_t a, uint16x8_t b, uint16x8_t c) noexcept {
    auto result = native::bcax<Arch>(native::simd<std::uint16_t, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint16_t, 8, Arch>::native_type, a)), native::simd<std::uint16_t, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint16_t, 8, Arch>::native_type, b)), native::simd<std::uint16_t, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint16_t, 8, Arch>::native_type, c)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint16_t, 8, Arch>>);
    return __builtin_bit_cast(uint16x8_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  uint32x4_t bcax(uint32x4_t a, uint32x4_t b, uint32x4_t c) noexcept {
    auto result = native::bcax<Arch>(native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, a)), native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, b)), native::simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint32_t, 4, Arch>::native_type, c)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint32_t, 4, Arch>>);
    return __builtin_bit_cast(uint32x4_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  uint64x2_t bcax(uint64x2_t a, uint64x2_t b, uint64x2_t c) noexcept {
    auto result = native::bcax<Arch>(native::simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint64_t, 2, Arch>::native_type, a)), native::simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint64_t, 2, Arch>::native_type, b)), native::simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint64_t, 2, Arch>::native_type, c)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint64_t, 2, Arch>>);
    return __builtin_bit_cast(uint64x2_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  int8x16_t bcax(int8x16_t a, int8x16_t b, int8x16_t c) noexcept {
    auto result = native::bcax<Arch>(native::simd<std::int8_t, 16, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::int8_t, 16, Arch>::native_type, a)), native::simd<std::int8_t, 16, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::int8_t, 16, Arch>::native_type, b)), native::simd<std::int8_t, 16, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::int8_t, 16, Arch>::native_type, c)));
    static_assert(std::same_as<decltype(result), native::simd<std::int8_t, 16, Arch>>);
    return __builtin_bit_cast(int8x16_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  int16x8_t bcax(int16x8_t a, int16x8_t b, int16x8_t c) noexcept {
    auto result = native::bcax<Arch>(native::simd<std::int16_t, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, Arch>::native_type, a)), native::simd<std::int16_t, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, Arch>::native_type, b)), native::simd<std::int16_t, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::int16_t, 8, Arch>::native_type, c)));
    static_assert(std::same_as<decltype(result), native::simd<std::int16_t, 8, Arch>>);
    return __builtin_bit_cast(int16x8_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  int32x4_t bcax(int32x4_t a, int32x4_t b, int32x4_t c) noexcept {
    auto result = native::bcax<Arch>(native::simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, Arch>::native_type, a)), native::simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, Arch>::native_type, b)), native::simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::int32_t, 4, Arch>::native_type, c)));
    static_assert(std::same_as<decltype(result), native::simd<std::int32_t, 4, Arch>>);
    return __builtin_bit_cast(int32x4_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  int64x2_t bcax(int64x2_t a, int64x2_t b, int64x2_t c) noexcept {
    auto result = native::bcax<Arch>(native::simd<std::int64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::int64_t, 2, Arch>::native_type, a)), native::simd<std::int64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::int64_t, 2, Arch>::native_type, b)), native::simd<std::int64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::int64_t, 2, Arch>::native_type, c)));
    static_assert(std::same_as<decltype(result), native::simd<std::int64_t, 2, Arch>>);
    return __builtin_bit_cast(int64x2_t, result.to_native());
  }
  template<native::isa Arch> requires(Arch.has(native::arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  uint64x2_t rax1(uint64x2_t a, uint64x2_t b) noexcept {
    auto result = native::rax1<Arch>(native::simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint64_t, 2, Arch>::native_type, a)), native::simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint64_t, 2, Arch>::native_type, b)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint64_t, 2, Arch>>);
    return __builtin_bit_cast(uint64x2_t, result.to_native());
  }
  template<native::isa Arch, unsigned Rotate> requires(Arch.has(native::arm_feature::sha3) && Rotate < 64)
  native_nodiscard native_inline native_const native_target("sha3")
  uint64x2_t xar(uint64x2_t a, uint64x2_t b) noexcept {
    auto result = native::xar<Arch, Rotate>(native::simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint64_t, 2, Arch>::native_type, a)), native::simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename native::simd<std::uint64_t, 2, Arch>::native_type, b)));
    static_assert(std::same_as<decltype(result), native::simd<std::uint64_t, 2, Arch>>);
    return __builtin_bit_cast(uint64x2_t, result.to_native());
  }
}
