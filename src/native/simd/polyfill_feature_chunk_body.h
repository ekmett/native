// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Repeated under a native half feature target. These bridges deliberately have
// ordinary constexpr inlining: the generic register scope may call a stronger
// feature scope, while callers compiled for that feature can inline the bridge.
namespace native::detail {
  template<class T,std::size_t N,isa<> A> requires NATIVE_POLYFILL_FEATURE_CHUNK(T,N,A)
  struct polyfill_feature_chunk<T,N,A> {
    using vector_type=simd<T,N,A>;
    using native_type=typename vector_type::native_type;
    static constexpr std::size_t lanes=N;
    vector_type value{};
    constexpr polyfill_feature_chunk() noexcept=default;
    constexpr explicit polyfill_feature_chunk(T element) noexcept : value(element) {}
    static constexpr polyfill_feature_chunk from_native(native_type bits) noexcept {
      polyfill_feature_chunk result; result.value=vector_type::from_native(bits); return result;
    }
    constexpr native_type to_native() const noexcept { return value.to_native(); }
    template<std::size_t Alignment=1> static constexpr polyfill_feature_chunk load_memory(T const * p) noexcept {
      polyfill_feature_chunk result; result.value=vector_type::template load_memory<Alignment>(p); return result;
    }
    template<std::size_t Alignment=1> constexpr void store_memory(T * p) const noexcept {
      value.template store_memory<Alignment>(p);
    }
#define NATIVE_POLYFILL_FEATURE_BINARY(OP) \
    friend constexpr polyfill_feature_chunk operator OP(polyfill_feature_chunk a,polyfill_feature_chunk b) noexcept \
      requires requires(vector_type x) { { x OP x } -> std::same_as<vector_type>; } { \
      polyfill_feature_chunk result; result.value=a.value OP b.value; return result; \
    }
    NATIVE_POLYFILL_FEATURE_BINARY(+)
    NATIVE_POLYFILL_FEATURE_BINARY(-)
    NATIVE_POLYFILL_FEATURE_BINARY(*)
    NATIVE_POLYFILL_FEATURE_BINARY(/)
#undef NATIVE_POLYFILL_FEATURE_BINARY
#define NATIVE_POLYFILL_FEATURE_COMPARE(OP) \
    friend constexpr auto operator OP(polyfill_feature_chunk a,polyfill_feature_chunk b) noexcept \
      requires requires(vector_type x) { (x OP x).to_bitset(); } { return a.value OP b.value; }
    NATIVE_POLYFILL_FEATURE_COMPARE(==)
    NATIVE_POLYFILL_FEATURE_COMPARE(!=)
    NATIVE_POLYFILL_FEATURE_COMPARE(<)
    NATIVE_POLYFILL_FEATURE_COMPARE(<=)
    NATIVE_POLYFILL_FEATURE_COMPARE(>)
    NATIVE_POLYFILL_FEATURE_COMPARE(>=)
#undef NATIVE_POLYFILL_FEATURE_COMPARE
    friend constexpr polyfill_feature_chunk operator-(polyfill_feature_chunk a) noexcept
      requires requires(vector_type x) { { -x } -> std::same_as<vector_type>; } {
      polyfill_feature_chunk result; result.value=-a.value; return result;
    }
#define NATIVE_POLYFILL_FEATURE_UNARY(NAME) \
    friend constexpr polyfill_feature_chunk NAME(polyfill_feature_chunk a) noexcept \
      requires requires(vector_type x) { { NAME(x) } -> std::same_as<vector_type>; } { \
      polyfill_feature_chunk result; result.value=NAME(a.value); return result; \
    }
    NATIVE_POLYFILL_FEATURE_UNARY(sqrt)
    NATIVE_POLYFILL_FEATURE_UNARY(abs)
    NATIVE_POLYFILL_FEATURE_UNARY(floor)
    NATIVE_POLYFILL_FEATURE_UNARY(ceil)
    NATIVE_POLYFILL_FEATURE_UNARY(trunc)
    NATIVE_POLYFILL_FEATURE_UNARY(round_even)
#undef NATIVE_POLYFILL_FEATURE_UNARY
    friend constexpr polyfill_feature_chunk fma(polyfill_feature_chunk a,polyfill_feature_chunk b,polyfill_feature_chunk c) noexcept
      requires requires(vector_type x) { { fma(x,x,x) } -> std::same_as<vector_type>; } {
      polyfill_feature_chunk result; result.value=fma(a.value,b.value,c.value); return result;
    }
  };
}
