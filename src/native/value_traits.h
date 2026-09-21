// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/kernel_policies.h"
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <type_traits>

// These primaries belong to the global module fragment, including when wide
// needs their exact-type traits before the hub defines the specializations.
namespace native {
  template<class T,std::size_t N,isa<> Arch> struct simd;
  template<std::size_t N,isa<> Arch> struct predicate;
  template<class U> requires (std::is_unsigned_v<U> && !std::same_as<U,bool> && (sizeof(U)==1 || sizeof(U)==2 || sizeof(U)==4 || sizeof(U)==8))
  struct mask_lane;

  namespace detail {
    template<class T> inline constexpr bool is_mask_lane=false;
    template<class U> inline constexpr bool is_mask_lane<mask_lane<U>> = true;

    // Unknown and derived types retain their declared architecture. Only exact
    // built-in specializations have implementation requirements known here.
    template<class T,class=void> struct value_traits {
      static constexpr isa<> value=scalar;
      static constexpr bool known=false;
      static constexpr bool aggregate_default=false;
    };
    template<class T> requires arch<std::remove_cvref_t<decltype(T::architecture)>> &&
      requires { typename std::integral_constant<std::remove_cvref_t<decltype(T::architecture)>,T::architecture>; }
    struct value_traits<T,std::void_t<decltype(T::architecture)>> {
      static_assert(arch_family_v<std::remove_cvref_t<decltype(T::architecture)>> == target_arch,
        "value architecture must match the compiler target family");
      static constexpr isa<> value=T::architecture;
      static constexpr bool known=true;
      static constexpr bool aggregate_default=false;
    };
    template<class T,std::size_t N,isa<> A>
    struct value_traits<simd<T,N,A>> {
      static constexpr isa<> value=A;
      static constexpr bool known=true;
      static constexpr bool aggregate_default=false;
    };
    template<class T> concept ordinary_simd_element =
      std::same_as<T,float> || std::same_as<T,bool> || is_mask_lane<T> ||
      std::same_as<T,std::int8_t> || std::same_as<T,std::uint8_t> ||
      std::same_as<T,std::int16_t> || std::same_as<T,std::uint16_t> ||
      std::same_as<T,std::int32_t> || std::same_as<T,std::uint32_t> ||
      std::same_as<T,std::int64_t> || std::same_as<T,std::uint64_t>;
    template<class T,std::size_t N,isa<> A> requires ordinary_simd_element<T>
    struct value_traits<simd<T,N,A>> {
      static constexpr isa<> value=abi_lookup<A,raw_kernel_policies>::architecture;
      static constexpr bool known=true;
      // float's one-lane specialization has a +0 member initializer. Bypassing
      // an unattributed std::array constructor preserves both initialization forms.
      static constexpr bool aggregate_default=std::same_as<T,float> && N==1;
    };
    template<std::size_t N,isa<> A> struct value_traits<predicate<N,A>> {
      static constexpr isa<> value=abi_lookup<A,raw_kernel_policies>::architecture;
      static constexpr bool known=true;
      static constexpr bool aggregate_default=false;
    };
    template<class T> struct value_architecture : value_traits<std::remove_cv_t<T>> {};
    template<class T> inline constexpr isa<> value_architecture_v=value_architecture<T>::value;
  }
}
