// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/attributes.h"
#include "native/value_traits.h"
#include <array>
#include <concepts>
#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>

namespace wide {
  namespace detail {
    enum class family { leaf, std_array, legacy };
    template<class T> struct shape { static constexpr auto kind=family::leaf; };
    template<class T,std::size_t N> struct array_shape {
      static constexpr std::size_t size=N;
      template<std::size_t> using element_type=T;
    };
    template<class T,std::size_t N> struct shape<std::array<T,N>> : array_shape<T,N> {
      static constexpr auto kind=family::std_array;
    };
    template<class T> using shape_t=shape<std::remove_cvref_t<T>>;
    template<class T> inline constexpr bool is_array=
      shape_t<T>::kind==family::std_array || shape_t<T>::kind==family::legacy;
  }

  /// A homogeneous batch stored in a standard array.
  template<class T> concept pack=detail::shape_t<T>::kind==detail::family::std_array;

  namespace detail {
    template<std::size_t I,class P>
    native_inline constexpr decltype(auto) source_get(P&& value) noexcept {
      if constexpr (shape_t<P>::kind==family::legacy)
        return std::get<I>(std::forward<P>(value).registers);
      else return std::get<I>(std::forward<P>(value));
    }

    // Element adaptation does not add an outer dimension. All construction is
    // dependent, so this header may precede the complete SIMD definitions.
    template<class T> struct element_traits {};
    template<> struct element_traits<float> { using type=::native::simd<float,1,::native::scalar>; };
    template<class T,std::size_t N,::native::isa<> A>
    struct element_traits<::native::simd<T,N,A>> { using type=::native::simd<T,N,A>; };
    template<std::size_t N,::native::isa<> A>
    struct element_traits<::native::predicate<N,A>> { using type=::native::predicate<N,A>; };
    template<class T> using adapted_t=typename element_traits<std::remove_cvref_t<T>>::type;
    template<class T> concept adaptable=requires { typename adapted_t<T>; };
    template<class P> requires is_array<P> && adaptable<typename shape_t<P>::template element_type<0>>
    struct element_traits<P> {
      using type=std::array<adapted_t<typename shape_t<P>::template element_type<0>>,shape_t<P>::size>;
    };
    template<class T> struct promotion_traits {};
    template<adaptable T> struct promotion_traits<T> {
      using canonical_type=std::conditional_t<shape_t<T>::kind==family::leaf,
        std::array<adapted_t<T>,1>,adapted_t<T>>;
    };

    template<adaptable T> native_inline constexpr adapted_t<T> adapt_element(T&& value);
    template<class P,std::size_t... I>
    native_inline constexpr adapted_t<P> adapt_pack(P&& value,std::index_sequence<I...>) {
      return {{adapt_element(source_get<I>(std::forward<P>(value)))...}};
    }
    template<adaptable T>
    native_inline constexpr adapted_t<T> adapt_element(T&& value) {
      if constexpr (shape_t<T>::kind==family::leaf)
        return adapted_t<T>(std::forward<T>(value));
      else return adapt_pack(std::forward<T>(value),std::make_index_sequence<shape_t<T>::size>{});
    }
  }

  template<class T> struct traits : detail::promotion_traits<std::remove_cvref_t<T>> {};
  template<class T> using canonical_t=typename traits<std::remove_cvref_t<T>>::canonical_type;
  template<class T> concept promotable=requires { typename canonical_t<T>; };

  /// Own a standard array of SIMD leaves; a scalar or SIMD input has extent one.
  template<promotable T>
  native_inline constexpr canonical_t<T> promote(T&& value) {
    if constexpr (detail::shape_t<T>::kind==detail::family::leaf)
      return {{detail::adapt_element(std::forward<T>(value))}};
    else return detail::adapt_element(std::forward<T>(value));
  }

  namespace detail {
    template<class T> inline constexpr bool scalar_vector=false;
    template<class T> inline constexpr bool scalar_vector<::native::simd<T,1,::native::scalar>> = true;
    template<class T> inline constexpr bool scalar_mask=false;
    template<class T> requires (std::same_as<T,bool> || ::native::detail::is_mask_lane<T>)
    inline constexpr bool scalar_mask<::native::simd<T,1,::native::scalar>> = true;
    template<> inline constexpr bool scalar_mask<::native::predicate<1,::native::scalar>> = true;

    template<class Original,class P> consteval bool compatible_pack() {
      if constexpr (!pack<P>) return false;
      else if constexpr (shape_t<Original>::kind==family::leaf) return shape_t<P>::size==1;
      else if constexpr (shape_t<Original>::size!=shape_t<P>::size) return false;
      else {
        using E=typename shape_t<Original>::template element_type<0>;
        using R=typename shape_t<P>::template element_type<0>;
        if constexpr (shape_t<E>::kind==family::leaf) return true;
        else return compatible_pack<E,R>();
      }
    }

