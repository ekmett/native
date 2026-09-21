// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

namespace fixture {
  // SM3("abc"), with complete schedule and compression built from public ops.
  // State vectors are D,C,B,A and H,G,F,E; message words remain ascending.
  template <native::isa<native::arm> A>
  __attribute__((target("sm4"))) constexpr bool sm3_known_answer() {
    std::array<std::uint32_t, 68> w{};
    w[0] = 0x61626380;
    w[15] = 24;
    for (unsigned j = 16; j < 68; j += 4) {
      auto read = [&](unsigned i) { return pack<A>({w[i], w[i + 1], w[i + 2], w[i + 3]}); };
      auto partial = native::sm3partw1(read(j - 16), read(j - 9), read(j - 4));
      auto next = unpack(native::sm3partw2(partial, read(j - 6), read(j - 13)));
      for (unsigned i = 0; i < 4; ++i)
        w[j + i] = next[i];
    }
    constexpr words abcd{0xda8a0600, 0x172442d7, 0x4914b2b9, 0x7380166f};
    constexpr words efgh{0xb0fb0e4e, 0xe38dee4d, 0x163138aa, 0xa96f30bc};
    auto left = pack<A>(abcd);
    auto right = pack<A>(efgh);
    for (unsigned j = 0; j < 64; ++j) {
      auto constant = std::rotl(std::uint32_t(j < 16 ? 0x79cc4519 : 0x7a879d8a), int(j));
      auto ss1 = native::sm3ss1(left, right, pack<A>({0, 0, 0, constant}));
      auto message = pack<A>({w[j], 0, 0, 0});
      auto derived = pack<A>({w[j] ^ w[j + 4], 0, 0, 0});
      if (j < 16) {
        left = native::sm3tt1a<A, 0>(left, ss1, derived);
        right = native::sm3tt2a<A, 0>(right, ss1, message);
      } else {
        left = native::sm3tt1b<A, 0>(left, ss1, derived);
        right = native::sm3tt2b<A, 0>(right, ss1, message);
      }
    }
    auto l = unpack(left);
    auto r = unpack(right);
    for (unsigned i = 0; i < 4; ++i) {
      l[i] ^= abcd[i];
      r[i] ^= efgh[i];
    }
    return l == words{0xdc10e4e2, 0xd1f2d46b, 0x62eeedd9, 0x66c7f0f4} &&
           r == words{0x8f4ba8e0, 0x297da02b, 0x5cf2f7a2, 0x4167c487};
  }

  // GB/T 32907-2016 example: same 128-bit key and plaintext.
  template <native::isa<native::arm> A>
  __attribute__((target("sm4"))) constexpr bool sm4_known_answer() {
    constexpr words original{0x01234567, 0x89abcdef, 0xfedcba98, 0x76543210};
    auto key = pack<A>({original[0] ^ 0xa3b1bac6, original[1] ^ 0x56aa3350,
                        original[2] ^ 0x677d9197, original[3] ^ 0xb27022dc});
    std::array<std::uint32_t, 32> round_keys{};
    for (unsigned group = 0; group < 8; ++group) {
      words constant{};
      for (unsigned i = 0; i < 4; ++i)
        for (unsigned byte = 0; byte < 4; ++byte)
          constant[i] = (constant[i] << 8) | (((group * 16 + i * 4 + byte) * 7) & 255);
      key = native::sm4ekey(key, pack<A>(constant));
      auto k = unpack(key);
      for (unsigned i = 0; i < 4; ++i)
        round_keys[group * 4 + i] = k[i];
    }
    if (words{round_keys[0], round_keys[1], round_keys[2], round_keys[3]} !=
        words{0xf12186f9, 0x41662b61, 0x5a6ab19a, 0x7ba92077})
      return false;
    auto data = pack<A>(original);
    for (unsigned group = 0; group < 8; ++group) {
      auto i = group * 4;
      data = native::sm4e(
          data, pack<A>({round_keys[i], round_keys[i + 1], round_keys[i + 2], round_keys[i + 3]}));
      if (group == 0 && unpack(data) != words{0x27fad345, 0xa18b4cb2, 0x11c1e22a, 0xcc13e2ee})
        return false;
    }
    constexpr words ciphertext{0x681edf34, 0xd206965e, 0x86b3e94f, 0x536e4246};
    auto result = unpack(data);
    for (unsigned i = 0; i < 4; ++i)
      if (result[3 - i] != ciphertext[i])
        return false;
    data = pack<A>(ciphertext);
    for (unsigned group = 0; group < 8; ++group) {
      auto i = 31 - group * 4;
      data = native::sm4e(
          data, pack<A>({round_keys[i], round_keys[i - 1], round_keys[i - 2], round_keys[i - 3]}));
    }
    result = unpack(data);
    for (unsigned i = 0; i < 4; ++i)
      if (result[3 - i] != original[i])
        return false;
    return true;
  }

  static_assert(sm3_known_answer<native::neon>());
  static_assert(sm3_known_answer<architecture>());
  static_assert(sm4_known_answer<native::neon>());
  static_assert(sm4_known_answer<architecture>());

  __attribute__((noinline, target("sm4"))) inline bool known_answers_runtime() {
    return sm3_known_answer<architecture>() && sm4_known_answer<architecture>();
  }
} // namespace fixture
