// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#include "prelude.h"
#include "oracle.h"

namespace sha_fixture {
  constexpr auto strong = native::target_features<native::x86>("sha");
  constexpr auto weak = native::target_features<native::x86>("sse2");
  constexpr auto broad = native::avx512 & native::x86_feature::sha;
  template<native::isa<native::x86> A>
  using vector = native::simd<std::uint32_t, 4, A>;

  template<native::isa<native::x86> A>
  consteval results constant(words a, words b, words c) {
    auto x = vector<A>::load(a.data());
    auto y = vector<A>::load(b.data());
    auto z = vector<A>::load(c.data());
    results out{};
    native::sha1rnds4<A, 0>(x, y).store(out[0].data());
    native::sha1rnds4<A, 1>(x, y).store(out[1].data());
    native::sha1rnds4<A, 2>(x, y).store(out[2].data());
    native::sha1rnds4<A, 3>(x, y).store(out[3].data());
    native::sha1nexte<A>(x, y).store(out[4].data());
    native::sha1msg1<A>(x, y).store(out[5].data());
    native::sha1msg2<A>(x, y).store(out[6].data());
    native::sha256rnds2<A>(x, y, z).store(out[7].data());
    native::sha256msg1<A>(x, y).store(out[8].data());
    native::sha256msg2<A>(x, y).store(out[9].data());
    return out;
  }

  consteval bool constant_bank() {
    for (unsigned trial = 0; trial < 64; ++trial) {
      words a{}, b{}, c{};
      for (unsigned lane = 0; lane < 4; ++lane) {
        a[lane] = std::rotl(0x80000001u + trial * 0x9e3779b9u, lane * 7);
        b[lane] = ~a[3 - lane] + trial * (lane + 1);
        c[lane] = 0xffffffffu - trial * 0x10203041u - lane;
      }
      auto expected = oracle(a, b, c);
      if (constant<strong>(a, b, c) != expected || constant<weak>(a, b, c) != expected ||
          constant<broad>(a, b, c) != expected) {
        return false;
      }
    }
    return true;
  }
  static_assert(constant_bank());

  // Compose the instruction message primitives and rounds into FIPS 180-4
  // SHA-1("abc"). A scalar recurrence independently checks all 80 schedule words.
  template<native::isa<native::x86> A>
  consteval bool digest() {
    std::array<std::uint32_t, 80> words1{};
    words1[0] = 0x61626380;
    words1[15] = 24;
    for (unsigned i = 16; i < 80; ++i) {
      words1[i] = std::rotl(words1[i - 3] ^ words1[i - 8] ^ words1[i - 14] ^ words1[i - 16], 1);
    }
    for (unsigned i = 16; i < 80; i += 4) {
      words a{}, b{}, c{};
      for (unsigned j = 0; j < 4; ++j) {
        a[3 - j] = words1[i - 16 + j];
        b[3 - j] = words1[i - 12 + j];
        c[3 - j] = words1[i - 4 + j];
      }
      auto first = native::sha1msg1<A>(vector<A>::load(a.data()), vector<A>::load(b.data()));
      first.store(a.data());
      for (unsigned j = 0; j < 4; ++j) {
        a[3 - j] ^= words1[i - 8 + j];
      }
      native::sha1msg2<A>(vector<A>::load(a.data()), vector<A>::load(c.data())).store(a.data());
      for (unsigned j = 0; j < 4; ++j) {
        if (a[3 - j] != words1[i + j]) {
          return false;
        }
      }
    }
    words state{0x10325476, 0x98badcfe, 0xefcdab89, 0x67452301};
    auto initial = state;
    std::uint32_t e = 0xc3d2e1f0;
    for (unsigned i = 0; i < 80; i += 4) {
      words message{words1[i + 3], words1[i + 2], words1[i + 1], words1[i] + e};
      auto previous = state;
      auto x = vector<A>::load(state.data());
      auto y = vector<A>::load(message.data());
      if (i < 20) {
        native::sha1rnds4<A, 0>(x, y).store(state.data());
      } else if (i < 40) {
        native::sha1rnds4<A, 1>(x, y).store(state.data());
      } else if (i < 60) {
        native::sha1rnds4<A, 2>(x, y).store(state.data());
      } else {
        native::sha1rnds4<A, 3>(x, y).store(state.data());
      }
      words zero{};
      native::sha1nexte<A>(vector<A>::load(previous.data()), vector<A>::load(zero.data())).store(zero.data());
      e = zero[3];
    }
    for (unsigned j = 0; j < 4; ++j) {
      state[j] += initial[j];
    }
    return state == words{0x7850c26c, 0xba3e2571, 0x4706816a, 0xa9993e36} && e + 0xc3d2e1f0u == 0x9cd0d89d;
  }
  static_assert(digest<strong>() && digest<weak>());

