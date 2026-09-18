// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Intentionally repeatable under one target family.
  public:
    /// Take an array of exactly the pack extent.
    SIMD_WIDE_TARGET simd_inline constexpr wide(std::array<T, N> values)
      noexcept(std::is_nothrow_move_constructible_v<std::array<T, N>>)
      requires detail::wide_family_is<SIMD_WIDE_FAMILY,T>
      : registers(std::move(values)) {}
    /// Construct one element from each argument, preserving argument order.
    template<class... U> requires detail::wide_family_is<SIMD_WIDE_FAMILY,T,U...> && (sizeof...(U) == N && N != 0) &&
      (std::constructible_from<T,U &&> && ...) &&
      (N != 1 || ((!detail::wide_traits<std::remove_cvref_t<U>>::value &&
        !std::same_as<std::remove_cvref_t<U>,std::array<T,N>>) && ...))
    explicit(N == 1) SIMD_WIDE_TARGET simd_inline constexpr wide(U &&... values)
      noexcept((std::is_nothrow_constructible_v<T,U &&> && ...))
      : registers{T(std::forward<U>(values))...} {}
    /// Convert one value to `T`, then copy it across the pack.
    template<class U> requires detail::wide_family_is<SIMD_WIDE_FAMILY,T,U> && (N != 1) && (!detail::wide_traits<std::remove_cvref_t<U>>::value) &&
      (!std::same_as<std::remove_cvref_t<U>,std::array<T, N>>) &&
      std::constructible_from<T,U const &> && (N == 0 || std::copy_constructible<T>)
    explicit SIMD_WIDE_TARGET simd_inline constexpr wide(U const & value)
      noexcept(std::is_nothrow_constructible_v<T,U const &> &&
        (N == 0 || std::is_nothrow_copy_constructible_v<T>))
      : wide(T(value), std::make_index_sequence<N>{}) {}

    /// Convert each element of a same-size pack; explicitness follows the element conversion.
    template<class U> requires detail::wide_family_is<SIMD_WIDE_FAMILY,T,U> && (!std::same_as<T,U>) && std::constructible_from<T,U const &>
    explicit(!std::is_convertible_v<U const &,T>) SIMD_WIDE_TARGET simd_inline constexpr wide(wide<U,N> const & source)
      noexcept(N == 0 || std::is_nothrow_constructible_v<T,U const &>)
      : wide(conversion_tag{}, source, std::make_index_sequence<N>{}) {}
    /// Convert each element of a same-size pack; explicitness follows the element conversion.
    template<class U> requires detail::wide_family_is<SIMD_WIDE_FAMILY,T,U> && (!std::same_as<T,U>) && std::constructible_from<T,U &&>
    explicit(!std::is_convertible_v<U &&,T>) SIMD_WIDE_TARGET simd_inline constexpr wide(wide<U,N> && source)
      noexcept(N == 0 || std::is_nothrow_constructible_v<T,U &&>)
      : wide(conversion_tag{}, std::move(source), std::make_index_sequence<N>{}) {}

    /// Copy `value` to every element; an empty pack makes no copies.
    simd_nodiscard static SIMD_WIDE_TARGET simd_inline constexpr wide broadcast(T const & value)
      noexcept(N == 0 || std::is_nothrow_copy_constructible_v<T>)
      requires detail::wide_family_is<SIMD_WIDE_FAMILY,T> && (N == 0 || std::copy_constructible<T>) {
      return wide(value, std::make_index_sequence<N>{});
    }

    /// Invoke `f` with all elements as arguments; the empty pack calls `f()`.
    template<class F> requires detail::wide_family_is<SIMD_WIDE_FAMILY,T>
    SIMD_WIDE_TARGET simd_inline constexpr decltype(auto) apply(F && f) noexcept(noexcept(std::apply(std::forward<F>(f), registers))) {
      return apply_indices(std::forward<F>(f), registers, std::make_index_sequence<N>{});
    }

    /// Invoke `f` with all elements as arguments; the empty pack calls `f()`.
    template<class F> requires detail::wide_family_is<SIMD_WIDE_FAMILY,T>
    SIMD_WIDE_TARGET simd_inline constexpr decltype(auto) apply(F && f) const noexcept(noexcept(std::apply(std::forward<F>(f), registers))) {
      return apply_indices(std::forward<F>(f), registers, std::make_index_sequence<N>{});
    }
  private:
    template<class W, std::size_t... I> requires detail::wide_family_is<SIMD_WIDE_FAMILY,T,W>
    SIMD_WIDE_TARGET simd_inline constexpr wide(conversion_tag, W && source, std::index_sequence<I...>)
      noexcept((std::is_nothrow_constructible_v<T,
        decltype(std::get<I>(std::forward<W>(source).registers))> && ...))
      : registers{T(std::get<I>(std::forward<W>(source).registers))...} {}
    template<class U, std::size_t... I> requires detail::wide_family_is<SIMD_WIDE_FAMILY,T> && std::same_as<std::remove_cvref_t<U>,T>
    SIMD_WIDE_TARGET simd_inline constexpr wide(U && value, std::index_sequence<I...>)
      noexcept(N == 0 || std::is_nothrow_copy_constructible_v<T>)
      : registers{((void)I,static_cast<T const &>(value))...} {}
    // Keep the callable on the force-inline path. MSVC can outline std::invoke
    // inside std::apply even when both the caller and functor are force-inline.
    template<class F, class A, std::size_t ... I> requires detail::wide_family_is<SIMD_WIDE_FAMILY,A>
    static SIMD_WIDE_TARGET simd_inline constexpr decltype(auto) apply_indices(F && f, A & a, std::index_sequence<I...>) {
      if constexpr (std::is_member_pointer_v<std::remove_reference_t<F>>)
        return std::invoke(std::forward<F>(f), std::get<I>(a)...);
      else
        return std::forward<F>(f)(std::get<I>(a)...);
    }

