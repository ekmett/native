// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/attributes.h"
#include "native/isa.h"
#include "native/value_traits.h"
#include "native/wide_pack.h"

#include <array>
#include <concepts>
#include <cmath>
#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace native {
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
      static constexpr isa<> value=scalar;
    };
    template<class T> struct wide_features<T,std::void_t<decltype(T::architecture)>> {
      static constexpr isa<> value=value_architecture_v<T>;
    };
    template<class T,std::size_t N> struct wide_features<wide<T,N>> : wide_features<T> {};
    template<class T,std::size_t N> struct wide_features<std::array<T,N>> : wide_features<T> {};
    template<class T,class U> struct wide_features<std::pair<T,U>> {
      static constexpr isa<> value=wide_features<std::remove_cvref_t<T>>::value &
        wide_features<std::remove_cvref_t<U>>::value;
    };
    // Only operands with a SIMD architecture participate. Scalar/custom values
    // retain the generic ADL path. Arrays and nested packs contribute their
    // element features; mixed conversions use the union of both endpoints.
    template<class... T> inline constexpr auto wide_target=[] {
      constexpr isa<> bits=(scalar & ... & wide_features<std::remove_cvref_t<T>>::value);
      constexpr auto index=abi_lookup<bits,wide_kernel_policies>::index;
      static_assert(index>=0,"wide operands require an available host declaration scope");
      return host_kernel_profiles::indices[index];
    }();
    template<class T> inline constexpr bool wide_equivalent_default=
      value_traits<std::remove_cv_t<T>>::aggregate_default;
    // MSVC's array can own a T with an attributed nontrivial constructor,
    // including a dummy element at N == 0.
    // Aggregate-initialize that storage in T's target scope, bypassing the
    // library's unattributed implicit array constructor. Leave trivial/deleted
    // defaults on the original defaulted path. Nonempty packs opt in only when
    // their concrete element guarantees equivalent default/value initialization.
    template<class T,std::size_t N> inline constexpr bool wide_target_default =
      (N == 0 || wide_equivalent_default<T>) && std::is_default_constructible_v<T> &&
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
    constexpr wide() requires (!detail::wide_target_default<T,N>) = default;
    /// Access element `I`, preserving constness and the value category of the pack.
    template<std::size_t I> requires (I < N)
    native_nodiscard native_inline constexpr T & get() & noexcept native_lifetimebound { return registers[I]; }
    /// Access element `I`, preserving constness and the value category of the pack.
    template<std::size_t I> requires (I < N)
    native_nodiscard native_inline constexpr T const & get() const & noexcept native_lifetimebound { return registers[I]; }
    /// Access element `I`, preserving constness and the value category of the pack.
    template<std::size_t I> requires (I < N)
    native_nodiscard native_inline constexpr T && get() && noexcept native_lifetimebound { return std::move(registers[I]); }
    /// Access element `I`, preserving constness and the value category of the pack.
    template<std::size_t I> requires (I < N)
    native_nodiscard native_inline constexpr T const && get() const && noexcept native_lifetimebound { return std::move(registers[I]); }

  private:
    struct conversion_tag {};
