#include "support/profile.h"
#include "support/guarded_pages.h"
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <concepts>
using namespace test_simd;
static std::uint64_t checks = 0;
static void check(bool condition) {
  ++checks;
  if (!condition) {
    std::fprintf(stderr, "failure at check %llu\n", static_cast<unsigned long long>(checks));
    std::abort();
  }
}
template <class T> using U = std::make_unsigned_t<T>;
template <class T> T word(std::uint64_t x) { return std::bit_cast<T>(static_cast<U<T>>(x)); }
template <class T> std::uint64_t bits(T x) { return std::uint64_t(std::bit_cast<U<T>>(x)); }
template <class T> T add(T a, T b) { return word<T>(bits(a) + bits(b)); }
template <class T> T sub(T a, T b) { return word<T>(bits(a) - bits(b)); }
template <class T> T mul(T a, T b) { return word<T>(bits(a) * bits(b)); }
template <class T, unsigned K> T right(T a) {
  if constexpr (K == 0)
    return a;
  else {
    auto x = bits(a) >> K;
    if constexpr (std::is_signed_v<T>)
      if (a < 0)
        x |= (~std::uint64_t(0) << (sizeof(T) * 8 - K));
    return word<T>(x);
  }
}
template <class T, std::size_t N, class F>
void result(test_vec<T, N> x, std::array<T, N> const &a, std::array<T, N> const &b, F f) {
  std::array<T, N> actual{};
  simd::store_simd(actual.data(), x);
  for (std::size_t i = 0; i < N; ++i)
    check(actual[i] == f(a[i], b[i]));
}
template <class V>
concept has_divide = requires(V a) {
  a / a;
  a / 1;
  1 / a;
};
template <class V>
concept has_remainder = requires(V a) {
  a % a;
  a % 1;
};
template <class V>
concept has_runtime_shift = requires(V a, unsigned n) {
  a << n;
  a >> n;
};
template<class V,class U> concept mixed_add = requires(V a,U b){a+b;b+a;};
static_assert(!mixed_add<test_vec<int32_t,1>,bool>);
static_assert(!mixed_add<test_vec<int32_t,1>,float>);
static_assert(!mixed_add<test_vec<int32_t,1>,test_vec<uint32_t,1>>);
template <class V>
concept has_bad_shift = requires(V a) { a << imm<sizeof(typename V::value_type) * 8>; };
template<class V>
concept has_bad_right_shift = requires(V a) { a >> imm<sizeof(typename V::value_type) * 8>; };
template <class T, std::size_t N, std::size_t... K>
void shifts(test_vec<T, N> value, std::array<T, N> const &a, std::index_sequence<K...>) {
  (result(value >> imm<K>, a, a, [](T x, T) { return right<T, K>(x); }), ...);
  (result(value << imm<K>, a, a, [](T x, T) { return word<T>(bits(x) << K); }), ...);
}
template <class T, std::size_t N> void test() {
  using V = test_vec<T, N>;
  static_assert(!has_divide<V> && !has_remainder<V> && !has_runtime_shift<V> && !has_bad_shift<V> && !has_bad_right_shift<V>);
  static_assert(sizeof(V) == sizeof(T) * N);
  static_assert(std::is_trivially_copyable_v<V>);
  std::array<T, N> a{}, b{};
  for (unsigned round = 0; round < 257; ++round) {
    for (std::size_t i = 0; i < N; ++i) {
      auto x = std::uint64_t(round) * 0x9e3779b97f4a7c15ull + std::uint64_t(i) * 0xd1b54a32d192ed03ull;
      a[i] = word<T>(x);
      b[i] = word<T>((x >> 29) ^ 0xfedcba9876543210ull);
      if (round < 8)
        a[i] = word<T>((round & 1) ? ~std::uint64_t(0) : std::uint64_t(1) << (round * 8));
    }
    auto va = simd::load_simd<test_vec<T,N>>(a.data()), vb = simd::load_simd<test_vec<T,N>>(b.data());
    result(va + vb, a, b, add<T>);
    result(va - vb, a, b, sub<T>);
    result(va * vb, a, b, mul<T>);
    result(va & vb, a, b, [](T x, T y) { return word<T>(bits(x) & bits(y)); });
    result(va | vb, a, b, [](T x, T y) { return word<T>(bits(x) | bits(y)); });
    result(va ^ vb, a, b, [](T x, T y) { return word<T>(bits(x) ^ bits(y)); });
    result(-va, a, b, [](T x, T) { return word<T>(0 - bits(x)); });
    result(va << imm<1>, a, b, [](T x, T) { return word<T>(bits(x) << 1); });
    result(va >> imm<1>, a, b, [](T x, T) { return right<T, 1>(x); });
    result(va << imm<sizeof(T) * 8 - 1>, a, b,
           [](T x, T) { return word<T>(bits(x) << (sizeof(T) * 8 - 1)); });
    result(va >> imm<sizeof(T) * 8 - 1>, a, b, [](T x, T) { return right<T, sizeof(T) * 8 - 1>(x); });
    result(va << imm<0>, a, b, [](T x, T) { return x; });
    result(va >> imm<0>, a, b, [](T x, T) { return x; });
    std::uint64_t equal = 0, greater = 0;
    for (std::size_t i = 0; i < N; ++i) {
      if (a[i] == b[i])
        equal |= std::uint64_t(1) << i;
      if (a[i] > b[i])
        greater |= std::uint64_t(1) << i;
    }
    check((va == vb).to_bitset() == equal);
    check((va > vb).to_bitset() == greater);
    check((va < vb).to_bitset() == (vb > va).to_bitset());
    check((va != vb).to_bitset() == (~(va == vb)).to_bitset());
    result(select(va > vb, va, vb), a, b, [](T x, T y) { return x > y ? x : y; });
    result(bit_select(va, va, vb), a, b,
           [](T x, T y) { return word<T>((bits(x) & bits(x)) | ((~bits(x)) & bits(y))); });
    result(masked_add(va > vb, vb, va, vb), a, b, [](T x, T y) { return x > y ? add(x, y) : y; });
    result(masked_sub(va > vb, vb, va, vb), a, b, [](T x, T y) { return x > y ? sub(x, y) : y; });
    result(masked_mul(va > vb, vb, va, vb), a, b, [](T x, T y) { return x > y ? mul(x, y) : y; });
    auto projected = mask_bits<T>(va > vb);
    std::array<U<T>, N> pm{};
    simd::store_simd(pm.data(), projected);
    for (std::size_t i = 0; i < N; ++i)
      check(pm[i] == (a[i] > b[i] ? U<T>(~U<T>(0)) : U<T>(0)));
  }
  shifts(simd::load_simd<test_vec<T,N>>(a.data()), a, std::make_index_sequence<sizeof(T) * 8>{});
  auto full = test_vec<SIMD_BACKEND_NAMESPACE::mask_lane_for<T>, N>::from_bitset(0xaaaaaaaaaaaaaaaaull);
  auto fa = simd::load_simd<test_vec<T,N>>(a.data()), fb = simd::load_simd<test_vec<T,N>>(b.data());
  std::array<T, N> selected{};
  simd::store_simd(selected.data(), masked_add(full, fb, fa, fb));
  for (std::size_t i = 0; i < N; ++i)
    check(selected[i] == ((i & 1) ? add(a[i], b[i]) : b[i]));
  simd::test::guarded_pages page;
  for (std::size_t n = 0; n <= N; ++n) {
    auto p = reinterpret_cast<T *>(page.end() - n * sizeof(T));
    if (n)
      std::memcpy(p, a.data(), n * sizeof(T));
    auto v = simd::load_simd_partial<test_vec<T,N>>(p, n);
    std::array<T, N> actual{};
    simd::store_simd(actual.data(), v);
    for (std::size_t i = 0; i < N; ++i)
      check(actual[i] == (i < n ? a[i] : T(0)));
    simd::store_simd_partial(p, simd::load_simd<test_vec<T,N>>(b.data()), n);
    for (std::size_t i = 0; i < n; ++i)
      check(p[i] == b[i]);
    auto q = reinterpret_cast<T *>(page.begin());
    simd::store_simd_partial(q, v, n);
    for (std::size_t i = 0; i < n; ++i)
      check(q[i] == a[i]);
  }
  auto zero = simd::load_simd_partial<test_vec<T,N>>(static_cast<T const *>(nullptr), 0);
  simd::store_simd_partial(static_cast<T *>(nullptr), zero, 0);
  check(none(zero != V(T(0))));
}
template <class T> void widths() {
  ::test<T, 1>();
#if defined(__AVX2__) || defined(__ARM_NEON)
  ::test<T, 16 / sizeof(T)>();
#endif
#if defined(__AVX2__)
  ::test<T, 32 / sizeof(T)>();
#endif
#if defined(__AVX512F__) && defined(__AVX512DQ__) && defined(__AVX512BW__)
  ::test<T, 64 / sizeof(T)>();
#endif
}
static_assert(test_vec<uint16_t,1>(65535u).value==65535u);
static_assert((test_vec<uint16_t,1>(65535u)*test_vec<uint16_t,1>(65535u)).value==1);
static_assert((test_vec<int32_t,1>(0x7fffffffu)+1).value==std::numeric_limits<int32_t>::min());
static_assert((test_vec<int64_t,1>(-1)>>imm<63>).value==-1);
static_assert(any(test_vec<int32_t,1>(-1)<test_vec<int32_t,1>(0)));
int main() {
  static_assert(std::same_as<decltype(simd::vec{test_arch{},int32_t(1)}), test_vec<int32_t, 1>>);
#if defined(__AVX2__) || defined(__ARM_NEON)
  static_assert(std::same_as<decltype(simd::vec{test_arch{},int32_t(1), int32_t(2), int32_t(3), int32_t(4)}), test_vec<int32_t, 4>>);
#endif
#if defined(__AVX2__) || defined(__ARM_NEON)
  simd::vec mixed{test_arch{},int16_t(1), uint16_t(2), int32_t(3), int32_t(4)};
  static_assert(std::same_as<decltype(mixed), test_vec<int32_t, 4>>);
  std::array<int32_t, 4> lanes{};
  simd::store_simd(lanes.data(), mixed);
  check((lanes == std::array<int32_t, 4>{1, 2, 3, 4}));
#endif
  widths<int8_t>();
  widths<uint8_t>();
  widths<int16_t>();
  widths<uint16_t>();
  widths<int32_t>();
  widths<uint32_t>();
  widths<int64_t>();
  widths<uint64_t>();
  std::printf("{\"passed\":true,\"checks\":%llu}\n", static_cast<unsigned long long>(checks));
}

/**
 * \file
 * \license
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
 * SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
 * \endlicense
 * \author Edward Kmett <ekmett@gmail.com>
 * \brief Checks native integer arithmetic, typed predicates, and guarded memory tails.
 */
