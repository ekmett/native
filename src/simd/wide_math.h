// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "simd/vec.h"
#include "simd/wide_pack.h"

namespace wide::detail {
  template<class V> struct native_ops;
}
#define SIMD_BACKEND_BODY "simd/wide_native_body.h"
#include "simd/simd/for_each_backend.h"
#undef SIMD_BACKEND_BODY

namespace wide::detail {
  template<class P> inline constexpr bool native_pack = false;
  template<class V, std::size_t N>
  inline constexpr bool native_pack<array<V, N>> = requires { sizeof(native_ops<V>); };
  template<class... V>
  inline constexpr bool native_pack<tuple<V...>> = (requires { sizeof(native_ops<V>); } && ...);
  template<class P> concept native_pack_argument = native_pack<std::remove_cvref_t<P>>;
}

namespace wide {
  /// Promote the same coefficient to each chain's computation type.
  template<detail::native_pack_argument P>
  simd_nodiscard native_inline auto constant_like(P const & shape, float value) noexcept {
    return map([value]<class V>(V const &) { return detail::native_ops<V>::constant(value); }, shape);
  }
  template<detail::native_pack_argument P, detail::native_pack_argument Q>
  simd_nodiscard native_inline auto operator*(P const & a, Q const & b) noexcept {
    return map([]<class V>(V const & x, V const & y) { return detail::native_ops<V>::mul(x, y); }, a, b);
  }
  template<detail::native_pack_argument P, detail::native_pack_argument Q>
  simd_nodiscard native_inline auto operator<(P const & a, Q const & b) noexcept {
    return map([]<class V>(V const & x, V const & y) { return detail::native_ops<V>::less(x, y); }, a, b);
  }
  template<detail::native_pack_argument P>
  simd_nodiscard native_inline auto operator!(P const & a) noexcept {
    return map([]<class V>(V const & x) { return detail::native_ops<V>::logical_not(x); }, a);
  }
  template<detail::native_pack_argument P, detail::native_pack_argument Q>
  simd_nodiscard native_inline auto min(P const & a, Q const & b) noexcept {
    return map([]<class V>(V const & x, V const & y) { return detail::native_ops<V>::minimum(x, y); }, a, b);
  }
  template<detail::native_pack_argument P>
  simd_nodiscard native_inline auto round_even(P const & a) noexcept {
    return map([]<class V>(V const & x) { return detail::native_ops<V>::round(x); }, a);
  }
  template<detail::native_pack_argument P, detail::native_pack_argument Q, detail::native_pack_argument R>
  simd_nodiscard native_inline auto fma(P const & a, Q const & b, R const & c) noexcept {
    return map([]<class V>(V const & x, V const & y, V const & z) { return detail::native_ops<V>::fused(x, y, z); }, a, b, c);
  }
  template<detail::native_pack_argument P, detail::native_pack_argument Q, detail::native_pack_argument R>
  simd_nodiscard native_inline auto masked_scaleb_zero(P const & m, Q const & a, R const & n) noexcept {
    return map([]<class M, class V>(M const & mask, V const & x, V const & e) {
      return detail::native_ops<V>::scale(mask, x, e);
    }, m, a, n);
  }

  namespace detail {
    template<class T> inline constexpr bool binary32_register = false;
    template<std::size_t N, ::simd::isa A>
    inline constexpr bool binary32_register<::simd::vec<float, N, A>> = true;
    template<class P> inline constexpr bool binary32_pack = false;
    template<class V, std::size_t N>
    inline constexpr bool binary32_pack<array<V, N>> = binary32_register<V>;
    template<class... V>
    inline constexpr bool binary32_pack<tuple<V...>> = (binary32_register<V> && ...);
  }
}

namespace math {
  namespace detail {
    // The single polynomial body, shared by generic and targeted entry points.
    template<bool Flush, ::wide::pack P>
    simd_nodiscard native_inline auto exp_reduced(P const & x) noexcept {
      auto const c = [&](float value) { return ::wide::constant_like(x, value); };
      auto const active = !(x < c(Flush ? -87.33654022216796875f : -104.f));
      // Keep x second: the ordered minimum preserves NaNs.
      auto r = min(c(88.72283935546875f), x);
      auto const n = round_even(r * c(1.4426950408889634f));
      r = fma(n, c(-0x1.62e400p-1f), r);
      r = fma(n, c(-0x1.7f7d1cp-20f), r);

      auto y = fma(r, c(0x1.a1d714d7b1510dp-13f), c(0x1.6da756e670ea6p-10f));
      y = fma(r, y, c(0x1.11105b3161a6fp-7f));
      y = fma(r, y, c(0x1.5554649b7487fp-5f));
      y = fma(r, y, c(0x1.555555c673724p-3f));
      y = fma(r, y, c(0x1.0000005c8dd89p-1f));
      auto const one = c(1.f);
      y = fma(r, y, one);
      y = fma(r, y, one);
      return std::tuple{active, y, n};
    }
  }

  /// Evaluate exp through the canonical SIMD pack and restore the input shape.
  template<bool Flush = false, ::wide::promotable T>
    requires (::wide::detail::binary32_pack<::wide::canonical_t<T>>)
  simd_nodiscard native_inline auto exp(T const & input) noexcept {
    // MSVC's array<T,0> may construct a dummy T; an empty batch needs no work.
    if constexpr (::wide::detail::shape_t<::wide::canonical_t<T>>::size == 0) {
      return std::remove_cvref_t<T>(input);
    } else {
      auto const x = ::wide::promote(input);
      auto const [active, y, n] = detail::exp_reduced<Flush>(x);
      return ::wide::demote<T>(masked_scaleb_zero(active, y, n));
    }
  }
}
