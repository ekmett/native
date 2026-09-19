// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Intentionally repeatable; each family owns its force-inline ADL helpers.
  namespace SIMD_WIDE_DETAIL {
    using namespace ::simd::detail;
    template<class T> using wide_element_t = typename wide_traits<T>::element_type;
    template<class A, class B> concept wide_arguments =
      (wide_traits<A>::value || wide_traits<B>::value) &&
      (!wide_traits<A>::value || !wide_traits<B>::value || wide_traits<A>::size == wide_traits<B>::size);
    template<class A, class B> inline constexpr std::size_t wide_size =
      wide_traits<A>::value ? wide_traits<A>::size : wide_traits<B>::size;

    template<std::size_t I, class T>
    simd_inline constexpr decltype(auto) wide_element(T const & value) {
      if constexpr (wide_traits<T>::value) return value.registers[I];
      else return (value);
    }
    template<class A, class B, class F, std::size_t... I>
    simd_inline constexpr auto wide_binary(A const & a, B const & b, F f, std::index_sequence<I...>) {
      using R = std::remove_cvref_t<decltype(f(std::declval<wide_element_t<A> const &>(),
        std::declval<wide_element_t<B> const &>()))>;
      return wide<R, sizeof...(I)>{{f(wide_element<I>(a), wide_element<I>(b))...}};
    }
    template<class T, std::size_t N, class F, std::size_t... I>
    simd_inline constexpr auto wide_unary(wide<T, N> const & a, F f, std::index_sequence<I...>) {
      using R = std::remove_cvref_t<decltype(f(std::declval<T const &>()))>;
      return wide<R, N>{{f(a.registers[I])...}};
    }
    template<class T, std::size_t N, class B, class F, std::size_t... I>
    simd_inline constexpr wide<T, N> & wide_mutate(wide<T, N> & a, B const & b, F f, std::index_sequence<I...>) {
      if constexpr (wide_traits<B>::value) (f(a.registers[I], b.registers[I]), ...);
      else if constexpr (N != 0) {
        // A broadcast can alias a destination register. Snapshot it before
        // publishing any lane, just as a register broadcast takes a value.
        auto value = b;
        (f(a.registers[I], value), ...);
      }
      return a;
    }
  }

  namespace SIMD_WIDE_DETAIL {
    using namespace ::simd::detail;
    template<class A> simd_inline constexpr decltype(auto) wide_storage(A const & a) noexcept {
      if constexpr (wide_traits<A>::value) return (a.registers);
      else return (a);
    }
    struct wide_add {
      template<class A,class B> simd_inline constexpr auto operator()(A const & a,B const & b) const
          noexcept(noexcept(a + b)) -> decltype(a + b) { return a + b; }
      template<class A,class B> simd_inline constexpr auto batch(A const & a,B const & b) const
          noexcept(noexcept(add(a,b))) -> decltype(add(a,b)) { return add(a,b); }
    };
    struct wide_sub {
      template<class A,class B> simd_inline constexpr auto operator()(A const & a,B const & b) const
          noexcept(noexcept(a - b)) -> decltype(a - b) { return a - b; }
      template<class A,class B> simd_inline constexpr auto batch(A const & a,B const & b) const
          noexcept(noexcept(sub(a,b))) -> decltype(sub(a,b)) { return sub(a,b); }
    };
    struct wide_mul {
      template<class A,class B> simd_inline constexpr auto operator()(A const & a,B const & b) const
          noexcept(noexcept(a * b)) -> decltype(a * b) { return a * b; }
      template<class A,class B> simd_inline constexpr auto batch(A const & a,B const & b) const
          noexcept(noexcept(mul(a,b))) -> decltype(mul(a,b)) { return mul(a,b); }
    };
    template<class A,class B,class F> concept wide_batch = requires(A const & a,B const & b) {
      wide{F{}.batch(wide_storage(a),wide_storage(b))};
    };
    template<class A,class B,class F> concept wide_batch_assignable = wide_batch<A,B,F> &&
      requires(A & a,B const & b) { a = wide{F{}.batch(wide_storage(a),wide_storage(b))}; };
    template<class A,class B,class F>
    simd_inline constexpr auto wide_dispatch(A const & a,B const & b,F f)
        noexcept([] {
          if constexpr (wide_batch<A,B,F>)
            return noexcept(wide{F{}.batch(wide_storage(std::declval<A const &>()),wide_storage(std::declval<B const &>()))});
          else return false;
        }()) {
      if constexpr (wide_batch<A,B,F>) return wide{f.batch(wide_storage(a),wide_storage(b))};
      else return wide_binary(a,b,f,std::make_index_sequence<wide_size<A,B>>{});
    }
  }

  // Reject non-pack operands before ISA routing: routing itself combines and
  // compares ISA values whose ADL also sees these generic binary operators.
  /// \ingroup wide_values
  /// Apply `+` to corresponding elements; a non-pack argument is broadcast.
  template<class A, class B> requires SIMD_WIDE_DETAIL::wide_arguments<A, B> && (::simd::detail::wide_target<A,B> == SIMD_WIDE_INDEX) &&
    requires(SIMD_WIDE_DETAIL::wide_element_t<A> const & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a + b; }
  simd_nodiscard simd_inline constexpr auto operator+(A const & a, B const & b)
      noexcept(noexcept(SIMD_WIDE_DETAIL::wide_dispatch(a,b,SIMD_WIDE_DETAIL::wide_add{}))) {
    return SIMD_WIDE_DETAIL::wide_dispatch(a,b,SIMD_WIDE_DETAIL::wide_add{});
  }

  /// \ingroup wide_values
  /// Apply `-` to corresponding elements; a non-pack argument is broadcast.
  template<class A, class B> requires SIMD_WIDE_DETAIL::wide_arguments<A, B> && (::simd::detail::wide_target<A,B> == SIMD_WIDE_INDEX) &&
    requires(SIMD_WIDE_DETAIL::wide_element_t<A> const & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a - b; }
  simd_nodiscard simd_inline constexpr auto operator-(A const & a, B const & b)
      noexcept(noexcept(SIMD_WIDE_DETAIL::wide_dispatch(a,b,SIMD_WIDE_DETAIL::wide_sub{}))) {
    return SIMD_WIDE_DETAIL::wide_dispatch(a,b,SIMD_WIDE_DETAIL::wide_sub{});
  }

  /// \ingroup wide_values
  /// Apply `*` to corresponding elements; a non-pack argument is broadcast.
  template<class A, class B> requires SIMD_WIDE_DETAIL::wide_arguments<A, B> && (::simd::detail::wide_target<A,B> == SIMD_WIDE_INDEX) &&
    requires(SIMD_WIDE_DETAIL::wide_element_t<A> const & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a * b; }
  simd_nodiscard simd_inline constexpr auto operator*(A const & a, B const & b)
      noexcept(noexcept(SIMD_WIDE_DETAIL::wide_dispatch(a,b,SIMD_WIDE_DETAIL::wide_mul{}))) {
    return SIMD_WIDE_DETAIL::wide_dispatch(a,b,SIMD_WIDE_DETAIL::wide_mul{});
  }

  /// \ingroup wide_values
  /// Apply `/` to corresponding elements; a non-pack argument is broadcast.
  template<class A, class B> requires SIMD_WIDE_DETAIL::wide_arguments<A, B> && (::simd::detail::wide_target<A,B> == SIMD_WIDE_INDEX) &&
    requires(SIMD_WIDE_DETAIL::wide_element_t<A> const & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a / b; }
  simd_nodiscard simd_inline constexpr auto operator/(A const & a, B const & b) {
    return SIMD_WIDE_DETAIL::wide_binary(a, b, [](auto const & x, auto const & y) -> decltype(x / y) { return x / y; },
      std::make_index_sequence<SIMD_WIDE_DETAIL::wide_size<A, B>>{});
  }

  /// \ingroup wide_values
  /// Apply `%` to corresponding elements; a non-pack argument is broadcast.
  template<class A, class B> requires SIMD_WIDE_DETAIL::wide_arguments<A, B> && (::simd::detail::wide_target<A,B> == SIMD_WIDE_INDEX) &&
    requires(SIMD_WIDE_DETAIL::wide_element_t<A> const & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a % b; }
  simd_nodiscard simd_inline constexpr auto operator%(A const & a, B const & b) {
    return SIMD_WIDE_DETAIL::wide_binary(a, b, [](auto const & x, auto const & y) -> decltype(x % y) { return x % y; },
      std::make_index_sequence<SIMD_WIDE_DETAIL::wide_size<A, B>>{});
  }

  /// \ingroup wide_values
  /// Apply `&` to corresponding elements; a non-pack argument is broadcast.
  template<class A, class B> requires SIMD_WIDE_DETAIL::wide_arguments<A, B> && (::simd::detail::wide_target<A,B> == SIMD_WIDE_INDEX) &&
    requires(SIMD_WIDE_DETAIL::wide_element_t<A> const & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a & b; }
  simd_nodiscard simd_inline constexpr auto operator&(A const & a, B const & b) {
    return SIMD_WIDE_DETAIL::wide_binary(a, b, [](auto const & x, auto const & y) -> decltype(x & y) { return x & y; },
      std::make_index_sequence<SIMD_WIDE_DETAIL::wide_size<A, B>>{});
  }

  /// \ingroup wide_values
  /// Apply `|` to corresponding elements; a non-pack argument is broadcast.
  template<class A, class B> requires SIMD_WIDE_DETAIL::wide_arguments<A, B> && (::simd::detail::wide_target<A,B> == SIMD_WIDE_INDEX) &&
    requires(SIMD_WIDE_DETAIL::wide_element_t<A> const & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a | b; }
  simd_nodiscard simd_inline constexpr auto operator|(A const & a, B const & b) {
    return SIMD_WIDE_DETAIL::wide_binary(a, b, [](auto const & x, auto const & y) -> decltype(x | y) { return x | y; },
      std::make_index_sequence<SIMD_WIDE_DETAIL::wide_size<A, B>>{});
  }

  /// \ingroup wide_values
  /// Apply `^` to corresponding elements; a non-pack argument is broadcast.
  template<class A, class B> requires SIMD_WIDE_DETAIL::wide_arguments<A, B> && (::simd::detail::wide_target<A,B> == SIMD_WIDE_INDEX) &&
    requires(SIMD_WIDE_DETAIL::wide_element_t<A> const & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a ^ b; }
  simd_nodiscard simd_inline constexpr auto operator^(A const & a, B const & b) {
    return SIMD_WIDE_DETAIL::wide_binary(a, b, [](auto const & x, auto const & y) -> decltype(x ^ y) { return x ^ y; },
      std::make_index_sequence<SIMD_WIDE_DETAIL::wide_size<A, B>>{});
  }

  /// \ingroup wide_values
  /// Apply `<<` to corresponding elements; a non-pack argument is broadcast.
  template<class A, class B> requires SIMD_WIDE_DETAIL::wide_arguments<A, B> && (::simd::detail::wide_target<A,B> == SIMD_WIDE_INDEX) &&
    requires(SIMD_WIDE_DETAIL::wide_element_t<A> const & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a << b; }
  simd_nodiscard simd_inline constexpr auto operator<<(A const & a, B const & b) {
    return SIMD_WIDE_DETAIL::wide_binary(a, b, [](auto const & x, auto const & y) -> decltype(x << y) { return x << y; },
      std::make_index_sequence<SIMD_WIDE_DETAIL::wide_size<A, B>>{});
  }

  /// \ingroup wide_values
  /// Apply `>>` to corresponding elements; a non-pack argument is broadcast.
  template<class A, class B> requires SIMD_WIDE_DETAIL::wide_arguments<A, B> && (::simd::detail::wide_target<A,B> == SIMD_WIDE_INDEX) &&
    requires(SIMD_WIDE_DETAIL::wide_element_t<A> const & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a >> b; }
  simd_nodiscard simd_inline constexpr auto operator>>(A const & a, B const & b) {
    return SIMD_WIDE_DETAIL::wide_binary(a, b, [](auto const & x, auto const & y) -> decltype(x >> y) { return x >> y; },
      std::make_index_sequence<SIMD_WIDE_DETAIL::wide_size<A, B>>{});
  }

  /// \ingroup wide_values
  /// Apply `==` to corresponding elements; a non-pack argument is broadcast.
  template<class A, class B> requires SIMD_WIDE_DETAIL::wide_arguments<A, B> && (::simd::detail::wide_target<A,B> == SIMD_WIDE_INDEX) &&
    requires(SIMD_WIDE_DETAIL::wide_element_t<A> const & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a == b; }
  simd_nodiscard simd_inline constexpr auto operator==(A const & a, B const & b) {
    return SIMD_WIDE_DETAIL::wide_binary(a, b, [](auto const & x, auto const & y) -> decltype(x == y) { return x == y; },
      std::make_index_sequence<SIMD_WIDE_DETAIL::wide_size<A, B>>{});
  }

  /// \ingroup wide_values
  /// Apply `!=` to corresponding elements; a non-pack argument is broadcast.
  template<class A, class B> requires SIMD_WIDE_DETAIL::wide_arguments<A, B> && (::simd::detail::wide_target<A,B> == SIMD_WIDE_INDEX) &&
    requires(SIMD_WIDE_DETAIL::wide_element_t<A> const & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a != b; }
  simd_nodiscard simd_inline constexpr auto operator!=(A const & a, B const & b) {
    return SIMD_WIDE_DETAIL::wide_binary(a, b, [](auto const & x, auto const & y) -> decltype(x != y) { return x != y; },
      std::make_index_sequence<SIMD_WIDE_DETAIL::wide_size<A, B>>{});
  }

  /// \ingroup wide_values
  /// Apply `<` to corresponding elements; a non-pack argument is broadcast.
  template<class A, class B> requires SIMD_WIDE_DETAIL::wide_arguments<A, B> && (::simd::detail::wide_target<A,B> == SIMD_WIDE_INDEX) &&
    requires(SIMD_WIDE_DETAIL::wide_element_t<A> const & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a < b; }
  simd_nodiscard simd_inline constexpr auto operator<(A const & a, B const & b) {
    return SIMD_WIDE_DETAIL::wide_binary(a, b, [](auto const & x, auto const & y) -> decltype(x < y) { return x < y; },
      std::make_index_sequence<SIMD_WIDE_DETAIL::wide_size<A, B>>{});
  }

  /// \ingroup wide_values
  /// Apply `>` to corresponding elements; a non-pack argument is broadcast.
  template<class A, class B> requires SIMD_WIDE_DETAIL::wide_arguments<A, B> && (::simd::detail::wide_target<A,B> == SIMD_WIDE_INDEX) &&
    requires(SIMD_WIDE_DETAIL::wide_element_t<A> const & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a > b; }
  simd_nodiscard simd_inline constexpr auto operator>(A const & a, B const & b) {
    return SIMD_WIDE_DETAIL::wide_binary(a, b, [](auto const & x, auto const & y) -> decltype(x > y) { return x > y; },
      std::make_index_sequence<SIMD_WIDE_DETAIL::wide_size<A, B>>{});
  }

  /// \ingroup wide_values
  /// Apply `<=` to corresponding elements; a non-pack argument is broadcast.
  template<class A, class B> requires SIMD_WIDE_DETAIL::wide_arguments<A, B> && (::simd::detail::wide_target<A,B> == SIMD_WIDE_INDEX) &&
    requires(SIMD_WIDE_DETAIL::wide_element_t<A> const & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a <= b; }
  simd_nodiscard simd_inline constexpr auto operator<=(A const & a, B const & b) {
    return SIMD_WIDE_DETAIL::wide_binary(a, b, [](auto const & x, auto const & y) -> decltype(x <= y) { return x <= y; },
      std::make_index_sequence<SIMD_WIDE_DETAIL::wide_size<A, B>>{});
  }

  /// \ingroup wide_values
  /// Apply `>=` to corresponding elements; a non-pack argument is broadcast.
  template<class A, class B> requires SIMD_WIDE_DETAIL::wide_arguments<A, B> && (::simd::detail::wide_target<A,B> == SIMD_WIDE_INDEX) &&
    requires(SIMD_WIDE_DETAIL::wide_element_t<A> const & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a >= b; }
  simd_nodiscard simd_inline constexpr auto operator>=(A const & a, B const & b) {
    return SIMD_WIDE_DETAIL::wide_binary(a, b, [](auto const & x, auto const & y) -> decltype(x >= y) { return x >= y; },
      std::make_index_sequence<SIMD_WIDE_DETAIL::wide_size<A, B>>{});
  }

  /// \ingroup wide_values
  /// Apply unary `+` to each element, retaining its result type.
  template<class T, std::size_t N> requires (::simd::detail::wide_target<T> == SIMD_WIDE_INDEX) && requires(T const & a) { +a; }
  simd_nodiscard simd_inline constexpr auto operator+(wide<T, N> const & a) {
    return SIMD_WIDE_DETAIL::wide_unary(a, [](auto const & x) -> decltype(+x) { return +x; }, std::make_index_sequence<N>{});
  }

  /// \ingroup wide_values
  /// Apply unary `-` to each element, retaining its result type.
  template<class T, std::size_t N> requires (::simd::detail::wide_target<T> == SIMD_WIDE_INDEX) && requires(T const & a) { -a; }
  simd_nodiscard simd_inline constexpr auto operator-(wide<T, N> const & a) {
    return SIMD_WIDE_DETAIL::wide_unary(a, [](auto const & x) -> decltype(-x) { return -x; }, std::make_index_sequence<N>{});
  }

  /// \ingroup wide_values
  /// Apply unary `~` to each element, retaining its result type.
  template<class T, std::size_t N> requires (::simd::detail::wide_target<T> == SIMD_WIDE_INDEX) && requires(T const & a) { ~a; }
  simd_nodiscard simd_inline constexpr auto operator~(wide<T, N> const & a) {
    return SIMD_WIDE_DETAIL::wide_unary(a, [](auto const & x) -> decltype(~x) { return ~x; }, std::make_index_sequence<N>{});
  }

  /// \ingroup wide_values
  /// Apply unary `!` to each element, retaining its result type.
  template<class T, std::size_t N> requires (::simd::detail::wide_target<T> == SIMD_WIDE_INDEX) && requires(T const & a) { !a; }
  simd_nodiscard simd_inline constexpr auto operator!(wide<T, N> const & a) {
    return SIMD_WIDE_DETAIL::wide_unary(a, [](auto const & x) -> decltype(!x) { return !x; }, std::make_index_sequence<N>{});
  }

  /// \ingroup wide_values
  /// Update the left pack with elementwise `+=`; a scalar right side is broadcast.
  template<class T, std::size_t N, class B> requires (::simd::detail::wide_target<T,B> == SIMD_WIDE_INDEX) && SIMD_WIDE_DETAIL::wide_arguments<wide<T, N>, B> &&
    (SIMD_WIDE_DETAIL::wide_batch_assignable<wide<T,N>,B,SIMD_WIDE_DETAIL::wide_add> ||
      ((SIMD_WIDE_DETAIL::wide_traits<B>::value || std::copy_constructible<B>) &&
        requires(T & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a += b; }))
  simd_inline constexpr wide<T, N> & operator+=(wide<T, N> & a, B const & b)
      noexcept([] {
        if constexpr (SIMD_WIDE_DETAIL::wide_batch_assignable<wide<T,N>,B,SIMD_WIDE_DETAIL::wide_add>)
          return noexcept(std::declval<wide<T,N> &>() = SIMD_WIDE_DETAIL::wide_dispatch(
            std::declval<wide<T,N> const &>(),std::declval<B const &>(),SIMD_WIDE_DETAIL::wide_add{}));
        else return false;
      }()) {
    if constexpr (SIMD_WIDE_DETAIL::wide_batch_assignable<wide<T,N>,B,SIMD_WIDE_DETAIL::wide_add>)
      return a = SIMD_WIDE_DETAIL::wide_dispatch(a,b,SIMD_WIDE_DETAIL::wide_add{});
    else return SIMD_WIDE_DETAIL::wide_mutate(a, b, [](auto & x, auto const & y) { x += y; }, std::make_index_sequence<N>{});
  }

  /// \ingroup wide_values
  /// Update the left pack with elementwise `-=`; a scalar right side is broadcast.
  template<class T, std::size_t N, class B> requires (::simd::detail::wide_target<T,B> == SIMD_WIDE_INDEX) && SIMD_WIDE_DETAIL::wide_arguments<wide<T, N>, B> &&
    (SIMD_WIDE_DETAIL::wide_batch_assignable<wide<T,N>,B,SIMD_WIDE_DETAIL::wide_sub> ||
      ((SIMD_WIDE_DETAIL::wide_traits<B>::value || std::copy_constructible<B>) &&
        requires(T & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a -= b; }))
  simd_inline constexpr wide<T, N> & operator-=(wide<T, N> & a, B const & b)
      noexcept([] {
        if constexpr (SIMD_WIDE_DETAIL::wide_batch_assignable<wide<T,N>,B,SIMD_WIDE_DETAIL::wide_sub>)
          return noexcept(std::declval<wide<T,N> &>() = SIMD_WIDE_DETAIL::wide_dispatch(
            std::declval<wide<T,N> const &>(),std::declval<B const &>(),SIMD_WIDE_DETAIL::wide_sub{}));
        else return false;
      }()) {
    if constexpr (SIMD_WIDE_DETAIL::wide_batch_assignable<wide<T,N>,B,SIMD_WIDE_DETAIL::wide_sub>)
      return a = SIMD_WIDE_DETAIL::wide_dispatch(a,b,SIMD_WIDE_DETAIL::wide_sub{});
    else return SIMD_WIDE_DETAIL::wide_mutate(a, b, [](auto & x, auto const & y) { x -= y; }, std::make_index_sequence<N>{});
  }

  /// \ingroup wide_values
  /// Update the left pack with elementwise `*=`; a scalar right side is broadcast.
  template<class T, std::size_t N, class B> requires (::simd::detail::wide_target<T,B> == SIMD_WIDE_INDEX) && SIMD_WIDE_DETAIL::wide_arguments<wide<T, N>, B> &&
    (SIMD_WIDE_DETAIL::wide_batch_assignable<wide<T,N>,B,SIMD_WIDE_DETAIL::wide_mul> ||
      ((SIMD_WIDE_DETAIL::wide_traits<B>::value || std::copy_constructible<B>) &&
        requires(T & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a *= b; }))
  simd_inline constexpr wide<T, N> & operator*=(wide<T, N> & a, B const & b)
      noexcept([] {
        if constexpr (SIMD_WIDE_DETAIL::wide_batch_assignable<wide<T,N>,B,SIMD_WIDE_DETAIL::wide_mul>)
          return noexcept(std::declval<wide<T,N> &>() = SIMD_WIDE_DETAIL::wide_dispatch(
            std::declval<wide<T,N> const &>(),std::declval<B const &>(),SIMD_WIDE_DETAIL::wide_mul{}));
        else return false;
      }()) {
    if constexpr (SIMD_WIDE_DETAIL::wide_batch_assignable<wide<T,N>,B,SIMD_WIDE_DETAIL::wide_mul>)
      return a = SIMD_WIDE_DETAIL::wide_dispatch(a,b,SIMD_WIDE_DETAIL::wide_mul{});
    else return SIMD_WIDE_DETAIL::wide_mutate(a, b, [](auto & x, auto const & y) { x *= y; }, std::make_index_sequence<N>{});
  }

  /// \ingroup wide_values
  /// Update the left pack with elementwise `/=`; a scalar right side is broadcast.
  template<class T, std::size_t N, class B> requires (::simd::detail::wide_target<T,B> == SIMD_WIDE_INDEX) && SIMD_WIDE_DETAIL::wide_arguments<wide<T, N>, B> &&
    (SIMD_WIDE_DETAIL::wide_traits<B>::value || std::copy_constructible<B>) &&
    requires(T & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a /= b; }
  simd_inline constexpr wide<T, N> & operator/=(wide<T, N> & a, B const & b) {
    return SIMD_WIDE_DETAIL::wide_mutate(a, b, [](auto & x, auto const & y) { x /= y; }, std::make_index_sequence<N>{});
  }

  /// \ingroup wide_values
  /// Update the left pack with elementwise `%=`; a scalar right side is broadcast.
  template<class T, std::size_t N, class B> requires (::simd::detail::wide_target<T,B> == SIMD_WIDE_INDEX) && SIMD_WIDE_DETAIL::wide_arguments<wide<T, N>, B> &&
    (SIMD_WIDE_DETAIL::wide_traits<B>::value || std::copy_constructible<B>) &&
    requires(T & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a %= b; }
  simd_inline constexpr wide<T, N> & operator%=(wide<T, N> & a, B const & b) {
    return SIMD_WIDE_DETAIL::wide_mutate(a, b, [](auto & x, auto const & y) { x %= y; }, std::make_index_sequence<N>{});
  }

  /// \ingroup wide_values
  /// Update the left pack with elementwise `&=`; a scalar right side is broadcast.
  template<class T, std::size_t N, class B> requires (::simd::detail::wide_target<T,B> == SIMD_WIDE_INDEX) && SIMD_WIDE_DETAIL::wide_arguments<wide<T, N>, B> &&
    (SIMD_WIDE_DETAIL::wide_traits<B>::value || std::copy_constructible<B>) &&
    requires(T & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a &= b; }
  simd_inline constexpr wide<T, N> & operator&=(wide<T, N> & a, B const & b) {
    return SIMD_WIDE_DETAIL::wide_mutate(a, b, [](auto & x, auto const & y) { x &= y; }, std::make_index_sequence<N>{});
  }

  /// \ingroup wide_values
  /// Update the left pack with elementwise `|=`; a scalar right side is broadcast.
  template<class T, std::size_t N, class B> requires (::simd::detail::wide_target<T,B> == SIMD_WIDE_INDEX) && SIMD_WIDE_DETAIL::wide_arguments<wide<T, N>, B> &&
    (SIMD_WIDE_DETAIL::wide_traits<B>::value || std::copy_constructible<B>) &&
    requires(T & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a |= b; }
  simd_inline constexpr wide<T, N> & operator|=(wide<T, N> & a, B const & b) {
    return SIMD_WIDE_DETAIL::wide_mutate(a, b, [](auto & x, auto const & y) { x |= y; }, std::make_index_sequence<N>{});
  }

  /// \ingroup wide_values
  /// Update the left pack with elementwise `^=`; a scalar right side is broadcast.
  template<class T, std::size_t N, class B> requires (::simd::detail::wide_target<T,B> == SIMD_WIDE_INDEX) && SIMD_WIDE_DETAIL::wide_arguments<wide<T, N>, B> &&
    (SIMD_WIDE_DETAIL::wide_traits<B>::value || std::copy_constructible<B>) &&
    requires(T & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a ^= b; }
  simd_inline constexpr wide<T, N> & operator^=(wide<T, N> & a, B const & b) {
    return SIMD_WIDE_DETAIL::wide_mutate(a, b, [](auto & x, auto const & y) { x ^= y; }, std::make_index_sequence<N>{});
  }

  /// \ingroup wide_values
  /// Update the left pack with elementwise `<<=`; a scalar right side is broadcast.
  template<class T, std::size_t N, class B> requires (::simd::detail::wide_target<T,B> == SIMD_WIDE_INDEX) && SIMD_WIDE_DETAIL::wide_arguments<wide<T, N>, B> &&
    (SIMD_WIDE_DETAIL::wide_traits<B>::value || std::copy_constructible<B>) &&
    requires(T & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a <<= b; }
  simd_inline constexpr wide<T, N> & operator<<=(wide<T, N> & a, B const & b) {
    return SIMD_WIDE_DETAIL::wide_mutate(a, b, [](auto & x, auto const & y) { x <<= y; }, std::make_index_sequence<N>{});
  }

  /// \ingroup wide_values
  /// Update the left pack with elementwise `>>=`; a scalar right side is broadcast.
  template<class T, std::size_t N, class B> requires (::simd::detail::wide_target<T,B> == SIMD_WIDE_INDEX) && SIMD_WIDE_DETAIL::wide_arguments<wide<T, N>, B> &&
    (SIMD_WIDE_DETAIL::wide_traits<B>::value || std::copy_constructible<B>) &&
    requires(T & a, SIMD_WIDE_DETAIL::wide_element_t<B> const & b) { a >>= b; }
  simd_inline constexpr wide<T, N> & operator>>=(wide<T, N> & a, B const & b) {
    return SIMD_WIDE_DETAIL::wide_mutate(a, b, [](auto & x, auto const & y) { x >>= y; }, std::make_index_sequence<N>{});
  }

  namespace SIMD_WIDE_DETAIL {
    using namespace ::simd::detail;
    using std::abs;
    using std::sqrt;
    using std::fma;
    using std::sin;
    using std::cos;
    using std::exp;
    using std::expm1;
    using std::log;
    using std::log1p;
    using std::tanh;
    using std::atan2;
    template<class... A> simd_inline constexpr auto adl_abs(A const &... a)
        noexcept(noexcept(abs(a...))) -> decltype(abs(a...)) { return abs(a...); }
    template<class... A> simd_inline constexpr auto adl_sqrt(A const &... a)
        noexcept(noexcept(sqrt(a...))) -> decltype(sqrt(a...)) { return sqrt(a...); }
    template<class... A> simd_inline constexpr auto adl_fma(A const &... a)
        noexcept(noexcept(fma(a...))) -> decltype(fma(a...)) { return fma(a...); }
    template<class... A> simd_inline constexpr auto adl_sin(A const &... a)
        noexcept(noexcept(sin(a...))) -> decltype(sin(a...)) { return sin(a...); }
    template<class... A> simd_inline constexpr auto adl_cos(A const &... a)
        noexcept(noexcept(cos(a...))) -> decltype(cos(a...)) { return cos(a...); }
    template<class... A> simd_inline constexpr auto adl_expm1(A const &... a)
        noexcept(noexcept(expm1(a...))) -> decltype(expm1(a...)) { return expm1(a...); }
    template<class... A> simd_inline constexpr auto adl_log(A const &... a)
        noexcept(noexcept(log(a...))) -> decltype(log(a...)) { return log(a...); }
    template<class... A> simd_inline constexpr auto adl_log1p(A const &... a)
        noexcept(noexcept(log1p(a...))) -> decltype(log1p(a...)) { return log1p(a...); }
    template<class... A> simd_inline constexpr auto adl_atan2(A const &... a)
        noexcept(noexcept(atan2(a...))) -> decltype(atan2(a...)) { return atan2(a...); }
    template<class... A> simd_inline constexpr auto adl_tanh(A const &... a)
        noexcept(noexcept(tanh(a...))) -> decltype(tanh(a...)) { return tanh(a...); }
    template<bool Flush,class A> requires requires(A const & a) { exp(a,std::bool_constant<Flush>{}); }
    simd_inline constexpr auto adl_exp(A const & a) noexcept(noexcept(exp(a,std::bool_constant<Flush>{}))) -> decltype(exp(a,std::bool_constant<Flush>{})) { return exp(a,std::bool_constant<Flush>{}); }
    template<bool Flush,class A> requires (!Flush) && (!requires(A const & a) { exp(a,std::bool_constant<Flush>{}); })
    simd_inline constexpr auto adl_exp(A const & a) noexcept(noexcept(exp(a))) -> decltype(exp(a)) { return exp(a); }
    template<class A> simd_inline constexpr auto adl_sincos(A const & a)
        noexcept(noexcept(sincos(a))) -> decltype(sincos(a)) { return sincos(a); }
    template<class M,class T> simd_inline constexpr auto adl_select(M const & m,T const & a,T const & b)
        noexcept(noexcept(select(m,a,b))) -> decltype(select(m,a,b)) { return select(m,a,b); }
  }
  namespace SIMD_WIDE_DETAIL {
    using namespace ::simd::detail;
    // Restrict standard fallbacks: some STLs leave their templates unconstrained.
    template<class T> requires std::is_arithmetic_v<T>
    simd_inline constexpr auto isfinite(T const & x)
        noexcept(noexcept(std::isfinite(x))) -> decltype(std::isfinite(x)) { return std::isfinite(x); }
    template<class T> requires std::is_arithmetic_v<T>
    simd_inline constexpr auto isinf(T const & x)
        noexcept(noexcept(std::isinf(x))) -> decltype(std::isinf(x)) { return std::isinf(x); }
    template<class T> requires std::is_arithmetic_v<T>
    simd_inline constexpr auto isnan(T const & x)
        noexcept(noexcept(std::isnan(x))) -> decltype(std::isnan(x)) { return std::isnan(x); }
    template<class T> requires std::is_arithmetic_v<T>
    simd_inline constexpr auto signbit(T const & x)
        noexcept(noexcept(std::signbit(x))) -> decltype(std::signbit(x)) { return std::signbit(x); }
    template<class T> requires std::is_arithmetic_v<T>
    simd_inline constexpr auto copysign(T const & x,T const & y)
        noexcept(noexcept(std::copysign(x,y))) -> decltype(std::copysign(x,y)) { return std::copysign(x,y); }
    template<class T> requires std::is_floating_point_v<T>
    simd_inline constexpr auto floor(T const & x)
        noexcept(noexcept(std::floor(x))) -> decltype(std::floor(x)) { return std::floor(x); }
    struct wide_floor {
      template<class T> simd_inline constexpr auto operator()(T const & x) const
          noexcept(noexcept(floor(x))) -> decltype(floor(x)) { return floor(x); }
    };
    template<class T> requires std::is_floating_point_v<T>
    simd_inline constexpr auto ceil(T const & x)
        noexcept(noexcept(std::ceil(x))) -> decltype(std::ceil(x)) { return std::ceil(x); }
    struct wide_ceil {
      template<class T> simd_inline constexpr auto operator()(T const & x) const
          noexcept(noexcept(ceil(x))) -> decltype(ceil(x)) { return ceil(x); }
    };
    template<class T> requires std::is_floating_point_v<T>
    simd_inline constexpr auto trunc(T const & x)
        noexcept(noexcept(std::trunc(x))) -> decltype(std::trunc(x)) { return std::trunc(x); }
    struct wide_trunc {
      template<class T> simd_inline constexpr auto operator()(T const & x) const
          noexcept(noexcept(trunc(x))) -> decltype(trunc(x)) { return trunc(x); }
    };
    struct wide_isfinite {
      template<class... T> simd_inline constexpr auto operator()(T const &... x) const
          noexcept(noexcept(isfinite(x...))) -> decltype(isfinite(x...)) { return isfinite(x...); }
    };
    struct wide_isinf {
      template<class... T> simd_inline constexpr auto operator()(T const &... x) const
          noexcept(noexcept(isinf(x...))) -> decltype(isinf(x...)) { return isinf(x...); }
    };
    struct wide_isnan {
      template<class... T> simd_inline constexpr auto operator()(T const &... x) const
          noexcept(noexcept(isnan(x...))) -> decltype(isnan(x...)) { return isnan(x...); }
    };
    struct wide_signbit {
      template<class... T> simd_inline constexpr auto operator()(T const &... x) const
          noexcept(noexcept(signbit(x...))) -> decltype(signbit(x...)) { return signbit(x...); }
    };
    struct wide_copysign {
      template<class... T> simd_inline constexpr auto operator()(T const &... x) const
          noexcept(noexcept(copysign(x...))) -> decltype(copysign(x...)) { return copysign(x...); }
    };
    template<class F,class... T>
    using wide_result = std::remove_cvref_t<std::invoke_result_t<F,T const &...>>;
    template<std::size_t N,class F,class... T> concept wide_value_operation =
      std::invocable<F,T const &...> && std::is_object_v<wide_result<F,T...>> &&
      std::is_move_constructible_v<std::array<wide_result<F,T...>,N>> &&
      (N == 0 || requires(T const &... x) { std::array<wide_result<F,T...>,1>{F{}(x...)}; });

    // Each lane is constructed directly from its ADL result; no default
    // construction or assignment is required. The array then moves into wide.
    template<class F,class T,std::size_t N> requires wide_value_operation<N,F,T>
    simd_inline constexpr auto wide_map(wide<T,N> const & input)
        noexcept([] {
          if constexpr (N == 0) return true;
          else return noexcept(wide<wide_result<F,T>,1>{
            std::array<wide_result<F,T>,1>{F{}(std::declval<T const &>())}});
        }()) {
      auto const & [...x] = input;
      return wide<wide_result<F,T>,N>{std::array<wide_result<F,T>,N>{F{}(x)...}};
    }
    template<class F,class T,std::size_t N> requires wide_value_operation<N,F,T,T>
    simd_inline constexpr auto wide_map(wide<T,N> const & a,wide<T,N> const & b)
        noexcept([] {
          if constexpr (N == 0) return true;
          else return noexcept(wide<wide_result<F,T,T>,1>{
            std::array<wide_result<F,T,T>,1>{F{}(std::declval<T const &>(),std::declval<T const &>())}});
        }()) {
      auto const & [...x] = a;
      auto const & [...y] = b;
      return wide<wide_result<F,T,T>,N>{std::array<wide_result<F,T,T>,N>{F{}(x,y)...}};
    }
  }
  /// \ingroup wide_values
  /// Classify each element as finite, preserving its actual bool or mask result type.
  /// The result construction and ADL call both contribute to conditional `noexcept`.
  template<class T,std::size_t N> requires (::simd::detail::wide_target<T> == SIMD_WIDE_INDEX) && SIMD_WIDE_DETAIL::wide_value_operation<N,SIMD_WIDE_DETAIL::wide_isfinite,T>
  simd_nodiscard simd_inline constexpr auto isfinite(wide<T,N> const & input)
      noexcept(noexcept(SIMD_WIDE_DETAIL::wide_map<SIMD_WIDE_DETAIL::wide_isfinite>(input))) {
    return SIMD_WIDE_DETAIL::wide_map<SIMD_WIDE_DETAIL::wide_isfinite>(input);
  }
  /// \ingroup wide_values
  /// Classify infinities using the element operation and its actual result type.
  template<class T,std::size_t N> requires (::simd::detail::wide_target<T> == SIMD_WIDE_INDEX) && SIMD_WIDE_DETAIL::wide_value_operation<N,SIMD_WIDE_DETAIL::wide_isinf,T>
  simd_nodiscard simd_inline constexpr auto isinf(wide<T,N> const & input)
      noexcept(noexcept(SIMD_WIDE_DETAIL::wide_map<SIMD_WIDE_DETAIL::wide_isinf>(input))) {
    return SIMD_WIDE_DETAIL::wide_map<SIMD_WIDE_DETAIL::wide_isinf>(input);
  }
  /// \ingroup wide_values
  /// Classify NaNs using the element operation and its actual result type.
  template<class T,std::size_t N> requires (::simd::detail::wide_target<T> == SIMD_WIDE_INDEX) && SIMD_WIDE_DETAIL::wide_value_operation<N,SIMD_WIDE_DETAIL::wide_isnan,T>
  simd_nodiscard simd_inline constexpr auto isnan(wide<T,N> const & input)
      noexcept(noexcept(SIMD_WIDE_DETAIL::wide_map<SIMD_WIDE_DETAIL::wide_isnan>(input))) {
    return SIMD_WIDE_DETAIL::wide_map<SIMD_WIDE_DETAIL::wide_isnan>(input);
  }
  /// \ingroup wide_values
  /// Read each element's sign predicate, retaining its actual mask or bool type.
  template<class T,std::size_t N> requires (::simd::detail::wide_target<T> == SIMD_WIDE_INDEX) && SIMD_WIDE_DETAIL::wide_value_operation<N,SIMD_WIDE_DETAIL::wide_signbit,T>
  simd_nodiscard simd_inline constexpr auto signbit(wide<T,N> const & input)
      noexcept(noexcept(SIMD_WIDE_DETAIL::wide_map<SIMD_WIDE_DETAIL::wide_signbit>(input))) {
    return SIMD_WIDE_DETAIL::wide_map<SIMD_WIDE_DETAIL::wide_signbit>(input);
  }
  /// \ingroup wide_values
  /// Copy signs from `b` to `a` using the homogeneous element operation.
  /// The result construction and ADL call both contribute to conditional `noexcept`.
  template<class T,std::size_t N> requires (::simd::detail::wide_target<T> == SIMD_WIDE_INDEX) && SIMD_WIDE_DETAIL::wide_value_operation<N,SIMD_WIDE_DETAIL::wide_copysign,T,T> &&
    std::same_as<SIMD_WIDE_DETAIL::wide_result<SIMD_WIDE_DETAIL::wide_copysign,T,T>,T>
  simd_nodiscard simd_inline constexpr wide<T,N> copysign(wide<T,N> const & a,wide<T,N> const & b)
      noexcept(noexcept(SIMD_WIDE_DETAIL::wide_map<SIMD_WIDE_DETAIL::wide_copysign>(a,b))) {
    return SIMD_WIDE_DETAIL::wide_map<SIMD_WIDE_DETAIL::wide_copysign>(a,b);
  }
  /// \ingroup wide_values
  /// Round each element down, using its ADL operation and retaining the element type.
  /// The result construction and ADL call both contribute to conditional `noexcept`.
  template<class T,std::size_t N> requires (::simd::detail::wide_target<T> == SIMD_WIDE_INDEX) && SIMD_WIDE_DETAIL::wide_value_operation<N,SIMD_WIDE_DETAIL::wide_floor,T> &&
    std::same_as<SIMD_WIDE_DETAIL::wide_result<SIMD_WIDE_DETAIL::wide_floor,T>,T>
  simd_nodiscard simd_inline constexpr wide<T,N> floor(wide<T,N> const & input)
      noexcept(noexcept(SIMD_WIDE_DETAIL::wide_map<SIMD_WIDE_DETAIL::wide_floor>(input))) {
    return SIMD_WIDE_DETAIL::wide_map<SIMD_WIDE_DETAIL::wide_floor>(input);
  }
  /// \ingroup wide_values
  /// Round each element up, using its ADL operation and retaining the element type.
  template<class T,std::size_t N> requires (::simd::detail::wide_target<T> == SIMD_WIDE_INDEX) && SIMD_WIDE_DETAIL::wide_value_operation<N,SIMD_WIDE_DETAIL::wide_ceil,T> &&
    std::same_as<SIMD_WIDE_DETAIL::wide_result<SIMD_WIDE_DETAIL::wide_ceil,T>,T>
  simd_nodiscard simd_inline constexpr wide<T,N> ceil(wide<T,N> const & input)
      noexcept(noexcept(SIMD_WIDE_DETAIL::wide_map<SIMD_WIDE_DETAIL::wide_ceil>(input))) {
    return SIMD_WIDE_DETAIL::wide_map<SIMD_WIDE_DETAIL::wide_ceil>(input);
  }
  /// \ingroup wide_values
  /// Round each element toward zero, using its ADL operation and retaining the element type.
  template<class T,std::size_t N> requires (::simd::detail::wide_target<T> == SIMD_WIDE_INDEX) && SIMD_WIDE_DETAIL::wide_value_operation<N,SIMD_WIDE_DETAIL::wide_trunc,T> &&
    std::same_as<SIMD_WIDE_DETAIL::wide_result<SIMD_WIDE_DETAIL::wide_trunc,T>,T>
  simd_nodiscard simd_inline constexpr wide<T,N> trunc(wide<T,N> const & input)
      noexcept(noexcept(SIMD_WIDE_DETAIL::wide_map<SIMD_WIDE_DETAIL::wide_trunc>(input))) {
    return SIMD_WIDE_DETAIL::wide_map<SIMD_WIDE_DETAIL::wide_trunc>(input);
  }
  /// \ingroup wide_values
  /// Copy one value to a pack with the explicitly selected extent.
  template<class R,std::size_t N> requires (::simd::detail::wide_target<R> == SIMD_WIDE_INDEX)
  simd_nodiscard simd_inline constexpr wide<R,N> broadcast(R value) noexcept(noexcept(wide<R,N>::broadcast(value))) {
    return wide<R,N>::broadcast(value);
  }
  /// \ingroup wide_values
  /// Apply the element library's absolute-value operation.
  template<class R,std::size_t N> requires (::simd::detail::wide_target<R> == SIMD_WIDE_INDEX) && requires(R const & x) { SIMD_WIDE_DETAIL::adl_abs(x); }
  simd_nodiscard simd_inline constexpr wide<R,N> abs(wide<R,N> const & input)
      noexcept([] {
        if constexpr (requires(std::array<R,N> const & a) { SIMD_WIDE_DETAIL::adl_abs(a); })
          return noexcept(wide<R,N>{SIMD_WIDE_DETAIL::adl_abs(std::declval<std::array<R,N> const &>())});
        else return noexcept(wide<R,N>()) && std::is_nothrow_move_constructible_v<wide<R,N>> &&
          (N == 0 || noexcept(std::declval<R &>() = SIMD_WIDE_DETAIL::adl_abs(std::declval<R const &>())));
      }()) {
    if constexpr (requires { SIMD_WIDE_DETAIL::adl_abs(input.registers); })
      return wide<R,N>{SIMD_WIDE_DETAIL::adl_abs(input.registers)};
    else {
      wide<R,N> result;
      auto & [...value] = result;
      auto const & [...x] = input;
      ([&] { value = SIMD_WIDE_DETAIL::adl_abs(x); }(), ...);
      return result;
    }
  }
  /// \ingroup wide_values
  /// Apply the element library's square-root operation.
  /// Uses one array call when available, otherwise an elementwise fallback.
  template<class R,std::size_t N> requires (::simd::detail::wide_target<R> == SIMD_WIDE_INDEX) && requires(R const & x) { SIMD_WIDE_DETAIL::adl_sqrt(x); }
  simd_nodiscard simd_inline constexpr wide<R,N> sqrt(wide<R,N> const & input)
      noexcept([] {
        if constexpr (requires(std::array<R,N> const & a) { SIMD_WIDE_DETAIL::adl_sqrt(a); })
          return noexcept(wide<R,N>{SIMD_WIDE_DETAIL::adl_sqrt(std::declval<std::array<R,N> const &>())});
        else return noexcept(wide<R,N>()) && std::is_nothrow_move_constructible_v<wide<R,N>> &&
          (N == 0 || noexcept(std::declval<R &>() = SIMD_WIDE_DETAIL::adl_sqrt(std::declval<R const &>())));
      }()) {
    if constexpr (requires { SIMD_WIDE_DETAIL::adl_sqrt(input.registers); })
      return wide<R,N>{SIMD_WIDE_DETAIL::adl_sqrt(input.registers)};
    else {
      wide<R,N> result;
      auto & [...value] = result;
      auto const & [...x] = input;
      ([&] { value = SIMD_WIDE_DETAIL::adl_sqrt(x); }(), ...);
      return result;
    }
  }
  /// \ingroup wide_values
  /// Apply the element library's sine operation in radians.
  /// Uses one array call when available, otherwise an elementwise fallback.
  template<class R,std::size_t N> requires (::simd::detail::wide_target<R> == SIMD_WIDE_INDEX) && requires(R const & x) { SIMD_WIDE_DETAIL::adl_sin(x); }
  simd_nodiscard simd_inline constexpr wide<R,N> sin(wide<R,N> const & input)
      noexcept([] {
        if constexpr (requires(std::array<R,N> const & a) { SIMD_WIDE_DETAIL::adl_sin(a); })
          return noexcept(wide<R,N>{SIMD_WIDE_DETAIL::adl_sin(std::declval<std::array<R,N> const &>())});
        else return noexcept(wide<R,N>()) && std::is_nothrow_move_constructible_v<wide<R,N>> &&
          (N == 0 || noexcept(std::declval<R &>() = SIMD_WIDE_DETAIL::adl_sin(std::declval<R const &>())));
      }()) {
    if constexpr (requires { SIMD_WIDE_DETAIL::adl_sin(input.registers); })
      return wide<R,N>{SIMD_WIDE_DETAIL::adl_sin(input.registers)};
    else {
      wide<R,N> result;
      auto & [...value] = result;
      auto const & [...x] = input;
      ([&] { value = SIMD_WIDE_DETAIL::adl_sin(x); }(), ...);
      return result;
    }
  }
  /// \ingroup wide_values
  /// Apply the element library's cosine operation in radians.
  /// Uses one array call when available, otherwise an elementwise fallback.
  template<class R,std::size_t N> requires (::simd::detail::wide_target<R> == SIMD_WIDE_INDEX) && requires(R const & x) { SIMD_WIDE_DETAIL::adl_cos(x); }
  simd_nodiscard simd_inline constexpr wide<R,N> cos(wide<R,N> const & input)
      noexcept([] {
        if constexpr (requires(std::array<R,N> const & a) { SIMD_WIDE_DETAIL::adl_cos(a); })
          return noexcept(wide<R,N>{SIMD_WIDE_DETAIL::adl_cos(std::declval<std::array<R,N> const &>())});
        else return noexcept(wide<R,N>()) && std::is_nothrow_move_constructible_v<wide<R,N>> &&
          (N == 0 || noexcept(std::declval<R &>() = SIMD_WIDE_DETAIL::adl_cos(std::declval<R const &>())));
      }()) {
    if constexpr (requires { SIMD_WIDE_DETAIL::adl_cos(input.registers); })
      return wide<R,N>{SIMD_WIDE_DETAIL::adl_cos(input.registers)};
    else {
      wide<R,N> result;
      auto & [...value] = result;
      auto const & [...x] = input;
      ([&] { value = SIMD_WIDE_DETAIL::adl_cos(x); }(), ...);
      return result;
    }
  }
  /// \ingroup wide_values
  /// Apply exponential; pass the compile-time Flush tag when the element supports it.
  /// Uses one array call when available, otherwise an elementwise fallback.
  template<bool Flush = false,class R,std::size_t N> requires (::simd::detail::wide_target<R> == SIMD_WIDE_INDEX) && requires(R const & x) { SIMD_WIDE_DETAIL::adl_exp<Flush>(x); }
  simd_nodiscard simd_inline constexpr wide<R,N> exp(wide<R,N> const & input)
      noexcept([] {
        if constexpr (requires(std::array<R,N> const & a) { SIMD_WIDE_DETAIL::adl_exp<Flush>(a); })
          return noexcept(wide<R,N>{SIMD_WIDE_DETAIL::adl_exp<Flush>(std::declval<std::array<R,N> const &>())});
        else return noexcept(wide<R,N>()) && std::is_nothrow_move_constructible_v<wide<R,N>> &&
          (N == 0 || noexcept(std::declval<R &>() = SIMD_WIDE_DETAIL::adl_exp<Flush>(std::declval<R const &>())));
      }()) {
    if constexpr (requires { SIMD_WIDE_DETAIL::adl_exp<Flush>(input.registers); })
      return wide<R,N>{SIMD_WIDE_DETAIL::adl_exp<Flush>(input.registers)};
    else {
      wide<R,N> result;
      auto & [...value] = result;
      auto const & [...x] = input;
      ([&] { value = SIMD_WIDE_DETAIL::adl_exp<Flush>(x); }(), ...);
      return result;
    }
  }
  /// \ingroup wide_values
  /// Apply the element library's exp(x)-1 operation.
  /// Uses one array call when available, otherwise an elementwise fallback.
  template<class R,std::size_t N> requires (::simd::detail::wide_target<R> == SIMD_WIDE_INDEX) && requires(R const & x) { SIMD_WIDE_DETAIL::adl_expm1(x); }
  simd_nodiscard simd_inline constexpr wide<R,N> expm1(wide<R,N> const & input)
      noexcept([] {
        if constexpr (requires(std::array<R,N> const & a) { SIMD_WIDE_DETAIL::adl_expm1(a); })
          return noexcept(wide<R,N>{SIMD_WIDE_DETAIL::adl_expm1(std::declval<std::array<R,N> const &>())});
        else return noexcept(wide<R,N>()) && std::is_nothrow_move_constructible_v<wide<R,N>> &&
          (N == 0 || noexcept(std::declval<R &>() = SIMD_WIDE_DETAIL::adl_expm1(std::declval<R const &>())));
      }()) {
    if constexpr (requires { SIMD_WIDE_DETAIL::adl_expm1(input.registers); })
      return wide<R,N>{SIMD_WIDE_DETAIL::adl_expm1(input.registers)};
    else {
      wide<R,N> result;
      auto & [...value] = result;
      auto const & [...x] = input;
      ([&] { value = SIMD_WIDE_DETAIL::adl_expm1(x); }(), ...);
      return result;
    }
  }
  /// \ingroup wide_values
  /// Apply the element library's natural logarithm.
  /// Uses one array call when available, otherwise an elementwise fallback.
  template<class R,std::size_t N> requires (::simd::detail::wide_target<R> == SIMD_WIDE_INDEX) && requires(R const & x) { SIMD_WIDE_DETAIL::adl_log(x); }
  simd_nodiscard simd_inline constexpr wide<R,N> log(wide<R,N> const & input)
      noexcept([] {
        if constexpr (requires(std::array<R,N> const & a) { SIMD_WIDE_DETAIL::adl_log(a); })
          return noexcept(wide<R,N>{SIMD_WIDE_DETAIL::adl_log(std::declval<std::array<R,N> const &>())});
        else return noexcept(wide<R,N>()) && std::is_nothrow_move_constructible_v<wide<R,N>> &&
          (N == 0 || noexcept(std::declval<R &>() = SIMD_WIDE_DETAIL::adl_log(std::declval<R const &>())));
      }()) {
    if constexpr (requires { SIMD_WIDE_DETAIL::adl_log(input.registers); })
      return wide<R,N>{SIMD_WIDE_DETAIL::adl_log(input.registers)};
    else {
      wide<R,N> result;
      auto & [...value] = result;
      auto const & [...x] = input;
      ([&] { value = SIMD_WIDE_DETAIL::adl_log(x); }(), ...);
      return result;
    }
  }
  /// \ingroup wide_values
  /// Apply the element library's log(1+x) operation.
  /// Uses one array call when available, otherwise an elementwise fallback.
  template<class R,std::size_t N> requires (::simd::detail::wide_target<R> == SIMD_WIDE_INDEX) && requires(R const & x) { SIMD_WIDE_DETAIL::adl_log1p(x); }
  simd_nodiscard simd_inline constexpr wide<R,N> log1p(wide<R,N> const & input)
      noexcept([] {
        if constexpr (requires(std::array<R,N> const & a) { SIMD_WIDE_DETAIL::adl_log1p(a); })
          return noexcept(wide<R,N>{SIMD_WIDE_DETAIL::adl_log1p(std::declval<std::array<R,N> const &>())});
        else return noexcept(wide<R,N>()) && std::is_nothrow_move_constructible_v<wide<R,N>> &&
          (N == 0 || noexcept(std::declval<R &>() = SIMD_WIDE_DETAIL::adl_log1p(std::declval<R const &>())));
      }()) {
    if constexpr (requires { SIMD_WIDE_DETAIL::adl_log1p(input.registers); })
      return wide<R,N>{SIMD_WIDE_DETAIL::adl_log1p(input.registers)};
    else {
      wide<R,N> result;
      auto & [...value] = result;
      auto const & [...x] = input;
      ([&] { value = SIMD_WIDE_DETAIL::adl_log1p(x); }(), ...);
      return result;
    }
  }
  /// \ingroup wide_values
  /// Apply the element library's hyperbolic tangent.
  /// Uses one array call when available, otherwise an elementwise fallback.
  template<class R,std::size_t N> requires (::simd::detail::wide_target<R> == SIMD_WIDE_INDEX) && requires(R const & x) { SIMD_WIDE_DETAIL::adl_tanh(x); }
  simd_nodiscard simd_inline constexpr wide<R,N> tanh(wide<R,N> const & input)
      noexcept([] {
        if constexpr (requires(std::array<R,N> const & a) { SIMD_WIDE_DETAIL::adl_tanh(a); })
          return noexcept(wide<R,N>{SIMD_WIDE_DETAIL::adl_tanh(std::declval<std::array<R,N> const &>())});
        else return noexcept(wide<R,N>()) && std::is_nothrow_move_constructible_v<wide<R,N>> &&
          (N == 0 || noexcept(std::declval<R &>() = SIMD_WIDE_DETAIL::adl_tanh(std::declval<R const &>())));
      }()) {
    if constexpr (requires { SIMD_WIDE_DETAIL::adl_tanh(input.registers); })
      return wide<R,N>{SIMD_WIDE_DETAIL::adl_tanh(input.registers)};
    else {
      wide<R,N> result;
      auto & [...value] = result;
      auto const & [...x] = input;
      ([&] { value = SIMD_WIDE_DETAIL::adl_tanh(x); }(), ...);
      return result;
    }
  }
  /// \ingroup wide_values
  /// Apply the element library's atan2(y,x), preserving argument order.
  /// Uses one array call when available, otherwise an elementwise fallback.
  template<class R,std::size_t N> requires (::simd::detail::wide_target<R> == SIMD_WIDE_INDEX) && requires(R const & x) { SIMD_WIDE_DETAIL::adl_atan2(x,x); }
  simd_nodiscard simd_inline constexpr wide<R,N> atan2(wide<R,N> const & y,wide<R,N> const & x)
      noexcept([] {
        if constexpr (requires(std::array<R,N> const & a) { SIMD_WIDE_DETAIL::adl_atan2(a,a); })
          return noexcept(wide<R,N>{SIMD_WIDE_DETAIL::adl_atan2(std::declval<std::array<R,N> const &>(),
            std::declval<std::array<R,N> const &>())});
        else return noexcept(wide<R,N>()) && std::is_nothrow_move_constructible_v<wide<R,N>> &&
          (N == 0 || noexcept(std::declval<R &>() = SIMD_WIDE_DETAIL::adl_atan2(
            std::declval<R const &>(),std::declval<R const &>())));
      }()) {
    if constexpr (requires { SIMD_WIDE_DETAIL::adl_atan2(y.registers,x.registers); })
      return wide<R,N>{SIMD_WIDE_DETAIL::adl_atan2(y.registers,x.registers)};
    else {
      wide<R,N> result;
      auto & [...value] = result;
      auto const & [...a] = y;
      auto const & [...b] = x;
      ([&] { value = SIMD_WIDE_DETAIL::adl_atan2(a,b); }(), ...);
      return result;
    }
  }
  /// \ingroup wide_values
  /// Apply the element library's fused multiply-add to corresponding elements.
  /// Uses one array call when available, otherwise an elementwise fallback.
  template<class R,std::size_t N> requires (::simd::detail::wide_target<R> == SIMD_WIDE_INDEX) && requires(R const & x) { SIMD_WIDE_DETAIL::adl_fma(x,x,x); }
  simd_nodiscard simd_inline constexpr wide<R,N> fma(wide<R,N> const & a,wide<R,N> const & b,wide<R,N> const & c)
      noexcept([] {
        if constexpr (requires(std::array<R,N> const & x) { SIMD_WIDE_DETAIL::adl_fma(x,x,x); })
          return noexcept(wide<R,N>{SIMD_WIDE_DETAIL::adl_fma(std::declval<std::array<R,N> const &>(),
            std::declval<std::array<R,N> const &>(),std::declval<std::array<R,N> const &>())});
        else return noexcept(wide<R,N>()) && std::is_nothrow_move_constructible_v<wide<R,N>> &&
          (N == 0 || noexcept(std::declval<R &>() = SIMD_WIDE_DETAIL::adl_fma(
            std::declval<R const &>(),std::declval<R const &>(),std::declval<R const &>())));
      }()) {
    if constexpr (requires { SIMD_WIDE_DETAIL::adl_fma(a.registers,b.registers,c.registers); })
      return wide<R,N>{SIMD_WIDE_DETAIL::adl_fma(a.registers,b.registers,c.registers)};
    else {
      wide<R,N> result;
      auto & [...value] = result;
      auto const & [...x] = a;
      auto const & [...y] = b;
      auto const & [...z] = c;
      ([&] { value = SIMD_WIDE_DETAIL::adl_fma(x,y,z); }(), ...);
      return result;
    }
  }
  /// \ingroup wide_values
  /// Return paired sine and cosine packs using the element's paired operation.
  /// Uses one array call when available, otherwise an elementwise fallback.
  template<class R,std::size_t N> requires (::simd::detail::wide_target<R> == SIMD_WIDE_INDEX) && requires(R const & x) { SIMD_WIDE_DETAIL::adl_sincos(x); }
  simd_nodiscard simd_inline constexpr auto sincos(wide<R,N> const & input)
      noexcept([] {
        if constexpr (requires(std::array<R,N> const & x) { SIMD_WIDE_DETAIL::adl_sincos(x); })
          return noexcept(SIMD_WIDE_DETAIL::adl_sincos(std::declval<std::array<R,N> const &>())) &&
            std::is_nothrow_move_constructible_v<std::array<R,N>>;
        else return noexcept(wide<R,N>()) && std::is_nothrow_move_constructible_v<wide<R,N>> &&
          (N == 0 || (noexcept(SIMD_WIDE_DETAIL::adl_sincos(std::declval<R const &>())) && noexcept(std::declval<R &>() = std::declval<decltype(SIMD_WIDE_DETAIL::adl_sincos(std::declval<R const &>())) &>().first) &&
            noexcept(std::declval<R &>() = std::declval<decltype(SIMD_WIDE_DETAIL::adl_sincos(std::declval<R const &>())) &>().second)));
      }()) {
    if constexpr (requires { SIMD_WIDE_DETAIL::adl_sincos(input.registers); }) {
      auto [sine,cosine] = SIMD_WIDE_DETAIL::adl_sincos(input.registers);
      return std::pair{wide<R,N>{std::move(sine)},wide<R,N>{std::move(cosine)}};
    } else {
      wide<R,N> sine,cosine;
      auto & [...s] = sine;
      auto & [...c] = cosine;
      auto const & [...x] = input;
      ([&] { auto pair = SIMD_WIDE_DETAIL::adl_sincos(x); s = pair.first; c = pair.second; }(), ...);
      return std::pair{std::move(sine),std::move(cosine)};
    }
  }
  /// \ingroup wide_values
  /// Choose from `a` or `b` according to the corresponding element mask.
  template <class R, std::size_t N> requires (::simd::detail::wide_target<R> == SIMD_WIDE_INDEX)
  simd_nodiscard simd_inline wide<R, N> select(wide<decltype(R{} == R{}), N> const & masks,
      wide<R, N> const & a, wide<R, N> const & b)
      noexcept([] {
        if constexpr (N == 0)
          return noexcept(wide<R,N>()) && std::is_nothrow_move_constructible_v<wide<R,N>>;
        else if constexpr (std::is_same_v<decltype(R{} == R{}), bool>)
          return noexcept(wide<R,N>()) && std::is_nothrow_move_constructible_v<wide<R,N>> &&
            noexcept(std::declval<R &>() = std::declval<R const &>());
        else
          return noexcept(wide<R,N>()) && std::is_nothrow_move_constructible_v<wide<R,N>> &&
            noexcept(std::declval<R &>() = SIMD_WIDE_DETAIL::adl_select(
              std::declval<decltype(R{} == R{}) const &>(),
              std::declval<R const &>(), std::declval<R const &>()));
      }()) {
    wide<R, N> result;
    auto & [...value] = result;
    auto const & [...mask] = masks;
    auto const & [...x] = a;
    auto const & [...y] = b;
    if constexpr (std::is_same_v<decltype(R{} == R{}), bool>)
      ([&] { value = mask ? x : y; }(), ...);
    else
      ([&] { value = SIMD_WIDE_DETAIL::adl_select(mask, x, y); }(), ...);
    return result;
  }
