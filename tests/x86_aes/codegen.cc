// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/x86/aes.h>

namespace {
  constexpr auto requirements = native::target_features<native::x86>("aes");
}

extern "C" native_noinline native_target("aes")
__m128i native_aes_aesenc(__m128i state, __m128i key) noexcept {
  return native::detail::x86_aes::aesenc<requirements>(state, key);
}

extern "C" native_noinline native_target("aes")
__m128i native_aes_aesenclast(__m128i state, __m128i key) noexcept {
  return native::detail::x86_aes::aesenclast<requirements>(state, key);
}

extern "C" native_noinline native_target("aes")
__m128i native_aes_aesdec(__m128i state, __m128i key) noexcept {
  return native::detail::x86_aes::aesdec<requirements>(state, key);
}

extern "C" native_noinline native_target("aes")
__m128i native_aes_aesdeclast(__m128i state, __m128i key) noexcept {
  return native::detail::x86_aes::aesdeclast<requirements>(state, key);
}

extern "C" native_noinline native_target("aes")
__m128i native_aes_aesimc(__m128i state) noexcept {
  return native::detail::x86_aes::aesimc<requirements>(state);
}

extern "C" native_noinline native_target("aes")
__m128i native_aes_aeskeygenassist_0(__m128i state) noexcept {
  return native::detail::x86_aes::aeskeygenassist<requirements, 0>(state);
}

extern "C" native_noinline native_target("aes")
__m128i native_aes_aeskeygenassist_1(__m128i state) noexcept {
  return native::detail::x86_aes::aeskeygenassist<requirements, 1>(state);
}

extern "C" native_noinline native_target("aes")
__m128i native_aes_aeskeygenassist_255(__m128i state) noexcept {
  return native::detail::x86_aes::aeskeygenassist<requirements, 255>(state);
}
