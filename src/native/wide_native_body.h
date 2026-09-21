// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Repeated in each raw backend's target scope. These ordinary-inline leaves
// let a generic native_inline algorithm reach its caller before leaf inlining.
namespace wide::detail {
  template<class V> requires requires { V::architecture; } && NATIVE_ARCH_REQUIRES(V::architecture)
  struct native_ops<V> {
    template<class T> static inline constexpr V constant(T x) noexcept { return V(x); }
#define NATIVE_WIDE_NATIVE_BINARY(name,op) \
    static inline constexpr auto name(V a,V b) noexcept { return a op b; }
    NATIVE_WIDE_NATIVE_BINARY(add,+)
    NATIVE_WIDE_NATIVE_BINARY(sub,-)
    NATIVE_WIDE_NATIVE_BINARY(mul,*)
    NATIVE_WIDE_NATIVE_BINARY(div,/)
    NATIVE_WIDE_NATIVE_BINARY(bit_and,&)
    NATIVE_WIDE_NATIVE_BINARY(bit_or,|)
    NATIVE_WIDE_NATIVE_BINARY(bit_xor,^)
    NATIVE_WIDE_NATIVE_BINARY(equal,==)
    NATIVE_WIDE_NATIVE_BINARY(unequal,!=)
    NATIVE_WIDE_NATIVE_BINARY(less,<)
    NATIVE_WIDE_NATIVE_BINARY(less_equal,<=)
    NATIVE_WIDE_NATIVE_BINARY(greater,>)
    NATIVE_WIDE_NATIVE_BINARY(greater_equal,>=)
#undef NATIVE_WIDE_NATIVE_BINARY
    static inline constexpr auto negate(V a) noexcept { return -a; }
    static inline constexpr auto bit_not(V a) noexcept { return ~a; }
    static inline constexpr auto logical_not(V a) noexcept { return !a; }
    static inline constexpr auto minimum(V a, V b) noexcept { return select(a < b, a, b); }
    static inline constexpr auto maximum(V a, V b) noexcept { return select(a > b, a, b); }
    static inline constexpr auto absolute(V a) noexcept { return abs(a); }
    static inline constexpr auto root(V a) noexcept { return sqrt(a); }
    static inline constexpr auto downward(V a) noexcept { return floor(a); }
    static inline constexpr auto upward(V a) noexcept { return ceil(a); }
    static inline constexpr auto truncate(V a) noexcept { return trunc(a); }
    static inline constexpr auto round(V a) noexcept { return round_even(a); }
    static inline constexpr auto fused(V a, V b, V c) noexcept { return fma(a, b, c); }
    static inline constexpr auto scale_all(V a,V n) noexcept { return scaleb(a,n); }
    template<class M>
    static inline constexpr auto choose(M m,V a,V b) noexcept { return select(m,a,b); }
    template<class M>
    static inline constexpr auto scale(M m, V a, V n) noexcept { return masked_scaleb_zero(m, a, n); }
    template<class M>
    static inline constexpr auto scale_merge(M m,V prior,V a,V n) noexcept { return masked_scaleb(m,prior,a,n); }
    static inline constexpr auto encode(V a) noexcept { return a.bits(); }
    static inline constexpr auto decode(V a) noexcept {
      using F=typename V::template rebind<float>;
      return F::from_bits(a);
    }
    template<unsigned Shift>
    static inline constexpr auto left(V a) noexcept { return a.template left<Shift>(); }
    template<class T>
    static inline constexpr auto mask_words(V a) noexcept { return ::native::mask_bits<T>(a); }

    // These conversions serve the bounded nonnegative trigonometric reducer.
    // Its integer values are below INT32_MAX, matching the original signed
    // native conversion instructions; this is not a general uint32 conversion.
    static inline constexpr auto trig_integer(V a) noexcept {
      using I=typename V::template rebind<std::uint32_t>;
      if consteval {
        std::array<float,V::lanes> x{};std::array<std::uint32_t,V::lanes> y{};a.store(x.data());
        for(std::size_t i=0;i<V::lanes;++i) y[i]=static_cast<std::uint32_t>(x[i]);
        return I::load(y.data());
      }
      if constexpr (V::lanes==1) return I(static_cast<std::uint32_t>(a.value));
      else if constexpr (V::lanes==2 || V::lanes==3)
        return I::from_storage(native_ops<typename V::storage_type>::trig_integer(a.to_storage()));
#if NATIVE_HAS_AVX2
      else if constexpr (V::lanes==4) return I::from_native(_mm_cvttps_epi32(a.value));
      else if constexpr (V::lanes==8) return I::from_native(_mm256_cvttps_epi32(a.value));
#endif
#if NATIVE_HAS_AVX512F
      else if constexpr (V::lanes==16) return I::from_native(_mm512_cvttps_epi32(a.value));
#endif
#if NATIVE_HAS_ARM_NEON
      else if constexpr (V::lanes==4)
        return I::from_native(vreinterpretq_u8_s32(vcvtq_s32_f32(a.value)));
#endif
    }
    static inline constexpr auto trig_float(V a) noexcept {
      using F=typename V::template rebind<float>;
      if consteval {
        std::array<std::uint32_t,V::lanes> x{};std::array<float,V::lanes> y{};a.store(x.data());
        for(std::size_t i=0;i<V::lanes;++i) y[i]=static_cast<float>(x[i]);
        return F::load(y.data());
      }
      if constexpr (V::lanes==1) return F(static_cast<float>(a.value));
      else if constexpr (V::lanes==2 || V::lanes==3)
        return F::from_storage(native_ops<typename V::storage_type>::trig_float(a.to_storage()));
#if NATIVE_HAS_AVX2
      else if constexpr (V::lanes==4) return F(_mm_cvtepi32_ps(a.value));
      else if constexpr (V::lanes==8) return F(_mm256_cvtepi32_ps(a.value));
#endif
#if NATIVE_HAS_AVX512F
      else if constexpr (V::lanes==16) return F(_mm512_cvtepi32_ps(a.value));
#endif
#if NATIVE_HAS_ARM_NEON
      else if constexpr (V::lanes==4) return F(vcvtq_f32_s32(vreinterpretq_s32_u8(a.value)));
#endif
    }
  };
}
