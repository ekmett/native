// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cstdint>
import native.arm.neon;

constexpr auto architecture = native::neon & native::arm_feature::rdm;

template<class T, unsigned N>
using vector = native::simd<T, N, architecture>;

template<class T, unsigned N>
constexpr auto pack(std::array<T, N> x) {
  return vector<T, N>::from_native(std::bit_cast<typename vector<T, N>::native_type>(x));
}

static_assert([] {
  auto x = pack<std::int64_t, 2>({4, 5});
  auto y = pack<std::int64_t, 2>({1, 1});
  auto result = native::sqadd(x, y);
  return std::bit_cast<std::array<std::int64_t, 2>>(result.to_native())[0] == 5;
}());
__attribute__((target("neon"))) vector<std::int64_t, 2>
positive_64x2_sqadd(vector<std::int64_t, 2> x, vector<std::int64_t, 2> y) {
  return native::sqadd(x, y);
}

__attribute__((target("neon"))) vector<std::int32_t, 4>
positive_sqadd(vector<std::int32_t, 4> x, vector<std::int32_t, 4> y) {
  return native::sqadd(x, y);
}
__attribute__((target("neon"))) vector<std::int64_t, 1>
positive_scalar_sqadd(vector<std::int64_t, 1> x, vector<std::int64_t, 1> y) {
  return native::sqadd(x, y);
}

static_assert([] {
  auto x = pack<std::uint64_t, 2>({4, 5});
  auto y = pack<std::uint64_t, 2>({1, 1});
  auto result = native::uqadd(x, y);
  return std::bit_cast<std::array<std::uint64_t, 2>>(result.to_native())[0] == 5;
}());
__attribute__((target("neon"))) vector<std::uint64_t, 2>
positive_64x2_uqadd(vector<std::uint64_t, 2> x, vector<std::uint64_t, 2> y) {
  return native::uqadd(x, y);
}

__attribute__((target("neon"))) vector<std::uint32_t, 4>
positive_uqadd(vector<std::uint32_t, 4> x, vector<std::uint32_t, 4> y) {
  return native::uqadd(x, y);
}
__attribute__((target("neon"))) vector<std::uint64_t, 1>
positive_scalar_uqadd(vector<std::uint64_t, 1> x, vector<std::uint64_t, 1> y) {
  return native::uqadd(x, y);
}

static_assert([] {
  auto x = pack<std::int64_t, 2>({4, 5});
  auto y = pack<std::int64_t, 2>({1, 1});
  auto result = native::sqsub(x, y);
  return std::bit_cast<std::array<std::int64_t, 2>>(result.to_native())[0] == 3;
}());
__attribute__((target("neon"))) vector<std::int64_t, 2>
positive_64x2_sqsub(vector<std::int64_t, 2> x, vector<std::int64_t, 2> y) {
  return native::sqsub(x, y);
}

__attribute__((target("neon"))) vector<std::int32_t, 4>
positive_sqsub(vector<std::int32_t, 4> x, vector<std::int32_t, 4> y) {
  return native::sqsub(x, y);
}
__attribute__((target("neon"))) vector<std::int64_t, 1>
positive_scalar_sqsub(vector<std::int64_t, 1> x, vector<std::int64_t, 1> y) {
  return native::sqsub(x, y);
}

static_assert([] {
  auto x = pack<std::uint64_t, 2>({4, 5});
  auto y = pack<std::uint64_t, 2>({1, 1});
  auto result = native::uqsub(x, y);
  return std::bit_cast<std::array<std::uint64_t, 2>>(result.to_native())[0] == 3;
}());
__attribute__((target("neon"))) vector<std::uint64_t, 2>
positive_64x2_uqsub(vector<std::uint64_t, 2> x, vector<std::uint64_t, 2> y) {
  return native::uqsub(x, y);
}

