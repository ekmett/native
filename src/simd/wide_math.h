// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "simd/vec.h"
#include "simd/wide_pack.h"

namespace wide::detail {
  template<class V> struct native_ops;
}
#define SIMD_BACKEND_BODY "simd/wide_native_body.h"
#include "simd/simd/for_each_backend.h"
#undef SIMD_BACKEND_BODY

namespace wide::detail {
  template<class P> inline constexpr bool native_pack = false;
  template<class V, std::size_t N>
  inline constexpr bool native_pack<array<V, N>> = requires { sizeof(native_ops<V>); };
  template<class... V>
  inline constexpr bool native_pack<tuple<V...>> = (requires { sizeof(native_ops<V>); } && ...);
  template<class P> concept native_pack_argument = native_pack<std::remove_cvref_t<P>>;
  template<class V> concept native_leaf_argument = (!pack<V>) &&
    requires { sizeof(native_ops<std::remove_cvref_t<V>>); };

  // Bind native operands once, then let map expand only the actual packs.
  // A broadcast is a const reference, never a value moved once per chain.
  template<class A> native_inline auto pack_operand(A const & value) noexcept {
    if constexpr (pack<A>) return std::tuple<A const &>{value};
    else return std::tuple<>{};
  }
  template<std::size_t I,class... A> consteval std::size_t pack_position() {
    constexpr bool packed[]{pack<A>...};
    std::size_t position=0;
    for(std::size_t j=0;j<I;++j) position+=packed[j];
    return position;
  }
  template<std::size_t I,class... A,class... V>
  native_inline decltype(auto) bound_operand(std::tuple<A const &...> const & arguments,
                                             std::tuple<V const &...> const & elements) noexcept {
    if constexpr (pack<std::tuple_element_t<I,std::tuple<A...>>>)
      return std::get<pack_position<I,A...>()>(elements);
    else return std::get<I>(arguments);
  }
  template<class F,class... A,class... V,std::size_t... I>
  native_inline auto invoke_bound(F const & function,std::tuple<A const &...> const & arguments,
      std::tuple<V const &...> const & elements,std::index_sequence<I...>)
      -> decltype(std::invoke(function,bound_operand<I>(arguments,elements)...)) {
    return std::invoke(function,bound_operand<I>(arguments,elements)...);
  }
  template<class F,class... A> struct bound_operation {
    F function;
    std::tuple<A const &...> arguments;
    template<class... V>
    native_inline auto operator()(V const &... value) const
      -> decltype(invoke_bound(function,arguments,std::tie(value...),std::index_sequence_for<A...>{})) {
      return invoke_bound(function,arguments,std::tie(value...),std::index_sequence_for<A...>{});
    }
  };
  template<class F,class... A,class... P>
  consteval bool accepts_map(std::type_identity<std::tuple<P...>>) {
    return requires(bound_operation<F,A...> operation,P... value) { map(operation,value...); };
  }
  template<class F,class... A> concept liftable = (pack<A> || ...) &&
    ((native_pack_argument<A> || native_leaf_argument<A>) && ...) &&
    accepts_map<F,A...>(std::type_identity<decltype(std::tuple_cat(pack_operand(std::declval<A const &>())...))>{});
  template<class F,class... A> requires liftable<F,A...>
  native_inline auto lift(F function,A const &... arguments) {
    auto operation=bound_operation<F,A...>{function,std::tie(arguments...)};
    return std::apply([&](auto const &... packed) { return map(operation,packed...); },
      std::tuple_cat(pack_operand(arguments)...));
  }

#define SIMD_WIDE_BINARY_OPERATION(name,bridge,op) \
  struct name { \
    template<class V> requires requires(V a) { a op a; } \
    native_inline auto operator()(V const & a,V const & b) const { return native_ops<V>::bridge(a,b); } \
  };
  SIMD_WIDE_BINARY_OPERATION(add,add,+)
  SIMD_WIDE_BINARY_OPERATION(subtract,sub,-)
  SIMD_WIDE_BINARY_OPERATION(multiply,mul,*)
  SIMD_WIDE_BINARY_OPERATION(divide,div,/)
  SIMD_WIDE_BINARY_OPERATION(bit_and,bit_and,&)
  SIMD_WIDE_BINARY_OPERATION(bit_or,bit_or,|)
  SIMD_WIDE_BINARY_OPERATION(bit_xor,bit_xor,^)
  SIMD_WIDE_BINARY_OPERATION(equal,equal,==)
  SIMD_WIDE_BINARY_OPERATION(unequal,unequal,!=)
  SIMD_WIDE_BINARY_OPERATION(less,less,<)
  SIMD_WIDE_BINARY_OPERATION(less_equal,less_equal,<=)
  SIMD_WIDE_BINARY_OPERATION(greater,greater,>)
  SIMD_WIDE_BINARY_OPERATION(greater_equal,greater_equal,>=)
#undef SIMD_WIDE_BINARY_OPERATION
#define SIMD_WIDE_UNARY_OPERATION(name,bridge,op) \
  struct name { \
    template<class V> requires requires(V a) { { op a } -> std::same_as<V>; } \
    native_inline auto operator()(V const & a) const { return native_ops<V>::bridge(a); } \
  };
  SIMD_WIDE_UNARY_OPERATION(negate,negate,-)
  SIMD_WIDE_UNARY_OPERATION(bit_not,bit_not,~)
  SIMD_WIDE_UNARY_OPERATION(logical_not,logical_not,!)
#undef SIMD_WIDE_UNARY_OPERATION
#define SIMD_WIDE_UNARY_MATH(name,bridge,operation) \
  struct name { \
    template<class V> requires requires(V a) { { operation(a) } -> std::same_as<V>; } \
    native_inline auto operator()(V const & a) const { return native_ops<V>::bridge(a); } \
  };
  SIMD_WIDE_UNARY_MATH(absolute,absolute,abs)
  SIMD_WIDE_UNARY_MATH(root,root,sqrt)
  SIMD_WIDE_UNARY_MATH(downward,downward,floor)
  SIMD_WIDE_UNARY_MATH(upward,upward,ceil)
  SIMD_WIDE_UNARY_MATH(truncate,truncate,trunc)
  SIMD_WIDE_UNARY_MATH(round,round,round_even)
#undef SIMD_WIDE_UNARY_MATH
  struct minimum {
    template<class V> requires requires(V a) { select(a<a,a,a); }
    native_inline auto operator()(V const & a,V const & b) const { return native_ops<V>::minimum(a,b); }
  };
  struct maximum {
    template<class V> requires requires(V a) { select(a>a,a,a); }
    native_inline auto operator()(V const & a,V const & b) const { return native_ops<V>::maximum(a,b); }
  };
  struct fused {
    template<class V> requires requires(V a) { fma(a,a,a); }
    native_inline auto operator()(V const & a,V const & b,V const & c) const { return native_ops<V>::fused(a,b,c); }
  };
  struct scale {
    template<class M,class V> requires requires(M m,V a) { masked_scaleb_zero(m,a,a); }
    native_inline auto operator()(M const & m,V const & a,V const & n) const { return native_ops<V>::scale(m,a,n); }
  };
  struct scale_all {
    template<class V> requires requires(V a) { scaleb(a,a); }
    native_inline auto operator()(V const & a,V const & n) const { return native_ops<V>::scale_all(a,n); }
  };
  struct choose {
    template<class M,class V> requires requires(M m,V a) { select(m,a,a); }
    native_inline auto operator()(M const & m,V const & a,V const & b) const { return native_ops<V>::choose(m,a,b); }
  };
  struct scale_merge {
    template<class M,class V> requires requires(M m,V a) { masked_scaleb(m,a,a,a); }
    native_inline auto operator()(M const & m,V const & prior,V const & a,V const & n) const {
      return native_ops<V>::scale_merge(m,prior,a,n);
    }
  };
  struct encode {
    template<class V> requires std::same_as<typename V::value_type,float>
    native_inline auto operator()(V const & a) const { return native_ops<V>::encode(a); }
  };
  struct decode {
    template<class V> requires std::same_as<typename V::value_type,std::uint32_t>
    native_inline auto operator()(V const & a) const { return native_ops<V>::decode(a); }
  };
  template<unsigned Shift> struct shift_left {
    template<class V> requires requires(V a) { a.template left<Shift>(); }
    native_inline auto operator()(V const & a) const { return native_ops<V>::template left<Shift>(a); }
  };
  template<class T> struct mask_words {
    template<class V> requires requires(V a) { ::simd::mask_bits<T>(a); }
    native_inline auto operator()(V const & a) const { return native_ops<V>::template mask_words<T>(a); }
  };
  struct trig_integer_operation {
    template<class V> requires std::same_as<typename V::value_type,float>
    native_inline auto operator()(V const & a) const { return native_ops<V>::trig_integer(a); }
  };
  struct trig_float_operation {
    template<class V> requires std::same_as<typename V::value_type,std::uint32_t>
    native_inline auto operator()(V const & a) const { return native_ops<V>::trig_float(a); }
  };
  template<class P> requires liftable<trig_integer_operation,P>
  native_inline auto trig_integer(P const & a) noexcept { return lift(trig_integer_operation{},a); }
  template<class P> requires liftable<trig_float_operation,P>
  native_inline auto trig_float(P const & a) noexcept { return lift(trig_float_operation{},a); }

