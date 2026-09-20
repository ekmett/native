// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Repeated in each raw backend's target scope. These ordinary-inline leaves
// let a generic native_inline algorithm reach its caller before leaf inlining.
namespace wide::detail {
  template<class V> requires requires { V::architecture; } && SIMD_ARCH_REQUIRES(V::architecture)
  struct native_ops<V> {
    template<class T> static inline V constant(T x) noexcept { return V(x); }
#define SIMD_WIDE_NATIVE_BINARY(name,op) \
    static inline auto name(V a,V b) noexcept { return a op b; }
    SIMD_WIDE_NATIVE_BINARY(add,+)
    SIMD_WIDE_NATIVE_BINARY(sub,-)
    SIMD_WIDE_NATIVE_BINARY(mul,*)
    SIMD_WIDE_NATIVE_BINARY(div,/)
    SIMD_WIDE_NATIVE_BINARY(bit_and,&)
    SIMD_WIDE_NATIVE_BINARY(bit_or,|)
    SIMD_WIDE_NATIVE_BINARY(bit_xor,^)
    SIMD_WIDE_NATIVE_BINARY(equal,==)
    SIMD_WIDE_NATIVE_BINARY(unequal,!=)
    SIMD_WIDE_NATIVE_BINARY(less,<)
    SIMD_WIDE_NATIVE_BINARY(less_equal,<=)
    SIMD_WIDE_NATIVE_BINARY(greater,>)
    SIMD_WIDE_NATIVE_BINARY(greater_equal,>=)
#undef SIMD_WIDE_NATIVE_BINARY
    static inline auto negate(V a) noexcept { return -a; }
    static inline auto bit_not(V a) noexcept { return ~a; }
    static inline auto logical_not(V a) noexcept { return !a; }
    static inline auto minimum(V a, V b) noexcept { return select(a < b, a, b); }
    static inline auto maximum(V a, V b) noexcept { return select(a > b, a, b); }
    static inline auto absolute(V a) noexcept { return abs(a); }
    static inline auto root(V a) noexcept { return sqrt(a); }
    static inline auto downward(V a) noexcept { return floor(a); }
    static inline auto upward(V a) noexcept { return ceil(a); }
    static inline auto truncate(V a) noexcept { return trunc(a); }
    static inline auto round(V a) noexcept { return round_even(a); }
    static inline auto fused(V a, V b, V c) noexcept { return fma(a, b, c); }
    static inline auto scale_all(V a,V n) noexcept { return scaleb(a,n); }
    template<class M>
    static inline auto choose(M m,V a,V b) noexcept { return select(m,a,b); }
    template<class M>
    static inline auto scale(M m, V a, V n) noexcept { return masked_scaleb_zero(m, a, n); }
    template<class M>
    static inline auto scale_merge(M m,V prior,V a,V n) noexcept { return masked_scaleb(m,prior,a,n); }
    static inline auto encode(V a) noexcept { return a.bits(); }
    static inline auto decode(V a) noexcept {
      using F=typename V::template rebind<float>;
      return F::from_bits(a);
    }
    template<unsigned Shift>
    static inline auto left(V a) noexcept { return a.template left<Shift>(); }
    template<class T>
    static inline auto mask_words(V a) noexcept { return ::simd::mask_bits<T>(a); }

    // These conversions serve the bounded nonnegative trigonometric reducer.
    // Its integer values are below INT32_MAX, matching the original signed
    // native conversion instructions; this is not a general uint32 conversion.
    static inline auto trig_integer(V a) noexcept {
      using I=typename V::template rebind<std::uint32_t>;
      if constexpr (V::lanes==1) return I(static_cast<std::uint32_t>(a.value));
      else if constexpr (V::lanes==2 || V::lanes==3)
        return I::from_storage(native_ops<typename V::storage_type>::trig_integer(a.to_storage()));
#if SIMD_HAS_AVX2
      else if constexpr (V::lanes==4) return I::from_native(_mm_cvttps_epi32(a.value));
      else if constexpr (V::lanes==8) return I::from_native(_mm256_cvttps_epi32(a.value));
#endif
#if SIMD_HAS_AVX512F
      else if constexpr (V::lanes==16) return I::from_native(_mm512_cvttps_epi32(a.value));
#endif
#if SIMD_HAS_ARM_NEON
      else if constexpr (V::lanes==4)
        return I::from_native(vreinterpretq_u8_s32(vcvtq_s32_f32(a.value)));
#endif
    }
    static inline auto trig_float(V a) noexcept {
      using F=typename V::template rebind<float>;
      if constexpr (V::lanes==1) return F(static_cast<float>(a.value));
      else if constexpr (V::lanes==2 || V::lanes==3)
        return F::from_storage(native_ops<typename V::storage_type>::trig_float(a.to_storage()));
#if SIMD_HAS_AVX2
      else if constexpr (V::lanes==4) return F(_mm_cvtepi32_ps(a.value));
      else if constexpr (V::lanes==8) return F(_mm256_cvtepi32_ps(a.value));
#endif
#if SIMD_HAS_AVX512F
      else if constexpr (V::lanes==16) return F(_mm512_cvtepi32_ps(a.value));
#endif
#if SIMD_HAS_ARM_NEON
      else if constexpr (V::lanes==4) return F(vcvtq_f32_s32(vreinterpretq_s32_u8(a.value)));
#endif
    }
  };
}
