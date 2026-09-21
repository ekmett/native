// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "constant_lanes.h"

namespace native::detail::arm_constant {
  constexpr std::uint8_t aes_product(std::uint8_t a, std::uint8_t b) noexcept {
    unsigned product = 0;
    for (unsigned bit = 0; bit < 8; ++bit) {
      if (b & (1u << bit)) product ^= a;
      a = static_cast<std::uint8_t>((a << 1) ^ ((a & 0x80) ? 0x11b : 0));
    }
    return static_cast<std::uint8_t>(product);
  }

  constexpr std::uint8_t aes_inverse(std::uint8_t a) noexcept {
    // a^254 in GF(2^8); this also maps zero to zero.
    auto result = std::uint8_t{1};
    for (unsigned exponent = 254; exponent; exponent >>= 1) {
      if (exponent & 1) result = aes_product(result, a);
      a = aes_product(a, a);
    }
    return result;
  }

  template<bool Inverse> constexpr std::uint8_t aes_substitute(std::uint8_t a) noexcept {
    if constexpr (Inverse)
      return aes_inverse(static_cast<std::uint8_t>(std::rotl(a, 1) ^ std::rotl(a, 3) ^ std::rotl(a, 6) ^ 5));
    else {
      auto x = aes_inverse(a);
      return static_cast<std::uint8_t>(x ^ std::rotl(x, 1) ^ std::rotl(x, 2) ^ std::rotl(x, 3) ^ std::rotl(x, 4) ^ 0x63);
    }
  }

  template<bool Inverse, class V> constexpr V aes_round(V state, V key) noexcept {
    auto a = lanes(state), k = lanes(key), result = a;
    for (unsigned col = 0; col < 4; ++col) for (unsigned row = 0; row < 4; ++row) {
      unsigned source = 4 * ((col + (Inverse ? 4 - row : row)) % 4) + row;
      result[4 * col + row] = aes_substitute<Inverse>(a[source] ^ k[source]);
    }
    return pack<V>(result);
  }

  template<bool Inverse, class V> constexpr V aes_mix(V state) noexcept {
    auto a = lanes(state), result = a;
    constexpr std::uint8_t coefficients[2][4]{{2, 3, 1, 1}, {14, 11, 13, 9}};
    for (unsigned col = 0; col < 4; ++col) for (unsigned row = 0; row < 4; ++row) {
      result[4 * col + row] = 0;
      for (unsigned term = 0; term < 4; ++term)
        result[4 * col + row] ^= aes_product(a[4 * col + term], coefficients[Inverse][(term + 4 - row) % 4]);
    }
    return pack<V>(result);
  }

  template<class R, unsigned Offset, class V> constexpr R pmull_bytes(V a, V b) noexcept {
    auto x = lanes(a), y = lanes(b);
    std::array<std::uint16_t, 8> result{};
    for (unsigned i = 0; i < 8; ++i) for (unsigned bit = 0; bit < 8; ++bit)
      if ((y[i + Offset] >> bit) & 1) result[i] ^= static_cast<std::uint16_t>(x[i + Offset]) << bit;
    return pack<R>(result);
  }

