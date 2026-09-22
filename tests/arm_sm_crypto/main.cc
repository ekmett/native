// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <cstdint>

namespace reference {
  using words = std::array<std::uint32_t, 4>;
  using input = std::array<words, 3>;

  constexpr std::uint32_t rotate(std::uint32_t x, unsigned distance) {
    auto n = distance % 32;
    return std::uint32_t((std::uint64_t(x) << n) | (std::uint64_t(x) >> ((32 - n) % 32)));
  }

  // Bit truth tables give parity, majority and choice independently of the
  // production word-level expressions. The high argument is the selector.
  constexpr std::uint32_t boolean(std::uint32_t a, std::uint32_t b, std::uint32_t c,
                                  unsigned table) {
    std::uint32_t result = 0;
    for (unsigned bit = 0; bit < 32; ++bit) {
      auto index = (((a >> bit) & 1) << 2) | (((b >> bit) & 1) << 1) | ((c >> bit) & 1);
      result |= ((table >> index) & 1) << bit;
    }
    return result;
  }

  constexpr words sm3ss1(input const &in) {
    return {0, 0, 0, rotate(rotate(in[0][3], 12) + in[1][3] + in[2][3], 7)};
  }

  constexpr words tt(input const &in, unsigned lane, bool second, bool late) {
    auto const &x = in[0];
    auto f = boolean(x[3], x[2], x[1], !late ? 0x96 : second ? 0xca : 0xe8);
    std::uint64_t sum = f;
    sum += x[0];
    sum += second ? in[1][3] : in[1][3] ^ rotate(x[3], 12);
    sum += in[2][lane];
    auto next = std::uint32_t(sum);
    if (second)
      next = next ^ rotate(next, 9) ^ rotate(next, 17);
    return {x[1], rotate(x[2], second ? 19 : 9), x[3], next};
  }

  constexpr words part1(input const &in) {
    words out{};
    for (unsigned i = 0; i < 4; ++i) {
      auto feedback = i == 3 ? out[0] : in[2][i + 1];
      auto base = in[0][i] ^ in[1][i] ^ rotate(feedback, 15);
      out[i] = base ^ rotate(base, 15) ^ rotate(base, 23);
    }
    return out;
  }

  constexpr words part2(input const &in) {
    words out{};
    for (unsigned i = 0; i < 4; ++i)
      out[i] = in[0][i] ^ in[1][i] ^ rotate(in[2][i], 7);
    auto feedback = in[1][0] ^ rotate(in[2][0], 7);
    out[3] ^= rotate(feedback, 15) ^ rotate(feedback, 30) ^ rotate(feedback, 38);
    return out;
  }

  // Fixed algorithm data in a separately decoded representation.
  inline constexpr char substitution[] = "d690e9fecce13db716b614c228fb2c05"
                                         "2b679a762abe04c3aa44132649860699"
                                         "9c4250f491ef987a33540b43edcfac62"
                                         "e4b31ca9c908e89580df94fa758f3fa6"
                                         "4707a7fcf37317ba83593c19e6854fa8"
                                         "686b81b27164da8bf8eb0f4b70569d35"
                                         "1e240e5e6358d1a225227c3b01217887"
                                         "d40046579fd327524c3602e7a0c4c89e"
                                         "eabf8ad240c738b5a3f7f2cef96115a1"
                                         "e0ae5da49b341a55ad933230f58cb1e3"
                                         "1df6e22e8266ca60c02923ab0d534e6f"
                                         "d5db3745defd8e2f03ff6a726d6c5b51"
                                         "8d1baf92bbddbc7f11d95c411f105ad8"
                                         "0ac13188a5cd7bbd2d74d012b8e5b4b0"
                                         "8969974a0c96777e65b9f109c56ec684"
                                         "18f07dec3adc4d2079ee5f3ed7cb3948";

  constexpr unsigned digit(char x) {
    return x <= '9' ? unsigned(x - '0') : unsigned(x - 'a' + 10);
  }

  constexpr std::uint32_t tau(std::uint32_t x) {
    std::uint32_t out = 0;
    for (unsigned i = 0; i < 4; ++i) {
      auto index = ((x >> (24 - i * 8)) & 255) * 2;
      out = (out << 8) | (digit(substitution[index]) * 16 + digit(substitution[index + 1]));
    }
    return out;
  }

