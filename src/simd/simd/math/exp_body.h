
namespace SIMD_BACKEND_NAMESPACE::native {
  // Compatibility entry points share the promoted pack graph.
  template<bool Flush = false, float_register V, std::size_t N>
  simd_nodiscard simd_flatten native_inline simd_pure std::array<V, N> exp(std::array<V, N> const & input) noexcept {
    return ::math::exp<Flush>(input);
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
  template<bool Flush = false, std::size_t L, ::simd::isa Arch> requires SIMD_ARCH_REQUIRES(Arch)
  simd_nodiscard simd_inline simd_pure vec<float,L,Arch> exp(vec<float,L,Arch> input) noexcept {
    return SIMD_BACKEND_NAMESPACE::native::exp<Flush>(input);
  }
  /// \ingroup vector_math
  /// Evaluate exp stage by stage across independent registers; N may be zero.
  template<bool Flush = false, std::size_t L, std::size_t N, ::simd::isa Arch> requires SIMD_ARCH_REQUIRES(Arch)
  simd_nodiscard simd_inline std::array<vec<float,L,Arch>,N> exp(std::array<vec<float,L,Arch>,N> const & input) noexcept {
    return SIMD_BACKEND_NAMESPACE::native::exp<Flush>(input);
  }
  // A tag argument avoids confusing this policy with register-width template
  // arguments on other exp overloads during dependent lookup.
  /// \ingroup vector_math
  /// Select the same exp cutoff through a bool_constant tag for dependent calls.
  template<bool Flush, std::size_t L, std::size_t N, ::simd::isa Arch> requires SIMD_ARCH_REQUIRES(Arch)
  simd_nodiscard simd_inline std::array<vec<float,L,Arch>,N> exp(
      std::array<vec<float,L,Arch>,N> const & input, std::bool_constant<Flush>) noexcept {
    return SIMD_BACKEND_NAMESPACE::native::exp<Flush>(input);
  }
  /// \ingroup vector_math
  /// Select the same exp cutoff through a bool_constant tag for dependent calls.
  template<bool Flush, std::size_t L, ::simd::isa Arch> requires SIMD_ARCH_REQUIRES(Arch)
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
