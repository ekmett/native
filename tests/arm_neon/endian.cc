// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cstdint>
import native_test.arm.neon.endian;

constexpr auto architecture = native::neon & native::arm_feature::rdm;

template<class T, unsigned N>
using vector = native::simd<T, N, architecture>;

template<class T, unsigned N>
constexpr auto pack(std::array<T, N> x) {
  return vector<T, N>::from_native(std::bit_cast<typename vector<T, N>::native_type>(x));
}

#if !defined(NATIVE_NEON_ENDIAN_REJECT)
static_assert([] {
  auto x = pack<std::int64_t, 2>({4, 5});
  auto y = pack<std::int64_t, 2>({1, 1});
  auto result = native::sqadd(x, y);
  return std::bit_cast<std::array<std::int64_t, 2>>(result.to_native())[0] == 5;
}());
#elif NATIVE_NEON_ENDIAN_REJECT == 0
__attribute__((target("neon"))) vector<std::int64_t, 2>
reject_sqadd(vector<std::int64_t, 2> x, vector<std::int64_t, 2> y) {
  return native::sqadd(x, y);
}
#endif

#if !defined(NATIVE_NEON_ENDIAN_REJECT)
__attribute__((target("neon"))) vector<std::int32_t, 4>
positive_sqadd(vector<std::int32_t, 4> x, vector<std::int32_t, 4> y) {
  return native::sqadd(x, y);
}
__attribute__((target("neon"))) vector<std::int64_t, 1>
positive_scalar_sqadd(vector<std::int64_t, 1> x, vector<std::int64_t, 1> y) {
  return native::sqadd(x, y);
}
#endif

#if !defined(NATIVE_NEON_ENDIAN_REJECT)
static_assert([] {
  auto x = pack<std::uint64_t, 2>({4, 5});
  auto y = pack<std::uint64_t, 2>({1, 1});
  auto result = native::uqadd(x, y);
  return std::bit_cast<std::array<std::uint64_t, 2>>(result.to_native())[0] == 5;
}());
#elif NATIVE_NEON_ENDIAN_REJECT == 1
__attribute__((target("neon"))) vector<std::uint64_t, 2>
reject_uqadd(vector<std::uint64_t, 2> x, vector<std::uint64_t, 2> y) {
  return native::uqadd(x, y);
}
#endif

#if !defined(NATIVE_NEON_ENDIAN_REJECT)
__attribute__((target("neon"))) vector<std::uint32_t, 4>
positive_uqadd(vector<std::uint32_t, 4> x, vector<std::uint32_t, 4> y) {
  return native::uqadd(x, y);
}
__attribute__((target("neon"))) vector<std::uint64_t, 1>
positive_scalar_uqadd(vector<std::uint64_t, 1> x, vector<std::uint64_t, 1> y) {
  return native::uqadd(x, y);
}
#endif

#if !defined(NATIVE_NEON_ENDIAN_REJECT)
static_assert([] {
  auto x = pack<std::int64_t, 2>({4, 5});
  auto y = pack<std::int64_t, 2>({1, 1});
  auto result = native::sqsub(x, y);
  return std::bit_cast<std::array<std::int64_t, 2>>(result.to_native())[0] == 3;
}());
#elif NATIVE_NEON_ENDIAN_REJECT == 2
__attribute__((target("neon"))) vector<std::int64_t, 2>
reject_sqsub(vector<std::int64_t, 2> x, vector<std::int64_t, 2> y) {
  return native::sqsub(x, y);
}
#endif

#if !defined(NATIVE_NEON_ENDIAN_REJECT)
__attribute__((target("neon"))) vector<std::int32_t, 4>
positive_sqsub(vector<std::int32_t, 4> x, vector<std::int32_t, 4> y) {
  return native::sqsub(x, y);
}
__attribute__((target("neon"))) vector<std::int64_t, 1>
positive_scalar_sqsub(vector<std::int64_t, 1> x, vector<std::int64_t, 1> y) {
  return native::sqsub(x, y);
}
#endif

#if !defined(NATIVE_NEON_ENDIAN_REJECT)
static_assert([] {
  auto x = pack<std::uint64_t, 2>({4, 5});
  auto y = pack<std::uint64_t, 2>({1, 1});
  auto result = native::uqsub(x, y);
  return std::bit_cast<std::array<std::uint64_t, 2>>(result.to_native())[0] == 3;
}());
#elif NATIVE_NEON_ENDIAN_REJECT == 3
__attribute__((target("neon"))) vector<std::uint64_t, 2>
reject_uqsub(vector<std::uint64_t, 2> x, vector<std::uint64_t, 2> y) {
  return native::uqsub(x, y);
}
#endif