#define NATIVE_WIDE_INDEX 15
#define NATIVE_WIDE_DETAIL detail::wide_generic_detail
#define NATIVE_WIDE_TARGET
#include "native/wide_members.h"
#undef NATIVE_WIDE_TARGET
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_WIDE_INDEX 10
#define NATIVE_WIDE_DETAIL detail::wide_avx2_detail
#define NATIVE_WIDE_TARGET __attribute__((target(NATIVE_KERNEL_TARGET_1)))
#include "native/wide_members.h"
#undef NATIVE_WIDE_TARGET
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_WIDE_INDEX 9
#define NATIVE_WIDE_DETAIL detail::wide_avx512_base_detail
#define NATIVE_WIDE_TARGET __attribute__((target(NATIVE_KERNEL_TARGET_2)))
#include "native/wide_members.h"
#undef NATIVE_WIDE_TARGET
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_WIDE_INDEX 7
#define NATIVE_WIDE_DETAIL detail::wide_avx512_bw_detail
#define NATIVE_WIDE_TARGET __attribute__((target(NATIVE_KERNEL_TARGET_3)))
#include "native/wide_members.h"
#undef NATIVE_WIDE_TARGET
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_WIDE_INDEX 8
#define NATIVE_WIDE_DETAIL detail::wide_avx512_vl_detail
#define NATIVE_WIDE_TARGET __attribute__((target(NATIVE_KERNEL_TARGET_4)))
#include "native/wide_members.h"
#undef NATIVE_WIDE_TARGET
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_WIDE_INDEX 3
#define NATIVE_WIDE_DETAIL detail::wide_avx512_bw_vl_detail
#define NATIVE_WIDE_TARGET __attribute__((target(NATIVE_KERNEL_TARGET_5)))
#include "native/wide_members.h"
#undef NATIVE_WIDE_TARGET
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_WIDE_INDEX 5
#define NATIVE_WIDE_DETAIL detail::wide_avx512_bw_bf16_detail
#define NATIVE_WIDE_TARGET __attribute__((target(NATIVE_KERNEL_TARGET_7)))
#include "native/wide_members.h"
#undef NATIVE_WIDE_TARGET
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_WIDE_INDEX 1
#define NATIVE_WIDE_DETAIL detail::wide_avx512_bw_vl_bf16_detail
#define NATIVE_WIDE_TARGET __attribute__((target(NATIVE_KERNEL_TARGET_9)))
#include "native/wide_members.h"
#undef NATIVE_WIDE_TARGET
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_WIDE_INDEX 6
#define NATIVE_WIDE_DETAIL detail::wide_avx512_bw_fp16_detail
#define NATIVE_WIDE_TARGET __attribute__((target(NATIVE_KERNEL_TARGET_11)))
#include "native/wide_members.h"
#undef NATIVE_WIDE_TARGET
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_WIDE_INDEX 2
#define NATIVE_WIDE_DETAIL detail::wide_avx512_bw_vl_fp16_detail
#define NATIVE_WIDE_TARGET __attribute__((target(NATIVE_KERNEL_TARGET_13)))
#include "native/wide_members.h"
#undef NATIVE_WIDE_TARGET
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_WIDE_INDEX 4
#define NATIVE_WIDE_DETAIL detail::wide_avx512_bw_bf16_fp16_detail
#define NATIVE_WIDE_TARGET __attribute__((target(NATIVE_KERNEL_TARGET_15)))
#include "native/wide_members.h"
#undef NATIVE_WIDE_TARGET
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_WIDE_INDEX 0
#define NATIVE_WIDE_DETAIL detail::wide_avx512_bw_vl_bf16_fp16_detail
#define NATIVE_WIDE_TARGET __attribute__((target(NATIVE_KERNEL_TARGET_17)))
#include "native/wide_members.h"
#undef NATIVE_WIDE_TARGET
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define NATIVE_WIDE_INDEX 14
#define NATIVE_WIDE_DETAIL detail::wide_neon_detail
#define NATIVE_WIDE_TARGET __attribute__((target(NATIVE_KERNEL_TARGET_20)))
#include "native/wide_members.h"
#undef NATIVE_WIDE_TARGET
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define NATIVE_WIDE_INDEX 12
#define NATIVE_WIDE_DETAIL detail::wide_neon_bf16_detail
#define NATIVE_WIDE_TARGET __attribute__((target(NATIVE_KERNEL_TARGET_21)))
#include "native/wide_members.h"
#undef NATIVE_WIDE_TARGET
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define NATIVE_WIDE_INDEX 13
#define NATIVE_WIDE_DETAIL detail::wide_neon_fp16_detail
#define NATIVE_WIDE_TARGET __attribute__((target(NATIVE_KERNEL_TARGET_22)))
#include "native/wide_members.h"
#undef NATIVE_WIDE_TARGET
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define NATIVE_WIDE_INDEX 11
#define NATIVE_WIDE_DETAIL detail::wide_neon_half_detail
#define NATIVE_WIDE_TARGET __attribute__((target(NATIVE_KERNEL_TARGET_23)))
#include "native/wide_members.h"
#undef NATIVE_WIDE_TARGET
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if NATIVE_HOST_WASM
#define NATIVE_WIDE_INDEX 16
#define NATIVE_WIDE_DETAIL detail::wide_wasm_detail
#define NATIVE_WIDE_TARGET __attribute__((target(NATIVE_KERNEL_TARGET_WASM)))
#include "native/wide_members.h"
#undef NATIVE_WIDE_TARGET
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

  };
  /// Deduce the common element type and the number of homogeneous constructor arguments.
  template<class T, class... U> requires (std::same_as<T,U> && ...)
  wide(T,U...) -> wide<T,1 + sizeof...(U)>;

