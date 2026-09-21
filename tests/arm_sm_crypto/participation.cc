// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <concepts>
#include <cstdint>
import native;
constexpr auto required = native::target_features<native::arm>("sm4");
constexpr auto stronger = required & native::arm_feature::rdm;
template <native::isa<native::arm> A = required> using vector = native::simd<std::uint32_t, 4, A>;
template <class V>
concept sm3_shape = requires(V x) { native::sm3ss1(x, x, x); };
template <class V>
concept sm4_shape = requires(V x) { native::sm4e(x, x); };
template <int Lane>
concept tt_lane = requires(vector<> x) {
  native::sm3tt1a<required, Lane>(x, x, x);
  native::sm3tt1b<required, Lane>(x, x, x);
  native::sm3tt2a<required, Lane>(x, x, x);
  native::sm3tt2b<required, Lane>(x, x, x);
};
static_assert(tt_lane<0> && tt_lane<3> && !tt_lane<-1> && !tt_lane<4>);
static_assert(sm3_shape<vector<>> && sm4_shape<vector<>>);
static_assert(!sm3_shape<native::simd<std::int32_t, 4, required>>);
static_assert(!sm3_shape<native::simd<std::uint32_t, 2, required>>);
static_assert(!sm4_shape<native::simd<std::uint64_t, 2, required>>);
static_assert(!sm4_shape<native::simd<std::uint32_t, 2, required>>);
#define CHECK_TYPE(op)                                                                             \
  static_assert(requires(vector<stronger> x) {                                                     \
    { native::op(x, x, x) } -> std::same_as<vector<stronger>>;                                     \
  })
CHECK_TYPE(sm3ss1);
CHECK_TYPE(sm3partw1);
CHECK_TYPE(sm3partw2);
#undef CHECK_TYPE
#define CHECK_TYPE(op)                                                                             \
  static_assert(requires(vector<stronger> x) {                                                     \
    { native::op<stronger, 0>(x, x, x) } -> std::same_as<vector<stronger>>;                        \
  })
CHECK_TYPE(sm3tt1a);
CHECK_TYPE(sm3tt1b);
CHECK_TYPE(sm3tt2a);
CHECK_TYPE(sm3tt2b);
#undef CHECK_TYPE
static_assert(requires(vector<stronger> x) {
  { native::sm4e(x, x) } -> std::same_as<vector<stronger>>;
});
static_assert(requires(vector<stronger> x) {
  { native::sm4ekey(x, x) } -> std::same_as<vector<stronger>>;
});

int main() {}
