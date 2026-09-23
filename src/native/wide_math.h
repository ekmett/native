// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: (BSD-2-Clause OR Apache-2.0) AND BSL-1.0
#pragma once
#include "native/simd.h"
#include "native/wide_pack.h"
#include <tuple>

namespace wide::detail {
  template<class V> struct native_ops;
}
#define NATIVE_BACKEND_BODY "native/wide_native_body.h"
#include "native/simd/for_each_backend.h"
#undef NATIVE_BACKEND_BODY

namespace wide::detail {
  template<class P> inline constexpr bool native_pack = false;
  template<class V, std::size_t N>
  inline constexpr bool native_pack<std::array<V, N>> = requires { sizeof(native_ops<V>); };
  template<class P> concept native_pack_argument = native_pack<std::remove_cvref_t<P>>;
  template<class V> concept native_leaf_argument = (!pack<V>) &&
    requires { sizeof(native_ops<std::remove_cvref_t<V>>); };

  // Every batch is homogeneous. Select each array's element directly and
  // broadcast native operands by reference, without tuple argument binding.
  template<class A> struct lift_argument { using type=std::remove_cvref_t<A>; };
  template<class V,std::size_t N> struct lift_argument<std::array<V,N>> { using type=V; };
  template<class A> using lift_argument_t=typename lift_argument<std::remove_cvref_t<A>>::type;
  template<class A,class... Rest> consteval std::size_t lift_size() {
    if constexpr (pack<A>) return shape_t<A>::size;
    else return lift_size<Rest...>();
  }
  template<std::size_t N,class A> consteval bool lift_size_matches() {
    if constexpr (pack<A>) return shape_t<A>::size==N;
    else return true;
  }
  template<class F,class... A> consteval bool lift_result_valid() {
    if constexpr (!std::is_invocable_v<F const &,lift_argument_t<A> const &...>) return false;
    else {
      using R=std::invoke_result_t<F const &,lift_argument_t<A> const &...>;
      return std::is_object_v<std::remove_cvref_t<R>> &&
        std::is_constructible_v<std::remove_cvref_t<R>,R>;
    }
  }
  template<class F,class... A> concept liftable = (pack<A> || ...) &&
    ((native_pack_argument<A> || native_leaf_argument<A>) && ...) &&
    (lift_size_matches<lift_size<A...>(),A>() && ...) && lift_result_valid<F,A...>();
  template<std::size_t I,class A>
  native_inline constexpr decltype(auto) lift_operand(A const & value) {
    if constexpr (pack<A>) return std::get<I>(value);
    else return (value);
  }
  template<std::size_t I,class F,class... A>
  native_inline constexpr auto lift_element(F const & function,A const &... arguments) {
    return function(lift_operand<I>(arguments)...);
  }
  template<class F,class... A,std::size_t... I>
  native_inline constexpr auto lift_array(F const & function,std::index_sequence<I...>,A const &... arguments) {
    using R=std::remove_cvref_t<std::invoke_result_t<F const &,lift_argument_t<A> const &...>>;
    return std::array<R,sizeof...(I)>{{lift_element<I>(function,arguments...)...}};
  }
  template<class F,class... A> requires liftable<F,A...>
  native_inline constexpr auto lift(F function,A const &... arguments) {
    return lift_array(function,std::make_index_sequence<lift_size<A...>()>{},arguments...);
  }

#define NATIVE_WIDE_BINARY_OPERATION(name,bridge,op) \
  struct name { \
    template<class V> requires requires(V a) { a op a; } \
    native_inline constexpr auto operator()(V const & a,V const & b) const { return native_ops<V>::bridge(a,b); } \
  };
  NATIVE_WIDE_BINARY_OPERATION(add,add,+)
  NATIVE_WIDE_BINARY_OPERATION(subtract,sub,-)
  NATIVE_WIDE_BINARY_OPERATION(multiply,mul,*)
  NATIVE_WIDE_BINARY_OPERATION(divide,div,/)
  NATIVE_WIDE_BINARY_OPERATION(bit_and,bit_and,&)
  NATIVE_WIDE_BINARY_OPERATION(bit_or,bit_or,|)
  NATIVE_WIDE_BINARY_OPERATION(bit_xor,bit_xor,^)
  NATIVE_WIDE_BINARY_OPERATION(equal,equal,==)
  NATIVE_WIDE_BINARY_OPERATION(unequal,unequal,!=)
  NATIVE_WIDE_BINARY_OPERATION(less,less,<)
  NATIVE_WIDE_BINARY_OPERATION(less_equal,less_equal,<=)
  NATIVE_WIDE_BINARY_OPERATION(greater,greater,>)
  NATIVE_WIDE_BINARY_OPERATION(greater_equal,greater_equal,>=)
#undef NATIVE_WIDE_BINARY_OPERATION
#define NATIVE_WIDE_UNARY_OPERATION(name,bridge,op) \
  struct name { \
    template<class V> requires requires(V a) { { op a } -> std::same_as<V>; } \
    native_inline constexpr auto operator()(V const & a) const { return native_ops<V>::bridge(a); } \
  };
  NATIVE_WIDE_UNARY_OPERATION(negate,negate,-)
  NATIVE_WIDE_UNARY_OPERATION(bit_not,bit_not,~)
  NATIVE_WIDE_UNARY_OPERATION(logical_not,logical_not,!)
#undef NATIVE_WIDE_UNARY_OPERATION
#define NATIVE_WIDE_UNARY_MATH(name,bridge,operation) \
  struct name { \
    template<class V> requires requires(V a) { { operation(a) } -> std::same_as<V>; } \
    native_inline constexpr auto operator()(V const & a) const { return native_ops<V>::bridge(a); } \
  };
  NATIVE_WIDE_UNARY_MATH(absolute,absolute,abs)
  NATIVE_WIDE_UNARY_MATH(root,root,sqrt)
  NATIVE_WIDE_UNARY_MATH(downward,downward,floor)
  NATIVE_WIDE_UNARY_MATH(upward,upward,ceil)
  NATIVE_WIDE_UNARY_MATH(truncate,truncate,trunc)
  NATIVE_WIDE_UNARY_MATH(round,round,round_even)
#undef NATIVE_WIDE_UNARY_MATH
  struct minimum {
    template<class V> requires requires(V a) { select(a<a,a,a); }
    native_inline constexpr auto operator()(V const & a,V const & b) const { return native_ops<V>::minimum(a,b); }
  };
  struct maximum {
    template<class V> requires requires(V a) { select(a>a,a,a); }
    native_inline constexpr auto operator()(V const & a,V const & b) const { return native_ops<V>::maximum(a,b); }
  };
  struct fused {
    template<class V> requires requires(V a) { fma(a,a,a); }
    native_inline constexpr auto operator()(V const & a,V const & b,V const & c) const { return native_ops<V>::fused(a,b,c); }
  };
  struct polynomial_madd {
    template<class V>
    native_inline constexpr auto operator()(V const & a,V const & b,V const & c) const {
      return native_ops<V>::polynomial_madd(a,b,c);
    }
  };
  struct exp_power_operation {
    template<class V>
    native_inline constexpr auto operator()(V const & n) const { return native_ops<V>::exp_power(n); }
  };
  template<class P> requires liftable<exp_power_operation,P>
  native_inline constexpr auto exp_power(P const & n) noexcept { return lift(exp_power_operation{},n); }
  struct exp_scale {
    template<class M,class V>
    native_inline constexpr auto operator()(M const & m,V const & replacement,V const & a,V const & n) const {
      return native_ops<V>::exp_scale(m,replacement,a,n);
    }
  };
  template<class P,class Q,class R> requires liftable<polynomial_madd,P,Q,R>
  native_inline constexpr auto madd(P const & a,Q const & b,R const & c) noexcept {
    return lift(polynomial_madd{},a,b,c);
  }
  template<class M,class P,class Q> requires liftable<exp_scale,M,P,P,Q>
  native_inline constexpr auto scale_exp(M const & in_range,P const & replacement,P const & y,Q const & n) noexcept {
    return lift(exp_scale{},in_range,replacement,y,n);
  }
  struct scale {
    template<class M,class V> requires requires(M m,V a) { masked_scaleb_zero(m,a,a); }
    native_inline constexpr auto operator()(M const & m,V const & a,V const & n) const { return native_ops<V>::scale(m,a,n); }
  };
  struct scale_all {
    template<class V> requires requires(V a) { scaleb(a,a); }
    native_inline constexpr auto operator()(V const & a,V const & n) const { return native_ops<V>::scale_all(a,n); }
  };
  struct choose {
    template<class M,class V> requires requires(M m,V a) { select(m,a,a); }
    native_inline constexpr auto operator()(M const & m,V const & a,V const & b) const { return native_ops<V>::choose(m,a,b); }
  };
  struct scale_merge {
    template<class M,class V> requires requires(M m,V a) { masked_scaleb(m,a,a,a); }
    native_inline constexpr auto operator()(M const & m,V const & prior,V const & a,V const & n) const {
      return native_ops<V>::scale_merge(m,prior,a,n);
    }
  };
  struct encode {
    template<class V> requires std::same_as<typename V::value_type,float> && requires(V a) { a.bits(); }
    native_inline constexpr auto operator()(V const & a) const { return native_ops<V>::encode(a); }
  };
  struct decode {
    template<class V> requires std::same_as<typename V::value_type,std::uint32_t> &&
      requires(V a) { V::template rebind<float>::from_bits(a); }
    native_inline constexpr auto operator()(V const & a) const { return native_ops<V>::decode(a); }
  };
  template<unsigned Shift> struct shift_left {
    template<class V> requires requires(V a) { a.template left<Shift>(); }
    native_inline constexpr auto operator()(V const & a) const { return native_ops<V>::template left<Shift>(a); }
  };
  template<unsigned Shift> struct shift_right {
    template<class V> requires requires(V a) { a.template right<Shift>(); }
    native_inline constexpr auto operator()(V const & a) const { return native_ops<V>::template right<Shift>(a); }
  };
  template<class T> struct mask_words {
    template<class V> requires requires(V a) { ::native::mask_bits<T>(a); }
    native_inline constexpr auto operator()(V const & a) const { return native_ops<V>::template mask_words<T>(a); }
  };
  struct trig_integer_operation {
    template<class V> requires std::same_as<typename V::value_type,float>
    native_inline constexpr auto operator()(V const & a) const { return native_ops<V>::trig_integer(a); }
  };
  struct tanh_coefficient {
    std::array<std::uint32_t,8> table;
    template<class V> requires std::same_as<typename V::value_type,std::uint32_t>
    native_inline constexpr auto operator()(V const & index) const noexcept {
      return native_ops<V>::tanh_coefficient(index,table);
    }
  };
  struct signed_float_operation {
    template<class V> requires std::same_as<typename V::value_type,std::uint32_t>
    native_inline constexpr auto operator()(V const & a) const { return native_ops<V>::signed_float(a); }
  };
  template<class P> requires liftable<trig_integer_operation,P>
  native_inline constexpr auto trig_integer(P const & a) noexcept { return lift(trig_integer_operation{},a); }
  template<class P> requires liftable<signed_float_operation,P>
  native_inline constexpr auto signed_float(P const & a) noexcept { return lift(signed_float_operation{},a); }

}