#define NATIVE_WIDE_INDEX 15
#define NATIVE_WIDE_DETAIL detail::wide_generic_detail
#include "native/wide_operations.h"
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_WIDE_INDEX 10
#define NATIVE_WIDE_DETAIL detail::wide_avx2_detail
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_1))), apply_to=function)
#include "native/wide_operations.h"
#pragma clang attribute pop
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_WIDE_INDEX 9
#define NATIVE_WIDE_DETAIL detail::wide_avx512_base_detail
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_2))), apply_to=function)
#include "native/wide_operations.h"
#pragma clang attribute pop
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_WIDE_INDEX 7
#define NATIVE_WIDE_DETAIL detail::wide_avx512_bw_detail
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_3))), apply_to=function)
#include "native/wide_operations.h"
#pragma clang attribute pop
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_WIDE_INDEX 8
#define NATIVE_WIDE_DETAIL detail::wide_avx512_vl_detail
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_4))), apply_to=function)
#include "native/wide_operations.h"
#pragma clang attribute pop
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_WIDE_INDEX 3
#define NATIVE_WIDE_DETAIL detail::wide_avx512_bw_vl_detail
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_5))), apply_to=function)
#include "native/wide_operations.h"
#pragma clang attribute pop
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_WIDE_INDEX 5
#define NATIVE_WIDE_DETAIL detail::wide_avx512_bw_bf16_detail
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_7))), apply_to=function)
#include "native/wide_operations.h"
#pragma clang attribute pop
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_WIDE_INDEX 1
#define NATIVE_WIDE_DETAIL detail::wide_avx512_bw_vl_bf16_detail
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_9))), apply_to=function)
#include "native/wide_operations.h"
#pragma clang attribute pop
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_WIDE_INDEX 6
#define NATIVE_WIDE_DETAIL detail::wide_avx512_bw_fp16_detail
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_11))), apply_to=function)
#include "native/wide_operations.h"
#pragma clang attribute pop
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_WIDE_INDEX 2
#define NATIVE_WIDE_DETAIL detail::wide_avx512_bw_vl_fp16_detail
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_13))), apply_to=function)
#include "native/wide_operations.h"
#pragma clang attribute pop
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_WIDE_INDEX 4
#define NATIVE_WIDE_DETAIL detail::wide_avx512_bw_bf16_fp16_detail
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_15))), apply_to=function)
#include "native/wide_operations.h"
#pragma clang attribute pop
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_WIDE_INDEX 0
#define NATIVE_WIDE_DETAIL detail::wide_avx512_bw_vl_bf16_fp16_detail
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_17))), apply_to=function)
#include "native/wide_operations.h"
#pragma clang attribute pop
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define NATIVE_WIDE_INDEX 14
#define NATIVE_WIDE_DETAIL detail::wide_neon_detail
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_20))), apply_to=function)
#include "native/wide_operations.h"
#pragma clang attribute pop
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define NATIVE_WIDE_INDEX 12
#define NATIVE_WIDE_DETAIL detail::wide_neon_bf16_detail
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_21))), apply_to=function)
#include "native/wide_operations.h"
#pragma clang attribute pop
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define NATIVE_WIDE_INDEX 13
#define NATIVE_WIDE_DETAIL detail::wide_neon_fp16_detail
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_22))), apply_to=function)
#include "native/wide_operations.h"
#pragma clang attribute pop
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define NATIVE_WIDE_INDEX 11
#define NATIVE_WIDE_DETAIL detail::wide_neon_half_detail
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_23))), apply_to=function)
#include "native/wide_operations.h"
#pragma clang attribute pop
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

#if NATIVE_HOST_WASM
#define NATIVE_WIDE_INDEX 16
#define NATIVE_WIDE_DETAIL detail::wide_wasm_detail
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_WASM))), apply_to=function)
#include "native/wide_operations.h"
#pragma clang attribute pop
#undef NATIVE_WIDE_DETAIL
#undef NATIVE_WIDE_INDEX
#endif

}
namespace std {
  // Structured binding lookup needs the primary names visible even when the
  // importer has not separately included <tuple>.
  using ::std::tuple_size;
  using ::std::tuple_element;
  using ::std::get;
  template<class T, size_t N>
  struct tuple_size<::native::wide<T, N>> : integral_constant<size_t, N> {};
  template<size_t I, class T, size_t N>
  struct tuple_element<I, ::native::wide<T, N>> : tuple_element<I, array<T, N>> {};
}
namespace wide::detail {
  // The legacy class belongs to native.wide. Specialize its adapter here rather
  // than forward-declaring that class in a global module fragment.
  template<class T,std::size_t N> struct shape<::native::wide<T,N>> : array_shape<T,N> {
    static constexpr auto kind=family::legacy;
    template<class R> using rebind=::native::wide<R,N>;
  };
}
