// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

namespace native::detail {
  enum class polyfill_element_kind { ordinary, binary16, bfloat16, binary64 };
  // Half storage types specialize this after native.numerics is imported. Their
  // ownership stays with that module; the semantic implementation remains GMF.
  template<class T> struct polyfill_element_traits {
    static constexpr bool supported=ordinary_simd_element<T> || std::same_as<T,double>;
    static constexpr bool arithmetic=std::same_as<T,float> || std::same_as<T,double>;
    static constexpr auto kind=std::same_as<T,double>?polyfill_element_kind::binary64:polyfill_element_kind::ordinary;
    using format=std::conditional_t<sizeof(T)==8,constexpr_float::binary64,
      std::conditional_t<sizeof(T)==2,constexpr_float::binary16,constexpr_float::binary32>>;
  };
  // Hardware-only lookup must never recursively select the emulated shape.
  template<isa<> A> inline constexpr isa<> hardware_isa=[] {
    auto result=A;
    result.allow_polyfill=false;
    return result;
  }();
  // Storage-only ordinary shapes are supplied by native.simd's instruction
  // interfaces. Keep their availability shared without depending on whether a
  // textual or module consumer has seen those later class definitions.
  template<class T,std::size_t N,isa<> A>
  inline constexpr bool instruction_only_shape=[] {
    if constexpr(!polyfill_element_traits<T>::supported || N<2 || N>64/sizeof(T)) return false;
    else {
      constexpr auto bytes=sizeof(T)*N;
#if NATIVE_HOST_NEON
      if constexpr(!(neon<=A)) return false;
      if constexpr(polyfill_element_traits<T>::kind==polyfill_element_kind::binary16)
        return N==4 || (N==8 && !(neon_fp16<=A));
      if constexpr(polyfill_element_traits<T>::kind==polyfill_element_kind::bfloat16)
        return N==4 || (N==8 && !(neon_bf16<=A));
      if constexpr(std::same_as<T,double>) return N==2;
      return simd_integer_element<T> && sizeof(T)<4 && bytes==8;
#elif NATIVE_HOST_X86
      if constexpr(!A.has(x86_feature::sse2)) return false;
      if constexpr(polyfill_element_traits<T>::kind==polyfill_element_kind::binary16) return N==4 || N==8;
      if constexpr(polyfill_element_traits<T>::kind==polyfill_element_kind::bfloat16)
        return N==4 || (N==8 && !(avx512_bf16<=A));
      if constexpr(bytes==8) return simd_integer_element<T> && sizeof(T)<4;
      if constexpr(bytes==16 || bytes==32) {
        if constexpr(bytes==32 && !A.has(x86_feature::avx)) return false;
        return std::same_as<T,double> || (!(avx2<=A) && (simd_integer_element<T> || std::same_as<T,float>));
      }
      if constexpr(bytes==64 && A.has(x86_feature::avx512f))
        return std::same_as<T,double> || ((simd_integer_element<T> || std::same_as<T,float>) &&
          (!(kernel_base<=A) || (sizeof(T)<4 && !A.has(x86_feature::avx512bw))));
      return false;
#else
      return false;
#endif
    }
  }();
  template<class T,std::size_t N,isa<> A>
  concept polyfill_shape=polyfill_element_traits<T>::supported && A.has(polyfill) && N>0 && N<=64 &&
    !instruction_only_shape<T,N,A> && !requires { sizeof(simd<T,N,hardware_isa<A>>); };

  template<class T,isa<> A,std::size_t M=64/sizeof(T)>
  consteval std::size_t polyfill_register_lanes() noexcept {
    if constexpr(!instruction_only_shape<T,M,A> &&
      requires { typename simd<T,M,hardware_isa<A>>::native_type; }) return M;
    else if constexpr(M>1) return polyfill_register_lanes<T,A,M/2>();
    else return 1;
  }
}

