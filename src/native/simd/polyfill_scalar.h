// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
namespace native::detail {
  struct polyfill_half_control {
    constexpr_float::rounding mode=constexpr_float::rounding::nearest_even;
    constexpr_float::policy policy{};
  };
  // These are baseline control-register reads, never permission to execute a
  // half instruction. Keep them in the GMF with every other platform definition.
  inline polyfill_half_control polyfill_half_environment() noexcept {
    polyfill_half_control result;
#if NATIVE_HOST_NEON
    std::uint64_t control;
    __asm__ volatile("mrs %0, fpcr":"=r"(control)::"memory");
    constexpr constexpr_float::rounding modes[]{constexpr_float::rounding::nearest_even,
      constexpr_float::rounding::upward,constexpr_float::rounding::downward,constexpr_float::rounding::toward_zero};
    result.mode=modes[(control>>22)&3];
    result.policy.flush_inputs=result.policy.flush_outputs=(control&(std::uint64_t{1}<<19))!=0;
    if(control&(std::uint64_t{1}<<25)) result.policy.nan=constexpr_float::nan_propagation::default_nan;
#elif NATIVE_HOST_X86
    std::uint32_t control;
    __asm__ volatile("stmxcsr %0":"=m"(control)::"memory");
    constexpr constexpr_float::rounding modes[]{constexpr_float::rounding::nearest_even,
      constexpr_float::rounding::downward,constexpr_float::rounding::upward,constexpr_float::rounding::toward_zero};
    result.mode=modes[(control>>13)&3];
    // Native x86 half arithmetic ignores MXCSR DAZ/FTZ.
#endif
    return result;
  }

