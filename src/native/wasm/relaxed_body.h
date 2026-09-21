// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#if NATIVE_HOST_WASM || defined(NATIVE_DOXYGEN)
export namespace native {
  /// \defgroup wasm_relaxed Relaxed SIMD
  /// Runtime calls require relaxed_simd in Arch and in the caller target.
  /// Constant evaluation chooses one valid result and does not promise engine
  /// agreement. Feature-absent tags are consteval-only and require SIMD128 storage.
  /// \{

  /// Select bytes; out-of-range indices have relaxed semantics.
  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  constexpr simd<std::uint8_t, 16, Arch> i8x16_relaxed_swizzle(simd<std::uint8_t, 16, Arch> a, simd<std::uint8_t, 16, Arch> b) noexcept {
    if consteval {
      return detail::wasm_relaxed_constant::swizzle(a, b);
    } else {
      return simd<std::uint8_t, 16, Arch>::from_native(
        detail::wasm_relaxed::i8x16_relaxed_swizzle<Arch>(a.to_native(), b.to_native()));
    }
  }

  /// Evaluate constant operands without the relaxed SIMD instruction feature.
  template<isa<wasm> Arch> requires(!Arch.has(wasm_feature::relaxed_simd) &&
      requires { sizeof(simd<std::uint8_t, 16, Arch>); })
  native_nodiscard consteval simd<std::uint8_t, 16, Arch> i8x16_relaxed_swizzle(simd<std::uint8_t, 16, Arch> a, simd<std::uint8_t, 16, Arch> b) noexcept {
    return detail::wasm_relaxed_constant::swizzle(a, b);
  }

  /// Truncate toward zero; invalid lanes may differ from saturation.
  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  constexpr simd<std::int32_t, 4, Arch> i32x4_relaxed_trunc_f32x4(simd<float, 4, Arch> a) noexcept {
    if consteval {
      return detail::wasm_relaxed_constant::truncation<simd<std::int32_t, 4, Arch>>(a);
    } else {
      return simd<std::int32_t, 4, Arch>::from_native(
        detail::wasm_relaxed::i32x4_relaxed_trunc_f32x4<Arch>(a.to_native()));
    }
  }

