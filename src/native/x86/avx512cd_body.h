// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#if NATIVE_HOST_X86 || defined(NATIVE_DOXYGEN)
export namespace native {
/** \defgroup x86_avx512cd AVX512CD
 * Conflict detection and leading-zero counts for 32-bit and 64-bit lanes.
 * Runtime calls require AVX512F and AVX512CD; 128/256-bit forms also require
 * AVX512VL. Admit CPU features and OS ZMM state before a matching target scope.
 * Conflict bits refer to every earlier source lane, including masked-off lanes.
 * Masks affect destination lanes only. Zero inputs have 32/64 leading zeros.
 * Constant evaluation uses exact integer semantics. Tags without instruction
 * features are compile-time-only and require complete SIMD storage.
 * \{ */

  /// For each 32-bit lane, set bit j when earlier source lane j is equal.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  constexpr simd<std::uint32_t, 4, Arch> vpconflictd(simd<std::uint32_t, 4, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<true>(value, simd<std::uint32_t, 4, Arch>{}, ~std::uint64_t{0}); }
    else { return simd<std::uint32_t, 4, Arch>::from_native(detail::x86_avx512cd::vpconflictd<Arch>(value.to_native())); }
  }

  /// Inactive destination lanes retain source; every input lane still participates.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  constexpr simd<std::uint32_t, 4, Arch> mask_vpconflictd(simd<std::uint32_t, 4, Arch> source, predicate<4, Arch> mask, simd<std::uint32_t, 4, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<true>(value, source, mask.to_bitset()); }
    else { return simd<std::uint32_t, 4, Arch>::from_native(detail::x86_avx512cd::mask_vpconflictd<Arch>(source.to_native(), mask.to_bitset(), value.to_native())); }
  }

  /// Inactive destination lanes become zero; every input lane still participates.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  constexpr simd<std::uint32_t, 4, Arch> maskz_vpconflictd(predicate<4, Arch> mask, simd<std::uint32_t, 4, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<true>(value, simd<std::uint32_t, 4, Arch>{}, mask.to_bitset()); }
    else { return simd<std::uint32_t, 4, Arch>::from_native(detail::x86_avx512cd::maskz_vpconflictd<Arch>(mask.to_bitset(), value.to_native())); }
  }

  /// Count leading zeros in each 32-bit lane; zero yields 32.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  constexpr simd<std::uint32_t, 4, Arch> vplzcntd(simd<std::uint32_t, 4, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<false>(value, simd<std::uint32_t, 4, Arch>{}, ~std::uint64_t{0}); }
    else { return simd<std::uint32_t, 4, Arch>::from_native(detail::x86_avx512cd::vplzcntd<Arch>(value.to_native())); }
  }

  /// Inactive destination lanes retain source; zero input yields the lane width.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  constexpr simd<std::uint32_t, 4, Arch> mask_vplzcntd(simd<std::uint32_t, 4, Arch> source, predicate<4, Arch> mask, simd<std::uint32_t, 4, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<false>(value, source, mask.to_bitset()); }
    else { return simd<std::uint32_t, 4, Arch>::from_native(detail::x86_avx512cd::mask_vplzcntd<Arch>(source.to_native(), mask.to_bitset(), value.to_native())); }
  }

  /// Inactive destination lanes become zero; zero input yields the lane width.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  constexpr simd<std::uint32_t, 4, Arch> maskz_vplzcntd(predicate<4, Arch> mask, simd<std::uint32_t, 4, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<false>(value, simd<std::uint32_t, 4, Arch>{}, mask.to_bitset()); }
    else { return simd<std::uint32_t, 4, Arch>::from_native(detail::x86_avx512cd::maskz_vplzcntd<Arch>(mask.to_bitset(), value.to_native())); }
  }

  /// For each 64-bit lane, set bit j when earlier source lane j is equal.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  constexpr simd<std::uint64_t, 2, Arch> vpconflictq(simd<std::uint64_t, 2, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<true>(value, simd<std::uint64_t, 2, Arch>{}, ~std::uint64_t{0}); }
    else { return simd<std::uint64_t, 2, Arch>::from_native(detail::x86_avx512cd::vpconflictq<Arch>(value.to_native())); }
  }

  /// Inactive destination lanes retain source; every input lane still participates.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  constexpr simd<std::uint64_t, 2, Arch> mask_vpconflictq(simd<std::uint64_t, 2, Arch> source, predicate<2, Arch> mask, simd<std::uint64_t, 2, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<true>(value, source, mask.to_bitset()); }
    else { return simd<std::uint64_t, 2, Arch>::from_native(detail::x86_avx512cd::mask_vpconflictq<Arch>(source.to_native(), mask.to_bitset(), value.to_native())); }
  }

  /// Inactive destination lanes become zero; every input lane still participates.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  constexpr simd<std::uint64_t, 2, Arch> maskz_vpconflictq(predicate<2, Arch> mask, simd<std::uint64_t, 2, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<true>(value, simd<std::uint64_t, 2, Arch>{}, mask.to_bitset()); }
    else { return simd<std::uint64_t, 2, Arch>::from_native(detail::x86_avx512cd::maskz_vpconflictq<Arch>(mask.to_bitset(), value.to_native())); }
  }

  /// Count leading zeros in each 64-bit lane; zero yields 64.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  constexpr simd<std::uint64_t, 2, Arch> vplzcntq(simd<std::uint64_t, 2, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<false>(value, simd<std::uint64_t, 2, Arch>{}, ~std::uint64_t{0}); }
    else { return simd<std::uint64_t, 2, Arch>::from_native(detail::x86_avx512cd::vplzcntq<Arch>(value.to_native())); }
  }

  /// Inactive destination lanes retain source; zero input yields the lane width.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  constexpr simd<std::uint64_t, 2, Arch> mask_vplzcntq(simd<std::uint64_t, 2, Arch> source, predicate<2, Arch> mask, simd<std::uint64_t, 2, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<false>(value, source, mask.to_bitset()); }
    else { return simd<std::uint64_t, 2, Arch>::from_native(detail::x86_avx512cd::mask_vplzcntq<Arch>(source.to_native(), mask.to_bitset(), value.to_native())); }
  }

  /// Inactive destination lanes become zero; zero input yields the lane width.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  constexpr simd<std::uint64_t, 2, Arch> maskz_vplzcntq(predicate<2, Arch> mask, simd<std::uint64_t, 2, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<false>(value, simd<std::uint64_t, 2, Arch>{}, mask.to_bitset()); }
    else { return simd<std::uint64_t, 2, Arch>::from_native(detail::x86_avx512cd::maskz_vplzcntq<Arch>(mask.to_bitset(), value.to_native())); }
  }

  /// For each 32-bit lane, set bit j when earlier source lane j is equal.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  constexpr simd<std::uint32_t, 8, Arch> vpconflictd(simd<std::uint32_t, 8, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<true>(value, simd<std::uint32_t, 8, Arch>{}, ~std::uint64_t{0}); }
    else { return simd<std::uint32_t, 8, Arch>::from_native(detail::x86_avx512cd::vpconflictd<Arch>(value.to_native())); }
  }

  /// Inactive destination lanes retain source; every input lane still participates.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  constexpr simd<std::uint32_t, 8, Arch> mask_vpconflictd(simd<std::uint32_t, 8, Arch> source, predicate<8, Arch> mask, simd<std::uint32_t, 8, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<true>(value, source, mask.to_bitset()); }
    else { return simd<std::uint32_t, 8, Arch>::from_native(detail::x86_avx512cd::mask_vpconflictd<Arch>(source.to_native(), mask.to_bitset(), value.to_native())); }
  }

  /// Inactive destination lanes become zero; every input lane still participates.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  constexpr simd<std::uint32_t, 8, Arch> maskz_vpconflictd(predicate<8, Arch> mask, simd<std::uint32_t, 8, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<true>(value, simd<std::uint32_t, 8, Arch>{}, mask.to_bitset()); }
    else { return simd<std::uint32_t, 8, Arch>::from_native(detail::x86_avx512cd::maskz_vpconflictd<Arch>(mask.to_bitset(), value.to_native())); }
  }

  /// Count leading zeros in each 32-bit lane; zero yields 32.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  constexpr simd<std::uint32_t, 8, Arch> vplzcntd(simd<std::uint32_t, 8, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<false>(value, simd<std::uint32_t, 8, Arch>{}, ~std::uint64_t{0}); }
    else { return simd<std::uint32_t, 8, Arch>::from_native(detail::x86_avx512cd::vplzcntd<Arch>(value.to_native())); }
  }

  /// Inactive destination lanes retain source; zero input yields the lane width.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  constexpr simd<std::uint32_t, 8, Arch> mask_vplzcntd(simd<std::uint32_t, 8, Arch> source, predicate<8, Arch> mask, simd<std::uint32_t, 8, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<false>(value, source, mask.to_bitset()); }
    else { return simd<std::uint32_t, 8, Arch>::from_native(detail::x86_avx512cd::mask_vplzcntd<Arch>(source.to_native(), mask.to_bitset(), value.to_native())); }
  }

  /// Inactive destination lanes become zero; zero input yields the lane width.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  constexpr simd<std::uint32_t, 8, Arch> maskz_vplzcntd(predicate<8, Arch> mask, simd<std::uint32_t, 8, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<false>(value, simd<std::uint32_t, 8, Arch>{}, mask.to_bitset()); }
    else { return simd<std::uint32_t, 8, Arch>::from_native(detail::x86_avx512cd::maskz_vplzcntd<Arch>(mask.to_bitset(), value.to_native())); }
  }

  /// For each 64-bit lane, set bit j when earlier source lane j is equal.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  constexpr simd<std::uint64_t, 4, Arch> vpconflictq(simd<std::uint64_t, 4, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<true>(value, simd<std::uint64_t, 4, Arch>{}, ~std::uint64_t{0}); }
    else { return simd<std::uint64_t, 4, Arch>::from_native(detail::x86_avx512cd::vpconflictq<Arch>(value.to_native())); }
  }

  /// Inactive destination lanes retain source; every input lane still participates.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  constexpr simd<std::uint64_t, 4, Arch> mask_vpconflictq(simd<std::uint64_t, 4, Arch> source, predicate<4, Arch> mask, simd<std::uint64_t, 4, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<true>(value, source, mask.to_bitset()); }
    else { return simd<std::uint64_t, 4, Arch>::from_native(detail::x86_avx512cd::mask_vpconflictq<Arch>(source.to_native(), mask.to_bitset(), value.to_native())); }
  }

  /// Inactive destination lanes become zero; every input lane still participates.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  constexpr simd<std::uint64_t, 4, Arch> maskz_vpconflictq(predicate<4, Arch> mask, simd<std::uint64_t, 4, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<true>(value, simd<std::uint64_t, 4, Arch>{}, mask.to_bitset()); }
    else { return simd<std::uint64_t, 4, Arch>::from_native(detail::x86_avx512cd::maskz_vpconflictq<Arch>(mask.to_bitset(), value.to_native())); }
  }

  /// Count leading zeros in each 64-bit lane; zero yields 64.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  constexpr simd<std::uint64_t, 4, Arch> vplzcntq(simd<std::uint64_t, 4, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<false>(value, simd<std::uint64_t, 4, Arch>{}, ~std::uint64_t{0}); }
    else { return simd<std::uint64_t, 4, Arch>::from_native(detail::x86_avx512cd::vplzcntq<Arch>(value.to_native())); }
  }

  /// Inactive destination lanes retain source; zero input yields the lane width.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  constexpr simd<std::uint64_t, 4, Arch> mask_vplzcntq(simd<std::uint64_t, 4, Arch> source, predicate<4, Arch> mask, simd<std::uint64_t, 4, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<false>(value, source, mask.to_bitset()); }
    else { return simd<std::uint64_t, 4, Arch>::from_native(detail::x86_avx512cd::mask_vplzcntq<Arch>(source.to_native(), mask.to_bitset(), value.to_native())); }
  }

  /// Inactive destination lanes become zero; zero input yields the lane width.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd,avx512vl")
  constexpr simd<std::uint64_t, 4, Arch> maskz_vplzcntq(predicate<4, Arch> mask, simd<std::uint64_t, 4, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<false>(value, simd<std::uint64_t, 4, Arch>{}, mask.to_bitset()); }
    else { return simd<std::uint64_t, 4, Arch>::from_native(detail::x86_avx512cd::maskz_vplzcntq<Arch>(mask.to_bitset(), value.to_native())); }
  }

  /// For each 32-bit lane, set bit j when earlier source lane j is equal.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd")
  constexpr simd<std::uint32_t, 16, Arch> vpconflictd(simd<std::uint32_t, 16, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<true>(value, simd<std::uint32_t, 16, Arch>{}, ~std::uint64_t{0}); }
    else { return simd<std::uint32_t, 16, Arch>::from_native(detail::x86_avx512cd::vpconflictd<Arch>(value.to_native())); }
  }

  /// Inactive destination lanes retain source; every input lane still participates.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd")
  constexpr simd<std::uint32_t, 16, Arch> mask_vpconflictd(simd<std::uint32_t, 16, Arch> source, predicate<16, Arch> mask, simd<std::uint32_t, 16, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<true>(value, source, mask.to_bitset()); }
    else { return simd<std::uint32_t, 16, Arch>::from_native(detail::x86_avx512cd::mask_vpconflictd<Arch>(source.to_native(), mask.to_bitset(), value.to_native())); }
  }

  /// Inactive destination lanes become zero; every input lane still participates.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd")
  constexpr simd<std::uint32_t, 16, Arch> maskz_vpconflictd(predicate<16, Arch> mask, simd<std::uint32_t, 16, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<true>(value, simd<std::uint32_t, 16, Arch>{}, mask.to_bitset()); }
    else { return simd<std::uint32_t, 16, Arch>::from_native(detail::x86_avx512cd::maskz_vpconflictd<Arch>(mask.to_bitset(), value.to_native())); }
  }

  /// Count leading zeros in each 32-bit lane; zero yields 32.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd")
  constexpr simd<std::uint32_t, 16, Arch> vplzcntd(simd<std::uint32_t, 16, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<false>(value, simd<std::uint32_t, 16, Arch>{}, ~std::uint64_t{0}); }
    else { return simd<std::uint32_t, 16, Arch>::from_native(detail::x86_avx512cd::vplzcntd<Arch>(value.to_native())); }
  }

  /// Inactive destination lanes retain source; zero input yields the lane width.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd")
  constexpr simd<std::uint32_t, 16, Arch> mask_vplzcntd(simd<std::uint32_t, 16, Arch> source, predicate<16, Arch> mask, simd<std::uint32_t, 16, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<false>(value, source, mask.to_bitset()); }
    else { return simd<std::uint32_t, 16, Arch>::from_native(detail::x86_avx512cd::mask_vplzcntd<Arch>(source.to_native(), mask.to_bitset(), value.to_native())); }
  }

  /// Inactive destination lanes become zero; zero input yields the lane width.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd")
  constexpr simd<std::uint32_t, 16, Arch> maskz_vplzcntd(predicate<16, Arch> mask, simd<std::uint32_t, 16, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<false>(value, simd<std::uint32_t, 16, Arch>{}, mask.to_bitset()); }
    else { return simd<std::uint32_t, 16, Arch>::from_native(detail::x86_avx512cd::maskz_vplzcntd<Arch>(mask.to_bitset(), value.to_native())); }
  }

  /// For each 64-bit lane, set bit j when earlier source lane j is equal.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd")
  constexpr simd<std::uint64_t, 8, Arch> vpconflictq(simd<std::uint64_t, 8, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<true>(value, simd<std::uint64_t, 8, Arch>{}, ~std::uint64_t{0}); }
    else { return simd<std::uint64_t, 8, Arch>::from_native(detail::x86_avx512cd::vpconflictq<Arch>(value.to_native())); }
  }

  /// Inactive destination lanes retain source; every input lane still participates.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd")
  constexpr simd<std::uint64_t, 8, Arch> mask_vpconflictq(simd<std::uint64_t, 8, Arch> source, predicate<8, Arch> mask, simd<std::uint64_t, 8, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<true>(value, source, mask.to_bitset()); }
    else { return simd<std::uint64_t, 8, Arch>::from_native(detail::x86_avx512cd::mask_vpconflictq<Arch>(source.to_native(), mask.to_bitset(), value.to_native())); }
  }

  /// Inactive destination lanes become zero; every input lane still participates.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd")
  constexpr simd<std::uint64_t, 8, Arch> maskz_vpconflictq(predicate<8, Arch> mask, simd<std::uint64_t, 8, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<true>(value, simd<std::uint64_t, 8, Arch>{}, mask.to_bitset()); }
    else { return simd<std::uint64_t, 8, Arch>::from_native(detail::x86_avx512cd::maskz_vpconflictq<Arch>(mask.to_bitset(), value.to_native())); }
  }

  /// Count leading zeros in each 64-bit lane; zero yields 64.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd")
  constexpr simd<std::uint64_t, 8, Arch> vplzcntq(simd<std::uint64_t, 8, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<false>(value, simd<std::uint64_t, 8, Arch>{}, ~std::uint64_t{0}); }
    else { return simd<std::uint64_t, 8, Arch>::from_native(detail::x86_avx512cd::vplzcntq<Arch>(value.to_native())); }
  }

  /// Inactive destination lanes retain source; zero input yields the lane width.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd")
  constexpr simd<std::uint64_t, 8, Arch> mask_vplzcntq(simd<std::uint64_t, 8, Arch> source, predicate<8, Arch> mask, simd<std::uint64_t, 8, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<false>(value, source, mask.to_bitset()); }
    else { return simd<std::uint64_t, 8, Arch>::from_native(detail::x86_avx512cd::mask_vplzcntq<Arch>(source.to_native(), mask.to_bitset(), value.to_native())); }
  }

  /// Inactive destination lanes become zero; zero input yields the lane width.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd))
  native_nodiscard native_inline native_const native_target("avx512f,avx512cd")
  constexpr simd<std::uint64_t, 8, Arch> maskz_vplzcntq(predicate<8, Arch> mask, simd<std::uint64_t, 8, Arch> value) noexcept {
    if consteval { return detail::x86_avx512cd_constant::evaluate<false>(value, simd<std::uint64_t, 8, Arch>{}, mask.to_bitset()); }
    else { return simd<std::uint64_t, 8, Arch>::from_native(detail::x86_avx512cd::maskz_vplzcntq<Arch>(mask.to_bitset(), value.to_native())); }
  }

  /// Evaluate vpconflictd at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 4, Arch> vpconflictd(simd<std::uint32_t, 4, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<true>(value, simd<std::uint32_t, 4, Arch>{}, ~std::uint64_t{0});
  }

  /// Evaluate mask_vpconflictd at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 4, Arch> mask_vpconflictd(simd<std::uint32_t, 4, Arch> source, predicate<4, Arch> mask, simd<std::uint32_t, 4, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<true>(value, source, mask.to_bitset());
  }

  /// Evaluate maskz_vpconflictd at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 4, Arch> maskz_vpconflictd(predicate<4, Arch> mask, simd<std::uint32_t, 4, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<true>(value, simd<std::uint32_t, 4, Arch>{}, mask.to_bitset());
  }

  /// Evaluate vplzcntd at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 4, Arch> vplzcntd(simd<std::uint32_t, 4, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<false>(value, simd<std::uint32_t, 4, Arch>{}, ~std::uint64_t{0});
  }

  /// Evaluate mask_vplzcntd at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 4, Arch> mask_vplzcntd(simd<std::uint32_t, 4, Arch> source, predicate<4, Arch> mask, simd<std::uint32_t, 4, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<false>(value, source, mask.to_bitset());
  }

  /// Evaluate maskz_vplzcntd at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 4, Arch> maskz_vplzcntd(predicate<4, Arch> mask, simd<std::uint32_t, 4, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<false>(value, simd<std::uint32_t, 4, Arch>{}, mask.to_bitset());
  }

  /// Evaluate vpconflictq at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint64_t, 2, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 2, Arch> vpconflictq(simd<std::uint64_t, 2, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<true>(value, simd<std::uint64_t, 2, Arch>{}, ~std::uint64_t{0});
  }

  /// Evaluate mask_vpconflictq at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint64_t, 2, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 2, Arch> mask_vpconflictq(simd<std::uint64_t, 2, Arch> source, predicate<2, Arch> mask, simd<std::uint64_t, 2, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<true>(value, source, mask.to_bitset());
  }

  /// Evaluate maskz_vpconflictq at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint64_t, 2, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 2, Arch> maskz_vpconflictq(predicate<2, Arch> mask, simd<std::uint64_t, 2, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<true>(value, simd<std::uint64_t, 2, Arch>{}, mask.to_bitset());
  }

  /// Evaluate vplzcntq at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint64_t, 2, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 2, Arch> vplzcntq(simd<std::uint64_t, 2, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<false>(value, simd<std::uint64_t, 2, Arch>{}, ~std::uint64_t{0});
  }

  /// Evaluate mask_vplzcntq at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint64_t, 2, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 2, Arch> mask_vplzcntq(simd<std::uint64_t, 2, Arch> source, predicate<2, Arch> mask, simd<std::uint64_t, 2, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<false>(value, source, mask.to_bitset());
  }

  /// Evaluate maskz_vplzcntq at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint64_t, 2, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 2, Arch> maskz_vplzcntq(predicate<2, Arch> mask, simd<std::uint64_t, 2, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<false>(value, simd<std::uint64_t, 2, Arch>{}, mask.to_bitset());
  }

  /// Evaluate vpconflictd at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint32_t, 8, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 8, Arch> vpconflictd(simd<std::uint32_t, 8, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<true>(value, simd<std::uint32_t, 8, Arch>{}, ~std::uint64_t{0});
  }

  /// Evaluate mask_vpconflictd at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint32_t, 8, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 8, Arch> mask_vpconflictd(simd<std::uint32_t, 8, Arch> source, predicate<8, Arch> mask, simd<std::uint32_t, 8, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<true>(value, source, mask.to_bitset());
  }

  /// Evaluate maskz_vpconflictd at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint32_t, 8, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 8, Arch> maskz_vpconflictd(predicate<8, Arch> mask, simd<std::uint32_t, 8, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<true>(value, simd<std::uint32_t, 8, Arch>{}, mask.to_bitset());
  }

  /// Evaluate vplzcntd at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint32_t, 8, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 8, Arch> vplzcntd(simd<std::uint32_t, 8, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<false>(value, simd<std::uint32_t, 8, Arch>{}, ~std::uint64_t{0});
  }

  /// Evaluate mask_vplzcntd at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint32_t, 8, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 8, Arch> mask_vplzcntd(simd<std::uint32_t, 8, Arch> source, predicate<8, Arch> mask, simd<std::uint32_t, 8, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<false>(value, source, mask.to_bitset());
  }

  /// Evaluate maskz_vplzcntd at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint32_t, 8, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 8, Arch> maskz_vplzcntd(predicate<8, Arch> mask, simd<std::uint32_t, 8, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<false>(value, simd<std::uint32_t, 8, Arch>{}, mask.to_bitset());
  }

  /// Evaluate vpconflictq at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint64_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 4, Arch> vpconflictq(simd<std::uint64_t, 4, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<true>(value, simd<std::uint64_t, 4, Arch>{}, ~std::uint64_t{0});
  }

  /// Evaluate mask_vpconflictq at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint64_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 4, Arch> mask_vpconflictq(simd<std::uint64_t, 4, Arch> source, predicate<4, Arch> mask, simd<std::uint64_t, 4, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<true>(value, source, mask.to_bitset());
  }

  /// Evaluate maskz_vpconflictq at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint64_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 4, Arch> maskz_vpconflictq(predicate<4, Arch> mask, simd<std::uint64_t, 4, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<true>(value, simd<std::uint64_t, 4, Arch>{}, mask.to_bitset());
  }

  /// Evaluate vplzcntq at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint64_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 4, Arch> vplzcntq(simd<std::uint64_t, 4, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<false>(value, simd<std::uint64_t, 4, Arch>{}, ~std::uint64_t{0});
  }

  /// Evaluate mask_vplzcntq at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint64_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 4, Arch> mask_vplzcntq(simd<std::uint64_t, 4, Arch> source, predicate<4, Arch> mask, simd<std::uint64_t, 4, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<false>(value, source, mask.to_bitset());
  }

  /// Evaluate maskz_vplzcntq at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd) && Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint64_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 4, Arch> maskz_vplzcntq(predicate<4, Arch> mask, simd<std::uint64_t, 4, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<false>(value, simd<std::uint64_t, 4, Arch>{}, mask.to_bitset());
  }

  /// Evaluate vpconflictd at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd)) &&
      requires { sizeof(simd<std::uint32_t, 16, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 16, Arch> vpconflictd(simd<std::uint32_t, 16, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<true>(value, simd<std::uint32_t, 16, Arch>{}, ~std::uint64_t{0});
  }

  /// Evaluate mask_vpconflictd at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd)) &&
      requires { sizeof(simd<std::uint32_t, 16, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 16, Arch> mask_vpconflictd(simd<std::uint32_t, 16, Arch> source, predicate<16, Arch> mask, simd<std::uint32_t, 16, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<true>(value, source, mask.to_bitset());
  }

  /// Evaluate maskz_vpconflictd at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd)) &&
      requires { sizeof(simd<std::uint32_t, 16, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 16, Arch> maskz_vpconflictd(predicate<16, Arch> mask, simd<std::uint32_t, 16, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<true>(value, simd<std::uint32_t, 16, Arch>{}, mask.to_bitset());
  }

  /// Evaluate vplzcntd at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd)) &&
      requires { sizeof(simd<std::uint32_t, 16, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 16, Arch> vplzcntd(simd<std::uint32_t, 16, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<false>(value, simd<std::uint32_t, 16, Arch>{}, ~std::uint64_t{0});
  }

  /// Evaluate mask_vplzcntd at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd)) &&
      requires { sizeof(simd<std::uint32_t, 16, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 16, Arch> mask_vplzcntd(simd<std::uint32_t, 16, Arch> source, predicate<16, Arch> mask, simd<std::uint32_t, 16, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<false>(value, source, mask.to_bitset());
  }

  /// Evaluate maskz_vplzcntd at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd)) &&
      requires { sizeof(simd<std::uint32_t, 16, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 16, Arch> maskz_vplzcntd(predicate<16, Arch> mask, simd<std::uint32_t, 16, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<false>(value, simd<std::uint32_t, 16, Arch>{}, mask.to_bitset());
  }

  /// Evaluate vpconflictq at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd)) &&
      requires { sizeof(simd<std::uint64_t, 8, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 8, Arch> vpconflictq(simd<std::uint64_t, 8, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<true>(value, simd<std::uint64_t, 8, Arch>{}, ~std::uint64_t{0});
  }

  /// Evaluate mask_vpconflictq at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd)) &&
      requires { sizeof(simd<std::uint64_t, 8, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 8, Arch> mask_vpconflictq(simd<std::uint64_t, 8, Arch> source, predicate<8, Arch> mask, simd<std::uint64_t, 8, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<true>(value, source, mask.to_bitset());
  }

  /// Evaluate maskz_vpconflictq at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd)) &&
      requires { sizeof(simd<std::uint64_t, 8, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 8, Arch> maskz_vpconflictq(predicate<8, Arch> mask, simd<std::uint64_t, 8, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<true>(value, simd<std::uint64_t, 8, Arch>{}, mask.to_bitset());
  }

  /// Evaluate vplzcntq at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd)) &&
      requires { sizeof(simd<std::uint64_t, 8, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 8, Arch> vplzcntq(simd<std::uint64_t, 8, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<false>(value, simd<std::uint64_t, 8, Arch>{}, ~std::uint64_t{0});
  }

  /// Evaluate mask_vplzcntq at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd)) &&
      requires { sizeof(simd<std::uint64_t, 8, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 8, Arch> mask_vplzcntq(simd<std::uint64_t, 8, Arch> source, predicate<8, Arch> mask, simd<std::uint64_t, 8, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<false>(value, source, mask.to_bitset());
  }

  /// Evaluate maskz_vplzcntq at compile time when its register storage is available.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512cd)) &&
      requires { sizeof(simd<std::uint64_t, 8, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 8, Arch> maskz_vplzcntq(predicate<8, Arch> mask, simd<std::uint64_t, 8, Arch> value) noexcept {
    return detail::x86_avx512cd_constant::evaluate<false>(value, simd<std::uint64_t, 8, Arch>{}, mask.to_bitset());
  }

  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, class... Args>
  void vpconflictd(Args...) = delete;

  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, class... Args>
  void mask_vpconflictd(Args...) = delete;

  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, class... Args>
  void maskz_vpconflictd(Args...) = delete;

  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, class... Args>
  void vplzcntd(Args...) = delete;

  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, class... Args>
  void mask_vplzcntd(Args...) = delete;

  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, class... Args>
  void maskz_vplzcntd(Args...) = delete;

  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, class... Args>
  void vpconflictq(Args...) = delete;

  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, class... Args>
  void mask_vpconflictq(Args...) = delete;

  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, class... Args>
  void maskz_vpconflictq(Args...) = delete;

  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, class... Args>
  void vplzcntq(Args...) = delete;

  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, class... Args>
  void mask_vplzcntq(Args...) = delete;

  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, class... Args>
  void maskz_vplzcntq(Args...) = delete;

/// \}
}
#endif
