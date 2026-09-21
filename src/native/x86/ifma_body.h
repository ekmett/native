// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#if NATIVE_HOST_X86 || defined(NATIVE_DOXYGEN)
export namespace native {
  /// \defgroup x86_ifma Integer fused multiply-add of 52-bit values
  /// Multiply the low 52 bits of each input lane and add either product half
  /// to the full 64-bit accumulator, modulo 2^64. Short unmasked operations use
  /// AVX-IFMA when tagged; otherwise they require AVX512IFMA and AVX512VL.
  /// Masked forms require EVEX; inactive lanes retain the accumulator or zero.
  /// Missing-feature overloads are consteval-only and require complete storage.
  /// \{

  /// Accumulate low 52-bit products in 2 lanes; unmasked.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avxifma) && Arch.has(x86_feature::avx))
  native_nodiscard native_inline native_const native_target("avxifma")
  constexpr simd<std::uint64_t, 2, Arch> madd52lo(
    simd<std::uint64_t, 2, Arch> accumulator, simd<std::uint64_t, 2, Arch> a, simd<std::uint64_t, 2, Arch> b) noexcept {
    if consteval {
      return detail::x86_ifma_constant::accumulate<false>(accumulator, a, b, ~std::uint64_t{0}, false);
    } else {
      return simd<std::uint64_t, 2, Arch>::from_native(
        detail::x86_ifma::madd52lo<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
    }
  }

  /// Accumulate low 52-bit products in 2 lanes; unmasked.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avxifma) && Arch.has(x86_feature::avx)) &&
      Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma,avx512vl")
  constexpr simd<std::uint64_t, 2, Arch> madd52lo(
    simd<std::uint64_t, 2, Arch> accumulator, simd<std::uint64_t, 2, Arch> a, simd<std::uint64_t, 2, Arch> b) noexcept {
    if consteval {
      return detail::x86_ifma_constant::accumulate<false>(accumulator, a, b, ~std::uint64_t{0}, false);
    } else {
      return simd<std::uint64_t, 2, Arch>::from_native(
        detail::x86_ifma::madd52lo<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
    }
  }

  /// Constant evaluation without the instruction features.
  template<isa<x86> Arch> requires(!((Arch.has(x86_feature::avxifma) && Arch.has(x86_feature::avx)) || (Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))) &&
      requires { sizeof(simd<std::uint64_t, 2, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 2, Arch> madd52lo(
    simd<std::uint64_t, 2, Arch> accumulator, simd<std::uint64_t, 2, Arch> a, simd<std::uint64_t, 2, Arch> b) noexcept {
    return detail::x86_ifma_constant::accumulate<false>(accumulator, a, b, ~std::uint64_t{0}, false);
  }

  /// Accumulate low 52-bit products in 4 lanes; unmasked.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avxifma) && Arch.has(x86_feature::avx))
  native_nodiscard native_inline native_const native_target("avxifma")
  constexpr simd<std::uint64_t, 4, Arch> madd52lo(
    simd<std::uint64_t, 4, Arch> accumulator, simd<std::uint64_t, 4, Arch> a, simd<std::uint64_t, 4, Arch> b) noexcept {
    if consteval {
      return detail::x86_ifma_constant::accumulate<false>(accumulator, a, b, ~std::uint64_t{0}, false);
    } else {
      return simd<std::uint64_t, 4, Arch>::from_native(
        detail::x86_ifma::madd52lo<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
    }
  }

  /// Accumulate low 52-bit products in 4 lanes; unmasked.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avxifma) && Arch.has(x86_feature::avx)) &&
      Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma,avx512vl")
  constexpr simd<std::uint64_t, 4, Arch> madd52lo(
    simd<std::uint64_t, 4, Arch> accumulator, simd<std::uint64_t, 4, Arch> a, simd<std::uint64_t, 4, Arch> b) noexcept {
    if consteval {
      return detail::x86_ifma_constant::accumulate<false>(accumulator, a, b, ~std::uint64_t{0}, false);
    } else {
      return simd<std::uint64_t, 4, Arch>::from_native(
        detail::x86_ifma::madd52lo<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
    }
  }

  /// Constant evaluation without the instruction features.
  template<isa<x86> Arch> requires(!((Arch.has(x86_feature::avxifma) && Arch.has(x86_feature::avx)) || (Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))) &&
      requires { sizeof(simd<std::uint64_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 4, Arch> madd52lo(
    simd<std::uint64_t, 4, Arch> accumulator, simd<std::uint64_t, 4, Arch> a, simd<std::uint64_t, 4, Arch> b) noexcept {
    return detail::x86_ifma_constant::accumulate<false>(accumulator, a, b, ~std::uint64_t{0}, false);
  }

  /// Accumulate low 52-bit products in 8 lanes; unmasked.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma")
  constexpr simd<std::uint64_t, 8, Arch> madd52lo(
    simd<std::uint64_t, 8, Arch> accumulator, simd<std::uint64_t, 8, Arch> a, simd<std::uint64_t, 8, Arch> b) noexcept {
    if consteval {
      return detail::x86_ifma_constant::accumulate<false>(accumulator, a, b, ~std::uint64_t{0}, false);
    } else {
      return simd<std::uint64_t, 8, Arch>::from_native(
        detail::x86_ifma::madd52lo<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
    }
  }

  /// Constant evaluation without the instruction features.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma)) &&
      requires { sizeof(simd<std::uint64_t, 8, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 8, Arch> madd52lo(
    simd<std::uint64_t, 8, Arch> accumulator, simd<std::uint64_t, 8, Arch> a, simd<std::uint64_t, 8, Arch> b) noexcept {
    return detail::x86_ifma_constant::accumulate<false>(accumulator, a, b, ~std::uint64_t{0}, false);
  }

  /// Reject raw registers, mixed tags and unsupported vector or mask shapes.
  template<isa<x86> Arch, class... Args>
  void madd52lo(Args...) = delete;

  /// Accumulate low 52-bit products in 2 lanes; inactive lanes retain the accumulator.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma,avx512vl")
  constexpr simd<std::uint64_t, 2, Arch> mask_madd52lo(
    simd<std::uint64_t, 2, Arch> accumulator, predicate<2, Arch> mask, simd<std::uint64_t, 2, Arch> a, simd<std::uint64_t, 2, Arch> b) noexcept {
    if consteval {
      return detail::x86_ifma_constant::accumulate<false>(accumulator, a, b, mask.to_bitset(), false);
    } else {
      return simd<std::uint64_t, 2, Arch>::from_native(
        detail::x86_ifma::mask_madd52lo<Arch>(accumulator.to_native(), static_cast<__mmask8>(mask.to_bitset()), a.to_native(), b.to_native()));
    }
  }

  /// Constant evaluation without the instruction features.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint64_t, 2, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 2, Arch> mask_madd52lo(
    simd<std::uint64_t, 2, Arch> accumulator, predicate<2, Arch> mask, simd<std::uint64_t, 2, Arch> a, simd<std::uint64_t, 2, Arch> b) noexcept {
    return detail::x86_ifma_constant::accumulate<false>(accumulator, a, b, mask.to_bitset(), false);
  }

  /// Accumulate low 52-bit products in 4 lanes; inactive lanes retain the accumulator.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma,avx512vl")
  constexpr simd<std::uint64_t, 4, Arch> mask_madd52lo(
    simd<std::uint64_t, 4, Arch> accumulator, predicate<4, Arch> mask, simd<std::uint64_t, 4, Arch> a, simd<std::uint64_t, 4, Arch> b) noexcept {
    if consteval {
      return detail::x86_ifma_constant::accumulate<false>(accumulator, a, b, mask.to_bitset(), false);
    } else {
      return simd<std::uint64_t, 4, Arch>::from_native(
        detail::x86_ifma::mask_madd52lo<Arch>(accumulator.to_native(), static_cast<__mmask8>(mask.to_bitset()), a.to_native(), b.to_native()));
    }
  }

  /// Constant evaluation without the instruction features.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint64_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 4, Arch> mask_madd52lo(
    simd<std::uint64_t, 4, Arch> accumulator, predicate<4, Arch> mask, simd<std::uint64_t, 4, Arch> a, simd<std::uint64_t, 4, Arch> b) noexcept {
    return detail::x86_ifma_constant::accumulate<false>(accumulator, a, b, mask.to_bitset(), false);
  }

  /// Accumulate low 52-bit products in 8 lanes; inactive lanes retain the accumulator.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma")
  constexpr simd<std::uint64_t, 8, Arch> mask_madd52lo(
    simd<std::uint64_t, 8, Arch> accumulator, predicate<8, Arch> mask, simd<std::uint64_t, 8, Arch> a, simd<std::uint64_t, 8, Arch> b) noexcept {
    if consteval {
      return detail::x86_ifma_constant::accumulate<false>(accumulator, a, b, mask.to_bitset(), false);
    } else {
      return simd<std::uint64_t, 8, Arch>::from_native(
        detail::x86_ifma::mask_madd52lo<Arch>(accumulator.to_native(), static_cast<__mmask8>(mask.to_bitset()), a.to_native(), b.to_native()));
    }
  }

  /// Constant evaluation without the instruction features.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma)) &&
      requires { sizeof(simd<std::uint64_t, 8, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 8, Arch> mask_madd52lo(
    simd<std::uint64_t, 8, Arch> accumulator, predicate<8, Arch> mask, simd<std::uint64_t, 8, Arch> a, simd<std::uint64_t, 8, Arch> b) noexcept {
    return detail::x86_ifma_constant::accumulate<false>(accumulator, a, b, mask.to_bitset(), false);
  }

  /// Reject raw registers, mixed tags and unsupported vector or mask shapes.
  template<isa<x86> Arch, class... Args>
  void mask_madd52lo(Args...) = delete;

  /// Accumulate low 52-bit products in 2 lanes; inactive lanes become zero.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma,avx512vl")
  constexpr simd<std::uint64_t, 2, Arch> maskz_madd52lo(
    predicate<2, Arch> mask, simd<std::uint64_t, 2, Arch> accumulator, simd<std::uint64_t, 2, Arch> a, simd<std::uint64_t, 2, Arch> b) noexcept {
    if consteval {
      return detail::x86_ifma_constant::accumulate<false>(accumulator, a, b, mask.to_bitset(), true);
    } else {
      return simd<std::uint64_t, 2, Arch>::from_native(
        detail::x86_ifma::maskz_madd52lo<Arch>(static_cast<__mmask8>(mask.to_bitset()), accumulator.to_native(), a.to_native(), b.to_native()));
    }
  }

  /// Constant evaluation without the instruction features.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint64_t, 2, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 2, Arch> maskz_madd52lo(
    predicate<2, Arch> mask, simd<std::uint64_t, 2, Arch> accumulator, simd<std::uint64_t, 2, Arch> a, simd<std::uint64_t, 2, Arch> b) noexcept {
    return detail::x86_ifma_constant::accumulate<false>(accumulator, a, b, mask.to_bitset(), true);
  }

  /// Accumulate low 52-bit products in 4 lanes; inactive lanes become zero.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma,avx512vl")
  constexpr simd<std::uint64_t, 4, Arch> maskz_madd52lo(
    predicate<4, Arch> mask, simd<std::uint64_t, 4, Arch> accumulator, simd<std::uint64_t, 4, Arch> a, simd<std::uint64_t, 4, Arch> b) noexcept {
    if consteval {
      return detail::x86_ifma_constant::accumulate<false>(accumulator, a, b, mask.to_bitset(), true);
    } else {
      return simd<std::uint64_t, 4, Arch>::from_native(
        detail::x86_ifma::maskz_madd52lo<Arch>(static_cast<__mmask8>(mask.to_bitset()), accumulator.to_native(), a.to_native(), b.to_native()));
    }
  }

  /// Constant evaluation without the instruction features.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint64_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 4, Arch> maskz_madd52lo(
    predicate<4, Arch> mask, simd<std::uint64_t, 4, Arch> accumulator, simd<std::uint64_t, 4, Arch> a, simd<std::uint64_t, 4, Arch> b) noexcept {
    return detail::x86_ifma_constant::accumulate<false>(accumulator, a, b, mask.to_bitset(), true);
  }

  /// Accumulate low 52-bit products in 8 lanes; inactive lanes become zero.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma")
  constexpr simd<std::uint64_t, 8, Arch> maskz_madd52lo(
    predicate<8, Arch> mask, simd<std::uint64_t, 8, Arch> accumulator, simd<std::uint64_t, 8, Arch> a, simd<std::uint64_t, 8, Arch> b) noexcept {
    if consteval {
      return detail::x86_ifma_constant::accumulate<false>(accumulator, a, b, mask.to_bitset(), true);
    } else {
      return simd<std::uint64_t, 8, Arch>::from_native(
        detail::x86_ifma::maskz_madd52lo<Arch>(static_cast<__mmask8>(mask.to_bitset()), accumulator.to_native(), a.to_native(), b.to_native()));
    }
  }

  /// Constant evaluation without the instruction features.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma)) &&
      requires { sizeof(simd<std::uint64_t, 8, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 8, Arch> maskz_madd52lo(
    predicate<8, Arch> mask, simd<std::uint64_t, 8, Arch> accumulator, simd<std::uint64_t, 8, Arch> a, simd<std::uint64_t, 8, Arch> b) noexcept {
    return detail::x86_ifma_constant::accumulate<false>(accumulator, a, b, mask.to_bitset(), true);
  }

  /// Reject raw registers, mixed tags and unsupported vector or mask shapes.
  template<isa<x86> Arch, class... Args>
  void maskz_madd52lo(Args...) = delete;

  /// Accumulate high 52-bit products in 2 lanes; unmasked.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avxifma) && Arch.has(x86_feature::avx))
  native_nodiscard native_inline native_const native_target("avxifma")
  constexpr simd<std::uint64_t, 2, Arch> madd52hi(
    simd<std::uint64_t, 2, Arch> accumulator, simd<std::uint64_t, 2, Arch> a, simd<std::uint64_t, 2, Arch> b) noexcept {
    if consteval {
      return detail::x86_ifma_constant::accumulate<true>(accumulator, a, b, ~std::uint64_t{0}, false);
    } else {
      return simd<std::uint64_t, 2, Arch>::from_native(
        detail::x86_ifma::madd52hi<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
    }
  }

  /// Accumulate high 52-bit products in 2 lanes; unmasked.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avxifma) && Arch.has(x86_feature::avx)) &&
      Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma,avx512vl")
  constexpr simd<std::uint64_t, 2, Arch> madd52hi(
    simd<std::uint64_t, 2, Arch> accumulator, simd<std::uint64_t, 2, Arch> a, simd<std::uint64_t, 2, Arch> b) noexcept {
    if consteval {
      return detail::x86_ifma_constant::accumulate<true>(accumulator, a, b, ~std::uint64_t{0}, false);
    } else {
      return simd<std::uint64_t, 2, Arch>::from_native(
        detail::x86_ifma::madd52hi<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
    }
  }

  /// Constant evaluation without the instruction features.
  template<isa<x86> Arch> requires(!((Arch.has(x86_feature::avxifma) && Arch.has(x86_feature::avx)) || (Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))) &&
      requires { sizeof(simd<std::uint64_t, 2, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 2, Arch> madd52hi(
    simd<std::uint64_t, 2, Arch> accumulator, simd<std::uint64_t, 2, Arch> a, simd<std::uint64_t, 2, Arch> b) noexcept {
    return detail::x86_ifma_constant::accumulate<true>(accumulator, a, b, ~std::uint64_t{0}, false);
  }

  /// Accumulate high 52-bit products in 4 lanes; unmasked.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avxifma) && Arch.has(x86_feature::avx))
  native_nodiscard native_inline native_const native_target("avxifma")
  constexpr simd<std::uint64_t, 4, Arch> madd52hi(
    simd<std::uint64_t, 4, Arch> accumulator, simd<std::uint64_t, 4, Arch> a, simd<std::uint64_t, 4, Arch> b) noexcept {
    if consteval {
      return detail::x86_ifma_constant::accumulate<true>(accumulator, a, b, ~std::uint64_t{0}, false);
    } else {
      return simd<std::uint64_t, 4, Arch>::from_native(
        detail::x86_ifma::madd52hi<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
    }
  }

  /// Accumulate high 52-bit products in 4 lanes; unmasked.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avxifma) && Arch.has(x86_feature::avx)) &&
      Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma,avx512vl")
  constexpr simd<std::uint64_t, 4, Arch> madd52hi(
    simd<std::uint64_t, 4, Arch> accumulator, simd<std::uint64_t, 4, Arch> a, simd<std::uint64_t, 4, Arch> b) noexcept {
    if consteval {
      return detail::x86_ifma_constant::accumulate<true>(accumulator, a, b, ~std::uint64_t{0}, false);
    } else {
      return simd<std::uint64_t, 4, Arch>::from_native(
        detail::x86_ifma::madd52hi<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
    }
  }

  /// Constant evaluation without the instruction features.
  template<isa<x86> Arch> requires(!((Arch.has(x86_feature::avxifma) && Arch.has(x86_feature::avx)) || (Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))) &&
      requires { sizeof(simd<std::uint64_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 4, Arch> madd52hi(
    simd<std::uint64_t, 4, Arch> accumulator, simd<std::uint64_t, 4, Arch> a, simd<std::uint64_t, 4, Arch> b) noexcept {
    return detail::x86_ifma_constant::accumulate<true>(accumulator, a, b, ~std::uint64_t{0}, false);
  }

  /// Accumulate high 52-bit products in 8 lanes; unmasked.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma")
  constexpr simd<std::uint64_t, 8, Arch> madd52hi(
    simd<std::uint64_t, 8, Arch> accumulator, simd<std::uint64_t, 8, Arch> a, simd<std::uint64_t, 8, Arch> b) noexcept {
    if consteval {
      return detail::x86_ifma_constant::accumulate<true>(accumulator, a, b, ~std::uint64_t{0}, false);
    } else {
      return simd<std::uint64_t, 8, Arch>::from_native(
        detail::x86_ifma::madd52hi<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
    }
  }

  /// Constant evaluation without the instruction features.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma)) &&
      requires { sizeof(simd<std::uint64_t, 8, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 8, Arch> madd52hi(
    simd<std::uint64_t, 8, Arch> accumulator, simd<std::uint64_t, 8, Arch> a, simd<std::uint64_t, 8, Arch> b) noexcept {
    return detail::x86_ifma_constant::accumulate<true>(accumulator, a, b, ~std::uint64_t{0}, false);
  }

  /// Reject raw registers, mixed tags and unsupported vector or mask shapes.
  template<isa<x86> Arch, class... Args>
  void madd52hi(Args...) = delete;

  /// Accumulate high 52-bit products in 2 lanes; inactive lanes retain the accumulator.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma,avx512vl")
  constexpr simd<std::uint64_t, 2, Arch> mask_madd52hi(
    simd<std::uint64_t, 2, Arch> accumulator, predicate<2, Arch> mask, simd<std::uint64_t, 2, Arch> a, simd<std::uint64_t, 2, Arch> b) noexcept {
    if consteval {
      return detail::x86_ifma_constant::accumulate<true>(accumulator, a, b, mask.to_bitset(), false);
    } else {
      return simd<std::uint64_t, 2, Arch>::from_native(
        detail::x86_ifma::mask_madd52hi<Arch>(accumulator.to_native(), static_cast<__mmask8>(mask.to_bitset()), a.to_native(), b.to_native()));
    }
  }

  /// Constant evaluation without the instruction features.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint64_t, 2, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 2, Arch> mask_madd52hi(
    simd<std::uint64_t, 2, Arch> accumulator, predicate<2, Arch> mask, simd<std::uint64_t, 2, Arch> a, simd<std::uint64_t, 2, Arch> b) noexcept {
    return detail::x86_ifma_constant::accumulate<true>(accumulator, a, b, mask.to_bitset(), false);
  }

  /// Accumulate high 52-bit products in 4 lanes; inactive lanes retain the accumulator.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma,avx512vl")
  constexpr simd<std::uint64_t, 4, Arch> mask_madd52hi(
    simd<std::uint64_t, 4, Arch> accumulator, predicate<4, Arch> mask, simd<std::uint64_t, 4, Arch> a, simd<std::uint64_t, 4, Arch> b) noexcept {
    if consteval {
      return detail::x86_ifma_constant::accumulate<true>(accumulator, a, b, mask.to_bitset(), false);
    } else {
      return simd<std::uint64_t, 4, Arch>::from_native(
        detail::x86_ifma::mask_madd52hi<Arch>(accumulator.to_native(), static_cast<__mmask8>(mask.to_bitset()), a.to_native(), b.to_native()));
    }
  }

  /// Constant evaluation without the instruction features.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint64_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 4, Arch> mask_madd52hi(
    simd<std::uint64_t, 4, Arch> accumulator, predicate<4, Arch> mask, simd<std::uint64_t, 4, Arch> a, simd<std::uint64_t, 4, Arch> b) noexcept {
    return detail::x86_ifma_constant::accumulate<true>(accumulator, a, b, mask.to_bitset(), false);
  }

  /// Accumulate high 52-bit products in 8 lanes; inactive lanes retain the accumulator.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma")
  constexpr simd<std::uint64_t, 8, Arch> mask_madd52hi(
    simd<std::uint64_t, 8, Arch> accumulator, predicate<8, Arch> mask, simd<std::uint64_t, 8, Arch> a, simd<std::uint64_t, 8, Arch> b) noexcept {
    if consteval {
      return detail::x86_ifma_constant::accumulate<true>(accumulator, a, b, mask.to_bitset(), false);
    } else {
      return simd<std::uint64_t, 8, Arch>::from_native(
        detail::x86_ifma::mask_madd52hi<Arch>(accumulator.to_native(), static_cast<__mmask8>(mask.to_bitset()), a.to_native(), b.to_native()));
    }
  }

  /// Constant evaluation without the instruction features.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma)) &&
      requires { sizeof(simd<std::uint64_t, 8, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 8, Arch> mask_madd52hi(
    simd<std::uint64_t, 8, Arch> accumulator, predicate<8, Arch> mask, simd<std::uint64_t, 8, Arch> a, simd<std::uint64_t, 8, Arch> b) noexcept {
    return detail::x86_ifma_constant::accumulate<true>(accumulator, a, b, mask.to_bitset(), false);
  }

  /// Reject raw registers, mixed tags and unsupported vector or mask shapes.
  template<isa<x86> Arch, class... Args>
  void mask_madd52hi(Args...) = delete;

  /// Accumulate high 52-bit products in 2 lanes; inactive lanes become zero.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma,avx512vl")
  constexpr simd<std::uint64_t, 2, Arch> maskz_madd52hi(
    predicate<2, Arch> mask, simd<std::uint64_t, 2, Arch> accumulator, simd<std::uint64_t, 2, Arch> a, simd<std::uint64_t, 2, Arch> b) noexcept {
    if consteval {
      return detail::x86_ifma_constant::accumulate<true>(accumulator, a, b, mask.to_bitset(), true);
    } else {
      return simd<std::uint64_t, 2, Arch>::from_native(
        detail::x86_ifma::maskz_madd52hi<Arch>(static_cast<__mmask8>(mask.to_bitset()), accumulator.to_native(), a.to_native(), b.to_native()));
    }
  }

  /// Constant evaluation without the instruction features.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint64_t, 2, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 2, Arch> maskz_madd52hi(
    predicate<2, Arch> mask, simd<std::uint64_t, 2, Arch> accumulator, simd<std::uint64_t, 2, Arch> a, simd<std::uint64_t, 2, Arch> b) noexcept {
    return detail::x86_ifma_constant::accumulate<true>(accumulator, a, b, mask.to_bitset(), true);
  }

  /// Accumulate high 52-bit products in 4 lanes; inactive lanes become zero.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma,avx512vl")
  constexpr simd<std::uint64_t, 4, Arch> maskz_madd52hi(
    predicate<4, Arch> mask, simd<std::uint64_t, 4, Arch> accumulator, simd<std::uint64_t, 4, Arch> a, simd<std::uint64_t, 4, Arch> b) noexcept {
    if consteval {
      return detail::x86_ifma_constant::accumulate<true>(accumulator, a, b, mask.to_bitset(), true);
    } else {
      return simd<std::uint64_t, 4, Arch>::from_native(
        detail::x86_ifma::maskz_madd52hi<Arch>(static_cast<__mmask8>(mask.to_bitset()), accumulator.to_native(), a.to_native(), b.to_native()));
    }
  }

  /// Constant evaluation without the instruction features.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma) &&
      Arch.has(x86_feature::avx512vl)) &&
      requires { sizeof(simd<std::uint64_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 4, Arch> maskz_madd52hi(
    predicate<4, Arch> mask, simd<std::uint64_t, 4, Arch> accumulator, simd<std::uint64_t, 4, Arch> a, simd<std::uint64_t, 4, Arch> b) noexcept {
    return detail::x86_ifma_constant::accumulate<true>(accumulator, a, b, mask.to_bitset(), true);
  }

  /// Accumulate high 52-bit products in 8 lanes; inactive lanes become zero.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma))
  native_nodiscard native_inline native_const native_target("avx512f,avx512ifma")
  constexpr simd<std::uint64_t, 8, Arch> maskz_madd52hi(
    predicate<8, Arch> mask, simd<std::uint64_t, 8, Arch> accumulator, simd<std::uint64_t, 8, Arch> a, simd<std::uint64_t, 8, Arch> b) noexcept {
    if consteval {
      return detail::x86_ifma_constant::accumulate<true>(accumulator, a, b, mask.to_bitset(), true);
    } else {
      return simd<std::uint64_t, 8, Arch>::from_native(
        detail::x86_ifma::maskz_madd52hi<Arch>(static_cast<__mmask8>(mask.to_bitset()), accumulator.to_native(), a.to_native(), b.to_native()));
    }
  }

  /// Constant evaluation without the instruction features.
  template<isa<x86> Arch> requires(!(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512ifma)) &&
      requires { sizeof(simd<std::uint64_t, 8, Arch>); })
  native_nodiscard consteval simd<std::uint64_t, 8, Arch> maskz_madd52hi(
    predicate<8, Arch> mask, simd<std::uint64_t, 8, Arch> accumulator, simd<std::uint64_t, 8, Arch> a, simd<std::uint64_t, 8, Arch> b) noexcept {
    return detail::x86_ifma_constant::accumulate<true>(accumulator, a, b, mask.to_bitset(), true);
  }

  /// Reject raw registers, mixed tags and unsupported vector or mask shapes.
  template<isa<x86> Arch, class... Args>
  void maskz_madd52hi(Args...) = delete;

  /// \}
}
#endif