    template<class Original,class R> native_inline constexpr auto restore_element(R&& value);
    template<class Original,class P,std::size_t... I>
    native_inline constexpr auto restore_pack(P&& value,std::index_sequence<I...>) {
      using S=shape_t<Original>;
      using E=typename S::template element_type<0>;
      using V=typename shape_t<P>::template element_type<0>;
      using R=decltype(restore_element<E>(std::declval<V>()));
      if constexpr (S::kind==family::std_array)
        return std::array<R,S::size>{{restore_element<E>(std::get<I>(std::forward<P>(value)))...}};
      else return typename S::template rebind<R>{std::array<R,S::size>{{
        restore_element<E>(std::get<I>(std::forward<P>(value)))...}}};
    }
    template<class Original,class R>
    native_inline constexpr auto restore_element(R&& value) {
      if constexpr (shape_t<Original>::kind!=family::leaf)
        return restore_pack<Original>(std::forward<R>(value),
          std::make_index_sequence<shape_t<Original>::size>{});
      else if constexpr (std::same_as<std::remove_cvref_t<Original>,float> &&
                         scalar_mask<std::remove_cvref_t<R>>)
        return value.to_native()!=0;
      else if constexpr (std::same_as<std::remove_cvref_t<Original>,float> &&
                         scalar_vector<std::remove_cvref_t<R>>)
        return value.to_native();
      else return std::remove_cvref_t<R>(std::forward<R>(value));
    }
  }

  /// Restore the source shape while retaining transformed result element types.
  template<class Original,pack P> requires promotable<Original> &&
    (detail::compatible_pack<Original,P>())
  native_inline constexpr auto demote(P&& result) {
    if constexpr (detail::shape_t<Original>::kind==detail::family::leaf)
      return detail::restore_element<Original>(std::get<0>(std::forward<P>(result)));
    else return detail::restore_pack<Original>(std::forward<P>(result),
      std::make_index_sequence<detail::shape_t<Original>::size>{});
  }

  namespace detail {
    template<class From,class T> using copy_const_t=
      std::conditional_t<std::is_const_v<std::remove_reference_t<From>>,std::add_const_t<T>,T>;
    template<class From,class T> using copy_cv_t=
      std::conditional_t<std::is_volatile_v<std::remove_reference_t<From>>,
        std::add_volatile_t<copy_const_t<From,T>>,copy_const_t<From,T>>;
    template<class P> using element_argument_t=
      std::conditional_t<std::is_lvalue_reference_v<P>,
        copy_cv_t<P,typename shape_t<P>::template element_type<0>>&,
        copy_cv_t<P,typename shape_t<P>::template element_type<0>>&&>;
    template<class F,class... P> using mapped_t=
      std::remove_cvref_t<std::invoke_result_t<F&,element_argument_t<P>...>>;
    template<std::size_t I,class F,class... P>
    native_inline constexpr decltype(auto) map_element(F& function,P&&... value) {
      return std::invoke(function,std::get<I>(std::forward<P>(value))...);
    }
    template<class F,class P,class... Rest,std::size_t... I>
    consteval bool map_compatible(std::index_sequence<I...>) {
      if constexpr (!std::is_invocable_v<F&,element_argument_t<P>,element_argument_t<Rest>...>) return false;
      else {
        using R=mapped_t<F,P,Rest...>;
        if constexpr (!std::is_object_v<R>) return false;
        else return requires(F& function,P&& first,Rest&&... rest) {
          std::array<R,sizeof...(I)>{{map_element<I>(function,
            std::forward<P>(first),std::forward<Rest>(rest)...)...}};
        };
      }
    }
    template<class F,class P,class... Rest,std::size_t... I>
    native_inline constexpr auto map_pack(F&& function,std::index_sequence<I...>,P&& first,Rest&&... rest) {
      return std::array<mapped_t<F,P,Rest...>,sizeof...(I)>{{
        map_element<I>(function,std::forward<P>(first),std::forward<Rest>(rest)...)...}};
    }
  }

  /// Advance equal-length arrays through one operation; empty arrays invoke nothing.
  template<class F,pack P,pack... Rest>
    requires ((detail::shape_t<P>::size==detail::shape_t<Rest>::size) && ...) &&
      (detail::map_compatible<F,P,Rest...>(std::make_index_sequence<detail::shape_t<P>::size>{}))
  native_inline constexpr auto map(F&& function,P&& first,Rest&&... rest) {
    return detail::map_pack(std::forward<F>(function),
      std::make_index_sequence<detail::shape_t<P>::size>{},
      std::forward<P>(first),std::forward<Rest>(rest)...);
  }
}
