// SPDX-FileCopyrightText: 2012 Giovanni Garberoglio
// SPDX-FileCopyrightText: 2017 Edward Kmett
// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: Zlib AND BSD-2-Clause
#pragma once
// Included by wide_math.h after the lifted native operations.
// Altered source: the original trig reducer and polynomials now operate on
// standard arrays, preserving each binary32 dependency stage.

namespace math {
  namespace detail {
    enum class trig_kind { sine, cosine, paired };

    // Precondition: every lane is finite and |x| < 8192.
    template<trig_kind K, ::wide::pack P>
    native_nodiscard native_inline constexpr auto trig(P const & original) noexcept {
      auto const encoded = ::wide::bits(original);
      auto const c = [&](float value) { return ::wide::constant_like(original, value); };
      auto const i = [&](std::uint32_t value) { return ::wide::constant_like(encoded, value); };
      auto sign_sine = ::wide::bit_and(encoded, i(0x80000000u));
      auto x = ::wide::from_bits(::wide::bit_and(encoded, i(0x7fffffffu)));
      auto y = ::wide::mul(x, c(1.27323954473516f));
      auto const j = ::wide::bit_and(::wide::add(::wide::detail::trig_integer(y), i(1)), i(0xfffffffeu));
      y = ::wide::detail::trig_float(j);
      sign_sine = ::wide::bit_xor(sign_sine, ::wide::left<29>(::wide::bit_and(j, i(4))));
      auto const sign_cosine = ::wide::left<29>(::wide::bit_and(::wide::bit_xor(::wide::sub(j, i(2)), i(0xffffffffu)), i(4)));
      auto quadrant = j;
      if constexpr (K == trig_kind::cosine) quadrant = ::wide::sub(quadrant, i(2));
      auto const mask = ::wide::mask_bits<std::uint32_t>(::wide::cmp_eq(::wide::bit_and(quadrant, i(2)), i(0)));
      x = ::wide::fma(y, c(-0.78515625f), x);
      x = ::wide::fma(y, c(-2.4187564849853515625e-4f), x);
      x = ::wide::fma(y, c(-3.77489497744594108e-8f), x);
      auto const z = ::wide::mul(x, x);
      auto cosine = ::wide::fma(c(2.443315711809948e-5f), z, c(-1.388731625493765e-3f));
      cosine = ::wide::fma(cosine, z, c(4.166664568298827e-2f));
      cosine = ::wide::mul(cosine, z);
      cosine = ::wide::mul(cosine, z);
      cosine = ::wide::sub(cosine, ::wide::mul(z, c(0.5f)));
      cosine = ::wide::add(cosine, c(1.f));
      auto sine = ::wide::fma(c(-1.9515295891e-4f), z, c(8.3321608736e-3f));
      sine = ::wide::fma(sine, z, c(-1.6666654611e-1f));
      sine = ::wide::mul(sine, z);
      sine = ::wide::fma(sine, x, x);
      auto selected_sine = ::wide::from_bits(::wide::bit_and(mask, ::wide::bits(sine)));
      auto selected_cosine = ::wide::from_bits(::wide::bit_and(::wide::bit_xor(mask, i(0xffffffffu)), ::wide::bits(cosine)));
      if constexpr (K == trig_kind::paired) {
        // Preserve subtraction selection, including its signed-zero effects.
        sine = ::wide::sub(sine, selected_sine);
        cosine = ::wide::sub(cosine, selected_cosine);
        selected_sine = ::wide::from_bits(::wide::bit_xor(::wide::bits(::wide::add(selected_cosine, selected_sine)), sign_sine));
        selected_cosine = ::wide::from_bits(::wide::bit_xor(::wide::bits(::wide::add(cosine, sine)), sign_cosine));
        return std::pair{selected_sine, selected_cosine};
      } else if constexpr (K == trig_kind::sine)
        return ::wide::from_bits(::wide::bit_xor(::wide::bits(::wide::add(selected_cosine, selected_sine)), sign_sine));
      else return ::wide::from_bits(::wide::bit_xor(::wide::bits(::wide::add(selected_cosine, selected_sine)), sign_cosine));
    }