  constexpr words sm4(input const &in, bool key) {
    std::array<std::uint32_t, 8> sequence{};
    for (unsigned i = 0; i < 4; ++i)
      sequence[i] = in[0][i];
    for (unsigned i = 0; i < 4; ++i) {
      auto nonlinear = tau(sequence[i + 1] ^ sequence[i + 2] ^ sequence[i + 3] ^ in[1][i]);
      auto linear = key ? nonlinear ^ rotate(nonlinear, 13) ^ rotate(nonlinear, 23)
                        : nonlinear ^ rotate(nonlinear, 2) ^ rotate(nonlinear, 10) ^
                              rotate(nonlinear, 18) ^ rotate(nonlinear, 24);
      sequence[i + 4] = sequence[i] ^ linear;
    }
    return {sequence[4], sequence[5], sequence[6], sequence[7]};
  }
} // namespace reference
#include "../property_check.h"
#include <bit>
#include <cstdio>
#include <type_traits>
import native.arm.sm3;
import native.arm.sm4;

namespace fixture {
  using reference::input;
  using reference::words;
  constexpr auto architecture = native::target_features<native::arm>("sm4");
  template <native::isa<native::arm> A> using vector = native::simd<std::uint32_t, 4, A>;

  template <native::isa<native::arm> A> constexpr vector<A> pack(words x) {
    return vector<A>::from_native(std::bit_cast<typename vector<A>::native_type>(x));
  }

  template <class V> constexpr words unpack(V x) {
    return std::bit_cast<words>(x.to_native());
  }

  struct sm3ss1 {
    static constexpr char const *name = "sm3ss1";

    static constexpr words expected(input const &in) {
      return reference::sm3ss1(in);
    }

    template <native::isa<native::arm> A>
    __attribute__((target("sm4"))) static constexpr words evaluate(input const &in) {
      return unpack(native::sm3ss1(pack<A>(in[0]), pack<A>(in[1]), pack<A>(in[2])));
    }

    __attribute__((noinline, target("sm4"))) static words runtime(input const &in) {
      return evaluate<architecture>(in);
    }
  };

  struct sm3tt1a0 {
    static constexpr char const *name = "sm3tt1a0";

    static constexpr words expected(input const &in) {
      return reference::tt(in, 0, false, false);
    }

    template <native::isa<native::arm> A>
    __attribute__((target("sm4"))) static constexpr words evaluate(input const &in) {
      return unpack(native::sm3tt1a<A, 0>(pack<A>(in[0]), pack<A>(in[1]), pack<A>(in[2])));
    }

    __attribute__((noinline, target("sm4"))) static words runtime(input const &in) {
      return evaluate<architecture>(in);
    }
  };

  struct sm3tt1a1 {
    static constexpr char const *name = "sm3tt1a1";

    static constexpr words expected(input const &in) {
      return reference::tt(in, 1, false, false);
    }

    template <native::isa<native::arm> A>
    __attribute__((target("sm4"))) static constexpr words evaluate(input const &in) {
      return unpack(native::sm3tt1a<A, 1>(pack<A>(in[0]), pack<A>(in[1]), pack<A>(in[2])));
    }

    __attribute__((noinline, target("sm4"))) static words runtime(input const &in) {
      return evaluate<architecture>(in);
    }
  };

  struct sm3tt1a2 {
    static constexpr char const *name = "sm3tt1a2";

    static constexpr words expected(input const &in) {
      return reference::tt(in, 2, false, false);
    }

    template <native::isa<native::arm> A>
    __attribute__((target("sm4"))) static constexpr words evaluate(input const &in) {
      return unpack(native::sm3tt1a<A, 2>(pack<A>(in[0]), pack<A>(in[1]), pack<A>(in[2])));
    }

    __attribute__((noinline, target("sm4"))) static words runtime(input const &in) {
      return evaluate<architecture>(in);
    }
  };

  struct sm3tt1a3 {
    static constexpr char const *name = "sm3tt1a3";

    static constexpr words expected(input const &in) {
      return reference::tt(in, 3, false, false);
    }

    template <native::isa<native::arm> A>
    __attribute__((target("sm4"))) static constexpr words evaluate(input const &in) {
      return unpack(native::sm3tt1a<A, 3>(pack<A>(in[0]), pack<A>(in[1]), pack<A>(in[2])));
    }

    __attribute__((noinline, target("sm4"))) static words runtime(input const &in) {
      return evaluate<architecture>(in);
    }
  };

  struct sm3tt1b0 {
    static constexpr char const *name = "sm3tt1b0";

    static constexpr words expected(input const &in) {
      return reference::tt(in, 0, false, true);
    }

