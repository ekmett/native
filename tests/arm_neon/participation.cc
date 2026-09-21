// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <concepts>
#include <cstdint>
import native;
constexpr auto neon = native::feature_closure(native::arm_feature::neon);
constexpr auto stronger = native::feature_closure(neon & native::arm_feature::rdm);
constexpr native::isa<native::arm> scalar{};
template <class T, std::size_t N, native::isa<native::arm> A = neon>
using V = native::simd<T, N, A>;
template <class L, class R>
concept signed_add = requires(L a, R b) { native::sqadd(a, b); };
template <class L, class R>
concept unsigned_add = requires(L a, R b) { native::uqadd(a, b); };
template <class L, class R>
concept signed_multiply = requires(L a, R b) { native::sqdmulh(a, b); };
template <class L, class R>
concept unsigned_shift = requires(L a, R b) { native::ushl(a, b); };
template <class T>
concept signed_narrow = requires(T a) { native::sqxtn(a); };
static_assert(signed_add<V<std::int8_t, 8>, V<std::int8_t, 8>>);
static_assert(!signed_add<V<std::uint8_t, 8>, V<std::uint8_t, 8>>);
static_assert(!unsigned_add<V<std::int8_t, 8>, V<std::int8_t, 8>>);
static_assert(!signed_add<V<std::int32_t, 4>, V<std::int32_t, 4, stronger>>);
static_assert(!signed_multiply<V<std::int8_t, 16>, V<std::int8_t, 16>>);
static_assert(!signed_multiply<V<std::int64_t, 2>, V<std::int64_t, 2>>);
static_assert(!signed_multiply<V<std::int16_t, 4, scalar>, V<std::int16_t, 4, scalar>>);
static_assert(unsigned_shift<V<std::uint16_t, 8>, V<std::int16_t, 8>>);
static_assert(!unsigned_shift<V<std::uint16_t, 8>, V<std::uint16_t, 8>>);
static_assert(!signed_narrow<V<std::int16_t, 4>>);
static_assert(!signed_narrow<V<std::int8_t, 16>>);
static_assert(!signed_narrow<V<std::uint16_t, 8>>);
static_assert(!signed_narrow<V<std::int64_t, 2, scalar>>);

consteval bool weak_constant() {
  using vector = V<std::int64_t, 1, scalar>;
  std::int64_t a = INT64_MAX, b = 1;
  auto result = native::sqadd(vector::load(&a), vector::load(&b));
  result.store(&a);
  return a == INT64_MAX;
}

static_assert(weak_constant());

__attribute__((target("neon"))) bool preserved_architecture() {
  using vector = V<std::int16_t, 8, stronger>;
  auto value = native::sqadd(vector(std::int16_t(1)), vector(std::int16_t(2)));
  static_assert(std::same_as<decltype(value), vector>);
  return all(value == vector(std::int16_t(3)));
}

int main() {
  if (!native::classify_isa(native::observe_arm_capabilities(), neon).admitted())
    return 77;
  return preserved_architecture() ? 0 : 1;
}