namespace wide {
  /// Construct one SIMD coefficient shared by every register in the array.
  template<class V,std::size_t N,class T>
    requires detail::native_leaf_argument<V> &&
      (std::same_as<T,float> || std::same_as<T,std::uint32_t>) && std::same_as<typename V::value_type,T>
  native_nodiscard native_inline constexpr V constant_like(std::array<V,N> const &, T value) noexcept {
    return detail::native_ops<V>::constant(value);
  }
#define NATIVE_WIDE_BINARY_API(name,operation) \
  template<class P,class Q> requires detail::liftable<detail::operation,P,Q> \
  native_nodiscard native_inline constexpr auto name(P const & a,Q const & b) noexcept { \
    return detail::lift(detail::operation{},a,b); \
  }
  NATIVE_WIDE_BINARY_API(add,add)
  NATIVE_WIDE_BINARY_API(sub,subtract)
  NATIVE_WIDE_BINARY_API(mul,multiply)
  NATIVE_WIDE_BINARY_API(div,divide)
  NATIVE_WIDE_BINARY_API(bit_and,bit_and)
  NATIVE_WIDE_BINARY_API(bit_or,bit_or)
  NATIVE_WIDE_BINARY_API(bit_xor,bit_xor)
  NATIVE_WIDE_BINARY_API(cmp_eq,equal)
  NATIVE_WIDE_BINARY_API(cmp_ne,unequal)
  NATIVE_WIDE_BINARY_API(cmp_lt,less)
  NATIVE_WIDE_BINARY_API(cmp_le,less_equal)
  NATIVE_WIDE_BINARY_API(cmp_gt,greater)
  NATIVE_WIDE_BINARY_API(cmp_ge,greater_equal)
  NATIVE_WIDE_BINARY_API(min,minimum)
  NATIVE_WIDE_BINARY_API(max,maximum)
  NATIVE_WIDE_BINARY_API(scaleb,scale_all)
#undef NATIVE_WIDE_BINARY_API
#define NATIVE_WIDE_UNARY_API(name,operation) \
  template<class P> requires detail::liftable<detail::operation,P> \
  native_nodiscard native_inline constexpr auto name(P const & a) noexcept { \
    return detail::lift(detail::operation{},a); \
  }
  NATIVE_WIDE_UNARY_API(negate,negate)
  NATIVE_WIDE_UNARY_API(bit_not,bit_not)
  NATIVE_WIDE_UNARY_API(mask_not,logical_not)
  NATIVE_WIDE_UNARY_API(abs,absolute)
  NATIVE_WIDE_UNARY_API(sqrt,root)
  NATIVE_WIDE_UNARY_API(floor,downward)
  NATIVE_WIDE_UNARY_API(ceil,upward)
  NATIVE_WIDE_UNARY_API(trunc,truncate)
  NATIVE_WIDE_UNARY_API(round_even,round)
  NATIVE_WIDE_UNARY_API(bits,encode)
  NATIVE_WIDE_UNARY_API(from_bits,decode)
#undef NATIVE_WIDE_UNARY_API
  /// Compute a*b+c with fused rounding in each SIMD lane of the result array.
  /// Array operands have equal lengths; a SIMD operand is shared across them.
  template<class P,class Q,class R> requires detail::liftable<detail::fused,P,Q,R>
  native_nodiscard native_inline constexpr auto fma(P const & a, Q const & b, R const & c) noexcept {
    return detail::lift(detail::fused{},a,b,c);
  }
  /// Scale active lanes of a by 2^floor(n), writing positive zero elsewhere.
  /// Apply the native masked scaling operation at each array position.
  template<class P,class Q,class R> requires detail::liftable<detail::scale,P,Q,R>
  native_nodiscard native_inline constexpr auto masked_scaleb_zero(P const & m, Q const & a, R const & n) noexcept {
    return detail::lift(detail::scale{},m,a,n);
  }
  /// Choose lanes from a where m is true and b elsewhere, at each array position.
  /// Array operands have equal lengths; a SIMD operand is shared across them.
  template<class M,class P,class Q> requires detail::liftable<detail::choose,M,P,Q>
  native_nodiscard native_inline constexpr auto select(M const & m,P const & a,Q const & b) noexcept {
    return detail::lift(detail::choose{},m,a,b);
  }
  /// Scale active lanes of a by 2^floor(n), preserving prior in inactive lanes.
  /// Apply the native masked scaling operation at each array position.
  template<class M,class P,class Q,class R> requires detail::liftable<detail::scale_merge,M,P,Q,R>
  native_nodiscard native_inline constexpr auto masked_scaleb(M const & m,P const & prior,Q const & a,R const & n) noexcept {
    return detail::lift(detail::scale_merge{},m,prior,a,n);
  }
  /// Shift each integer lane left by Shift bits, preserving the array shape.
  /// Participation follows the SIMD element's compile-time shift constraints.
  template<unsigned Shift,class P> requires detail::liftable<detail::shift_left<Shift>,P>
  native_nodiscard native_inline constexpr auto left(P const & a) noexcept {
    return detail::lift(detail::shift_left<Shift>{},a);
  }
  /// Shift each integer lane right by Shift bits, preserving the array shape.
  template<unsigned Shift,class P> requires detail::liftable<detail::shift_right<Shift>,P>
  native_nodiscard native_inline constexpr auto right(P const & a) noexcept {
    return detail::lift(detail::shift_right<Shift>{},a);
  }
  /// Expand each mask lane to an unsigned integer zero/all-one word for T.
  /// Preserve the array shape and each SIMD element's lane count.
  template<class T,class P> requires detail::liftable<detail::mask_words<T>,P>
  native_nodiscard native_inline constexpr auto mask_bits(P const & a) noexcept {
    return detail::lift(detail::mask_words<T>{},a);
  }