__attribute__((target("neon"))) vector<std::uint32_t, 4>
positive_uqsub(vector<std::uint32_t, 4> x, vector<std::uint32_t, 4> y) {
  return native::uqsub(x, y);
}
__attribute__((target("neon"))) vector<std::uint64_t, 1>
positive_scalar_uqsub(vector<std::uint64_t, 1> x, vector<std::uint64_t, 1> y) {
  return native::uqsub(x, y);
}

static_assert([] {
  auto x = pack<std::int64_t, 2>({4, 5});
  auto y = pack<std::int64_t, 2>({1, 1});
  auto result = native::sshl(x, y);
  return std::bit_cast<std::array<std::int64_t, 2>>(result.to_native())[0] == 8;
}());
__attribute__((target("neon"))) vector<std::int64_t, 2>
positive_64x2_sshl(vector<std::int64_t, 2> x, vector<std::int64_t, 2> y) {
  return native::sshl(x, y);
}

__attribute__((target("neon"))) vector<std::int32_t, 4>
positive_sshl(vector<std::int32_t, 4> x, vector<std::int32_t, 4> y) {
  return native::sshl(x, y);
}
__attribute__((target("neon"))) vector<std::int64_t, 1>
positive_scalar_sshl(vector<std::int64_t, 1> x, vector<std::int64_t, 1> y) {
  return native::sshl(x, y);
}

static_assert([] {
  auto x = pack<std::uint64_t, 2>({4, 5});
  auto y = pack<std::int64_t, 2>({1, 1});
  auto result = native::ushl(x, y);
  return std::bit_cast<std::array<std::uint64_t, 2>>(result.to_native())[0] == 8;
}());
__attribute__((target("neon"))) vector<std::uint64_t, 2>
positive_64x2_ushl(vector<std::uint64_t, 2> x, vector<std::int64_t, 2> y) {
  return native::ushl(x, y);
}

__attribute__((target("neon"))) vector<std::uint32_t, 4>
positive_ushl(vector<std::uint32_t, 4> x, vector<std::int32_t, 4> y) {
  return native::ushl(x, y);
}
__attribute__((target("neon"))) vector<std::uint64_t, 1>
positive_scalar_ushl(vector<std::uint64_t, 1> x, vector<std::int64_t, 1> y) {
  return native::ushl(x, y);
}

static_assert([] {
  auto x = pack<std::int64_t, 2>({4, 5});
  auto y = pack<std::int64_t, 2>({1, 1});
  auto result = native::srshl(x, y);
  return std::bit_cast<std::array<std::int64_t, 2>>(result.to_native())[0] == 8;
}());
__attribute__((target("neon"))) vector<std::int64_t, 2>
positive_64x2_srshl(vector<std::int64_t, 2> x, vector<std::int64_t, 2> y) {
  return native::srshl(x, y);
}

__attribute__((target("neon"))) vector<std::int32_t, 4>
positive_srshl(vector<std::int32_t, 4> x, vector<std::int32_t, 4> y) {
  return native::srshl(x, y);
}
__attribute__((target("neon"))) vector<std::int64_t, 1>
positive_scalar_srshl(vector<std::int64_t, 1> x, vector<std::int64_t, 1> y) {
  return native::srshl(x, y);
}

static_assert([] {
  auto x = pack<std::uint64_t, 2>({4, 5});
  auto y = pack<std::int64_t, 2>({1, 1});
  auto result = native::urshl(x, y);
  return std::bit_cast<std::array<std::uint64_t, 2>>(result.to_native())[0] == 8;
}());
__attribute__((target("neon"))) vector<std::uint64_t, 2>
positive_64x2_urshl(vector<std::uint64_t, 2> x, vector<std::int64_t, 2> y) {
  return native::urshl(x, y);
}

__attribute__((target("neon"))) vector<std::uint32_t, 4>
positive_urshl(vector<std::uint32_t, 4> x, vector<std::int32_t, 4> y) {
  return native::urshl(x, y);
}
__attribute__((target("neon"))) vector<std::uint64_t, 1>
positive_scalar_urshl(vector<std::uint64_t, 1> x, vector<std::int64_t, 1> y) {
  return native::urshl(x, y);
}

