// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "simd/attributes.h"
#include "simd/isa.h"
#include "simd/kernel_policies.h"

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
    template<class T,class U> struct wide_features<std::pair<T,U>> {
      static constexpr feature_set value=wide_features<std::remove_cvref_t<T>>::value|
        wide_features<std::remove_cvref_t<U>>::value;
    };
    // Only operands with a SIMD architecture participate. Scalar/custom values
    // retain the generic ADL path. Arrays and nested packs contribute their
    // element features; mixed conversions use the union of both endpoints.
    template<unsigned Scope,class... T> consteval bool wide_kernel_matches() {
      constexpr auto bits=(feature_set{0} | ... | wide_features<std::remove_cvref_t<T>>::value);
      // A custom cross-host conversion cannot form an isa tag. Preserve the
      // previous operand-union preference for its x86 declaration scope.
      constexpr auto selected=(bits&x86_features) && (bits&arm_features)
        ? ((bits&avx2::features)==avx2::features ? bits&x86_features : bits&arm_features) : bits;
      return requires_abi<isa<selected>,wide_kernel_policies,wide_scope_index<Scope>()>;
    }
    template<unsigned Scope,class... T> concept wide_family_is=wide_kernel_matches<Scope,T...>();
    // MSVC's empty array can own a dummy T with a nontrivial constructor.
    // Aggregate-initialize that storage in T's target scope, bypassing the
    // library's unattributed implicit array constructor. Leave trivial/deleted
    // defaults and every nonempty pack on the original defaulted path.
    template<class T,std::size_t N> inline constexpr bool wide_empty_default =
      N == 0 && std::is_default_constructible_v<T> &&
      !std::is_trivially_default_constructible_v<T> &&
      std::is_default_constructible_v<std::array<T,N>> &&
      !std::is_trivially_default_constructible_v<std::array<T,N>>;
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
    constexpr wide() requires (!detail::wide_empty_default<T,N>) = default;
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
#define SIMD_WIDE_TARGET __attribute__((target(SIMD_KERNEL_TARGET_1)))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 2
#define SIMD_WIDE_DETAIL detail::wide_avx512_base_detail
#define SIMD_WIDE_TARGET __attribute__((target(SIMD_KERNEL_TARGET_2)))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 3
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_detail
#define SIMD_WIDE_TARGET __attribute__((target(SIMD_KERNEL_TARGET_3)))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 4
#define SIMD_WIDE_DETAIL detail::wide_avx512_vl_detail
#define SIMD_WIDE_TARGET __attribute__((target(SIMD_KERNEL_TARGET_4)))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 5
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_vl_detail
#define SIMD_WIDE_TARGET __attribute__((target(SIMD_KERNEL_TARGET_5)))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 7
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_bf16_detail
#define SIMD_WIDE_TARGET __attribute__((target(SIMD_KERNEL_TARGET_7)))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 9
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_vl_bf16_detail
#define SIMD_WIDE_TARGET __attribute__((target(SIMD_KERNEL_TARGET_9)))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 11
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_fp16_detail
#define SIMD_WIDE_TARGET __attribute__((target(SIMD_KERNEL_TARGET_11)))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 13
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_vl_fp16_detail
#define SIMD_WIDE_TARGET __attribute__((target(SIMD_KERNEL_TARGET_13)))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 15
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_bf16_fp16_detail
#define SIMD_WIDE_TARGET __attribute__((target(SIMD_KERNEL_TARGET_15)))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 17
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_vl_bf16_fp16_detail
#define SIMD_WIDE_TARGET __attribute__((target(SIMD_KERNEL_TARGET_17)))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define SIMD_WIDE_FAMILY 20
#define SIMD_WIDE_DETAIL detail::wide_neon_detail
#define SIMD_WIDE_TARGET __attribute__((target(SIMD_KERNEL_TARGET_20)))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define SIMD_WIDE_FAMILY 21
#define SIMD_WIDE_DETAIL detail::wide_neon_bf16_detail
#define SIMD_WIDE_TARGET __attribute__((target(SIMD_KERNEL_TARGET_21)))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define SIMD_WIDE_FAMILY 22
#define SIMD_WIDE_DETAIL detail::wide_neon_fp16_detail
#define SIMD_WIDE_TARGET __attribute__((target(SIMD_KERNEL_TARGET_22)))
#include "simd/wide_members.h"
#undef SIMD_WIDE_TARGET
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define SIMD_WIDE_FAMILY 23
#define SIMD_WIDE_DETAIL detail::wide_neon_half_detail
#define SIMD_WIDE_TARGET __attribute__((target(SIMD_KERNEL_TARGET_23)))
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
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_1))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 2
#define SIMD_WIDE_DETAIL detail::wide_avx512_base_detail
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_2))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 3
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_detail
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_3))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 4
#define SIMD_WIDE_DETAIL detail::wide_avx512_vl_detail
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_4))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 5
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_vl_detail
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_5))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 7
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_bf16_detail
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_7))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 9
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_vl_bf16_detail
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_9))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 11
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_fp16_detail
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_11))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 13
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_vl_fp16_detail
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_13))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 15
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_bf16_fp16_detail
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_15))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_WIDE_FAMILY 17
#define SIMD_WIDE_DETAIL detail::wide_avx512_bw_vl_bf16_fp16_detail
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_17))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define SIMD_WIDE_FAMILY 20
#define SIMD_WIDE_DETAIL detail::wide_neon_detail
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_20))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define SIMD_WIDE_FAMILY 21
#define SIMD_WIDE_DETAIL detail::wide_neon_bf16_detail
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_21))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define SIMD_WIDE_FAMILY 22
#define SIMD_WIDE_DETAIL detail::wide_neon_fp16_detail
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_22))), apply_to=function)
#include "simd/wide_operations.h"
#pragma clang attribute pop
#undef SIMD_WIDE_DETAIL
#undef SIMD_WIDE_FAMILY
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define SIMD_WIDE_FAMILY 23
#define SIMD_WIDE_DETAIL detail::wide_neon_half_detail
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_23))), apply_to=function)
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