#if !defined(NATIVE_NEON_ENDIAN_REJECT)
__attribute__((target("neon"))) vector<std::uint32_t, 4>
positive_uqsub(vector<std::uint32_t, 4> x, vector<std::uint32_t, 4> y) {
  return native::uqsub(x, y);
}
__attribute__((target("neon"))) vector<std::uint64_t, 1>
positive_scalar_uqsub(vector<std::uint64_t, 1> x, vector<std::uint64_t, 1> y) {
  return native::uqsub(x, y);
}
#endif

#if !defined(NATIVE_NEON_ENDIAN_REJECT)
static_assert([] {
  auto x = pack<std::int64_t, 2>({4, 5});
  auto y = pack<std::int64_t, 2>({1, 1});
  auto result = native::sshl(x, y);
  return std::bit_cast<std::array<std::int64_t, 2>>(result.to_native())[0] == 8;
}());
#elif NATIVE_NEON_ENDIAN_REJECT == 4
__attribute__((target("neon"))) vector<std::int64_t, 2>
reject_sshl(vector<std::int64_t, 2> x, vector<std::int64_t, 2> y) {
  return native::sshl(x, y);
}
#endif

#if !defined(NATIVE_NEON_ENDIAN_REJECT)
__attribute__((target("neon"))) vector<std::int32_t, 4>
positive_sshl(vector<std::int32_t, 4> x, vector<std::int32_t, 4> y) {
  return native::sshl(x, y);
}
__attribute__((target("neon"))) vector<std::int64_t, 1>
positive_scalar_sshl(vector<std::int64_t, 1> x, vector<std::int64_t, 1> y) {
  return native::sshl(x, y);
}
#endif

#if !defined(NATIVE_NEON_ENDIAN_REJECT)
static_assert([] {
  auto x = pack<std::uint64_t, 2>({4, 5});
  auto y = pack<std::int64_t, 2>({1, 1});
  auto result = native::ushl(x, y);
  return std::bit_cast<std::array<std::uint64_t, 2>>(result.to_native())[0] == 8;
}());
#elif NATIVE_NEON_ENDIAN_REJECT == 5
__attribute__((target("neon"))) vector<std::uint64_t, 2>
reject_ushl(vector<std::uint64_t, 2> x, vector<std::int64_t, 2> y) {
  return native::ushl(x, y);
}
#endif

#if !defined(NATIVE_NEON_ENDIAN_REJECT)
__attribute__((target("neon"))) vector<std::uint32_t, 4>
positive_ushl(vector<std::uint32_t, 4> x, vector<std::int32_t, 4> y) {
  return native::ushl(x, y);
}
__attribute__((target("neon"))) vector<std::uint64_t, 1>
positive_scalar_ushl(vector<std::uint64_t, 1> x, vector<std::int64_t, 1> y) {
  return native::ushl(x, y);
}
#endif

#if !defined(NATIVE_NEON_ENDIAN_REJECT)
static_assert([] {
  auto x = pack<std::int64_t, 2>({4, 5});
  auto y = pack<std::int64_t, 2>({1, 1});
  auto result = native::srshl(x, y);
  return std::bit_cast<std::array<std::int64_t, 2>>(result.to_native())[0] == 8;
}());
#elif NATIVE_NEON_ENDIAN_REJECT == 6
__attribute__((target("neon"))) vector<std::int64_t, 2>
reject_srshl(vector<std::int64_t, 2> x, vector<std::int64_t, 2> y) {
  return native::srshl(x, y);
}
#endif

#if !defined(NATIVE_NEON_ENDIAN_REJECT)
__attribute__((target("neon"))) vector<std::int32_t, 4>
positive_srshl(vector<std::int32_t, 4> x, vector<std::int32_t, 4> y) {
  return native::srshl(x, y);
}
__attribute__((target("neon"))) vector<std::int64_t, 1>
positive_scalar_srshl(vector<std::int64_t, 1> x, vector<std::int64_t, 1> y) {
  return native::srshl(x, y);
}
#endif

#if !defined(NATIVE_NEON_ENDIAN_REJECT)
static_assert([] {
  auto x = pack<std::uint64_t, 2>({4, 5});
  auto y = pack<std::int64_t, 2>({1, 1});
  auto result = native::urshl(x, y);
  return std::bit_cast<std::array<std::uint64_t, 2>>(result.to_native())[0] == 8;
}());
#elif NATIVE_NEON_ENDIAN_REJECT == 7
__attribute__((target("neon"))) vector<std::uint64_t, 2>
reject_urshl(vector<std::uint64_t, 2> x, vector<std::int64_t, 2> y) {
  return native::urshl(x, y);
}
#endif

#if !defined(NATIVE_NEON_ENDIAN_REJECT)
__attribute__((target("neon"))) vector<std::uint32_t, 4>
positive_urshl(vector<std::uint32_t, 4> x, vector<std::int32_t, 4> y) {
  return native::urshl(x, y);
}
__attribute__((target("neon"))) vector<std::uint64_t, 1>
positive_scalar_urshl(vector<std::uint64_t, 1> x, vector<std::int64_t, 1> y) {
  return native::urshl(x, y);
}
#endif

