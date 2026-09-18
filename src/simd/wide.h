// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "simd/attributes.h"
#include "simd/isa.h"

#include <array>
#include <concepts>
#include <cmath>
#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace simd {
  template<class T, std::size_t N> struct wide;
  namespace detail {
    template<class T> struct wide_traits {
      using element_type = T;
      static constexpr bool value = false;
      static constexpr std::size_t size = 0;
    };
    template<class T, std::size_t N> struct wide_traits<wide<T, N>> {
      using element_type = T;
      static constexpr bool value = true;
      static constexpr std::size_t size = N;
    };
  }

  namespace detail {
    template<class T, class = void> struct wide_features {
      static constexpr feature_set value = 0;
    };
    template<class T> struct wide_features<T,std::void_t<typename T::architecture>> {
      static constexpr feature_set value = [] {
        if constexpr (architecture<typename T::architecture>) return T::architecture::features;
        else return feature_set{0};
      }();
    };
    template<class T,std::size_t N> struct wide_features<wide<T,N>> : wide_features<T> {};
    template<class T,std::size_t N> struct wide_features<std::array<T,N>> : wide_features<T> {};
    // Only operands with a SIMD architecture participate. Scalar/custom values
    // retain the generic ADL path. Arrays and nested packs contribute their
    // element features; mixed conversions use the union of both endpoints.
    constexpr unsigned wide_family(feature_set bits) {
      constexpr auto base512 = avx2::features | feature::avx512f | feature::avx512dq;
      if ((bits & base512) == base512)
        return 2 + ((bits & feature_set(feature::avx512bw)) ? 1 : 0) +
          ((bits & feature_set(feature::avx512vl)) ? 2 : 0) +
          ((bits & feature_set(feature::avx512_bf16)) ? 4 : 0) +
          ((bits & feature_set(feature::avx512_fp16)) ? 8 : 0);
      if ((bits & avx2::features) == avx2::features) return 1;
      if (bits & feature_set(feature::neon))
        return 20 + ((bits & feature_set(feature::neon_bf16)) ? 1 : 0) +
          ((bits & feature_set(feature::neon_fp16)) ? 2 : 0);
      return 0;
    }
    template<unsigned Family,class... T> concept wide_family_is =
      wide_family((feature_set{0} | ... | wide_features<std::remove_cvref_t<T>>::value)) == Family;
  }
  /** \defgroup wide_values Wide register packs
   * A fixed number of independent elements, usually native registers. Operations
   * preserve the element library's contracts. Batch-aware math calls one ADL
   * array kernel when available; otherwise it applies the element operation.
   * This does not repartition a pack or choose a different architecture.
   * \snippet api.cc wide_values
   */
  /** \ingroup wide_values
   * \brief Own `N` values of `T`, including an empty pack when `N == 0`.
   * `registers` is the underlying array. Tuple access and structured bindings
   * refer to the actual elements. Value initialization follows `T`'s rules;
   * no general floating-point or zero-initialization policy is added here.
   */
  template<class T, std::size_t N>
  struct wide {
    // Packed library aggregates can underalign arrays of native mask types.
    alignas(T) std::array<T, N> registers;

    /// Default-initialize the underlying array. Use `wide{}` for value initialization.
    constexpr wide() = default;
    /// Access element `I`, preserving constness and the value category of the pack.
    template<std::size_t I> requires (I < N)
    simd_nodiscard simd_inline constexpr T & get() & noexcept simd_lifetimebound { return registers[I]; }
    /// Access element `I`, preserving constness and the value category of the pack.
    template<std::size_t I> requires (I < N)
    simd_nodiscard simd_inline constexpr T const & get() const & noexcept simd_lifetimebound { return registers[I]; }
    /// Access element `I`, preserving constness and the value category of the pack.
    template<std::size_t I> requires (I < N)
    simd_nodiscard simd_inline constexpr T && get() && noexcept simd_lifetimebound { return std::move(registers[I]); }
    /// Access element `I`, preserving constness and the value category of the pack.
    template<std::size_t I> requires (I < N)
    simd_nodiscard simd_inline constexpr T const && get() const && noexcept simd_lifetimebound { return std::move(registers[I]); }

  private:
    struct conversion_tag {};
#define SIMD_WIDE_FAMILY 0
#define SIMD_WIDE_DETAIL detail::wide_generic_detail
#define SIMD_WIDE_TARGET
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 1
#define SIMD_WIDE_DETAIL detail::wide_avx2_detail
#define SIMD_WIDE_TARGET __attribute__((target("avx2,fma,bmi2")))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 2
#define SIMD_WIDE_DETAIL detail::wide_avx512_base_detail
#define SIMD_WIDE_TARGET __attribute__((target("avx2,fma,bmi2,avx512f,avx512dq")))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 3
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_detail
#define SIMD_WIDE_TARGET __attribute__((target("avx2,fma,bmi2,avx512f,avx512dq,avx512bw")))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 4
#define SIMD_WIDE_DETAIL detail::wide_avx512_vl_detail
#define SIMD_WIDE_TARGET __attribute__((target("avx2,fma,bmi2,avx512f,avx512dq,avx512vl")))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 5
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_vl_detail
#define SIMD_WIDE_TARGET __attribute__((target("avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512vl")))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 7
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_bf16_detail
#define SIMD_WIDE_TARGET __attribute__((target("avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512bf16")))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 9
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_vl_bf16_detail
#define SIMD_WIDE_TARGET __attribute__((target("avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512vl,avx512bf16")))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 11
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_fp16_detail
#define SIMD_WIDE_TARGET __attribute__((target("avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512fp16")))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 13
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_vl_fp16_detail
#define SIMD_WIDE_TARGET __attribute__((target("avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512vl,avx512fp16")))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 15
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_bf16_fp16_detail
#define SIMD_WIDE_TARGET __attribute__((target("avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512bf16,avx512fp16")))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 17
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_vl_bf16_fp16_detail
#define SIMD_WIDE_TARGET __attribute__((target("avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512vl,avx512bf16,avx512fp16")))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define SIMD_WIDE_FAMILY 20
#define SIMD_WIDE_DETAIL detail::wide_neon_detail
#define SIMD_WIDE_TARGET __attribute__((target("neon")))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define SIMD_WIDE_FAMILY 21
#define SIMD_WIDE_DETAIL detail::wide_neon_bf16_detail
#define SIMD_WIDE_TARGET __attribute__((target("neon,bf16")))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define SIMD_WIDE_FAMILY 22
#define SIMD_WIDE_DETAIL detail::wide_neon_fp16_detail
#define SIMD_WIDE_TARGET __attribute__((target("neon,fullfp16")))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define SIMD_WIDE_FAMILY 23
#define SIMD_WIDE_DETAIL detail::wide_neon_half_detail
#define SIMD_WIDE_TARGET __attribute__((target("neon,fullfp16,bf16")))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

  };
  /// Deduce the common element type and the number of homogeneous constructor arguments.
  template<class T, class... U> requires (std::same_as<T,U> && ...)
  wide(T,U...) -> wide<T,1 + sizeof...(U)>;