  namespace detail {
    template<class T> inline constexpr bool binary32_register = false;
    template<std::size_t N, ::native::isa<> A>
    inline constexpr bool binary32_register<::native::simd<float, N, A>> =
      requires(::native::simd<float,N,A> a) { { a+a } -> std::same_as<::native::simd<float,N,A>>; };
    template<class P> inline constexpr bool binary32_array = false;
    template<class V, std::size_t N>
    inline constexpr bool binary32_array<std::array<V, N>> = binary32_register<V>;
    template<class P> inline constexpr bool binary32_pack = binary32_array<P>;
  }
}

namespace math {
  namespace detail {
    template<class T, class C> concept horner_coefficient = std::same_as<C, float> ||
      std::same_as<C, typename ::wide::detail::shape_t<::wide::canonical_t<T>>::template element_type<0>>;

    template<class P, class C0, class C1, class... C>
    native_inline constexpr auto horner_kernel(P const & z, C0 const & first,
        C1 const & second, C const &... rest) noexcept {
      auto h = ::wide::detail::madd(first, z, second);
      ((h = ::wide::detail::madd(h, z, rest)), ...);
      return h;
    }

    template<class T, class C0, class... C>
    native_inline constexpr auto evaluate_horner(T const & z,
        C0 const & first, C const &... rest) noexcept {
      auto const input = ::wide::promote(z);
      auto const coefficient = [&](auto const & value) {
        if constexpr (std::same_as<std::remove_cvref_t<decltype(value)>, float>)
          return ::wide::constant_like(input, value);
        else return value;
      };
      if constexpr (sizeof...(C) == 0) {
        auto const value = coefficient(first);
        return ::wide::demote<T>(::wide::map([&](auto const &) { return value; }, input));
      } else {
        return ::wide::demote<T>(horner_kernel(input,
          coefficient(first), coefficient(rest)...));
      }
    }

    template<class... C> struct horner_polynomial {
      std::tuple<C...> coefficients;

      template<::wide::promotable T>
        requires (::wide::detail::binary32_array<::wide::canonical_t<T>>) &&
          (horner_coefficient<T, C> && ...)
      native_nodiscard native_inline constexpr auto operator()(T const & z) const noexcept {
        auto const & [...values] = coefficients;
        return evaluate_horner(z, values...);
      }
    };
  }

  /// Own a binary32 polynomial's coefficients in descending power order.
  /// Calling the result preserves the argument's scalar, SIMD, array or wide
  /// shape. Coefficients are floats or matching SIMD values shared across the
  /// pack. Require at least one; a constant polynomial performs no arithmetic.
  /// Each remaining coefficient adds one multiply-add, fused where supported
  /// and separate on baseline Wasm SIMD.
  template<class C0, class... C>
    requires (std::same_as<C0, float> || ::wide::detail::binary32_register<C0>) &&
      ((std::same_as<C, float> || ::wide::detail::binary32_register<C>) && ...)
  native_nodiscard native_inline constexpr auto horner(C0 first, C... rest) noexcept {
    return detail::horner_polynomial<C0, C...>{{first, rest...}};
  }