    template <native::isa<native::arm> A>
    __attribute__((target("sm4"))) static constexpr words evaluate(input const &in) {
      return unpack(native::sm3tt1b<A, 0>(pack<A>(in[0]), pack<A>(in[1]), pack<A>(in[2])));
    }

    __attribute__((noinline, target("sm4"))) static words runtime(input const &in) {
      return evaluate<architecture>(in);
    }
  };

  struct sm3tt1b1 {
    static constexpr char const *name = "sm3tt1b1";

    static constexpr words expected(input const &in) {
      return reference::tt(in, 1, false, true);
    }

    template <native::isa<native::arm> A>
    __attribute__((target("sm4"))) static constexpr words evaluate(input const &in) {
      return unpack(native::sm3tt1b<A, 1>(pack<A>(in[0]), pack<A>(in[1]), pack<A>(in[2])));
    }

    __attribute__((noinline, target("sm4"))) static words runtime(input const &in) {
      return evaluate<architecture>(in);
    }
  };

  struct sm3tt1b2 {
    static constexpr char const *name = "sm3tt1b2";

    static constexpr words expected(input const &in) {
      return reference::tt(in, 2, false, true);
    }

    template <native::isa<native::arm> A>
    __attribute__((target("sm4"))) static constexpr words evaluate(input const &in) {
      return unpack(native::sm3tt1b<A, 2>(pack<A>(in[0]), pack<A>(in[1]), pack<A>(in[2])));
    }

    __attribute__((noinline, target("sm4"))) static words runtime(input const &in) {
      return evaluate<architecture>(in);
    }
  };

  struct sm3tt1b3 {
    static constexpr char const *name = "sm3tt1b3";

    static constexpr words expected(input const &in) {
      return reference::tt(in, 3, false, true);
    }

    template <native::isa<native::arm> A>
    __attribute__((target("sm4"))) static constexpr words evaluate(input const &in) {
      return unpack(native::sm3tt1b<A, 3>(pack<A>(in[0]), pack<A>(in[1]), pack<A>(in[2])));
    }

    __attribute__((noinline, target("sm4"))) static words runtime(input const &in) {
      return evaluate<architecture>(in);
    }
  };

  struct sm3tt2a0 {
    static constexpr char const *name = "sm3tt2a0";

    static constexpr words expected(input const &in) {
      return reference::tt(in, 0, true, false);
    }

    template <native::isa<native::arm> A>
    __attribute__((target("sm4"))) static constexpr words evaluate(input const &in) {
      return unpack(native::sm3tt2a<A, 0>(pack<A>(in[0]), pack<A>(in[1]), pack<A>(in[2])));
    }

    __attribute__((noinline, target("sm4"))) static words runtime(input const &in) {
      return evaluate<architecture>(in);
    }
  };

  struct sm3tt2a1 {
    static constexpr char const *name = "sm3tt2a1";

    static constexpr words expected(input const &in) {
      return reference::tt(in, 1, true, false);
    }

    template <native::isa<native::arm> A>
    __attribute__((target("sm4"))) static constexpr words evaluate(input const &in) {
      return unpack(native::sm3tt2a<A, 1>(pack<A>(in[0]), pack<A>(in[1]), pack<A>(in[2])));
    }

    __attribute__((noinline, target("sm4"))) static words runtime(input const &in) {
      return evaluate<architecture>(in);
    }
  };

  struct sm3tt2a2 {
    static constexpr char const *name = "sm3tt2a2";

    static constexpr words expected(input const &in) {
      return reference::tt(in, 2, true, false);
    }

    template <native::isa<native::arm> A>
    __attribute__((target("sm4"))) static constexpr words evaluate(input const &in) {
      return unpack(native::sm3tt2a<A, 2>(pack<A>(in[0]), pack<A>(in[1]), pack<A>(in[2])));
    }

    __attribute__((noinline, target("sm4"))) static words runtime(input const &in) {
      return evaluate<architecture>(in);
    }
  };

  struct sm3tt2a3 {
    static constexpr char const *name = "sm3tt2a3";

    static constexpr words expected(input const &in) {
      return reference::tt(in, 3, true, false);
    }

    template <native::isa<native::arm> A>
    __attribute__((target("sm4"))) static constexpr words evaluate(input const &in) {
      return unpack(native::sm3tt2a<A, 3>(pack<A>(in[0]), pack<A>(in[1]), pack<A>(in[2])));
    }

    __attribute__((noinline, target("sm4"))) static words runtime(input const &in) {
      return evaluate<architecture>(in);
    }
  };

