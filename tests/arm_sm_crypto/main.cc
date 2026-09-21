// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "reference.h"
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

#include "known_answers.h"

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