  template<class P> struct constant_type {};
  template<class V,std::size_t N> struct constant_type<array<V,N>> { using type=V; };
  template<class V,class... Rest> requires (std::same_as<V,Rest> && ...)
  struct constant_type<tuple<V,Rest...>> { using type=V; };
  template<class P,class T> inline constexpr bool matching_coefficient=false;
  template<class V,std::size_t N,class T>
  inline constexpr bool matching_coefficient<array<V,N>,T> = std::same_as<typename V::value_type,T>;
  template<class... V,class T>
  inline constexpr bool matching_coefficient<tuple<V...>,T> = (std::same_as<typename V::value_type,T> && ...);
}

namespace wide {
  /// Share one SIMD coefficient for homogeneous chains; retain a coefficient
  /// pack only when the chains require different computation types.
  template<detail::native_pack_argument P,class T>
    requires (std::same_as<T,float> || std::same_as<T,std::uint32_t>) &&
      detail::matching_coefficient<std::remove_cvref_t<P>,T>
  simd_nodiscard native_inline auto constant_like(P const & shape, T value) noexcept {
    using S=std::remove_cvref_t<P>;
    if constexpr (requires { typename detail::constant_type<S>::type; }) {
      using V=typename detail::constant_type<S>::type;
      return detail::native_ops<V>::constant(value);
    } else return map([value]<class V>(V const &) { return detail::native_ops<V>::constant(value); }, shape);
  }
#define SIMD_WIDE_BINARY_API(name,operation) \
  template<class P,class Q> requires detail::liftable<detail::operation,P,Q> \
  simd_nodiscard native_inline auto name(P const & a,Q const & b) noexcept { \
    return detail::lift(detail::operation{},a,b); \
  }
  SIMD_WIDE_BINARY_API(operator+,add)
  SIMD_WIDE_BINARY_API(operator-,subtract)
  SIMD_WIDE_BINARY_API(operator*,multiply)
  SIMD_WIDE_BINARY_API(operator/,divide)
  SIMD_WIDE_BINARY_API(operator&,bit_and)
  SIMD_WIDE_BINARY_API(operator|,bit_or)
  SIMD_WIDE_BINARY_API(operator^,bit_xor)
  SIMD_WIDE_BINARY_API(operator==,equal)
  SIMD_WIDE_BINARY_API(operator!=,unequal)
  SIMD_WIDE_BINARY_API(operator<,less)
  SIMD_WIDE_BINARY_API(operator<=,less_equal)
  SIMD_WIDE_BINARY_API(operator>,greater)
  SIMD_WIDE_BINARY_API(operator>=,greater_equal)
  SIMD_WIDE_BINARY_API(min,minimum)
  SIMD_WIDE_BINARY_API(max,maximum)
  SIMD_WIDE_BINARY_API(scaleb,scale_all)
#undef SIMD_WIDE_BINARY_API
#define SIMD_WIDE_UNARY_API(name,operation) \
  template<class P> requires detail::liftable<detail::operation,P> \
  simd_nodiscard native_inline auto name(P const & a) noexcept { \
    return detail::lift(detail::operation{},a); \
  }
  SIMD_WIDE_UNARY_API(operator-,negate)
  SIMD_WIDE_UNARY_API(operator~,bit_not)
  SIMD_WIDE_UNARY_API(operator!,logical_not)
  SIMD_WIDE_UNARY_API(abs,absolute)
  SIMD_WIDE_UNARY_API(sqrt,root)
  SIMD_WIDE_UNARY_API(floor,downward)
  SIMD_WIDE_UNARY_API(ceil,upward)
  SIMD_WIDE_UNARY_API(trunc,truncate)
  SIMD_WIDE_UNARY_API(round_even,round)
  SIMD_WIDE_UNARY_API(bits,encode)
  SIMD_WIDE_UNARY_API(from_bits,decode)
#undef SIMD_WIDE_UNARY_API
  template<class P,class Q,class R> requires detail::liftable<detail::fused,P,Q,R>
  simd_nodiscard native_inline auto fma(P const & a, Q const & b, R const & c) noexcept {
    return detail::lift(detail::fused{},a,b,c);
  }
  template<class P,class Q,class R> requires detail::liftable<detail::scale,P,Q,R>
  simd_nodiscard native_inline auto masked_scaleb_zero(P const & m, Q const & a, R const & n) noexcept {
    return detail::lift(detail::scale{},m,a,n);
  }
  template<class M,class P,class Q> requires detail::liftable<detail::choose,M,P,Q>
  simd_nodiscard native_inline auto select(M const & m,P const & a,Q const & b) noexcept {
    return detail::lift(detail::choose{},m,a,b);
  }
  template<class M,class P,class Q,class R> requires detail::liftable<detail::scale_merge,M,P,Q,R>
  simd_nodiscard native_inline auto masked_scaleb(M const & m,P const & prior,Q const & a,R const & n) noexcept {
    return detail::lift(detail::scale_merge{},m,prior,a,n);
  }
  template<unsigned Shift,class P> requires detail::liftable<detail::shift_left<Shift>,P>
  simd_nodiscard native_inline auto left(P const & a) noexcept {
    return detail::lift(detail::shift_left<Shift>{},a);
  }
  template<class T,class P> requires detail::liftable<detail::mask_words<T>,P>
  simd_nodiscard native_inline auto mask_bits(P const & a) noexcept {
    return detail::lift(detail::mask_words<T>{},a);
  }

