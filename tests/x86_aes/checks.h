// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#include "prelude.h"

extern "C" bool native_aes_admission(
  std::uint32_t leaf1_ecx, std::uint32_t leaf1_edx, std::uint32_t max_basic_leaf = 1) noexcept;

namespace aes_fixture {
  using native::x86_feature;
  constexpr auto strong = native::feature_closure(native::isa<native::x86>{x86_feature::aes});
  constexpr auto weak = native::target_features<native::x86>("sse2");
  static_assert(strong.has(x86_feature::aes) && strong.has(x86_feature::sse2));
  static_assert(!strong.has(x86_feature::avx) && !strong.has(x86_feature::ssse3));
  using bytes = std::array<std::uint8_t, 16>;

  template<native::isa<native::x86> A>
  using vector = native::simd<std::uint8_t, 16, A>;

  template<native::isa<native::x86> A, class V>
  concept round_shape = requires(V x) {
    { native::aesenc<A>(x, x) } noexcept -> std::same_as<V>;
    { native::aesenclast<A>(x, x) } noexcept -> std::same_as<V>;
    { native::aesdec<A>(x, x) } noexcept -> std::same_as<V>;
    { native::aesdeclast<A>(x, x) } noexcept -> std::same_as<V>;
    { native::aesimc<A>(x) } noexcept -> std::same_as<V>;
  };

  template<native::isa<native::x86> A, class V, unsigned I>
  concept key_shape = requires(V x) {
    { native::aeskeygenassist<A, I>(x) } noexcept -> std::same_as<V>;
  };
  static_assert(round_shape<strong, vector<strong>> && round_shape<weak, vector<weak>>);
  static_assert(!round_shape<strong, vector<weak>> && !round_shape<strong, __m128i>);
  static_assert(!round_shape<strong, native::simd<float, 4, strong>>);
  static_assert(!round_shape<strong, native::simd<std::uint32_t, 4, strong>>);
  static_assert(key_shape<strong, vector<strong>, 255> && key_shape<weak, vector<weak>, 255>);
  static_assert(!key_shape<strong, vector<strong>, 256> && !key_shape<weak, vector<weak>, 256>);
  static_assert(!key_shape<strong, __m128i, 0> && !key_shape<strong, vector<weak>, 0>);

  template<auto A>
  concept architecture_shape = requires(vector<strong> x) { native::aesenc<A>(x, x); };
  static_assert(!architecture_shape<native::isa<native::arm>{}>);

