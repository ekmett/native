// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Repeated in each raw backend's target scope. These ordinary-inline leaves
// let a generic native_inline algorithm reach its caller before leaf inlining.
namespace wide::detail {
  template<class V> requires requires { V::architecture; } && SIMD_ARCH_REQUIRES(V::architecture)
  struct native_ops<V> {
    static inline V constant(float x) noexcept { return V(x); }
    static inline auto mul(V a, V b) noexcept { return a * b; }
    static inline auto less(V a, V b) noexcept { return a < b; }
    static inline auto logical_not(V a) noexcept { return !a; }
    static inline auto minimum(V a, V b) noexcept { return select(a < b, a, b); }
    static inline auto round(V a) noexcept { return round_even(a); }
    static inline auto fused(V a, V b, V c) noexcept { return fma(a, b, c); }
    template<class M>
    static inline auto scale(M m, V a, V n) noexcept { return masked_scaleb_zero(m, a, n); }
  };
}
