
namespace NATIVE_BACKEND_NAMESPACE::native {
  // Compatibility entry points share the promoted pack graph.
  template<bool Flush = false, unsigned Degree = 6, float_register V, std::size_t N>
    requires (Degree >= 1 && Degree <= 7)
  native_nodiscard native_flatten native_inline constexpr native_pure std::array<V, N> exp(std::array<V, N> const & input) noexcept {
    if constexpr (N == 0) return input;
    else {
      auto const [masks, replacements, values, exponents] =
        ::math::detail::exp_reduced<Flush, Degree>(::wide::promote(input));
      auto const & [...in_range] = masks;
      auto const & [...replacement] = replacements;
      auto const & [...y] = values;
      auto const & [...n] = exponents;
      // Finish in this entry's target scope: the generic scaling bridge can
      // exceed Clang's inline-cost budget for AVX-512 without VL.
      return {{::wide::detail::native_ops<V>::exp_scale(in_range, replacement, y, n)...}};
    }
  }
  template<bool Flush = false, unsigned Degree = 6, float_register V>
    requires (Degree >= 1 && Degree <= 7)
  native_nodiscard native_inline constexpr native_pure V exp(V x) noexcept {
    return ::NATIVE_BACKEND_NAMESPACE::native::exp<Flush, Degree>(std::array{x})[0];
  }
#if NATIVE_HAS_ARM_NEON
  native_nodiscard native_inline constexpr native_pure float32x4_t exp(float32x4_t x) noexcept {
    return ::NATIVE_BACKEND_NAMESPACE::native::exp(fp32x4(x)).value;
  }
  template<std::size_t N>
  native_nodiscard native_inline constexpr native_pure std::array<float32x4_t, N> exp(std::array<float32x4_t, N> const & input) noexcept {
    if constexpr (N == 0) return {};
    else {
      auto const & [...value] = input;
      auto const [...result] = ::NATIVE_BACKEND_NAMESPACE::native::exp(std::array{fp32x4(value)...});
      return {{result.value...}};
    }
  }
#endif
}