  namespace detail {
    // The single polynomial body, shared by generic and targeted entry points.
    template<bool Flush, class V, std::size_t N>
      requires (::wide::detail::binary32_register<V>)
    native_nodiscard native_inline constexpr auto exp_reduced(std::array<V, N> const & x) noexcept {
      auto const c = [&](float value) { return ::wide::constant_like(x, value); };
      auto const active = ::wide::mask_not(::wide::cmp_lt(x, c(Flush ? -87.33654022216796875f : -104.f)));
      // Classify independently; range flags are consumed only at the finish.
      // Unordered comparisons leave NaNs on the arithmetic propagation path.
      // Compare with the last binary32 input before n=128. Strict comparison
      // keeps NaNs on the arithmetic path and needs only one lane comparison.
      auto const overflow = ::wide::cmp_gt(x, c(88.37625885009765625f));
      auto const in_range = ::wide::bit_and(active, ::wide::mask_not(overflow));
      auto const replacement = ::wide::select(overflow,
        c(std::bit_cast<float>(0x7f800000u)), c(0.f));
      auto r = x;
      auto const n = ::wide::round_even(::wide::mul(r, c(1.4426950408889634f)));
      r = ::wide::detail::madd(n, c(-0x1.62e400p-1f), r);
      r = ::wide::detail::madd(n, c(-0x1.7f7d1cp-20f), r);

      auto y = ::wide::detail::madd(r, c(0x1.a1d714d7b1510dp-13f), c(0x1.6da756e670ea6p-10f));
      y = ::wide::detail::madd(r, y, c(0x1.11105b3161a6fp-7f));
      y = ::wide::detail::madd(r, y, c(0x1.5554649b7487fp-5f));
      y = ::wide::detail::madd(r, y, c(0x1.555555c673724p-3f));
      y = ::wide::detail::madd(r, y, c(0x1.0000005c8dd89p-1f));
      auto const one = c(1.f);
      y = ::wide::detail::madd(r, y, one);
      y = ::wide::detail::madd(r, y, one);
      return std::tuple{in_range, replacement, y, n};
    }
  }

  /// Evaluate exp through a homogeneous SIMD array and restore the input shape.
  /// Scalar and SIMD inputs promote to singleton arrays; tuples are unsupported.
  template<bool Flush = false, ::wide::promotable T>
    requires (::wide::detail::binary32_array<::wide::canonical_t<T>>)
  native_nodiscard native_inline constexpr auto exp(T const & input) noexcept {
    // MSVC's array<T,0> may construct a dummy T; an empty batch needs no work.
    if constexpr (::wide::detail::shape_t<::wide::canonical_t<T>>::size == 0) {
      return std::remove_cvref_t<T>(input);
    } else {
      auto const x = ::wide::promote(input);
      auto const [in_range, replacement, y, n] = detail::exp_reduced<Flush>(x);
      return ::wide::demote<T>(::wide::detail::scale_exp(in_range, replacement, y, n));
    }
  }
}
// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: (BSD-2-Clause OR Apache-2.0) AND BSL-1.0
// Adapted from FTZ's seven-interval tanh polynomial, with native arithmetic.
namespace math::detail {
  template<class V,std::size_t N> requires (::wide::detail::binary32_register<V>)
  native_nodiscard native_inline constexpr auto tanh_kernel(std::array<V,N> const & input) noexcept {
    namespace w=::wide;
    auto const word=w::bits(input);
    auto const u=[&](std::uint32_t value) {return w::constant_like(word,value);};
    auto const magnitude=w::bit_and(word,u(0x7fffffffu));
    // Bound large inputs before squaring; the tiny-region result below keeps
    // original words, including subnormals under the caller's FP environment.
    auto const safe=w::select(w::cmp_lt(magnitude,u(0x41200000u)),magnitude,u(0x3f800000u));
    auto const x=w::from_bits(safe);
    auto const z=w::mul(x,x);
    auto const interval=w::select(w::cmp_le(safe,u(0x40800000u)),
      w::select(w::cmp_le(safe,u(0x40000000u)),
        w::select(w::cmp_le(safe,u(0x3f800000u)),u(0),u(1)),
        w::select(w::cmp_le(safe,u(0x40400000u)),u(2),u(3))),
      w::select(w::cmp_le(safe,u(0x41000000u)),
        w::select(w::cmp_le(safe,u(0x40c00000u)),u(4),u(5)),u(6)));
    auto const coefficient=[&](std::uint32_t c0,std::uint32_t c1,std::uint32_t c2,
        std::uint32_t c3,std::uint32_t c4,std::uint32_t c5,std::uint32_t c6) {
      return w::detail::lift(w::detail::tanh_coefficient{{c0,c1,c2,c3,c4,c5,c6,c6}},interval);
    };
    auto const t=w::detail::madd(z,
      coefficient(0x40000000u,0x3f000000u,0x3e800000u,0x3e800000u,0x3d800000u,0x3d800000u,0x3d000000u),
      coefficient(0xbf800000u,0xbfa00000u,0xbfd00000u,0xc0480000u,0xbfd00000u,0xc0480000u,0xc0240000u));
    auto h=coefficient(0x00000000u,0x00000000u,0x00000000u,0x00000000u,0xb9405fbfu,0x00000000u,0x00000000u);
    h=w::detail::madd(h,t,coefficient(0x00000000u,0x38752140u,0x00000000u,0x00000000u,0x39ab4d5fu,0x00000000u,0x00000000u));
    h=w::detail::madd(h,t,coefficient(0x00000000u,0xb9183513u,0xb947bd66u,0xb58f6d10u,0xb9c08feeu,0xb59171b1u,0x00000000u));
    h=w::detail::madd(h,t,coefficient(0x34facb37u,0x398d9ee0u,0x39dfe5a4u,0x36863471u,0x3a2bf5fau,0x3671d749u,0x00000000u));
    h=w::detail::madd(h,t,coefficient(0xb63a0d2du,0xba2fdf3au,0xba4a3864u,0xb758ec9du,0xbaa65b2au,0xb72757e1u,0xb811c415u));
    h=w::detail::madd(h,t,coefficient(0x37813497u,0x3ae1130du,0x3ae2b84bu,0x384b3fb7u,0x3b1584c1u,0x380d4deeu,0x38c8e73cu));
    h=w::detail::madd(h,t,coefficient(0xb8bfb3f8u,0xbb8bc302u,0xbb813c08u,0xb9404721u,0xbb86bc79u,0xb8f4646bu,0xb980a2b8u));
    h=w::detail::madd(h,t,coefficient(0x3a0e6d24u,0x3c2d6773u,0x3c1129ceu,0x3a356f7bu,0x3bf71905u,0x39d46b54u,0x3a37296bu));
    h=w::detail::madd(h,t,coefficient(0xbb535f6cu,0xbcd7a178u,0xbca3c0b8u,0xbb2d3d49u,0xbc67da1fu,0xbabdbc02u,0xbb06690eu));
    h=w::detail::madd(h,t,coefficient(0x3c9d20e4u,0x3d86d45du,0x3d3bcdf4u,0x3c2a7e14u,0x3ce35ff2u,0x3bb1ed8au,0x3bcea86au));
    h=w::detail::madd(h,t,coefficient(0xbded544du,0xbe2e2df9u,0xbde4f8bcu,0xbd36d397u,0xbd76f5e5u,0xbcb95c19u,0xbcb08499u));
    h=w::detail::madd(h,t,coefficient(0x3f5c6e3eu,0x3f14c222u,0x3ec662fcu,0x3e9091d7u,0x3e48ced7u,0x3e10d0b5u,0x3de229ecu));

    auto result=w::bits(w::mul(x,h));
    auto const sign=w::bit_and(word,u(0x80000000u));
    result=w::bit_or(w::min(result,u(0x3f800000u)),sign);
    result=w::select(w::cmp_ge(magnitude,u(0x41200000u)),w::bit_or(sign,u(0x3f800000u)),result);
    result=w::select(w::cmp_le(magnitude,u(0x39800000u)),word,result);
    result=w::select(w::cmp_gt(magnitude,u(0x7f800000u)),u(0x7fc00000u),result);
    return w::from_bits(result);
  }
}
namespace math {
  /// Hyperbolic tangent; preserve signed zero and tiny inputs, and saturate
  /// infinities to signed one. NaNs return a quiet NaN without a payload promise.
  template<::wide::promotable T> requires (::wide::detail::binary32_array<::wide::canonical_t<T>>)
  native_nodiscard native_inline constexpr auto tanh(T const & input) noexcept {
    if constexpr (::wide::detail::shape_t<::wide::canonical_t<T>>::size==0) return std::remove_cvref_t<T>(input);
    else return ::wide::demote<T>(detail::tanh_kernel(::wide::promote(input)));
  }
}

