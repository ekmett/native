// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"

import native.x86.aes;

namespace {
  constexpr auto requirements = native::target_features<native::x86>("aes");
  using bytes = native::simd<std::uint8_t, 16, requirements>;
}

extern "C" native_noinline native_target("aes")
__m128i native_aes_aesenc(__m128i state, __m128i key) noexcept {
  return native::aesenc<requirements>(bytes::from_native(state), bytes::from_native(key)).to_native();
}

extern "C" native_noinline native_target("aes")
__m128i native_aes_aesenclast(__m128i state, __m128i key) noexcept {
  return native::aesenclast<requirements>(bytes::from_native(state), bytes::from_native(key)).to_native();
}

extern "C" native_noinline native_target("aes")
__m128i native_aes_aesdec(__m128i state, __m128i key) noexcept {
  return native::aesdec<requirements>(bytes::from_native(state), bytes::from_native(key)).to_native();
}

extern "C" native_noinline native_target("aes")
__m128i native_aes_aesdeclast(__m128i state, __m128i key) noexcept {
  return native::aesdeclast<requirements>(bytes::from_native(state), bytes::from_native(key)).to_native();
}

extern "C" native_noinline native_target("aes")
__m128i native_aes_aesimc(__m128i state) noexcept {
  return native::aesimc<requirements>(bytes::from_native(state)).to_native();
}

extern "C" native_noinline native_target("aes")
__m128i native_aes_aeskeygenassist_0(__m128i state) noexcept {
  return native::aeskeygenassist<requirements, 0>(bytes::from_native(state)).to_native();
}

extern "C" native_noinline native_target("aes")
__m128i native_aes_aeskeygenassist_1(__m128i state) noexcept {
  return native::aeskeygenassist<requirements, 1>(bytes::from_native(state)).to_native();
}

extern "C" native_noinline native_target("aes")
__m128i native_aes_aeskeygenassist_255(__m128i state) noexcept {
  return native::aeskeygenassist<requirements, 255>(bytes::from_native(state)).to_native();
}
