
#if !defined(__cpp_structured_bindings) || __cpp_structured_bindings < 202411L
#error "Array exp requires C++26 structured-binding packs. Use a compiler with C++26 structured-binding packs."
#endif

namespace SIMD_BACKEND_NAMESPACE::native {
  // Sollya degree 7. Every line advances all independent register chains.
  // The FTZ cutoff skips the entire subnormal tail; the polynomial needs no FTZ.
  template<bool Flush = false, float_register V, std::size_t N>
  simd_nodiscard simd_flatten simd_inline simd_pure std::array<V, N> exp(std::array<V, N> const & input) noexcept {
    if constexpr (N == 0) return {};
    else {
      auto const & [...x] = input;
      alignas(typename V::mask_type) auto const [...active] = std::array{
        (~(x < V(Flush ? -87.33654022216796875f : -104.f)))...};
      // Put x second: the ordered min keeps NaNs, while clamping positive overflow.
      auto [...r] = std::array{min(V(88.72283935546875f), x)...};
      auto const [...n] = std::array{round_even(r * V(1.4426950408889634f))...};
      ((r = fma(n, V(-0x1.62e400p-1f), r)), ...);
      ((r = fma(n, V(-0x1.7f7d1cp-20f), r)), ...);

      auto [...y] = std::array{
        fma(r, V(0x1.a1d714d7b1510dp-13f), V(0x1.6da756e670ea6p-10f))...};
      ((y = fma(r, y, V(0x1.11105b3161a6fp-7f))), ...);
      ((y = fma(r, y, V(0x1.5554649b7487fp-5f))), ...);
      ((y = fma(r, y, V(0x1.555555c673724p-3f))), ...);
      ((y = fma(r, y, V(0x1.0000005c8dd89p-1f))), ...);
      V const one{1.0f};
      ((y = fma(r, y, one)), ...);
      ((y = fma(r, y, one)), ...);
      return {{masked_scaleb_zero(active, y, n)...}};
    }
  }
  template<bool Flush = false, float_register V>
  simd_nodiscard simd_inline simd_pure V exp(V x) noexcept {
    return SIMD_BACKEND_NAMESPACE::native::exp<Flush>(std::array{x})[0];
  }
#if SIMD_HAS_ARM_NEON
  simd_nodiscard simd_inline simd_pure float32x4_t exp(float32x4_t x) noexcept {
    return SIMD_BACKEND_NAMESPACE::native::exp(fp32x4(x)).value;
  }
  template<std::size_t N>
  simd_nodiscard simd_inline simd_pure std::array<float32x4_t, N> exp(std::array<float32x4_t, N> const & input) noexcept {
    if constexpr (N == 0) return {};
    else {
      auto const & [...value] = input;
      auto const [...result] = SIMD_BACKEND_NAMESPACE::native::exp(std::array{fp32x4(value)...});
      return {{result.value...}};
    }
  }
#endif
}

namespace simd {
  /** \ingroup vector_math
   * \brief Evaluate the binary32 range-reduced exponential approximation.
   * This uses the library's degree-seven polynomial and exponent scaling graph;
   * it is not a claim of correctly rounded exp for every input. `Flush` selects
   * the early underflow cutoff at compile time; it does not change CPU controls
   * or turn a raw vector into a policy-bearing FTZ type.
   * \snippet api.cc exponential
   */
  template<bool Flush = false, std::size_t L, SIMD_ARCH_CONCEPT Arch>
  simd_nodiscard simd_inline simd_pure vec<float,L,Arch> exp(vec<float,L,Arch> input) noexcept {
    return SIMD_BACKEND_NAMESPACE::native::exp<Flush>(input);
  }
  /// \ingroup vector_math
  /// Evaluate exp stage by stage across independent registers; N may be zero.
  template<bool Flush = false, std::size_t L, std::size_t N, SIMD_ARCH_CONCEPT Arch>
  simd_nodiscard simd_inline std::array<vec<float,L,Arch>,N> exp(std::array<vec<float,L,Arch>,N> const & input) noexcept {
    return SIMD_BACKEND_NAMESPACE::native::exp<Flush>(input);
  }
  // A tag argument avoids confusing this policy with register-width template
  // arguments on other exp overloads during dependent lookup.
  /// \ingroup vector_math
  /// Select the same exp cutoff through a bool_constant tag for dependent calls.
  template<bool Flush, std::size_t L, std::size_t N, SIMD_ARCH_CONCEPT Arch>
  simd_nodiscard simd_inline std::array<vec<float,L,Arch>,N> exp(
      std::array<vec<float,L,Arch>,N> const & input, std::bool_constant<Flush>) noexcept {
    return SIMD_BACKEND_NAMESPACE::native::exp<Flush>(input);
  }
  /// \ingroup vector_math
  /// Select the same exp cutoff through a bool_constant tag for dependent calls.
  template<bool Flush, std::size_t L, SIMD_ARCH_CONCEPT Arch>
  simd_nodiscard simd_inline vec<float,L,Arch> exp(vec<float,L,Arch> input, std::bool_constant<Flush>) noexcept {
    return SIMD_BACKEND_NAMESPACE::native::exp<Flush>(input);
  }
}

/**
 * \file
 * \license
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
 * SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
 * \endlicense
 * \author Edward Kmett <ekmett@gmail.com>
 * \brief Range-reduced exponential over scalar, SIMD, and register-pack inputs.
 */
