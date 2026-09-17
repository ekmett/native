#pragma once

/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense */

#include <cstdint>
#include <type_traits>
#include <immintrin.h>
#include "attributes/common.hpp"
#include "numerics.hpp"
#include "types.hpp"

/// \cond
#ifdef __AVX512F__
#define IF512(x,y) x
#define ON512(...) __VA_ARGS__
#else
#define IF512(x,y) y
#define ON512(...)
#endif

#if defined(__AVX512VL__) && defined(__AVX512FP16__)
#define ON512FP16(...) __VA_ARGS__
#define IF512FP16(x,y) x
#else
#define ON512FP16(...)
#define IF512FP16(x,y) y
#endif
/// \endcond

namespace ein {
/// \addtogroup simd
/// \{

/// \brief largest simd register width supported on this platform in bytes
constexpr size_t max_simd_size
#ifdef __AVX512F__
  = 64;
#else
  = 32;
#endif

namespace detail {
  template <typename T>
  struct storage_type_impl {
    using type = T;
  };
  template <> struct storage_type_impl<bf16> { using type = __bf16; };
  template <> struct storage_type_impl<fp16> { using type = _Float16; };
}

/// \brief The type used to store \p T in a simd_data_t.
/// \hideinitializer \hideinlinesource
template <typename T>
using storage_type = typename detail::storage_type_impl<T>::type;

template <typename T>
concept has_storage_type = requires {
  typename storage_type<T>;
  requires sizeof(T) == sizeof(storage_type<T>);
  requires requires(T t, storage_type<T> s) {
    std::bit_cast<storage_type<T>>(t);
    std::bit_cast<T>(s);
  };
};

namespace detail {
  template <typename T> struct has_lifted_operations_impl : std::false_type {};
  template <> struct has_lifted_operations_impl<bf16> : std::true_type {};
  template <> struct has_lifted_operations_impl<fp16> : std::true_type {};
}

/// \brief Does this type have operations that semantically correct when lifted to the simd_data_t level?
/// \details If not, then this is merely a storage type when converted to simd_data_t.
/// \hideinitializer \hideinlinesource
template <typename T>
concept has_lifted_operations = detail::has_lifted_operations_impl<T>::value;

/// \brief \ref ein::simd_data_t<\p T,\p N> is defined
template <typename T, size_t N>
concept has_simd_type =
     has_storage_type<T>
  && std::is_pod_v<storage_type<T>>
  && std::is_pod_v<T>
  && one_of<sizeof(T),1,2,4,8>
  && one_of<sizeof(T)*N,16,32,64>
  && sizeof(T)*N <= max_simd_size;

// \brief unadulterated clang/gcc vector extension type
template <typename T, size_t N>
requires has_simd_type<T,N>
using simd_data_t = storage_type<T> __attribute__((__vector_size__(N*sizeof(storage_type<T>)),__aligned__(N*sizeof(storage_type<T>))));

/// \brief can we convert simd_data_t<U,N> -> simd_data_t<T,N> automatically using gcc vector extensions?
template <typename U, typename T, size_t N>
concept has_builtin_convertvector
      = has_simd_type<storage_type<U>,N>
     && has_simd_type<storage_type<T>,N>
     && has_lifted_operations<T>
     && has_lifted_operations<U>
     && requires (simd_data_t<storage_type<U>,N> x) {
          __builtin_convertvector(x,simd_data_t<storage_type<T>,N>);
        };

/// \brief is this type one of the types that is handed well automatically by clang/gcc vector extensions?
// \todo only add __fp16 and _Float16 on Sapphire Rapids
template <typename T>
concept simd_builtin = one_of_t<T,int8_t,uint8_t,int16_t,uint16_t,int32_t,uint32_t,float,double,__fp16,__bf16,_Float16>;

namespace detail {
template <size_t N> struct si {};
template <size_t N> struct ps {};
template <size_t N> struct pd {};

template <> struct si<128> { using type = __m128i; };
template <> struct ps<128> { using type = __m128; };
template <> struct pd<128> { using type = __m128d; };

template <> struct si<256> { using type = __m256i; };
template <> struct ps<256> { using type = __m256; };
template <> struct pd<256> { using type = __m256d; };

#ifdef __AVX512F__
template <> struct si<512> { using type = __m512i; };
template <> struct ps<512> { using type = __m512; };
template <> struct pd<512> { using type = __m512d; };
#endif

#if defined(__AVX512FP16__) && defined(__AVX512VL__)
template <size_t N> struct ph {};
template <> struct ph<128> { using type = __m128ph; };
template <> struct ph<256> { using type = __m256ph; };
template <> struct ph<512> { using type = __m512ph; };
#endif

template <typename T, size_t N> struct simd_intrinsic {};

template <size_t N>
struct simd_intrinsic<float,N> {
  using type = typename ps<N*sizeof(float)*8>::type;
};

template <size_t N>
struct simd_intrinsic<double,N> {
  using type = typename pd<N*sizeof(double)*8>::type;
};

#if defined(__AVX512FP16__) && defined(__AVX512VL__)
template <size_t N>
struct simd_intrinsic<__fp16,N> {
  using type = typename ph<N*sizeof(__fp16)*8>::type;
};

template <size_t N>
struct simd_intrinsic<_Float16,N> {
  using type = typename ph<N*sizeof(_Float16)*8>::type;
};

template <size_t N>
struct simd_intrinsic<ein::fp16,N> {
  using type = typename ph<N*sizeof(ein::fp16)*8>::type;
};
#endif

template <
#if defined(__AVX512FP16__) && defined(__AVX512VL__)
  not_one_of_t<float,double,__fp16,_Float16,ein::fp16> T,
#else
  not_one_of_t<float,double> T,
#endif
  size_t N
>
struct simd_intrinsic<T,N> {
  using type = typename ps<N*sizeof(T)*8>::type;
};
}

/// \brief Returns the Intel intrinsic type associated with a simd register full of \p N values of type \p T.
/// \details this can differ from the preferred type used by clang/gcc vector extensions.
/// \hideinitializer \hideinlinesource
template <typename T, size_t N>
requires has_simd_type<T,N>
using simd_intrinsic_t = typename detail::simd_intrinsic<T,N>::type;

/// \brief Do we want to use AVX512's notion of an _mmask8, _mmask16, _mmask32, or _mmask64 for masking operations
// involving \p N values of type \p T at a time. Currently I prefer to use it if available, and if the `_mmaskN` type
// matches the register size.
template <typename T, size_t N>
concept has_mmask
#if __AVX512F__
      = ((has_simd_type<T,N>) && (N >= 8));
#else
      = false;
#endif

#ifdef __AVX512F__
namespace {
template <size_t N> struct mmask {};
template <> struct mmask<8>  { using type = __mmask8; };
template <> struct mmask<16> { using type = __mmask16; };
template <> struct mmask<32> { using type = __mmask32; };
template <> struct mmask<64> { using type = __mmask64; };
}

/// If AVX512 is enabled returns the type of an n-bit mmask.
/// \hideinitializer \hideinlinesource
template <size_t N>
requires one_of<N,8,16,32,64>
using mmask_t
  = typename mmask<N>::type;
#endif

/// What type of mask should I use?
/// \hideinitializer
template <typename T, size_t N>
requires has_simd_type<T,N>
using simd_mask_t =
#ifdef __AVX512F__
  std::conditional_t<has_mmask<T,N>, mmask<N>, simd_intrinsic_t<T,N>>;
#else
  simd_intrinsic_t<T,N>;
#endif

ein_nodiscard ein_inline ein_const ein_artificial
__m128 cast_ps(__m128i a) noexcept { return _mm_castsi128_ps(a); }

ein_nodiscard ein_inline ein_const ein_artificial
__m128 cast_ps(__m128 a) noexcept { return a; }

ein_nodiscard ein_inline ein_const ein_artificial
__m256 cast_ps(__m256i a) noexcept { return _mm256_castsi256_ps(a); }

ein_nodiscard ein_inline ein_const ein_artificial
__m256 cast_ps(__m256 a) noexcept { return a; }

ein_nodiscard ein_inline ein_const ein_artificial
__m128d cast_pd(__m128i a) noexcept { return _mm_castsi128_pd(a); }

ein_nodiscard ein_inline ein_const ein_artificial
__m128d cast_pd(__m128d a) noexcept { return a; }

ein_nodiscard ein_inline ein_const ein_artificial
__m256d cast_pd(__m256i a) noexcept { return _mm256_castsi256_pd(a); }

ein_nodiscard ein_inline ein_const ein_artificial
__m256d cast_pd(__m256d a) noexcept { return a; }

ein_nodiscard ein_inline ein_const ein_artificial
__m128i cast_si(__m128 a) noexcept { return _mm_castps_si128(a); }

ein_nodiscard ein_inline ein_const ein_artificial
__m128i cast_si(__m128d a) noexcept { return _mm_castpd_si128(a); }

ein_nodiscard ein_inline ein_const ein_artificial
__m128i cast_si(__m128i a) noexcept { return a; }

ein_nodiscard ein_inline ein_const ein_artificial
__m256i cast_si(__m256 a) noexcept { return _mm256_castps_si256(a); }

ein_nodiscard ein_inline ein_const ein_artificial
__m256i cast_si(__m256d a) noexcept { return _mm256_castpd_si256(a); }

ein_nodiscard ein_inline ein_const ein_artificial
__m256i cast_si(__m256i a) noexcept { return a; }

#ifdef __AVX512F__

ein_nodiscard ein_inline ein_const ein_artificial
__m512 cast_ps(__m512i a) noexcept { return _mm512_castsi512_ps(a); }

ein_nodiscard ein_inline ein_const ein_artificial
__m512d cast_pd(__m512i a) noexcept { return _mm512_castsi512_pd(a); }

ein_nodiscard ein_inline ein_const ein_artificial
__m512 cast_ps(__m512 a) noexcept { return a; }

ein_nodiscard ein_inline ein_const ein_artificial
__m512d cast_pd(__m512d a) noexcept { return a; }

ein_nodiscard ein_inline ein_const ein_artificial
__m512i cast_si(__m512 a) noexcept { return _mm512_castps_si512(a); }

ein_nodiscard ein_inline ein_const ein_artificial
__m512i cast_si(__m512d a) noexcept { return _mm512_castpd_si512(a); }

ein_nodiscard ein_inline ein_const ein_artificial
__m512i cast_si(__m512i a) noexcept { return a; }

#if defined(__AVX512FP16__) && defined(__AVX512VL__)
__m128i cast_si(__m128ph a) noexcept { return _mm_castph_si128(a); }
__m128 cast_ps(__m128ph a) noexcept { return _mm_castph_ps(a); }
__m128d cast_pd(__m128ph a) noexcept { return _mm_castph_pd(a); }
__m128ph cast_ph(__m128ph a) noexcept { return a; }

__m256i cast_si(__m256ph a) noexcept { return _mm256_castph_si256(a); }
__m256 cast_ps(__m256ph a) noexcept { return _mm256_castph_ps(a); }
__m256d cast_pd(__m256ph a) noexcept { return _mm256_castph_pd(a); }
__m256ph cast_ph(__m256ph a) noexcept { return a; }

__m512i cast_si(__m512ph a) noexcept { return _mm512_castph_si512(a); }
__m512 cast_ps(__m512ph a) noexcept { return _mm512_castph_ps(a); }
__m512d cast_pd(__m512ph a) noexcept { return _mm512_castph_pd(a); }
__m512ph cast_ph(__m512ph a) noexcept { return a; }

__m128ph cast_ph(__m128 a) noexcept { return _mm_castps_ph(a); }
__m256ph cast_ph(__m256 a) noexcept { return _mm256_castps_ph(a); }
__m512ph cast_ph(__m512 a) noexcept { return _mm512_castps_ph(a); }

__m128ph cast_ph(__m128i a) noexcept { return _mm_castsi128_ph(a); }
__m256ph cast_ph(__m256i a) noexcept { return _mm256_castsi256_ph(a); }
__m512ph cast_ph(__m512i a) noexcept { return _mm512_castsi512_ph(a); }

__m128ph cast_ph(__m128d a) noexcept { return _mm_castpd_ph(a); }
__m256ph cast_ph(__m256d a) noexcept { return _mm256_castpd_ph(a); }
__m512ph cast_ph(__m512d a) noexcept { return _mm512_castpd_ph(a); }
#endif // __AVX512FP16__ && __AVX512VL__

#endif // __AVX512F__

/// \}
} // namespace ein

