// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

#include <array>
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <immintrin.h>
#include <native/attributes.h>

#if defined(__SHA__) || defined(__AES__) || defined(__VAES__) || defined(__AVX__)
#error Crypto admission must run at the provider baseline
#endif
#if NATIVE_TEST_INTERFACE == 1
import native.x86.vaes;
import native.features;
#else
import native;
#endif
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

#include <array>
#include <cstdint>

namespace vaes_fixture {
  using bytes = std::array<std::uint8_t, 16>;

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

}

namespace vaes_fixture {
  constexpr auto vex128 = native::target_features<native::x86>("avx,aes");
  constexpr auto vex256 = native::target_features<native::x86>("avx,vaes");
  constexpr auto evex512 = native::target_features<native::x86>("avx512f,vaes");
  constexpr auto weak128 = native::target_features<native::x86>("sse2");
  constexpr auto weak256 = native::target_features<native::x86>("avx");
  constexpr auto weak512 = native::target_features<native::x86>("avx512f");
  constexpr auto broad = native::avx512 & native::x86_feature::aes & native::x86_feature::vaes;

  template<unsigned N>
  using block = std::array<std::uint8_t, N>;
  template<unsigned N>
  using results = std::array<block<N>, 4>;

  template<unsigned N>
  constexpr results<N> oracle(block<N> const & a, block<N> const & key) {
    results<N> out{};
    for (unsigned base = 0; base < N; base += 16) {
      bytes state{}, round_key{};
      for (unsigned i = 0; i < 16; ++i) {
        state[i] = a[base + i];
        round_key[i] = key[base + i];
      }
      for (unsigned operation = 0; operation < 4; ++operation) {
        auto value = round(state, round_key, operation >= 2, operation % 2 != 0);
        for (unsigned i = 0; i < 16; ++i) {
          out[operation][base + i] = value[i];
        }
      }
    }
    return out;
  }

  template<native::isa<native::x86> A, unsigned N>
  consteval results<N> constant(block<N> const & a, block<N> const & key) {
    using vector = native::simd<std::uint8_t, N, A>;
    auto x = vector::load(a.data());
    auto y = vector::load(key.data());
    results<N> out{};
    native::vaesenc<A>(x, y).store(out[0].data());
    native::vaesenclast<A>(x, y).store(out[1].data());
    native::vaesdec<A>(x, y).store(out[2].data());
    native::vaesdeclast<A>(x, y).store(out[3].data());
    return out;
  }

  template<native::isa<native::x86> Strong, native::isa<native::x86> Weak, unsigned N>
  consteval bool constant_bank() {
    for (unsigned trial = 0; trial < 16; ++trial) {
      block<N> state{}, key{};
      for (unsigned i = 0; i < N; ++i) {
        state[i] = static_cast<std::uint8_t>(trial * 16 + i);
        key[i] = static_cast<std::uint8_t>(255 - i * 7 + trial);
      }
      auto expected = oracle<N>(state, key);
      if (constant<Strong, N>(state, key) != expected || constant<Weak, N>(state, key) != expected ||
          constant<broad, N>(state, key) != expected) {
        return false;
      }
    }
    return true;
  }
  static_assert(constant_bank<vex128, weak128, 16>());
  static_assert(constant_bank<vex256, weak256, 32>());
  static_assert(constant_bank<evex512, weak512, 64>());

  // FIPS 197 Appendix B's first round, repeated with independently keyed lanes
  // above. These fixed bytes anchor the table oracle to a published AES vector.
  constexpr bytes initial{0x19, 0x3d, 0xe3, 0xbe, 0xa0, 0xf4, 0xe2, 0x2b,
    0x9a, 0xc6, 0x8d, 0x2a, 0xe9, 0xf8, 0x48, 0x08};
  constexpr bytes round_key{0xa0, 0xfa, 0xfe, 0x17, 0x88, 0x54, 0x2c, 0xb1,
    0x23, 0xa3, 0x39, 0x39, 0x2a, 0x6c, 0x76, 0x05};
  constexpr bytes first{0xa4, 0x9c, 0x7f, 0xf2, 0x68, 0x9f, 0x35, 0x2b,
    0x6b, 0x5b, 0xea, 0x43, 0x02, 0x6a, 0x50, 0x49};
  static_assert(constant<vex128, 16>(initial, round_key)[0] == first);

