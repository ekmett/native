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
    // SIMD128 has no fused instruction. Keep its polynomial graph explicitly
    // noncontracting, including in relaxed-SIMD callers; do not weaken fma.
    static inline constexpr auto polynomial_madd(V a, V b, V c) noexcept {
#if NATIVE_HAS_WASM_SIMD128
#pragma clang fp contract(off)
      return a * b + c;
#else
      return fma(a, b, c);
#endif
    }
#if NATIVE_HOST_X86
    // Exp only needs a normal power-of-two field, or zero. CVTT returns the
    // signed indefinite integer for NaN/out-of-range inputs; MAX maps it and
    // negative fields to zero without testing the floating-point input.
    static inline constexpr V exp_factor(V biased) noexcept {
      if consteval {
        auto words=::native::detail::float_constant::words(biased);
        for(auto & word:words) {
          auto const magnitude=word & 0x7fffffffu;
          auto const integer=magnitude>=0x4f000000u ? INT32_MIN :
            ::native::detail::float_constant::fcvtzs(word);
          word=std::uint32_t(integer>0 ? integer : 0)<<23;
        }
        return V::load_bits(words.data());
      } else {
        if constexpr(V::lanes==1) {
          auto const integer=_mm_cvttss_si32(_mm_set_ss(biased.to_native()));
          return V::from_bits(std::uint32_t(integer>0 ? integer : 0)<<23);
        } else if constexpr(V::lanes==2 || V::lanes==3) {
          // Zero input padding converts to zero and stays zero after the shift.
          V result;
          result.value=__builtin_bit_cast(typename V::native_type,
            native_ops<typename V::storage_type>::exp_factor(biased.to_storage()).to_native());
          return result;
        }
#if NATIVE_HAS_AVX2
        else if constexpr(V::lanes==4) {
          auto const integer=_mm_max_epi32(_mm_cvttps_epi32(biased.to_native()),_mm_setzero_si128());
          return V::from_native(_mm_castsi128_ps(_mm_slli_epi32(integer,23)));
        } else if constexpr(V::lanes==8) {
          auto const integer=_mm256_max_epi32(_mm256_cvttps_epi32(biased.to_native()),_mm256_setzero_si256());
          return V::from_native(_mm256_castsi256_ps(_mm256_slli_epi32(integer,23)));
        }
#endif
      }
    }
#endif
    template<class M>
    static inline constexpr auto exp_scale(M in_range, V replacement, V y, V n) noexcept {
#if NATIVE_HAS_AVX512F
      if constexpr (V::lanes == 1 || V::lanes == 16 || (NATIVE_HAS_AVX512VL && V::lanes > 1))
        return masked_scaleb(in_range, replacement, y, n);
      else
#endif
      {
#if NATIVE_HOST_NEON
        // The biased field is unsigned: FCVTZU maps underflow and NaN to zero
        // without a compare. NaN y survives the multiply. Range flags stay off
        // the arithmetic chain and select the completed result below.
        auto const biased = ::native::fcvtzu(n + V(127.f));
        auto const result = y * V::from_bits(biased.template left<23>());
#elif NATIVE_HOST_X86
        auto const result = y * exp_factor(n + V(127.f));
#else
        // This is exp's bounded reconstruction, not a scaling instruction.
        // Finite n within exp's output range is integral in [-150,128].
        // Split the biased exponent into two normal powers of two:
        // the first product is exact and normal; only the second can underflow.
        // Other scalar backends still use a C++ cast with a finite precondition.
        n = select(in_range & (n == n), n, V(0.f));
        auto const biased = trig_integer(n + V(254.f));
        auto const first = biased.template right<1>();
        auto const second = biased - first;
        auto const result = (y * V::from_bits(first.template left<23>())) *
          V::from_bits(second.template left<23>());
#endif
        return select(in_range, result, replacement);
      }
    }
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

    // Math reducers retain the signed conversion's bits in unsigned storage
    // for exponent fields and shifts. ARM uses the defined FCVTZS instruction;
    // the other scalar/constant paths require a bounded nonnegative input.
    static inline constexpr auto trig_integer(V a) noexcept {
      using I=typename V::template rebind<std::uint32_t>;
#if NATIVE_HOST_NEON
      return I::from_native(__builtin_bit_cast(typename I::native_type,
        ::native::fcvtzs(a).to_native()));
#else
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
#if NATIVE_HAS_WASM_SIMD128
      else if constexpr (V::lanes==4) return ::native::trunc_sat<std::uint32_t>(a);
#endif
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
#if NATIVE_HAS_WASM_SIMD128
      else if constexpr (V::lanes==4) return ::native::convert<float>(a);
#endif
    }
  };
}