  struct sm3tt2b0 {
    static constexpr char const *name = "sm3tt2b0";

    static constexpr words expected(input const &in) {
      return reference::tt(in, 0, true, true);
    }

    template <native::isa<native::arm> A>
    __attribute__((target("sm4"))) static constexpr words evaluate(input const &in) {
      return unpack(native::sm3tt2b<A, 0>(pack<A>(in[0]), pack<A>(in[1]), pack<A>(in[2])));
    }

    __attribute__((noinline, target("sm4"))) static words runtime(input const &in) {
      return evaluate<architecture>(in);
    }
  };

  struct sm3tt2b1 {
    static constexpr char const *name = "sm3tt2b1";

    static constexpr words expected(input const &in) {
      return reference::tt(in, 1, true, true);
    }

    template <native::isa<native::arm> A>
    __attribute__((target("sm4"))) static constexpr words evaluate(input const &in) {
      return unpack(native::sm3tt2b<A, 1>(pack<A>(in[0]), pack<A>(in[1]), pack<A>(in[2])));
    }

    __attribute__((noinline, target("sm4"))) static words runtime(input const &in) {
      return evaluate<architecture>(in);
    }
  };

  struct sm3tt2b2 {
    static constexpr char const *name = "sm3tt2b2";

    static constexpr words expected(input const &in) {
      return reference::tt(in, 2, true, true);
    }

    template <native::isa<native::arm> A>
    __attribute__((target("sm4"))) static constexpr words evaluate(input const &in) {
      return unpack(native::sm3tt2b<A, 2>(pack<A>(in[0]), pack<A>(in[1]), pack<A>(in[2])));
    }

    __attribute__((noinline, target("sm4"))) static words runtime(input const &in) {
      return evaluate<architecture>(in);
    }
  };

  struct sm3tt2b3 {
    static constexpr char const *name = "sm3tt2b3";

    static constexpr words expected(input const &in) {
      return reference::tt(in, 3, true, true);
    }

    template <native::isa<native::arm> A>
    __attribute__((target("sm4"))) static constexpr words evaluate(input const &in) {
      return unpack(native::sm3tt2b<A, 3>(pack<A>(in[0]), pack<A>(in[1]), pack<A>(in[2])));
    }

    __attribute__((noinline, target("sm4"))) static words runtime(input const &in) {
      return evaluate<architecture>(in);
    }
  };

  struct sm3partw1 {
    static constexpr char const *name = "sm3partw1";

    static constexpr words expected(input const &in) {
      return reference::part1(in);
    }

    template <native::isa<native::arm> A>
    __attribute__((target("sm4"))) static constexpr words evaluate(input const &in) {
      return unpack(native::sm3partw1(pack<A>(in[0]), pack<A>(in[1]), pack<A>(in[2])));
    }

    __attribute__((noinline, target("sm4"))) static words runtime(input const &in) {
      return evaluate<architecture>(in);
    }
  };

  struct sm3partw2 {
    static constexpr char const *name = "sm3partw2";

    static constexpr words expected(input const &in) {
      return reference::part2(in);
    }

    template <native::isa<native::arm> A>
    __attribute__((target("sm4"))) static constexpr words evaluate(input const &in) {
      return unpack(native::sm3partw2(pack<A>(in[0]), pack<A>(in[1]), pack<A>(in[2])));
    }

    __attribute__((noinline, target("sm4"))) static words runtime(input const &in) {
      return evaluate<architecture>(in);
    }
  };

  struct sm4e {
    static constexpr char const *name = "sm4e";

    static constexpr words expected(input const &in) {
      return reference::sm4(in, false);
    }

    template <native::isa<native::arm> A>
    __attribute__((target("sm4"))) static constexpr words evaluate(input const &in) {
      return unpack(native::sm4e(pack<A>(in[0]), pack<A>(in[1])));
    }

    __attribute__((noinline, target("sm4"))) static words runtime(input const &in) {
      return evaluate<architecture>(in);
    }
  };

  struct sm4ekey {
    static constexpr char const *name = "sm4ekey";

    static constexpr words expected(input const &in) {
      return reference::sm4(in, true);
    }

    template <native::isa<native::arm> A>
    __attribute__((target("sm4"))) static constexpr words evaluate(input const &in) {
      return unpack(native::sm4ekey(pack<A>(in[0]), pack<A>(in[1])));
    }

    __attribute__((noinline, target("sm4"))) static words runtime(input const &in) {
      return evaluate<architecture>(in);
    }
  };

