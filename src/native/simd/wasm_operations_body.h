// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// SIMD128 instruction families, included in the SIMD128 target scope.
namespace native {
  namespace detail {
    template <class T> constexpr T wasm_saturate(std::int64_t x) noexcept {
      if (x < std::int64_t(std::numeric_limits<T>::min()))
        return std::numeric_limits<T>::min();
      if (x > std::int64_t(std::numeric_limits<T>::max()))
        return std::numeric_limits<T>::max();
      return T(x);
    }

    template <class To, class From> constexpr To wasm_trunc_sat(From x) noexcept {
      if (x != x)
        return 0;
      if (x <= double(std::numeric_limits<To>::min()))
        return std::numeric_limits<To>::min();
      if (x >= double(std::numeric_limits<To>::max()))
        return std::numeric_limits<To>::max();
      return To(x);
    }
  } // namespace detail

  /// Saturate signed or unsigned byte/halfword arithmetic at the lane limits.
  template <simd_integer_element T, std::size_t N, isa<> A>
    requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16 && sizeof(T) <= 2)
  native_inline constexpr simd<T, N, A> add_sat(simd<T, N, A> a, simd<T, N, A> b) noexcept {
    using V = simd<T, N, A>;
    if consteval {
      return detail::wasm_map(
          [](T x, T y) { return detail::wasm_saturate<T>(std::int64_t(x) + std::int64_t(y)); }, a,
          b);
    } else {
      if constexpr (std::same_as<T, std::int8_t>)
        return V::from_native(wasm_i8x16_add_sat(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<T, std::uint8_t>)
        return V::from_native(wasm_u8x16_add_sat(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<T, std::int16_t>)
        return V::from_native(wasm_i16x8_add_sat(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<T, std::uint16_t>)
        return V::from_native(wasm_u16x8_add_sat(a.to_native(), b.to_native()));
    }
  }

  /// Saturate signed or unsigned byte/halfword arithmetic at the lane limits.
  template <simd_integer_element T, std::size_t N, isa<> A>
    requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16 && sizeof(T) <= 2)
  native_inline constexpr simd<T, N, A> sub_sat(simd<T, N, A> a, simd<T, N, A> b) noexcept {
    using V = simd<T, N, A>;
    if consteval {
      return detail::wasm_map(
          [](T x, T y) { return detail::wasm_saturate<T>(std::int64_t(x) - std::int64_t(y)); }, a,
          b);
    } else {
      if constexpr (std::same_as<T, std::int8_t>)
        return V::from_native(wasm_i8x16_sub_sat(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<T, std::uint8_t>)
        return V::from_native(wasm_u8x16_sub_sat(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<T, std::int16_t>)
        return V::from_native(wasm_i16x8_sub_sat(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<T, std::uint16_t>)
        return V::from_native(wasm_u16x8_sub_sat(a.to_native(), b.to_native()));
    }
  }

  /// Rounded unsigned average: (a+b+1)/2 without intermediate overflow.
  template <simd_integer_element T, std::size_t N, isa<> A>
    requires(A.has(wasm_feature::simd128)) &&
            (std::is_unsigned_v<T> && sizeof(T) * N == 16 && sizeof(T) <= 2)
  native_inline constexpr simd<T, N, A> average_round(simd<T, N, A> a, simd<T, N, A> b) noexcept {
    if consteval {
      return detail::wasm_map([](T x, T y) { return T((unsigned(x) + y + 1) / 2); }, a, b);
    } else {
      if constexpr (sizeof(T) == 1)
        return simd<T, N, A>::from_native(wasm_u8x16_avgr(a.to_native(), b.to_native()));
      else
        return simd<T, N, A>::from_native(wasm_u16x8_avgr(a.to_native(), b.to_native()));
    }
  }

  /// Absolute value with the minimum signed lane retaining its representation.
  template <simd_integer_element T, std::size_t N, isa<> A>
    requires(A.has(wasm_feature::simd128)) && (std::is_signed_v<T> && sizeof(T) * N == 16)
  native_inline constexpr simd<T, N, A> abs(simd<T, N, A> a) noexcept {
    using V = simd<T, N, A>;
    using U = std::make_unsigned_t<T>;
    if consteval {
      return detail::wasm_map([](T x) { return x < 0 ? std::bit_cast<T>(U(0 - U(x))) : x; }, a);
    } else {
      if constexpr (sizeof(T) == 1)
        return V::from_native(wasm_i8x16_abs(a.to_native()));
      else if constexpr (sizeof(T) == 2)
        return V::from_native(wasm_i16x8_abs(a.to_native()));
      else if constexpr (sizeof(T) == 4)
        return V::from_native(wasm_i32x4_abs(a.to_native()));
      else if constexpr (sizeof(T) == 8)
        return V::from_native(wasm_i64x2_abs(a.to_native()));
    }
  }

  /// Minimum/maximum; floating NaNs propagate and signed zeros follow WebAssembly rules.
  template <detail::wasm_number T, std::size_t N, isa<> A>
    requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16)
  native_inline constexpr simd<T, N, A> min(simd<T, N, A> a, simd<T, N, A> b) noexcept {
    using V = simd<T, N, A>;
    if consteval {
      return detail::wasm_map(
          [](T x, T y) {
            if constexpr (std::is_floating_point_v<T>) {
              using F = detail::wasm_format<T>;
              using U = typename F::bits_type;
              auto xx = std::bit_cast<U>(x), yy = std::bit_cast<U>(y);
              if (detail::constexpr_float::is_nan<F>(xx) || detail::constexpr_float::is_nan<F>(yy))
                return std::bit_cast<T>(detail::constexpr_float::default_nan<F>({}));
              if (detail::constexpr_float::is_zero<F>(xx) &&
                  detail::constexpr_float::is_zero<F>(yy))
                return std::bit_cast<T>(U(xx | yy));
            }
            return y < x ? y : x;
          },
          a, b);
    } else {
      if constexpr (std::same_as<T, float>)
        return V::from_native(wasm_f32x4_min(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<T, double>)
        return V::from_native(wasm_f64x2_min(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<T, std::int8_t>)
        return V::from_native(wasm_i8x16_min(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<T, std::uint8_t>)
        return V::from_native(wasm_u8x16_min(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<T, std::int16_t>)
        return V::from_native(wasm_i16x8_min(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<T, std::uint16_t>)
        return V::from_native(wasm_u16x8_min(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<T, std::int32_t>)
        return V::from_native(wasm_i32x4_min(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<T, std::uint32_t>)
        return V::from_native(wasm_u32x4_min(a.to_native(), b.to_native()));
      else
        return select(a < b, a, b);
    }
  }

  /// Minimum/maximum; floating NaNs propagate and signed zeros follow WebAssembly rules.
  template <detail::wasm_number T, std::size_t N, isa<> A>
    requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16)
  native_inline constexpr simd<T, N, A> max(simd<T, N, A> a, simd<T, N, A> b) noexcept {
    using V = simd<T, N, A>;
    if consteval {
      return detail::wasm_map(
          [](T x, T y) {
            if constexpr (std::is_floating_point_v<T>) {
              using F = detail::wasm_format<T>;
              using U = typename F::bits_type;
              auto xx = std::bit_cast<U>(x), yy = std::bit_cast<U>(y);
              if (detail::constexpr_float::is_nan<F>(xx) || detail::constexpr_float::is_nan<F>(yy))
                return std::bit_cast<T>(detail::constexpr_float::default_nan<F>({}));
              if (detail::constexpr_float::is_zero<F>(xx) &&
                  detail::constexpr_float::is_zero<F>(yy))
                return std::bit_cast<T>(U(xx & yy));
            }
            return y > x ? y : x;
          },
          a, b);
    } else {
      if constexpr (std::same_as<T, float>)
        return V::from_native(wasm_f32x4_max(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<T, double>)
        return V::from_native(wasm_f64x2_max(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<T, std::int8_t>)
        return V::from_native(wasm_i8x16_max(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<T, std::uint8_t>)
        return V::from_native(wasm_u8x16_max(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<T, std::int16_t>)
        return V::from_native(wasm_i16x8_max(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<T, std::uint16_t>)
        return V::from_native(wasm_u16x8_max(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<T, std::int32_t>)
        return V::from_native(wasm_i32x4_max(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<T, std::uint32_t>)
        return V::from_native(wasm_u32x4_max(a.to_native(), b.to_native()));
      else
        return select(a > b, a, b);
    }
  }

  /// Pseudo minimum/maximum selects the first operand for unordered or equal lanes.
  template <detail::wasm_number T, std::size_t N, isa<> A>
    requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16 && std::is_floating_point_v<T>)
  native_inline constexpr simd<T, N, A> pmin(simd<T, N, A> a, simd<T, N, A> b) noexcept {
    using V = simd<T, N, A>;
    if consteval {
      return detail::wasm_map([](T x, T y) { return y < x ? y : x; }, a, b);
    } else {
      if constexpr (std::same_as<T, float>)
        return V::from_native(wasm_f32x4_pmin(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<T, double>)
        return V::from_native(wasm_f64x2_pmin(a.to_native(), b.to_native()));
    }
  }

  /// Pseudo minimum/maximum selects the first operand for unordered or equal lanes.
  template <detail::wasm_number T, std::size_t N, isa<> A>
    requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16 && std::is_floating_point_v<T>)
  native_inline constexpr simd<T, N, A> pmax(simd<T, N, A> a, simd<T, N, A> b) noexcept {
    using V = simd<T, N, A>;
    if consteval {
      return detail::wasm_map([](T x, T y) { return y > x ? y : x; }, a, b);
    } else {
      if constexpr (std::same_as<T, float>)
        return V::from_native(wasm_f32x4_pmax(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<T, double>)
        return V::from_native(wasm_f64x2_pmax(a.to_native(), b.to_native()));
    }
  }

  /// Widen the lower half of integer lanes, preserving signedness.
  template <simd_integer_element T, std::size_t N, isa<> A>
    requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16 && sizeof(T) <= 4)
  native_inline constexpr auto extend_low(simd<T, N, A> a) noexcept {
    using U0 = std::conditional_t<sizeof(T) == 1, std::uint16_t,
                                  std::conditional_t<sizeof(T) == 2, std::uint32_t, std::uint64_t>>;
    using U = std::conditional_t<std::is_signed_v<T>, std::make_signed_t<U0>, U0>;
    using V = simd<U, N / 2, A>;
    if consteval {
      auto x = detail::wasm_lanes(a);
      std::array<U, N / 2> r{};
      for (std::size_t i = 0; i < N / 2; ++i)
        r[i] = x[i];
      return V::load(r.data());
    } else {
      if constexpr (std::same_as<T, std::int8_t>)
        return V::from_native(wasm_i16x8_extend_low_i8x16(a.to_native()));
      else if constexpr (std::same_as<T, std::uint8_t>)
        return V::from_native(wasm_u16x8_extend_low_u8x16(a.to_native()));
      else if constexpr (std::same_as<T, std::int16_t>)
        return V::from_native(wasm_i32x4_extend_low_i16x8(a.to_native()));
      else if constexpr (std::same_as<T, std::uint16_t>)
        return V::from_native(wasm_u32x4_extend_low_u16x8(a.to_native()));
      else if constexpr (std::same_as<T, std::int32_t>)
        return V::from_native(wasm_i64x2_extend_low_i32x4(a.to_native()));
      else if constexpr (std::same_as<T, std::uint32_t>)
        return V::from_native(wasm_u64x2_extend_low_u32x4(a.to_native()));
    }
  }

  /// Multiply widened lower integer lanes; the complete product fits.
  template <simd_integer_element T, std::size_t N, isa<> A>
    requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16 && sizeof(T) <= 4)
  native_inline constexpr auto multiply_widened_low(simd<T, N, A> a, simd<T, N, A> b) noexcept {
    return extend_low(a) * extend_low(b);
  }

  /// Widen the upper half of integer lanes, preserving signedness.
  template <simd_integer_element T, std::size_t N, isa<> A>
    requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16 && sizeof(T) <= 4)
  native_inline constexpr auto extend_high(simd<T, N, A> a) noexcept {
    using U0 = std::conditional_t<sizeof(T) == 1, std::uint16_t,
                                  std::conditional_t<sizeof(T) == 2, std::uint32_t, std::uint64_t>>;
    using U = std::conditional_t<std::is_signed_v<T>, std::make_signed_t<U0>, U0>;
    using V = simd<U, N / 2, A>;
    if consteval {
      auto x = detail::wasm_lanes(a);
      std::array<U, N / 2> r{};
      for (std::size_t i = 0; i < N / 2; ++i)
        r[i] = x[i + N / 2];
      return V::load(r.data());
    } else {
      if constexpr (std::same_as<T, std::int8_t>)
        return V::from_native(wasm_i16x8_extend_high_i8x16(a.to_native()));
      else if constexpr (std::same_as<T, std::uint8_t>)
        return V::from_native(wasm_u16x8_extend_high_u8x16(a.to_native()));
      else if constexpr (std::same_as<T, std::int16_t>)
        return V::from_native(wasm_i32x4_extend_high_i16x8(a.to_native()));
      else if constexpr (std::same_as<T, std::uint16_t>)
        return V::from_native(wasm_u32x4_extend_high_u16x8(a.to_native()));
      else if constexpr (std::same_as<T, std::int32_t>)
        return V::from_native(wasm_i64x2_extend_high_i32x4(a.to_native()));
      else if constexpr (std::same_as<T, std::uint32_t>)
        return V::from_native(wasm_u64x2_extend_high_u32x4(a.to_native()));
    }
  }

  /// Multiply widened upper integer lanes; the complete product fits.
  template <simd_integer_element T, std::size_t N, isa<> A>
    requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16 && sizeof(T) <= 4)
  native_inline constexpr auto multiply_widened_high(simd<T, N, A> a, simd<T, N, A> b) noexcept {
    return extend_high(a) * extend_high(b);
  }

  /// Saturating concatenate from signed source lanes, including unsigned destinations.
  template <simd_integer_element To, simd_integer_element From, std::size_t N, isa<> A>
    requires(A.has(wasm_feature::simd128)) &&
            (sizeof(From) * N == 16 && sizeof(From) == 2 * sizeof(To) && sizeof(To) <= 2 &&
             std::is_signed_v<From>)
  native_inline constexpr simd<To, N * 2, A> narrow_sat(simd<From, N, A> a,
                                                        simd<From, N, A> b) noexcept {
    using V = simd<To, N * 2, A>;
    if consteval {
      auto x = detail::wasm_lanes(a), y = detail::wasm_lanes(b);
      std::array<To, N * 2> r{};
      for (std::size_t i = 0; i < N; ++i) {
        r[i] = detail::wasm_saturate<To>(x[i]);
        r[i + N] = detail::wasm_saturate<To>(y[i]);
      }
      return V::load(r.data());
    } else {
      if constexpr (std::same_as<To, std::int8_t>)
        return V::from_native(wasm_i8x16_narrow_i16x8(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<To, std::uint8_t>)
        return V::from_native(wasm_u8x16_narrow_i16x8(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<To, std::int16_t>)
        return V::from_native(wasm_i16x8_narrow_i32x4(a.to_native(), b.to_native()));
      else if constexpr (std::same_as<To, std::uint16_t>)
        return V::from_native(wasm_u16x8_narrow_i32x4(a.to_native(), b.to_native()));
    }
  }

  /// Multiply signed Q15 lanes, round by adding 2^14, shift by 15 and saturate.
  template <isa<> A>
    requires(A.has(wasm_feature::simd128))
  native_inline constexpr simd<std::int16_t, 8, A>
  q15mulr_sat(simd<std::int16_t, 8, A> a, simd<std::int16_t, 8, A> b) noexcept {
    if consteval {
      return detail::wasm_map(
          [](std::int16_t x, std::int16_t y) {
            return detail::wasm_saturate<std::int16_t>((std::int64_t(x) * y + 16384) >> 15);
          },
          a, b);
    } else {
      return simd<std::int16_t, 8, A>::from_native(
          wasm_i16x8_q15mulr_sat(a.to_native(), b.to_native()));
    }
  }

  /// Sum adjacent signed halfword products modulo 2^32.
  template <isa<> A>
    requires(A.has(wasm_feature::simd128))
  native_inline constexpr simd<std::int32_t, 4, A> dot(simd<std::int16_t, 8, A> a,
                                                       simd<std::int16_t, 8, A> b) noexcept {
    using V = simd<std::int32_t, 4, A>;
    if consteval {
      auto x = detail::wasm_lanes(a), y = detail::wasm_lanes(b);
      std::array<std::int32_t, 4> r{};
      for (std::size_t i = 0; i < 4; ++i)
        r[i] = std::bit_cast<std::int32_t>(std::uint32_t(
            std::int64_t(x[2 * i]) * y[2 * i] + std::int64_t(x[2 * i + 1]) * y[2 * i + 1]));
      return V::load(r.data());
    } else {
      return V::from_native(wasm_i32x4_dot_i16x8(a.to_native(), b.to_native()));
    }
  }

  /// Saturating floating-to-integer conversion; NaN becomes zero. Binary64 leaves the upper two
  /// result lanes zero.
  template <simd_integer_element To, std::floating_point From, std::size_t N, isa<> A>
    requires(A.has(wasm_feature::simd128)) && (sizeof(From) * N == 16 && sizeof(To) == 4)
  native_inline constexpr simd<To, 4, A> trunc_sat(simd<From, N, A> a) noexcept {
    using V = simd<To, 4, A>;
    if consteval {
      auto x = detail::wasm_lanes(a);
      std::array<To, 4> r{};
      for (std::size_t i = 0; i < N; ++i)
        r[i] = detail::wasm_trunc_sat<To>(x[i]);
      return V::load(r.data());
    } else {
      if constexpr (sizeof(From) == 4 && std::is_signed_v<To>)
        return V::from_native(wasm_i32x4_trunc_sat_f32x4(a.to_native()));
      else if constexpr (sizeof(From) == 4)
        return V::from_native(wasm_u32x4_trunc_sat_f32x4(a.to_native()));
      else if constexpr (std::is_signed_v<To>)
        return V::from_native(wasm_i32x4_trunc_sat_f64x2_zero(a.to_native()));
      else
        return V::from_native(wasm_u32x4_trunc_sat_f64x2_zero(a.to_native()));
    }
  }

  /// Native integer-to-float or float-width conversion. Double results consume the low two source
  /// lanes; float demotion zeroes its upper two lanes.
  template <std::floating_point To, detail::wasm_number From, std::size_t N, isa<> A>
    requires(A.has(wasm_feature::simd128)) &&
            (sizeof(From) * N == 16 && (sizeof(From) == 4 || std::is_floating_point_v<From>) &&
             !std::same_as<To, From>)
  native_inline constexpr simd<To, 16 / sizeof(To), A> convert(simd<From, N, A> a) noexcept {
    using V = simd<To, 16 / sizeof(To), A>;
    if consteval {
      auto x = detail::wasm_lanes(a);
      std::array<To, V::lanes> r{};
      for (std::size_t i = 0; i < std::min(N, V::lanes); ++i) {
        if constexpr (std::is_floating_point_v<From>) {
          using F = detail::wasm_format<From>;
          using G = detail::wasm_format<To>;
          r[i] = std::bit_cast<To>(detail::constexpr_float::convert_bits<G, F>(
              std::bit_cast<typename F::bits_type>(x[i])));
        } else
          r[i] = To(x[i]);
      }
      return V::load(r.data());
    } else {
      if constexpr (std::same_as<From, float>)
        return V::from_native(wasm_f64x2_promote_low_f32x4(a.to_native()));
      else if constexpr (std::same_as<From, double>)
        return V::from_native(wasm_f32x4_demote_f64x2_zero(a.to_native()));
      else if constexpr (sizeof(To) == 4 && std::is_signed_v<From>)
        return V::from_native(wasm_f32x4_convert_i32x4(a.to_native()));
      else if constexpr (sizeof(To) == 4)
        return V::from_native(wasm_f32x4_convert_u32x4(a.to_native()));
      else if constexpr (std::is_signed_v<From>)
        return V::from_native(wasm_f64x2_convert_low_i32x4(a.to_native()));
      else
        return V::from_native(wasm_f64x2_convert_low_u32x4(a.to_native()));
    }
  }

  /// Read one scalar and broadcast it; the access is exactly sizeof(T) bytes.
  template <class V>
    requires detail::wasm_number<typename V::value_type> &&
             (V::architecture.has(wasm_feature::simd128)) &&
             (V::lanes * sizeof(typename V::value_type) == 16)
  native_inline constexpr V load_splat(typename V::value_type const *p) noexcept {
    return V(*p);
  }

  /// Read one scalar into lane I, preserving the other lanes.
  template <std::size_t I, detail::wasm_number T, std::size_t N, isa<> A>
    requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16 && I < N)
  native_inline constexpr simd<T, N, A> load_lane(T const *p, simd<T, N, A> a) noexcept {
    return a.template replace<I>(*p);
  }

  /// Write only lane I to one scalar object.
  template <std::size_t I, detail::wasm_number T, std::size_t N, isa<> A>
    requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16 && I < N)
  native_inline constexpr void store_lane(T *p, simd<T, N, A> a) noexcept {
    *p = a.template get<I>();
  }

  /// Read one 32- or 64-bit lane and zero the other lanes.
  template <class V>
    requires(V::architecture.has(wasm_feature::simd128)) &&
            (V::lanes * sizeof(typename V::value_type) == 16 && sizeof(typename V::value_type) >= 4)
  native_inline constexpr V load_zero(typename V::value_type const *p) noexcept {
    if consteval {
      std::array<typename V::value_type, V::lanes> a{};
      a[0] = *p;
      return V::load(a.data());
    } else {
      if constexpr (sizeof(typename V::value_type) == 4)
        return V::from_native(wasm_v128_load32_zero(p));
      else
        return V::from_native(wasm_v128_load64_zero(p));
    }
  }

  /// Read exactly eight bytes of source lanes and widen them with their signedness.
  template <simd_integer_element To, simd_integer_element From, isa<> A>
    requires(A.has(wasm_feature::simd128)) &&
            (sizeof(To) == 2 * sizeof(From) && std::is_signed_v<To> == std::is_signed_v<From>)
  native_inline constexpr simd<To, 16 / sizeof(To), A> load_widened(From const *p) noexcept {
    using V = simd<To, 16 / sizeof(To), A>;
    if consteval {
      std::array<To, V::lanes> a{};
      for (std::size_t i = 0; i < V::lanes; ++i)
        a[i] = p[i];
      return V::load(a.data());
    } else {
      if constexpr (std::same_as<To, std::int16_t>)
        return V::from_native(wasm_i16x8_load8x8(p));
      else if constexpr (std::same_as<To, std::uint16_t>)
        return V::from_native(wasm_u16x8_load8x8(p));
      else if constexpr (std::same_as<To, std::int32_t>)
        return V::from_native(wasm_i32x4_load16x4(p));
      else if constexpr (std::same_as<To, std::uint32_t>)
        return V::from_native(wasm_u32x4_load16x4(p));
      else if constexpr (std::same_as<To, std::int64_t>)
        return V::from_native(wasm_i64x2_load32x2(p));
      else if constexpr (std::same_as<To, std::uint64_t>)
        return V::from_native(wasm_u64x2_load32x2(p));
    }
  }

  /// Signed widening pairwise sums; unsigned overloads are shared with the integer API.
  template <simd_integer_element T, std::size_t N, isa<> A>
    requires(A.has(wasm_feature::simd128)) &&
            (std::is_signed_v<T> && sizeof(T) * N == 16 && sizeof(T) <= 2)
  native_inline constexpr auto pairwise_add_widened(simd<T, N, A> a) noexcept {
    using U = std::conditional_t<sizeof(T) == 1, std::int16_t, std::int32_t>;
    using V = simd<U, N / 2, A>;
    if consteval {
      auto x = detail::wasm_lanes(a);
      std::array<U, N / 2> r{};
      for (std::size_t i = 0; i < N / 2; ++i)
        r[i] = U(x[2 * i]) + x[2 * i + 1];
      return V::load(r.data());
    } else {
      if constexpr (sizeof(T) == 1)
        return V::from_native(wasm_i16x8_extadd_pairwise_i8x16(a.to_native()));
      else
        return V::from_native(wasm_i32x4_extadd_pairwise_i16x8(a.to_native()));
    }
  }

  /// Gather each integer lane's sign bit into bit i of the scalar result.
  template <simd_integer_element T, std::size_t N, isa<> A>
    requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16)
  native_inline constexpr std::uint32_t bitmask(simd<T, N, A> a) noexcept {
    if consteval {
      auto x = detail::wasm_lanes(a);
      std::uint32_t r = 0;
      for (std::size_t i = 0; i < N; ++i)
        r |= std::uint32_t(std::make_unsigned_t<T>(x[i]) >> (sizeof(T) * 8 - 1)) << i;
      return r;
    } else {
      if constexpr (sizeof(T) == 1)
        return wasm_i8x16_bitmask(a.to_native());
      else if constexpr (sizeof(T) == 2)
        return wasm_i16x8_bitmask(a.to_native());
      else if constexpr (sizeof(T) == 4)
        return wasm_i32x4_bitmask(a.to_native());
      else
        return wasm_i64x2_bitmask(a.to_native());
    }
  }

  /// Test whether at least one integer lane is nonzero.
  template <simd_integer_element T, std::size_t N, isa<> A>
    requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16)
  native_inline constexpr bool any(simd<T, N, A> a) noexcept {
    if consteval {
      for (auto x : detail::wasm_lanes(a))
        if (x)
          return true;
      return false;
    } else {
      return wasm_v128_any_true(a.to_native());
    }
  }

  /// Test whether every integer lane is nonzero.
  template <simd_integer_element T, std::size_t N, isa<> A>
    requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16)
  native_inline constexpr bool all(simd<T, N, A> a) noexcept {
    if consteval {
      for (auto x : detail::wasm_lanes(a))
        if (!x)
          return false;
      return true;
    } else {
      if constexpr (sizeof(T) == 1)
        return wasm_i8x16_all_true(a.to_native());
      else if constexpr (sizeof(T) == 2)
        return wasm_i16x8_all_true(a.to_native());
      else if constexpr (sizeof(T) == 4)
        return wasm_i32x4_all_true(a.to_native());
      else
        return wasm_i64x2_all_true(a.to_native());
    }
  }

} // namespace native
