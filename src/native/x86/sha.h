// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#if NATIVE_HOST_X86
#include <immintrin.h>

namespace native::detail::x86_sha {
  template<isa<x86> Arch, unsigned Selector> requires(Arch.has(x86_feature::sha) && Selector < 4)
  native_nodiscard native_inline native_const native_target("sha")
  __m128i sha1rnds4(__m128i a, __m128i b) noexcept {
    return _mm_sha1rnds4_epu32(a, b, Selector);
  }

  template<isa<x86> Arch, unsigned Selector, class... Args>
  void sha1rnds4(Args...) = delete;

  template<isa<x86> Arch> requires(Arch.has(x86_feature::sha))
  native_nodiscard native_inline native_const native_target("sha")
  __m128i sha1nexte(__m128i a, __m128i b) noexcept {
    return _mm_sha1nexte_epu32(a, b);
  }

  template<isa<x86> Arch, class... Args>
  void sha1nexte(Args...) = delete;

  template<isa<x86> Arch> requires(Arch.has(x86_feature::sha))
  native_nodiscard native_inline native_const native_target("sha")
  __m128i sha1msg1(__m128i a, __m128i b) noexcept {
    return _mm_sha1msg1_epu32(a, b);
  }

  template<isa<x86> Arch, class... Args>
  void sha1msg1(Args...) = delete;

  template<isa<x86> Arch> requires(Arch.has(x86_feature::sha))
  native_nodiscard native_inline native_const native_target("sha")
  __m128i sha1msg2(__m128i a, __m128i b) noexcept {
    return _mm_sha1msg2_epu32(a, b);
  }

  template<isa<x86> Arch, class... Args>
  void sha1msg2(Args...) = delete;

  template<isa<x86> Arch> requires(Arch.has(x86_feature::sha))
  native_nodiscard native_inline native_const native_target("sha")
  __m128i sha256rnds2(__m128i a, __m128i b, __m128i c) noexcept {
    return _mm_sha256rnds2_epu32(a, b, c);
  }

  template<isa<x86> Arch, class... Args>
  void sha256rnds2(Args...) = delete;

  template<isa<x86> Arch> requires(Arch.has(x86_feature::sha))
  native_nodiscard native_inline native_const native_target("sha")
  __m128i sha256msg1(__m128i a, __m128i b) noexcept {
    return _mm_sha256msg1_epu32(a, b);
  }

  template<isa<x86> Arch, class... Args>
  void sha256msg1(Args...) = delete;

  template<isa<x86> Arch> requires(Arch.has(x86_feature::sha))
  native_nodiscard native_inline native_const native_target("sha")
  __m128i sha256msg2(__m128i a, __m128i b) noexcept {
    return _mm_sha256msg2_epu32(a, b);
  }

  template<isa<x86> Arch, class... Args>
  void sha256msg2(Args...) = delete;

}
#endif

#include <array>
#include <bit>
#include <cstdint>

namespace native::detail::x86_sha_constant {
  template<class V>
  constexpr auto lanes(V value) noexcept {
    std::array<std::uint32_t, 4> result{};
    value.store(result.data());
    return result;
  }

  constexpr std::uint32_t choose(std::uint32_t x, std::uint32_t y, std::uint32_t z) noexcept {
    return (x & y) ^ (~x & z);
  }

  constexpr std::uint32_t majority(std::uint32_t x, std::uint32_t y, std::uint32_t z) noexcept {
    return (x & y) ^ (x & z) ^ (y & z);
  }

  constexpr std::uint32_t small_sigma0(std::uint32_t x) noexcept {
    return std::rotr(x, 7) ^ std::rotr(x, 18) ^ (x >> 3);
  }

  constexpr std::uint32_t small_sigma1(std::uint32_t x) noexcept {
    return std::rotr(x, 17) ^ std::rotr(x, 19) ^ (x >> 10);
  }

