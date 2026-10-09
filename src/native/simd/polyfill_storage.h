// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
namespace native::detail {
  template<class T,std::size_t N,isa<> A,class Self> struct polyfill_storage_operations {};
  /// Semantic operations for a native representation whose instructions are
  /// absent. Transfers preserve that representation; arithmetic uses explicitly
  /// permitted scalar lanes and retains the public architecture and mask domain.
  template<class T,std::size_t N,isa<> A,class Self>
    requires(A.has(polyfill) && polyfill_element_traits<T>::supported)
  struct polyfill_storage_operations<T,N,A,Self> {
    using emulated_type=simd<T,N,isa<>(polyfill)>;
    using word_type=std::conditional_t<sizeof(T)==1,std::uint8_t,
      std::conditional_t<sizeof(T)==2,std::uint16_t,
        std::conditional_t<sizeof(T)==4,std::uint32_t,std::uint64_t>>>;
    using vector_mask_type=simd<mask_lane<word_type>,N,A>;
    static constexpr emulated_type project(Self value) noexcept {
      std::array<T,N> lanes{}; value.store(lanes.data());
      return emulated_type::load(lanes.data());
    }
    static constexpr Self restore(emulated_type value) noexcept {
      std::array<T,N> lanes{}; value.store(lanes.data());
      return Self::load(lanes.data());
    }
#define NATIVE_POLYFILL_STORAGE_BINARY(OP) \
    /** Apply the permitted lane operation, retaining native storage. */ \
    friend constexpr Self operator OP(Self a,Self b) noexcept \
      requires requires(emulated_type x) { { x OP x } -> std::same_as<emulated_type>; } { \
      return restore(project(a) OP project(b)); \
    } \
    /** Apply the operation in place. */ \
    constexpr Self & operator OP##=(Self b) noexcept \
      requires requires(emulated_type x) { { x OP x } -> std::same_as<emulated_type>; } { \
      auto & self=static_cast<Self &>(*this); return self=self OP b; \
    }
    NATIVE_POLYFILL_STORAGE_BINARY(+)
    NATIVE_POLYFILL_STORAGE_BINARY(-)
    NATIVE_POLYFILL_STORAGE_BINARY(*)
    NATIVE_POLYFILL_STORAGE_BINARY(/)
    NATIVE_POLYFILL_STORAGE_BINARY(%)
    NATIVE_POLYFILL_STORAGE_BINARY(&)
    NATIVE_POLYFILL_STORAGE_BINARY(|)
    NATIVE_POLYFILL_STORAGE_BINARY(^)
#undef NATIVE_POLYFILL_STORAGE_BINARY
    template<class U> static constexpr bool scalar_operand=
      simd_integer_element<T>?simd_integer_element<U>:
        (polyfill_element_traits<T>::arithmetic && std::convertible_to<U,T>);
#define NATIVE_POLYFILL_STORAGE_SCALAR(OP) \
    /** Broadcast a compatible scalar before the logical lane operation. */ \
    template<class U> requires(scalar_operand<U> && requires(emulated_type x,U y) { x OP emulated_type(y); }) \
    friend constexpr Self operator OP(Self a,U b) noexcept { return restore(project(a) OP emulated_type(b)); } \
    /** Broadcast a compatible scalar before the logical lane operation. */ \
    template<class U> requires(scalar_operand<U> && requires(emulated_type x,U y) { emulated_type(y) OP x; }) \
    friend constexpr Self operator OP(U a,Self b) noexcept { return restore(emulated_type(a) OP project(b)); }
    NATIVE_POLYFILL_STORAGE_SCALAR(+)
    NATIVE_POLYFILL_STORAGE_SCALAR(-)
    NATIVE_POLYFILL_STORAGE_SCALAR(*)
    NATIVE_POLYFILL_STORAGE_SCALAR(/)
    NATIVE_POLYFILL_STORAGE_SCALAR(%)
    NATIVE_POLYFILL_STORAGE_SCALAR(&)
    NATIVE_POLYFILL_STORAGE_SCALAR(|)
    NATIVE_POLYFILL_STORAGE_SCALAR(^)
#undef NATIVE_POLYFILL_STORAGE_SCALAR
#define NATIVE_POLYFILL_STORAGE_COMPARE(OP) \
    /** Compare logical lanes in the native storage mask domain. */ \
    friend constexpr auto operator OP(Self a,Self b) noexcept \
      requires requires(emulated_type x) { (x OP x).to_bitset(); } { \
      return Self::mask_type::from_bitset((project(a) OP project(b)).to_bitset()); \
    }
    NATIVE_POLYFILL_STORAGE_COMPARE(==)
    NATIVE_POLYFILL_STORAGE_COMPARE(!=)
    NATIVE_POLYFILL_STORAGE_COMPARE(<)
    NATIVE_POLYFILL_STORAGE_COMPARE(<=)
    NATIVE_POLYFILL_STORAGE_COMPARE(>)
    NATIVE_POLYFILL_STORAGE_COMPARE(>=)
#undef NATIVE_POLYFILL_STORAGE_COMPARE
#define NATIVE_POLYFILL_STORAGE_SCALAR_COMPARE(OP) \
    /** Compare against a compatible broadcast scalar. */ \
    template<class U> requires(scalar_operand<U> && requires(emulated_type x,U y) { (x OP emulated_type(y)).to_bitset(); }) \
    friend constexpr auto operator OP(Self a,U b) noexcept { return Self::mask_type::from_bitset((project(a) OP emulated_type(b)).to_bitset()); } \
    /** Compare a compatible broadcast scalar against logical lanes. */ \
    template<class U> requires(scalar_operand<U> && requires(emulated_type x,U y) { (emulated_type(y) OP x).to_bitset(); }) \
    friend constexpr auto operator OP(U a,Self b) noexcept { return Self::mask_type::from_bitset((emulated_type(a) OP project(b)).to_bitset()); }
    NATIVE_POLYFILL_STORAGE_SCALAR_COMPARE(==)
    NATIVE_POLYFILL_STORAGE_SCALAR_COMPARE(!=)
    NATIVE_POLYFILL_STORAGE_SCALAR_COMPARE(<)
    NATIVE_POLYFILL_STORAGE_SCALAR_COMPARE(<=)
    NATIVE_POLYFILL_STORAGE_SCALAR_COMPARE(>)
    NATIVE_POLYFILL_STORAGE_SCALAR_COMPARE(>=)
#undef NATIVE_POLYFILL_STORAGE_SCALAR_COMPARE