  template<class T> constexpr T choose(T a, T b, T c) noexcept { return c ^ (a & (b ^ c)); }
  template<class T> constexpr T majority(T a, T b, T c) noexcept { return (a & b) | (c & (a | b)); }
  template<int Mode, class V> constexpr V sha1_rounds(V state, std::uint32_t e, V message) noexcept {
    auto a = lanes(state), w = lanes(message);
    for (unsigned i = 0; i < 4; ++i) {
      auto f = Mode == 0 ? choose(a[1], a[2], a[3]) : Mode == 1 ? a[1] ^ a[2] ^ a[3] : majority(a[1], a[2], a[3]);
      auto next = std::rotl(a[0], 5) + f + e + w[i];
      e = a[3]; a[3] = a[2]; a[2] = std::rotl(a[1], 30); a[1] = a[0]; a[0] = next;
    }
    return pack<V>(a);
  }
  template<class V> constexpr V sha1su0(V x, V y, V z) noexcept {
    auto a = lanes(x), b = lanes(y), c = lanes(z);
    return pack<V>(std::array{a[0] ^ a[2] ^ c[0], a[1] ^ a[3] ^ c[1], a[2] ^ b[0] ^ c[2], a[3] ^ b[1] ^ c[3]});
  }
  template<class V> constexpr V sha1su1(V x, V y) noexcept {
    auto a = lanes(x), b = lanes(y);
    auto first = std::rotl(a[0] ^ b[1], 1);
    return pack<V>(std::array{first, std::rotl(a[1] ^ b[2], 1), std::rotl(a[2] ^ b[3], 1), std::rotl(a[3] ^ first, 1)});
  }
  constexpr std::uint32_t small0(std::uint32_t x) noexcept { return std::rotr(x, 7) ^ std::rotr(x, 18) ^ (x >> 3); }
  constexpr std::uint32_t small1(std::uint32_t x) noexcept { return std::rotr(x, 17) ^ std::rotr(x, 19) ^ (x >> 10); }
  constexpr std::uint64_t small0(std::uint64_t x) noexcept { return std::rotr(x, 1) ^ std::rotr(x, 8) ^ (x >> 7); }
  constexpr std::uint64_t small1(std::uint64_t x) noexcept { return std::rotr(x, 19) ^ std::rotr(x, 61) ^ (x >> 6); }
  constexpr std::uint32_t big0(std::uint32_t x) noexcept { return std::rotr(x, 2) ^ std::rotr(x, 13) ^ std::rotr(x, 22); }
  constexpr std::uint32_t big1(std::uint32_t x) noexcept { return std::rotr(x, 6) ^ std::rotr(x, 11) ^ std::rotr(x, 25); }
  constexpr std::uint64_t big0(std::uint64_t x) noexcept { return std::rotr(x, 28) ^ std::rotr(x, 34) ^ std::rotr(x, 39); }
  constexpr std::uint64_t big1(std::uint64_t x) noexcept { return std::rotr(x, 14) ^ std::rotr(x, 18) ^ std::rotr(x, 41); }
  template<bool Second, class V> constexpr V sha256_rounds(V first, V second, V message) noexcept {
    auto x = lanes(Second ? second : first), y = lanes(Second ? first : second), w = lanes(message);
    for (unsigned i = 0; i < 4; ++i) {
      auto t = y[3] + big1(y[0]) + choose(y[0], y[1], y[2]) + w[i];
      auto next = t + big0(x[0]) + majority(x[0], x[1], x[2]);
      y = {x[3] + t, y[0], y[1], y[2]};
      x = {next, x[0], x[1], x[2]};
    }
    return pack<V>(Second ? y : x);
  }
  template<class V> constexpr V sha256su0(V x, V y) noexcept {
    auto a = lanes(x), b = lanes(y);
    return pack<V>(std::array{a[0] + small0(a[1]), a[1] + small0(a[2]), a[2] + small0(a[3]), a[3] + small0(b[0])});
  }
  template<class V> constexpr V sha256su1(V x, V y, V z) noexcept {
    auto a = lanes(x), b = lanes(y), c = lanes(z);
    a[0] += b[1] + small1(c[2]); a[1] += b[2] + small1(c[3]);
    a[2] += b[3] + small1(a[0]); a[3] += c[0] + small1(a[1]);
    return pack<V>(a);
  }
  template<class V> constexpr V sha512h(V x, V y, V z) noexcept {
    auto a = lanes(x), b = lanes(y), c = lanes(z);
    a[1] += big1(c[1]) + choose(c[1], b[0], b[1]);
    auto e = a[1] + c[0];
    a[0] += big1(e) + choose(e, c[1], b[0]);
    return pack<V>(a);
  }
  template<class V> constexpr V sha512h2(V x, V y, V z) noexcept {
    auto a = lanes(x), b = lanes(y), c = lanes(z);
    a[1] += big0(c[0]) + majority(c[0], c[1], b[0]);
    a[0] += big0(a[1]) + majority(a[1], c[0], c[1]);
    return pack<V>(a);
  }
  template<class V> constexpr V sha512su0(V x, V y) noexcept {
    auto a = lanes(x), b = lanes(y);
    return pack<V>(std::array{a[0] + small0(a[1]), a[1] + small0(b[0])});
  }
  template<class V> constexpr V sha512su1(V x, V y, V z) noexcept {
    auto a = lanes(x), b = lanes(y), c = lanes(z);
    return pack<V>(std::array{a[0] + small1(b[0]) + c[0], a[1] + small1(b[1]) + c[1]});
  }
  template<bool Clear, class V> constexpr V logical(V x, V y, V z) noexcept {
    auto a = lanes(x), b = lanes(y), c = lanes(z);
    for (unsigned i = 0; i < V::lanes; ++i) a[i] = Clear ? a[i] ^ (b[i] & ~c[i]) : a[i] ^ b[i] ^ c[i];
    return pack<V>(a);
  }
  template<class V> constexpr V rax1(V x, V y) noexcept {
    auto a = lanes(x), b = lanes(y);
    return pack<V>(std::array{a[0] ^ std::rotl(b[0], 1), a[1] ^ std::rotl(b[1], 1)});
  }
  template<unsigned Rotate, class V> constexpr V xar(V x, V y) noexcept {
    auto a = lanes(x), b = lanes(y);
    return pack<V>(std::array{std::rotr(a[0] ^ b[0], Rotate), std::rotr(a[1] ^ b[1], Rotate)});
  }
}