#if defined(EIN_TESTING) || defined(EIN_TESTING_SIMD_DATA)
#include <string_view>
#include "types.hpp"

TEMPLATE_TEST_CASE("simd_data","[simd_data]",int8_t,uint8_t,int16_t,uint16_t,int32_t,uint32_t,int64_t,uint64_t,float,double) {
  using namespace ein;

  constexpr size_t N128 = 16/sizeof(TestType);
  constexpr size_t N256 = 32/sizeof(TestType);
#ifdef __AVX512F__
  constexpr size_t N512 = 64/sizeof(TestType);
#endif
  SECTION("has_simd_data") {
    STATIC_REQUIRE(has_simd_type<TestType,N128>);
    STATIC_REQUIRE(has_simd_type<TestType,N256>);
#ifdef __AVX512F__
    STATIC_REQUIRE(has_simd_type<TestType,N512>);
#endif
  }
  SECTION("simd_data_t") {
    using d128 = simd_data_t<TestType,N128>;
    using d256 = simd_data_t<TestType,N256>;
#ifdef __AVX512F__
    using d512 = simd_data_t<TestType,N512>;
#endif
    SECTION("has the right size") {
      CHECK(sizeof(d128) == 16);
      CHECK(sizeof(d256) == 32);
#ifdef __AVX512F__
      CHECK(sizeof(d512) == 64);
#endif
    }
    d128 x128{TestType{}};
    d256 x256{TestType{}};
#ifdef __AVX512F__
    d512 x512{TestType{}};
#endif
    SECTION("can be indexed at the right type") {
      STATIC_REQUIRE(std::is_same_v<std::remove_cvref_t<decltype(x128[0])>, storage_type<TestType>>);
      STATIC_REQUIRE(std::is_same_v<std::remove_cvref_t<decltype(x256[0])>, storage_type<TestType>>);
#ifdef __AVX512F__
      STATIC_REQUIRE(std::is_same_v<std::remove_cvref_t<decltype(x512[0])>, storage_type<TestType>>);
#endif
    }
    SECTION("can be indexed with the right value") {
      CHECK(x128[0] == storage_type<TestType>{});
      CHECK(x256[0] == storage_type<TestType>{});
#ifdef __AVX512F__
      CHECK(x512[0] == storage_type<TestType>{});
#endif
    }
  }

  SECTION("simd_intrinsic_t") {
    using t128 = simd_intrinsic_t<TestType,N128>;
    using t256 = simd_intrinsic_t<TestType,N256>;
#ifdef __AVX512F__
    using t512 = simd_intrinsic_t<TestType,N512>;
#endif

    [[maybe_unused]] t128 x128{};
    [[maybe_unused]] t256 x256{};
#ifdef __AVX512F__
    [[maybe_unused]] t512 x512{};
#endif

//     SECTION("cast_ps") {
//       CHECK(sizeof(cast_ps(x128)) == sizeof(x128));
//       CHECK(sizeof(cast_ps(x256)) == sizeof(x256));
// #ifdef __AVX512F__
//       CHECK(sizeof(cast_ps(x512)) == sizeof(x512));
// #endif
//     }

//     SECTION("cast_si") {
//       CHECK(sizeof(cast_si(x128)) == sizeof(x128));
//       CHECK(sizeof(cast_si(x256)) == sizeof(x256));
// #ifdef __AVX512F__
//       CHECK(sizeof(cast_si(x512)) == sizeof(x512));
// #endif
//     }

//     SECTION("cast_pd") {
//       CHECK(sizeof(cast_pd(x128)) == sizeof(x128));
//       CHECK(sizeof(cast_pd(x256)) == sizeof(x256));
// #ifdef __AVX512F__
//       CHECK(sizeof(cast_pd(x512)) == sizeof(x512));
// #endif
//     }
   }
}

#endif