  template<native::isa<native::x86> A>
  consteval bool digest256() {
    constexpr std::uint32_t constants[]{
      0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
      0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
      0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
      0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
      0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
      0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
      0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
      0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };
    std::array<std::uint32_t, 64> schedule{};
    schedule[0] = 0x61626380;
    schedule[15] = 24;
    for (unsigned i = 16; i < 64; ++i) {
      schedule[i] = sigma(schedule[i - 2], true) + schedule[i - 7] +
        sigma(schedule[i - 15], false) + schedule[i - 16];
    }
    for (unsigned i = 16; i < 64; i += 4) {
      auto first = native::sha256msg1<A>(vector<A>::load(schedule.data() + i - 16),
        vector<A>::load(schedule.data() + i - 12));
      words adjusted{};
      first.store(adjusted.data());
      for (unsigned j = 0; j < 4; ++j) {
        adjusted[j] += schedule[i - 7 + j];
      }
      native::sha256msg2<A>(vector<A>::load(adjusted.data()),
        vector<A>::load(schedule.data() + i - 4)).store(adjusted.data());
      for (unsigned j = 0; j < 4; ++j) {
        if (adjusted[j] != schedule[i + j]) {
          return false;
        }
      }
    }
    words cdgh{0x5be0cd19, 0x1f83d9ab, 0xa54ff53a, 0x3c6ef372};
    words abef{0x9b05688c, 0x510e527f, 0xbb67ae85, 0x6a09e667};
    auto initial_cdgh = cdgh;
    auto initial_abef = abef;
    for (unsigned i = 0; i < 64; i += 2) {
      words message{schedule[i] + constants[i], schedule[i + 1] + constants[i + 1], 0xdeadbeef, 0x12345678};
      auto previous = abef;
      native::sha256rnds2<A>(vector<A>::load(cdgh.data()), vector<A>::load(abef.data()),
        vector<A>::load(message.data())).store(abef.data());
      cdgh = previous;
    }
    for (unsigned j = 0; j < 4; ++j) {
      cdgh[j] += initial_cdgh[j];
      abef[j] += initial_abef[j];
    }
    return abef == words{0x96177a9c, 0xb00361a3, 0x8f01cfea, 0xba7816bf} &&
      cdgh == words{0xf20015ad, 0xb410ff61, 0x5dae2223, 0x414140de};
  }
  static_assert(digest256<strong>() && digest256<weak>());

  native_noinline native_target("sha")
  void evaluate(words const & a, words const & b, words const & c, results & out) noexcept {
    auto x = vector<strong>::load(a.data());
    auto y = vector<strong>::load(b.data());
    auto z = vector<strong>::load(c.data());
    native::sha1rnds4<strong, 0>(x, y).store(out[0].data());
    native::sha1rnds4<strong, 1>(x, y).store(out[1].data());
    native::sha1rnds4<strong, 2>(x, y).store(out[2].data());
    native::sha1rnds4<strong, 3>(x, y).store(out[3].data());
    native::sha1nexte<strong>(x, y).store(out[4].data());
    native::sha1msg1<strong>(x, y).store(out[5].data());
    native::sha1msg2<strong>(x, y).store(out[6].data());
    native::sha256rnds2<strong>(x, y, z).store(out[7].data());
    native::sha256msg1<strong>(x, y).store(out[8].data());
    native::sha256msg2<strong>(x, y).store(out[9].data());
  }

  int run() {
    auto admission = native::classify_isa(native::observe_x86_capabilities(), strong);
    if (!admission.admitted()) {
      std::printf("SKIP SHA runtime: %s\n", admission.reason());
      return 77;
    }
    std::uint32_t random = 0x243f6a88;
    for (unsigned trial = 0; trial < 1024; ++trial) {
      words a{}, b{}, c{};
      for (auto * input : {&a, &b, &c}) {
        for (auto & lane : *input) {
          random ^= random << 13;
          random ^= random >> 17;
          random ^= random << 5;
          lane = trial == 0 ? 0 : trial == 1 ? 0xffffffffu : random;
        }
      }
      results out{};
      evaluate(a, b, c, out);
      auto expected = oracle(a, b, c);
      for (unsigned operation = 0; operation < out.size(); ++operation) {
        if (out[operation] != expected[operation]) {
          std::printf("SHA operation %u failed trial %u\n", operation, trial);
          return 1;
        }
      }
    }
    return 0;
  }
}