namespace native::detail {
  /// \ingroup masks
  /// Compact logical mask for an emulated shape, with unused bits cleared.
  template<class Self,std::size_t N,isa<> A> requires(N>0 && N<=64)
  struct polyfill_mask {
    static constexpr isa<> architecture=A;
    static constexpr std::size_t lanes=N;
    using native_type=std::uint64_t;
    using mask_type=Self;
    using mask=Self;
    static constexpr bool compact=true;
  private:
    native_type value_{};
    static constexpr native_type active=[] {
      if constexpr(N==64) return ~native_type{};
      else return (native_type{1}<<N)-1;
    }();
  public:
    /// Initialize every logical mask lane to false.
    constexpr polyfill_mask() noexcept=default;
    /// Broadcast one truth value to the logical lanes.
    explicit constexpr polyfill_mask(bool value) noexcept : value_(value?active:0) {}
    /// Copy one bit per logical lane, clearing unused high bits.
    static constexpr Self from_bitset(native_type bits) noexcept {
      Self result; result.value_=bits&active; return result;
    }
    /// Copy a logical mask bitset.
    static constexpr Self from_bits(native_type bits) noexcept { return from_bitset(bits); }
    /// Adopt the compact storage, clearing padding bits.
    static constexpr Self from_native(native_type bits) noexcept { return from_bitset(bits); }
    /// Return the compact representation.
    constexpr native_type to_native() const noexcept { return value_; }
    /// Return one bit per logical mask lane.
    constexpr native_type to_bitset() const noexcept { return value_; }
    /// Return one bit per logical mask lane.
    constexpr native_type bits() const noexcept { return value_; }
    /// Test whether any logical lane is true.
    friend constexpr bool any(Self value) noexcept { return value.value_!=0; }
    /// Test whether every logical lane is true.
    friend constexpr bool all(Self value) noexcept { return value.value_==active; }
    /// Test whether every logical lane is false.
    friend constexpr bool none(Self value) noexcept { return value.value_==0; }
    /// Complement every logical lane, leaving padding clear.
    friend constexpr Self operator~(Self value) noexcept { return from_bitset(~value.value_); }
    /// Complement every logical lane.
    friend constexpr Self operator!(Self value) noexcept { return ~value; }
    /// Intersect corresponding mask lanes.
    friend constexpr Self operator&(Self a,Self b) noexcept { return from_bitset(a.value_&b.value_); }
    /// Unite corresponding mask lanes.
    friend constexpr Self operator|(Self a,Self b) noexcept { return from_bitset(a.value_|b.value_); }
    /// Toggle corresponding mask lanes.
    friend constexpr Self operator^(Self a,Self b) noexcept { return from_bitset(a.value_^b.value_); }
    /// Compare corresponding mask truth values.
    friend constexpr Self operator==(Self a,Self b) noexcept { return ~(a^b); }
    /// Compare corresponding mask truth values for inequality.
    friend constexpr Self operator!=(Self a,Self b) noexcept { return a^b; }
    /// Intersect with another mask in place.
    constexpr Self & operator&=(Self b) noexcept { return static_cast<Self &>(*this)=static_cast<Self &>(*this)&b; }
    /// Unite with another mask in place.
    constexpr Self & operator|=(Self b) noexcept { return static_cast<Self &>(*this)=static_cast<Self &>(*this)|b; }
    /// Toggle another mask in place.
    constexpr Self & operator^=(Self b) noexcept { return static_cast<Self &>(*this)=static_cast<Self &>(*this)^b; }
    /// Choose each mask lane from a or b.
    friend constexpr Self select(Self m,Self a,Self b) noexcept { return (m&a)|(~m&b); }
  };

  template<std::size_t N,isa<> A> requires(N>0 && N<=64)
  struct polyfill_predicate : polyfill_mask<polyfill_predicate<N,A>,N,A> {
    using base=polyfill_mask<polyfill_predicate,N,A>;
    using base::base;
    constexpr polyfill_predicate() noexcept=default;
  };
}

namespace native {
  /// Compact predicate for scalar emulation, independent of native register masks.
  template<std::size_t N,isa<> A> requires(A.has(polyfill) && A<=scalar && N>0 && N<=64)
  struct predicate<N,A> : detail::polyfill_mask<predicate<N,A>,N,A> {
    using base=detail::polyfill_mask<predicate,N,A>;
    using base::base;
    constexpr predicate() noexcept=default;
  };
}

#include "native/simd/polyfill_scalar.h"