namespace native {
  // SIMD and array overloads let native::wide reuse the staged array kernel.
#define NATIVE_PROMOTED_MATH_ENTRY(name) \
  template<std::size_t L, ::native::isa<> Arch> \
    requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<L> \
  native_nodiscard native_inline constexpr simd<float,L,Arch> name(simd<float,L,Arch> input) noexcept { \
    return ::math::name(input); \
  } \
  template<std::size_t L, std::size_t N, ::native::isa<> Arch> \
    requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<L> \
  native_nodiscard native_inline constexpr std::array<simd<float,L,Arch>,N> \
  name(std::array<simd<float,L,Arch>,N> const & input) noexcept { return ::math::name(input); }
  NATIVE_PROMOTED_MATH_ENTRY(expm1)
  NATIVE_PROMOTED_MATH_ENTRY(damping_gain)
  NATIVE_PROMOTED_MATH_ENTRY(log)
  NATIVE_PROMOTED_MATH_ENTRY(log2)
  NATIVE_PROMOTED_MATH_ENTRY(log1p)
  NATIVE_PROMOTED_MATH_ENTRY(tanh)
#undef NATIVE_PROMOTED_MATH_ENTRY
  /// \ingroup vector_math
  /// Base-two exponential with the same compile-time underflow policy as exp.
  template<bool Flush = false, std::size_t L, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<L>
  native_nodiscard native_inline constexpr simd<float,L,Arch>
  exp2(simd<float,L,Arch> input) noexcept { return ::math::exp2<Flush>(input); }
  /// \ingroup vector_math
  /// Evaluate exp2 stage by stage across independent registers.
  template<bool Flush = false, std::size_t L, std::size_t N, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<L>
  native_nodiscard native_inline constexpr std::array<simd<float,L,Arch>,N>
  exp2(std::array<simd<float,L,Arch>,N> const & input) noexcept { return ::math::exp2<Flush>(input); }
  /// \ingroup vector_math
  /// Select exp2's underflow policy through a tag for dependent calls.
  template<bool Flush, std::size_t L, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<L>
  native_nodiscard native_inline constexpr simd<float,L,Arch>
  exp2(simd<float,L,Arch> input, std::bool_constant<Flush>) noexcept { return ::math::exp2<Flush>(input); }
  /// \ingroup vector_math
  /// Select exp2's underflow policy for a register batch through a tag.
  template<bool Flush, std::size_t L, std::size_t N, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<L>
  native_nodiscard native_inline constexpr std::array<simd<float,L,Arch>,N>
  exp2(std::array<simd<float,L,Arch>,N> const & input, std::bool_constant<Flush>) noexcept {
    return ::math::exp2<Flush>(input);
  }
  /// \ingroup vector_math
  /// Evaluate atan2(y,x), retaining the common SIMD architecture and lane count.
  template<std::size_t L, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<L>
  native_nodiscard native_inline constexpr simd<float,L,Arch>
  atan2(simd<float,L,Arch> y, simd<float,L,Arch> x) noexcept { return ::math::atan2(y,x); }
  /// \ingroup vector_math
  /// Advance each atan2 stage across matching arrays of independent registers.
  template<std::size_t L, std::size_t N, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<L>
  native_nodiscard native_inline constexpr std::array<simd<float,L,Arch>,N>
  atan2(std::array<simd<float,L,Arch>,N> const & y,
      std::array<simd<float,L,Arch>,N> const & x) noexcept { return ::math::atan2(y,x); }
  /** \ingroup vector_math
   * \brief Evaluate the binary32 range-reduced exponential approximation.
   * `Degree` selects a polynomial from one through seven, defaulting to six.
   * Every degree shares range reduction and exponent scaling; none promises
   * correctly rounded exp for every input. `Flush` selects
   * the early underflow cutoff at compile time; it does not change CPU controls
   * or turn a raw vector into a policy-bearing FTZ type.
   * \snippet api.cc exponential
   */
  template<bool Flush = false, unsigned Degree = 6, std::size_t L, ::native::isa<> Arch> requires (Degree >= 1 && Degree <= 7) && NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<L>
  native_nodiscard native_inline constexpr native_pure simd<float,L,Arch> exp(simd<float,L,Arch> input) noexcept {
    return ::NATIVE_BACKEND_NAMESPACE::native::exp<Flush, Degree>(input);
  }
  /// \ingroup vector_math
  /// Evaluate exp stage by stage across independent registers; N may be zero.
  template<bool Flush = false, unsigned Degree = 6, std::size_t L, std::size_t N, ::native::isa<> Arch> requires (Degree >= 1 && Degree <= 7) && NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<L>
  native_nodiscard native_inline constexpr std::array<simd<float,L,Arch>,N> exp(std::array<simd<float,L,Arch>,N> const & input) noexcept {
    return ::NATIVE_BACKEND_NAMESPACE::native::exp<Flush, Degree>(input);
  }
  // A tag argument avoids confusing this policy with register-width template
  // arguments on other exp overloads during dependent lookup.
  /// \ingroup vector_math
  /// Pass cutoff and degree through constant tags for dependent calls.
  template<bool Flush, unsigned Degree = 6, std::size_t L, std::size_t N, ::native::isa<> Arch> requires (Degree >= 1 && Degree <= 7) && NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<L>
  native_nodiscard native_inline constexpr std::array<simd<float,L,Arch>,N> exp(
      std::array<simd<float,L,Arch>,N> const & input, std::bool_constant<Flush>, std::integral_constant<unsigned, Degree> = {}) noexcept {
    return ::NATIVE_BACKEND_NAMESPACE::native::exp<Flush, Degree>(input);
  }
  /// \ingroup vector_math
  /// Pass cutoff and degree through constant tags for dependent calls.
  template<bool Flush, unsigned Degree = 6, std::size_t L, ::native::isa<> Arch> requires (Degree >= 1 && Degree <= 7) && NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<L>
  native_nodiscard native_inline constexpr simd<float,L,Arch> exp(simd<float,L,Arch> input, std::bool_constant<Flush>, std::integral_constant<unsigned, Degree> = {}) noexcept {
    return ::NATIVE_BACKEND_NAMESPACE::native::exp<Flush, Degree>(input);
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
