// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "../property_check.h"
#include <array>
#include <bit>
#include <cstdint>
#include <type_traits>
import native.arm.aes;
import native.arm.pmull;
import native.arm.sha;

namespace {
  using input = std::array<std::array<std::uint64_t, 2>, 3>;
  using output = std::array<std::uint64_t, 2>;
  template<class T, unsigned N, native::isa<native::arm> A>
  constexpr auto vector(std::array<std::uint64_t, 2> bits) {
    using V = native::simd<T, N, A>;
    auto words = std::bit_cast<std::array<T, 16 / sizeof(T)>>(bits);
    std::array<T, sizeof(typename V::native_type) / sizeof(T)> storage{};
    for (unsigned i = 0; i < N; ++i) storage[i] = words[i];
    return V::from_native(std::bit_cast<typename V::native_type>(storage));
  }
  template<class V> constexpr output result(V value) {
    static_assert(sizeof(typename V::native_type) == 16);
    return std::bit_cast<output>(value.to_native());
  }
  struct sample { input operands; output expected; bool same_weak; };
  template<class Case> constexpr auto cases = native_test::property_cases<24>(native_test::property_seed,
    [](native_test::property_rng &rng) {
      input operands{};
      for (auto &v : operands) for (auto &word : v) word = rng.next();
      auto expected = Case::template evaluate<Case::arch>(operands);
      auto weak = Case::template evaluate<native::neon>(operands);
      return sample{operands, expected, expected == weak};
    });
  template<class Case> consteval bool constant_checks() {
    for (auto const &entry : cases<Case>) if (!entry.same_weak) return false;
    return true;
  }
  // Separate constant initializers keep each AES case within the compiler's
  // evaluation budget while covering all basis bits without raising that limit.
  template<class Case, unsigned Bit> constexpr bool basis_case = [] consteval {
    input operands{};
    for (auto &v : operands) for (auto &word : v)
      word = Bit == 64 ? 0 : Bit == 65 ? ~std::uint64_t{} : std::uint64_t{1} << Bit;
    return Case::template evaluate<Case::arch>(operands) == Case::template evaluate<native::neon>(operands);
  }();
  template<class Case> bool check() {
    static_assert(constant_checks<Case>());
    static_assert([]<unsigned... Bit>(std::integer_sequence<unsigned, Bit...>) {
      return (basis_case<Case, Bit> && ...);
    }(std::make_integer_sequence<unsigned, 66>{}));
    for (unsigned index = 0; index < cases<Case>.size(); ++index) {
      auto const &entry = cases<Case>[index];
      if (!native_test::property_equal(Case::name, native_test::property_seed, index,
          entry.expected, Case::runtime(entry.operands), entry.operands)) return false;
    }
    return true;
  }
  struct case_0 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::aes);
    static constexpr char name[] = "aese_uint8_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::aese<A>(vector<std::uint8_t, 16, A>(in[0]), vector<std::uint8_t, 16, A>(in[1])));
    }
    __attribute__((target("aes"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_1 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::aes);
    static constexpr char name[] = "aesd_uint8_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::aesd<A>(vector<std::uint8_t, 16, A>(in[0]), vector<std::uint8_t, 16, A>(in[1])));
    }
    __attribute__((target("aes"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_2 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::aes);
    static constexpr char name[] = "aesmc_uint8_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::aesmc<A>(vector<std::uint8_t, 16, A>(in[0])));
    }
    __attribute__((target("aes"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_3 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::aes);
    static constexpr char name[] = "aesimc_uint8_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::aesimc<A>(vector<std::uint8_t, 16, A>(in[0])));
    }
    __attribute__((target("aes"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_4 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::pmull);
    static constexpr char name[] = "pmull_uint64_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::pmull<A>(static_cast<std::uint64_t>(in[0][0]), static_cast<std::uint64_t>(in[1][0])));
    }
    __attribute__((target("aes"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_5 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::pmull);
    static constexpr char name[] = "pmull2_uint64_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::pmull2<A>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("aes"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_6 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::neon);
    static constexpr char name[] = "pmull_uint16_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::pmull<A>(vector<std::uint8_t, 8, A>(in[0]), vector<std::uint8_t, 8, A>(in[1])));
    }
    __attribute__((target("neon"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_7 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::neon);
    static constexpr char name[] = "pmull2_uint16_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::pmull2<A>(vector<std::uint8_t, 16, A>(in[0]), vector<std::uint8_t, 16, A>(in[1])));
    }
    __attribute__((target("neon"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_8 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha1);
    static constexpr char name[] = "sha1c_uint32_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::sha1c<A>(vector<std::uint32_t, 4, A>(in[0]), static_cast<std::uint32_t>(in[1][0]), vector<std::uint32_t, 4, A>(in[2])));
    }
    __attribute__((target("sha2"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_9 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha1);
    static constexpr char name[] = "sha1p_uint32_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::sha1p<A>(vector<std::uint32_t, 4, A>(in[0]), static_cast<std::uint32_t>(in[1][0]), vector<std::uint32_t, 4, A>(in[2])));
    }
    __attribute__((target("sha2"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_10 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha1);
    static constexpr char name[] = "sha1m_uint32_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::sha1m<A>(vector<std::uint32_t, 4, A>(in[0]), static_cast<std::uint32_t>(in[1][0]), vector<std::uint32_t, 4, A>(in[2])));
    }
    __attribute__((target("sha2"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_11 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha1);
    static constexpr char name[] = "sha1su0_uint32_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::sha1su0<A>(vector<std::uint32_t, 4, A>(in[0]), vector<std::uint32_t, 4, A>(in[1]), vector<std::uint32_t, 4, A>(in[2])));
    }
    __attribute__((target("sha2"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_12 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha1);
    static constexpr char name[] = "sha1su1_uint32_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::sha1su1<A>(vector<std::uint32_t, 4, A>(in[0]), vector<std::uint32_t, 4, A>(in[1])));
    }
    __attribute__((target("sha2"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_13 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha2);
    static constexpr char name[] = "sha256h_uint32_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::sha256h<A>(vector<std::uint32_t, 4, A>(in[0]), vector<std::uint32_t, 4, A>(in[1]), vector<std::uint32_t, 4, A>(in[2])));
    }
    __attribute__((target("sha2"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_14 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha2);
    static constexpr char name[] = "sha256h2_uint32_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::sha256h2<A>(vector<std::uint32_t, 4, A>(in[0]), vector<std::uint32_t, 4, A>(in[1]), vector<std::uint32_t, 4, A>(in[2])));
    }
    __attribute__((target("sha2"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_15 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha2);
    static constexpr char name[] = "sha256su0_uint32_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::sha256su0<A>(vector<std::uint32_t, 4, A>(in[0]), vector<std::uint32_t, 4, A>(in[1])));
    }
    __attribute__((target("sha2"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_16 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha2);
    static constexpr char name[] = "sha256su1_uint32_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::sha256su1<A>(vector<std::uint32_t, 4, A>(in[0]), vector<std::uint32_t, 4, A>(in[1]), vector<std::uint32_t, 4, A>(in[2])));
    }
    __attribute__((target("sha2"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_17 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha512);
    static constexpr char name[] = "sha512h_uint64_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::sha512h<A>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1]), vector<std::uint64_t, 2, A>(in[2])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_18 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha512);
    static constexpr char name[] = "sha512h2_uint64_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::sha512h2<A>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1]), vector<std::uint64_t, 2, A>(in[2])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_19 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha512);
    static constexpr char name[] = "sha512su0_uint64_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::sha512su0<A>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_20 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha512);
    static constexpr char name[] = "sha512su1_uint64_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::sha512su1<A>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1]), vector<std::uint64_t, 2, A>(in[2])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_21 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "eor3_uint8_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::eor3<A>(vector<std::uint8_t, 16, A>(in[0]), vector<std::uint8_t, 16, A>(in[1]), vector<std::uint8_t, 16, A>(in[2])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_22 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "eor3_uint16_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::eor3<A>(vector<std::uint16_t, 8, A>(in[0]), vector<std::uint16_t, 8, A>(in[1]), vector<std::uint16_t, 8, A>(in[2])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_23 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "eor3_uint32_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::eor3<A>(vector<std::uint32_t, 4, A>(in[0]), vector<std::uint32_t, 4, A>(in[1]), vector<std::uint32_t, 4, A>(in[2])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_24 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "eor3_uint64_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::eor3<A>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1]), vector<std::uint64_t, 2, A>(in[2])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_25 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "eor3_int8_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::eor3<A>(vector<std::int8_t, 16, A>(in[0]), vector<std::int8_t, 16, A>(in[1]), vector<std::int8_t, 16, A>(in[2])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_26 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "eor3_int16_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::eor3<A>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_27 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "eor3_int32_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::eor3<A>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_28 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "eor3_int64_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::eor3<A>(vector<std::int64_t, 2, A>(in[0]), vector<std::int64_t, 2, A>(in[1]), vector<std::int64_t, 2, A>(in[2])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_29 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "bcax_uint8_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::bcax<A>(vector<std::uint8_t, 16, A>(in[0]), vector<std::uint8_t, 16, A>(in[1]), vector<std::uint8_t, 16, A>(in[2])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_30 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "bcax_uint16_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::bcax<A>(vector<std::uint16_t, 8, A>(in[0]), vector<std::uint16_t, 8, A>(in[1]), vector<std::uint16_t, 8, A>(in[2])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_31 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "bcax_uint32_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::bcax<A>(vector<std::uint32_t, 4, A>(in[0]), vector<std::uint32_t, 4, A>(in[1]), vector<std::uint32_t, 4, A>(in[2])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_32 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "bcax_uint64_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::bcax<A>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1]), vector<std::uint64_t, 2, A>(in[2])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_33 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "bcax_int8_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::bcax<A>(vector<std::int8_t, 16, A>(in[0]), vector<std::int8_t, 16, A>(in[1]), vector<std::int8_t, 16, A>(in[2])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_34 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "bcax_int16_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::bcax<A>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_35 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "bcax_int32_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::bcax<A>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_36 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "bcax_int64_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::bcax<A>(vector<std::int64_t, 2, A>(in[0]), vector<std::int64_t, 2, A>(in[1]), vector<std::int64_t, 2, A>(in[2])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_37 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "rax1_uint64_t";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::rax1<A>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_38 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_0";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 0>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_39 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_1";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 1>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_40 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_2";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 2>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_41 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_3";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 3>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_42 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_4";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 4>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_43 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_5";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 5>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_44 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_6";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 6>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_45 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_7";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 7>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_46 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_8";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 8>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_47 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_9";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 9>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_48 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_10";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 10>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_49 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_11";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 11>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_50 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_12";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 12>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_51 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_13";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 13>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_52 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_14";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 14>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_53 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_15";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 15>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_54 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_16";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 16>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_55 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_17";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 17>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_56 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_18";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 18>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_57 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_19";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 19>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_58 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_20";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 20>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_59 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_21";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 21>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_60 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_22";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 22>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_61 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_23";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 23>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_62 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_24";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 24>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_63 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_25";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 25>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_64 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_26";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 26>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_65 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_27";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 27>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_66 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_28";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 28>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_67 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_29";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 29>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_68 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_30";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 30>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_69 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_31";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 31>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_70 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_32";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 32>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_71 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_33";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 33>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_72 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_34";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 34>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_73 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_35";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 35>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_74 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_36";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 36>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_75 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_37";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 37>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_76 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_38";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 38>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_77 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_39";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 39>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_78 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_40";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 40>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_79 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_41";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 41>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_80 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_42";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 42>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_81 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_43";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 43>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_82 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_44";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 44>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_83 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_45";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 45>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_84 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_46";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 46>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_85 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_47";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 47>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_86 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_48";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 48>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_87 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_49";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 49>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_88 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_50";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 50>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_89 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_51";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 51>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_90 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_52";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 52>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_91 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_53";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 53>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_92 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_54";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 54>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_93 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_55";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 55>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_94 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_56";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 56>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_95 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_57";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 57>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_96 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_58";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 58>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_97 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_59";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 59>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_98 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_60";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 60>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_99 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_61";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 61>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_100 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_62";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 62>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  struct case_101 {
    static constexpr auto arch = native::feature_closure(native::arm_feature::sha3);
    static constexpr char name[] = "xar_uint64_t_63";
    template<native::isa<native::arm> A> static constexpr output evaluate(input const &in) {
      return result(native::xar<A, 63>(vector<std::uint64_t, 2, A>(in[0]), vector<std::uint64_t, 2, A>(in[1])));
    }
    __attribute__((target("sha3"), noinline)) static output runtime(input const &in) { return evaluate<arch>(in); }
  };
  consteval bool aes_identities() {
    using V = native::simd<std::uint8_t, 16, native::neon>;
    constexpr std::array<std::uint8_t, 16> fips_input{0xdb,0x13,0x53,0x45,0xf2,0x0a,0x22,0x5c,0x01,0x01,0x01,0x01,0xc6,0xc6,0xc6,0xc6};
    constexpr std::array<std::uint8_t, 16> fips_output{0x8e,0x4d,0xa1,0xbc,0x9f,0xdc,0x58,0x9d,0x01,0x01,0x01,0x01,0xc6,0xc6,0xc6,0xc6};
    if (std::bit_cast<decltype(fips_output)>(native::aesmc(V(fips_input)).to_native()) != fips_output) return false;
    for (unsigned first = 0; first < 256; first += 16) {
      std::array<std::uint8_t, 16> a{};
      for (unsigned i = 0; i < 16; ++i) a[i] = first + i;
      V state(a), zero(std::uint8_t{});
      if (result(native::aesd(native::aese(state, zero), zero)) != result(state)) return false;
      if (result(native::aesimc(native::aesmc(state))) != result(state)) return false;
    }
    return true;
  }
  static_assert(aes_identities());
  // FIPS 180-4, the single padded block for "abc". These fixed digests are
  // independent of the instruction-level semantic implementation.
  consteval bool hash_known_answers() {
    using V = native::simd<std::uint32_t, 4, native::neon>;
    constexpr std::array<std::uint32_t, 64> k{
      0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
      0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
      0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
      0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
      0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
      0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
      0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
      0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};
    std::array<V, 20> w{};
    w[0] = V(0x61626380u,0u,0u,0u); w[3] = V(0u,0u,0u,24u);
    for (unsigned i = 4; i < 16; ++i)
      w[i] = native::sha256su1(native::sha256su0(w[i-4],w[i-3]),w[i-2],w[i-1]);
    constexpr std::array<std::uint32_t, 8> initial{
      0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};
    V a(initial[0],initial[1],initial[2],initial[3]), e(initial[4],initial[5],initial[6],initial[7]);
    for (unsigned i = 0; i < 16; ++i) {
      auto words = std::bit_cast<std::array<std::uint32_t, 4>>(w[i].to_native());
      for (unsigned lane = 0; lane < 4; ++lane) words[lane] += k[4*i+lane];
      auto old = a;
      a = native::sha256h(a,e,V(words)); e = native::sha256h2(e,old,V(words));
    }
    auto ah = std::bit_cast<std::array<std::uint32_t, 4>>(a.to_native());
    auto eh = std::bit_cast<std::array<std::uint32_t, 4>>(e.to_native());
    constexpr std::array<std::uint32_t,8> digest{
      0xba7816bf,0x8f01cfea,0x414140de,0x5dae2223,0xb00361a3,0x96177a9c,0xb410ff61,0xf20015ad};
    for (unsigned i = 0; i < 4; ++i)
      if (ah[i]+initial[i]!=digest[i] || eh[i]+initial[i+4]!=digest[i+4]) return false;
    for (unsigned i = 4; i < 20; ++i)
      w[i] = native::sha1su1(native::sha1su0(w[i-4],w[i-3],w[i-2]),w[i-1]);
    constexpr std::array<std::uint32_t,5> initial1{0x67452301,0xefcdab89,0x98badcfe,0x10325476,0xc3d2e1f0};
    a = V(initial1[0],initial1[1],initial1[2],initial1[3]);
    auto last = initial1[4];
    for (unsigned i = 0; i < 20; ++i) {
      auto words = std::bit_cast<std::array<std::uint32_t,4>>(w[i].to_native());
      std::uint32_t key = i < 5 ? 0x5a827999 : i < 10 ? 0x6ed9eba1 : i < 15 ? 0x8f1bbcdc : 0xca62c1d6;
      for (auto &word : words) word += key;
      auto old = std::bit_cast<std::array<std::uint32_t,4>>(a.to_native())[0];
      if (i < 5) a = native::sha1c(a,last,V(words));
      else if (i < 10 || i >= 15) a = native::sha1p(a,last,V(words));
      else a = native::sha1m(a,last,V(words));
      last = native::sha1h<native::neon>(old);
    }
    ah = std::bit_cast<std::array<std::uint32_t,4>>(a.to_native());
    constexpr std::array<std::uint32_t,5> digest1{0xa9993e36,0x4706816a,0xba3e2571,0x7850c26c,0x9cd0d89d};
    for (unsigned i=0;i<4;++i) if (ah[i]+initial1[i]!=digest1[i]) return false;
    return last+initial1[4]==digest1[4];
  }
  static_assert(hash_known_answers());

}
int main() {
  auto cpu = native::observe_arm_capabilities();
  for (auto target : {"aes", "sha2", "sha3"})
    if (!native::classify_isa(cpu, native::target_features<native::arm>(target)).admitted()) return 77;
  bool ok = true;
  ok = check<case_0>() && ok;
  ok = check<case_1>() && ok;
  ok = check<case_2>() && ok;
  ok = check<case_3>() && ok;
  ok = check<case_4>() && ok;
  ok = check<case_5>() && ok;
  ok = check<case_6>() && ok;
  ok = check<case_7>() && ok;
  ok = check<case_8>() && ok;
  ok = check<case_9>() && ok;
  ok = check<case_10>() && ok;
  ok = check<case_11>() && ok;
  ok = check<case_12>() && ok;
  ok = check<case_13>() && ok;
  ok = check<case_14>() && ok;
  ok = check<case_15>() && ok;
  ok = check<case_16>() && ok;
  ok = check<case_17>() && ok;
  ok = check<case_18>() && ok;
  ok = check<case_19>() && ok;
  ok = check<case_20>() && ok;
  ok = check<case_21>() && ok;
  ok = check<case_22>() && ok;
  ok = check<case_23>() && ok;
  ok = check<case_24>() && ok;
  ok = check<case_25>() && ok;
  ok = check<case_26>() && ok;
  ok = check<case_27>() && ok;
  ok = check<case_28>() && ok;
  ok = check<case_29>() && ok;
  ok = check<case_30>() && ok;
  ok = check<case_31>() && ok;
  ok = check<case_32>() && ok;
  ok = check<case_33>() && ok;
  ok = check<case_34>() && ok;
  ok = check<case_35>() && ok;
  ok = check<case_36>() && ok;
  ok = check<case_37>() && ok;
  ok = check<case_38>() && ok;
  ok = check<case_39>() && ok;
  ok = check<case_40>() && ok;
  ok = check<case_41>() && ok;
  ok = check<case_42>() && ok;
  ok = check<case_43>() && ok;
  ok = check<case_44>() && ok;
  ok = check<case_45>() && ok;
  ok = check<case_46>() && ok;
  ok = check<case_47>() && ok;
  ok = check<case_48>() && ok;
  ok = check<case_49>() && ok;
  ok = check<case_50>() && ok;
  ok = check<case_51>() && ok;
  ok = check<case_52>() && ok;
  ok = check<case_53>() && ok;
  ok = check<case_54>() && ok;
  ok = check<case_55>() && ok;
  ok = check<case_56>() && ok;
  ok = check<case_57>() && ok;
  ok = check<case_58>() && ok;
  ok = check<case_59>() && ok;
  ok = check<case_60>() && ok;
  ok = check<case_61>() && ok;
  ok = check<case_62>() && ok;
  ok = check<case_63>() && ok;
  ok = check<case_64>() && ok;
  ok = check<case_65>() && ok;
  ok = check<case_66>() && ok;
  ok = check<case_67>() && ok;
  ok = check<case_68>() && ok;
  ok = check<case_69>() && ok;
  ok = check<case_70>() && ok;
  ok = check<case_71>() && ok;
  ok = check<case_72>() && ok;
  ok = check<case_73>() && ok;
  ok = check<case_74>() && ok;
  ok = check<case_75>() && ok;
  ok = check<case_76>() && ok;
  ok = check<case_77>() && ok;
  ok = check<case_78>() && ok;
  ok = check<case_79>() && ok;
  ok = check<case_80>() && ok;
  ok = check<case_81>() && ok;
  ok = check<case_82>() && ok;
  ok = check<case_83>() && ok;
  ok = check<case_84>() && ok;
  ok = check<case_85>() && ok;
  ok = check<case_86>() && ok;
  ok = check<case_87>() && ok;
  ok = check<case_88>() && ok;
  ok = check<case_89>() && ok;
  ok = check<case_90>() && ok;
  ok = check<case_91>() && ok;
  ok = check<case_92>() && ok;
  ok = check<case_93>() && ok;
  ok = check<case_94>() && ok;
  ok = check<case_95>() && ok;
  ok = check<case_96>() && ok;
  ok = check<case_97>() && ok;
  ok = check<case_98>() && ok;
  ok = check<case_99>() && ok;
  ok = check<case_100>() && ok;
  ok = check<case_101>() && ok;
  return ok ? 0 : 1;
}