// Adapted coefficient use from SLEEF 3.9.0 atan2kf; Boost 1.0 notice below.
namespace math::detail {
  // Raw subnormal inputs are signed zero. Normal operands use one packed
  // min/max ratio, with no software flushing or changes to FP controls.
  // A tiny finite result may underflow according to the caller's FP mode.
  template<class V, std::size_t N>
    requires (::wide::detail::binary32_register<V>)
  native_nodiscard native_inline constexpr auto atan2_kernel(
      std::array<V, N> const & y, std::array<V, N> const & x) noexcept {
    namespace w = ::wide;
    auto const c = [&](float value) { return w::constant_like(y, value); };
    auto const yw = w::bits(y), xw = w::bits(x);
    auto const u = [&](std::uint32_t value) { return w::constant_like(yw, value); };
    auto const f = [&](std::uint32_t value) { return c(std::bit_cast<float>(value)); };
    auto ay = w::bit_and(yw, u(0x7fffffffu));
    auto ax = w::bit_and(xw, u(0x7fffffffu));
    ay = w::select(w::cmp_lt(ay, u(0x00800000u)), u(0), ay);
    ax = w::select(w::cmp_lt(ax, u(0x00800000u)), u(0), ax);
    auto const swap = w::cmp_gt(ay, ax);
    auto const negative_x = w::cmp_ne(w::bit_and(xw, u(0x80000000u)), u(0));
    auto const a = w::select(swap, ax, ay);
    auto const b = w::select(swap, ay, ax);
    auto const ratio = w::div(w::from_bits(a), w::from_bits(b));
    auto const tiny = w::cmp_le(w::bits(ratio), u(0x39800000u));
    auto const z = w::mul(ratio, ratio);
    auto const h = ::math::horner(
      f(0x3b390ccdu), f(0xbc82b80du), f(0x3d2e19b6u), f(0xbd995ffau),
      f(0x3dd9ccf2u), f(0xbe116f9fu), f(0x3e4cb9a7u), f(0xbeaaaa5du))(z);
    auto angle = w::detail::madd(w::mul(z, h), ratio, ratio);
    angle = w::select(tiny, ratio, angle);
    angle = w::select(swap, w::sub(f(0x3fc90fdbu), angle), angle);
    angle = w::select(negative_x, w::sub(f(0x40490fdbu), angle), angle);
    auto result = w::bits(angle);
    auto const axis = w::select(negative_x, u(0x40490fdbu), u(0));
    result = w::select(w::cmp_eq(ax, u(0)), u(0x3fc90fdbu), result);
    result = w::select(w::cmp_eq(ax, u(0x7f800000u)), axis, result);
    result = w::select(w::cmp_eq(ay, u(0x7f800000u)),
      w::select(w::cmp_eq(ax, u(0x7f800000u)),
        w::select(negative_x, u(0x4016cbe4u), u(0x3f490fdbu)), u(0x3fc90fdbu)), result);
    result = w::select(w::cmp_eq(ay, u(0)), axis, result);
    result = w::bit_or(result, w::bit_and(yw, u(0x80000000u)));
    result = w::select(w::bit_or(w::cmp_gt(ay, u(0x7f800000u)),
      w::cmp_gt(ax, u(0x7f800000u))), u(0x7fc00000u), result);
    return w::from_bits(result);
  }
}
namespace math {
  /// atan2(y,x) in radians, preserving the common operand shape and signed axes.
  /// Subnormal inputs are signed zero; tiny outputs follow the caller's FP mode.
  template<::wide::promotable T>
    requires (::wide::detail::binary32_array<::wide::canonical_t<T>>)
  native_nodiscard native_inline constexpr auto atan2(T const & y, T const & x) noexcept {
    if constexpr (::wide::detail::shape_t<::wide::canonical_t<T>>::size == 0)
      return std::remove_cvref_t<T>(y);
    else return ::wide::demote<T>(detail::atan2_kernel(::wide::promote(y), ::wide::promote(x)));
  }
}