  // SHA-1 packs A,B,C,D from the most significant dword down. The first
  // message dword already contains E; later rounds obtain E from the old D.
  template<unsigned Selector, class V>
  constexpr V rounds1(V state, V message) noexcept {
    auto x = lanes(state);
    auto words = lanes(message);
    auto a = x[3];
    auto b = x[2];
    auto c = x[1];
    auto d = x[0];
    std::uint32_t e = 0;
    constexpr std::uint32_t constants[]{0x5a827999, 0x6ed9eba1, 0x8f1bbcdc, 0xca62c1d6};
    for (unsigned round = 0; round < 4; ++round) {
      std::uint32_t function;
      if constexpr (Selector == 0) {
        function = choose(b, c, d);
      } else if constexpr (Selector == 2) {
        function = majority(b, c, d);
      } else {
        function = b ^ c ^ d;
      }
      auto next = std::rotl(a, 5) + function + e + words[3 - round] + constants[Selector];
      e = d;
      d = c;
      c = std::rotl(b, 30);
      b = a;
      a = next;
    }
    std::array<std::uint32_t, 4> result{d, c, b, a};
    return V::load(result.data());
  }

  template<class V>
  constexpr V next_e(V state, V message) noexcept {
    auto x = lanes(state);
    auto result = lanes(message);
    result[3] += std::rotl(x[3], 30);
    return V::load(result.data());
  }

  template<class V>
  constexpr V message1_first(V a, V b) noexcept {
    auto x = lanes(a);
    auto y = lanes(b);
    std::array<std::uint32_t, 4> result{x[0] ^ y[2], x[1] ^ y[3], x[2] ^ x[0], x[3] ^ x[1]};
    return V::load(result.data());
  }

  template<class V>
  constexpr V message1_last(V a, V b) noexcept {
    auto x = lanes(a);
    auto y = lanes(b);
    std::array<std::uint32_t, 4> result{};
    result[3] = std::rotl(x[3] ^ y[2], 1);
    result[2] = std::rotl(x[2] ^ y[1], 1);
    result[1] = std::rotl(x[1] ^ y[0], 1);
    result[0] = std::rotl(x[0] ^ result[3], 1);
    return V::load(result.data());
  }

  // CDGH and ABEF are packed from the most significant dword down. Only
  // message dwords 0/1 participate; both already contain the round constants.
  template<class V>
  constexpr V rounds256(V cdgh, V abef, V message) noexcept {
    auto x = lanes(cdgh);
    auto y = lanes(abef);
    auto words = lanes(message);
    auto a = y[3];
    auto b = y[2];
    auto c = x[3];
    auto d = x[2];
    auto e = y[1];
    auto f = y[0];
    auto g = x[1];
    auto h = x[0];
    for (unsigned round = 0; round < 2; ++round) {
      auto sigma1 = std::rotr(e, 6) ^ std::rotr(e, 11) ^ std::rotr(e, 25);
      auto sigma0 = std::rotr(a, 2) ^ std::rotr(a, 13) ^ std::rotr(a, 22);
      auto first = h + sigma1 + choose(e, f, g) + words[round];
      auto second = sigma0 + majority(a, b, c);
      h = g;
      g = f;
      f = e;
      e = d + first;
      d = c;
      c = b;
      b = a;
      a = first + second;
    }
    std::array<std::uint32_t, 4> result{f, e, b, a};
    return V::load(result.data());
  }

  template<class V>
  constexpr V message256_first(V a, V b) noexcept {
    auto x = lanes(a);
    auto y = lanes(b);
    std::array<std::uint32_t, 4> result{x[0] + small_sigma0(x[1]), x[1] + small_sigma0(x[2]),
      x[2] + small_sigma0(x[3]), x[3] + small_sigma0(y[0])};
    return V::load(result.data());
  }

  template<class V>
  constexpr V message256_last(V a, V b) noexcept {
    auto x = lanes(a);
    auto y = lanes(b);
    std::array<std::uint32_t, 4> result{};
    result[0] = x[0] + small_sigma1(y[2]);
    result[1] = x[1] + small_sigma1(y[3]);
    result[2] = x[2] + small_sigma1(result[0]);
    result[3] = x[3] + small_sigma1(result[1]);
    return V::load(result.data());
  }
}
