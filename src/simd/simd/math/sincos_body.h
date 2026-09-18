
// Altered source: paired polynomial and reducer with explicit binary32
// operation order. Original notices are retained below.
// Precondition: finite input with |x| < 8192. No global math replacement.
namespace SIMD_BACKEND_NAMESPACE::native {
  namespace detail {
    template <class V> struct trig_conversion;
    template <> struct trig_conversion<fp32x1> {
      static simd_inline uint32x1 integer(fp32x1 x) noexcept {
        return uint32x1(static_cast<std::uint32_t>(x.value));
      }
      static simd_inline fp32x1 floating(uint32x1 x) noexcept {
        return fp32x1(static_cast<float>(x.value));
      }
    };
#if SIMD_HAS_AVX2
    template <> struct trig_conversion<fp32x4> {
      static simd_inline uint32x4 integer(fp32x4 x) noexcept {
        return uint32x4::from_native(_mm_cvttps_epi32(x.value));
      }
      static simd_inline fp32x4 floating(uint32x4 x) noexcept {
        return fp32x4(_mm_cvtepi32_ps(x.value));
      }
    };
    template <> struct trig_conversion<fp32x8> {
      static simd_inline uint32x8 integer(fp32x8 x) noexcept {
        return uint32x8::from_native(_mm256_cvttps_epi32(x.value));
      }
      static simd_inline fp32x8 floating(uint32x8 x) noexcept {
        return fp32x8(_mm256_cvtepi32_ps(x.value));
      }
    };
#endif
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ
    template <> struct trig_conversion<fp32x16> {
      static simd_inline uint32x16 integer(fp32x16 x) noexcept {
        return uint32x16::from_native(_mm512_cvttps_epi32(x.value));
      }
      static simd_inline fp32x16 floating(uint32x16 x) noexcept {
        return fp32x16(_mm512_cvtepi32_ps(x.value));
      }
    };
#endif
#if SIMD_HAS_ARM_NEON
    template <> struct trig_conversion<fp32x4> {
      static simd_inline uint32x4 integer(fp32x4 x) noexcept {
        return uint32x4::from_native(vreinterpretq_u8_s32(vcvtq_s32_f32(x.value)));
      }
      static simd_inline fp32x4 floating(uint32x4 x) noexcept {
        return fp32x4(vcvtq_f32_s32(vreinterpretq_s32_u8(x.value)));
      }
    };
#endif
    enum class trig_kind { sine, cosine, paired };
    template <trig_kind K, float_register V, std::size_t N>
    simd_flatten simd_inline auto trig(std::array<V, N> const & input) noexcept {
      using B = fp32_bit_bridge<V>;
      using I = typename B::bits_type;
      using C = trig_conversion<V>;
      if constexpr (N == 0) {
        if constexpr (K == trig_kind::paired) return std::pair<std::array<V, N>, std::array<V, N>>{};
        else return std::array<V, N>{};
      } else {
        auto const & [...original] = input;
        auto [...sign_sine] = std::array{(B::encode(original) & I(0x80000000u))...};
        auto [...x] = std::array{B::decode(B::encode(original) & I(0x7fffffffu))...};
        auto [...y] = std::array{(x * V(1.27323954473516f))...};
        auto const [...j] = std::array{((C::integer(y) + I(1)) & I(0xfffffffeu))...};
        ((y = C::floating(j)), ...);
        ((sign_sine = sign_sine ^ ((j & I(4)).template left<29>())), ...);
        auto const [...sign_cosine] = std::array{((((j - I(2)) ^ I(0xffffffffu)) & I(4)).template left<29>())...};
        auto [...mask] = std::array<I, N>{};
        if constexpr (K == trig_kind::cosine) {
          ((mask = mask_bits<::simd::uint32_t>(((j - I(2)) & I(2)) == I(0))), ...);
        } else {
          ((mask = mask_bits<::simd::uint32_t>((j & I(2)) == I(0))), ...);
        }
        ((x = fma(y, V(-0.78515625f), x)), ...);
        ((x = fma(y, V(-2.4187564849853515625e-4f), x)), ...);
        ((x = fma(y, V(-3.77489497744594108e-8f), x)), ...);
        auto const [...z] = std::array{(x * x)...};
        auto [...cosine] = std::array{fma(V(2.443315711809948e-5f), z, V(-1.388731625493765e-3f))...};
        ((cosine = fma(cosine, z, V(4.166664568298827e-2f))), ...);
        ((cosine = cosine * z), ...);
        ((cosine = cosine * z), ...);
        ((cosine = cosine - z * V(0.5f)), ...);
        ((cosine = cosine + V(1.0f)), ...);
        auto [...sine] = std::array{fma(V(-1.9515295891e-4f), z, V(8.3321608736e-3f))...};
        ((sine = fma(sine, z, V(-1.6666654611e-1f))), ...);
        ((sine = sine * z), ...);
        ((sine = fma(sine, x, x)), ...);
        auto [...selected_sine] = std::array{B::decode(mask & B::encode(sine))...};
        auto [...selected_cosine] = std::array{B::decode((mask ^ I(0xffffffffu)) & B::encode(cosine))...};
        if constexpr (K == trig_kind::paired) {
          // Preserve subtraction selection, including its signed-zero effects.
          ((sine = sine - selected_sine), ...);
          ((cosine = cosine - selected_cosine), ...);
          ((selected_sine = B::decode(B::encode(selected_cosine + selected_sine) ^ sign_sine)), ...);
          ((selected_cosine = B::decode(B::encode(cosine + sine) ^ sign_cosine)), ...);
          return std::pair{std::array{selected_sine...}, std::array{selected_cosine...}};
        } else if constexpr (K == trig_kind::sine) {
          return std::array{B::decode(B::encode(selected_cosine + selected_sine) ^ sign_sine)...};
        } else {
          return std::array{B::decode(B::encode(selected_cosine + selected_sine) ^ sign_cosine)...};
        }
      }
    }
  } // namespace detail
  template <float_register V, std::size_t N>
  simd_inline std::array<V, N> sin(std::array<V, N> const & x) noexcept {
    return detail::trig<detail::trig_kind::sine>(x);
  }
  template <float_register V, std::size_t N>
  simd_inline std::array<V, N> cos(std::array<V, N> const & x) noexcept {
    return detail::trig<detail::trig_kind::cosine>(x);
  }
  template <float_register V, std::size_t N>
  simd_inline std::pair<std::array<V, N>, std::array<V, N>> sincos(std::array<V, N> const & x) noexcept {
    return detail::trig<detail::trig_kind::paired>(x);
  }
} // namespace SIMD_BACKEND_NAMESPACE::native

/*
   AVX implementation of sin, cos, sincos, exp and log

   Based on "sse_mathfun.h", by Julien Pommier
   http://gruntthepeon.free.fr/ssemath/

   Copyright (C) 2012 Giovanni Garberoglio
   Interdisciplinary Laboratory for Computational Science (LISC)
   Fondazione Bruno Kessler and University of Trento
   via Sommarive, 18
   I-38123 Trento (Italy)

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/* RTS repository license (retained verbatim):
Software License Agreement (BSD 2-Clause License)
========================================

Copyright 2017 Edward Kmett

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL YAHOO! INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
 * \file
 * \license
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2012 Giovanni Garberoglio
 * SPDX-FileCopyrightText: 2017 Edward Kmett
 * SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
 * SPDX-License-Identifier: Zlib AND BSD-2-Clause
 * \endlicense
 * \author Edward Kmett <ekmett@gmail.com>
 * \brief Sine and cosine polynomials, range reduction, and raw register-pack evaluation.
 */
