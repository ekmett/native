#pragma once
// Independent staged split-scale exponential reference. Separate names allow
// this graph to coexist with the pack/native-scale implementation.
import simd.wide;
#include "support/profile.h"
#include <limits>

namespace exp_before {
  using namespace SIMD_BACKEND_NAMESPACE::native;
  using simd::wide;
  namespace detail {
    template<float_register V> struct exp_state {
      V x, n, r, y;
      simd_inline explicit exp_state(V input) : x(input) {}
    };
    template<float_register V> struct exp_stages {
      template<class ... S> simd_inline wide<V, sizeof...(S)> operator()(S ... a) const noexcept {
        ((a.r = min(max(a.x, V(-104)), V(88.72283935546875f))), ...);
        ((a.n = round_even(a.r * V(1.4426950408889634f))), ...);
        ((a.r = fma(a.n, V(-0x1.62e400p-1f), a.r)), ...);
        ((a.r = fma(a.n, V(-0x1.7f7d1cp-20f), a.r)), ...);
        // Integer conversion used finite values. Restore NaNs now, then release
        // the original inputs before Horner evaluation. Clamped finite endpoints
        // naturally produce zero/infinity, including either infinite input.
        ((a.r = select(a.x == a.x, a.r, a.x)), ...);
        ((a.y = V(0x1.a1d714d7b1510dp-13f)), ...);
        ((a.y = fma(a.r, a.y, V(0x1.6da756e670ea6p-10f))), ...);
        ((a.y = fma(a.r, a.y, V(0x1.11105b3161a6fp-7f))), ...);
        ((a.y = fma(a.r, a.y, V(0x1.5554649b7487fp-5f))), ...);
        ((a.y = fma(a.r, a.y, V(0x1.555555c673724p-3f))), ...);
        ((a.y = fma(a.r, a.y, V(0x1.0000005c8dd89p-1f))), ...);
        ((a.y = fma(a.r, a.y, V(0x1.ffffffffb0eefp-1f))), ...);
        ((a.y = fma(a.r, a.y, V(0x1.ffffffffa70c9p-1f))), ...);
        // Split scaling handles n=128 and subnormal results with normal factors.
        ((a.r = min(max(a.n, V(-126)), V(127))), ...);
        ((a.y = (a.y * normal_pow2(a.n - a.r)) * normal_pow2(a.r)), ...);
        return {{a.y...}};
      }
    };
  }
  namespace detail {
    template<float_register V> struct exp_registers {
      template<class ... X> simd_inline auto operator()(X ... x) const noexcept {
        return exp_stages<V>{}(exp_state<V>(x)...);
      }
    };
  }
  // Sollya degree 7, tools/sollya_exp.sollya; coefficients round to FP32.
  // Gradual underflow follows the caller's FP environment; no FTZ/DAZ changes.
  // Each dependency stage expands across independent register chains.
  template<float_register V, std::size_t N>
  simd_nodiscard simd_inline simd_pure wide<V, N> exp(wide<V, N> const & input) noexcept {
    return input.apply(detail::exp_registers<V>{});
  }
  template<float_register V> simd_nodiscard simd_inline simd_pure V exp(V x) noexcept { return exp_before::exp(wide<V, 1>{{x}}).registers[0]; }
#if defined(__ARM_NEON)
  // Compatibility for existing channel-packed callers.
  simd_nodiscard simd_inline simd_pure float32x4_t exp(float32x4_t x) noexcept { return exp_before::exp(fp32x4(x)).value; }
  template<std::size_t N> simd_nodiscard simd_inline simd_pure wide<float32x4_t, N> exp(wide<float32x4_t, N> x) noexcept {
    wide<fp32x4, N> input;
    for (std::size_t i = 0; i < N; ++i) input.registers[i] = fp32x4(x.registers[i]);
    auto output = exp(input);
    for (std::size_t i = 0; i < N; ++i) x.registers[i] = output.registers[i].value;
    return x;
  }
#endif
}

/**
 * \file
 * \license
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
 * SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
 * \endlicense
 * \author Edward Kmett <ekmett@gmail.com>
 * \brief Evaluates the independent split-scale exponential reference.
 */