  /// Evaluate constant operands without the relaxed SIMD instruction feature.
  template<isa<wasm> Arch> requires(!Arch.has(wasm_feature::relaxed_simd) &&
      requires { sizeof(simd<std::int32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::int32_t, 4, Arch> i32x4_relaxed_trunc_f32x4(simd<float, 4, Arch> a) noexcept {
    return detail::wasm_relaxed_constant::truncation<simd<std::int32_t, 4, Arch>>(a);
  }

  /// Truncate toward zero; invalid lanes may differ from saturation.
  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  constexpr simd<std::uint32_t, 4, Arch> u32x4_relaxed_trunc_f32x4(simd<float, 4, Arch> a) noexcept {
    if consteval {
      return detail::wasm_relaxed_constant::truncation<simd<std::uint32_t, 4, Arch>>(a);
    } else {
      return simd<std::uint32_t, 4, Arch>::from_native(
        detail::wasm_relaxed::u32x4_relaxed_trunc_f32x4<Arch>(a.to_native()));
    }
  }

  /// Evaluate constant operands without the relaxed SIMD instruction feature.
  template<isa<wasm> Arch> requires(!Arch.has(wasm_feature::relaxed_simd) &&
      requires { sizeof(simd<std::uint32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 4, Arch> u32x4_relaxed_trunc_f32x4(simd<float, 4, Arch> a) noexcept {
    return detail::wasm_relaxed_constant::truncation<simd<std::uint32_t, 4, Arch>>(a);
  }

  /// Truncate toward zero; invalid lanes may differ from saturation.
  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  constexpr simd<std::int32_t, 4, Arch> i32x4_relaxed_trunc_f64x2_zero(simd<double, 2, Arch> a) noexcept {
    if consteval {
      return detail::wasm_relaxed_constant::truncation<simd<std::int32_t, 4, Arch>>(a);
    } else {
      return simd<std::int32_t, 4, Arch>::from_native(
        detail::wasm_relaxed::i32x4_relaxed_trunc_f64x2_zero<Arch>(a.to_native()));
    }
  }

  /// Evaluate constant operands without the relaxed SIMD instruction feature.
  template<isa<wasm> Arch> requires(!Arch.has(wasm_feature::relaxed_simd) &&
      requires { sizeof(simd<std::int32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::int32_t, 4, Arch> i32x4_relaxed_trunc_f64x2_zero(simd<double, 2, Arch> a) noexcept {
    return detail::wasm_relaxed_constant::truncation<simd<std::int32_t, 4, Arch>>(a);
  }

  /// Truncate toward zero; invalid lanes may differ from saturation.
  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  constexpr simd<std::uint32_t, 4, Arch> u32x4_relaxed_trunc_f64x2_zero(simd<double, 2, Arch> a) noexcept {
    if consteval {
      return detail::wasm_relaxed_constant::truncation<simd<std::uint32_t, 4, Arch>>(a);
    } else {
      return simd<std::uint32_t, 4, Arch>::from_native(
        detail::wasm_relaxed::u32x4_relaxed_trunc_f64x2_zero<Arch>(a.to_native()));
    }
  }

  /// Evaluate constant operands without the relaxed SIMD instruction feature.
  template<isa<wasm> Arch> requires(!Arch.has(wasm_feature::relaxed_simd) &&
      requires { sizeof(simd<std::uint32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 4, Arch> u32x4_relaxed_trunc_f64x2_zero(simd<double, 2, Arch> a) noexcept {
    return detail::wasm_relaxed_constant::truncation<simd<std::uint32_t, 4, Arch>>(a);
  }

  /// Compute a * b + c with fused or separate rounding.
  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  constexpr simd<float, 4, Arch> f32x4_relaxed_madd(simd<float, 4, Arch> a, simd<float, 4, Arch> b, simd<float, 4, Arch> c) noexcept {
    if consteval {
      return detail::wasm_relaxed_constant::multiply_add<false>(a, b, c);
    } else {
      return simd<float, 4, Arch>::from_native(
        detail::wasm_relaxed::f32x4_relaxed_madd<Arch>(a.to_native(), b.to_native(), c.to_native()));
    }
  }

  /// Evaluate constant operands without the relaxed SIMD instruction feature.
  template<isa<wasm> Arch> requires(!Arch.has(wasm_feature::relaxed_simd) &&
      requires { sizeof(simd<float, 4, Arch>); })
  native_nodiscard consteval simd<float, 4, Arch> f32x4_relaxed_madd(simd<float, 4, Arch> a, simd<float, 4, Arch> b, simd<float, 4, Arch> c) noexcept {
    return detail::wasm_relaxed_constant::multiply_add<false>(a, b, c);
  }

  /// Compute -(a * b) + c with fused or separate rounding.
  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  constexpr simd<float, 4, Arch> f32x4_relaxed_nmadd(simd<float, 4, Arch> a, simd<float, 4, Arch> b, simd<float, 4, Arch> c) noexcept {
    if consteval {
      return detail::wasm_relaxed_constant::multiply_add<true>(a, b, c);
    } else {
      return simd<float, 4, Arch>::from_native(
        detail::wasm_relaxed::f32x4_relaxed_nmadd<Arch>(a.to_native(), b.to_native(), c.to_native()));
    }
  }

  /// Evaluate constant operands without the relaxed SIMD instruction feature.
  template<isa<wasm> Arch> requires(!Arch.has(wasm_feature::relaxed_simd) &&
      requires { sizeof(simd<float, 4, Arch>); })
  native_nodiscard consteval simd<float, 4, Arch> f32x4_relaxed_nmadd(simd<float, 4, Arch> a, simd<float, 4, Arch> b, simd<float, 4, Arch> c) noexcept {
    return detail::wasm_relaxed_constant::multiply_add<true>(a, b, c);
  }

  /// Compute a * b + c with fused or separate rounding.
  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  constexpr simd<double, 2, Arch> f64x2_relaxed_madd(simd<double, 2, Arch> a, simd<double, 2, Arch> b, simd<double, 2, Arch> c) noexcept {
    if consteval {
      return detail::wasm_relaxed_constant::multiply_add<false>(a, b, c);
    } else {
      return simd<double, 2, Arch>::from_native(
        detail::wasm_relaxed::f64x2_relaxed_madd<Arch>(a.to_native(), b.to_native(), c.to_native()));
    }
  }

  /// Evaluate constant operands without the relaxed SIMD instruction feature.
  template<isa<wasm> Arch> requires(!Arch.has(wasm_feature::relaxed_simd) &&
      requires { sizeof(simd<double, 2, Arch>); })
  native_nodiscard consteval simd<double, 2, Arch> f64x2_relaxed_madd(simd<double, 2, Arch> a, simd<double, 2, Arch> b, simd<double, 2, Arch> c) noexcept {
    return detail::wasm_relaxed_constant::multiply_add<false>(a, b, c);
  }

  /// Compute -(a * b) + c with fused or separate rounding.
  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  constexpr simd<double, 2, Arch> f64x2_relaxed_nmadd(simd<double, 2, Arch> a, simd<double, 2, Arch> b, simd<double, 2, Arch> c) noexcept {
    if consteval {
      return detail::wasm_relaxed_constant::multiply_add<true>(a, b, c);
    } else {
      return simd<double, 2, Arch>::from_native(
        detail::wasm_relaxed::f64x2_relaxed_nmadd<Arch>(a.to_native(), b.to_native(), c.to_native()));
    }
  }

  /// Evaluate constant operands without the relaxed SIMD instruction feature.
  template<isa<wasm> Arch> requires(!Arch.has(wasm_feature::relaxed_simd) &&
      requires { sizeof(simd<double, 2, Arch>); })
  native_nodiscard consteval simd<double, 2, Arch> f64x2_relaxed_nmadd(simd<double, 2, Arch> a, simd<double, 2, Arch> b, simd<double, 2, Arch> c) noexcept {
    return detail::wasm_relaxed_constant::multiply_add<true>(a, b, c);
  }

  /// Select with mask c; partial masks permit bit or sign-bit lane selection.
  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  constexpr simd<std::uint8_t, 16, Arch> i8x16_relaxed_laneselect(
    simd<std::uint8_t, 16, Arch> a,
    simd<std::uint8_t, 16, Arch> b,
    simd<std::uint8_t, 16, Arch> c) noexcept {
    if consteval {
      return detail::wasm_relaxed_constant::lane_select(a, b, c);
    } else {
      return simd<std::uint8_t, 16, Arch>::from_native(
        detail::wasm_relaxed::i8x16_relaxed_laneselect<Arch>(a.to_native(), b.to_native(), c.to_native()));
    }
  }

  /// Evaluate constant operands without the relaxed SIMD instruction feature.
  template<isa<wasm> Arch> requires(!Arch.has(wasm_feature::relaxed_simd) &&
      requires { sizeof(simd<std::uint8_t, 16, Arch>); })
  native_nodiscard consteval simd<std::uint8_t, 16, Arch> i8x16_relaxed_laneselect(
    simd<std::uint8_t, 16, Arch> a,
    simd<std::uint8_t, 16, Arch> b,
    simd<std::uint8_t, 16, Arch> c) noexcept {
    return detail::wasm_relaxed_constant::lane_select(a, b, c);
  }

  /// Select with mask c; partial masks permit bit or sign-bit lane selection.
  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  constexpr simd<std::uint16_t, 8, Arch> i16x8_relaxed_laneselect(
    simd<std::uint16_t, 8, Arch> a,
    simd<std::uint16_t, 8, Arch> b,
    simd<std::uint16_t, 8, Arch> c) noexcept {
    if consteval {
      return detail::wasm_relaxed_constant::lane_select(a, b, c);
    } else {
      return simd<std::uint16_t, 8, Arch>::from_native(
        detail::wasm_relaxed::i16x8_relaxed_laneselect<Arch>(a.to_native(), b.to_native(), c.to_native()));
    }
  }

  /// Evaluate constant operands without the relaxed SIMD instruction feature.
  template<isa<wasm> Arch> requires(!Arch.has(wasm_feature::relaxed_simd) &&
      requires { sizeof(simd<std::uint16_t, 8, Arch>); })
  native_nodiscard consteval simd<std::uint16_t, 8, Arch> i16x8_relaxed_laneselect(
    simd<std::uint16_t, 8, Arch> a,
    simd<std::uint16_t, 8, Arch> b,
    simd<std::uint16_t, 8, Arch> c) noexcept {
    return detail::wasm_relaxed_constant::lane_select(a, b, c);
  }

  /// Select with mask c; partial masks permit bit or sign-bit lane selection.
  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  constexpr simd<std::uint32_t, 4, Arch> i32x4_relaxed_laneselect(
    simd<std::uint32_t, 4, Arch> a,
    simd<std::uint32_t, 4, Arch> b,
    simd<std::uint32_t, 4, Arch> c) noexcept {
    if consteval {
      return detail::wasm_relaxed_constant::lane_select(a, b, c);
    } else {
      return simd<std::uint32_t, 4, Arch>::from_native(
        detail::wasm_relaxed::i32x4_relaxed_laneselect<Arch>(a.to_native(), b.to_native(), c.to_native()));
    }
  }

  /// Evaluate constant operands without the relaxed SIMD instruction feature.
  template<isa<wasm> Arch> requires(!Arch.has(wasm_feature::relaxed_simd) &&
      requires { sizeof(simd<std::uint32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 4, Arch> i32x4_relaxed_laneselect(
    simd<std::uint32_t, 4, Arch> a,
    simd<std::uint32_t, 4, Arch> b,
    simd<std::uint32_t, 4, Arch> c) noexcept {
    return detail::wasm_relaxed_constant::lane_select(a, b, c);
  }

  /// Select with mask c; partial masks permit bit or sign-bit lane selection.
  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  constexpr simd<std::uint64_t, 2, Arch> i64x2_relaxed_laneselect(
    simd<std::uint64_t, 2, Arch> a,
    simd<std::uint64_t, 2, Arch> b,
    simd<std::uint64_t, 2, Arch> c) noexcept {
    if consteval {
      return detail::wasm_relaxed_constant::lane_select(a, b, c);
    } else {
      return simd<std::uint64_t, 2, Arch>::from_native(
        detail::wasm_relaxed::i64x2_relaxed_laneselect<Arch>(a.to_native(), b.to_native(), c.to_native()));
    }
  }

  /// Evaluate constant operands without the relaxed SIMD instruction feature.
  template<isa<wasm> Arch> requires(!Arch.has(wasm_feature::relaxed_simd) &&
      requires { sizeof(simd<std::uint64_t, 2, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 2, Arch> i64x2_relaxed_laneselect(
    simd<std::uint64_t, 2, Arch> a,
    simd<std::uint64_t, 2, Arch> b,
    simd<std::uint64_t, 2, Arch> c) noexcept {
    return detail::wasm_relaxed_constant::lane_select(a, b, c);
  }

  /// Minimum with relaxed NaN and signed-zero selection.
  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  constexpr simd<float, 4, Arch> f32x4_relaxed_min(simd<float, 4, Arch> a, simd<float, 4, Arch> b) noexcept {
    if consteval {
      return detail::wasm_relaxed_constant::minimum_maximum<false>(a, b);
    } else {
      return simd<float, 4, Arch>::from_native(
        detail::wasm_relaxed::f32x4_relaxed_min<Arch>(a.to_native(), b.to_native()));
    }
  }

  /// Evaluate constant operands without the relaxed SIMD instruction feature.
  template<isa<wasm> Arch> requires(!Arch.has(wasm_feature::relaxed_simd) &&
      requires { sizeof(simd<float, 4, Arch>); })
  native_nodiscard consteval simd<float, 4, Arch> f32x4_relaxed_min(simd<float, 4, Arch> a, simd<float, 4, Arch> b) noexcept {
    return detail::wasm_relaxed_constant::minimum_maximum<false>(a, b);
  }

  /// Maximum with relaxed NaN and signed-zero selection.
  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  constexpr simd<float, 4, Arch> f32x4_relaxed_max(simd<float, 4, Arch> a, simd<float, 4, Arch> b) noexcept {
    if consteval {
      return detail::wasm_relaxed_constant::minimum_maximum<true>(a, b);
    } else {
      return simd<float, 4, Arch>::from_native(
        detail::wasm_relaxed::f32x4_relaxed_max<Arch>(a.to_native(), b.to_native()));
    }
  }

  /// Evaluate constant operands without the relaxed SIMD instruction feature.
  template<isa<wasm> Arch> requires(!Arch.has(wasm_feature::relaxed_simd) &&
      requires { sizeof(simd<float, 4, Arch>); })
  native_nodiscard consteval simd<float, 4, Arch> f32x4_relaxed_max(simd<float, 4, Arch> a, simd<float, 4, Arch> b) noexcept {
    return detail::wasm_relaxed_constant::minimum_maximum<true>(a, b);
  }

  /// Minimum with relaxed NaN and signed-zero selection.
  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  constexpr simd<double, 2, Arch> f64x2_relaxed_min(simd<double, 2, Arch> a, simd<double, 2, Arch> b) noexcept {
    if consteval {
      return detail::wasm_relaxed_constant::minimum_maximum<false>(a, b);
    } else {
      return simd<double, 2, Arch>::from_native(
        detail::wasm_relaxed::f64x2_relaxed_min<Arch>(a.to_native(), b.to_native()));
    }
  }

  /// Evaluate constant operands without the relaxed SIMD instruction feature.
  template<isa<wasm> Arch> requires(!Arch.has(wasm_feature::relaxed_simd) &&
      requires { sizeof(simd<double, 2, Arch>); })
  native_nodiscard consteval simd<double, 2, Arch> f64x2_relaxed_min(simd<double, 2, Arch> a, simd<double, 2, Arch> b) noexcept {
    return detail::wasm_relaxed_constant::minimum_maximum<false>(a, b);
  }

  /// Maximum with relaxed NaN and signed-zero selection.
  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  constexpr simd<double, 2, Arch> f64x2_relaxed_max(simd<double, 2, Arch> a, simd<double, 2, Arch> b) noexcept {
    if consteval {
      return detail::wasm_relaxed_constant::minimum_maximum<true>(a, b);
    } else {
      return simd<double, 2, Arch>::from_native(
        detail::wasm_relaxed::f64x2_relaxed_max<Arch>(a.to_native(), b.to_native()));
    }
  }

  /// Evaluate constant operands without the relaxed SIMD instruction feature.
  template<isa<wasm> Arch> requires(!Arch.has(wasm_feature::relaxed_simd) &&
      requires { sizeof(simd<double, 2, Arch>); })
  native_nodiscard consteval simd<double, 2, Arch> f64x2_relaxed_max(simd<double, 2, Arch> a, simd<double, 2, Arch> b) noexcept {
    return detail::wasm_relaxed_constant::minimum_maximum<true>(a, b);
  }

  /// Rounded Q15 product; the overflowing -32768 squared case is relaxed.
  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  constexpr simd<std::int16_t, 8, Arch> i16x8_relaxed_q15mulr(simd<std::int16_t, 8, Arch> a, simd<std::int16_t, 8, Arch> b) noexcept {
    if consteval {
      return detail::wasm_relaxed_constant::q15_multiply(a, b);
    } else {
      return simd<std::int16_t, 8, Arch>::from_native(
        detail::wasm_relaxed::i16x8_relaxed_q15mulr<Arch>(a.to_native(), b.to_native()));
    }
  }

  /// Evaluate constant operands without the relaxed SIMD instruction feature.
  template<isa<wasm> Arch> requires(!Arch.has(wasm_feature::relaxed_simd) &&
      requires { sizeof(simd<std::int16_t, 8, Arch>); })
  native_nodiscard consteval simd<std::int16_t, 8, Arch> i16x8_relaxed_q15mulr(simd<std::int16_t, 8, Arch> a, simd<std::int16_t, 8, Arch> b) noexcept {
    return detail::wasm_relaxed_constant::q15_multiply(a, b);
  }

  /// Pairwise signed-byte dot product; high bits of b have relaxed semantics.
  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  constexpr simd<std::int16_t, 8, Arch> i16x8_relaxed_dot_i8x16_i7x16(simd<std::int8_t, 16, Arch> a, simd<std::uint8_t, 16, Arch> b) noexcept {
    if consteval {
      return detail::wasm_relaxed_constant::dot<simd<std::int16_t, 8, Arch>>(a, b);
    } else {
      return simd<std::int16_t, 8, Arch>::from_native(
        detail::wasm_relaxed::i16x8_relaxed_dot_i8x16_i7x16<Arch>(a.to_native(), b.to_native()));
    }
  }

  /// Evaluate constant operands without the relaxed SIMD instruction feature.
  template<isa<wasm> Arch> requires(!Arch.has(wasm_feature::relaxed_simd) &&
      requires { sizeof(simd<std::int16_t, 8, Arch>); })
  native_nodiscard consteval simd<std::int16_t, 8, Arch> i16x8_relaxed_dot_i8x16_i7x16(simd<std::int8_t, 16, Arch> a, simd<std::uint8_t, 16, Arch> b) noexcept {
    return detail::wasm_relaxed_constant::dot<simd<std::int16_t, 8, Arch>>(a, b);
  }

  /// Accumulate two adjacent relaxed dot pairs into c modulo 32 bits.
  template<isa<wasm> Arch> requires(Arch.has(wasm_feature::relaxed_simd))
  native_nodiscard native_inline native_target("relaxed-simd")
  constexpr simd<std::int32_t, 4, Arch> i32x4_relaxed_dot_i8x16_i7x16_add(
    simd<std::int8_t, 16, Arch> a,
    simd<std::uint8_t, 16, Arch> b,
    simd<std::int32_t, 4, Arch> c) noexcept {
    if consteval {
      return detail::wasm_relaxed_constant::dot_add(a, b, c);
    } else {
      return simd<std::int32_t, 4, Arch>::from_native(
        detail::wasm_relaxed::i32x4_relaxed_dot_i8x16_i7x16_add<Arch>(a.to_native(), b.to_native(), c.to_native()));
    }
  }

  /// Evaluate constant operands without the relaxed SIMD instruction feature.
  template<isa<wasm> Arch> requires(!Arch.has(wasm_feature::relaxed_simd) &&
      requires { sizeof(simd<std::int32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::int32_t, 4, Arch> i32x4_relaxed_dot_i8x16_i7x16_add(
    simd<std::int8_t, 16, Arch> a,
    simd<std::uint8_t, 16, Arch> b,
    simd<std::int32_t, 4, Arch> c) noexcept {
    return detail::wasm_relaxed_constant::dot_add(a, b, c);
  }

  /// \}
}
#endif
