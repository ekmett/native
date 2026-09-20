// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "simd/attributes.h"
#include "simd/value_traits.h"
#include <array>
#include <concepts>
#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace wide {
  /// An owning, homogeneous collection of independent computation chains.
  template<class T,std::size_t N> struct array {
    alignas(T) std::array<T,N> values;
    template<std::size_t I> requires (I<N)
    simd_inline constexpr T & get() & noexcept { return values[I]; }
    template<std::size_t I> requires (I<N)
    simd_inline constexpr T const & get() const & noexcept { return values[I]; }
    template<std::size_t I> requires (I<N)
    simd_inline constexpr T && get() && noexcept { return std::move(values[I]); }
    template<std::size_t I> requires (I<N)
    simd_inline constexpr T const && get() const && noexcept { return std::move(values[I]); }
  };
  template<class T,class... U> requires (std::same_as<T,U> && ...)
  array(T,U...) -> array<T,1+sizeof...(U)>;
  template<class T,std::size_t N> array(std::array<T,N>) -> array<T,N>;

  /// An owning, heterogeneous collection of independent computation chains.
  template<class... T> struct tuple {
    static_assert((std::is_object_v<T> && ...), "wide::tuple owns its elements");
    alignas(T...) std::tuple<T...> values;
    constexpr tuple() = default;
    template<class... U> requires (sizeof...(U)>0 && sizeof...(T)==sizeof...(U)) &&
      std::constructible_from<std::tuple<T...>,U&&...>
    simd_inline constexpr explicit tuple(U&&... value)
      noexcept(std::is_nothrow_constructible_v<std::tuple<T...>,U&&...>)
      : values(std::forward<U>(value)...) {}
    simd_inline constexpr explicit tuple(std::tuple<T...> value)
      noexcept(std::is_nothrow_move_constructible_v<std::tuple<T...>>)
      : values(std::move(value)) {}
    template<std::size_t I> requires (I<sizeof...(T))
    simd_inline constexpr decltype(auto) get() & noexcept { return std::get<I>(values); }
    template<std::size_t I> requires (I<sizeof...(T))
    simd_inline constexpr decltype(auto) get() const & noexcept { return std::get<I>(values); }
    template<std::size_t I> requires (I<sizeof...(T))
    simd_inline constexpr decltype(auto) get() && noexcept { return std::get<I>(std::move(values)); }
    template<std::size_t I> requires (I<sizeof...(T))
    simd_inline constexpr decltype(auto) get() const && noexcept { return std::get<I>(std::move(values)); }
  };
  template<class... T> tuple(T...) -> tuple<T...>;
  template<class... T> tuple(std::tuple<T...>) -> tuple<T...>;

  namespace detail {
    enum class family { leaf, array, tuple, std_array, std_tuple, legacy };
    template<class T> struct shape { static constexpr auto kind=family::leaf; };
    template<class T,std::size_t N> struct array_shape {
      static constexpr std::size_t size=N;
      template<std::size_t> using element_type=T;
    };
    template<class... T> struct tuple_shape {
      static constexpr std::size_t size=sizeof...(T);
      template<std::size_t I> using element_type=std::tuple_element_t<I,std::tuple<T...>>;
    };
    template<class T,std::size_t N> struct shape<array<T,N>> : array_shape<T,N> {
      static constexpr auto kind=family::array;
    };
    template<class... T> struct shape<tuple<T...>> : tuple_shape<T...> {
      static constexpr auto kind=family::tuple;
    };
    template<class T,std::size_t N> struct shape<std::array<T,N>> : array_shape<T,N> {
      static constexpr auto kind=family::std_array;
    };
    template<class... T> struct shape<std::tuple<T...>> : tuple_shape<T...> {
      static constexpr auto kind=family::std_tuple;
    };
    template<class T> using shape_t=shape<std::remove_cvref_t<T>>;
    template<class T> inline constexpr bool is_array=
      shape_t<T>::kind==family::array || shape_t<T>::kind==family::std_array ||
      shape_t<T>::kind==family::legacy;
    template<class T> inline constexpr bool is_tuple=
      shape_t<T>::kind==family::tuple || shape_t<T>::kind==family::std_tuple;
  }

  template<class T> concept pack=detail::shape_t<T>::kind==detail::family::array ||
    detail::shape_t<T>::kind==detail::family::tuple;

  template<std::size_t I,pack P> requires (I<detail::shape_t<P>::size)
  simd_inline constexpr decltype(auto) get(P&& value) noexcept {
    return std::forward<P>(value).template get<I>();
  }

  namespace detail {
    template<std::size_t I,class P>
    simd_inline constexpr decltype(auto) source_get(P&& value) noexcept {
      if constexpr (pack<P>) return ::wide::get<I>(std::forward<P>(value));
      else if constexpr (shape_t<P>::kind==family::legacy)
        return std::get<I>(std::forward<P>(value).registers);
      else return std::get<I>(std::forward<P>(value));
    }

    // Element adaptation does not add an outer dimension. All construction is
    // dependent, so this header may precede the complete SIMD definitions.
    template<class T> struct element_traits {};
    template<> struct element_traits<float> { using type=simd::vec<float,1,simd::scalar>; };
    template<class T,std::size_t N,simd::isa A>
    struct element_traits<simd::vec<T,N,A>> { using type=simd::vec<T,N,A>; };
    template<std::size_t N,simd::isa A>
    struct element_traits<simd::predicate<N,A>> { using type=simd::predicate<N,A>; };
    template<class T> using adapted_t=typename element_traits<std::remove_cvref_t<T>>::type;
    template<class T> concept adaptable=requires { typename adapted_t<T>; };
    template<class P> requires is_array<P> && adaptable<typename shape_t<P>::template element_type<0>>
    struct element_traits<P> {
      using type=array<adapted_t<typename shape_t<P>::template element_type<0>>,shape_t<P>::size>;
    };
    template<class... T> requires (adaptable<T> && ...)
    struct element_traits<tuple<T...>> { using type=tuple<adapted_t<T>...>; };
    template<class... T> requires (adaptable<T> && ...)
    struct element_traits<std::tuple<T...>> { using type=tuple<adapted_t<T>...>; };

    template<class T> struct promotion_traits {};
    template<adaptable T> struct promotion_traits<T> {
      using canonical_type=std::conditional_t<shape_t<T>::kind==family::leaf,
        array<adapted_t<T>,1>,adapted_t<T>>;
    };

    template<adaptable T> simd_inline constexpr adapted_t<T> adapt_element(T&& value);
    template<class P,std::size_t... I>
    simd_inline constexpr adapted_t<P> adapt_pack(P&& value,std::index_sequence<I...>) {
      if constexpr (is_array<P>)
        return {{adapt_element(source_get<I>(std::forward<P>(value)))...}};
      else return adapted_t<P>{adapt_element(source_get<I>(std::forward<P>(value)))...};
    }
    template<adaptable T>
    simd_inline constexpr adapted_t<T> adapt_element(T&& value) {
      if constexpr (shape_t<T>::kind==family::leaf)
        return adapted_t<T>(std::forward<T>(value));
      else return adapt_pack(std::forward<T>(value),std::make_index_sequence<shape_t<T>::size>{});
    }
  }

  template<class T> struct traits : detail::promotion_traits<std::remove_cvref_t<T>> {};
  template<class T> using canonical_t=typename traits<std::remove_cvref_t<T>>::canonical_type;
  template<class T> concept promotable=requires { typename canonical_t<T>; };

  /// Own a canonical pack; a scalar or SIMD leaf contributes one outer element.
  template<promotable T>
  simd_inline constexpr canonical_t<T> promote(T&& value) {
    if constexpr (detail::shape_t<T>::kind==detail::family::leaf)
      return {{detail::adapt_element(std::forward<T>(value))}};
    else return detail::adapt_element(std::forward<T>(value));
  }

  namespace detail {
    template<class T> inline constexpr bool scalar_vector=false;
    template<class T> inline constexpr bool scalar_vector<simd::vec<T,1,simd::scalar>> = true;
    template<class T> inline constexpr bool scalar_mask=false;
    template<class T> requires (std::same_as<T,bool> || simd::detail::is_mask_lane<T>)
    inline constexpr bool scalar_mask<simd::vec<T,1,simd::scalar>> = true;
    template<> inline constexpr bool scalar_mask<simd::predicate<1,simd::scalar>> = true;

    template<class Original,class P> consteval bool compatible_pack();
    template<class Original,class R> consteval bool compatible_element() {
      if constexpr (shape_t<Original>::kind==family::leaf) return true;
      else return compatible_pack<Original,R>();
    }
    template<class Original,class P,std::size_t... I>
    consteval bool compatible_elements(std::index_sequence<I...>) {
      return (compatible_element<typename shape_t<Original>::template element_type<I>,
        typename shape_t<P>::template element_type<I>>() && ...);
    }
    template<class Original,class P> consteval bool compatible_pack() {
      if constexpr (!pack<P>) return false;
      else if constexpr (shape_t<Original>::kind==family::leaf)
        return shape_t<P>::kind==family::array && shape_t<P>::size==1;
      else if constexpr (is_array<Original> != is_array<P> ||
                         shape_t<Original>::size!=shape_t<P>::size) return false;
      else if constexpr (is_array<Original> && shape_t<Original>::size==0)
        return compatible_element<typename shape_t<Original>::template element_type<0>,
          typename shape_t<P>::template element_type<0>>();
      else return compatible_elements<Original,P>(std::make_index_sequence<shape_t<P>::size>{});
    }

    template<class Original,class R> simd_inline constexpr auto restore_element(R&& value);
    template<class Original,class P,std::size_t... I>
    simd_inline constexpr auto restore_pack(P&& value,std::index_sequence<I...>) {
      using S=shape_t<Original>;
      if constexpr (is_array<Original>) {
        using E=typename S::template element_type<0>;
        using V=typename shape_t<P>::template element_type<0>;
        using R=decltype(restore_element<E>(std::declval<V>()));
        if constexpr (S::kind==family::array)
          return array<R,S::size>{{restore_element<E>(::wide::get<I>(std::forward<P>(value)))...}};
        else if constexpr (S::kind==family::std_array)
          return std::array<R,S::size>{{restore_element<E>(::wide::get<I>(std::forward<P>(value)))...}};
        else return typename S::template rebind<R>{std::array<R,S::size>{{
          restore_element<E>(::wide::get<I>(std::forward<P>(value)))...}}};
      } else if constexpr (S::kind==family::tuple)
        return tuple{restore_element<typename S::template element_type<I>>(
          ::wide::get<I>(std::forward<P>(value)))...};
      else return std::tuple{restore_element<typename S::template element_type<I>>(
        ::wide::get<I>(std::forward<P>(value)))...};
    }
    template<class Original,class R>
    simd_inline constexpr auto restore_element(R&& value) {
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
  simd_inline constexpr auto demote(P&& result) {
    if constexpr (detail::shape_t<Original>::kind==detail::family::leaf)
      return detail::restore_element<Original>(::wide::get<0>(std::forward<P>(result)));
    else return detail::restore_pack<Original>(std::forward<P>(result),
      std::make_index_sequence<detail::shape_t<Original>::size>{});
  }

  namespace detail {
    template<class From,class T> using copy_const_t=
      std::conditional_t<std::is_const_v<std::remove_reference_t<From>>,std::add_const_t<T>,T>;
    template<class From,class T> using copy_cv_t=
      std::conditional_t<std::is_volatile_v<std::remove_reference_t<From>>,
        std::add_volatile_t<copy_const_t<From,T>>,copy_const_t<From,T>>;
    template<class P,std::size_t I> using element_argument_t=
      std::conditional_t<std::is_lvalue_reference_v<P>,
        copy_cv_t<P,typename shape_t<P>::template element_type<I>>&,
        copy_cv_t<P,typename shape_t<P>::template element_type<I>>&&>;
    template<std::size_t I,class F,class... P> using mapped_t=
      std::remove_cvref_t<std::invoke_result_t<F&,element_argument_t<P,I>...>>;
    template<std::size_t I,class F,class... P>
    simd_inline constexpr decltype(auto) map_element(F& function,P&&... value) {
      return std::invoke(function,::wide::get<I>(std::forward<P>(value))...);
    }
    template<class F,class P,class... Rest,std::size_t... I>
    simd_inline constexpr auto map_pack(F&& function,std::index_sequence<I...>,P&& first,Rest&&... rest) {
      // Tuple construction keeps distinct result types. An empty homogeneous
      // pack still obtains its result type from the callable's element contract.
      if constexpr (shape_t<P>::kind==family::array) {
        using R=mapped_t<0,F,P,Rest...>;
        static_assert((std::same_as<R,mapped_t<I,F,P,Rest...>> && ...),
          "wide::map over an array requires homogeneous results");
        return array<R,sizeof...(I)>{{
          map_element<I>(function,std::forward<P>(first),std::forward<Rest>(rest)...)...}};
      } else return tuple{
        map_element<I>(function,std::forward<P>(first),std::forward<Rest>(rest)...)...};
    }
  }

  /// Advance every element through one operation, preserving the first pack's family.
  template<class F,pack P,pack... Rest>
    requires ((detail::shape_t<P>::size==detail::shape_t<Rest>::size) && ...)
  simd_inline constexpr auto map(F&& function,P&& first,Rest&&... rest) {
    return detail::map_pack(std::forward<F>(function),
      std::make_index_sequence<detail::shape_t<P>::size>{},
      std::forward<P>(first),std::forward<Rest>(rest)...);
  }
}

namespace std {
  template<class T,size_t N> struct tuple_size<::wide::array<T,N>> : integral_constant<size_t,N> {};
  template<size_t I,class T,size_t N> struct tuple_element<I,::wide::array<T,N>> : tuple_element<I,array<T,N>> {};
  template<class... T> struct tuple_size<::wide::tuple<T...>> : integral_constant<size_t,sizeof...(T)> {};
  template<size_t I,class... T> struct tuple_element<I,::wide::tuple<T...>> : tuple_element<I,tuple<T...>> {};
}