#define SIMD_WIDE_FAMILY 0
#define SIMD_WIDE_DETAIL detail::wide_generic_detail
#include "simd/wide_operations.h"
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 1
#define SIMD_WIDE_DETAIL detail::wide_avx2_detail
#pragma clang attribute push(__attribute__((target("avx2,fma,bmi2"))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 2
#define SIMD_WIDE_DETAIL detail::wide_avx512_base_detail
#pragma clang attribute push(__attribute__((target("avx2,fma,bmi2,avx512f,avx512dq"))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 3
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_detail
#pragma clang attribute push(__attribute__((target("avx2,fma,bmi2,avx512f,avx512dq,avx512bw"))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 4
#define SIMD_WIDE_DETAIL detail::wide_avx512_vl_detail
#pragma clang attribute push(__attribute__((target("avx2,fma,bmi2,avx512f,avx512dq,avx512vl"))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 5
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_vl_detail
#pragma clang attribute push(__attribute__((target("avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512vl"))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 7
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_bf16_detail
#pragma clang attribute push(__attribute__((target("avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512bf16"))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 9
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_vl_bf16_detail
#pragma clang attribute push(__attribute__((target("avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512vl,avx512bf16"))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 11
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_fp16_detail
#pragma clang attribute push(__attribute__((target("avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512fp16"))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 13
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_vl_fp16_detail
#pragma clang attribute push(__attribute__((target("avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512vl,avx512fp16"))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 15
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_bf16_fp16_detail
#pragma clang attribute push(__attribute__((target("avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512bf16,avx512fp16"))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 17
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_vl_bf16_fp16_detail
#pragma clang attribute push(__attribute__((target("avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512vl,avx512bf16,avx512fp16"))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define SIMD_WIDE_FAMILY 20
#define SIMD_WIDE_DETAIL detail::wide_neon_detail
#pragma clang attribute push(__attribute__((target("neon"))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define SIMD_WIDE_FAMILY 21
#define SIMD_WIDE_DETAIL detail::wide_neon_bf16_detail
#pragma clang attribute push(__attribute__((target("neon,bf16"))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define SIMD_WIDE_FAMILY 22
#define SIMD_WIDE_DETAIL detail::wide_neon_fp16_detail
#pragma clang attribute push(__attribute__((target("neon,fullfp16"))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define SIMD_WIDE_FAMILY 23
#define SIMD_WIDE_DETAIL detail::wide_neon_half_detail
#pragma clang attribute push(__attribute__((target("neon,fullfp16,bf16"))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

}
namespace std {
  // Structured binding lookup needs the primary names visible even when the
  // importer has not separately included <tuple>.
  using ::std::tuple_size;
  using ::std::tuple_element;
  using ::std::get;
  template<class T, size_t N>
  struct tuple_size<simd::wide<T, N>> : integral_constant<size_t, N> {};
  template<size_t I, class T, size_t N>
  struct tuple_element<I, simd::wide<T, N>> : tuple_element<I, array<T, N>> {};
}
