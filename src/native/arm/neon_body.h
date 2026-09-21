// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#if NATIVE_HOST_NEON || defined(NATIVE_DOXYGEN)
export namespace native {
  /// \defgroup arm_neon NEON integer instructions
  /// Typed instruction operations require arm_feature::neon and a matching caller target.
  /// Saturation may set sticky FPSR.QC at runtime; constant evaluation computes values only.
  /// On big endian, two-lane 64-bit add/subtract and variable shifts are immediate-only:
  /// current Clang inline-assembly lowering adds register permutations for those shapes.
  /// \{

  /// Signed saturating lane addition.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(Arch.has(arm_feature::neon) &&
      (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ || sizeof(T) != 8 || N != 2) &&
      std::is_signed_v<T> && (sizeof(T) <= 8) &&
      (sizeof(T) * N == 8 || sizeof(T) * N == 16))
  native_nodiscard native_inline native_target("neon") constexpr simd<T, N, Arch>
  sqadd(simd<T, N, Arch> a, simd<T, N, Arch> b) noexcept {
    if consteval {
      return detail::arm_neon_constant::binary(
        a, b, [](auto x, auto y) { return detail::arm_neon_constant::add(x, y); });
    } else {
      if constexpr (sizeof(T) == 1 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqadd(detail::arm_neon::to_register<int8x8_t>(a),
            detail::arm_neon::to_register<int8x8_t>(b)));
      else if constexpr (sizeof(T) == 1 && N == 16)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqadd(detail::arm_neon::to_register<int8x16_t>(a),
            detail::arm_neon::to_register<int8x16_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqadd(detail::arm_neon::to_register<int16x4_t>(a),
            detail::arm_neon::to_register<int16x4_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqadd(detail::arm_neon::to_register<int16x8_t>(a),
            detail::arm_neon::to_register<int16x8_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqadd(detail::arm_neon::to_register<int32x2_t>(a),
            detail::arm_neon::to_register<int32x2_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqadd(detail::arm_neon::to_register<int32x4_t>(a),
            detail::arm_neon::to_register<int32x4_t>(b)));
      else if constexpr (sizeof(T) == 8 && N == 1)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqadd(detail::arm_neon::to_register<int64x1_t>(a),
            detail::arm_neon::to_register<int64x1_t>(b)));
      else if constexpr (sizeof(T) == 8 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqadd(detail::arm_neon::to_register<int64x2_t>(a),
            detail::arm_neon::to_register<int64x2_t>(b)));
    }
  }

  /// Evaluate constant operands when NEON is absent or the big-endian runtime shape is unavailable.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires((!Arch.has(arm_feature::neon) ||
        (__BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__ && sizeof(T) == 8 && N == 2)) &&
      std::is_signed_v<T> && (sizeof(T) <= 8) &&
      (sizeof(T) * N == 8 || sizeof(T) * N == 16) && requires { sizeof(simd<T, N, Arch>); })
  native_nodiscard consteval simd<T, N, Arch> sqadd(simd<T, N, Arch> a,
    simd<T, N, Arch> b) noexcept {
    return detail::arm_neon_constant::binary(
      a, b, [](auto x, auto y) { return detail::arm_neon_constant::add(x, y); });
  }

  /// Unsigned saturating lane addition.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(Arch.has(arm_feature::neon) &&
      (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ || sizeof(T) != 8 || N != 2) &&
      std::is_unsigned_v<T> && (sizeof(T) <= 8) &&
      (sizeof(T) * N == 8 || sizeof(T) * N == 16))
  native_nodiscard native_inline native_target("neon") constexpr simd<T, N, Arch>
  uqadd(simd<T, N, Arch> a, simd<T, N, Arch> b) noexcept {
    if consteval {
      return detail::arm_neon_constant::binary(
        a, b, [](auto x, auto y) { return detail::arm_neon_constant::add(x, y); });
    } else {
      if constexpr (sizeof(T) == 1 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqadd(detail::arm_neon::to_register<uint8x8_t>(a),
            detail::arm_neon::to_register<uint8x8_t>(b)));
      else if constexpr (sizeof(T) == 1 && N == 16)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqadd(detail::arm_neon::to_register<uint8x16_t>(a),
            detail::arm_neon::to_register<uint8x16_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqadd(detail::arm_neon::to_register<uint16x4_t>(a),
            detail::arm_neon::to_register<uint16x4_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqadd(detail::arm_neon::to_register<uint16x8_t>(a),
            detail::arm_neon::to_register<uint16x8_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqadd(detail::arm_neon::to_register<uint32x2_t>(a),
            detail::arm_neon::to_register<uint32x2_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqadd(detail::arm_neon::to_register<uint32x4_t>(a),
            detail::arm_neon::to_register<uint32x4_t>(b)));
      else if constexpr (sizeof(T) == 8 && N == 1)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqadd(detail::arm_neon::to_register<uint64x1_t>(a),
            detail::arm_neon::to_register<uint64x1_t>(b)));
      else if constexpr (sizeof(T) == 8 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqadd(detail::arm_neon::to_register<uint64x2_t>(a),
            detail::arm_neon::to_register<uint64x2_t>(b)));
    }
  }

  /// Evaluate constant operands when NEON is absent or the big-endian runtime shape is unavailable.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires((!Arch.has(arm_feature::neon) ||
        (__BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__ && sizeof(T) == 8 && N == 2)) &&
      std::is_unsigned_v<T> && (sizeof(T) <= 8) &&
      (sizeof(T) * N == 8 || sizeof(T) * N == 16) && requires { sizeof(simd<T, N, Arch>); })
  native_nodiscard consteval simd<T, N, Arch> uqadd(simd<T, N, Arch> a,
    simd<T, N, Arch> b) noexcept {
    return detail::arm_neon_constant::binary(
      a, b, [](auto x, auto y) { return detail::arm_neon_constant::add(x, y); });
  }

  /// Signed saturating lane subtraction.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(Arch.has(arm_feature::neon) &&
      (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ || sizeof(T) != 8 || N != 2) &&
      std::is_signed_v<T> && (sizeof(T) <= 8) &&
      (sizeof(T) * N == 8 || sizeof(T) * N == 16))
  native_nodiscard native_inline native_target("neon") constexpr simd<T, N, Arch>
  sqsub(simd<T, N, Arch> a, simd<T, N, Arch> b) noexcept {
    if consteval {
      return detail::arm_neon_constant::binary(
        a, b, [](auto x, auto y) { return detail::arm_neon_constant::sub(x, y); });
    } else {
      if constexpr (sizeof(T) == 1 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqsub(detail::arm_neon::to_register<int8x8_t>(a),
            detail::arm_neon::to_register<int8x8_t>(b)));
      else if constexpr (sizeof(T) == 1 && N == 16)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqsub(detail::arm_neon::to_register<int8x16_t>(a),
            detail::arm_neon::to_register<int8x16_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqsub(detail::arm_neon::to_register<int16x4_t>(a),
            detail::arm_neon::to_register<int16x4_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqsub(detail::arm_neon::to_register<int16x8_t>(a),
            detail::arm_neon::to_register<int16x8_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqsub(detail::arm_neon::to_register<int32x2_t>(a),
            detail::arm_neon::to_register<int32x2_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqsub(detail::arm_neon::to_register<int32x4_t>(a),
            detail::arm_neon::to_register<int32x4_t>(b)));
      else if constexpr (sizeof(T) == 8 && N == 1)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqsub(detail::arm_neon::to_register<int64x1_t>(a),
            detail::arm_neon::to_register<int64x1_t>(b)));
      else if constexpr (sizeof(T) == 8 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqsub(detail::arm_neon::to_register<int64x2_t>(a),
            detail::arm_neon::to_register<int64x2_t>(b)));
    }
  }

  /// Evaluate constant operands when NEON is absent or the big-endian runtime shape is unavailable.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires((!Arch.has(arm_feature::neon) ||
        (__BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__ && sizeof(T) == 8 && N == 2)) &&
      std::is_signed_v<T> && (sizeof(T) <= 8) &&
      (sizeof(T) * N == 8 || sizeof(T) * N == 16) && requires { sizeof(simd<T, N, Arch>); })
  native_nodiscard consteval simd<T, N, Arch> sqsub(simd<T, N, Arch> a,
    simd<T, N, Arch> b) noexcept {
    return detail::arm_neon_constant::binary(
      a, b, [](auto x, auto y) { return detail::arm_neon_constant::sub(x, y); });
  }

  /// Unsigned saturating lane subtraction.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(Arch.has(arm_feature::neon) &&
      (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ || sizeof(T) != 8 || N != 2) &&
      std::is_unsigned_v<T> && (sizeof(T) <= 8) &&
      (sizeof(T) * N == 8 || sizeof(T) * N == 16))
  native_nodiscard native_inline native_target("neon") constexpr simd<T, N, Arch>
  uqsub(simd<T, N, Arch> a, simd<T, N, Arch> b) noexcept {
    if consteval {
      return detail::arm_neon_constant::binary(
        a, b, [](auto x, auto y) { return detail::arm_neon_constant::sub(x, y); });
    } else {
      if constexpr (sizeof(T) == 1 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqsub(detail::arm_neon::to_register<uint8x8_t>(a),
            detail::arm_neon::to_register<uint8x8_t>(b)));
      else if constexpr (sizeof(T) == 1 && N == 16)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqsub(detail::arm_neon::to_register<uint8x16_t>(a),
            detail::arm_neon::to_register<uint8x16_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqsub(detail::arm_neon::to_register<uint16x4_t>(a),
            detail::arm_neon::to_register<uint16x4_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqsub(detail::arm_neon::to_register<uint16x8_t>(a),
            detail::arm_neon::to_register<uint16x8_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqsub(detail::arm_neon::to_register<uint32x2_t>(a),
            detail::arm_neon::to_register<uint32x2_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqsub(detail::arm_neon::to_register<uint32x4_t>(a),
            detail::arm_neon::to_register<uint32x4_t>(b)));
      else if constexpr (sizeof(T) == 8 && N == 1)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqsub(detail::arm_neon::to_register<uint64x1_t>(a),
            detail::arm_neon::to_register<uint64x1_t>(b)));
      else if constexpr (sizeof(T) == 8 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqsub(detail::arm_neon::to_register<uint64x2_t>(a),
            detail::arm_neon::to_register<uint64x2_t>(b)));
    }
  }

  /// Evaluate constant operands when NEON is absent or the big-endian runtime shape is unavailable.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires((!Arch.has(arm_feature::neon) ||
        (__BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__ && sizeof(T) == 8 && N == 2)) &&
      std::is_unsigned_v<T> && (sizeof(T) <= 8) &&
      (sizeof(T) * N == 8 || sizeof(T) * N == 16) && requires { sizeof(simd<T, N, Arch>); })
  native_nodiscard consteval simd<T, N, Arch> uqsub(simd<T, N, Arch> a,
    simd<T, N, Arch> b) noexcept {
    return detail::arm_neon_constant::binary(
      a, b, [](auto x, auto y) { return detail::arm_neon_constant::sub(x, y); });
  }

  /// Signed doubled multiply-high, saturating the minimum-times-minimum case.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(Arch.has(arm_feature::neon) && std::is_signed_v<T> &&
      (sizeof(T) == 2 || sizeof(T) == 4) && (sizeof(T) * N == 8 || sizeof(T) * N == 16))
  native_nodiscard native_inline native_target("neon") constexpr simd<T, N, Arch>
  sqdmulh(simd<T, N, Arch> a, simd<T, N, Arch> b) noexcept {
    if consteval {
      return detail::arm_neon_constant::binary(a, b, [](auto x, auto y) {
        return detail::arm_neon_constant::multiply_high<false>(x, y);
      });
    } else {
      if constexpr (sizeof(T) == 2 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqdmulh(detail::arm_neon::to_register<int16x4_t>(a),
            detail::arm_neon::to_register<int16x4_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqdmulh(detail::arm_neon::to_register<int16x8_t>(a),
            detail::arm_neon::to_register<int16x8_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqdmulh(detail::arm_neon::to_register<int32x2_t>(a),
            detail::arm_neon::to_register<int32x2_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqdmulh(detail::arm_neon::to_register<int32x4_t>(a),
            detail::arm_neon::to_register<int32x4_t>(b)));
    }
  }

  /// Evaluate constant operands without NEON when the storage shape exists.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(!Arch.has(arm_feature::neon) && std::is_signed_v<T> &&
      (sizeof(T) == 2 || sizeof(T) == 4) && (sizeof(T) * N == 8 || sizeof(T) * N == 16) &&
      requires { sizeof(simd<T, N, Arch>); })
  native_nodiscard consteval simd<T, N, Arch> sqdmulh(simd<T, N, Arch> a,
    simd<T, N, Arch> b) noexcept {
    return detail::arm_neon_constant::binary(
      a, b, [](auto x, auto y) { return detail::arm_neon_constant::multiply_high<false>(x, y); });
  }

  /// Signed doubled multiply-high with rounding before final saturation.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(Arch.has(arm_feature::neon) && std::is_signed_v<T> &&
      (sizeof(T) == 2 || sizeof(T) == 4) && (sizeof(T) * N == 8 || sizeof(T) * N == 16))
  native_nodiscard native_inline native_target("neon") constexpr simd<T, N, Arch>
  sqrdmulh(simd<T, N, Arch> a, simd<T, N, Arch> b) noexcept {
    if consteval {
      return detail::arm_neon_constant::binary(a, b, [](auto x, auto y) {
        return detail::arm_neon_constant::multiply_high<true>(x, y);
      });
    } else {
      if constexpr (sizeof(T) == 2 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqrdmulh(detail::arm_neon::to_register<int16x4_t>(a),
            detail::arm_neon::to_register<int16x4_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqrdmulh(detail::arm_neon::to_register<int16x8_t>(a),
            detail::arm_neon::to_register<int16x8_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqrdmulh(detail::arm_neon::to_register<int32x2_t>(a),
            detail::arm_neon::to_register<int32x2_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqrdmulh(detail::arm_neon::to_register<int32x4_t>(a),
            detail::arm_neon::to_register<int32x4_t>(b)));
    }
  }

  /// Evaluate constant operands without NEON when the storage shape exists.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(!Arch.has(arm_feature::neon) && std::is_signed_v<T> &&
      (sizeof(T) == 2 || sizeof(T) == 4) && (sizeof(T) * N == 8 || sizeof(T) * N == 16) &&
      requires { sizeof(simd<T, N, Arch>); })
  native_nodiscard consteval simd<T, N, Arch> sqrdmulh(simd<T, N, Arch> a,
    simd<T, N, Arch> b) noexcept {
    return detail::arm_neon_constant::binary(
      a, b, [](auto x, auto y) { return detail::arm_neon_constant::multiply_high<true>(x, y); });
  }

  /// Shift by each count lane's signed low byte; negative counts shift right.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(Arch.has(arm_feature::neon) &&
      (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ || sizeof(T) != 8 || N != 2) &&
      std::is_signed_v<T> && (sizeof(T) <= 8) &&
      (sizeof(T) * N == 8 || sizeof(T) * N == 16))
  native_nodiscard native_inline native_target("neon") constexpr simd<T, N, Arch>
  sshl(simd<T, N, Arch> a, simd<std::make_signed_t<T>, N, Arch> b) noexcept {
    if consteval {
      return detail::arm_neon_constant::binary(a, b, [](auto x, auto y) {
        return detail::arm_neon_constant::shift<false, false>(x, y);
      });
    } else {
      if constexpr (sizeof(T) == 1 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sshl(detail::arm_neon::to_register<int8x8_t>(a),
            detail::arm_neon::to_register<int8x8_t>(b)));
      else if constexpr (sizeof(T) == 1 && N == 16)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sshl(detail::arm_neon::to_register<int8x16_t>(a),
            detail::arm_neon::to_register<int8x16_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sshl(detail::arm_neon::to_register<int16x4_t>(a),
            detail::arm_neon::to_register<int16x4_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sshl(detail::arm_neon::to_register<int16x8_t>(a),
            detail::arm_neon::to_register<int16x8_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sshl(detail::arm_neon::to_register<int32x2_t>(a),
            detail::arm_neon::to_register<int32x2_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sshl(detail::arm_neon::to_register<int32x4_t>(a),
            detail::arm_neon::to_register<int32x4_t>(b)));
      else if constexpr (sizeof(T) == 8 && N == 1)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sshl(detail::arm_neon::to_register<int64x1_t>(a),
            detail::arm_neon::to_register<int64x1_t>(b)));
      else if constexpr (sizeof(T) == 8 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sshl(detail::arm_neon::to_register<int64x2_t>(a),
            detail::arm_neon::to_register<int64x2_t>(b)));
    }
  }

  /// Evaluate constant operands when NEON is absent or the big-endian runtime shape is unavailable.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires((!Arch.has(arm_feature::neon) ||
        (__BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__ && sizeof(T) == 8 && N == 2)) &&
      std::is_signed_v<T> && (sizeof(T) <= 8) &&
      (sizeof(T) * N == 8 || sizeof(T) * N == 16) && requires { sizeof(simd<T, N, Arch>); })
  native_nodiscard consteval simd<T, N, Arch>
  sshl(simd<T, N, Arch> a, simd<std::make_signed_t<T>, N, Arch> b) noexcept {
    return detail::arm_neon_constant::binary(
      a, b, [](auto x, auto y) { return detail::arm_neon_constant::shift<false, false>(x, y); });
  }

  /// Shift by each count lane's signed low byte; negative counts shift right with rounding.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(Arch.has(arm_feature::neon) &&
      (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ || sizeof(T) != 8 || N != 2) &&
      std::is_signed_v<T> && (sizeof(T) <= 8) &&
      (sizeof(T) * N == 8 || sizeof(T) * N == 16))
  native_nodiscard native_inline native_target("neon") constexpr simd<T, N, Arch>
  srshl(simd<T, N, Arch> a, simd<std::make_signed_t<T>, N, Arch> b) noexcept {
    if consteval {
      return detail::arm_neon_constant::binary(
        a, b, [](auto x, auto y) { return detail::arm_neon_constant::shift<true, false>(x, y); });
    } else {
      if constexpr (sizeof(T) == 1 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::srshl(detail::arm_neon::to_register<int8x8_t>(a),
            detail::arm_neon::to_register<int8x8_t>(b)));
      else if constexpr (sizeof(T) == 1 && N == 16)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::srshl(detail::arm_neon::to_register<int8x16_t>(a),
            detail::arm_neon::to_register<int8x16_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::srshl(detail::arm_neon::to_register<int16x4_t>(a),
            detail::arm_neon::to_register<int16x4_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::srshl(detail::arm_neon::to_register<int16x8_t>(a),
            detail::arm_neon::to_register<int16x8_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::srshl(detail::arm_neon::to_register<int32x2_t>(a),
            detail::arm_neon::to_register<int32x2_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::srshl(detail::arm_neon::to_register<int32x4_t>(a),
            detail::arm_neon::to_register<int32x4_t>(b)));
      else if constexpr (sizeof(T) == 8 && N == 1)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::srshl(detail::arm_neon::to_register<int64x1_t>(a),
            detail::arm_neon::to_register<int64x1_t>(b)));
      else if constexpr (sizeof(T) == 8 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::srshl(detail::arm_neon::to_register<int64x2_t>(a),
            detail::arm_neon::to_register<int64x2_t>(b)));
    }
  }

  /// Evaluate constant operands when NEON is absent or the big-endian runtime shape is unavailable.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires((!Arch.has(arm_feature::neon) ||
        (__BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__ && sizeof(T) == 8 && N == 2)) &&
      std::is_signed_v<T> && (sizeof(T) <= 8) &&
      (sizeof(T) * N == 8 || sizeof(T) * N == 16) && requires { sizeof(simd<T, N, Arch>); })
  native_nodiscard consteval simd<T, N, Arch>
  srshl(simd<T, N, Arch> a, simd<std::make_signed_t<T>, N, Arch> b) noexcept {
    return detail::arm_neon_constant::binary(
      a, b, [](auto x, auto y) { return detail::arm_neon_constant::shift<true, false>(x, y); });
  }

  /// Shift by each count lane's signed low byte; negative counts shift right. Left shifts saturate
  /// and can set FPSR.QC.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(Arch.has(arm_feature::neon) &&
      (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ || sizeof(T) != 8 || N != 2) &&
      std::is_signed_v<T> && (sizeof(T) <= 8) &&
      (sizeof(T) * N == 8 || sizeof(T) * N == 16))
  native_nodiscard native_inline native_target("neon") constexpr simd<T, N, Arch>
  sqshl(simd<T, N, Arch> a, simd<std::make_signed_t<T>, N, Arch> b) noexcept {
    if consteval {
      return detail::arm_neon_constant::binary(
        a, b, [](auto x, auto y) { return detail::arm_neon_constant::shift<false, true>(x, y); });
    } else {
      if constexpr (sizeof(T) == 1 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqshl(detail::arm_neon::to_register<int8x8_t>(a),
            detail::arm_neon::to_register<int8x8_t>(b)));
      else if constexpr (sizeof(T) == 1 && N == 16)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqshl(detail::arm_neon::to_register<int8x16_t>(a),
            detail::arm_neon::to_register<int8x16_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqshl(detail::arm_neon::to_register<int16x4_t>(a),
            detail::arm_neon::to_register<int16x4_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqshl(detail::arm_neon::to_register<int16x8_t>(a),
            detail::arm_neon::to_register<int16x8_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqshl(detail::arm_neon::to_register<int32x2_t>(a),
            detail::arm_neon::to_register<int32x2_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqshl(detail::arm_neon::to_register<int32x4_t>(a),
            detail::arm_neon::to_register<int32x4_t>(b)));
      else if constexpr (sizeof(T) == 8 && N == 1)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqshl(detail::arm_neon::to_register<int64x1_t>(a),
            detail::arm_neon::to_register<int64x1_t>(b)));
      else if constexpr (sizeof(T) == 8 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqshl(detail::arm_neon::to_register<int64x2_t>(a),
            detail::arm_neon::to_register<int64x2_t>(b)));
    }
  }

  /// Evaluate constant operands when NEON is absent or the big-endian runtime shape is unavailable.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires((!Arch.has(arm_feature::neon) ||
        (__BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__ && sizeof(T) == 8 && N == 2)) &&
      std::is_signed_v<T> && (sizeof(T) <= 8) &&
      (sizeof(T) * N == 8 || sizeof(T) * N == 16) && requires { sizeof(simd<T, N, Arch>); })
  native_nodiscard consteval simd<T, N, Arch>
  sqshl(simd<T, N, Arch> a, simd<std::make_signed_t<T>, N, Arch> b) noexcept {
    return detail::arm_neon_constant::binary(
      a, b, [](auto x, auto y) { return detail::arm_neon_constant::shift<false, true>(x, y); });
  }

  /// Shift by each count lane's signed low byte; negative counts shift right with rounding. Left
  /// shifts saturate and can set FPSR.QC.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(Arch.has(arm_feature::neon) &&
      (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ || sizeof(T) != 8 || N != 2) &&
      std::is_signed_v<T> && (sizeof(T) <= 8) &&
      (sizeof(T) * N == 8 || sizeof(T) * N == 16))
  native_nodiscard native_inline native_target("neon") constexpr simd<T, N, Arch>
  sqrshl(simd<T, N, Arch> a, simd<std::make_signed_t<T>, N, Arch> b) noexcept {
    if consteval {
      return detail::arm_neon_constant::binary(
        a, b, [](auto x, auto y) { return detail::arm_neon_constant::shift<true, true>(x, y); });
    } else {
      if constexpr (sizeof(T) == 1 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqrshl(detail::arm_neon::to_register<int8x8_t>(a),
            detail::arm_neon::to_register<int8x8_t>(b)));
      else if constexpr (sizeof(T) == 1 && N == 16)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqrshl(detail::arm_neon::to_register<int8x16_t>(a),
            detail::arm_neon::to_register<int8x16_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqrshl(detail::arm_neon::to_register<int16x4_t>(a),
            detail::arm_neon::to_register<int16x4_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqrshl(detail::arm_neon::to_register<int16x8_t>(a),
            detail::arm_neon::to_register<int16x8_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqrshl(detail::arm_neon::to_register<int32x2_t>(a),
            detail::arm_neon::to_register<int32x2_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqrshl(detail::arm_neon::to_register<int32x4_t>(a),
            detail::arm_neon::to_register<int32x4_t>(b)));
      else if constexpr (sizeof(T) == 8 && N == 1)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqrshl(detail::arm_neon::to_register<int64x1_t>(a),
            detail::arm_neon::to_register<int64x1_t>(b)));
      else if constexpr (sizeof(T) == 8 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::sqrshl(detail::arm_neon::to_register<int64x2_t>(a),
            detail::arm_neon::to_register<int64x2_t>(b)));
    }
  }

  /// Evaluate constant operands when NEON is absent or the big-endian runtime shape is unavailable.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires((!Arch.has(arm_feature::neon) ||
        (__BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__ && sizeof(T) == 8 && N == 2)) &&
      std::is_signed_v<T> && (sizeof(T) <= 8) &&
      (sizeof(T) * N == 8 || sizeof(T) * N == 16) && requires { sizeof(simd<T, N, Arch>); })
  native_nodiscard consteval simd<T, N, Arch>
  sqrshl(simd<T, N, Arch> a, simd<std::make_signed_t<T>, N, Arch> b) noexcept {
    return detail::arm_neon_constant::binary(
      a, b, [](auto x, auto y) { return detail::arm_neon_constant::shift<true, true>(x, y); });
  }

  /// Shift by each count lane's signed low byte; negative counts shift right.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(Arch.has(arm_feature::neon) &&
      (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ || sizeof(T) != 8 || N != 2) &&
      std::is_unsigned_v<T> && (sizeof(T) <= 8) &&
      (sizeof(T) * N == 8 || sizeof(T) * N == 16))
  native_nodiscard native_inline native_target("neon") constexpr simd<T, N, Arch>
  ushl(simd<T, N, Arch> a, simd<std::make_signed_t<T>, N, Arch> b) noexcept {
    if consteval {
      return detail::arm_neon_constant::binary(a, b, [](auto x, auto y) {
        return detail::arm_neon_constant::shift<false, false>(x, y);
      });
    } else {
      if constexpr (sizeof(T) == 1 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::ushl(detail::arm_neon::to_register<uint8x8_t>(a),
            detail::arm_neon::to_register<int8x8_t>(b)));
      else if constexpr (sizeof(T) == 1 && N == 16)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::ushl(detail::arm_neon::to_register<uint8x16_t>(a),
            detail::arm_neon::to_register<int8x16_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::ushl(detail::arm_neon::to_register<uint16x4_t>(a),
            detail::arm_neon::to_register<int16x4_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::ushl(detail::arm_neon::to_register<uint16x8_t>(a),
            detail::arm_neon::to_register<int16x8_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::ushl(detail::arm_neon::to_register<uint32x2_t>(a),
            detail::arm_neon::to_register<int32x2_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::ushl(detail::arm_neon::to_register<uint32x4_t>(a),
            detail::arm_neon::to_register<int32x4_t>(b)));
      else if constexpr (sizeof(T) == 8 && N == 1)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::ushl(detail::arm_neon::to_register<uint64x1_t>(a),
            detail::arm_neon::to_register<int64x1_t>(b)));
      else if constexpr (sizeof(T) == 8 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::ushl(detail::arm_neon::to_register<uint64x2_t>(a),
            detail::arm_neon::to_register<int64x2_t>(b)));
    }
  }

  /// Evaluate constant operands when NEON is absent or the big-endian runtime shape is unavailable.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires((!Arch.has(arm_feature::neon) ||
        (__BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__ && sizeof(T) == 8 && N == 2)) &&
      std::is_unsigned_v<T> && (sizeof(T) <= 8) &&
      (sizeof(T) * N == 8 || sizeof(T) * N == 16) && requires { sizeof(simd<T, N, Arch>); })
  native_nodiscard consteval simd<T, N, Arch>
  ushl(simd<T, N, Arch> a, simd<std::make_signed_t<T>, N, Arch> b) noexcept {
    return detail::arm_neon_constant::binary(
      a, b, [](auto x, auto y) { return detail::arm_neon_constant::shift<false, false>(x, y); });
  }

  /// Shift by each count lane's signed low byte; negative counts shift right with rounding.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(Arch.has(arm_feature::neon) &&
      (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ || sizeof(T) != 8 || N != 2) &&
      std::is_unsigned_v<T> && (sizeof(T) <= 8) &&
      (sizeof(T) * N == 8 || sizeof(T) * N == 16))
  native_nodiscard native_inline native_target("neon") constexpr simd<T, N, Arch>
  urshl(simd<T, N, Arch> a, simd<std::make_signed_t<T>, N, Arch> b) noexcept {
    if consteval {
      return detail::arm_neon_constant::binary(
        a, b, [](auto x, auto y) { return detail::arm_neon_constant::shift<true, false>(x, y); });
    } else {
      if constexpr (sizeof(T) == 1 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::urshl(detail::arm_neon::to_register<uint8x8_t>(a),
            detail::arm_neon::to_register<int8x8_t>(b)));
      else if constexpr (sizeof(T) == 1 && N == 16)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::urshl(detail::arm_neon::to_register<uint8x16_t>(a),
            detail::arm_neon::to_register<int8x16_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::urshl(detail::arm_neon::to_register<uint16x4_t>(a),
            detail::arm_neon::to_register<int16x4_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::urshl(detail::arm_neon::to_register<uint16x8_t>(a),
            detail::arm_neon::to_register<int16x8_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::urshl(detail::arm_neon::to_register<uint32x2_t>(a),
            detail::arm_neon::to_register<int32x2_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::urshl(detail::arm_neon::to_register<uint32x4_t>(a),
            detail::arm_neon::to_register<int32x4_t>(b)));
      else if constexpr (sizeof(T) == 8 && N == 1)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::urshl(detail::arm_neon::to_register<uint64x1_t>(a),
            detail::arm_neon::to_register<int64x1_t>(b)));
      else if constexpr (sizeof(T) == 8 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::urshl(detail::arm_neon::to_register<uint64x2_t>(a),
            detail::arm_neon::to_register<int64x2_t>(b)));
    }
  }

  /// Evaluate constant operands when NEON is absent or the big-endian runtime shape is unavailable.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires((!Arch.has(arm_feature::neon) ||
        (__BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__ && sizeof(T) == 8 && N == 2)) &&
      std::is_unsigned_v<T> && (sizeof(T) <= 8) &&
      (sizeof(T) * N == 8 || sizeof(T) * N == 16) && requires { sizeof(simd<T, N, Arch>); })
  native_nodiscard consteval simd<T, N, Arch>
  urshl(simd<T, N, Arch> a, simd<std::make_signed_t<T>, N, Arch> b) noexcept {
    return detail::arm_neon_constant::binary(
      a, b, [](auto x, auto y) { return detail::arm_neon_constant::shift<true, false>(x, y); });
  }

  /// Shift by each count lane's signed low byte; negative counts shift right. Left shifts saturate
  /// and can set FPSR.QC.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(Arch.has(arm_feature::neon) &&
      (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ || sizeof(T) != 8 || N != 2) &&
      std::is_unsigned_v<T> && (sizeof(T) <= 8) &&
      (sizeof(T) * N == 8 || sizeof(T) * N == 16))
  native_nodiscard native_inline native_target("neon") constexpr simd<T, N, Arch>
  uqshl(simd<T, N, Arch> a, simd<std::make_signed_t<T>, N, Arch> b) noexcept {
    if consteval {
      return detail::arm_neon_constant::binary(
        a, b, [](auto x, auto y) { return detail::arm_neon_constant::shift<false, true>(x, y); });
    } else {
      if constexpr (sizeof(T) == 1 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqshl(detail::arm_neon::to_register<uint8x8_t>(a),
            detail::arm_neon::to_register<int8x8_t>(b)));
      else if constexpr (sizeof(T) == 1 && N == 16)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqshl(detail::arm_neon::to_register<uint8x16_t>(a),
            detail::arm_neon::to_register<int8x16_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqshl(detail::arm_neon::to_register<uint16x4_t>(a),
            detail::arm_neon::to_register<int16x4_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqshl(detail::arm_neon::to_register<uint16x8_t>(a),
            detail::arm_neon::to_register<int16x8_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqshl(detail::arm_neon::to_register<uint32x2_t>(a),
            detail::arm_neon::to_register<int32x2_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqshl(detail::arm_neon::to_register<uint32x4_t>(a),
            detail::arm_neon::to_register<int32x4_t>(b)));
      else if constexpr (sizeof(T) == 8 && N == 1)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqshl(detail::arm_neon::to_register<uint64x1_t>(a),
            detail::arm_neon::to_register<int64x1_t>(b)));
      else if constexpr (sizeof(T) == 8 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqshl(detail::arm_neon::to_register<uint64x2_t>(a),
            detail::arm_neon::to_register<int64x2_t>(b)));
    }
  }

  /// Evaluate constant operands when NEON is absent or the big-endian runtime shape is unavailable.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires((!Arch.has(arm_feature::neon) ||
        (__BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__ && sizeof(T) == 8 && N == 2)) &&
      std::is_unsigned_v<T> && (sizeof(T) <= 8) &&
      (sizeof(T) * N == 8 || sizeof(T) * N == 16) && requires { sizeof(simd<T, N, Arch>); })
  native_nodiscard consteval simd<T, N, Arch>
  uqshl(simd<T, N, Arch> a, simd<std::make_signed_t<T>, N, Arch> b) noexcept {
    return detail::arm_neon_constant::binary(
      a, b, [](auto x, auto y) { return detail::arm_neon_constant::shift<false, true>(x, y); });
  }

  /// Shift by each count lane's signed low byte; negative counts shift right with rounding. Left
  /// shifts saturate and can set FPSR.QC.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(Arch.has(arm_feature::neon) &&
      (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ || sizeof(T) != 8 || N != 2) &&
      std::is_unsigned_v<T> && (sizeof(T) <= 8) &&
      (sizeof(T) * N == 8 || sizeof(T) * N == 16))
  native_nodiscard native_inline native_target("neon") constexpr simd<T, N, Arch>
  uqrshl(simd<T, N, Arch> a, simd<std::make_signed_t<T>, N, Arch> b) noexcept {
    if consteval {
      return detail::arm_neon_constant::binary(
        a, b, [](auto x, auto y) { return detail::arm_neon_constant::shift<true, true>(x, y); });
    } else {
      if constexpr (sizeof(T) == 1 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqrshl(detail::arm_neon::to_register<uint8x8_t>(a),
            detail::arm_neon::to_register<int8x8_t>(b)));
      else if constexpr (sizeof(T) == 1 && N == 16)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqrshl(detail::arm_neon::to_register<uint8x16_t>(a),
            detail::arm_neon::to_register<int8x16_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqrshl(detail::arm_neon::to_register<uint16x4_t>(a),
            detail::arm_neon::to_register<int16x4_t>(b)));
      else if constexpr (sizeof(T) == 2 && N == 8)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqrshl(detail::arm_neon::to_register<uint16x8_t>(a),
            detail::arm_neon::to_register<int16x8_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqrshl(detail::arm_neon::to_register<uint32x2_t>(a),
            detail::arm_neon::to_register<int32x2_t>(b)));
      else if constexpr (sizeof(T) == 4 && N == 4)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqrshl(detail::arm_neon::to_register<uint32x4_t>(a),
            detail::arm_neon::to_register<int32x4_t>(b)));
      else if constexpr (sizeof(T) == 8 && N == 1)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqrshl(detail::arm_neon::to_register<uint64x1_t>(a),
            detail::arm_neon::to_register<int64x1_t>(b)));
      else if constexpr (sizeof(T) == 8 && N == 2)
        return detail::arm_neon::from_register<simd<T, N, Arch>>(
          detail::arm_neon::uqrshl(detail::arm_neon::to_register<uint64x2_t>(a),
            detail::arm_neon::to_register<int64x2_t>(b)));
    }
  }

  /// Evaluate constant operands when NEON is absent or the big-endian runtime shape is unavailable.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires((!Arch.has(arm_feature::neon) ||
        (__BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__ && sizeof(T) == 8 && N == 2)) &&
      std::is_unsigned_v<T> && (sizeof(T) <= 8) &&
      (sizeof(T) * N == 8 || sizeof(T) * N == 16) && requires { sizeof(simd<T, N, Arch>); })
  native_nodiscard consteval simd<T, N, Arch>
  uqrshl(simd<T, N, Arch> a, simd<std::make_signed_t<T>, N, Arch> b) noexcept {
    return detail::arm_neon_constant::binary(
      a, b, [](auto x, auto y) { return detail::arm_neon_constant::shift<true, true>(x, y); });
  }

  /// Narrow 128 bits with saturation to a 64-bit result.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(Arch.has(arm_feature::neon) && std::is_signed_v<T> && sizeof(T) * N == 16 &&
      (sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8))
  native_nodiscard native_inline native_target("neon") constexpr auto
  sqxtn(simd<T, N, Arch> a) noexcept {
    using element =
        std::conditional_t<sizeof(T) == 2, std::int8_t,
                           std::conditional_t<sizeof(T) == 4, std::int16_t, std::int32_t>>;
    using result = simd<element, N, Arch>;
    if consteval {
      return detail::arm_neon_constant::narrow_low<result>(a);
    } else {
      if constexpr (sizeof(T) == 2)
        return detail::arm_neon::from_register<result>(
          detail::arm_neon::sqxtn(detail::arm_neon::to_register<int16x8_t>(a)));
      else if constexpr (sizeof(T) == 4)
        return detail::arm_neon::from_register<result>(
          detail::arm_neon::sqxtn(detail::arm_neon::to_register<int32x4_t>(a)));
      else if constexpr (sizeof(T) == 8)
        return detail::arm_neon::from_register<result>(
          detail::arm_neon::sqxtn(detail::arm_neon::to_register<int64x2_t>(a)));
    }
  }

  /// Evaluate constant operands without NEON when storage exists.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(!Arch.has(arm_feature::neon) && std::is_signed_v<T> && sizeof(T) * N == 16 &&
      (sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8) &&
      requires { sizeof(simd<T, N, Arch>); })
  native_nodiscard consteval auto sqxtn(simd<T, N, Arch> a) noexcept {
    using element =
        std::conditional_t<sizeof(T) == 2, std::int8_t,
                           std::conditional_t<sizeof(T) == 4, std::int16_t, std::int32_t>>;
    using result = simd<element, N, Arch>;
    return detail::arm_neon_constant::narrow_low<result>(a);
  }

  /// Narrow with saturation and append above the preserved low half.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(Arch.has(arm_feature::neon) && std::is_signed_v<T> && sizeof(T) * N == 16 &&
      (sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8))
  native_nodiscard native_inline native_target("neon") constexpr auto sqxtn_high(
      simd<std::conditional_t<sizeof(T) == 2, std::int8_t,
                              std::conditional_t<sizeof(T) == 4, std::int16_t, std::int32_t>>,
           N, Arch>
          low,
      simd<T, N, Arch> a) noexcept {
    using element =
        std::conditional_t<sizeof(T) == 2, std::int8_t,
                           std::conditional_t<sizeof(T) == 4, std::int16_t, std::int32_t>>;
    using result = simd<element, 2 * N, Arch>;
    if consteval {
      return detail::arm_neon_constant::narrow_high<result>(low, a);
    } else {
      if constexpr (sizeof(T) == 2)
        return detail::arm_neon::from_register<result>(
          detail::arm_neon::sqxtn_high(detail::arm_neon::to_register<int8x8_t>(low),
            detail::arm_neon::to_register<int16x8_t>(a)));
      else if constexpr (sizeof(T) == 4)
        return detail::arm_neon::from_register<result>(
          detail::arm_neon::sqxtn_high(detail::arm_neon::to_register<int16x4_t>(low),
            detail::arm_neon::to_register<int32x4_t>(a)));
      else if constexpr (sizeof(T) == 8)
        return detail::arm_neon::from_register<result>(
          detail::arm_neon::sqxtn_high(detail::arm_neon::to_register<int32x2_t>(low),
            detail::arm_neon::to_register<int64x2_t>(a)));
    }
  }

  /// Evaluate constant operands without NEON when storage exists.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(!Arch.has(arm_feature::neon) && std::is_signed_v<T> && sizeof(T) * N == 16 &&
      (sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8) &&
      requires { sizeof(simd<T, N, Arch>); })
  native_nodiscard consteval auto sqxtn_high(
      simd<std::conditional_t<sizeof(T) == 2, std::int8_t,
                              std::conditional_t<sizeof(T) == 4, std::int16_t, std::int32_t>>,
           N, Arch>
          low,
      simd<T, N, Arch> a) noexcept {
    using element =
        std::conditional_t<sizeof(T) == 2, std::int8_t,
                           std::conditional_t<sizeof(T) == 4, std::int16_t, std::int32_t>>;
    using result = simd<element, 2 * N, Arch>;
    return detail::arm_neon_constant::narrow_high<result>(low, a);
  }

  /// Narrow 128 bits with saturation to a 64-bit result.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(Arch.has(arm_feature::neon) && std::is_unsigned_v<T> && sizeof(T) * N == 16 &&
      (sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8))
  native_nodiscard native_inline native_target("neon") constexpr auto
  uqxtn(simd<T, N, Arch> a) noexcept {
    using element =
        std::conditional_t<sizeof(T) == 2, std::uint8_t,
                           std::conditional_t<sizeof(T) == 4, std::uint16_t, std::uint32_t>>;
    using result = simd<element, N, Arch>;
    if consteval {
      return detail::arm_neon_constant::narrow_low<result>(a);
    } else {
      if constexpr (sizeof(T) == 2)
        return detail::arm_neon::from_register<result>(
          detail::arm_neon::uqxtn(detail::arm_neon::to_register<uint16x8_t>(a)));
      else if constexpr (sizeof(T) == 4)
        return detail::arm_neon::from_register<result>(
          detail::arm_neon::uqxtn(detail::arm_neon::to_register<uint32x4_t>(a)));
      else if constexpr (sizeof(T) == 8)
        return detail::arm_neon::from_register<result>(
          detail::arm_neon::uqxtn(detail::arm_neon::to_register<uint64x2_t>(a)));
    }
  }

  /// Evaluate constant operands without NEON when storage exists.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(!Arch.has(arm_feature::neon) && std::is_unsigned_v<T> && sizeof(T) * N == 16 &&
      (sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8) &&
      requires { sizeof(simd<T, N, Arch>); })
  native_nodiscard consteval auto uqxtn(simd<T, N, Arch> a) noexcept {
    using element =
        std::conditional_t<sizeof(T) == 2, std::uint8_t,
                           std::conditional_t<sizeof(T) == 4, std::uint16_t, std::uint32_t>>;
    using result = simd<element, N, Arch>;
    return detail::arm_neon_constant::narrow_low<result>(a);
  }

  /// Narrow with saturation and append above the preserved low half.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(Arch.has(arm_feature::neon) && std::is_unsigned_v<T> && sizeof(T) * N == 16 &&
      (sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8))
  native_nodiscard native_inline native_target("neon") constexpr auto uqxtn_high(
      simd<std::conditional_t<sizeof(T) == 2, std::uint8_t,
                              std::conditional_t<sizeof(T) == 4, std::uint16_t, std::uint32_t>>,
           N, Arch>
          low,
      simd<T, N, Arch> a) noexcept {
    using element =
        std::conditional_t<sizeof(T) == 2, std::uint8_t,
                           std::conditional_t<sizeof(T) == 4, std::uint16_t, std::uint32_t>>;
    using result = simd<element, 2 * N, Arch>;
    if consteval {
      return detail::arm_neon_constant::narrow_high<result>(low, a);
    } else {
      if constexpr (sizeof(T) == 2)
        return detail::arm_neon::from_register<result>(
          detail::arm_neon::uqxtn_high(detail::arm_neon::to_register<uint8x8_t>(low),
            detail::arm_neon::to_register<uint16x8_t>(a)));
      else if constexpr (sizeof(T) == 4)
        return detail::arm_neon::from_register<result>(
          detail::arm_neon::uqxtn_high(detail::arm_neon::to_register<uint16x4_t>(low),
            detail::arm_neon::to_register<uint32x4_t>(a)));
      else if constexpr (sizeof(T) == 8)
        return detail::arm_neon::from_register<result>(
          detail::arm_neon::uqxtn_high(detail::arm_neon::to_register<uint32x2_t>(low),
            detail::arm_neon::to_register<uint64x2_t>(a)));
    }
  }

  /// Evaluate constant operands without NEON when storage exists.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(!Arch.has(arm_feature::neon) && std::is_unsigned_v<T> && sizeof(T) * N == 16 &&
      (sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8) &&
      requires { sizeof(simd<T, N, Arch>); })
  native_nodiscard consteval auto uqxtn_high(
      simd<std::conditional_t<sizeof(T) == 2, std::uint8_t,
                              std::conditional_t<sizeof(T) == 4, std::uint16_t, std::uint32_t>>,
           N, Arch>
          low,
      simd<T, N, Arch> a) noexcept {
    using element =
        std::conditional_t<sizeof(T) == 2, std::uint8_t,
                           std::conditional_t<sizeof(T) == 4, std::uint16_t, std::uint32_t>>;
    using result = simd<element, 2 * N, Arch>;
    return detail::arm_neon_constant::narrow_high<result>(low, a);
  }

  /// Narrow 128 bits with saturation to a 64-bit result.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(Arch.has(arm_feature::neon) && std::is_signed_v<T> && sizeof(T) * N == 16 &&
      (sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8))
  native_nodiscard native_inline native_target("neon") constexpr auto
  sqxtun(simd<T, N, Arch> a) noexcept {
    using element =
        std::conditional_t<sizeof(T) == 2, std::uint8_t,
                           std::conditional_t<sizeof(T) == 4, std::uint16_t, std::uint32_t>>;
    using result = simd<element, N, Arch>;
    if consteval {
      return detail::arm_neon_constant::narrow_low<result>(a);
    } else {
      if constexpr (sizeof(T) == 2)
        return detail::arm_neon::from_register<result>(
          detail::arm_neon::sqxtun(detail::arm_neon::to_register<int16x8_t>(a)));
      else if constexpr (sizeof(T) == 4)
        return detail::arm_neon::from_register<result>(
          detail::arm_neon::sqxtun(detail::arm_neon::to_register<int32x4_t>(a)));
      else if constexpr (sizeof(T) == 8)
        return detail::arm_neon::from_register<result>(
          detail::arm_neon::sqxtun(detail::arm_neon::to_register<int64x2_t>(a)));
    }
  }

  /// Evaluate constant operands without NEON when storage exists.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(!Arch.has(arm_feature::neon) && std::is_signed_v<T> && sizeof(T) * N == 16 &&
      (sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8) &&
      requires { sizeof(simd<T, N, Arch>); })
  native_nodiscard consteval auto sqxtun(simd<T, N, Arch> a) noexcept {
    using element =
        std::conditional_t<sizeof(T) == 2, std::uint8_t,
                           std::conditional_t<sizeof(T) == 4, std::uint16_t, std::uint32_t>>;
    using result = simd<element, N, Arch>;
    return detail::arm_neon_constant::narrow_low<result>(a);
  }

  /// Narrow with saturation and append above the preserved low half.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(Arch.has(arm_feature::neon) && std::is_signed_v<T> && sizeof(T) * N == 16 &&
      (sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8))
  native_nodiscard native_inline native_target("neon") constexpr auto sqxtun_high(
      simd<std::conditional_t<sizeof(T) == 2, std::uint8_t,
                              std::conditional_t<sizeof(T) == 4, std::uint16_t, std::uint32_t>>,
           N, Arch>
          low,
      simd<T, N, Arch> a) noexcept {
    using element =
        std::conditional_t<sizeof(T) == 2, std::uint8_t,
                           std::conditional_t<sizeof(T) == 4, std::uint16_t, std::uint32_t>>;
    using result = simd<element, 2 * N, Arch>;
    if consteval {
      return detail::arm_neon_constant::narrow_high<result>(low, a);
    } else {
      if constexpr (sizeof(T) == 2)
        return detail::arm_neon::from_register<result>(
          detail::arm_neon::sqxtun_high(detail::arm_neon::to_register<uint8x8_t>(low),
            detail::arm_neon::to_register<int16x8_t>(a)));
      else if constexpr (sizeof(T) == 4)
        return detail::arm_neon::from_register<result>(
          detail::arm_neon::sqxtun_high(detail::arm_neon::to_register<uint16x4_t>(low),
            detail::arm_neon::to_register<int32x4_t>(a)));
      else if constexpr (sizeof(T) == 8)
        return detail::arm_neon::from_register<result>(
          detail::arm_neon::sqxtun_high(detail::arm_neon::to_register<uint32x2_t>(low),
            detail::arm_neon::to_register<int64x2_t>(a)));
    }
  }

  /// Evaluate constant operands without NEON when storage exists.
  template<isa<arm> Arch, simd_integer_element T, std::size_t N>
    requires(!Arch.has(arm_feature::neon) && std::is_signed_v<T> && sizeof(T) * N == 16 &&
      (sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8) &&
      requires { sizeof(simd<T, N, Arch>); })
  native_nodiscard consteval auto sqxtun_high(
      simd<std::conditional_t<sizeof(T) == 2, std::uint8_t,
                              std::conditional_t<sizeof(T) == 4, std::uint16_t, std::uint32_t>>,
           N, Arch>
          low,
      simd<T, N, Arch> a) noexcept {
    using element =
        std::conditional_t<sizeof(T) == 2, std::uint8_t,
                           std::conditional_t<sizeof(T) == 4, std::uint16_t, std::uint32_t>>;
    using result = simd<element, 2 * N, Arch>;
    return detail::arm_neon_constant::narrow_high<result>(low, a);
  }

  /// \}
} // namespace native
#endif