    template<trig_kind K, ::wide::promotable T>
      requires (::wide::detail::binary32_pack<::wide::canonical_t<T>>)
    native_nodiscard native_inline constexpr auto trig_result(T const & input) noexcept {
      if constexpr (::wide::detail::shape_t<::wide::canonical_t<T>>::size == 0) {
        if constexpr (K == trig_kind::paired)
          return std::pair{std::remove_cvref_t<T>(input), std::remove_cvref_t<T>(input)};
        else return std::remove_cvref_t<T>(input);
      } else if constexpr (K == trig_kind::paired) {
        auto const [sine, cosine] = trig<K>(::wide::promote(input));
        return std::pair{::wide::demote<T>(sine), ::wide::demote<T>(cosine)};
      } else return ::wide::demote<T>(trig<K>(::wide::promote(input)));
    }
  }

  /// Sine in radians; finite binary32 lanes with |x| < 8192.
  template<::wide::promotable T> requires (::wide::detail::binary32_pack<::wide::canonical_t<T>>)
  native_nodiscard native_inline constexpr auto sin(T const & input) noexcept {
    return detail::trig_result<detail::trig_kind::sine>(input);
  }
  /// Cosine in radians; finite binary32 lanes with |x| < 8192.
  template<::wide::promotable T> requires (::wide::detail::binary32_pack<::wide::canonical_t<T>>)
  native_nodiscard native_inline constexpr auto cos(T const & input) noexcept {
    return detail::trig_result<detail::trig_kind::cosine>(input);
  }
  /// Paired sine/cosine sharing one reducer; each result retains the input shape.
  template<::wide::promotable T> requires (::wide::detail::binary32_pack<::wide::canonical_t<T>>)
  native_nodiscard native_inline constexpr auto sincos(T const & input) noexcept {
    return detail::trig_result<detail::trig_kind::paired>(input);
  }

  /// Replace subnormal binary32 lanes with signed zero without changing FP controls.
  template<::wide::promotable T> requires (::wide::detail::binary32_pack<::wide::canonical_t<T>>)
  native_nodiscard native_inline constexpr auto flush_to_zero(T const & input) noexcept {
    if constexpr (::wide::detail::shape_t<::wide::canonical_t<T>>::size == 0)
      return std::remove_cvref_t<T>(input);
    else {
      auto bits = ::wide::bits(::wide::promote(input));
      auto const i = [&](std::uint32_t value) { return ::wide::constant_like(bits, value); };
      auto const exponent = ::wide::bit_and(bits, i(0x7f800000u));
      auto const zero = ::wide::mask_bits<std::uint32_t>(::wide::cmp_eq(exponent, i(0)));
      auto const clear = ::wide::bit_and(zero, i(0x007fffffu));
      bits = ::wide::bit_and(bits, ::wide::bit_xor(clear, i(0xffffffffu)));
      return ::wide::demote<T>(::wide::from_bits(bits));
    }
  }

  // These primitives retain their native leaf semantics inside one wide stage.
#define NATIVE_PROMOTED_UNARY(name) \
  template<::wide::promotable T> requires (::wide::detail::binary32_pack<::wide::canonical_t<T>>) \
  native_nodiscard native_inline constexpr auto name(T const & input) noexcept { \
    if constexpr (::wide::detail::shape_t<::wide::canonical_t<T>>::size == 0) \
      return std::remove_cvref_t<T>(input); \
    else return ::wide::demote<T>(::wide::name(::wide::promote(input))); \
  }
  NATIVE_PROMOTED_UNARY(abs)
  NATIVE_PROMOTED_UNARY(sqrt)
  NATIVE_PROMOTED_UNARY(floor)
  NATIVE_PROMOTED_UNARY(ceil)
  NATIVE_PROMOTED_UNARY(trunc)
  NATIVE_PROMOTED_UNARY(round_even)
#undef NATIVE_PROMOTED_UNARY
}

namespace wide {
  // Qualified convenience aliases; standard arrays keep their ordinary ADL.
  using ::math::exp;
  using ::math::sin;
  using ::math::cos;
  using ::math::sincos;
  using ::math::flush_to_zero;
}

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