  // Independent FIPS-197 substitution table. The production constant evaluator
  // computes multiplicative inverses and affine transforms instead.
  constexpr std::uint8_t sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
  };

  constexpr auto inverse_sbox = [] {
    std::array<std::uint8_t, 256> result{};
    for (unsigned i = 0; i < 256; ++i) {
      result[sbox[i]] = static_cast<std::uint8_t>(i);
    }
    return result;
  }();

  constexpr std::uint8_t twice(std::uint8_t x) {
    return static_cast<std::uint8_t>((x << 1) ^ ((x >> 7) * 0x1b));
  }

  constexpr bytes mix(bytes x, bool inverse) {
    // Inverse MixColumns factors as a pre-whitening step plus forward mixing.
    for (unsigned i = 0; i < 16; i += 4) {
      if (inverse) {
        auto u = twice(twice(x[i] ^ x[i + 2]));
        auto v = twice(twice(x[i + 1] ^ x[i + 3]));
        x[i] ^= u;
        x[i + 2] ^= u;
        x[i + 1] ^= v;
        x[i + 3] ^= v;
      }
      auto first = x[i];
      auto all = x[i] ^ x[i + 1] ^ x[i + 2] ^ x[i + 3];
      x[i] ^= all ^ twice(x[i] ^ x[i + 1]);
      x[i + 1] ^= all ^ twice(x[i + 1] ^ x[i + 2]);
      x[i + 2] ^= all ^ twice(x[i + 2] ^ x[i + 3]);
      x[i + 3] ^= all ^ twice(x[i + 3] ^ first);
    }
    return x;
  }

  constexpr bytes round(bytes x, bytes key, bool inverse, bool last) {
    for (auto & b : x) {
      b = inverse ? inverse_sbox[b] : sbox[b];
    }
    for (unsigned row = 1; row < 4; ++row) {
      for (unsigned step = 0; step < (inverse ? 4 - row : row); ++step) {
        auto first = x[row];
        for (unsigned col = 0; col < 3; ++col) {
          x[4 * col + row] = x[4 * (col + 1) + row];
        }
        x[12 + row] = first;
      }
    }
    if (!last) {
      x = mix(x, inverse);
    }
    for (unsigned i = 0; i < 16; ++i) {
      x[i] ^= key[i];
    }
    return x;
  }

  constexpr bytes keygen(bytes input, unsigned immediate) {
    bytes result{};
    for (unsigned half = 0; half < 2; ++half) {
      for (unsigned j = 0; j < 4; ++j) {
        result[8 * half + j] = sbox[input[8 * half + 4 + j]];
      }
      for (unsigned j = 0; j < 4; ++j) {
        result[8 * half + 4 + j] = result[8 * half + (j + 1) % 4];
      }
      result[8 * half + 4] ^= immediate;
    }
    return result;
  }

  using immediates = std::integer_sequence<unsigned, 0, 1, 2, 4, 8, 16, 32, 64, 128, 27, 54, 255>;
  constexpr unsigned immediate_values[]{0, 1, 2, 4, 8, 16, 32, 64, 128, 27, 54, 255};
  using outputs = std::array<bytes, 17>;

  constexpr outputs expected(bytes a, bytes b) {
    outputs result{
      round(a, b, false, false), round(a, b, false, true),
      round(a, b, true, false), round(a, b, true, true), mix(a, true)
    };
    for (unsigned i = 0; i < 12; ++i) {
      result[5 + i] = keygen(a, immediate_values[i]);
    }
    return result;
  }

  template<native::isa<native::x86> A, unsigned... I>
  consteval outputs constant(bytes input, bytes key, std::integer_sequence<unsigned, I...>) {
    auto a = vector<A>::load(input.data());
    auto b = vector<A>::load(key.data());
    outputs result{};
    native::aesenc<A>(a, b).store(result[0].data());
    native::aesenclast<A>(a, b).store(result[1].data());
    native::aesdec<A>(a, b).store(result[2].data());
    native::aesdeclast<A>(a, b).store(result[3].data());
    native::aesimc<A>(a).store(result[4].data());
    unsigned index = 5;
    (native::aeskeygenassist<A, I>(a).store(result[index++].data()), ...);
    return result;
  }

  template<unsigned... I>
  native_noinline native_target("aes")
  void evaluate(bytes const & input, bytes const & key, outputs & result,
    std::integer_sequence<unsigned, I...>) {
    auto a = vector<strong>::load(input.data());
    auto b = vector<strong>::load(key.data());
    native::aesenc<strong>(a, b).store(result[0].data());
    native::aesenclast<strong>(a, b).store(result[1].data());
    native::aesdec<strong>(a, b).store(result[2].data());
    native::aesdeclast<strong>(a, b).store(result[3].data());
    native::aesimc<strong>(a).store(result[4].data());
    unsigned index = 5;
    ([&] native_target("aes") {
      native::aeskeygenassist<strong, I>(a).store(result[index++].data());
    }(), ...);
  }

  constexpr bytes plaintext{
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
  };

  constexpr bytes key{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

  constexpr bytes ciphertext{
    0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30, 0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a
  };
  // FIPS-197 Appendix C.1 initial state and first-round output.

  constexpr bytes initial{
    0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0
  };

  constexpr bytes first_key{
    0xd6, 0xaa, 0x74, 0xfd, 0xd2, 0xaf, 0x72, 0xfa, 0xda, 0xa6, 0x78, 0xf1, 0xd6, 0xab, 0x76, 0xfe
  };

  constexpr bytes first_round{
    0x89, 0xd8, 0x10, 0xe8, 0x85, 0x5a, 0xce, 0x68, 0x2d, 0x18, 0x43, 0xd8, 0xcb, 0x12, 0x8f, 0xe4
  };
  static_assert(round(initial, first_key, false, false) == first_round);
  static_assert(constant<strong>(initial, first_key, immediates{}) == expected(initial, first_key));
  static_assert(constant<weak>(initial, first_key, immediates{}) == expected(initial, first_key));
  static_assert(constant<strong>(bytes{}, bytes{}, immediates{}) == expected(bytes{}, bytes{}));
  static_assert(constant<weak>(plaintext, key, immediates{}) == expected(plaintext, key));

  constexpr auto round_keys = [] {
    std::array<bytes, 11> result{};
    result[0] = key;
    std::uint8_t rcon = 1;
    for (unsigned round = 1; round < 11; ++round) {
      auto const & old = result[round - 1];
      auto & next = result[round];
      for (unsigned i = 0; i < 4; ++i) {
        next[i] = old[i] ^ sbox[old[12 + (i + 1) % 4]] ^ (i == 0 ? rcon : 0);
      }
      for (unsigned i = 4; i < 16; ++i) {
        next[i] = old[i] ^ next[i - 4];
      }
      rcon = twice(rcon);
    }
    return result;
  }();

  template<native::isa<native::x86> A>
  consteval bytes encrypt_constant() {
    auto state = vector<A>::load(initial.data());
    for (unsigned i = 1; i < 10; ++i) {
      state = native::aesenc<A>(state, vector<A>::load(round_keys[i].data()));
    }
    state = native::aesenclast<A>(state, vector<A>::load(round_keys[10].data()));
    bytes result{};
    state.store(result.data());
    return result;
  }

  template<native::isa<native::x86> A>
  consteval bytes decrypt_constant() {
    bytes input = ciphertext;
    for (unsigned i = 0; i < 16; ++i) {
      input[i] ^= round_keys[10][i];
    }
    auto state = vector<A>::load(input.data());
    for (unsigned i = 9; i > 0; --i) {
      state = native::aesdec<A>(state, native::aesimc<A>(vector<A>::load(round_keys[i].data())));
    }
    state = native::aesdeclast<A>(state, vector<A>::load(key.data()));
    bytes result{};
    state.store(result.data());
    return result;
  }
  static_assert(encrypt_constant<strong>() == ciphertext);
  static_assert(encrypt_constant<weak>() == ciphertext);
  static_assert(decrypt_constant<strong>() == plaintext);
  static_assert(decrypt_constant<weak>() == plaintext);

  native_noinline native_target("aes")
  bool full_cipher() {
    auto state = vector<strong>::load(initial.data());
    for (unsigned i = 1; i < 10; ++i) {
      state = native::aesenc<strong>(state, vector<strong>::load(round_keys[i].data()));
    }
    state = native::aesenclast<strong>(state, vector<strong>::load(round_keys[10].data()));
    bytes result{};
    state.store(result.data());
    if (result != ciphertext) {
      return false;
    }
    for (unsigned i = 0; i < 16; ++i) {
      result[i] ^= round_keys[10][i];
    }
    state = vector<strong>::load(result.data());
    for (unsigned i = 9; i > 0; --i) {
      state = native::aesdec<strong>(state,
        native::aesimc<strong>(vector<strong>::load(round_keys[i].data())));
    }
    state = native::aesdeclast<strong>(state, vector<strong>::load(key.data()));
    state.store(result.data());
    return result == plaintext;
  }

  int run() {
    constexpr std::uint32_t sse2 = (1u << 23) | (1u << 25) | (1u << 26);
    if (native_aes_admission(1u << 25, sse2, 0)) {
      std::puts("AES admission accepted an unobserved CPUID leaf");
      return 1;
    }
    for (unsigned bit = 0; bit < 32; ++bit) {
      if (native_aes_admission(1u << bit, sse2) != (bit == 25)) {
        std::puts("AES admission confused CPUID feature bits");
        return 1;
      }
    }
    for (unsigned bit : {23u, 25u, 26u}) {
      if (native_aes_admission(1u << 25, sse2 & ~(1u << bit))) {
        std::puts("AES admission ignored a register prerequisite");
        return 1;
      }
    }
    auto admission = native::classify_isa(native::observe_x86_capabilities(), strong);
    if (!admission.admitted()) {
      std::printf("SKIP AES: %s\n", admission.reason());
      return 77;
    }
    if (!full_cipher()) {
      std::puts("AES-128 known-answer encrypt/decrypt failed");
      return 1;
    }
    std::uint64_t random = 0x243f6a8885a308d3;
    for (unsigned trial = 0; trial < 256; ++trial) {
      bytes a{};
      bytes b{};
      for (unsigned i = 0; i < 16; ++i) {
        random ^= random << 13;
        random ^= random >> 7;
        random ^= random << 17;
        a[i] = trial < 16 ? static_cast<std::uint8_t>(trial * 16 + i) : static_cast<std::uint8_t>(random);
        b[i] = static_cast<std::uint8_t>(random >> 32);
      }
      outputs actual{};
      evaluate(a, b, actual, immediates{});
      if (actual != expected(a, b)) {
        std::printf("AES oracle mismatch at trial %u\n", trial);
        return 1;
      }
    }
    outputs actual{};
    evaluate(initial, first_key, actual, immediates{});
    if (actual != constant<weak>(initial, first_key, immediates{})) {
      return 1;
    }
    std::puts("AES: FIPS-197 encrypt/decrypt, constexpr and 256 independent round cases passed");
    return 0;
  }
}