/*
Copyright Naoki Shibata and contributors 2010 - 2025.
Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

// Adapted from FTZ's cancellation-safe expm1 and reduced log/log1p graphs.
namespace math {
  namespace detail {
    template<bool Gain, class V, std::size_t N>
      requires (::wide::detail::binary32_register<V>)
    native_nodiscard native_inline constexpr auto expm1_kernel(
        std::array<V, N> const & input) noexcept {
      if constexpr (N == 0) return input;
      else {
        auto const c = [&](float value) { return ::wide::constant_like(input, value); };
        auto const w = [&](std::uint32_t bits) { return c(std::bit_cast<float>(bits)); };
        auto const x = [&] {
          if constexpr (Gain) return ::wide::negate(input);
          else return input;
        }();
        auto const n = ::wide::round_even(::wide::mul(x, w(0x3fb8aa3bu)));
        auto const n_zero = ::wide::cmp_eq(n, c(0.f));
        auto r = ::wide::detail::madd(n, w(0xbf317200u), x);
        r = ::wide::detail::madd(n, w(0xb5bfbe8eu), r);
        r = ::wide::select(n_zero, x, r);
        auto const z = ::wide::mul(r, r);
        auto h = ::wide::detail::madd(r, w(0x3493f27eu), w(0x3638ef1du));
        h = ::wide::detail::madd(r, h, w(0x37d00d01u));
        h = ::wide::detail::madd(r, h, w(0x39500d01u));
        h = ::wide::detail::madd(r, h, w(0x3ab60b61u));
        h = ::wide::detail::madd(r, h, w(0x3c088889u));
        h = ::wide::detail::madd(r, h, w(0x3d2aaaabu));
        h = ::wide::detail::madd(r, h, w(0x3e2aaaabu));
        h = ::wide::detail::madd(r, h, w(0x3f000000u));
        auto p = ::wide::detail::madd(z, h, r);
        auto const bits = ::wide::bits(r);
        auto const u = [&](std::uint32_t value) { return ::wide::constant_like(bits, value); };
        auto const not_tiny = ::wide::cmp_gt(::wide::bit_and(bits, u(0x7fffffffu)), u(0x33000000u));
        p = ::wide::select(not_tiny, p, r);

        // Only the reconstruction factor is bounded below; the input and
        // polynomial are untouched. max's unordered case supplies -24 so a NaN
        // polynomial propagates while the factor remains well-defined.
        auto const factor_n = ::wide::max(n, c(-24.f));
        auto const scale = ::wide::detail::exp_power(factor_n);
        auto result = ::wide::detail::madd(scale, p, ::wide::sub(scale, c(1.f)));
        result = ::wide::select(n_zero, p, result);
        result = ::wide::select(::wide::cmp_eq(n, c(-25.f)),
          ::wide::select(::wide::cmp_gt(p, c(0.f)), w(0xbf7fffffu), c(-1.f)), result);
        result = ::wide::select(::wide::cmp_lt(n, c(-25.f)), c(-1.f), result);
        // Retain general exp's accepted early positive overflow threshold.
        result = ::wide::select(::wide::cmp_gt(x, c(88.37625885009765625f)),
          w(0x7f800000u), result);
        if constexpr (Gain) return ::wide::negate(result);
        else return result;
      }
    }
  }

  /// Cancellation-safe exp(x)-1. Preserve input shape and signed zero;
  /// NaNs propagate, negative infinity maps to -1, and early overflow follows exp.
  template<::wide::promotable T>
    requires (::wide::detail::binary32_array<::wide::canonical_t<T>>)
  native_nodiscard native_inline constexpr auto expm1(T const & input) noexcept {
    if constexpr (::wide::detail::shape_t<::wide::canonical_t<T>>::size == 0)
      return std::remove_cvref_t<T>(input);
    else return ::wide::demote<T>(detail::expm1_kernel<false>(::wide::promote(input)));
  }

  /// Evaluate -expm1(-x) with the same cancellation-safe graph.
  /// Nonnegative damping inputs approach one; no domain-admission protocol.
  template<::wide::promotable T>
    requires (::wide::detail::binary32_array<::wide::canonical_t<T>>)
  native_nodiscard native_inline constexpr auto damping_gain(T const & input) noexcept {
    if constexpr (::wide::detail::shape_t<::wide::canonical_t<T>>::size == 0)
      return std::remove_cvref_t<T>(input);
    else return ::wide::demote<T>(detail::expm1_kernel<true>(::wide::promote(input)));
  }
}

namespace math::detail {
  // Normal inputs retain the FTZ hardware polynomial and reduction graph.
  // There is no software flushing between arithmetic operations and no FP
  // control change. log treats subnormal inputs as signed zero; log1p returns
  // the original bits for |x|<=2^-25, including signed zero and subnormals.
  // NaNs and domain errors return the FTZ graph's canonical quiet NaN.
  template<bool OnePlus, class V, std::size_t N>
    requires (::wide::detail::binary32_register<V>)
  native_nodiscard native_inline constexpr auto log_kernel(std::array<V, N> const & input) noexcept {
    namespace w = ::wide;
    auto const c = [&](float value) { return w::constant_like(input, value); };
    auto const word = w::bits(input);
    auto const u = [&](std::uint32_t value) { return w::constant_like(word, value); };
    auto const magnitude = w::bit_and(word, u(0x7fffffffu));
    auto const sign = w::bit_and(word, u(0x80000000u));
    auto const valid = [&] {
      if constexpr (OnePlus) return w::bit_and(w::cmp_lt(magnitude, u(0x7f800000u)),
        w::bit_or(w::cmp_eq(sign, u(0)), w::cmp_lt(magnitude, u(0x3f800000u))));
      else return w::bit_and(w::cmp_ge(word, u(0x00800000u)), w::cmp_lt(word, u(0x7f800000u)));
    }();
    auto const direct = [&] {
      if constexpr (OnePlus) return w::bit_and(valid, w::cmp_le(magnitude,
        w::select(w::cmp_ne(sign, u(0)), u(0x3f000000u), u(0x3f800000u))));
      else return valid;
    }();
    auto const positive = [&] {
      if constexpr (OnePlus) return w::bits(w::add(c(1.f), w::from_bits(
        w::select(w::bit_and(valid, w::mask_not(direct)), word, u(0)))));
      else return w::select(valid, word, u(0x3f800000u));
    }();
    auto exponent = w::sub(w::right<23>(positive), u(127));
    auto mantissa = w::bit_or(w::bit_and(positive, u(0x007fffffu)), u(0x3f800000u));
    auto const upper = w::cmp_ge(mantissa, u(0x3fc00000u));
    mantissa = w::sub(mantissa, w::select(upper, u(0x00800000u), u(0)));
    exponent = w::add(exponent, w::select(upper, u(1), u(0)));
    auto const reduced = w::sub(w::from_bits(mantissa), c(1.f));
    auto const argument = [&] {
      if constexpr (OnePlus) return w::select(direct, input, reduced);
      else return reduced;
    }();
    auto const negative = w::cmp_ne(w::bit_and(w::bits(argument), u(0x80000000u)), u(0));
    auto const coefficient = [&](std::uint32_t negative_word, std::uint32_t positive_word) {
      return w::select(negative, c(std::bit_cast<float>(negative_word)), c(std::bit_cast<float>(positive_word)));
    };
    auto const square = w::mul(argument, argument);
    auto const t = w::detail::madd(argument, coefficient(0x40800000u, 0x40000000u),
      coefficient(0x3f800000u, 0xbf800000u));
    auto h = coefficient(0x00000000u, 0xb29c7ee2u);
    h = w::detail::madd(h, t, coefficient(0x00000000u, 0x3378ea39u));
    h = w::detail::madd(h, t, coefficient(0xb44f5480u, 0xb3faaccbu));
    h = w::detail::madd(h, t, coefficient(0x352754efu, 0x34c9e1cdu));
    h = w::detail::madd(h, t, coefficient(0xb5bb75dbu, 0xb5b13b5eu));
    h = w::detail::madd(h, t, coefficient(0x369a1c19u, 0x36902a0au));
    h = w::detail::madd(h, t, coefficient(0xb7866f43u, 0xb76af011u));
    h = w::detail::madd(h, t, coefficient(0x3861235au, 0x38423d8au));
    h = w::detail::madd(h, t, coefficient(0xb93e98dfu, 0xb9225d51u));
    h = w::detail::madd(h, t, coefficient(0x3a24a041u, 0x3a0988b0u));
    h = w::detail::madd(h, t, coefficient(0xbb117f6au, 0xbaed1a41u));
    h = w::detail::madd(h, t, coefficient(0x3c04b7c5u, 0x3bd13ce0u));
    h = w::detail::madd(h, t, coefficient(0xbcfda364u, 0xbcbeef90u));
    h = w::detail::madd(h, t, coefficient(0x3e029133u, 0x3db786beu));
    h = w::detail::madd(h, t, coefficient(0xbf1a5884u, 0xbec19b82u));
    auto const polynomial = w::detail::madd(square, h, argument);
    auto const e = w::detail::signed_float(exponent);
    auto const low = w::detail::madd(e, c(std::bit_cast<float>(0x35bfbe8eu)), polynomial);
    auto result = w::bits(w::detail::madd(e, c(std::bit_cast<float>(0x3f317200u)), low));
    if constexpr (OnePlus) {
      result = w::select(direct, w::bits(polynomial), result);
      result = w::select(w::cmp_le(magnitude, u(0x33000000u)), word, result);
      result = w::select(w::cmp_eq(word, u(0xbf800000u)), u(0xff800000u), result);
      result = w::select(w::cmp_eq(word, u(0x7f800000u)), word, result);
      result = w::select(w::bit_or(w::cmp_gt(magnitude, u(0x7f800000u)),
        w::bit_and(w::cmp_ne(sign, u(0)), w::cmp_gt(magnitude, u(0x3f800000u)))), u(0x7fc00000u), result);
    } else {
      result = w::select(w::cmp_eq(word, u(0x7f800000u)), word, result);
      result = w::select(w::cmp_ne(sign, u(0)), u(0x7fc00000u), result);
      result = w::select(w::cmp_lt(magnitude, u(0x00800000u)), u(0xff800000u), result);
      result = w::select(w::cmp_gt(magnitude, u(0x7f800000u)), u(0x7fc00000u), result);
    }
    return w::from_bits(result);
  }
}
namespace math {
  /// Natural logarithm; subnormal inputs are treated as signed zero.
  template<::wide::promotable T> requires (::wide::detail::binary32_array<::wide::canonical_t<T>>)
  native_nodiscard native_inline constexpr auto log(T const & input) noexcept {
    if constexpr (::wide::detail::shape_t<::wide::canonical_t<T>>::size == 0) return std::remove_cvref_t<T>(input);
    else return ::wide::demote<T>(detail::log_kernel<false>(::wide::promote(input)));
  }
  /// Cancellation-safe log(1+x), preserving signed zero and tiny inputs.
  template<::wide::promotable T> requires (::wide::detail::binary32_array<::wide::canonical_t<T>>)
  native_nodiscard native_inline constexpr auto log1p(T const & input) noexcept {
    if constexpr (::wide::detail::shape_t<::wide::canonical_t<T>>::size == 0) return std::remove_cvref_t<T>(input);
    else return ::wide::demote<T>(detail::log_kernel<true>(::wide::promote(input)));
  }
}

// SPDX-FileCopyrightText: 2012 Giovanni Garberoglio
// SPDX-FileCopyrightText: 2017 Edward Kmett
// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: Zlib AND BSD-2-Clause
// Included by wide_math.h after the lifted native operations.
// Altered source: the original trig reducer and polynomials now operate on
// standard arrays, preserving each binary32 dependency stage.

namespace math {
  namespace detail {
    enum class trig_kind { sine, cosine, paired };

    // Precondition: every lane is finite and |x| < 8192.
    template<trig_kind K, ::wide::pack P>
    native_nodiscard native_inline constexpr auto trig(P const & original) noexcept {
      auto const encoded = ::wide::bits(original);
      auto const c = [&](float value) { return ::wide::constant_like(original, value); };
      auto const i = [&](std::uint32_t value) { return ::wide::constant_like(encoded, value); };
      auto sign_sine = ::wide::bit_and(encoded, i(0x80000000u));
      auto x = ::wide::from_bits(::wide::bit_and(encoded, i(0x7fffffffu)));
      auto y = ::wide::mul(x, c(1.27323954473516f));
      auto const j = ::wide::bit_and(::wide::add(::wide::detail::trig_integer(y), i(1)), i(0xfffffffeu));
      y = ::wide::detail::signed_float(j);
      sign_sine = ::wide::bit_xor(sign_sine, ::wide::left<29>(::wide::bit_and(j, i(4))));
      auto const sign_cosine = ::wide::left<29>(::wide::bit_and(::wide::bit_xor(::wide::sub(j, i(2)), i(0xffffffffu)), i(4)));
      auto quadrant = j;
      if constexpr (K == trig_kind::cosine) quadrant = ::wide::sub(quadrant, i(2));
      auto const mask = ::wide::mask_bits<std::uint32_t>(::wide::cmp_eq(::wide::bit_and(quadrant, i(2)), i(0)));
      x = ::wide::detail::madd(y, c(-0.78515625f), x);
      x = ::wide::detail::madd(y, c(-2.4187564849853515625e-4f), x);
      x = ::wide::detail::madd(y, c(-3.77489497744594108e-8f), x);
      auto const z = ::wide::mul(x, x);
      auto cosine = ::wide::detail::madd(c(2.443315711809948e-5f), z, c(-1.388731625493765e-3f));
      cosine = ::wide::detail::madd(cosine, z, c(4.166664568298827e-2f));
      cosine = ::wide::mul(cosine, z);
      cosine = ::wide::mul(cosine, z);
      cosine = ::wide::sub(cosine, ::wide::mul(z, c(0.5f)));
      cosine = ::wide::add(cosine, c(1.f));
      auto sine = ::wide::detail::madd(c(-1.9515295891e-4f), z, c(8.3321608736e-3f));
      sine = ::wide::detail::madd(sine, z, c(-1.6666654611e-1f));
      sine = ::wide::mul(sine, z);
      sine = ::wide::detail::madd(sine, x, x);
      auto selected_sine = ::wide::from_bits(::wide::bit_and(mask, ::wide::bits(sine)));
      auto selected_cosine = ::wide::from_bits(::wide::bit_and(::wide::bit_xor(mask, i(0xffffffffu)), ::wide::bits(cosine)));
      if constexpr (K == trig_kind::paired) {
        // Preserve subtraction selection, including its signed-zero effects.
        sine = ::wide::sub(sine, selected_sine);
        cosine = ::wide::sub(cosine, selected_cosine);
        selected_sine = ::wide::from_bits(::wide::bit_xor(::wide::bits(::wide::add(selected_cosine, selected_sine)), sign_sine));
        selected_cosine = ::wide::from_bits(::wide::bit_xor(::wide::bits(::wide::add(cosine, sine)), sign_cosine));
        return std::pair{selected_sine, selected_cosine};
      } else if constexpr (K == trig_kind::sine)
        return ::wide::from_bits(::wide::bit_xor(::wide::bits(::wide::add(selected_cosine, selected_sine)), sign_sine));
      else return ::wide::from_bits(::wide::bit_xor(::wide::bits(::wide::add(selected_cosine, selected_sine)), sign_cosine));
    }

    template<trig_kind K, ::wide::promotable T>
      requires (::wide::detail::binary32_pack<::wide::canonical_t<T>>)
    native_nodiscard native_inline constexpr auto trig_result(T const & input) noexcept {
      if constexpr (::wide::detail::shape_t<::wide::canonical_t<T>>::size == 0) {
        if constexpr (K == trig_kind::paired)
          return std::pair{std::remove_cvref_t<T>(input), std::remove_cvref_t<T>(input)};
        else return std::remove_cvref_t<T>(input);
      } else if constexpr (K == trig_kind::paired) {
        auto const [sine, cosine] = trig<K>(::wide::promote(input));
        return std::pair{::wide::demote<T>(sine), ::wide::demote<T>(cosine)};
      } else return ::wide::demote<T>(trig<K>(::wide::promote(input)));
    }
  }

  /// Sine in radians; finite binary32 lanes with |x| < 8192.
  template<::wide::promotable T> requires (::wide::detail::binary32_pack<::wide::canonical_t<T>>)
  native_nodiscard native_inline constexpr auto sin(T const & input) noexcept {
    return detail::trig_result<detail::trig_kind::sine>(input);
  }
  /// Cosine in radians; finite binary32 lanes with |x| < 8192.
  template<::wide::promotable T> requires (::wide::detail::binary32_pack<::wide::canonical_t<T>>)
  native_nodiscard native_inline constexpr auto cos(T const & input) noexcept {
    return detail::trig_result<detail::trig_kind::cosine>(input);
  }
  /// Paired sine/cosine sharing one reducer; each result retains the input shape.
  template<::wide::promotable T> requires (::wide::detail::binary32_pack<::wide::canonical_t<T>>)
  native_nodiscard native_inline constexpr auto sincos(T const & input) noexcept {
    return detail::trig_result<detail::trig_kind::paired>(input);
  }

  /// Replace subnormal binary32 lanes with signed zero without changing FP controls.
  template<::wide::promotable T> requires (::wide::detail::binary32_pack<::wide::canonical_t<T>>)
  native_nodiscard native_inline constexpr auto flush_to_zero(T const & input) noexcept {
    if constexpr (::wide::detail::shape_t<::wide::canonical_t<T>>::size == 0)
      return std::remove_cvref_t<T>(input);
    else {
      auto bits = ::wide::bits(::wide::promote(input));
      auto const i = [&](std::uint32_t value) { return ::wide::constant_like(bits, value); };
      auto const exponent = ::wide::bit_and(bits, i(0x7f800000u));
      auto const zero = ::wide::mask_bits<std::uint32_t>(::wide::cmp_eq(exponent, i(0)));
      auto const clear = ::wide::bit_and(zero, i(0x007fffffu));
      bits = ::wide::bit_and(bits, ::wide::bit_xor(clear, i(0xffffffffu)));
      return ::wide::demote<T>(::wide::from_bits(bits));
    }
  }

  // These primitives retain their native leaf semantics inside one wide stage.
#define NATIVE_PROMOTED_UNARY(name) \
  template<::wide::promotable T> requires (::wide::detail::binary32_pack<::wide::canonical_t<T>>) \
  native_nodiscard native_inline constexpr auto name(T const & input) noexcept { \
    if constexpr (::wide::detail::shape_t<::wide::canonical_t<T>>::size == 0) \
      return std::remove_cvref_t<T>(input); \
    else return ::wide::demote<T>(::wide::name(::wide::promote(input))); \
  }
  NATIVE_PROMOTED_UNARY(abs)
  NATIVE_PROMOTED_UNARY(sqrt)
  NATIVE_PROMOTED_UNARY(floor)
  NATIVE_PROMOTED_UNARY(ceil)
  NATIVE_PROMOTED_UNARY(trunc)
  NATIVE_PROMOTED_UNARY(round_even)
#undef NATIVE_PROMOTED_UNARY
}

namespace wide {
  // Qualified convenience aliases; standard arrays keep their ordinary ADL.
  using ::math::horner;
  using ::math::exp;
  using ::math::expm1;
  using ::math::damping_gain;
  using ::math::log;
  using ::math::log1p;
  using ::math::tanh;
  using ::math::atan2;
  using ::math::sin;
  using ::math::cos;
  using ::math::sincos;
  using ::math::flush_to_zero;
}

/*
   AVX implementation of sin, cos, sincos, exp and log

   Based on "sse_mathfun.h", by Julien Pommier
   http://gruntthepeon.free.fr/ssemath/

   Copyright (C) 2012 Giovanni Garberoglio
   Interdisciplinary Laboratory for Computational Science (LISC)
   Fondazione Bruno Kessler and University of Trento
   via Sommarive, 18
   I-38123 Trento (Italy)

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/* RTS repository license (retained verbatim):
Software License Agreement (BSD 2-Clause License)
========================================

Copyright 2017 Edward Kmett

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL YAHOO! INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
