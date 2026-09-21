// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cstdint>
import native.arm.rdm;
#include "../arm_integer_properties.h"

static_assert(native::sqrdmlah<native::isa<native::arm>{}>(std::int16_t{-32768}, std::int16_t{-32768}, std::int16_t{-32768}) == 0);
static_assert(native::sqrdmlsh<native::isa<native::arm>{}>(std::int32_t{0}, std::int32_t{1}, std::int32_t{1073741824}) == 0);
static_assert(native::sqrdmlah<native::isa<native::arm>{}>(std::int32_t{0}, std::int32_t{1}, std::int32_t{1073741824}) == 1);

namespace {
  using namespace arm_integer_properties;
  struct case_0 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_form0_lane-1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah<A>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), scalar<int16_t>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, false, -1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah<A>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), scalar<int16_t>(in[2]))); }
  };
  struct case_1 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form1_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 0>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, false, 0>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 0>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_2 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form1_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 1>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, false, 1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 1>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_3 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form1_lane2";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 2>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, false, 2>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 2>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_4 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form1_lane3";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 3>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, false, 3>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 3>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_5 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form2_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 0>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, false, 0>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 0>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_6 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form2_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 1>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, false, 1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 1>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_7 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form2_lane2";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 2>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, false, 2>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 2>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_8 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form2_lane3";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 3>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, false, 3>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 3>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_9 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form2_lane4";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 4>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, false, 4>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 4>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_10 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form2_lane5";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 5>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, false, 5>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 5>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_11 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form2_lane6";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 6>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, false, 6>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 6>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_12 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form2_lane7";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 7>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, false, 7>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 7>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_13 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_form3_lane-1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah<A>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, false, -1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah<A>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_14 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form4_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 0>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, false, 0>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 0>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_15 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form4_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 1>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, false, 1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 1>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_16 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form4_lane2";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 2>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, false, 2>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 2>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_17 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form4_lane3";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 3>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, false, 3>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 3>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_18 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form5_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 0>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, false, 0>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 0>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_19 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form5_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 1>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, false, 1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 1>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_20 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form5_lane2";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 2>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, false, 2>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 2>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_21 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form5_lane3";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 3>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, false, 3>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 3>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_22 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form5_lane4";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 4>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, false, 4>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 4>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_23 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form5_lane5";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 5>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, false, 5>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 5>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_24 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form5_lane6";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 6>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, false, 6>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 6>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_25 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form5_lane7";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 7>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, false, 7>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 7>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_26 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_form6_lane-1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah<A>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, false, -1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah<A>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_27 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form7_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 0>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, false, 0>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 0>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_28 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form7_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 1>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, false, 1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 1>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_29 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form7_lane2";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 2>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, false, 2>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 2>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_30 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form7_lane3";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 3>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, false, 3>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 3>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_31 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form8_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 0>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, false, 0>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 0>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_32 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form8_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 1>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, false, 1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 1>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_33 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form8_lane2";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 2>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, false, 2>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 2>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_34 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form8_lane3";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 3>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, false, 3>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 3>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_35 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form8_lane4";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 4>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, false, 4>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 4>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_36 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form8_lane5";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 5>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, false, 5>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 5>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_37 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form8_lane6";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 6>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, false, 6>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 6>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_38 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form8_lane7";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 7>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, false, 7>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 7>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_39 {
    using edge_type = int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_form9_lane-1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah<A>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), scalar<int32_t>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int32_t, 1, false, -1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah<A>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), scalar<int32_t>(in[2]))); }
  };
  struct case_40 {
    using edge_type = int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form10_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 0>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), vector<std::int32_t, 2, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int32_t, 1, false, 0>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 0>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), vector<std::int32_t, 2, A>(in[2]))); }
  };
  struct case_41 {
    using edge_type = int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form10_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 1>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), vector<std::int32_t, 2, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int32_t, 1, false, 1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 1>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), vector<std::int32_t, 2, A>(in[2]))); }
  };
  struct case_42 {
    using edge_type = int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form11_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 0>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int32_t, 1, false, 0>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 0>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_43 {
    using edge_type = int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form11_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 1>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int32_t, 1, false, 1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 1>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_44 {
    using edge_type = int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form11_lane2";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 2>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int32_t, 1, false, 2>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 2>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_45 {
    using edge_type = int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form11_lane3";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 3>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int32_t, 1, false, 3>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 3>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_46 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_form12_lane-1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah<A>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 2, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 2, false, -1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah<A>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 2, A>(in[2]))); }
  };
  struct case_47 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form13_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 0>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 2, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 2, false, 0>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 0>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 2, A>(in[2]))); }
  };
  struct case_48 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form13_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 1>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 2, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 2, false, 1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 1>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 2, A>(in[2]))); }
  };
  struct case_49 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form14_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 0>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 2, false, 0>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 0>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_50 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form14_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 1>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 2, false, 1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 1>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_51 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form14_lane2";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 2>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 2, false, 2>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 2>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_52 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form14_lane3";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 3>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 2, false, 3>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 3>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_53 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_form15_lane-1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah<A>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 4, false, -1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah<A>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_54 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form16_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 0>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 2, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 4, false, 0>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 0>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 2, A>(in[2]))); }
  };
  struct case_55 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form16_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 1>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 2, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 4, false, 1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 1>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 2, A>(in[2]))); }
  };
  struct case_56 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form17_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 0>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 4, false, 0>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 0>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_57 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form17_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 1>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 4, false, 1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 1>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_58 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form17_lane2";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 2>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 4, false, 2>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 2>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_59 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlah_lane_form17_lane3";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlah_lane<A, 3>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 4, false, 3>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlah_lane<A, 3>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_60 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_form18_lane-1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh<A>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), scalar<int16_t>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, true, -1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh<A>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), scalar<int16_t>(in[2]))); }
  };
  struct case_61 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form19_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 0>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, true, 0>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 0>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_62 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form19_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 1>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, true, 1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 1>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_63 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form19_lane2";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 2>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, true, 2>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 2>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_64 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form19_lane3";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 3>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, true, 3>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 3>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_65 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form20_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 0>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, true, 0>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 0>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_66 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form20_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 1>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, true, 1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 1>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_67 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form20_lane2";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 2>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, true, 2>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 2>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_68 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form20_lane3";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 3>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, true, 3>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 3>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_69 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form20_lane4";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 4>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, true, 4>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 4>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_70 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form20_lane5";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 5>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, true, 5>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 5>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_71 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form20_lane6";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 6>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, true, 6>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 6>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_72 {
    using edge_type = int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form20_lane7";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 7>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int16_t, 1, true, 7>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 7>(scalar<int16_t>(in[0]), scalar<int16_t>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_73 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_form21_lane-1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh<A>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, true, -1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh<A>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_74 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form22_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 0>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, true, 0>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 0>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_75 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form22_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 1>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, true, 1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 1>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_76 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form22_lane2";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 2>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, true, 2>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 2>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_77 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form22_lane3";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 3>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, true, 3>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 3>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_78 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form23_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 0>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, true, 0>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 0>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_79 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form23_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 1>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, true, 1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 1>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_80 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form23_lane2";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 2>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, true, 2>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 2>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_81 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form23_lane3";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 3>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, true, 3>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 3>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_82 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form23_lane4";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 4>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, true, 4>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 4>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_83 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form23_lane5";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 5>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, true, 5>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 5>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_84 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form23_lane6";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 6>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, true, 6>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 6>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_85 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form23_lane7";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 7>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 4, true, 7>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 7>(vector<std::int16_t, 4, A>(in[0]), vector<std::int16_t, 4, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_86 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_form24_lane-1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh<A>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, true, -1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh<A>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_87 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form25_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 0>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, true, 0>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 0>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_88 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form25_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 1>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, true, 1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 1>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_89 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form25_lane2";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 2>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, true, 2>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 2>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_90 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form25_lane3";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 3>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, true, 3>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 3>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 4, A>(in[2]))); }
  };
  struct case_91 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form26_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 0>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, true, 0>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 0>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_92 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form26_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 1>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, true, 1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 1>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_93 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form26_lane2";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 2>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, true, 2>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 2>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_94 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form26_lane3";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 3>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, true, 3>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 3>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_95 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form26_lane4";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 4>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, true, 4>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 4>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_96 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form26_lane5";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 5>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, true, 5>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 5>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_97 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form26_lane6";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 6>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, true, 6>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 6>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_98 {
    using edge_type = std::int16_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form26_lane7";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 7>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int16_t, 8, true, 7>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 7>(vector<std::int16_t, 8, A>(in[0]), vector<std::int16_t, 8, A>(in[1]), vector<std::int16_t, 8, A>(in[2]))); }
  };
  struct case_99 {
    using edge_type = int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_form27_lane-1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh<A>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), scalar<int32_t>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int32_t, 1, true, -1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh<A>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), scalar<int32_t>(in[2]))); }
  };
  struct case_100 {
    using edge_type = int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form28_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 0>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), vector<std::int32_t, 2, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int32_t, 1, true, 0>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 0>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), vector<std::int32_t, 2, A>(in[2]))); }
  };
  struct case_101 {
    using edge_type = int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form28_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 1>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), vector<std::int32_t, 2, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int32_t, 1, true, 1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 1>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), vector<std::int32_t, 2, A>(in[2]))); }
  };
  struct case_102 {
    using edge_type = int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form29_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 0>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int32_t, 1, true, 0>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 0>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_103 {
    using edge_type = int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form29_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 1>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int32_t, 1, true, 1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 1>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_104 {
    using edge_type = int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form29_lane2";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 2>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int32_t, 1, true, 2>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 2>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_105 {
    using edge_type = int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form29_lane3";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 3>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<int32_t, 1, true, 3>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 3>(scalar<int32_t>(in[0]), scalar<int32_t>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_106 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_form30_lane-1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh<A>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 2, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 2, true, -1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh<A>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 2, A>(in[2]))); }
  };
  struct case_107 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form31_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 0>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 2, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 2, true, 0>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 0>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 2, A>(in[2]))); }
  };
  struct case_108 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form31_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 1>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 2, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 2, true, 1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 1>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 2, A>(in[2]))); }
  };
  struct case_109 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form32_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 0>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 2, true, 0>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 0>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_110 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form32_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 1>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 2, true, 1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 1>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_111 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form32_lane2";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 2>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 2, true, 2>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 2>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_112 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form32_lane3";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 3>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 2, true, 3>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 3>(vector<std::int32_t, 2, A>(in[0]), vector<std::int32_t, 2, A>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_113 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_form33_lane-1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh<A>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 4, true, -1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh<A>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_114 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form34_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 0>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 2, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 4, true, 0>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 0>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 2, A>(in[2]))); }
  };
  struct case_115 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form34_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 1>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 2, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 4, true, 1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 1>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 2, A>(in[2]))); }
  };
  struct case_116 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form35_lane0";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 0>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 4, true, 0>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 0>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_117 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form35_lane1";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 1>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 4, true, 1>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 1>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_118 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form35_lane2";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 2>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 4, true, 2>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 2>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
  struct case_119 {
    using edge_type = std::int32_t;
    static constexpr auto arch = native::feature_closure(native::arm_feature::rdm);
    static constexpr char name[] = "sqrdmlsh_lane_form35_lane3";
    template<native::isa<native::arm> A> static consteval output evaluate(input const &in) {
      return result(native::sqrdmlsh_lane<A, 3>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 4, A>(in[2])));
    }
    static constexpr output reference(input const &in) { return rdm_reference<std::int32_t, 4, true, 3>(in); }
    __attribute__((target("rdm"), noinline)) static output runtime(input const &in) { constexpr auto A = arch; return result(native::sqrdmlsh_lane<A, 3>(vector<std::int32_t, 4, A>(in[0]), vector<std::int32_t, 4, A>(in[1]), vector<std::int32_t, 4, A>(in[2]))); }
  };
}
int main() {
  auto cpu = native::observe_arm_capabilities();
  auto admission = native::classify_isa(cpu, native::target_features<native::arm>("rdm"));
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
  ok = check<case_102>() && ok;
  ok = check<case_103>() && ok;
  ok = check<case_104>() && ok;
  ok = check<case_105>() && ok;
  ok = check<case_106>() && ok;
  ok = check<case_107>() && ok;
  ok = check<case_108>() && ok;
  ok = check<case_109>() && ok;
  ok = check<case_110>() && ok;
  ok = check<case_111>() && ok;
  ok = check<case_112>() && ok;
  ok = check<case_113>() && ok;
  ok = check<case_114>() && ok;
  ok = check<case_115>() && ok;
  ok = check<case_116>() && ok;
  ok = check<case_117>() && ok;
  ok = check<case_118>() && ok;
  ok = check<case_119>() && ok;
  return ok ? 0 : 1;
}
