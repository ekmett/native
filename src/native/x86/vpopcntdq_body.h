// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#if NATIVE_HOST_X86 || defined(NATIVE_DOXYGEN)
export namespace native {
/** \defgroup x86_vpopcntdq VPOPCNTDQ
 * Population counts of independent 32-bit and 64-bit integer lanes.
 * Arch must contain AVX512F and AVX512VPOPCNTDQ; 128/256-bit forms also
 * require AVX512VL. Admit CPU features and OS ZMM state before entering
 * a matching target scope. These operations do not access memory or flags.
 * \{ */

  /// Count set bits in each of the 16 32-bit lanes; results are in [0, 32].
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq")
  simd<std::uint32_t, 16, Arch> vpopcntd(simd<std::uint32_t, 16, Arch> value) noexcept {
    return simd<std::uint32_t, 16, Arch>::from_native(detail::x86_vpopcntdq::vpopcntd<Arch>(value.to_native()));
  }

  /// Inactive lanes retain source; mask bits above the lane count are ignored.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq")
  simd<std::uint32_t, 16, Arch> mask_vpopcntd(simd<std::uint32_t, 16, Arch> source, predicate<16, Arch> mask, simd<std::uint32_t, 16, Arch> value) noexcept {
    return simd<std::uint32_t, 16, Arch>::from_native(detail::x86_vpopcntdq::mask_vpopcntd<Arch>(source.to_native(), mask.to_bitset(), value.to_native()));
  }

  /// Inactive lanes become zero; mask bits above the lane count are ignored.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq")
  simd<std::uint32_t, 16, Arch> maskz_vpopcntd(predicate<16, Arch> mask, simd<std::uint32_t, 16, Arch> value) noexcept {
    return simd<std::uint32_t, 16, Arch>::from_native(detail::x86_vpopcntdq::maskz_vpopcntd<Arch>(mask.to_bitset(), value.to_native()));
  }

  /// Count set bits in each of the 8 64-bit lanes; results are in [0, 64].
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq")
  simd<std::uint64_t, 8, Arch> vpopcntq(simd<std::uint64_t, 8, Arch> value) noexcept {
    return simd<std::uint64_t, 8, Arch>::from_native(detail::x86_vpopcntdq::vpopcntq<Arch>(value.to_native()));
  }

  /// Inactive lanes retain source; mask bits above the lane count are ignored.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq")
  simd<std::uint64_t, 8, Arch> mask_vpopcntq(simd<std::uint64_t, 8, Arch> source, predicate<8, Arch> mask, simd<std::uint64_t, 8, Arch> value) noexcept {
    return simd<std::uint64_t, 8, Arch>::from_native(detail::x86_vpopcntdq::mask_vpopcntq<Arch>(source.to_native(), mask.to_bitset(), value.to_native()));
  }

  /// Inactive lanes become zero; mask bits above the lane count are ignored.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq")
  simd<std::uint64_t, 8, Arch> maskz_vpopcntq(predicate<8, Arch> mask, simd<std::uint64_t, 8, Arch> value) noexcept {
    return simd<std::uint64_t, 8, Arch>::from_native(detail::x86_vpopcntdq::maskz_vpopcntq<Arch>(mask.to_bitset(), value.to_native()));
  }

  /// Count set bits in each of the 4 32-bit lanes; results are in [0, 32].
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq,avx512vl")
  simd<std::uint32_t, 4, Arch> vpopcntd(simd<std::uint32_t, 4, Arch> value) noexcept {
    return simd<std::uint32_t, 4, Arch>::from_native(detail::x86_vpopcntdq::vpopcntd<Arch>(value.to_native()));
  }

  /// Inactive lanes retain source; mask bits above the lane count are ignored.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq,avx512vl")
  simd<std::uint32_t, 4, Arch> mask_vpopcntd(simd<std::uint32_t, 4, Arch> source, predicate<4, Arch> mask, simd<std::uint32_t, 4, Arch> value) noexcept {
    return simd<std::uint32_t, 4, Arch>::from_native(detail::x86_vpopcntdq::mask_vpopcntd<Arch>(source.to_native(), mask.to_bitset(), value.to_native()));
  }

  /// Inactive lanes become zero; mask bits above the lane count are ignored.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq,avx512vl")
  simd<std::uint32_t, 4, Arch> maskz_vpopcntd(predicate<4, Arch> mask, simd<std::uint32_t, 4, Arch> value) noexcept {
    return simd<std::uint32_t, 4, Arch>::from_native(detail::x86_vpopcntdq::maskz_vpopcntd<Arch>(mask.to_bitset(), value.to_native()));
  }