  namespace detail {
    template<class T> inline constexpr bool binary32_register = false;
    template<std::size_t N, ::simd::isa A>
    inline constexpr bool binary32_register<::simd::vec<float, N, A>> = true;
    template<class P> inline constexpr bool binary32_array = false;
    template<class V, std::size_t N>
    inline constexpr bool binary32_array<array<V, N>> = binary32_register<V>;
    template<class P> inline constexpr bool binary32_pack = false;
    template<class V, std::size_t N>
    inline constexpr bool binary32_pack<array<V, N>> = binary32_array<array<V, N>>;
    template<class... V>
    inline constexpr bool binary32_pack<tuple<V...>> = (binary32_register<V> && ...);
  }
}

namespace math {
  namespace detail {
    // The single polynomial body, shared by generic and targeted entry points.
    template<bool Flush, class V, std::size_t N>
      requires (::wide::detail::binary32_register<V>)
    simd_nodiscard native_inline auto exp_reduced(::wide::array<V, N> const & x) noexcept {
      auto const c = [&](float value) { return ::wide::constant_like(x, value); };
      auto const active = !(x < c(Flush ? -87.33654022216796875f : -104.f));
      // Keep x second: the ordered minimum preserves NaNs.
      auto r = min(c(88.72283935546875f), x);
      auto const n = round_even(r * c(1.4426950408889634f));
      r = fma(n, c(-0x1.62e400p-1f), r);
      r = fma(n, c(-0x1.7f7d1cp-20f), r);

      auto y = fma(r, c(0x1.a1d714d7b1510dp-13f), c(0x1.6da756e670ea6p-10f));
      y = fma(r, y, c(0x1.11105b3161a6fp-7f));
      y = fma(r, y, c(0x1.5554649b7487fp-5f));
      y = fma(r, y, c(0x1.555555c673724p-3f));
      y = fma(r, y, c(0x1.0000005c8dd89p-1f));
      auto const one = c(1.f);
      y = fma(r, y, one);
      y = fma(r, y, one);
      return std::tuple{active, y, n};
    }
  }

  /// Evaluate exp through a homogeneous SIMD array and restore the input shape.
  /// Scalar and SIMD inputs promote to singleton arrays; tuples are unsupported.
  template<bool Flush = false, ::wide::promotable T>
    requires (::wide::detail::binary32_array<::wide::canonical_t<T>>)
  simd_nodiscard native_inline auto exp(T const & input) noexcept {
    // MSVC's array<T,0> may construct a dummy T; an empty batch needs no work.
    if constexpr (::wide::detail::shape_t<::wide::canonical_t<T>>::size == 0) {
      return std::remove_cvref_t<T>(input);
    } else {
      auto const x = ::wide::promote(input);
      auto const [active, y, n] = detail::exp_reduced<Flush>(x);
      return ::wide::demote<T>(masked_scaleb_zero(active, y, n));
    }
  }
}

#include "simd/wide_math_kernels.h"
