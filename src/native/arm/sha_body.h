// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#if NATIVE_HOST_NEON || defined(NATIVE_DOXYGEN)
export namespace native {
  /// \defgroup arm_sha SHA instruction helpers
  /// SHA-1, SHA-256 and SHA-512 state/schedule operations and SHA-3 Boolean
  /// helpers. These are individual instructions, not complete hash functions.
  /// Lane zero contains the first named word. Inputs contain native integer
  /// words: callers perform message byte-order conversion and supply constants.
  /// Arch names the instruction feature. SHA-1/SHA-256 use compiler "sha2";
  /// SHA-512/SHA-3 use "sha3". Admit that complete target_features set before
  /// entering the target leaf; compiler bundles include sibling features.
  /// These integer operations do not read or modify FPCR, FPSR or NZCV.
  /// \{

  /// Perform four SHA-1 rounds using choice; wk already includes the round constants.
  template<isa Arch> requires(Arch.has(arm_feature::sha1))
  native_nodiscard native_inline native_const native_target("sha2")
  simd<std::uint32_t, 4, Arch> sha1c(simd<std::uint32_t, 4, Arch> abcd, std::uint32_t e, simd<std::uint32_t, 4, Arch> wk) noexcept {
    auto result = detail::arm_sha::sha1c<Arch>(__builtin_bit_cast(uint32x4_t, abcd.to_native()), e, __builtin_bit_cast(uint32x4_t, wk.to_native()));
    return simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint32_t, 4, Arch>::native_type, result));
  }

  /// Perform four SHA-1 rounds using parity; wk already includes the round constants.
  template<isa Arch> requires(Arch.has(arm_feature::sha1))
  native_nodiscard native_inline native_const native_target("sha2")
  simd<std::uint32_t, 4, Arch> sha1p(simd<std::uint32_t, 4, Arch> abcd, std::uint32_t e, simd<std::uint32_t, 4, Arch> wk) noexcept {
    auto result = detail::arm_sha::sha1p<Arch>(__builtin_bit_cast(uint32x4_t, abcd.to_native()), e, __builtin_bit_cast(uint32x4_t, wk.to_native()));
    return simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint32_t, 4, Arch>::native_type, result));
  }

  /// Perform four SHA-1 rounds using majority; wk already includes the round constants.
  template<isa Arch> requires(Arch.has(arm_feature::sha1))
  native_nodiscard native_inline native_const native_target("sha2")
  simd<std::uint32_t, 4, Arch> sha1m(simd<std::uint32_t, 4, Arch> abcd, std::uint32_t e, simd<std::uint32_t, 4, Arch> wk) noexcept {
    auto result = detail::arm_sha::sha1m<Arch>(__builtin_bit_cast(uint32x4_t, abcd.to_native()), e, __builtin_bit_cast(uint32x4_t, wk.to_native()));
    return simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint32_t, 4, Arch>::native_type, result));
  }

  /// Rotate a SHA-1 state word right by two bits; Arch defaults to the module baseline.
  template<isa Arch=NATIVE_BASELINE> requires(Arch.has(arm_feature::sha1))
  native_nodiscard native_inline native_const native_target("sha2")
  constexpr std::uint32_t sha1h(std::uint32_t a) noexcept {
    if consteval { return (a>>2)|(a<<30); }
    else { return detail::arm_sha::sha1h<Arch>(a); }
  }

  /// Evaluate the SHA-1 rotation at compile time without requiring SHA instructions.
  template<isa Arch=NATIVE_BASELINE> requires(!Arch.has(arm_feature::sha1))
  native_nodiscard consteval std::uint32_t sha1h(std::uint32_t a) noexcept {
    return (a>>2)|(a<<30);
  }

  /// Compute the first part of the SHA-1 schedule update for four consecutive words.
  template<isa Arch> requires(Arch.has(arm_feature::sha1))
  native_nodiscard native_inline native_const native_target("sha2")
  simd<std::uint32_t, 4, Arch> sha1su0(simd<std::uint32_t, 4, Arch> w0_3, simd<std::uint32_t, 4, Arch> w4_7, simd<std::uint32_t, 4, Arch> w8_11) noexcept {
    auto result = detail::arm_sha::sha1su0<Arch>(__builtin_bit_cast(uint32x4_t, w0_3.to_native()), __builtin_bit_cast(uint32x4_t, w4_7.to_native()), __builtin_bit_cast(uint32x4_t, w8_11.to_native()));
    return simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint32_t, 4, Arch>::native_type, result));
  }

  /// Finish the four-word SHA-1 schedule update from sha1su0 and words 12 through 15.
  template<isa Arch> requires(Arch.has(arm_feature::sha1))
  native_nodiscard native_inline native_const native_target("sha2")
  simd<std::uint32_t, 4, Arch> sha1su1(simd<std::uint32_t, 4, Arch> partial, simd<std::uint32_t, 4, Arch> w12_15) noexcept {
    auto result = detail::arm_sha::sha1su1<Arch>(__builtin_bit_cast(uint32x4_t, partial.to_native()), __builtin_bit_cast(uint32x4_t, w12_15.to_native()));
    return simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint32_t, 4, Arch>::native_type, result));
  }

  /// Return a,b,c,d after four SHA-256 rounds; wk includes round constants.
  template<isa Arch> requires(Arch.has(arm_feature::sha2))
  native_nodiscard native_inline native_const native_target("sha2")
  simd<std::uint32_t, 4, Arch> sha256h(simd<std::uint32_t, 4, Arch> abcd, simd<std::uint32_t, 4, Arch> efgh, simd<std::uint32_t, 4, Arch> wk) noexcept {
    auto result = detail::arm_sha::sha256h<Arch>(__builtin_bit_cast(uint32x4_t, abcd.to_native()), __builtin_bit_cast(uint32x4_t, efgh.to_native()), __builtin_bit_cast(uint32x4_t, wk.to_native()));
    return simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint32_t, 4, Arch>::native_type, result));
  }

  /// Return e,f,g,h after four SHA-256 rounds; abcd is the state before those rounds.
  template<isa Arch> requires(Arch.has(arm_feature::sha2))
  native_nodiscard native_inline native_const native_target("sha2")
  simd<std::uint32_t, 4, Arch> sha256h2(simd<std::uint32_t, 4, Arch> efgh, simd<std::uint32_t, 4, Arch> abcd, simd<std::uint32_t, 4, Arch> wk) noexcept {
    auto result = detail::arm_sha::sha256h2<Arch>(__builtin_bit_cast(uint32x4_t, efgh.to_native()), __builtin_bit_cast(uint32x4_t, abcd.to_native()), __builtin_bit_cast(uint32x4_t, wk.to_native()));
    return simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint32_t, 4, Arch>::native_type, result));
  }

  /// Compute the first part of a four-word SHA-256 schedule update.
  template<isa Arch> requires(Arch.has(arm_feature::sha2))
  native_nodiscard native_inline native_const native_target("sha2")
  simd<std::uint32_t, 4, Arch> sha256su0(simd<std::uint32_t, 4, Arch> w0_3, simd<std::uint32_t, 4, Arch> w4_7) noexcept {
    auto result = detail::arm_sha::sha256su0<Arch>(__builtin_bit_cast(uint32x4_t, w0_3.to_native()), __builtin_bit_cast(uint32x4_t, w4_7.to_native()));
    return simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint32_t, 4, Arch>::native_type, result));
  }

  /// Finish the four-word SHA-256 schedule update from sha256su0.
  template<isa Arch> requires(Arch.has(arm_feature::sha2))
  native_nodiscard native_inline native_const native_target("sha2")
  simd<std::uint32_t, 4, Arch> sha256su1(simd<std::uint32_t, 4, Arch> partial, simd<std::uint32_t, 4, Arch> w8_11, simd<std::uint32_t, 4, Arch> w12_15) noexcept {
    auto result = detail::arm_sha::sha256su1<Arch>(__builtin_bit_cast(uint32x4_t, partial.to_native()), __builtin_bit_cast(uint32x4_t, w8_11.to_native()), __builtin_bit_cast(uint32x4_t, w12_15.to_native()));
    return simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint32_t, 4, Arch>::native_type, result));
  }

  /// Apply SHA512H to prepared round sums and packed f,g and d,e state.
  template<isa Arch> requires(Arch.has(arm_feature::sha512))
  native_nodiscard native_inline native_const native_target("sha3")
  simd<std::uint64_t, 2, Arch> sha512h(simd<std::uint64_t, 2, Arch> sum, simd<std::uint64_t, 2, Arch> fg, simd<std::uint64_t, 2, Arch> de) noexcept {
    auto result = detail::arm_sha::sha512h<Arch>(__builtin_bit_cast(uint64x2_t, sum.to_native()), __builtin_bit_cast(uint64x2_t, fg.to_native()), __builtin_bit_cast(uint64x2_t, de.to_native()));
    return simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint64_t, 2, Arch>::native_type, result));
  }

  /// Apply SHA512H2 to prepared sums, c, and the packed a,b state.
  template<isa Arch> requires(Arch.has(arm_feature::sha512))
  native_nodiscard native_inline native_const native_target("sha3")
  simd<std::uint64_t, 2, Arch> sha512h2(simd<std::uint64_t, 2, Arch> sum_ab, simd<std::uint64_t, 2, Arch> c_, simd<std::uint64_t, 2, Arch> ab) noexcept {
    auto result = detail::arm_sha::sha512h2<Arch>(__builtin_bit_cast(uint64x2_t, sum_ab.to_native()), __builtin_bit_cast(uint64x2_t, c_.to_native()), __builtin_bit_cast(uint64x2_t, ab.to_native()));
    return simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint64_t, 2, Arch>::native_type, result));
  }

  /// Compute the first part of a two-word SHA-512 schedule update.
  template<isa Arch> requires(Arch.has(arm_feature::sha512))
  native_nodiscard native_inline native_const native_target("sha3")
  simd<std::uint64_t, 2, Arch> sha512su0(simd<std::uint64_t, 2, Arch> w0_1, simd<std::uint64_t, 2, Arch> w2_) noexcept {
    auto result = detail::arm_sha::sha512su0<Arch>(__builtin_bit_cast(uint64x2_t, w0_1.to_native()), __builtin_bit_cast(uint64x2_t, w2_.to_native()));
    return simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint64_t, 2, Arch>::native_type, result));
  }

  /// Finish the two-word SHA-512 schedule update from sha512su0.
  template<isa Arch> requires(Arch.has(arm_feature::sha512))
  native_nodiscard native_inline native_const native_target("sha3")
  simd<std::uint64_t, 2, Arch> sha512su1(simd<std::uint64_t, 2, Arch> partial, simd<std::uint64_t, 2, Arch> w14_15, simd<std::uint64_t, 2, Arch> w9_10) noexcept {
    auto result = detail::arm_sha::sha512su1<Arch>(__builtin_bit_cast(uint64x2_t, partial.to_native()), __builtin_bit_cast(uint64x2_t, w14_15.to_native()), __builtin_bit_cast(uint64x2_t, w9_10.to_native()));
    return simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint64_t, 2, Arch>::native_type, result));
  }

  /// Return a XOR b XOR c for every bit.
  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  simd<std::uint8_t, 16, Arch> eor3(simd<std::uint8_t, 16, Arch> a, simd<std::uint8_t, 16, Arch> b, simd<std::uint8_t, 16, Arch> c) noexcept {
    auto result = detail::arm_sha::eor3<Arch>(__builtin_bit_cast(uint8x16_t, a.to_native()), __builtin_bit_cast(uint8x16_t, b.to_native()), __builtin_bit_cast(uint8x16_t, c.to_native()));
    return simd<std::uint8_t, 16, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint8_t, 16, Arch>::native_type, result));
  }

  /// Return a XOR b XOR c for every bit.
  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  simd<std::uint16_t, 8, Arch> eor3(simd<std::uint16_t, 8, Arch> a, simd<std::uint16_t, 8, Arch> b, simd<std::uint16_t, 8, Arch> c) noexcept {
    auto result = detail::arm_sha::eor3<Arch>(__builtin_bit_cast(uint16x8_t, a.to_native()), __builtin_bit_cast(uint16x8_t, b.to_native()), __builtin_bit_cast(uint16x8_t, c.to_native()));
    return simd<std::uint16_t, 8, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint16_t, 8, Arch>::native_type, result));
  }

  /// Return a XOR b XOR c for every bit.
  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  simd<std::uint32_t, 4, Arch> eor3(simd<std::uint32_t, 4, Arch> a, simd<std::uint32_t, 4, Arch> b, simd<std::uint32_t, 4, Arch> c) noexcept {
    auto result = detail::arm_sha::eor3<Arch>(__builtin_bit_cast(uint32x4_t, a.to_native()), __builtin_bit_cast(uint32x4_t, b.to_native()), __builtin_bit_cast(uint32x4_t, c.to_native()));
    return simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint32_t, 4, Arch>::native_type, result));
  }

  /// Return a XOR b XOR c for every bit.
  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  simd<std::uint64_t, 2, Arch> eor3(simd<std::uint64_t, 2, Arch> a, simd<std::uint64_t, 2, Arch> b, simd<std::uint64_t, 2, Arch> c) noexcept {
    auto result = detail::arm_sha::eor3<Arch>(__builtin_bit_cast(uint64x2_t, a.to_native()), __builtin_bit_cast(uint64x2_t, b.to_native()), __builtin_bit_cast(uint64x2_t, c.to_native()));
    return simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint64_t, 2, Arch>::native_type, result));
  }

  /// Return a XOR b XOR c for every bit.
  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  simd<std::int8_t, 16, Arch> eor3(simd<std::int8_t, 16, Arch> a, simd<std::int8_t, 16, Arch> b, simd<std::int8_t, 16, Arch> c) noexcept {
    auto result = detail::arm_sha::eor3<Arch>(__builtin_bit_cast(int8x16_t, a.to_native()), __builtin_bit_cast(int8x16_t, b.to_native()), __builtin_bit_cast(int8x16_t, c.to_native()));
    return simd<std::int8_t, 16, Arch>::from_native(__builtin_bit_cast(typename simd<std::int8_t, 16, Arch>::native_type, result));
  }

  /// Return a XOR b XOR c for every bit.
  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  simd<std::int16_t, 8, Arch> eor3(simd<std::int16_t, 8, Arch> a, simd<std::int16_t, 8, Arch> b, simd<std::int16_t, 8, Arch> c) noexcept {
    auto result = detail::arm_sha::eor3<Arch>(__builtin_bit_cast(int16x8_t, a.to_native()), __builtin_bit_cast(int16x8_t, b.to_native()), __builtin_bit_cast(int16x8_t, c.to_native()));
    return simd<std::int16_t, 8, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 8, Arch>::native_type, result));
  }

  /// Return a XOR b XOR c for every bit.
  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  simd<std::int32_t, 4, Arch> eor3(simd<std::int32_t, 4, Arch> a, simd<std::int32_t, 4, Arch> b, simd<std::int32_t, 4, Arch> c) noexcept {
    auto result = detail::arm_sha::eor3<Arch>(__builtin_bit_cast(int32x4_t, a.to_native()), __builtin_bit_cast(int32x4_t, b.to_native()), __builtin_bit_cast(int32x4_t, c.to_native()));
    return simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 4, Arch>::native_type, result));
  }

  /// Return a XOR b XOR c for every bit.
  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  simd<std::int64_t, 2, Arch> eor3(simd<std::int64_t, 2, Arch> a, simd<std::int64_t, 2, Arch> b, simd<std::int64_t, 2, Arch> c) noexcept {
    auto result = detail::arm_sha::eor3<Arch>(__builtin_bit_cast(int64x2_t, a.to_native()), __builtin_bit_cast(int64x2_t, b.to_native()), __builtin_bit_cast(int64x2_t, c.to_native()));
    return simd<std::int64_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::int64_t, 2, Arch>::native_type, result));
  }

  /// Return a XOR (b AND NOT c) for every bit.
  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  simd<std::uint8_t, 16, Arch> bcax(simd<std::uint8_t, 16, Arch> a, simd<std::uint8_t, 16, Arch> b, simd<std::uint8_t, 16, Arch> c) noexcept {
    auto result = detail::arm_sha::bcax<Arch>(__builtin_bit_cast(uint8x16_t, a.to_native()), __builtin_bit_cast(uint8x16_t, b.to_native()), __builtin_bit_cast(uint8x16_t, c.to_native()));
    return simd<std::uint8_t, 16, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint8_t, 16, Arch>::native_type, result));
  }

  /// Return a XOR (b AND NOT c) for every bit.
  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  simd<std::uint16_t, 8, Arch> bcax(simd<std::uint16_t, 8, Arch> a, simd<std::uint16_t, 8, Arch> b, simd<std::uint16_t, 8, Arch> c) noexcept {
    auto result = detail::arm_sha::bcax<Arch>(__builtin_bit_cast(uint16x8_t, a.to_native()), __builtin_bit_cast(uint16x8_t, b.to_native()), __builtin_bit_cast(uint16x8_t, c.to_native()));
    return simd<std::uint16_t, 8, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint16_t, 8, Arch>::native_type, result));
  }

  /// Return a XOR (b AND NOT c) for every bit.
  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  simd<std::uint32_t, 4, Arch> bcax(simd<std::uint32_t, 4, Arch> a, simd<std::uint32_t, 4, Arch> b, simd<std::uint32_t, 4, Arch> c) noexcept {
    auto result = detail::arm_sha::bcax<Arch>(__builtin_bit_cast(uint32x4_t, a.to_native()), __builtin_bit_cast(uint32x4_t, b.to_native()), __builtin_bit_cast(uint32x4_t, c.to_native()));
    return simd<std::uint32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint32_t, 4, Arch>::native_type, result));
  }

  /// Return a XOR (b AND NOT c) for every bit.
  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  simd<std::uint64_t, 2, Arch> bcax(simd<std::uint64_t, 2, Arch> a, simd<std::uint64_t, 2, Arch> b, simd<std::uint64_t, 2, Arch> c) noexcept {
    auto result = detail::arm_sha::bcax<Arch>(__builtin_bit_cast(uint64x2_t, a.to_native()), __builtin_bit_cast(uint64x2_t, b.to_native()), __builtin_bit_cast(uint64x2_t, c.to_native()));
    return simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint64_t, 2, Arch>::native_type, result));
  }

  /// Return a XOR (b AND NOT c) for every bit.
  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  simd<std::int8_t, 16, Arch> bcax(simd<std::int8_t, 16, Arch> a, simd<std::int8_t, 16, Arch> b, simd<std::int8_t, 16, Arch> c) noexcept {
    auto result = detail::arm_sha::bcax<Arch>(__builtin_bit_cast(int8x16_t, a.to_native()), __builtin_bit_cast(int8x16_t, b.to_native()), __builtin_bit_cast(int8x16_t, c.to_native()));
    return simd<std::int8_t, 16, Arch>::from_native(__builtin_bit_cast(typename simd<std::int8_t, 16, Arch>::native_type, result));
  }

  /// Return a XOR (b AND NOT c) for every bit.
  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  simd<std::int16_t, 8, Arch> bcax(simd<std::int16_t, 8, Arch> a, simd<std::int16_t, 8, Arch> b, simd<std::int16_t, 8, Arch> c) noexcept {
    auto result = detail::arm_sha::bcax<Arch>(__builtin_bit_cast(int16x8_t, a.to_native()), __builtin_bit_cast(int16x8_t, b.to_native()), __builtin_bit_cast(int16x8_t, c.to_native()));
    return simd<std::int16_t, 8, Arch>::from_native(__builtin_bit_cast(typename simd<std::int16_t, 8, Arch>::native_type, result));
  }

  /// Return a XOR (b AND NOT c) for every bit.
  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  simd<std::int32_t, 4, Arch> bcax(simd<std::int32_t, 4, Arch> a, simd<std::int32_t, 4, Arch> b, simd<std::int32_t, 4, Arch> c) noexcept {
    auto result = detail::arm_sha::bcax<Arch>(__builtin_bit_cast(int32x4_t, a.to_native()), __builtin_bit_cast(int32x4_t, b.to_native()), __builtin_bit_cast(int32x4_t, c.to_native()));
    return simd<std::int32_t, 4, Arch>::from_native(__builtin_bit_cast(typename simd<std::int32_t, 4, Arch>::native_type, result));
  }

  /// Return a XOR (b AND NOT c) for every bit.
  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  simd<std::int64_t, 2, Arch> bcax(simd<std::int64_t, 2, Arch> a, simd<std::int64_t, 2, Arch> b, simd<std::int64_t, 2, Arch> c) noexcept {
    auto result = detail::arm_sha::bcax<Arch>(__builtin_bit_cast(int64x2_t, a.to_native()), __builtin_bit_cast(int64x2_t, b.to_native()), __builtin_bit_cast(int64x2_t, c.to_native()));
    return simd<std::int64_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::int64_t, 2, Arch>::native_type, result));
  }

  /// Return a XOR rotate_left(b, 1) in each 64-bit lane.
  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  simd<std::uint64_t, 2, Arch> rax1(simd<std::uint64_t, 2, Arch> a, simd<std::uint64_t, 2, Arch> b) noexcept {
    auto result = detail::arm_sha::rax1<Arch>(__builtin_bit_cast(uint64x2_t, a.to_native()), __builtin_bit_cast(uint64x2_t, b.to_native()));
    return simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint64_t, 2, Arch>::native_type, result));
  }

  /// Rotate a XOR b right by Rotate bits in each 64-bit lane; Rotate is in [0, 63].
  template<isa Arch, unsigned Rotate> requires(Arch.has(arm_feature::sha3) && Rotate < 64)
  native_nodiscard native_inline native_const native_target("sha3")
  simd<std::uint64_t, 2, Arch> xar(simd<std::uint64_t, 2, Arch> a, simd<std::uint64_t, 2, Arch> b) noexcept {
    auto result = detail::arm_sha::xar<Arch, Rotate>(__builtin_bit_cast(uint64x2_t, a.to_native()), __builtin_bit_cast(uint64x2_t, b.to_native()));
    return simd<std::uint64_t, 2, Arch>::from_native(__builtin_bit_cast(typename simd<std::uint64_t, 2, Arch>::native_type, result));
  }

  // Require exact semantic shapes and a consistent feature tag.
  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa Arch, class... T> void sha1c(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa Arch, class... T> void sha1p(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa Arch, class... T> void sha1m(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa Arch=NATIVE_BASELINE, class... T> void sha1h(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa Arch, class... T> void sha1su0(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa Arch, class... T> void sha1su1(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa Arch, class... T> void sha256h(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa Arch, class... T> void sha256h2(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa Arch, class... T> void sha256su0(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa Arch, class... T> void sha256su1(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa Arch, class... T> void sha512h(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa Arch, class... T> void sha512h2(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa Arch, class... T> void sha512su0(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa Arch, class... T> void sha512su1(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa Arch, class... T> void eor3(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa Arch, class... T> void bcax(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa Arch, class... T> void rax1(T...) = delete;

  /// Reject unsupported argument shapes or unavailable instruction features.
  template<isa Arch, unsigned Rotate, class... T> void xar(T...) = delete;

  /// \}
}
#endif