  /// Count set bits in each of the 2 64-bit lanes; results are in [0, 64].
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq,avx512vl")
  simd<std::uint64_t, 2, Arch> vpopcntq(simd<std::uint64_t, 2, Arch> value) noexcept {
    return simd<std::uint64_t, 2, Arch>::from_native(detail::x86_vpopcntdq::vpopcntq<Arch>(value.to_native()));
  }

  /// Inactive lanes retain source; mask bits above the lane count are ignored.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq,avx512vl")
  simd<std::uint64_t, 2, Arch> mask_vpopcntq(simd<std::uint64_t, 2, Arch> source, predicate<2, Arch> mask, simd<std::uint64_t, 2, Arch> value) noexcept {
    return simd<std::uint64_t, 2, Arch>::from_native(detail::x86_vpopcntdq::mask_vpopcntq<Arch>(source.to_native(), mask.to_bitset(), value.to_native()));
  }

  /// Inactive lanes become zero; mask bits above the lane count are ignored.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq,avx512vl")
  simd<std::uint64_t, 2, Arch> maskz_vpopcntq(predicate<2, Arch> mask, simd<std::uint64_t, 2, Arch> value) noexcept {
    return simd<std::uint64_t, 2, Arch>::from_native(detail::x86_vpopcntdq::maskz_vpopcntq<Arch>(mask.to_bitset(), value.to_native()));
  }

  /// Count set bits in each of the 8 32-bit lanes; results are in [0, 32].
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq,avx512vl")
  simd<std::uint32_t, 8, Arch> vpopcntd(simd<std::uint32_t, 8, Arch> value) noexcept {
    return simd<std::uint32_t, 8, Arch>::from_native(detail::x86_vpopcntdq::vpopcntd<Arch>(value.to_native()));
  }

  /// Inactive lanes retain source; mask bits above the lane count are ignored.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq,avx512vl")
  simd<std::uint32_t, 8, Arch> mask_vpopcntd(simd<std::uint32_t, 8, Arch> source, predicate<8, Arch> mask, simd<std::uint32_t, 8, Arch> value) noexcept {
    return simd<std::uint32_t, 8, Arch>::from_native(detail::x86_vpopcntdq::mask_vpopcntd<Arch>(source.to_native(), mask.to_bitset(), value.to_native()));
  }

  /// Inactive lanes become zero; mask bits above the lane count are ignored.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq,avx512vl")
  simd<std::uint32_t, 8, Arch> maskz_vpopcntd(predicate<8, Arch> mask, simd<std::uint32_t, 8, Arch> value) noexcept {
    return simd<std::uint32_t, 8, Arch>::from_native(detail::x86_vpopcntdq::maskz_vpopcntd<Arch>(mask.to_bitset(), value.to_native()));
  }

  /// Count set bits in each of the 4 64-bit lanes; results are in [0, 64].
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq,avx512vl")
  simd<std::uint64_t, 4, Arch> vpopcntq(simd<std::uint64_t, 4, Arch> value) noexcept {
    return simd<std::uint64_t, 4, Arch>::from_native(detail::x86_vpopcntdq::vpopcntq<Arch>(value.to_native()));
  }

  /// Inactive lanes retain source; mask bits above the lane count are ignored.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq,avx512vl")
  simd<std::uint64_t, 4, Arch> mask_vpopcntq(simd<std::uint64_t, 4, Arch> source, predicate<4, Arch> mask, simd<std::uint64_t, 4, Arch> value) noexcept {
    return simd<std::uint64_t, 4, Arch>::from_native(detail::x86_vpopcntdq::mask_vpopcntq<Arch>(source.to_native(), mask.to_bitset(), value.to_native()));
  }

  /// Inactive lanes become zero; mask bits above the lane count are ignored.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vpopcntdq) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vpopcntdq,avx512vl")
  simd<std::uint64_t, 4, Arch> maskz_vpopcntq(predicate<4, Arch> mask, simd<std::uint64_t, 4, Arch> value) noexcept {
    return simd<std::uint64_t, 4, Arch>::from_native(detail::x86_vpopcntdq::maskz_vpopcntq<Arch>(mask.to_bitset(), value.to_native()));
  }
  // Reject implicit register conversions, mixed tags and wrong element types.
  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, class... Args>
  void vpopcntd(Args...) = delete;
  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, class... Args>
  void mask_vpopcntd(Args...) = delete;
  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, class... Args>
  void maskz_vpopcntd(Args...) = delete;
  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, class... Args>
  void vpopcntq(Args...) = delete;
  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, class... Args>
  void mask_vpopcntq(Args...) = delete;
  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, class... Args>
  void maskz_vpopcntq(Args...) = delete;

/// \}
}
#endif
