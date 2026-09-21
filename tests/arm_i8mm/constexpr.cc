// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cstdint>
import native.arm.i8mm;
#include "../arm_integer_properties.h"

namespace {
  using namespace arm_integer_properties;
  struct case_0 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "smmla_form0_lane-1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::smmla<A>(vector<std::int32_t, 4, A>(in[0]), vector<std::int8_t, 16, A>(in[1]), vector<std::int8_t, 16, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 4, std::int8_t, std::int8_t, true, -1>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::smmla<A>(vector<std::int32_t, 4, A>(in[0]), vector<std::int8_t, 16, A>(in[1]), vector<std::int8_t, 16, A>(in[2]))); }
  };
  struct case_1 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "ummla_form1_lane-1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::ummla<A>(vector<std::uint32_t, 4, A>(in[0]), vector<std::uint8_t, 16, A>(in[1]), vector<std::uint8_t, 16, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::uint32_t, 4, std::uint8_t, std::uint8_t, true, -1>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::ummla<A>(vector<std::uint32_t, 4, A>(in[0]), vector<std::uint8_t, 16, A>(in[1]), vector<std::uint8_t, 16, A>(in[2]))); }
  };
  struct case_2 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "usmmla_form2_lane-1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::usmmla<A>(vector<std::int32_t, 4, A>(in[0]), vector<std::uint8_t, 16, A>(in[1]), vector<std::int8_t, 16, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 4, std::uint8_t, std::int8_t, true, -1>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::usmmla<A>(vector<std::int32_t, 4, A>(in[0]), vector<std::uint8_t, 16, A>(in[1]), vector<std::int8_t, 16, A>(in[2]))); }
  };
  struct case_3 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "usdot_form3_lane-1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::usdot<A>(vector<std::int32_t, 2, A>(in[0]), vector<std::uint8_t, 8, A>(in[1]), vector<std::int8_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 2, std::uint8_t, std::int8_t, false, -1>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::usdot<A>(vector<std::int32_t, 2, A>(in[0]), vector<std::uint8_t, 8, A>(in[1]), vector<std::int8_t, 8, A>(in[2]))); }
  };
  struct case_4 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "usdot_lane_form4_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::usdot_lane<A, 0>(vector<std::int32_t, 2, A>(in[0]), vector<std::uint8_t, 8, A>(in[1]), vector<std::int8_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 2, std::uint8_t, std::int8_t, false, 0>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::usdot_lane<A, 0>(vector<std::int32_t, 2, A>(in[0]), vector<std::uint8_t, 8, A>(in[1]), vector<std::int8_t, 8, A>(in[2]))); }
  };
  struct case_5 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "usdot_lane_form4_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::usdot_lane<A, 1>(vector<std::int32_t, 2, A>(in[0]), vector<std::uint8_t, 8, A>(in[1]), vector<std::int8_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 2, std::uint8_t, std::int8_t, false, 1>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::usdot_lane<A, 1>(vector<std::int32_t, 2, A>(in[0]), vector<std::uint8_t, 8, A>(in[1]), vector<std::int8_t, 8, A>(in[2]))); }
  };
  struct case_6 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "sudot_lane_form5_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sudot_lane<A, 0>(vector<std::int32_t, 2, A>(in[0]), vector<std::int8_t, 8, A>(in[1]), vector<std::uint8_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 2, std::int8_t, std::uint8_t, false, 0>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sudot_lane<A, 0>(vector<std::int32_t, 2, A>(in[0]), vector<std::int8_t, 8, A>(in[1]), vector<std::uint8_t, 8, A>(in[2]))); }
  };
  struct case_7 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "sudot_lane_form5_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sudot_lane<A, 1>(vector<std::int32_t, 2, A>(in[0]), vector<std::int8_t, 8, A>(in[1]), vector<std::uint8_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 2, std::int8_t, std::uint8_t, false, 1>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sudot_lane<A, 1>(vector<std::int32_t, 2, A>(in[0]), vector<std::int8_t, 8, A>(in[1]), vector<std::uint8_t, 8, A>(in[2]))); }
  };
  struct case_8 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "usdot_lane_form6_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::usdot_lane<A, 0>(vector<std::int32_t, 2, A>(in[0]), vector<std::uint8_t, 8, A>(in[1]), vector<std::int8_t, 16, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 2, std::uint8_t, std::int8_t, false, 0>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::usdot_lane<A, 0>(vector<std::int32_t, 2, A>(in[0]), vector<std::uint8_t, 8, A>(in[1]), vector<std::int8_t, 16, A>(in[2]))); }
  };
  struct case_9 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "usdot_lane_form6_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::usdot_lane<A, 1>(vector<std::int32_t, 2, A>(in[0]), vector<std::uint8_t, 8, A>(in[1]), vector<std::int8_t, 16, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 2, std::uint8_t, std::int8_t, false, 1>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::usdot_lane<A, 1>(vector<std::int32_t, 2, A>(in[0]), vector<std::uint8_t, 8, A>(in[1]), vector<std::int8_t, 16, A>(in[2]))); }
  };
  struct case_10 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "usdot_lane_form6_lane2";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::usdot_lane<A, 2>(vector<std::int32_t, 2, A>(in[0]), vector<std::uint8_t, 8, A>(in[1]), vector<std::int8_t, 16, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 2, std::uint8_t, std::int8_t, false, 2>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::usdot_lane<A, 2>(vector<std::int32_t, 2, A>(in[0]), vector<std::uint8_t, 8, A>(in[1]), vector<std::int8_t, 16, A>(in[2]))); }
  };
  struct case_11 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "usdot_lane_form6_lane3";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::usdot_lane<A, 3>(vector<std::int32_t, 2, A>(in[0]), vector<std::uint8_t, 8, A>(in[1]), vector<std::int8_t, 16, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 2, std::uint8_t, std::int8_t, false, 3>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::usdot_lane<A, 3>(vector<std::int32_t, 2, A>(in[0]), vector<std::uint8_t, 8, A>(in[1]), vector<std::int8_t, 16, A>(in[2]))); }
  };
  struct case_12 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "sudot_lane_form7_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sudot_lane<A, 0>(vector<std::int32_t, 2, A>(in[0]), vector<std::int8_t, 8, A>(in[1]), vector<std::uint8_t, 16, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 2, std::int8_t, std::uint8_t, false, 0>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sudot_lane<A, 0>(vector<std::int32_t, 2, A>(in[0]), vector<std::int8_t, 8, A>(in[1]), vector<std::uint8_t, 16, A>(in[2]))); }
  };
  struct case_13 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "sudot_lane_form7_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sudot_lane<A, 1>(vector<std::int32_t, 2, A>(in[0]), vector<std::int8_t, 8, A>(in[1]), vector<std::uint8_t, 16, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 2, std::int8_t, std::uint8_t, false, 1>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sudot_lane<A, 1>(vector<std::int32_t, 2, A>(in[0]), vector<std::int8_t, 8, A>(in[1]), vector<std::uint8_t, 16, A>(in[2]))); }
  };
  struct case_14 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "sudot_lane_form7_lane2";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sudot_lane<A, 2>(vector<std::int32_t, 2, A>(in[0]), vector<std::int8_t, 8, A>(in[1]), vector<std::uint8_t, 16, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 2, std::int8_t, std::uint8_t, false, 2>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sudot_lane<A, 2>(vector<std::int32_t, 2, A>(in[0]), vector<std::int8_t, 8, A>(in[1]), vector<std::uint8_t, 16, A>(in[2]))); }
  };
  struct case_15 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "sudot_lane_form7_lane3";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sudot_lane<A, 3>(vector<std::int32_t, 2, A>(in[0]), vector<std::int8_t, 8, A>(in[1]), vector<std::uint8_t, 16, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 2, std::int8_t, std::uint8_t, false, 3>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sudot_lane<A, 3>(vector<std::int32_t, 2, A>(in[0]), vector<std::int8_t, 8, A>(in[1]), vector<std::uint8_t, 16, A>(in[2]))); }
  };
  struct case_16 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "usdot_form8_lane-1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::usdot<A>(vector<std::int32_t, 4, A>(in[0]), vector<std::uint8_t, 16, A>(in[1]), vector<std::int8_t, 16, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 4, std::uint8_t, std::int8_t, false, -1>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::usdot<A>(vector<std::int32_t, 4, A>(in[0]), vector<std::uint8_t, 16, A>(in[1]), vector<std::int8_t, 16, A>(in[2]))); }
  };
  struct case_17 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "usdot_lane_form9_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::usdot_lane<A, 0>(vector<std::int32_t, 4, A>(in[0]), vector<std::uint8_t, 16, A>(in[1]), vector<std::int8_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 4, std::uint8_t, std::int8_t, false, 0>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::usdot_lane<A, 0>(vector<std::int32_t, 4, A>(in[0]), vector<std::uint8_t, 16, A>(in[1]), vector<std::int8_t, 8, A>(in[2]))); }
  };
  struct case_18 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "usdot_lane_form9_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::usdot_lane<A, 1>(vector<std::int32_t, 4, A>(in[0]), vector<std::uint8_t, 16, A>(in[1]), vector<std::int8_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 4, std::uint8_t, std::int8_t, false, 1>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::usdot_lane<A, 1>(vector<std::int32_t, 4, A>(in[0]), vector<std::uint8_t, 16, A>(in[1]), vector<std::int8_t, 8, A>(in[2]))); }
  };
  struct case_19 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "sudot_lane_form10_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sudot_lane<A, 0>(vector<std::int32_t, 4, A>(in[0]), vector<std::int8_t, 16, A>(in[1]), vector<std::uint8_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 4, std::int8_t, std::uint8_t, false, 0>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sudot_lane<A, 0>(vector<std::int32_t, 4, A>(in[0]), vector<std::int8_t, 16, A>(in[1]), vector<std::uint8_t, 8, A>(in[2]))); }
  };
  struct case_20 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "sudot_lane_form10_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sudot_lane<A, 1>(vector<std::int32_t, 4, A>(in[0]), vector<std::int8_t, 16, A>(in[1]), vector<std::uint8_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 4, std::int8_t, std::uint8_t, false, 1>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sudot_lane<A, 1>(vector<std::int32_t, 4, A>(in[0]), vector<std::int8_t, 16, A>(in[1]), vector<std::uint8_t, 8, A>(in[2]))); }
  };
  struct case_21 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "usdot_lane_form11_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::usdot_lane<A, 0>(vector<std::int32_t, 4, A>(in[0]), vector<std::uint8_t, 16, A>(in[1]), vector<std::int8_t, 16, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 4, std::uint8_t, std::int8_t, false, 0>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::usdot_lane<A, 0>(vector<std::int32_t, 4, A>(in[0]), vector<std::uint8_t, 16, A>(in[1]), vector<std::int8_t, 16, A>(in[2]))); }
  };
  struct case_22 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "usdot_lane_form11_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::usdot_lane<A, 1>(vector<std::int32_t, 4, A>(in[0]), vector<std::uint8_t, 16, A>(in[1]), vector<std::int8_t, 16, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 4, std::uint8_t, std::int8_t, false, 1>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::usdot_lane<A, 1>(vector<std::int32_t, 4, A>(in[0]), vector<std::uint8_t, 16, A>(in[1]), vector<std::int8_t, 16, A>(in[2]))); }
  };
  struct case_23 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "usdot_lane_form11_lane2";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::usdot_lane<A, 2>(vector<std::int32_t, 4, A>(in[0]), vector<std::uint8_t, 16, A>(in[1]), vector<std::int8_t, 16, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 4, std::uint8_t, std::int8_t, false, 2>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::usdot_lane<A, 2>(vector<std::int32_t, 4, A>(in[0]), vector<std::uint8_t, 16, A>(in[1]), vector<std::int8_t, 16, A>(in[2]))); }
  };
  struct case_24 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "usdot_lane_form11_lane3";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::usdot_lane<A, 3>(vector<std::int32_t, 4, A>(in[0]), vector<std::uint8_t, 16, A>(in[1]), vector<std::int8_t, 16, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 4, std::uint8_t, std::int8_t, false, 3>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::usdot_lane<A, 3>(vector<std::int32_t, 4, A>(in[0]), vector<std::uint8_t, 16, A>(in[1]), vector<std::int8_t, 16, A>(in[2]))); }
  };
  struct case_25 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "sudot_lane_form12_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sudot_lane<A, 0>(vector<std::int32_t, 4, A>(in[0]), vector<std::int8_t, 16, A>(in[1]), vector<std::uint8_t, 16, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 4, std::int8_t, std::uint8_t, false, 0>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sudot_lane<A, 0>(vector<std::int32_t, 4, A>(in[0]), vector<std::int8_t, 16, A>(in[1]), vector<std::uint8_t, 16, A>(in[2]))); }
  };
  struct case_26 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "sudot_lane_form12_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sudot_lane<A, 1>(vector<std::int32_t, 4, A>(in[0]), vector<std::int8_t, 16, A>(in[1]), vector<std::uint8_t, 16, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 4, std::int8_t, std::uint8_t, false, 1>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sudot_lane<A, 1>(vector<std::int32_t, 4, A>(in[0]), vector<std::int8_t, 16, A>(in[1]), vector<std::uint8_t, 16, A>(in[2]))); }
  };
  struct case_27 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "sudot_lane_form12_lane2";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sudot_lane<A, 2>(vector<std::int32_t, 4, A>(in[0]), vector<std::int8_t, 16, A>(in[1]), vector<std::uint8_t, 16, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 4, std::int8_t, std::uint8_t, false, 2>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sudot_lane<A, 2>(vector<std::int32_t, 4, A>(in[0]), vector<std::int8_t, 16, A>(in[1]), vector<std::uint8_t, 16, A>(in[2]))); }
  };
  struct case_28 {
    using edge_type = void;
    static constexpr auto arch = native::feature_closure(native::arm_feature::i8mm);
    static constexpr char name[] = "sudot_lane_form12_lane3";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sudot_lane<A, 3>(vector<std::int32_t, 4, A>(in[0]), vector<std::int8_t, 16, A>(in[1]), vector<std::uint8_t, 16, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return dot_reference<std::int32_t, 4, std::int8_t, std::uint8_t, false, 3>(in); }
    __attribute__((target("i8mm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sudot_lane<A, 3>(vector<std::int32_t, 4, A>(in[0]), vector<std::int8_t, 16, A>(in[1]), vector<std::uint8_t, 16, A>(in[2]))); }
  };
}
int main() {
  auto cpu = native::observe_arm_capabilities();
  auto admission = native::classify_isa(cpu, native::target_features<native::arm>("i8mm"));
  if (admission.invalid_features) return 1;
  if (!admission.admitted()) return 77;
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
  return ok ? 0 : 1;
}