#if !defined(NATIVE_NEON_ENDIAN_REJECT)
static_assert([] {
  auto x = pack<std::int64_t, 2>({4, 5});
  auto y = pack<std::int64_t, 2>({1, 1});
  auto result = native::sqshl(x, y);
  return std::bit_cast<std::array<std::int64_t, 2>>(result.to_native())[0] == 8;
}());
#elif NATIVE_NEON_ENDIAN_REJECT == 8
__attribute__((target("neon"))) vector<std::int64_t, 2>
reject_sqshl(vector<std::int64_t, 2> x, vector<std::int64_t, 2> y) {
  return native::sqshl(x, y);
}
#endif

#if !defined(NATIVE_NEON_ENDIAN_REJECT)
__attribute__((target("neon"))) vector<std::int32_t, 4>
positive_sqshl(vector<std::int32_t, 4> x, vector<std::int32_t, 4> y) {
  return native::sqshl(x, y);
}
__attribute__((target("neon"))) vector<std::int64_t, 1>
positive_scalar_sqshl(vector<std::int64_t, 1> x, vector<std::int64_t, 1> y) {
  return native::sqshl(x, y);
}
#endif

#if !defined(NATIVE_NEON_ENDIAN_REJECT)
static_assert([] {
  auto x = pack<std::uint64_t, 2>({4, 5});
  auto y = pack<std::int64_t, 2>({1, 1});
  auto result = native::uqshl(x, y);
  return std::bit_cast<std::array<std::uint64_t, 2>>(result.to_native())[0] == 8;
}());
#elif NATIVE_NEON_ENDIAN_REJECT == 9
__attribute__((target("neon"))) vector<std::uint64_t, 2>
reject_uqshl(vector<std::uint64_t, 2> x, vector<std::int64_t, 2> y) {
  return native::uqshl(x, y);
}
#endif

#if !defined(NATIVE_NEON_ENDIAN_REJECT)
__attribute__((target("neon"))) vector<std::uint32_t, 4>
positive_uqshl(vector<std::uint32_t, 4> x, vector<std::int32_t, 4> y) {
  return native::uqshl(x, y);
}
__attribute__((target("neon"))) vector<std::uint64_t, 1>
positive_scalar_uqshl(vector<std::uint64_t, 1> x, vector<std::int64_t, 1> y) {
  return native::uqshl(x, y);
}
#endif

#if !defined(NATIVE_NEON_ENDIAN_REJECT)
static_assert([] {
  auto x = pack<std::int64_t, 2>({4, 5});
  auto y = pack<std::int64_t, 2>({1, 1});
  auto result = native::sqrshl(x, y);
  return std::bit_cast<std::array<std::int64_t, 2>>(result.to_native())[0] == 8;
}());
#elif NATIVE_NEON_ENDIAN_REJECT == 10
__attribute__((target("neon"))) vector<std::int64_t, 2>
reject_sqrshl(vector<std::int64_t, 2> x, vector<std::int64_t, 2> y) {
  return native::sqrshl(x, y);
}
#endif

#if !defined(NATIVE_NEON_ENDIAN_REJECT)
__attribute__((target("neon"))) vector<std::int32_t, 4>
positive_sqrshl(vector<std::int32_t, 4> x, vector<std::int32_t, 4> y) {
  return native::sqrshl(x, y);
}
__attribute__((target("neon"))) vector<std::int64_t, 1>
positive_scalar_sqrshl(vector<std::int64_t, 1> x, vector<std::int64_t, 1> y) {
  return native::sqrshl(x, y);
}
#endif

#if !defined(NATIVE_NEON_ENDIAN_REJECT)
static_assert([] {
  auto x = pack<std::uint64_t, 2>({4, 5});
  auto y = pack<std::int64_t, 2>({1, 1});
  auto result = native::uqrshl(x, y);
  return std::bit_cast<std::array<std::uint64_t, 2>>(result.to_native())[0] == 8;
}());
#elif NATIVE_NEON_ENDIAN_REJECT == 11
__attribute__((target("neon"))) vector<std::uint64_t, 2>
reject_uqrshl(vector<std::uint64_t, 2> x, vector<std::int64_t, 2> y) {
  return native::uqrshl(x, y);
}
#endif

#if !defined(NATIVE_NEON_ENDIAN_REJECT)
__attribute__((target("neon"))) vector<std::uint32_t, 4>
positive_uqrshl(vector<std::uint32_t, 4> x, vector<std::int32_t, 4> y) {
  return native::uqrshl(x, y);
}
__attribute__((target("neon"))) vector<std::uint64_t, 1>
positive_scalar_uqrshl(vector<std::uint64_t, 1> x, vector<std::int64_t, 1> y) {
  return native::uqrshl(x, y);
}
#endif

int main() {}