static_assert([] {
  auto x = pack<std::int64_t, 2>({4, 5});
  auto y = pack<std::int64_t, 2>({1, 1});
  auto result = native::sqshl(x, y);
  return std::bit_cast<std::array<std::int64_t, 2>>(result.to_native())[0] == 8;
}());
__attribute__((target("neon"))) vector<std::int64_t, 2>
positive_64x2_sqshl(vector<std::int64_t, 2> x, vector<std::int64_t, 2> y) {
  return native::sqshl(x, y);
}

__attribute__((target("neon"))) vector<std::int32_t, 4>
positive_sqshl(vector<std::int32_t, 4> x, vector<std::int32_t, 4> y) {
  return native::sqshl(x, y);
}
__attribute__((target("neon"))) vector<std::int64_t, 1>
positive_scalar_sqshl(vector<std::int64_t, 1> x, vector<std::int64_t, 1> y) {
  return native::sqshl(x, y);
}

static_assert([] {
  auto x = pack<std::uint64_t, 2>({4, 5});
  auto y = pack<std::int64_t, 2>({1, 1});
  auto result = native::uqshl(x, y);
  return std::bit_cast<std::array<std::uint64_t, 2>>(result.to_native())[0] == 8;
}());
__attribute__((target("neon"))) vector<std::uint64_t, 2>
positive_64x2_uqshl(vector<std::uint64_t, 2> x, vector<std::int64_t, 2> y) {
  return native::uqshl(x, y);
}

__attribute__((target("neon"))) vector<std::uint32_t, 4>
positive_uqshl(vector<std::uint32_t, 4> x, vector<std::int32_t, 4> y) {
  return native::uqshl(x, y);
}
__attribute__((target("neon"))) vector<std::uint64_t, 1>
positive_scalar_uqshl(vector<std::uint64_t, 1> x, vector<std::int64_t, 1> y) {
  return native::uqshl(x, y);
}

static_assert([] {
  auto x = pack<std::int64_t, 2>({4, 5});
  auto y = pack<std::int64_t, 2>({1, 1});
  auto result = native::sqrshl(x, y);
  return std::bit_cast<std::array<std::int64_t, 2>>(result.to_native())[0] == 8;
}());
__attribute__((target("neon"))) vector<std::int64_t, 2>
positive_64x2_sqrshl(vector<std::int64_t, 2> x, vector<std::int64_t, 2> y) {
  return native::sqrshl(x, y);
}

__attribute__((target("neon"))) vector<std::int32_t, 4>
positive_sqrshl(vector<std::int32_t, 4> x, vector<std::int32_t, 4> y) {
  return native::sqrshl(x, y);
}
__attribute__((target("neon"))) vector<std::int64_t, 1>
positive_scalar_sqrshl(vector<std::int64_t, 1> x, vector<std::int64_t, 1> y) {
  return native::sqrshl(x, y);
}

static_assert([] {
  auto x = pack<std::uint64_t, 2>({4, 5});
  auto y = pack<std::int64_t, 2>({1, 1});
  auto result = native::uqrshl(x, y);
  return std::bit_cast<std::array<std::uint64_t, 2>>(result.to_native())[0] == 8;
}());
__attribute__((target("neon"))) vector<std::uint64_t, 2>
positive_64x2_uqrshl(vector<std::uint64_t, 2> x, vector<std::int64_t, 2> y) {
  return native::uqrshl(x, y);
}

__attribute__((target("neon"))) vector<std::uint32_t, 4>
positive_uqrshl(vector<std::uint32_t, 4> x, vector<std::int32_t, 4> y) {
  return native::uqrshl(x, y);
}
__attribute__((target("neon"))) vector<std::uint64_t, 1>
positive_scalar_uqrshl(vector<std::uint64_t, 1> x, vector<std::int64_t, 1> y) {
  return native::uqrshl(x, y);
}

int main() {}
