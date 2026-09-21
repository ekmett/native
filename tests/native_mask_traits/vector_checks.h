// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <array>
#include <concepts>
#include <cstdint>

namespace mask_fixture { struct custom_element { float value; }; }

template<> struct native::simd_traits<mask_fixture::custom_element> {
  using storage_type = float;
};
// The SIMD extension must supply its raw mask even when the consumer's
// customization contributes no associated types or operations of its own.
template<class Raw, class Self>
struct native::simd_customization<mask_fixture::custom_element, Raw, Self> {};

namespace mask_fixture {
  template<class T, std::size_t N, native::isa<> A>
  consteval bool vector_mask_matches() {
    using V = native::simd<T, N, A>;
    using M = typename V::mask_type;
    static_assert(std::same_as<typename native::mask_traits<V>::type, M>);
    static_assert(std::same_as<native::mask<V>, M>);
    static_assert(std::same_as<native::mask<V const volatile&>, M>);
    static_assert(std::same_as<native::mask<native::simd<T, N, A>>, M>);
    static_assert(std::same_as<native::mask<std::array<V, 2>>, std::array<M, 2>>);
    return true;
  }

  static_assert(vector_mask_matches<float, 1, native::scalar>());
  static_assert(vector_mask_matches<std::int32_t, 1, native::scalar>());
  static_assert(vector_mask_matches<std::uint64_t, 1, native::scalar>());
  static_assert(vector_mask_matches<bool, 1, native::scalar>());
  static_assert(vector_mask_matches<native::mask32, 1, native::scalar>());
  static_assert(vector_mask_matches<custom_element, 1, native::scalar>());
  static_assert(std::same_as<native::mask<native::simd<custom_element, 1, native::scalar>>,
    native::mask<native::simd<float, 1, native::scalar>>>);
  static_assert(std::same_as<native::mask<native::mask8>, native::mask8>);
  static_assert(std::same_as<native::mask<native::mask16>, native::mask16>);
  static_assert(std::same_as<native::mask<native::mask32 const&>, native::mask32>);
  static_assert(std::same_as<native::mask<native::mask64>, native::mask64>);
  static_assert(!has_mask<native::simd<double, 4, native::scalar>>);
  static_assert(!has_mask<native::simd<unsupported, 1, native::scalar>>);
  static_assert(!has_mask<native::predicate<0, native::scalar>>);
  static_assert(!has_mask<native::predicate<65, native::scalar>>);

#if !defined(NATIVE_MASK_TRAITS_SCALAR_ONLY) && (defined(__x86_64__) || defined(_M_X64))
  static_assert(vector_mask_matches<float, 2, native::avx2>());
  static_assert(vector_mask_matches<float, 3, native::avx2>());
  static_assert(vector_mask_matches<float, 4, native::avx2>());
  static_assert(vector_mask_matches<float, 8, native::avx2>());
  static_assert(vector_mask_matches<float, 16, native::avx512>());
  static_assert(vector_mask_matches<std::int32_t, 2, native::avx2>());
  static_assert(vector_mask_matches<std::uint32_t, 3, native::avx2>());
  static_assert(vector_mask_matches<std::int32_t, 4, native::avx2>());
  static_assert(vector_mask_matches<std::uint32_t, 8, native::avx2>());
  static_assert(vector_mask_matches<std::int32_t, 16, native::avx512>());
  static_assert(vector_mask_matches<native::mask32, 3, native::avx2>());
  static_assert(vector_mask_matches<native::mask32, 8, native::avx2>());
  static_assert(vector_mask_matches<native::mask32, 16, native::avx512>());
  static_assert(vector_mask_matches<custom_element, 8, native::avx2>());
  static_assert(std::same_as<native::mask<native::simd<custom_element, 8, native::avx2>>,
    native::mask<native::simd<float, 8, native::avx2>>>);
  static_assert(std::same_as<native::mask<native::simd<float, 4, native::avx2>>,
    native::simd<native::mask32, 4, native::avx2>>);
  static_assert(std::same_as<native::mask<native::simd<float, 4, native::avx512>>,
    native::predicate<4, native::avx512>>);
  static_assert(std::same_as<native::mask<native::simd<float, 16, native::avx512>>,
    native::predicate<16, native::avx512>>);
  static_assert(std::same_as<native::mask<native::predicate<3, native::avx512> const&>,
    native::predicate<3, native::avx512>>);
  static_assert(std::same_as<native::mask<native::predicate<16, native::avx512>>,
    native::predicate<16, native::avx512>>);
  static_assert(!has_mask<native::simd<float, 5, native::avx2>>);
  static_assert(!has_mask<native::predicate<0, native::avx512>>);
  static_assert(!has_mask<native::predicate<65, native::avx512>>);
#elif !defined(NATIVE_MASK_TRAITS_SCALAR_ONLY) && (defined(__aarch64__) || defined(_M_ARM64))
  static_assert(vector_mask_matches<float, 2, native::neon>());
  static_assert(vector_mask_matches<float, 3, native::neon>());
  static_assert(vector_mask_matches<float, 4, native::neon>());
  static_assert(vector_mask_matches<std::int32_t, 2, native::neon>());
  static_assert(vector_mask_matches<std::uint32_t, 3, native::neon>());
  static_assert(vector_mask_matches<std::int32_t, 4, native::neon>());
  static_assert(vector_mask_matches<native::mask32, 3, native::neon>());
  static_assert(vector_mask_matches<native::mask32, 4, native::neon>());
  static_assert(vector_mask_matches<custom_element, 4, native::neon>());
  static_assert(std::same_as<native::mask<native::simd<custom_element, 4, native::neon>>,
    native::mask<native::simd<float, 4, native::neon>>>);
  static_assert(std::same_as<native::mask<native::simd<float, 4, native::neon>>,
    native::simd<native::mask32, 4, native::neon>>);
  static_assert(!has_mask<native::simd<float, 5, native::neon>>);
#endif
}