  native_noinline native_target("avx,aes")
  void evaluate128(block<16> const & a, block<16> const & key, results<16> & out) noexcept {
    using vector = native::simd<std::uint8_t, 16, vex128>;
    auto x = vector::load(a.data());
    auto y = vector::load(key.data());
    native::vaesenc<vex128>(x, y).store(out[0].data());
    native::vaesenclast<vex128>(x, y).store(out[1].data());
    native::vaesdec<vex128>(x, y).store(out[2].data());
    native::vaesdeclast<vex128>(x, y).store(out[3].data());
  }

  native_noinline native_target("avx,vaes")
  void evaluate256(block<32> const & a, block<32> const & key, results<32> & out) noexcept {
    using vector = native::simd<std::uint8_t, 32, vex256>;
    auto x = vector::load(a.data());
    auto y = vector::load(key.data());
    native::vaesenc<vex256>(x, y).store(out[0].data());
    native::vaesenclast<vex256>(x, y).store(out[1].data());
    native::vaesdec<vex256>(x, y).store(out[2].data());
    native::vaesdeclast<vex256>(x, y).store(out[3].data());
  }

  native_noinline native_target("avx512f,vaes")
  void evaluate512(block<64> const & a, block<64> const & key, results<64> & out) noexcept {
    using vector = native::simd<std::uint8_t, 64, evex512>;
    auto x = vector::load(a.data());
    auto y = vector::load(key.data());
    native::vaesenc<evex512>(x, y).store(out[0].data());
    native::vaesenclast<evex512>(x, y).store(out[1].data());
    native::vaesdec<evex512>(x, y).store(out[2].data());
    native::vaesdeclast<evex512>(x, y).store(out[3].data());
  }

  template<unsigned N>
  int exercise(native::isa<native::x86> requirements, void (*evaluate)(block<N> const &, block<N> const &, results<N> &) noexcept) {
    auto admission = native::classify_isa(native::observe_x86_capabilities(), requirements);
    if (!admission.admitted()) {
      std::printf("SKIP VAES %u-bit runtime: %s\n", N * 8, admission.reason());
      return 77;
    }
    std::uint32_t random = 0x9e3779b9;
    for (unsigned trial = 0; trial < 1024; ++trial) {
      block<N> a{}, key{};
      for (auto * input : {&a, &key}) {
        for (auto & lane : *input) {
          random ^= random << 13;
          random ^= random >> 17;
          random ^= random << 5;
          lane = static_cast<std::uint8_t>(trial == 0 ? 0 : trial == 1 ? 255 : random);
        }
      }
      results<N> out{};
      evaluate(a, key, out);
      auto expected = oracle<N>(a, key);
      for (unsigned operation = 0; operation < out.size(); ++operation) {
        if (out[operation] != expected[operation]) {
          std::printf("VAES %u-bit operation %u failed trial %u\n", N * 8, operation, trial);
          return 1;
        }
      }
    }
    return 0;
  }

  int run() {
    int a = exercise<16>(vex128, evaluate128);
    int b = exercise<32>(vex256, evaluate256);
    int c = exercise<64>(evex512, evaluate512);
    return a == 1 || b == 1 || c == 1 ? 1 : a == 77 && b == 77 && c == 77 ? 77 : 0;
  }
}

extern "C" native_noinline unsigned native_vaes_baseline_import(unsigned a, unsigned b) noexcept {
  return (a ^ b) + a;
}

int main() {
  return vaes_fixture::run();
}
