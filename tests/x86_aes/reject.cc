// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#if NATIVE_AES_CASE == 2
#include <native/x86/aes.h>
#else
#include "prelude.h"

import native.x86.aes;
#endif

namespace {
  constexpr auto strong = native::target_features<native::x86>("aes");
  constexpr auto weak = native::target_features<native::x86>("sse2");
#if (NATIVE_AES_CASE == 2 || NATIVE_AES_CASE == 3) && !NATIVE_AES_POSITIVE
  constexpr auto requirements = weak;
#else
  constexpr auto requirements = strong;
#endif

#if NATIVE_AES_CASE == 2
  using namespace native::detail::x86_aes;
  using input = __m128i;
#else
  using namespace native;
#if NATIVE_AES_CASE == 4 && !NATIVE_AES_POSITIVE
  using input = __m128i;
#elif NATIVE_AES_CASE == 5 && !NATIVE_AES_POSITIVE
  using input = simd<float, 4, requirements>;
#elif NATIVE_AES_CASE == 6 && !NATIVE_AES_POSITIVE
  using input = simd<std::uint32_t, 4, requirements>;
#else
  using input = simd<std::uint8_t, 16, requirements>;
#endif
#endif

#if NATIVE_AES_CASE == 7 && !NATIVE_AES_POSITIVE
  using key_input = native::simd<std::uint8_t, 16, weak>;
#else
  using key_input = input;
#endif
}

#if NATIVE_AES_CASE != 1 || NATIVE_AES_POSITIVE
native_target("aes")
#endif
auto aes_boundary(input state, key_input key, unsigned immediate) noexcept {
#if NATIVE_AES_OPERATION == 0
  return aesenc<requirements>(state, key);
#elif NATIVE_AES_OPERATION == 1
  return aesenclast<requirements>(state, key);
#elif NATIVE_AES_OPERATION == 2
  return aesdec<requirements>(state, key);
#elif NATIVE_AES_OPERATION == 3
  return aesdeclast<requirements>(state, key);
#elif NATIVE_AES_OPERATION == 4
  return aesimc<requirements>(state);
#elif NATIVE_AES_OPERATION == 5
#if NATIVE_AES_CASE == 8 && !NATIVE_AES_POSITIVE
  return aeskeygenassist<requirements, 256>(state);
#elif NATIVE_AES_CASE == 9 && !NATIVE_AES_POSITIVE
  return aeskeygenassist<requirements, -1>(state);
#elif NATIVE_AES_CASE == 10 && !NATIVE_AES_POSITIVE
  return aeskeygenassist<requirements, 0x100000000ULL>(state);
#elif NATIVE_AES_CASE == 11 && !NATIVE_AES_POSITIVE
  return aeskeygenassist<requirements, immediate>(state);
#else
  return aeskeygenassist<requirements, 1>(state);
#endif
#else
#error Select one AES operation
#endif
}
