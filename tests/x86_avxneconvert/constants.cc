// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cstdint>
#include <type_traits>
#include <utility>
#if NATIVE_NE_INTERFACE == 1
import native.x86;
#elif NATIVE_NE_INTERFACE == 2
import native;
#else
import native.x86.avxneconvert;
#endif
#include "checks.h"
#include "../half_storage/native_bridge.h"
template<unsigned... I> constexpr bool all_cases(std::integer_sequence<unsigned, I...>) {
  return (checks::constant_case<I>() && ...);
}
static_assert(all_cases(std::make_integer_sequence<unsigned, 66>{}));
static_assert(native::test::bf16_storage_only<native::simd<native::bf16, 4, checks::strong>>);
static_assert(native::test::bf16_storage_only<native::simd<native::bf16, 8, checks::strong>>);
static_assert(native::test::bf16_storage_only<native::simd<native::fp16, 4, checks::strong>>);
static_assert(native::test::bf16_storage_only<native::simd<native::fp16, 8, checks::strong>>);
static_assert(native::test::bf16_storage_only<native::simd<native::bf16, 8, native::avx512_bf16>>);
static_assert(native::test::bf16_storage_only<native::simd<native::bf16, 16, native::avx512_bf16>>);
static_assert(native::test::bf16_storage_only<native::simd<native::bf16, 32, native::avx512_bf16>>);
template<class V>
concept closed_half_storage =
    native::test::bf16_storage_only<V> && !requires(V a) { ~a; } && !requires(V a) { !a; } &&
    !requires(V a, typename V::native_type n) { a -= n; } &&
    !requires(V a, typename V::native_type n) { a += n; } &&
    !requires(V a, typename V::native_type n) { a *= n; } &&
    !requires(V a, typename V::native_type n) { a /= n; } &&
    !requires(V a, typename V::native_type n) { a %= n; } &&
    !requires(V a, typename V::native_type n) { a &= n; } &&
    !requires(V a, typename V::native_type n) { a |= n; } &&
    !requires(V a, typename V::native_type n) { a ^= n; } &&
    !requires(V a, typename V::native_type n) { a <<= n; } &&
    !requires(V a, typename V::native_type n) { a >>= n; } &&
    !requires(V a, typename V::native_type n) { n - a; } &&
    !requires(V a, typename V::native_type n) { a - n; } &&
    !requires(V a, typename V::native_type n) { a == n; } &&
    !requires(V a, typename V::native_type n) { n == a; };
static_assert(closed_half_storage<native::simd<native::bf16, 4, checks::strong>>);
static_assert(closed_half_storage<native::simd<native::bf16, 8, checks::strong>>);
static_assert(closed_half_storage<native::simd<native::fp16, 4, checks::strong>>);
static_assert(closed_half_storage<native::simd<native::fp16, 8, checks::strong>>);
static_assert(sizeof(native::simd<native::bf16, 8, native::avx512_bf16>) == 16);
static_assert(sizeof(native::simd<native::bf16, 4, checks::strong>) == 16);
static_assert(
    std::same_as<typename native::simd<native::bf16, 4, checks::strong>::value_type, native::bf16>);
static_assert(noexcept(native::bcstnesh_ps<checks::strong, 4>(nullptr)));
constexpr auto sse2 = native::target_features<native::x86>("sse2");
static_assert(checks::verify(checks::constants<sse2, 4, 0>()));
constexpr auto combined =
    native::target_features<native::x86>("avxneconvert,avx512bf16,avx512vl,avx512bw,avx512dq,fma");
static_assert(checks::verify(checks::constants<combined, 4, 1>()));
static_assert(checks::verify(checks::constants<combined, 8, 1>()));
constexpr bool upper_zero() {
  constexpr std::array<float, 4> values{1.f, -1.f, 0.f, 2.f};
  auto r = native::cvtneps_bf16(native::simd<float, 4, checks::strong>::load(values.data()));
  auto words = std::bit_cast<std::array<std::uint16_t, 8>>(r.to_native());
  return words[4] == 0 && words[5] == 0 && words[6] == 0 && words[7] == 0;
}
static_assert(upper_zero());
constexpr bool single_source() {
  constexpr auto b = native::bf16::from_bits(1);
  constexpr auto h = native::fp16::from_bits(0xfc01);
  std::array<float, 8> x{}, y{};
  native::bcstnebf16_ps<checks::strong, 8>(&b).store(x.data());
  native::bcstnesh_ps<checks::weak, 8>(&h).store(y.data());
  for (unsigned i = 0; i < 8; ++i)
    if (std::bit_cast<std::uint32_t>(x[i]) != 0x00010000u ||
        std::bit_cast<std::uint32_t>(y[i]) != 0xffc02000u)
      return false;
  return true;
}
static_assert(single_source());
int main() {
  // Recheck emitted constant packets against the independent arithmetic oracle.
  constexpr auto p = checks::constants<checks::strong, 8, 17>();
  return checks::verify(p) ? 0 : 1;
}
