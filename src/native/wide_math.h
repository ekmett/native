// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
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
  template<class T> struct mask_words {
    template<class V> requires requires(V a) { ::native::mask_bits<T>(a); }
    native_inline constexpr auto operator()(V const & a) const { return native_ops<V>::template mask_words<T>(a); }
  };
  struct trig_integer_operation {
    template<class V> requires std::same_as<typename V::value_type,float>
    native_inline constexpr auto operator()(V const & a) const { return native_ops<V>::trig_integer(a); }
  };
  struct trig_float_operation {
    template<class V> requires std::same_as<typename V::value_type,std::uint32_t>
    native_inline constexpr auto operator()(V const & a) const { return native_ops<V>::trig_float(a); }
  };
  template<class P> requires liftable<trig_integer_operation,P>
  native_inline constexpr auto trig_integer(P const & a) noexcept { return lift(trig_integer_operation{},a); }
  template<class P> requires liftable<trig_float_operation,P>
  native_inline constexpr auto trig_float(P const & a) noexcept { return lift(trig_float_operation{},a); }

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
      y = ::wide::detail::trig_float(j);
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
  using ::math::exp;
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