  /// One representation-preserving scalar chunk for a maintained floating type.
  /// Software half arithmetic rounds once in the element format. BF16 remains
  /// storage/conversion-only, matching its established element contract.
  template<class T,isa<> A> struct polyfill_scalar {
    using value_type=T;
    using native_type=T;
    using format=typename polyfill_element_traits<T>::format;
    using word=typename format::bits_type;
    using mask=polyfill_predicate<1,A>;
    T value{};
    constexpr polyfill_scalar() noexcept=default;
    constexpr polyfill_scalar(T x) noexcept : value(x) {}
    static constexpr auto bits(T x) noexcept { return std::bit_cast<word>(x); }
    static constexpr T element(word x) noexcept { return std::bit_cast<T>(x); }
    static constexpr polyfill_scalar from_native(T x) noexcept { return x; }
    constexpr T to_native() const noexcept { return value; }
    template<std::size_t Alignment=1>
    static constexpr polyfill_scalar load_memory(T const * p) noexcept {
      if consteval { return *p; } else { T x; std::memcpy(&x,p,sizeof(T)); return x; }
    }
    static constexpr polyfill_scalar load(T const * p) noexcept { return load_memory(p); }
    template<std::size_t Alignment=1>
    constexpr void store_memory(T * p) const noexcept {
      if consteval { *p=value; } else { std::memcpy(p,&value,sizeof(T)); }
    }
    constexpr void store(T * p) const noexcept { store_memory(p); }
#define NATIVE_POLYFILL_SCALAR_BINARY(OP,FN) \
    /** Apply element-format arithmetic with one rounding. */ \
    friend constexpr polyfill_scalar operator OP(polyfill_scalar a,polyfill_scalar b) noexcept \
      requires(polyfill_element_traits<T>::arithmetic) { \
      if consteval { return element(constexpr_float::FN<format>(bits(a.value),bits(b.value))); } \
      else { \
        if constexpr(sizeof(T)==2) { \
          auto control=polyfill_half_environment(); \
          return element(constexpr_float::FN<format>(bits(a.value),bits(b.value),control.mode,control.policy)); \
        } \
        else return T(a.value OP b.value); \
      } \
    }
    NATIVE_POLYFILL_SCALAR_BINARY(+,add_bits)
    NATIVE_POLYFILL_SCALAR_BINARY(-,sub_bits)
    NATIVE_POLYFILL_SCALAR_BINARY(*,mul_bits)
    NATIVE_POLYFILL_SCALAR_BINARY(/,div_bits)
#undef NATIVE_POLYFILL_SCALAR_BINARY
    /// Negate by changing only the sign bit.
    friend constexpr polyfill_scalar operator-(polyfill_scalar a) noexcept
      requires(polyfill_element_traits<T>::arithmetic) { return element(bits(a.value)^format::sign_mask); }
    /// Compare without treating NaNs as ordered values.
    friend constexpr mask operator<(polyfill_scalar a,polyfill_scalar b) noexcept
      requires(polyfill_element_traits<T>::arithmetic) {
      if consteval { return mask::from_bitset(constexpr_float::less_bits<format>(bits(a.value),bits(b.value))); }
      else {
        if constexpr(sizeof(T)==2) {
          auto p=polyfill_half_environment().policy;
          return mask::from_bitset(constexpr_float::less_bits<format>(constexpr_float::flush_input<format>(bits(a.value),p),
            constexpr_float::flush_input<format>(bits(b.value),p)));
        } else return mask::from_bitset(a.value<b.value);
      }
    }
    /// Compare numerical values; both signed zeros compare equal.
    friend constexpr mask operator==(polyfill_scalar a,polyfill_scalar b) noexcept
      requires(polyfill_element_traits<T>::arithmetic) {
      if consteval { return mask::from_bitset(constexpr_float::equal_bits<format>(bits(a.value),bits(b.value))); }
      else {
        if constexpr(sizeof(T)==2) {
          auto p=polyfill_half_environment().policy;
          return mask::from_bitset(constexpr_float::equal_bits<format>(constexpr_float::flush_input<format>(bits(a.value),p),
            constexpr_float::flush_input<format>(bits(b.value),p)));
        } else return mask::from_bitset(a.value==b.value);
      }
    }
    /// Fuse multiply and add, rounding once to the lane format.
    friend constexpr polyfill_scalar fma(polyfill_scalar a,polyfill_scalar b,polyfill_scalar c) noexcept
      requires(polyfill_element_traits<T>::arithmetic) {
      if consteval { return element(constexpr_float::fma_bits<format>(bits(a.value),bits(b.value),bits(c.value))); }
      else {
        if constexpr(sizeof(T)==2) {
          auto control=polyfill_half_environment();
          return element(constexpr_float::fma_bits<format>(bits(a.value),bits(b.value),bits(c.value),control.mode,control.policy));
        }
        else return std::fma(a.value,b.value,c.value);
      }
    }
    /// Correctly rounded square root in the lane format.
    friend constexpr polyfill_scalar sqrt(polyfill_scalar a) noexcept
      requires(polyfill_element_traits<T>::arithmetic) {
      if consteval { return element(constexpr_float::sqrt_bits<format>(bits(a.value))); }
      else {
        if constexpr(sizeof(T)==2) {
          auto control=polyfill_half_environment();
          return element(constexpr_float::sqrt_bits<format>(bits(a.value),control.mode,control.policy));
        }
        else return std::sqrt(a.value);
      }
    }
    /// Clear the sign bit, preserving every other representation bit.
    friend constexpr polyfill_scalar abs(polyfill_scalar a) noexcept { return element(bits(a.value)&~format::sign_mask); }
#define NATIVE_POLYFILL_SCALAR_ROUND(NAME,MODE) \
    /** Round to an integral value in the element format. */ \
    friend constexpr polyfill_scalar NAME(polyfill_scalar a) noexcept \
      requires(polyfill_element_traits<T>::arithmetic) { \
      return element(constexpr_float::round_integral_bits<format>(bits(a.value),constexpr_float::rounding::MODE)); \
    }
    NATIVE_POLYFILL_SCALAR_ROUND(floor,downward)
    NATIVE_POLYFILL_SCALAR_ROUND(ceil,upward)
    NATIVE_POLYFILL_SCALAR_ROUND(trunc,toward_zero)
    NATIVE_POLYFILL_SCALAR_ROUND(round_even,nearest_even)
#undef NATIVE_POLYFILL_SCALAR_ROUND
  };
  template<class T,std::size_t N,isa<> A,bool Native=requires { typename simd<T,N,A>::native_type; }>
  struct polyfill_chunk { using type=polyfill_scalar<T,A>; };
  template<class T,std::size_t N,isa<> A> struct polyfill_chunk<T,N,A,true> { using type=simd<T,N,A>; };
}