  struct sample {
    input operands;
    words expected;
  };
  template <class Case>
  constexpr auto cases = [] {
    std::array<sample, 96> result{};
    native_test::property_rng rng{native_test::property_seed};
    for (unsigned index = 0; index < result.size(); ++index) {
      input in{};
      for (unsigned operand = 0; operand < 3; ++operand)
        for (unsigned lane = 0; lane < 4; ++lane) {
          auto x = std::uint32_t(rng.next());
          if (index < 32)
            x = std::uint32_t(1) << ((index + operand * 7 + lane * 3) % 32);
          if (index == 32)
            x = 0;
          if (index == 33)
            x = ~std::uint32_t{};
          in[operand][lane] = x;
        }
      result[index] = {in, Case::expected(in)};
    }
    return result;
  }();

  template <class Case> consteval bool constant_check() {
    for (auto const &entry : cases<Case>) {
      if (Case::template evaluate<architecture>(entry.operands) != entry.expected)
        return false;
      if (Case::template evaluate<native::neon>(entry.operands) != entry.expected)
        return false;
    }
    return true;
  }

  template <class Case> bool runtime_check() {
    unsigned index = 0;
    for (auto const &entry : cases<Case>) {
      if (!native_test::property_equal(Case::name, native_test::property_seed, index++,
                                       entry.expected, Case::runtime(entry.operands),
                                       entry.operands))
        return false;
    }
    return true;
  }

  static_assert(constant_check<sm3ss1>());
  static_assert(constant_check<sm3tt1a0>());
  static_assert(constant_check<sm3tt1a1>());
  static_assert(constant_check<sm3tt1a2>());
  static_assert(constant_check<sm3tt1a3>());
  static_assert(constant_check<sm3tt1b0>());
  static_assert(constant_check<sm3tt1b1>());
  static_assert(constant_check<sm3tt1b2>());
  static_assert(constant_check<sm3tt1b3>());
  static_assert(constant_check<sm3tt2a0>());
  static_assert(constant_check<sm3tt2a1>());
  static_assert(constant_check<sm3tt2a2>());
  static_assert(constant_check<sm3tt2a3>());
  static_assert(constant_check<sm3tt2b0>());
  static_assert(constant_check<sm3tt2b1>());
  static_assert(constant_check<sm3tt2b2>());
  static_assert(constant_check<sm3tt2b3>());
  static_assert(constant_check<sm3partw1>());
  static_assert(constant_check<sm3partw2>());
  static_assert(constant_check<sm4e>());
  static_assert(constant_check<sm4ekey>());
} // namespace fixture

// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

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

#if defined(NATIVE_SM_CONSTANT_TEST)
int main() {}
#else
int main() {
  auto cpu = native::observe_arm_capabilities();
  auto admission = native::classify_isa(cpu, fixture::architecture);
  if (!admission.admitted()) {
    std::printf("SKIP SM3/SM4 runtime: %s\n", admission.reason());
    return 77;
  }
  return fixture::known_answers_runtime() && fixture::runtime_check<fixture::sm3ss1>() &&
                 fixture::runtime_check<fixture::sm3tt1a0>() &&
                 fixture::runtime_check<fixture::sm3tt1a1>() &&
                 fixture::runtime_check<fixture::sm3tt1a2>() &&
                 fixture::runtime_check<fixture::sm3tt1a3>() &&
                 fixture::runtime_check<fixture::sm3tt1b0>() &&
                 fixture::runtime_check<fixture::sm3tt1b1>() &&
                 fixture::runtime_check<fixture::sm3tt1b2>() &&
                 fixture::runtime_check<fixture::sm3tt1b3>() &&
                 fixture::runtime_check<fixture::sm3tt2a0>() &&
                 fixture::runtime_check<fixture::sm3tt2a1>() &&
                 fixture::runtime_check<fixture::sm3tt2a2>() &&
                 fixture::runtime_check<fixture::sm3tt2a3>() &&
                 fixture::runtime_check<fixture::sm3tt2b0>() &&
                 fixture::runtime_check<fixture::sm3tt2b1>() &&
                 fixture::runtime_check<fixture::sm3tt2b2>() &&
                 fixture::runtime_check<fixture::sm3tt2b3>() &&
                 fixture::runtime_check<fixture::sm3partw1>() &&
                 fixture::runtime_check<fixture::sm3partw2>() &&
                 fixture::runtime_check<fixture::sm4e>() &&
                 fixture::runtime_check<fixture::sm4ekey>()
             ? 0
             : 1;
}

#endif