    /// Negate logical values with the scalar lane contract.
    friend constexpr Self operator-(Self value) noexcept
      requires requires(emulated_type x) { { -x } -> std::same_as<emulated_type>; } { return restore(-project(value)); }
    /// Complement integral representations.
    friend constexpr Self operator~(Self value) noexcept
      requires requires(emulated_type x) { { ~x } -> std::same_as<emulated_type>; } { return restore(~project(value)); }
    /// Select from a and b in exactly the corresponding mask domain.
    template<class M> requires(std::same_as<M,predicate<N,A>> || std::same_as<M,polyfill_predicate<N,A>> || std::same_as<M,vector_mask_type>)
    friend constexpr Self select(M mask,Self a,Self b) noexcept {
      return restore(select(emulated_type::mask_type::from_bitset(mask.to_bitset()),project(a),project(b)));
    }
#define NATIVE_POLYFILL_STORAGE_UNARY(NAME) \
    /** Apply the scalar operation and restore native storage. */ \
    friend constexpr Self NAME(Self value) noexcept \
      requires requires(emulated_type x) { { NAME(x) } -> std::same_as<emulated_type>; } { return restore(NAME(project(value))); }
    NATIVE_POLYFILL_STORAGE_UNARY(sqrt)
    NATIVE_POLYFILL_STORAGE_UNARY(abs)
    NATIVE_POLYFILL_STORAGE_UNARY(floor)
    NATIVE_POLYFILL_STORAGE_UNARY(ceil)
    NATIVE_POLYFILL_STORAGE_UNARY(trunc)
    NATIVE_POLYFILL_STORAGE_UNARY(round_even)
    NATIVE_POLYFILL_STORAGE_UNARY(popcount)
#undef NATIVE_POLYFILL_STORAGE_UNARY
    /// Compute a*b+c with one rounding in each lane.
    friend constexpr Self fma(Self a,Self b,Self c) noexcept
      requires(polyfill_element_traits<T>::arithmetic) { return restore(fma(project(a),project(b),project(c))); }
    /// Select the smaller lane; the second operand wins on equality or unordered.
    friend constexpr Self min(Self a,Self b) noexcept requires(polyfill_element_traits<T>::arithmetic) { return select(a<b,a,b); }
    /// Select the larger lane; the second operand wins on equality or unordered.
    friend constexpr Self max(Self a,Self b) noexcept requires(polyfill_element_traits<T>::arithmetic) { return select(a>b,a,b); }
    /// Read one logical lane selected at compile time.
    template<std::size_t I> requires(I<N)
    constexpr T get() const noexcept { return project(static_cast<Self const &>(*this)).template get<I>(); }
    /// Replace one logical lane without changing other representations.
    template<std::size_t I> requires(I<N)
    constexpr Self set(T value) const noexcept { return restore(project(static_cast<Self const &>(*this)).template set<I>(value)); }
    /// Synonym for replacing one compile-time-selected logical lane.
    template<std::size_t I> requires(I<N)
    constexpr Self replace(T value) const noexcept { return this->template set<I>(value); }
    /// Encode normal powers of two for integral exponents in [-126,127].
    friend constexpr Self normal_pow2(Self n) noexcept requires(std::same_as<T,float>) { return restore(normal_pow2(project(n))); }
    /// Shift logical lanes left by the valid immediate count.
    template<std::size_t K> requires(simd_integer_element<T> && K<sizeof(T)*8)
    friend constexpr Self operator<<(Self value,imm_t<K>) noexcept { return value.template left<K>(); }
    /// Shift logical lanes right by the valid immediate count.
    template<std::size_t K> requires(simd_integer_element<T> && K<sizeof(T)*8)
    friend constexpr Self operator>>(Self value,imm_t<K>) noexcept { return value.template right<K>(); }
    /// Shift unsigned 32-bit lanes by corresponding counts; counts >=32 yield zero.
    friend constexpr Self operator<<(Self value,Self counts) noexcept requires(std::same_as<T,std::uint32_t>) {
      return restore(project(value)<<project(counts));
    }
    /// Shift logical lanes left by a valid immediate count.
    template<std::size_t K>
    constexpr Self left() const noexcept
      requires requires(emulated_type x) { x.template left<K>(); } {
      return restore(project(static_cast<Self const &>(*this)).template left<K>());
    }
    /// Shift logical lanes right by a valid immediate count.
    template<std::size_t K>
    constexpr Self right() const noexcept
      requires requires(emulated_type x) { x.template right<K>(); } {
      return restore(project(static_cast<Self const &>(*this)).template right<K>());
    }
  };
}
