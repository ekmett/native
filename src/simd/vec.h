#pragma once
// Raw SIMD types and operations for the selected compile-time ISA profile.
#include "simd/config.h"
#include "simd/simd/common.h"

#include <array>
#include <bit>
#include <cstring>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#if SIMD_HAS_AVX2 || SIMD_HAS_AVX512F
#include <immintrin.h>
#endif
#if SIMD_HAS_ARM_NEON
#include <arm_neon.h>
#endif

namespace simd {
  namespace detail::SIMD_BACKEND {
    template <class T> inline constexpr std::size_t mask_lane_bytes = [] {
      if constexpr (::simd::simd_custom_element<T>) return sizeof(typename ::simd::simd_traits<T>::storage_type);
      else return sizeof(T);
    }();
    template <class T> concept mask_element = ::simd::simd_custom_element<T> ||
      (std::is_arithmetic_v<T> && !std::is_const_v<T> && !std::is_volatile_v<T> &&
       (sizeof(T)==1 || sizeof(T)==2 || sizeof(T)==4 || sizeof(T)==8));
    template <std::size_t B,std::size_t N> inline constexpr bool mask_compact =
#if SIMD_HAS_AVX512F
      N>1 && (B>=4
#if SIMD_HAS_AVX512BW
      || B==1 || B==2
#endif
      ) && (B*N==64
#if SIMD_HAS_AVX512VL
      || B*N==16 || B*N==32
#endif
      );
#else
      false;
#endif
    template <std::size_t B,std::size_t N> inline constexpr bool mask_shape = N==1 || (B*N==64 && mask_compact<B,N>)
#if SIMD_HAS_AVX2
      || B*N==16 || B*N==32
#endif
#if SIMD_HAS_ARM_NEON
      || B*N==16
#endif
      ;
    template <std::size_t N> inline constexpr std::uint64_t mask_low_bits = [] {
      if constexpr (N==64) return ~std::uint64_t(0);
      else return (std::uint64_t(1)<<N)-1;
    }();
    template <std::size_t B> using mask_word = std::conditional_t<B==1,std::uint8_t,
      std::conditional_t<B==2,std::uint16_t,std::conditional_t<B==4,std::uint32_t,std::uint64_t>>>;

    template<class U> struct mask_scalar_ops {
      using native_type=U;
      static simd_inline simd_const constexpr native_type normalize(native_type x) noexcept { return x?native_type(~U(0)):U(0); }
      static simd_inline simd_const constexpr native_type broadcast(bool x) noexcept { return normalize(U(x)); }
      static simd_inline simd_const constexpr native_type bit_and(native_type a,native_type b) noexcept { return U(a&b); }
      static simd_inline simd_const constexpr native_type bit_or(native_type a,native_type b) noexcept { return U(a|b); }
      static simd_inline simd_const constexpr native_type bit_xor(native_type a,native_type b) noexcept { return U(a^b); }
      static simd_inline simd_const constexpr native_type bit_not(native_type a) noexcept { return U(~a); }
      static simd_inline simd_const constexpr bool any(native_type a) noexcept { return a!=0; }
      static simd_inline simd_const constexpr bool all(native_type a) noexcept { return a==U(~U(0)); }
      static simd_inline simd_const constexpr std::uint64_t bits(native_type a) noexcept { return a?1:0; }
      static simd_inline simd_const constexpr native_type from_bits(std::uint64_t a) noexcept { return broadcast((a&1)!=0); }
    };
    template <std::size_t N> struct mask_compact_ops {
      using native_type = std::conditional_t<(N<=8),std::uint8_t,std::conditional_t<(N<=16),std::uint16_t,
        std::conditional_t<(N<=32),std::uint32_t,std::uint64_t>>>;
      static simd_inline simd_const constexpr native_type normalize(native_type a) noexcept {
        if constexpr(N==8 || N==16 || N==32 || N==64) return a;
        else return native_type(std::uint64_t(a)&mask_low_bits<N>);
      }
      static simd_inline simd_const constexpr native_type broadcast(bool a) noexcept { return a?native_type(mask_low_bits<N>):native_type(0); }
      static simd_inline simd_const constexpr native_type bit_and(native_type a,native_type b) noexcept {
        if (std::is_constant_evaluated()) return native_type(a&b);
#if SIMD_HAS_AVX512F
#if SIMD_HAS_AVX512DQ
        if constexpr(N<=8) return _kand_mask8(a,b);
        else
#endif
        if constexpr(N<=16) return native_type(_kand_mask16(a,b));
#if SIMD_HAS_AVX512BW
        else if constexpr(N<=32) return _kand_mask32(a,b);
        else return _kand_mask64(a,b);
#else
        else return native_type(a&b);
#endif
#else
        return native_type(a&b);
#endif
      }
      static simd_inline simd_const constexpr native_type bit_or(native_type a,native_type b) noexcept {
        if (std::is_constant_evaluated()) return native_type(a|b);
#if SIMD_HAS_AVX512F
#if SIMD_HAS_AVX512DQ
        if constexpr(N<=8) return _kor_mask8(a,b);
        else
#endif
        if constexpr(N<=16) return native_type(_kor_mask16(a,b));
#if SIMD_HAS_AVX512BW
        else if constexpr(N<=32) return _kor_mask32(a,b);
        else return _kor_mask64(a,b);
#else
        else return native_type(a|b);
#endif
#else
        return native_type(a|b);
#endif
      }
      static simd_inline simd_const constexpr native_type bit_xor(native_type a,native_type b) noexcept {
        if (std::is_constant_evaluated()) return native_type(a^b);
#if SIMD_HAS_AVX512F
#if SIMD_HAS_AVX512DQ
        if constexpr(N<=8) return _kxor_mask8(a,b);
        else
#endif
        if constexpr(N<=16) return native_type(_kxor_mask16(a,b));
#if SIMD_HAS_AVX512BW
        else if constexpr(N<=32) return _kxor_mask32(a,b);
        else return _kxor_mask64(a,b);
#else
        else return native_type(a^b);
#endif
#else
        return native_type(a^b);
#endif
      }
      static simd_inline simd_const constexpr native_type bit_not(native_type a) noexcept {
        if(std::is_constant_evaluated()) return native_type((~std::uint64_t(a))&mask_low_bits<N>);
        // Partial mask8 values complement only their N logical lanes.
        if constexpr(N<8) return bit_xor(a,native_type(mask_low_bits<N>));
#if SIMD_HAS_AVX512F
#if SIMD_HAS_AVX512DQ
        else if constexpr(N==8) return _knot_mask8(a);
#endif
        else if constexpr(N<=16) return native_type(_knot_mask16(a));
#if SIMD_HAS_AVX512BW
        else if constexpr(N<=32) return _knot_mask32(a);
        else return _knot_mask64(a);
#else
        else return native_type(~a);
#endif
#else
        else return native_type(~a);
#endif
      }
      static simd_inline simd_const constexpr bool any(native_type a) noexcept { return a!=0; }
      static simd_inline simd_const constexpr bool all(native_type a) noexcept { return a==native_type(mask_low_bits<N>); }
      static simd_inline simd_const constexpr std::uint64_t bits(native_type a) noexcept { return a; }
      static simd_inline simd_const constexpr native_type from_bits(std::uint64_t a) noexcept { return normalize(native_type(a)); }
    };
    template <std::size_t B,std::size_t N> struct mask_vector_ops {
#if SIMD_HAS_AVX2
      using native_type = std::conditional_t<B*N==16,__m128i,__m256i>;
      static simd_inline simd_const native_type broadcast(bool a) noexcept {
        if constexpr (B*N==16) return _mm_set1_epi32(a?-1:0);
        else return _mm256_set1_epi32(a?-1:0);
      }
      static simd_inline simd_const native_type bit_and(native_type a,native_type b) noexcept {
        if constexpr (B*N==16) return _mm_and_si128(a,b);
        else return _mm256_and_si256(a,b);
      }
      static simd_inline simd_const native_type bit_or(native_type a,native_type b) noexcept {
        if constexpr (B*N==16) return _mm_or_si128(a,b);
        else return _mm256_or_si256(a,b);
      }
      static simd_inline simd_const native_type bit_xor(native_type a,native_type b) noexcept {
        if constexpr (B*N==16) return _mm_xor_si128(a,b);
        else return _mm256_xor_si256(a,b);
      }
      static simd_inline simd_const native_type bit_not(native_type a) noexcept { return bit_xor(a,broadcast(true)); }
      static simd_inline simd_const native_type normalize(native_type a) noexcept {
        auto z=broadcast(false);
        if constexpr (B*N==16) {
          if constexpr (B==1) return bit_not(_mm_cmpeq_epi8(a,z));
          else if constexpr (B==2) return bit_not(_mm_cmpeq_epi16(a,z));
          else if constexpr (B==4) return bit_not(_mm_cmpeq_epi32(a,z));
          else return bit_not(_mm_cmpeq_epi64(a,z));
        } else {
          if constexpr (B==1) return bit_not(_mm256_cmpeq_epi8(a,z));
          else if constexpr (B==2) return bit_not(_mm256_cmpeq_epi16(a,z));
          else if constexpr (B==4) return bit_not(_mm256_cmpeq_epi32(a,z));
          else return bit_not(_mm256_cmpeq_epi64(a,z));
        }
      }
      static simd_inline simd_const bool any(native_type a) noexcept {
        if constexpr (B*N==16) return _mm_testz_si128(a,a)==0;
        else return _mm256_testz_si256(a,a)==0;
      }
      static simd_inline simd_const bool all(native_type a) noexcept {
        if constexpr (B*N==16) return _mm_movemask_epi8(a)==0xffff;
        else return _mm256_movemask_epi8(a)==-1;
      }
      static simd_inline simd_const std::uint64_t bits(native_type a) noexcept {
        std::uint32_t bytes;
        if constexpr (B*N==16) bytes=std::uint32_t(_mm_movemask_epi8(a));
        else bytes=std::uint32_t(_mm256_movemask_epi8(a));
        std::uint64_t result=0;
        for(std::size_t i=0;i<N;++i) result|=std::uint64_t((bytes>>(i*B))&1)<<i;
        return result;
      }
      static simd_inline simd_const native_type from_bits(std::uint64_t bits) noexcept {
        std::array<mask_word<B>,N> a{};
        for(std::size_t i=0;i<N;++i) a[i]=((bits>>i)&1)?mask_word<B>(~mask_word<B>(0)):mask_word<B>(0);
        if constexpr (B*N==16) return _mm_loadu_si128(reinterpret_cast<__m128i const *>(a.data()));
        else return _mm256_loadu_si256(reinterpret_cast<__m256i const *>(a.data()));
      }
#elif SIMD_HAS_ARM_NEON
      using native_type = uint8x16_t;
      static simd_inline simd_const native_type broadcast(bool a) noexcept { return vdupq_n_u8(a?0xff:0); }
      static simd_inline simd_const native_type bit_and(native_type a,native_type b) noexcept { return vandq_u8(a,b); }
      static simd_inline simd_const native_type bit_or(native_type a,native_type b) noexcept { return vorrq_u8(a,b); }
      static simd_inline simd_const native_type bit_xor(native_type a,native_type b) noexcept { return veorq_u8(a,b); }
      static simd_inline simd_const native_type bit_not(native_type a) noexcept { return vmvnq_u8(a); }
      static simd_inline simd_const native_type normalize(native_type a) noexcept {
        if constexpr(B==1) return bit_not(vceqq_u8(a,vdupq_n_u8(0)));
        else if constexpr(B==2) return bit_not(vreinterpretq_u8_u16(vceqq_u16(vreinterpretq_u16_u8(a),vdupq_n_u16(0))));
        else if constexpr(B==4) return bit_not(vreinterpretq_u8_u32(vceqq_u32(vreinterpretq_u32_u8(a),vdupq_n_u32(0))));
        else return bit_not(vreinterpretq_u8_u64(vceqq_u64(vreinterpretq_u64_u8(a),vdupq_n_u64(0))));
      }
      static simd_inline simd_const bool any(native_type a) noexcept { return vmaxvq_u8(a)!=0; }
      static simd_inline simd_const bool all(native_type a) noexcept { return vminvq_u8(a)==0xff; }
      static simd_inline simd_const std::uint64_t bits(native_type a) noexcept {
        std::array<std::uint8_t,16> bytes{};vst1q_u8(bytes.data(),a);
        std::uint64_t result=0;
        for(std::size_t i=0;i<N;++i) result|=std::uint64_t(bytes[i*B]!=0)<<i;
        return result;
      }
      static simd_inline simd_const native_type from_bits(std::uint64_t bits) noexcept {
        std::array<std::uint8_t,16> a{};
        for(std::size_t i=0;i<N;++i) for(std::size_t j=0;j<B;++j) a[i*B+j]=((bits>>i)&1)?0xff:0;
        return vld1q_u8(a.data());
      }
#endif
    };
    template<std::size_t B,std::size_t N> struct mask_vector512_ops {
#if SIMD_HAS_AVX512F
      using native_type=__m512i;
      static simd_inline simd_const native_type broadcast(bool a) noexcept { return _mm512_set1_epi32(a?-1:0); }
      static simd_inline simd_const native_type bit_and(native_type a,native_type b) noexcept { return _mm512_and_si512(a,b); }
      static simd_inline simd_const native_type bit_or(native_type a,native_type b) noexcept { return _mm512_or_si512(a,b); }
      static simd_inline simd_const native_type bit_xor(native_type a,native_type b) noexcept { return _mm512_xor_si512(a,b); }
      static simd_inline simd_const native_type bit_not(native_type a) noexcept { return bit_xor(a,broadcast(true)); }
      static simd_inline simd_const std::uint64_t bits(native_type a) noexcept {
        if constexpr(B==1) return _mm512_cmpneq_epi8_mask(a,_mm512_setzero_si512());
        else if constexpr(B==2) return _mm512_cmpneq_epi16_mask(a,_mm512_setzero_si512());
        else if constexpr(B==4) return _mm512_cmpneq_epi32_mask(a,_mm512_setzero_si512());
        else return _mm512_cmpneq_epi64_mask(a,_mm512_setzero_si512());
      }
      static simd_inline simd_const native_type from_bits(std::uint64_t a) noexcept {
        if constexpr(B==1) return _mm512_maskz_set1_epi8(__mmask64(a),-1);
        else if constexpr(B==2) return _mm512_maskz_set1_epi16(__mmask32(a),-1);
        else if constexpr(B==4) return _mm512_maskz_set1_epi32(__mmask16(a),-1);
        else return _mm512_maskz_set1_epi64(__mmask8(a),-1);
      }
      static simd_inline simd_const native_type normalize(native_type a) noexcept { return from_bits(bits(a)); }
      static simd_inline simd_const bool any(native_type a) noexcept { return bits(a)!=0; }
      static simd_inline simd_const bool all(native_type a) noexcept { return bits(a)==mask_low_bits<N>; }
#endif
    };
    template<std::size_t B,std::size_t N> using mask_full_ops=std::conditional_t<N==1,mask_scalar_ops<mask_word<B>>,
      std::conditional_t<B*N==64,mask_vector512_ops<B,N>,mask_vector_ops<B,N>>>;
    template<std::size_t N> inline constexpr bool predicate_shape=
#if SIMD_HAS_AVX512F
      (N==1 || N==2 || N==3 || N==4 || N==8 || N==16
#if SIMD_HAS_AVX512BW
       || N==32 || N==64
#endif
      );
#else
      false;
#endif
  }

  template<class U,std::size_t N> requires SIMD_BACKEND_NAMESPACE::mask_shape<sizeof(U),N>
  /// \ingroup masks
  /// A full-vector mask whose lanes are canonical zero/all-one values.
  /// `from_native` normalizes whole lanes; `unsafe_from_native` requires that
  /// invariant already. `to_bitset` places logical lane i in bit i.
  struct alignas(typename SIMD_BACKEND_NAMESPACE::mask_full_ops<sizeof(U),N>::native_type) vec<mask_lane<U>,N,SIMD_ARCH> : detail::swizzle_access<mask_lane<U>,N,SIMD_ARCH> {
    using architecture = SIMD_ARCH;
    /// Select this architecture and forward arguments to the corresponding constructor.
    /// Exception behavior is exactly that of the forwarded construction.
    template<class... X> requires std::constructible_from<vec,X...>
    simd_inline constexpr vec(SIMD_ARCH, X &&... x)
        noexcept(std::is_nothrow_constructible_v<vec,X...>) : vec(std::forward<X>(x)...) {}
    template <class X> using rebind = vec<X,N,SIMD_ARCH>;
    /// Load exactly the logical lanes; the template alignment is a caller promise, never permission to read padding.
    template <std::size_t A = 1>
    simd_nodiscard static simd_inline vec load_memory(mask_lane<U> const * p) noexcept { return load(p); }
    /// Store exactly the logical lanes; the template alignment is a caller promise, never permission to write padding.
    template <std::size_t A = 1>
    simd_inline void store_memory(mask_lane<U> * p) const noexcept { store(p); }

    using value_type=mask_lane<U>;
    using storage_type=U;
    using ops=SIMD_BACKEND_NAMESPACE::mask_full_ops<sizeof(U),N>;
    using native_type=typename ops::native_type;
    using mask_type=vec;
    using mask = mask_type;
    using predicate_type = predicate<N,SIMD_ARCH>;
    static constexpr std::size_t lanes=N;
    static constexpr bool compact=false;
    /// Initialize every logical lane to false.
    simd_inline constexpr vec() noexcept : value_(ops::broadcast(false)) {}
    /// Broadcast the supplied truth value to every logical lane.
    explicit simd_inline constexpr vec(bool value) noexcept : value_(ops::broadcast(value)) {}
    /// Broadcast the supplied truth value to every logical lane.
    simd_inline constexpr vec(value_type value) noexcept : vec(value.to_bool()) {}
    /// Construct logical lanes in argument order. Any element conversions determine the exception specification.
    template<class... X> requires(N>1 && sizeof...(X)==N) && (std::same_as<X,value_type>&&...)
    simd_inline vec(X... values) noexcept : vec(std::array<value_type,N>{values...}) {}
    /// Read all logical lanes from an unaligned element pointer.
    explicit simd_inline vec(std::array<value_type,N> const & values) noexcept : vec(load(values.data())) {}
    // Safe native import interprets each whole U-sized lane as nonzero truth.
    /// Import native mask storage, normalizing nonzero lanes to true.
    simd_nodiscard static simd_inline simd_const constexpr vec from_native(native_type value) noexcept { return vec(raw{},ops::normalize(value)); }
    // Caller promises zero/all-ones for EVERY lane. Arbitrary bitselect masks
    // must use bit_select instead, never this canonical predicate domain.
    /// Adopt storage with the precondition that every mask lane is canonical zero or all ones.
    simd_nodiscard static simd_inline simd_const constexpr vec unsafe_from_native(native_type value) noexcept { return vec(raw{},value); }
    /// Return the native storage representation.
    simd_nodiscard simd_inline simd_pure constexpr native_type to_native() const noexcept { return value_; }
    /// Import lane i from bit i, clearing bits above the logical lane count.
    simd_nodiscard static simd_inline simd_const constexpr vec from_bitset(std::uint64_t value) noexcept { return vec(raw{},ops::from_bits(value)); }
    /// Pack each logical lane truth value into bit i; higher bits are zero.
    simd_nodiscard simd_inline simd_pure constexpr std::uint64_t to_bitset() const noexcept { return ops::bits(value_); }
    /// Read all logical lanes from an unaligned element pointer.
    simd_nodiscard static simd_inline simd_pure vec load(value_type const * p) noexcept {
      native_type value;std::memcpy(&value,static_cast<void const *>(p),sizeof(value));return unsafe_from_native(value);
    }
    /// Write all logical lanes to an unaligned element pointer.
    simd_inline void store(value_type * p) const noexcept { std::memcpy(static_cast<void *>(p),&value_,sizeof(value_)); }
    /// Invert each lane truth value, preserving the mask representation.
    simd_nodiscard friend simd_inline simd_const constexpr vec operator~(vec a) noexcept { return vec(raw{},ops::bit_not(a.value_)); }
    /// Return the lane-wise logical complement, retaining this mask type.
    simd_nodiscard friend simd_inline simd_const constexpr vec operator!(vec a) noexcept { return ~a; }
    /// Bitwise AND of corresponding lane representations.
    simd_nodiscard friend simd_inline simd_const constexpr vec operator&(vec a,vec b) noexcept { return vec(raw{},ops::bit_and(a.value_,b.value_)); }
    /// Bitwise OR of corresponding lane representations.
    simd_nodiscard friend simd_inline simd_const constexpr vec operator|(vec a,vec b) noexcept { return vec(raw{},ops::bit_or(a.value_,b.value_)); }
    /// Bitwise XOR of corresponding lane representations.
    simd_nodiscard friend simd_inline simd_const constexpr vec operator^(vec a,vec b) noexcept { return vec(raw{},ops::bit_xor(a.value_,b.value_)); }
    /// Return a mask whose lanes are true where `a == b` holds.
    simd_nodiscard friend simd_inline simd_const constexpr vec operator==(vec a,vec b) noexcept { return ~(a^b); }
    /// Return a mask whose lanes are true where `a != b` holds.
    simd_nodiscard friend simd_inline simd_const constexpr vec operator!=(vec a,vec b) noexcept { return a^b; }
    /// Apply the corresponding lane-wise AND operation in place and return *this.
    simd_inline constexpr vec & operator&=(vec b) noexcept { return *this=*this&b; }
    /// Apply the corresponding lane-wise OR operation in place and return *this.
    simd_inline constexpr vec & operator|=(vec b) noexcept { return *this=*this|b; }
    /// Apply the corresponding lane-wise XOR operation in place and return *this.
    simd_inline constexpr vec & operator^=(vec b) noexcept { return *this=*this^b; }
    /// Return whether at least one logical lane is true.
    simd_nodiscard friend simd_inline simd_const constexpr bool any(vec a) noexcept { return ops::any(a.value_); }
    /// Return whether every logical lane is true.
    simd_nodiscard friend simd_inline simd_const constexpr bool all(vec a) noexcept { return ops::all(a.value_); }
    /// Return true exactly when no logical lane is true.
    simd_nodiscard friend simd_inline simd_const constexpr bool none(vec a) noexcept { return !any(a); }
    /// Choose a in true lanes and b in false lanes; both values are already evaluated.
    simd_nodiscard friend simd_inline simd_const constexpr vec select(vec predicate,vec a,vec b) noexcept { return (predicate&a)|(~predicate&b); }
  private:
    struct raw {};
    simd_inline constexpr vec(raw,native_type value) noexcept : value_(value) {}
    native_type value_;
  };

  /// \ingroup masks
  /// Compact comparison storage. Every import clears bits above the lane count,
  /// including the compatibility `unsafe_from_native` spelling.

  template<std::size_t N> requires SIMD_BACKEND_NAMESPACE::predicate_shape<N>
  struct predicate<N,SIMD_ARCH> {
    using ops=SIMD_BACKEND_NAMESPACE::mask_compact_ops<N>;
    using native_type=typename ops::native_type;
    static constexpr std::size_t lanes=N;
    static constexpr bool compact=true;
    /// Initialize every logical lane to false.
    simd_inline constexpr predicate() noexcept = default;
    /// Broadcast the supplied truth value to every logical lane.
    explicit simd_inline constexpr predicate(bool value) noexcept : value_(ops::broadcast(value)) {}
    /// Import compact mask bits and clear bits above the lane count.
    simd_nodiscard static simd_inline simd_const constexpr predicate from_native(native_type value) noexcept { return predicate(raw{},ops::normalize(value)); }
    /// Import compact bits and clear bits above the lane count, just like from_native.
    simd_nodiscard static simd_inline simd_const constexpr predicate unsafe_from_native(native_type value) noexcept { return from_native(value); }
    /// Return the native storage representation.
    simd_nodiscard simd_inline simd_pure constexpr native_type to_native() const noexcept { return value_; }
    /// Import lane truth from the low logical-lane bits.
    simd_nodiscard static simd_inline simd_const constexpr predicate from_bitset(std::uint64_t value) noexcept { return from_native(native_type(value)); }
    /// Pack lane truth into low bits, with lane zero in bit zero.
    simd_nodiscard simd_inline simd_pure constexpr std::uint64_t to_bitset() const noexcept { return value_; }
    /// Invert each lane truth value, preserving the mask representation.
    simd_nodiscard friend simd_inline simd_const constexpr predicate operator~(predicate a) noexcept { return predicate(raw{},ops::bit_not(a.value_)); }
    /// Return the lane-wise logical complement, retaining this mask type.
    simd_nodiscard friend simd_inline simd_const constexpr predicate operator!(predicate a) noexcept { return ~a; }
    /// Bitwise AND of corresponding lane representations.
    simd_nodiscard friend simd_inline simd_const constexpr predicate operator&(predicate a,predicate b) noexcept { return predicate(raw{},ops::bit_and(a.value_,b.value_)); }
    /// Bitwise OR of corresponding lane representations.
    simd_nodiscard friend simd_inline simd_const constexpr predicate operator|(predicate a,predicate b) noexcept { return predicate(raw{},ops::bit_or(a.value_,b.value_)); }
    /// Bitwise XOR of corresponding lane representations.
    simd_nodiscard friend simd_inline simd_const constexpr predicate operator^(predicate a,predicate b) noexcept { return predicate(raw{},ops::bit_xor(a.value_,b.value_)); }
    /// Return a mask whose lanes are true where `a == b` holds.
    simd_nodiscard friend simd_inline simd_const constexpr predicate operator==(predicate a,predicate b) noexcept { return ~(a^b); }
    /// Return a mask whose lanes are true where `a != b` holds.
    simd_nodiscard friend simd_inline simd_const constexpr predicate operator!=(predicate a,predicate b) noexcept { return a^b; }
    /// Apply the corresponding lane-wise AND operation in place and return *this.
    simd_inline constexpr predicate & operator&=(predicate b) noexcept { return *this=*this&b; }
    /// Apply the corresponding lane-wise OR operation in place and return *this.
    simd_inline constexpr predicate & operator|=(predicate b) noexcept { return *this=*this|b; }
    /// Apply the corresponding lane-wise XOR operation in place and return *this.
    simd_inline constexpr predicate & operator^=(predicate b) noexcept { return *this=*this^b; }
    /// Return whether at least one logical lane is true.
    simd_nodiscard friend simd_inline simd_const constexpr bool any(predicate a) noexcept { return a.value_!=0; }
    /// Return whether every logical lane is true.
    simd_nodiscard friend simd_inline simd_const constexpr bool all(predicate a) noexcept { return ops::all(a.value_); }
    /// Return true exactly when no logical lane is true.
    simd_nodiscard friend simd_inline simd_const constexpr bool none(predicate a) noexcept { return !any(a); }
    /// Choose a in true lanes and b in false lanes; both values are already evaluated.
    simd_nodiscard friend simd_inline simd_const constexpr predicate select(predicate p,predicate a,predicate b) noexcept { return (p&a)|(~p&b); }
  private:
    struct raw {};
    simd_inline constexpr predicate(raw,native_type value) noexcept : value_(value) {}
    native_type value_=0;
  };

  namespace detail::SIMD_BACKEND {
    template<class T> using mask_lane_for=mask_lane<mask_word<mask_lane_bytes<T>>>;
    template<class T,std::size_t N> using comparison_mask=std::conditional_t<mask_compact<mask_lane_bytes<T>,N>,
      predicate<N,SIMD_ARCH>,vec<mask_lane_for<T>,N,SIMD_ARCH>>;
  }
  namespace detail::SIMD_BACKEND {
  template<class T,std::size_t N,class U,std::size_t A=1,simd_access Access=simd_access::ordinary>
    requires simd_mask_element<T> && std::same_as<T,U> && requires { typename vec<T,N,SIMD_ARCH>::native_type; }
  simd_nodiscard simd_inline simd_pure vec<T,N,SIMD_ARCH> load_simd(U const * p,simd_memory<A,Access> = {}) noexcept { return vec<T,N,SIMD_ARCH>::load(p); }
  template<class U,class T,std::size_t N,std::size_t A=1,simd_access Access=simd_access::ordinary>
    requires simd_mask_element<T> && std::same_as<T,U> && requires { typename vec<T,N,SIMD_ARCH>::native_type; }
  simd_inline void store_simd(U * p,vec<T,N,SIMD_ARCH> value,simd_memory<A,Access> = {}) noexcept { value.store(p); }
  template<class T,std::size_t N,class U,std::size_t A=1,simd_access Access=simd_access::ordinary>
    requires simd_mask_element<T> && std::same_as<T,U> && requires { typename vec<T,N,SIMD_ARCH>::native_type; }
  simd_nodiscard simd_inline simd_pure vec<T,N,SIMD_ARCH> load_simd_partial(U const * p,std::size_t count,T fill=T{},simd_memory<A,Access> = {}) noexcept {
    std::array<T,N> a;a.fill(fill);for(std::size_t i=0;i<count;++i)a[i]=p[i];return vec<T,N,SIMD_ARCH>::load(a.data());
  }
  template<class U,class T,std::size_t N,std::size_t A=1,simd_access Access=simd_access::ordinary>
    requires simd_mask_element<T> && std::same_as<T,U> && requires { typename vec<T,N,SIMD_ARCH>::native_type; }
  simd_inline void store_simd_partial(U * p,vec<T,N,SIMD_ARCH> value,std::size_t count,simd_memory<A,Access> = {}) noexcept {
    std::array<T,N> a;value.store(a.data());for(std::size_t i=0;i<count;++i)p[i]=a[i];
  }
  }
  /// \ingroup masks
  /// Change full-mask lane width without changing lane truth or lane count.
  template<class U,class T,std::size_t N> requires simd_mask_element<U> && simd_mask_element<T> &&
    requires { typename vec<U,N,SIMD_ARCH>::native_type; typename vec<T,N,SIMD_ARCH>::native_type; }
  simd_nodiscard simd_inline simd_const vec<U,N,SIMD_ARCH> mask_cast(vec<T,N,SIMD_ARCH> value) noexcept {
    if constexpr(sizeof(U)==sizeof(T)) return vec<U,N,SIMD_ARCH>::unsafe_from_native(value.to_native());
    else return vec<U,N,SIMD_ARCH>::from_bitset(value.to_bitset());
  }
  /// \ingroup masks
  /// Compress full-vector truth into a supported compact predicate.
  template<class T,std::size_t N> requires simd_mask_element<T> && SIMD_BACKEND_NAMESPACE::predicate_shape<N> && requires { typename vec<T,N,SIMD_ARCH>::native_type; }
  simd_nodiscard simd_inline simd_const predicate<N,SIMD_ARCH> to_predicate(vec<T,N,SIMD_ARCH> value) noexcept {
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512VL
    if constexpr(sizeof(T)*N==16 && SIMD_BACKEND_NAMESPACE::mask_compact<sizeof(T),N>) {
      auto x=value.to_native();auto z=_mm_setzero_si128();
      if constexpr(sizeof(T)==1) return predicate<N,SIMD_ARCH>::from_native(_mm_cmpneq_epi8_mask(x,z));
      else if constexpr(sizeof(T)==2) return predicate<N,SIMD_ARCH>::from_native(_mm_cmpneq_epi16_mask(x,z));
      else if constexpr(sizeof(T)==4) return predicate<N,SIMD_ARCH>::from_native(_mm_cmpneq_epi32_mask(x,z));
      else return predicate<N,SIMD_ARCH>::from_native(_mm_cmpneq_epi64_mask(x,z));
    } else if constexpr(sizeof(T)*N==32 && SIMD_BACKEND_NAMESPACE::mask_compact<sizeof(T),N>) {
      auto x=value.to_native();auto z=_mm256_setzero_si256();
      if constexpr(sizeof(T)==1) return predicate<N,SIMD_ARCH>::from_native(_mm256_cmpneq_epi8_mask(x,z));
      else if constexpr(sizeof(T)==2) return predicate<N,SIMD_ARCH>::from_native(_mm256_cmpneq_epi16_mask(x,z));
      else if constexpr(sizeof(T)==4) return predicate<N,SIMD_ARCH>::from_native(_mm256_cmpneq_epi32_mask(x,z));
      else return predicate<N,SIMD_ARCH>::from_native(_mm256_cmpneq_epi64_mask(x,z));
    } else
#endif
    return predicate<N,SIMD_ARCH>::from_bitset(value.to_bitset());
  }
  /// \ingroup masks
  /// Expand a predicate to canonical zero/all-one lanes of mask element `T`.
  template<class T,std::size_t N> requires simd_mask_element<T> && SIMD_BACKEND_NAMESPACE::predicate_shape<N> && requires { typename vec<T,N,SIMD_ARCH>::native_type; }
  simd_nodiscard simd_inline simd_const vec<T,N,SIMD_ARCH> to_vector_mask(predicate<N,SIMD_ARCH> value) noexcept {
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512VL
    if constexpr(sizeof(T)*N==16 && SIMD_BACKEND_NAMESPACE::mask_compact<sizeof(T),N>) {
      auto k=value.to_native();
      if constexpr(sizeof(T)==1) return vec<T,N,SIMD_ARCH>::unsafe_from_native(_mm_maskz_set1_epi8(k,-1));
      else if constexpr(sizeof(T)==2) return vec<T,N,SIMD_ARCH>::unsafe_from_native(_mm_maskz_set1_epi16(k,-1));
      else if constexpr(sizeof(T)==4) return vec<T,N,SIMD_ARCH>::unsafe_from_native(_mm_maskz_set1_epi32(k,-1));
      else return vec<T,N,SIMD_ARCH>::unsafe_from_native(_mm_maskz_set1_epi64(k,-1));
    } else if constexpr(sizeof(T)*N==32 && SIMD_BACKEND_NAMESPACE::mask_compact<sizeof(T),N>) {
      auto k=value.to_native();
      if constexpr(sizeof(T)==1) return vec<T,N,SIMD_ARCH>::unsafe_from_native(_mm256_maskz_set1_epi8(k,-1));
      else if constexpr(sizeof(T)==2) return vec<T,N,SIMD_ARCH>::unsafe_from_native(_mm256_maskz_set1_epi16(k,-1));
      else if constexpr(sizeof(T)==4) return vec<T,N,SIMD_ARCH>::unsafe_from_native(_mm256_maskz_set1_epi32(k,-1));
      else return vec<T,N,SIMD_ARCH>::unsafe_from_native(_mm256_maskz_set1_epi64(k,-1));
    } else
#endif
    return vec<T,N,SIMD_ARCH>::from_bitset(value.to_bitset());
  }
}


namespace simd {
  namespace detail::SIMD_BACKEND {
    template<std::size_t N> simd_nodiscard simd_inline simd_const constexpr auto bool_ones() noexcept {
      if constexpr(N==1) return std::uint8_t(1);
#if SIMD_HAS_AVX2
      else if constexpr(N==16) return _mm_set1_epi8(1);
      else if constexpr(N==32) return _mm256_set1_epi8(1);
#endif
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512BW
      else if constexpr(N==64) return _mm512_set1_epi8(1);
#endif
#if SIMD_HAS_ARM_NEON
      else return vdupq_n_u8(1);
#endif
    }
  }
  /// \ingroup masks
  /// Boolean data stores one byte, zero or one, per lane.
  /// Use explicit conversions to cross into full masks or compact predicates.
  template<std::size_t N> requires SIMD_BACKEND_NAMESPACE::mask_shape<1,N>
  struct vec<bool,N,SIMD_ARCH> : detail::swizzle_access<bool,N,SIMD_ARCH> {
    using architecture = SIMD_ARCH;
    /// Select this architecture and forward arguments to the corresponding constructor.
    /// Exception behavior is exactly that of the forwarded construction.
    template<class... X> requires std::constructible_from<vec,X...>
    simd_inline constexpr vec(SIMD_ARCH, X &&... x)
        noexcept(std::is_nothrow_constructible_v<vec,X...>) : vec(std::forward<X>(x)...) {}
    template <class X> using rebind = vec<X,N,SIMD_ARCH>;
    /// Load exactly the logical lanes; the template alignment is a caller promise, never permission to read padding.
    template <std::size_t A = 1>
    simd_nodiscard static simd_inline vec load_memory(bool const * p) noexcept { return load(p); }
    /// Store exactly the logical lanes; the template alignment is a caller promise, never permission to write padding.
    template <std::size_t A = 1>
    simd_inline void store_memory(bool * p) const noexcept { store(p); }

    using value_type=bool;
    using storage_type=std::uint8_t;
    using ops=SIMD_BACKEND_NAMESPACE::mask_full_ops<1,N>;
    using native_type=typename ops::native_type;
    using vector_mask_type=vec<mask8,N,SIMD_ARCH>;
    using mask_type=SIMD_BACKEND_NAMESPACE::comparison_mask<bool,N>;
    using mask = mask_type;
    using predicate_type = predicate<N,SIMD_ARCH>;
    static constexpr std::size_t lanes=N;
    /// Initialize every logical lane to false.
    simd_inline constexpr vec() noexcept : value_(ops::broadcast(false)) {}
    /// Broadcast the supplied truth value to every logical lane.
    explicit simd_inline constexpr vec(bool value) noexcept : value_(value?SIMD_BACKEND_NAMESPACE::bool_ones<N>():ops::broadcast(false)) {}
    /// Construct logical lanes in argument order. Any element conversions determine the exception specification.
    template<class... X> requires(N>1 && sizeof...(X)==N) && (std::same_as<X,bool>&&...)
    simd_inline vec(X... value) noexcept : vec(std::array<bool,N>{value...}) {}
    /// Read all logical lanes from an unaligned element pointer.
    explicit simd_inline vec(std::array<bool,N> const & value) noexcept : vec(load(value.data())) {}
    /// Import native byte lanes, converting each nonzero byte to Boolean one.
    simd_nodiscard static simd_inline simd_const constexpr vec from_native(native_type value) noexcept {
      return vec(raw{},ops::bit_and(ops::normalize(value),SIMD_BACKEND_NAMESPACE::bool_ones<N>()));
    }
    // Caller promises EVERY byte is 0 or 1, never an all-ones mask byte.
    /// Adopt storage with the precondition that every Boolean byte is zero or one.
    simd_nodiscard static simd_inline simd_const constexpr vec unsafe_from_native(native_type value) noexcept { return vec(raw{},value); }
    /// Return the native storage representation.
    simd_nodiscard simd_inline simd_pure constexpr native_type to_native() const noexcept { return value_; }
    /// Read all logical lanes from an unaligned element pointer.
    simd_nodiscard static simd_inline simd_pure vec load(bool const * p) noexcept {
      std::array<std::uint8_t,N> bytes{};
      for(std::size_t i=0;i<N;++i) bytes[i]=p[i]?1:0;
      native_type value;std::memcpy(&value,bytes.data(),sizeof(value));return unsafe_from_native(value);
    }
    /// Write all logical lanes to an unaligned element pointer.
    simd_inline void store(bool * p) const noexcept {
      std::array<std::uint8_t,N> bytes{};std::memcpy(bytes.data(),&value_,sizeof(value_));
      for(std::size_t i=0;i<N;++i) p[i]=bytes[i]!=0;
    }
    // The caller supplies 0 <= count <= N. Zero touches no pointer, even null.
    /// Read exactly n logical lanes and fill the remainder; require n <= lanes. For n == 0, p may be null.
    simd_nodiscard static simd_inline simd_pure vec load_partial(bool const * p,std::size_t count,bool fill=false) noexcept {
      std::array<bool,N> values;values.fill(fill);
      for(std::size_t i=0;i<count;++i) values[i]=p[i];
      return load(values.data());
    }
    /// Write exactly n logical lanes; require n <= lanes. For n == 0, p may be null.
    simd_inline void store_partial(bool * p,std::size_t count) const noexcept {
      std::array<bool,N> values;store(values.data());
      for(std::size_t i=0;i<count;++i) p[i]=values[i];
    }
    /// Return the lane-wise logical complement, retaining this mask type.
    simd_nodiscard friend simd_inline simd_const constexpr vec operator!(vec a) noexcept { return vec(raw{},ops::bit_xor(a.value_,SIMD_BACKEND_NAMESPACE::bool_ones<N>())); }
    /// Invert each lane truth value, preserving the mask representation.
    simd_nodiscard friend simd_inline simd_const constexpr vec operator~(vec a) noexcept { return !a; }
    /// Bitwise AND of corresponding lane representations.
    simd_nodiscard friend simd_inline simd_const constexpr vec operator&(vec a,vec b) noexcept { return vec(raw{},ops::bit_and(a.value_,b.value_)); }
    /// Bitwise OR of corresponding lane representations.
    simd_nodiscard friend simd_inline simd_const constexpr vec operator|(vec a,vec b) noexcept { return vec(raw{},ops::bit_or(a.value_,b.value_)); }
    /// Bitwise XOR of corresponding lane representations.
    simd_nodiscard friend simd_inline simd_const constexpr vec operator^(vec a,vec b) noexcept { return vec(raw{},ops::bit_xor(a.value_,b.value_)); }
    /// Return a mask whose lanes are true where `a != b` holds.
    simd_nodiscard friend simd_inline simd_const constexpr mask_type operator!=(vec a,vec b) noexcept {
      auto x=ops::bit_xor(a.value_,b.value_);
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512BW
      if constexpr(N==64) return mask_type::from_native(_mm512_cmpneq_epi8_mask(x,_mm512_setzero_si512()));
#if SIMD_HAS_AVX512VL
      else if constexpr(N==32) return mask_type::from_native(_mm256_cmpneq_epi8_mask(x,_mm256_setzero_si256()));
      else if constexpr(N==16) return mask_type::from_native(_mm_cmpneq_epi8_mask(x,_mm_setzero_si128()));
#endif
      else
#endif
      return vector_mask_type::from_native(x);
    }
    /// Return a mask whose lanes are true where `a == b` holds.
    simd_nodiscard friend simd_inline simd_const constexpr mask_type operator==(vec a,vec b) noexcept { return ~(a!=b); }
    /// Apply the corresponding lane-wise AND operation in place and return *this.
    simd_inline constexpr vec & operator&=(vec b) noexcept { return *this=*this&b; }
    /// Apply the corresponding lane-wise OR operation in place and return *this.
    simd_inline constexpr vec & operator|=(vec b) noexcept { return *this=*this|b; }
    /// Apply the corresponding lane-wise XOR operation in place and return *this.
    simd_inline constexpr vec & operator^=(vec b) noexcept { return *this=*this^b; }
    /// Return whether at least one logical lane is true.
    simd_nodiscard friend simd_inline simd_const constexpr bool any(vec a) noexcept { return ops::any(a.value_); }
    /// Return whether every logical lane is true.
    simd_nodiscard friend simd_inline simd_const constexpr bool all(vec a) noexcept { return none(!a); }
    /// Return true exactly when no logical lane is true.
    simd_nodiscard friend simd_inline simd_const constexpr bool none(vec a) noexcept { return !any(a); }
    /// Choose a in true lanes and b in false lanes; both values are already evaluated.
    template<class M> requires(std::same_as<M,mask_type> || std::same_as<M,vector_mask_type>)
    simd_nodiscard friend simd_inline simd_const constexpr vec select(M m,vec a,vec b) noexcept {
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512BW
      if constexpr(M::compact) {
        if constexpr(N==64) return vec(raw{},_mm512_mask_blend_epi8(m.to_native(),b.value_,a.value_));
#if SIMD_HAS_AVX512VL
        else if constexpr(N==32) return vec(raw{},_mm256_mask_blend_epi8(m.to_native(),b.value_,a.value_));
        else return vec(raw{},_mm_mask_blend_epi8(m.to_native(),b.value_,a.value_));
#endif
      } else
#endif
      return vec(raw{},ops::bit_or(ops::bit_and(m.to_native(),a.value_),ops::bit_and(ops::bit_not(m.to_native()),b.value_)));
    }
  private:
    struct raw {};
    simd_inline constexpr vec(raw,native_type value) noexcept : value_(value) {}
    native_type value_;
  };

  namespace detail::SIMD_BACKEND {
  template<class T,std::size_t N,class U,std::size_t A=1,simd_access Access=simd_access::ordinary>
    requires std::same_as<T,bool> && std::same_as<U,bool> && requires { typename vec<T,N,SIMD_ARCH>::native_type; }
  simd_nodiscard simd_inline simd_pure vec<T,N,SIMD_ARCH> load_simd(U const * p,simd_memory<A,Access> = {}) noexcept { return vec<T,N,SIMD_ARCH>::load(p); }
  template<class U,class T,std::size_t N,std::size_t A=1,simd_access Access=simd_access::ordinary>
    requires std::same_as<T,bool> && std::same_as<U,bool> && requires { typename vec<T,N,SIMD_ARCH>::native_type; }
  simd_inline void store_simd(U * p,vec<T,N,SIMD_ARCH> value,simd_memory<A,Access> = {}) noexcept { value.store(p); }
  template<class T,std::size_t N,class U,std::size_t A=1,simd_access Access=simd_access::ordinary>
    requires std::same_as<T,bool> && std::same_as<U,bool> && requires { typename vec<T,N,SIMD_ARCH>::native_type; }
  simd_nodiscard simd_inline simd_pure vec<T,N,SIMD_ARCH> load_simd_partial(U const * p,std::size_t count,T fill=false,simd_memory<A,Access> = {}) noexcept {
    return vec<T,N,SIMD_ARCH>::load_partial(p,count,fill);
  }
  template<class U,class T,std::size_t N,std::size_t A=1,simd_access Access=simd_access::ordinary>
    requires std::same_as<T,bool> && std::same_as<U,bool> && requires { typename vec<T,N,SIMD_ARCH>::native_type; }
  simd_inline void store_simd_partial(U * p,vec<T,N,SIMD_ARCH> value,std::size_t count,simd_memory<A,Access> = {}) noexcept { value.store_partial(p,count); }
  }
  /// Expand lane truth into canonical zero/all-one full-vector mask lanes.
  template<std::size_t N> requires requires { typename vec<bool,N,SIMD_ARCH>::native_type; }
  simd_nodiscard simd_inline simd_const vec<mask8,N,SIMD_ARCH> to_vector_mask(vec<bool,N,SIMD_ARCH> value) noexcept {
    return vec<mask8,N,SIMD_ARCH>::from_native(value.to_native());
  }
  /// Convert lane truth into Boolean data lanes represented as zero or one, preserving the lane count.
  template<class T,std::size_t N> requires simd_mask_element<T> && requires { typename vec<bool,N,SIMD_ARCH>::native_type; typename vec<T,N,SIMD_ARCH>::native_type; }
  simd_nodiscard simd_inline simd_const vec<bool,N,SIMD_ARCH> to_bool(vec<T,N,SIMD_ARCH> value) noexcept {
    auto byte_mask=mask_cast<mask8>(value);
    return vec<bool,N,SIMD_ARCH>::unsafe_from_native(vec<bool,N,SIMD_ARCH>::ops::bit_and(byte_mask.to_native(),SIMD_BACKEND_NAMESPACE::bool_ones<N>()));
  }
  /// Convert lane truth into Boolean data lanes represented as zero or one, preserving the lane count.
  template<std::size_t N> requires SIMD_BACKEND_NAMESPACE::predicate_shape<N> && requires { typename vec<bool,N,SIMD_ARCH>::native_type; }
  simd_nodiscard simd_inline simd_const vec<bool,N,SIMD_ARCH> to_bool(predicate<N,SIMD_ARCH> value) noexcept { return to_bool(to_vector_mask<mask8>(value)); }
  /// Compress lane truth into the supported compact predicate representation.
  template<std::size_t N> requires SIMD_BACKEND_NAMESPACE::predicate_shape<N> && requires { typename vec<bool,N,SIMD_ARCH>::native_type; }
  simd_nodiscard simd_inline simd_const predicate<N,SIMD_ARCH> to_predicate(vec<bool,N,SIMD_ARCH> value) noexcept {
    auto result=value!=vec<bool,N,SIMD_ARCH>(false);
    if constexpr(decltype(result)::compact) return result;
    else return to_predicate(result);
  }
}

#include <bit>

namespace SIMD_BACKEND_NAMESPACE {
  template <simd_integer_element T> simd_nodiscard simd_inline constexpr auto integer_word(T value) noexcept {
    return std::bit_cast<std::make_unsigned_t<T>>(value);
  }
  template <simd_integer_element T, class U>
  simd_nodiscard simd_inline constexpr T integer_wrap(U value) noexcept {
    return std::bit_cast<T>(static_cast<std::make_unsigned_t<T>>(value));
  }
  template <simd_integer_element T>
  using integer_work_word = std::conditional_t<(sizeof(T) < 4), uint32_t, std::make_unsigned_t<T>>;
  template <simd_integer_element T>
  simd_nodiscard simd_inline constexpr T integer_scalar_add(T a, T b) noexcept {
    using U = integer_work_word<T>;
    return integer_wrap<T>(U(integer_word(a)) + U(integer_word(b)));
  }
  template <simd_integer_element T>
  simd_nodiscard simd_inline constexpr T integer_scalar_sub(T a, T b) noexcept {
    using U = integer_work_word<T>;
    return integer_wrap<T>(U(integer_word(a)) - U(integer_word(b)));
  }
  template <simd_integer_element T>
  simd_nodiscard simd_inline constexpr T integer_scalar_mul(T a, T b) noexcept {
    using U = integer_work_word<T>;
    return integer_wrap<T>(U(integer_word(a)) * U(integer_word(b)));
  }

#if SIMD_HAS_AVX2
  template <simd_integer_element T>
  simd_nodiscard simd_inline simd_const __m128i integer_broadcast_16(T x) noexcept {
    if constexpr (sizeof(T) == 1)
      return _mm_set1_epi8(std::bit_cast<int8_t>(x));
    else if constexpr (sizeof(T) == 2)
      return _mm_set1_epi16(std::bit_cast<int16_t>(x));
    else if constexpr (sizeof(T) == 4)
      return _mm_set1_epi32(std::bit_cast<int32_t>(x));
    else if constexpr (sizeof(T) == 8)
      return _mm_set1_epi64x(std::bit_cast<int64_t>(x));
  }
  template <simd_integer_element T>
  simd_nodiscard simd_inline simd_const __m128i integer_add(__m128i a, __m128i b) noexcept {
    if constexpr (sizeof(T) == 1)
      return _mm_add_epi8(a, b);
    else if constexpr (sizeof(T) == 2)
      return _mm_add_epi16(a, b);
    else if constexpr (sizeof(T) == 4)
      return _mm_add_epi32(a, b);
    else if constexpr (sizeof(T) == 8)
      return _mm_add_epi64(a, b);
  }
  template <simd_integer_element T>
  simd_nodiscard simd_inline simd_const __m128i integer_sub(__m128i a, __m128i b) noexcept {
    if constexpr (sizeof(T) == 1)
      return _mm_sub_epi8(a, b);
    else if constexpr (sizeof(T) == 2)
      return _mm_sub_epi16(a, b);
    else if constexpr (sizeof(T) == 4)
      return _mm_sub_epi32(a, b);
    else if constexpr (sizeof(T) == 8)
      return _mm_sub_epi64(a, b);
  }
  template <simd_integer_element T>
  simd_nodiscard simd_inline simd_const __m128i integer_mul(__m128i a, __m128i b) noexcept {
    if constexpr (sizeof(T) == 1) {
      // Independent low-byte products inside each word, with cross-byte bits removed.
      auto low = _mm_mullo_epi16(a, b);
      auto high = _mm_mullo_epi16(_mm_srli_epi16(a, 8), _mm_srli_epi16(b, 8));
      return _mm_or_si128(_mm_and_si128(low, _mm_set1_epi16(255)), _mm_slli_epi16(high, 8));
    } else if constexpr (sizeof(T) == 2)
      return _mm_mullo_epi16(a, b);
    if constexpr (sizeof(T) == 4)
      return _mm_mullo_epi32(a, b);
    else if constexpr (sizeof(T) == 8) {
#if SIMD_HAS_AVX512DQ && SIMD_HAS_AVX512VL
      return _mm_mullo_epi64(a, b);
#else
      // Low 64 bits: low32*low32 plus both cross products shifted by 32.
      auto cross =
          _mm_add_epi64(_mm_mul_epu32(a, _mm_srli_epi64(b, 32)), _mm_mul_epu32(_mm_srli_epi64(a, 32), b));
      return _mm_add_epi64(_mm_mul_epu32(a, b), _mm_slli_epi64(cross, 32));
#endif
    }
  }
  template <simd_integer_element T, unsigned S>
    requires(S < sizeof(T) * 8)
  simd_nodiscard simd_inline simd_const __m128i integer_left(__m128i a) noexcept {
    if constexpr (S == 0)
      return a;
    else {
      if constexpr (sizeof(T) == 1)
        return _mm_and_si128(_mm_slli_epi16(a, S),
                             _mm_set1_epi8(std::bit_cast<int8_t>(uint8_t((255u << S) & 255u))));
      else if constexpr (sizeof(T) == 2)
        return _mm_slli_epi16(a, S);
      if constexpr (sizeof(T) == 4)
        return _mm_slli_epi32(a, S);
      else if constexpr (sizeof(T) == 8)
        return _mm_slli_epi64(a, S);
    }
  }
  template <simd_integer_element T, unsigned S>
    requires(S < sizeof(T) * 8)
  simd_nodiscard simd_inline simd_const __m128i integer_right(__m128i a) noexcept {
    if constexpr (S == 0)
      return a;
    else {
      if constexpr (sizeof(T) == 1) {
        auto low = _mm_and_si128(_mm_srli_epi16(a, S), _mm_set1_epi8(int8_t(255u >> S)));
        if constexpr (std::is_unsigned_v<T>)
          return low;
        else {
          auto negative = _mm_cmpgt_epi8(_mm_setzero_si128(), a);
          return _mm_or_si128(
              low, _mm_and_si128(negative, _mm_set1_epi8(std::bit_cast<int8_t>(uint8_t(255u ^ (255u >> S))))));
        }
      } else if constexpr (sizeof(T) == 2) {
        if constexpr (std::is_signed_v<T>)
          return _mm_srai_epi16(a, S);
        else
          return _mm_srli_epi16(a, S);
      }
      if constexpr (sizeof(T) == 4) {
        if constexpr (std::is_signed_v<T>)
          return _mm_srai_epi32(a, S);
        else
          return _mm_srli_epi32(a, S);
      } else if constexpr (sizeof(T) == 8) {
        if constexpr (std::is_unsigned_v<T>)
          return _mm_srli_epi64(a, S);
        else {
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512VL
          return _mm_srai_epi64(a, S);
#else
          auto negative = _mm_cmpgt_epi64(_mm_setzero_si128(), a);
          return _mm_or_si128(
              _mm_srli_epi64(a, S),
              _mm_and_si128(negative, _mm_set1_epi64x(std::bit_cast<int64_t>(~uint64_t(0) << (64 - S)))));
#endif
        }
      }
    }
  }
#endif

#if SIMD_HAS_AVX2
  template <simd_integer_element T>
  simd_nodiscard simd_inline simd_const __m256i integer_broadcast_32(T x) noexcept {
    if constexpr (sizeof(T) == 1)
      return _mm256_set1_epi8(std::bit_cast<int8_t>(x));
    else if constexpr (sizeof(T) == 2)
      return _mm256_set1_epi16(std::bit_cast<int16_t>(x));
    else if constexpr (sizeof(T) == 4)
      return _mm256_set1_epi32(std::bit_cast<int32_t>(x));
    else if constexpr (sizeof(T) == 8)
      return _mm256_set1_epi64x(std::bit_cast<int64_t>(x));
  }
  template <simd_integer_element T>
  simd_nodiscard simd_inline simd_const __m256i integer_add(__m256i a, __m256i b) noexcept {
    if constexpr (sizeof(T) == 1)
      return _mm256_add_epi8(a, b);
    else if constexpr (sizeof(T) == 2)
      return _mm256_add_epi16(a, b);
    else if constexpr (sizeof(T) == 4)
      return _mm256_add_epi32(a, b);
    else if constexpr (sizeof(T) == 8)
      return _mm256_add_epi64(a, b);
  }
  template <simd_integer_element T>
  simd_nodiscard simd_inline simd_const __m256i integer_sub(__m256i a, __m256i b) noexcept {
    if constexpr (sizeof(T) == 1)
      return _mm256_sub_epi8(a, b);
    else if constexpr (sizeof(T) == 2)
      return _mm256_sub_epi16(a, b);
    else if constexpr (sizeof(T) == 4)
      return _mm256_sub_epi32(a, b);
    else if constexpr (sizeof(T) == 8)
      return _mm256_sub_epi64(a, b);
  }
  template <simd_integer_element T>
  simd_nodiscard simd_inline simd_const __m256i integer_mul(__m256i a, __m256i b) noexcept {
    if constexpr (sizeof(T) == 1) {
      // Independent low-byte products inside each word, with cross-byte bits removed.
      auto low = _mm256_mullo_epi16(a, b);
      auto high = _mm256_mullo_epi16(_mm256_srli_epi16(a, 8), _mm256_srli_epi16(b, 8));
      return _mm256_or_si256(_mm256_and_si256(low, _mm256_set1_epi16(255)), _mm256_slli_epi16(high, 8));
    } else if constexpr (sizeof(T) == 2)
      return _mm256_mullo_epi16(a, b);
    if constexpr (sizeof(T) == 4)
      return _mm256_mullo_epi32(a, b);
    else if constexpr (sizeof(T) == 8) {
#if SIMD_HAS_AVX512DQ && SIMD_HAS_AVX512VL
      return _mm256_mullo_epi64(a, b);
#else
      // Low 64 bits: low32*low32 plus both cross products shifted by 32.
      auto cross = _mm256_add_epi64(_mm256_mul_epu32(a, _mm256_srli_epi64(b, 32)),
                                    _mm256_mul_epu32(_mm256_srli_epi64(a, 32), b));
      return _mm256_add_epi64(_mm256_mul_epu32(a, b), _mm256_slli_epi64(cross, 32));
#endif
    }
  }
  template <simd_integer_element T, unsigned S>
    requires(S < sizeof(T) * 8)
  simd_nodiscard simd_inline simd_const __m256i integer_left(__m256i a) noexcept {
    if constexpr (S == 0)
      return a;
    else {
      if constexpr (sizeof(T) == 1)
        return _mm256_and_si256(_mm256_slli_epi16(a, S),
                                _mm256_set1_epi8(std::bit_cast<int8_t>(uint8_t((255u << S) & 255u))));
      else if constexpr (sizeof(T) == 2)
        return _mm256_slli_epi16(a, S);
      if constexpr (sizeof(T) == 4)
        return _mm256_slli_epi32(a, S);
      else if constexpr (sizeof(T) == 8)
        return _mm256_slli_epi64(a, S);
    }
  }
  template <simd_integer_element T, unsigned S>
    requires(S < sizeof(T) * 8)
  simd_nodiscard simd_inline simd_const __m256i integer_right(__m256i a) noexcept {
    if constexpr (S == 0)
      return a;
    else {
      if constexpr (sizeof(T) == 1) {
        auto low = _mm256_and_si256(_mm256_srli_epi16(a, S), _mm256_set1_epi8(int8_t(255u >> S)));
        if constexpr (std::is_unsigned_v<T>)
          return low;
        else {
          auto negative = _mm256_cmpgt_epi8(_mm256_setzero_si256(), a);
          return _mm256_or_si256(
              low,
              _mm256_and_si256(negative, _mm256_set1_epi8(std::bit_cast<int8_t>(uint8_t(255u ^ (255u >> S))))));
        }
      } else if constexpr (sizeof(T) == 2) {
        if constexpr (std::is_signed_v<T>)
          return _mm256_srai_epi16(a, S);
        else
          return _mm256_srli_epi16(a, S);
      }
      if constexpr (sizeof(T) == 4) {
        if constexpr (std::is_signed_v<T>)
          return _mm256_srai_epi32(a, S);
        else
          return _mm256_srli_epi32(a, S);
      } else if constexpr (sizeof(T) == 8) {
        if constexpr (std::is_unsigned_v<T>)
          return _mm256_srli_epi64(a, S);
        else {
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512VL
          return _mm256_srai_epi64(a, S);
#else
          auto negative = _mm256_cmpgt_epi64(_mm256_setzero_si256(), a);
          return _mm256_or_si256(
              _mm256_srli_epi64(a, S),
              _mm256_and_si256(negative, _mm256_set1_epi64x(std::bit_cast<int64_t>(~uint64_t(0) << (64 - S)))));
#endif
        }
      }
    }
  }
#endif

#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ
  template <simd_integer_element T>
  simd_nodiscard simd_inline simd_const __m512i integer_broadcast_64(T x) noexcept {
#if SIMD_HAS_AVX512BW
    if constexpr (sizeof(T) == 1)
      return _mm512_set1_epi8(std::bit_cast<int8_t>(x));
    else if constexpr (sizeof(T) == 2)
      return _mm512_set1_epi16(std::bit_cast<int16_t>(x));
#endif
    if constexpr (sizeof(T) == 4)
      return _mm512_set1_epi32(std::bit_cast<int32_t>(x));
    else if constexpr (sizeof(T) == 8)
      return _mm512_set1_epi64(std::bit_cast<int64_t>(x));
  }
  template <simd_integer_element T>
  simd_nodiscard simd_inline simd_const __m512i integer_add(__m512i a, __m512i b) noexcept {
#if SIMD_HAS_AVX512BW
    if constexpr (sizeof(T) == 1)
      return _mm512_add_epi8(a, b);
    else if constexpr (sizeof(T) == 2)
      return _mm512_add_epi16(a, b);
#endif
    if constexpr (sizeof(T) == 4)
      return _mm512_add_epi32(a, b);
    else if constexpr (sizeof(T) == 8)
      return _mm512_add_epi64(a, b);
  }
  template <simd_integer_element T>
  simd_nodiscard simd_inline simd_const __m512i integer_sub(__m512i a, __m512i b) noexcept {
#if SIMD_HAS_AVX512BW
    if constexpr (sizeof(T) == 1)
      return _mm512_sub_epi8(a, b);
    else if constexpr (sizeof(T) == 2)
      return _mm512_sub_epi16(a, b);
#endif
    if constexpr (sizeof(T) == 4)
      return _mm512_sub_epi32(a, b);
    else if constexpr (sizeof(T) == 8)
      return _mm512_sub_epi64(a, b);
  }
  template <simd_integer_element T>
  simd_nodiscard simd_inline simd_const __m512i integer_mul(__m512i a, __m512i b) noexcept {
#if SIMD_HAS_AVX512BW
    if constexpr (sizeof(T) == 1) {
      // Independent low-byte products inside each word, with cross-byte bits removed.
      auto low = _mm512_mullo_epi16(a, b);
      auto high = _mm512_mullo_epi16(_mm512_srli_epi16(a, 8), _mm512_srli_epi16(b, 8));
      return _mm512_or_si512(_mm512_and_si512(low, _mm512_set1_epi16(255)), _mm512_slli_epi16(high, 8));
    } else if constexpr (sizeof(T) == 2)
      return _mm512_mullo_epi16(a, b);
#endif
    if constexpr (sizeof(T) == 4)
      return _mm512_mullo_epi32(a, b);
    else if constexpr (sizeof(T) == 8) {
      return _mm512_mullo_epi64(a, b);
    }
  }
  template <simd_integer_element T, unsigned S>
    requires(S < sizeof(T) * 8)
  simd_nodiscard simd_inline simd_const __m512i integer_left(__m512i a) noexcept {
    if constexpr (S == 0)
      return a;
    else {
#if SIMD_HAS_AVX512BW
      if constexpr (sizeof(T) == 1)
        return _mm512_and_si512(_mm512_slli_epi16(a, S),
                                _mm512_set1_epi8(std::bit_cast<int8_t>(uint8_t((255u << S) & 255u))));
      else if constexpr (sizeof(T) == 2)
        return _mm512_slli_epi16(a, S);
#endif
      if constexpr (sizeof(T) == 4)
        return _mm512_slli_epi32(a, S);
      else if constexpr (sizeof(T) == 8)
        return _mm512_slli_epi64(a, S);
    }
  }
  template <simd_integer_element T, unsigned S>
    requires(S < sizeof(T) * 8)
  simd_nodiscard simd_inline simd_const __m512i integer_right(__m512i a) noexcept {
    if constexpr (S == 0)
      return a;
    else {
#if SIMD_HAS_AVX512BW
      if constexpr (sizeof(T) == 1) {
        auto low = _mm512_and_si512(_mm512_srli_epi16(a, S), _mm512_set1_epi8(int8_t(255u >> S)));
        if constexpr (std::is_unsigned_v<T>)
          return low;
        else {
          auto negative = _mm512_movm_epi8(_mm512_cmp_epi8_mask(a, _mm512_setzero_si512(), _MM_CMPINT_LT));
          return _mm512_or_si512(
              low,
              _mm512_and_si512(negative, _mm512_set1_epi8(std::bit_cast<int8_t>(uint8_t(255u ^ (255u >> S))))));
        }
      } else if constexpr (sizeof(T) == 2) {
        if constexpr (std::is_signed_v<T>)
          return _mm512_srai_epi16(a, S);
        else
          return _mm512_srli_epi16(a, S);
      }
#endif
      if constexpr (sizeof(T) == 4) {
        if constexpr (std::is_signed_v<T>)
          return _mm512_srai_epi32(a, S);
        else
          return _mm512_srli_epi32(a, S);
      } else if constexpr (sizeof(T) == 8) {
        if constexpr (std::is_unsigned_v<T>)
          return _mm512_srli_epi64(a, S);
        else {
          return _mm512_srai_epi64(a, S);
        }
      }
    }
  }
#endif
#if SIMD_HAS_AVX2
  simd_nodiscard simd_inline simd_const __m128i integer_and(__m128i a, __m128i b) noexcept {
    return _mm_and_si128(a, b);
  }
  simd_nodiscard simd_inline simd_const __m128i integer_or(__m128i a, __m128i b) noexcept {
    return _mm_or_si128(a, b);
  }
  simd_nodiscard simd_inline simd_const __m128i integer_xor(__m128i a, __m128i b) noexcept {
    return _mm_xor_si128(a, b);
  }
  template <simd_integer_element T, bool Greater>
  simd_nodiscard simd_inline simd_const auto integer_compare(__m128i a, __m128i b) noexcept {
    if constexpr (mask_compact<sizeof(T), 16 / sizeof(T)>) {
      if constexpr (sizeof(T) == 1) {
        if constexpr (Greater) {
          if constexpr (std::is_signed_v<T>)
            return _mm_cmp_epi8_mask(a, b, _MM_CMPINT_GT);
          else
            return _mm_cmp_epu8_mask(a, b, _MM_CMPINT_GT);
        } else
          return _mm_cmp_epi8_mask(a, b, _MM_CMPINT_EQ);
      } else if constexpr (sizeof(T) == 2) {
        if constexpr (Greater) {
          if constexpr (std::is_signed_v<T>)
            return _mm_cmp_epi16_mask(a, b, _MM_CMPINT_GT);
          else
            return _mm_cmp_epu16_mask(a, b, _MM_CMPINT_GT);
        } else
          return _mm_cmp_epi16_mask(a, b, _MM_CMPINT_EQ);
      } else if constexpr (sizeof(T) == 4) {
        if constexpr (Greater) {
          if constexpr (std::is_signed_v<T>)
            return _mm_cmp_epi32_mask(a, b, _MM_CMPINT_GT);
          else
            return _mm_cmp_epu32_mask(a, b, _MM_CMPINT_GT);
        } else
          return _mm_cmp_epi32_mask(a, b, _MM_CMPINT_EQ);
      } else if constexpr (sizeof(T) == 8) {
        if constexpr (Greater) {
          if constexpr (std::is_signed_v<T>)
            return _mm_cmp_epi64_mask(a, b, _MM_CMPINT_GT);
          else
            return _mm_cmp_epu64_mask(a, b, _MM_CMPINT_GT);
        } else
          return _mm_cmp_epi64_mask(a, b, _MM_CMPINT_EQ);
      }
    } else {
      if constexpr (sizeof(T) == 1) {
        if constexpr (!Greater)
          return _mm_cmpeq_epi8(a, b);
        else if constexpr (std::is_signed_v<T>)
          return _mm_cmpgt_epi8(a, b);
        else {
          auto bias = _mm_set1_epi8(int8_t(-128));
          return _mm_cmpgt_epi8(_mm_xor_si128(a, bias), _mm_xor_si128(b, bias));
        }
      } else if constexpr (sizeof(T) == 2) {
        if constexpr (!Greater)
          return _mm_cmpeq_epi16(a, b);
        else if constexpr (std::is_signed_v<T>)
          return _mm_cmpgt_epi16(a, b);
        else {
          auto bias = _mm_set1_epi16(int16_t(-32768));
          return _mm_cmpgt_epi16(_mm_xor_si128(a, bias), _mm_xor_si128(b, bias));
        }
      } else if constexpr (sizeof(T) == 4) {
        if constexpr (!Greater)
          return _mm_cmpeq_epi32(a, b);
        else if constexpr (std::is_signed_v<T>)
          return _mm_cmpgt_epi32(a, b);
        else {
          auto bias = _mm_set1_epi32(std::bit_cast<int32_t>(uint32_t(0x80000000u)));
          return _mm_cmpgt_epi32(_mm_xor_si128(a, bias), _mm_xor_si128(b, bias));
        }
      } else if constexpr (sizeof(T) == 8) {
        if constexpr (!Greater)
          return _mm_cmpeq_epi64(a, b);
        else if constexpr (std::is_signed_v<T>)
          return _mm_cmpgt_epi64(a, b);
        else {
          auto bias = _mm_set1_epi64x(std::bit_cast<int64_t>(uint64_t(1) << 63));
          return _mm_cmpgt_epi64(_mm_xor_si128(a, bias), _mm_xor_si128(b, bias));
        }
      }
    }
  }
  template <simd_integer_element T, class M>
  simd_nodiscard simd_inline simd_const __m128i integer_select(M m, __m128i a, __m128i b) noexcept {
    if constexpr (M::compact) {
      if constexpr (sizeof(T) == 1)
        return _mm_mask_blend_epi8(m.to_native(), b, a);
      else if constexpr (sizeof(T) == 2)
        return _mm_mask_blend_epi16(m.to_native(), b, a);
      else if constexpr (sizeof(T) == 4)
        return _mm_mask_blend_epi32(m.to_native(), b, a);
      else if constexpr (sizeof(T) == 8)
        return _mm_mask_blend_epi64(m.to_native(), b, a);
    } else
      return _mm_or_si128(_mm_and_si128(m.to_native(), a), _mm_andnot_si128(m.to_native(), b));
  }
  template <simd_integer_element T, class M>
  simd_nodiscard simd_inline simd_const __m128i integer_masked_add(M m, __m128i prior, __m128i a,
                                                                   __m128i b) noexcept {
    if constexpr (M::compact) {
      if constexpr (sizeof(T) == 1)
        return _mm_mask_add_epi8(prior, m.to_native(), a, b);
      else if constexpr (sizeof(T) == 2)
        return _mm_mask_add_epi16(prior, m.to_native(), a, b);
      else if constexpr (sizeof(T) == 4)
        return _mm_mask_add_epi32(prior, m.to_native(), a, b);
      else if constexpr (sizeof(T) == 8)
        return _mm_mask_add_epi64(prior, m.to_native(), a, b);
    } else
      return integer_select<T>(m, integer_add<T>(a, b), prior);
  }
  template <simd_integer_element T, class M>
  simd_nodiscard simd_inline simd_const __m128i integer_masked_sub(M m, __m128i prior, __m128i a,
                                                                   __m128i b) noexcept {
    if constexpr (M::compact) {
      if constexpr (sizeof(T) == 1)
        return _mm_mask_sub_epi8(prior, m.to_native(), a, b);
      else if constexpr (sizeof(T) == 2)
        return _mm_mask_sub_epi16(prior, m.to_native(), a, b);
      else if constexpr (sizeof(T) == 4)
        return _mm_mask_sub_epi32(prior, m.to_native(), a, b);
      else if constexpr (sizeof(T) == 8)
        return _mm_mask_sub_epi64(prior, m.to_native(), a, b);
    } else
      return integer_select<T>(m, integer_sub<T>(a, b), prior);
  }
#endif

#if SIMD_HAS_AVX2
  simd_nodiscard simd_inline simd_const __m256i integer_and(__m256i a, __m256i b) noexcept {
    return _mm256_and_si256(a, b);
  }
  simd_nodiscard simd_inline simd_const __m256i integer_or(__m256i a, __m256i b) noexcept {
    return _mm256_or_si256(a, b);
  }
  simd_nodiscard simd_inline simd_const __m256i integer_xor(__m256i a, __m256i b) noexcept {
    return _mm256_xor_si256(a, b);
  }
  template <simd_integer_element T, bool Greater>
  simd_nodiscard simd_inline simd_const auto integer_compare(__m256i a, __m256i b) noexcept {
    if constexpr (mask_compact<sizeof(T), 32 / sizeof(T)>) {
      if constexpr (sizeof(T) == 1) {
        if constexpr (Greater) {
          if constexpr (std::is_signed_v<T>)
            return _mm256_cmp_epi8_mask(a, b, _MM_CMPINT_GT);
          else
            return _mm256_cmp_epu8_mask(a, b, _MM_CMPINT_GT);
        } else
          return _mm256_cmp_epi8_mask(a, b, _MM_CMPINT_EQ);
      } else if constexpr (sizeof(T) == 2) {
        if constexpr (Greater) {
          if constexpr (std::is_signed_v<T>)
            return _mm256_cmp_epi16_mask(a, b, _MM_CMPINT_GT);
          else
            return _mm256_cmp_epu16_mask(a, b, _MM_CMPINT_GT);
        } else
          return _mm256_cmp_epi16_mask(a, b, _MM_CMPINT_EQ);
      } else if constexpr (sizeof(T) == 4) {
        if constexpr (Greater) {
          if constexpr (std::is_signed_v<T>)
            return _mm256_cmp_epi32_mask(a, b, _MM_CMPINT_GT);
          else
            return _mm256_cmp_epu32_mask(a, b, _MM_CMPINT_GT);
        } else
          return _mm256_cmp_epi32_mask(a, b, _MM_CMPINT_EQ);
      } else if constexpr (sizeof(T) == 8) {
        if constexpr (Greater) {
          if constexpr (std::is_signed_v<T>)
            return _mm256_cmp_epi64_mask(a, b, _MM_CMPINT_GT);
          else
            return _mm256_cmp_epu64_mask(a, b, _MM_CMPINT_GT);
        } else
          return _mm256_cmp_epi64_mask(a, b, _MM_CMPINT_EQ);
      }
    } else {
      if constexpr (sizeof(T) == 1) {
        if constexpr (!Greater)
          return _mm256_cmpeq_epi8(a, b);
        else if constexpr (std::is_signed_v<T>)
          return _mm256_cmpgt_epi8(a, b);
        else {
          auto bias = _mm256_set1_epi8(int8_t(-128));
          return _mm256_cmpgt_epi8(_mm256_xor_si256(a, bias), _mm256_xor_si256(b, bias));
        }
      } else if constexpr (sizeof(T) == 2) {
        if constexpr (!Greater)
          return _mm256_cmpeq_epi16(a, b);
        else if constexpr (std::is_signed_v<T>)
          return _mm256_cmpgt_epi16(a, b);
        else {
          auto bias = _mm256_set1_epi16(int16_t(-32768));
          return _mm256_cmpgt_epi16(_mm256_xor_si256(a, bias), _mm256_xor_si256(b, bias));
        }
      } else if constexpr (sizeof(T) == 4) {
        if constexpr (!Greater)
          return _mm256_cmpeq_epi32(a, b);
        else if constexpr (std::is_signed_v<T>)
          return _mm256_cmpgt_epi32(a, b);
        else {
          auto bias = _mm256_set1_epi32(std::bit_cast<int32_t>(uint32_t(0x80000000u)));
          return _mm256_cmpgt_epi32(_mm256_xor_si256(a, bias), _mm256_xor_si256(b, bias));
        }
      } else if constexpr (sizeof(T) == 8) {
        if constexpr (!Greater)
          return _mm256_cmpeq_epi64(a, b);
        else if constexpr (std::is_signed_v<T>)
          return _mm256_cmpgt_epi64(a, b);
        else {
          auto bias = _mm256_set1_epi64x(std::bit_cast<int64_t>(uint64_t(1) << 63));
          return _mm256_cmpgt_epi64(_mm256_xor_si256(a, bias), _mm256_xor_si256(b, bias));
        }
      }
    }
  }
  template <simd_integer_element T, class M>
  simd_nodiscard simd_inline simd_const __m256i integer_select(M m, __m256i a, __m256i b) noexcept {
    if constexpr (M::compact) {
      if constexpr (sizeof(T) == 1)
        return _mm256_mask_blend_epi8(m.to_native(), b, a);
      else if constexpr (sizeof(T) == 2)
        return _mm256_mask_blend_epi16(m.to_native(), b, a);
      else if constexpr (sizeof(T) == 4)
        return _mm256_mask_blend_epi32(m.to_native(), b, a);
      else if constexpr (sizeof(T) == 8)
        return _mm256_mask_blend_epi64(m.to_native(), b, a);
    } else
      return _mm256_or_si256(_mm256_and_si256(m.to_native(), a), _mm256_andnot_si256(m.to_native(), b));
  }
  template <simd_integer_element T, class M>
  simd_nodiscard simd_inline simd_const __m256i integer_masked_add(M m, __m256i prior, __m256i a,
                                                                   __m256i b) noexcept {
    if constexpr (M::compact) {
      if constexpr (sizeof(T) == 1)
        return _mm256_mask_add_epi8(prior, m.to_native(), a, b);
      else if constexpr (sizeof(T) == 2)
        return _mm256_mask_add_epi16(prior, m.to_native(), a, b);
      else if constexpr (sizeof(T) == 4)
        return _mm256_mask_add_epi32(prior, m.to_native(), a, b);
      else if constexpr (sizeof(T) == 8)
        return _mm256_mask_add_epi64(prior, m.to_native(), a, b);
    } else
      return integer_select<T>(m, integer_add<T>(a, b), prior);
  }
  template <simd_integer_element T, class M>
  simd_nodiscard simd_inline simd_const __m256i integer_masked_sub(M m, __m256i prior, __m256i a,
                                                                   __m256i b) noexcept {
    if constexpr (M::compact) {
      if constexpr (sizeof(T) == 1)
        return _mm256_mask_sub_epi8(prior, m.to_native(), a, b);
      else if constexpr (sizeof(T) == 2)
        return _mm256_mask_sub_epi16(prior, m.to_native(), a, b);
      else if constexpr (sizeof(T) == 4)
        return _mm256_mask_sub_epi32(prior, m.to_native(), a, b);
      else if constexpr (sizeof(T) == 8)
        return _mm256_mask_sub_epi64(prior, m.to_native(), a, b);
    } else
      return integer_select<T>(m, integer_sub<T>(a, b), prior);
  }
#endif

#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ
  simd_nodiscard simd_inline simd_const __m512i integer_and(__m512i a, __m512i b) noexcept {
    return _mm512_and_si512(a, b);
  }
  simd_nodiscard simd_inline simd_const __m512i integer_or(__m512i a, __m512i b) noexcept {
    return _mm512_or_si512(a, b);
  }
  simd_nodiscard simd_inline simd_const __m512i integer_xor(__m512i a, __m512i b) noexcept {
    return _mm512_xor_si512(a, b);
  }
  template <simd_integer_element T, bool Greater>
  simd_nodiscard simd_inline simd_const auto integer_compare(__m512i a, __m512i b) noexcept {
    if constexpr (mask_compact<sizeof(T), 64 / sizeof(T)>) {
#if SIMD_HAS_AVX512BW
      if constexpr (sizeof(T) == 1) {
        if constexpr (Greater) {
          if constexpr (std::is_signed_v<T>)
            return _mm512_cmp_epi8_mask(a, b, _MM_CMPINT_GT);
          else
            return _mm512_cmp_epu8_mask(a, b, _MM_CMPINT_GT);
        } else
          return _mm512_cmp_epi8_mask(a, b, _MM_CMPINT_EQ);
      } else if constexpr (sizeof(T) == 2) {
        if constexpr (Greater) {
          if constexpr (std::is_signed_v<T>)
            return _mm512_cmp_epi16_mask(a, b, _MM_CMPINT_GT);
          else
            return _mm512_cmp_epu16_mask(a, b, _MM_CMPINT_GT);
        } else
          return _mm512_cmp_epi16_mask(a, b, _MM_CMPINT_EQ);
      }
#endif
      if constexpr (sizeof(T) == 4) {
        if constexpr (Greater) {
          if constexpr (std::is_signed_v<T>)
            return _mm512_cmp_epi32_mask(a, b, _MM_CMPINT_GT);
          else
            return _mm512_cmp_epu32_mask(a, b, _MM_CMPINT_GT);
        } else
          return _mm512_cmp_epi32_mask(a, b, _MM_CMPINT_EQ);
      } else if constexpr (sizeof(T) == 8) {
        if constexpr (Greater) {
          if constexpr (std::is_signed_v<T>)
            return _mm512_cmp_epi64_mask(a, b, _MM_CMPINT_GT);
          else
            return _mm512_cmp_epu64_mask(a, b, _MM_CMPINT_GT);
        } else
          return _mm512_cmp_epi64_mask(a, b, _MM_CMPINT_EQ);
      }
    } else {
      static_assert(sizeof(T) == 0, "512-bit integer comparisons require native predicates");
    }
  }
  template <simd_integer_element T, class M>
  simd_nodiscard simd_inline simd_const __m512i integer_select(M m, __m512i a, __m512i b) noexcept {
    if constexpr (M::compact) {
#if SIMD_HAS_AVX512BW
      if constexpr (sizeof(T) == 1)
        return _mm512_mask_blend_epi8(m.to_native(), b, a);
      else if constexpr (sizeof(T) == 2)
        return _mm512_mask_blend_epi16(m.to_native(), b, a);
#endif
      if constexpr (sizeof(T) == 4)
        return _mm512_mask_blend_epi32(m.to_native(), b, a);
      else if constexpr (sizeof(T) == 8)
        return _mm512_mask_blend_epi64(m.to_native(), b, a);
    } else
      return _mm512_or_si512(_mm512_and_si512(m.to_native(), a), _mm512_andnot_si512(m.to_native(), b));
  }
  template <simd_integer_element T, class M>
  simd_nodiscard simd_inline simd_const __m512i integer_masked_add(M m, __m512i prior, __m512i a,
                                                                   __m512i b) noexcept {
    if constexpr (M::compact) {
#if SIMD_HAS_AVX512BW
      if constexpr (sizeof(T) == 1)
        return _mm512_mask_add_epi8(prior, m.to_native(), a, b);
      else if constexpr (sizeof(T) == 2)
        return _mm512_mask_add_epi16(prior, m.to_native(), a, b);
#endif
      if constexpr (sizeof(T) == 4)
        return _mm512_mask_add_epi32(prior, m.to_native(), a, b);
      else if constexpr (sizeof(T) == 8)
        return _mm512_mask_add_epi64(prior, m.to_native(), a, b);
    } else
      return integer_select<T>(m, integer_add<T>(a, b), prior);
  }
  template <simd_integer_element T, class M>
  simd_nodiscard simd_inline simd_const __m512i integer_masked_sub(M m, __m512i prior, __m512i a,
                                                                   __m512i b) noexcept {
    if constexpr (M::compact) {
#if SIMD_HAS_AVX512BW
      if constexpr (sizeof(T) == 1)
        return _mm512_mask_sub_epi8(prior, m.to_native(), a, b);
      else if constexpr (sizeof(T) == 2)
        return _mm512_mask_sub_epi16(prior, m.to_native(), a, b);
#endif
      if constexpr (sizeof(T) == 4)
        return _mm512_mask_sub_epi32(prior, m.to_native(), a, b);
      else if constexpr (sizeof(T) == 8)
        return _mm512_mask_sub_epi64(prior, m.to_native(), a, b);
    } else
      return integer_select<T>(m, integer_sub<T>(a, b), prior);
  }
#endif

#if SIMD_HAS_ARM_NEON
  simd_nodiscard simd_inline simd_const uint8x16_t integer_and(uint8x16_t a, uint8x16_t b) noexcept {
    return vandq_u8(a, b);
  }
  simd_nodiscard simd_inline simd_const uint8x16_t integer_or(uint8x16_t a, uint8x16_t b) noexcept {
    return vorrq_u8(a, b);
  }
  simd_nodiscard simd_inline simd_const uint8x16_t integer_xor(uint8x16_t a, uint8x16_t b) noexcept {
    return veorq_u8(a, b);
  }
  template <simd_integer_element T>
  simd_nodiscard simd_inline simd_const uint8x16_t integer_add(uint8x16_t a, uint8x16_t b) noexcept {
    if constexpr (sizeof(T) == 1) {
      return vaddq_u8(a, b);
    } else if constexpr (sizeof(T) == 2) {
      return vreinterpretq_u8_u16(vaddq_u16(vreinterpretq_u16_u8(a), vreinterpretq_u16_u8(b)));
    } else if constexpr (sizeof(T) == 4) {
      return vreinterpretq_u8_u32(vaddq_u32(vreinterpretq_u32_u8(a), vreinterpretq_u32_u8(b)));
    } else if constexpr (sizeof(T) == 8) {
      return vreinterpretq_u8_u64(vaddq_u64(vreinterpretq_u64_u8(a), vreinterpretq_u64_u8(b)));
    }
  }
  template <simd_integer_element T>
  simd_nodiscard simd_inline simd_const uint8x16_t integer_sub(uint8x16_t a, uint8x16_t b) noexcept {
    if constexpr (sizeof(T) == 1) {
      return vsubq_u8(a, b);
    } else if constexpr (sizeof(T) == 2) {
      return vreinterpretq_u8_u16(vsubq_u16(vreinterpretq_u16_u8(a), vreinterpretq_u16_u8(b)));
    } else if constexpr (sizeof(T) == 4) {
      return vreinterpretq_u8_u32(vsubq_u32(vreinterpretq_u32_u8(a), vreinterpretq_u32_u8(b)));
    } else if constexpr (sizeof(T) == 8) {
      return vreinterpretq_u8_u64(vsubq_u64(vreinterpretq_u64_u8(a), vreinterpretq_u64_u8(b)));
    }
  }
  template <simd_integer_element T>
  simd_nodiscard simd_inline simd_const uint8x16_t integer_mul(uint8x16_t a, uint8x16_t b) noexcept {
    if constexpr (sizeof(T) == 1) {
      return vmulq_u8(a, b);
    } else if constexpr (sizeof(T) == 2) {
      return vreinterpretq_u8_u16(vmulq_u16(vreinterpretq_u16_u8(a), vreinterpretq_u16_u8(b)));
    } else if constexpr (sizeof(T) == 4) {
      return vreinterpretq_u8_u32(vmulq_u32(vreinterpretq_u32_u8(a), vreinterpretq_u32_u8(b)));
    } else if constexpr (sizeof(T) == 8) {
      auto x = vreinterpretq_u64_u8(a), y = vreinterpretq_u64_u8(b);
      auto xl = vmovn_u64(x), yl = vmovn_u64(y);
      auto cross =
          vadd_u32(vmul_u32(xl, vmovn_u64(vshrq_n_u64(y, 32))), vmul_u32(vmovn_u64(vshrq_n_u64(x, 32)), yl));
      return vreinterpretq_u8_u64(vaddq_u64(vmull_u32(xl, yl), vshlq_n_u64(vmovl_u32(cross), 32)));
    }
  }
  template <simd_integer_element T>
  simd_nodiscard simd_inline simd_const uint8x16_t integer_broadcast_16(T x) noexcept {
    if constexpr (sizeof(T) == 1) {
      return vdupq_n_u8(integer_word(x));
    } else if constexpr (sizeof(T) == 2) {
      return vreinterpretq_u8_u16(vdupq_n_u16(integer_word(x)));
    } else if constexpr (sizeof(T) == 4) {
      return vreinterpretq_u8_u32(vdupq_n_u32(integer_word(x)));
    } else if constexpr (sizeof(T) == 8) {
      return vreinterpretq_u8_u64(vdupq_n_u64(integer_word(x)));
    }
  }
  template <simd_integer_element T, unsigned S>
    requires(S < sizeof(T) * 8)
  simd_nodiscard simd_inline simd_const uint8x16_t integer_left(uint8x16_t a) noexcept {
    if constexpr (S == 0)
      return a;
    else {
      if constexpr (sizeof(T) == 1) {
        return vshlq_n_u8(a, S);
      } else if constexpr (sizeof(T) == 2) {
        return vreinterpretq_u8_u16(vshlq_n_u16(vreinterpretq_u16_u8(a), S));
      } else if constexpr (sizeof(T) == 4) {
        return vreinterpretq_u8_u32(vshlq_n_u32(vreinterpretq_u32_u8(a), S));
      } else if constexpr (sizeof(T) == 8) {
        return vreinterpretq_u8_u64(vshlq_n_u64(vreinterpretq_u64_u8(a), S));
      }
    }
  }
  template <simd_integer_element T, unsigned S>
    requires(S < sizeof(T) * 8)
  simd_nodiscard simd_inline simd_const uint8x16_t integer_right(uint8x16_t a) noexcept {
    if constexpr (S == 0)
      return a;
    else {
      if constexpr (sizeof(T) == 1) {
        if constexpr (std::is_unsigned_v<T>)
          return vshrq_n_u8(a, S);
        else
          return vreinterpretq_u8_s8(vshrq_n_s8(vreinterpretq_s8_u8(a), S));
      } else if constexpr (sizeof(T) == 2) {
        if constexpr (std::is_unsigned_v<T>)
          return vreinterpretq_u8_u16(vshrq_n_u16(vreinterpretq_u16_u8(a), S));
        else
          return vreinterpretq_u8_s16(vshrq_n_s16(vreinterpretq_s16_u8(a), S));
      } else if constexpr (sizeof(T) == 4) {
        if constexpr (std::is_unsigned_v<T>)
          return vreinterpretq_u8_u32(vshrq_n_u32(vreinterpretq_u32_u8(a), S));
        else
          return vreinterpretq_u8_s32(vshrq_n_s32(vreinterpretq_s32_u8(a), S));
      } else if constexpr (sizeof(T) == 8) {
        if constexpr (std::is_unsigned_v<T>)
          return vreinterpretq_u8_u64(vshrq_n_u64(vreinterpretq_u64_u8(a), S));
        else
          return vreinterpretq_u8_s64(vshrq_n_s64(vreinterpretq_s64_u8(a), S));
      }
    }
  }
  template <simd_integer_element T, bool Greater>
  simd_nodiscard simd_inline simd_const uint8x16_t integer_compare(uint8x16_t a, uint8x16_t b) noexcept {
    if constexpr (sizeof(T) == 1) {
      if constexpr (!Greater)
        return vceqq_u8(a, b);
      else if constexpr (std::is_unsigned_v<T>)
        return vcgtq_u8(a, b);
      else
        return vcgtq_s8(vreinterpretq_s8_u8(a), vreinterpretq_s8_u8(b));
    } else if constexpr (sizeof(T) == 2) {
      if constexpr (!Greater)
        return vreinterpretq_u8_u16(vceqq_u16(vreinterpretq_u16_u8(a), vreinterpretq_u16_u8(b)));
      else if constexpr (std::is_unsigned_v<T>)
        return vreinterpretq_u8_u16(vcgtq_u16(vreinterpretq_u16_u8(a), vreinterpretq_u16_u8(b)));
      else
        return vreinterpretq_u8_u16(vcgtq_s16(vreinterpretq_s16_u8(a), vreinterpretq_s16_u8(b)));
    } else if constexpr (sizeof(T) == 4) {
      if constexpr (!Greater)
        return vreinterpretq_u8_u32(vceqq_u32(vreinterpretq_u32_u8(a), vreinterpretq_u32_u8(b)));
      else if constexpr (std::is_unsigned_v<T>)
        return vreinterpretq_u8_u32(vcgtq_u32(vreinterpretq_u32_u8(a), vreinterpretq_u32_u8(b)));
      else
        return vreinterpretq_u8_u32(vcgtq_s32(vreinterpretq_s32_u8(a), vreinterpretq_s32_u8(b)));
    } else if constexpr (sizeof(T) == 8) {
      if constexpr (!Greater)
        return vreinterpretq_u8_u64(vceqq_u64(vreinterpretq_u64_u8(a), vreinterpretq_u64_u8(b)));
      else if constexpr (std::is_unsigned_v<T>)
        return vreinterpretq_u8_u64(vcgtq_u64(vreinterpretq_u64_u8(a), vreinterpretq_u64_u8(b)));
      else
        return vreinterpretq_u8_u64(vcgtq_s64(vreinterpretq_s64_u8(a), vreinterpretq_s64_u8(b)));
    }
  }
  template <simd_integer_element T, class M>
  simd_nodiscard simd_inline simd_const uint8x16_t integer_select(M m, uint8x16_t a, uint8x16_t b) noexcept {
    return vbslq_u8(m.to_native(), a, b);
  }
  template <simd_integer_element T, class M>
  simd_nodiscard simd_inline simd_const uint8x16_t integer_masked_add(M m, uint8x16_t prior, uint8x16_t a,
                                                                      uint8x16_t b) noexcept {
    return integer_select<T>(m, integer_add<T>(a, b), prior);
  }
  template <simd_integer_element T, class M>
  simd_nodiscard simd_inline simd_const uint8x16_t integer_masked_sub(M m, uint8x16_t prior, uint8x16_t a,
                                                                      uint8x16_t b) noexcept {
    return integer_select<T>(m, integer_sub<T>(a, b), prior);
  }
#endif

#if SIMD_HAS_AVX2
  template <simd_integer_element T, class M>
  simd_nodiscard simd_inline simd_const __m128i integer_masked_mul(M m, __m128i prior, __m128i a,
                                                                   __m128i b) noexcept {
    if constexpr (M::compact && sizeof(T) > 1) {
      if constexpr (sizeof(T) == 2)
        return _mm_mask_mullo_epi16(prior, m.to_native(), a, b);
      if constexpr (sizeof(T) == 4)
        return _mm_mask_mullo_epi32(prior, m.to_native(), a, b);
      else if constexpr (sizeof(T) == 8)
        return _mm_mask_mullo_epi64(prior, m.to_native(), a, b);
    } else
      return integer_select<T>(m, integer_mul<T>(a, b), prior);
  }
#endif
#if SIMD_HAS_AVX2
  template <simd_integer_element T, class M>
  simd_nodiscard simd_inline simd_const __m256i integer_masked_mul(M m, __m256i prior, __m256i a,
                                                                   __m256i b) noexcept {
    if constexpr (M::compact && sizeof(T) > 1) {
      if constexpr (sizeof(T) == 2)
        return _mm256_mask_mullo_epi16(prior, m.to_native(), a, b);
      if constexpr (sizeof(T) == 4)
        return _mm256_mask_mullo_epi32(prior, m.to_native(), a, b);
      else if constexpr (sizeof(T) == 8)
        return _mm256_mask_mullo_epi64(prior, m.to_native(), a, b);
    } else
      return integer_select<T>(m, integer_mul<T>(a, b), prior);
  }
#endif
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ
  template <simd_integer_element T, class M>
  simd_nodiscard simd_inline simd_const __m512i integer_masked_mul(M m, __m512i prior, __m512i a,
                                                                   __m512i b) noexcept {
    if constexpr (M::compact && sizeof(T) > 1) {
#if SIMD_HAS_AVX512BW
      if constexpr (sizeof(T) == 2)
        return _mm512_mask_mullo_epi16(prior, m.to_native(), a, b);
#endif
      if constexpr (sizeof(T) == 4)
        return _mm512_mask_mullo_epi32(prior, m.to_native(), a, b);
      else if constexpr (sizeof(T) == 8)
        return _mm512_mask_mullo_epi64(prior, m.to_native(), a, b);
    } else
      return integer_select<T>(m, integer_mul<T>(a, b), prior);
  }
#endif
#if SIMD_HAS_ARM_NEON
  template <simd_integer_element T, class M>
  simd_nodiscard simd_inline simd_const uint8x16_t integer_masked_mul(M m, uint8x16_t prior, uint8x16_t a,
                                                                      uint8x16_t b) noexcept {
    return integer_select<T>(m, integer_mul<T>(a, b), prior);
  }
#endif
#if SIMD_HAS_AVX2
  simd_nodiscard simd_inline simd_const __m128i integer_bit_select(__m128i m,__m128i a,__m128i b) noexcept {
    return _mm_or_si128(_mm_and_si128(m,a),_mm_andnot_si128(m,b));
  }
#endif
#if SIMD_HAS_AVX2
  simd_nodiscard simd_inline simd_const __m256i integer_bit_select(__m256i m,__m256i a,__m256i b) noexcept {
    return _mm256_or_si256(_mm256_and_si256(m,a),_mm256_andnot_si256(m,b));
  }
#endif
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ
  simd_nodiscard simd_inline simd_const __m512i integer_bit_select(__m512i m,__m512i a,__m512i b) noexcept {
    return _mm512_or_si512(_mm512_and_si512(m,a),_mm512_andnot_si512(m,b));
  }
#endif
#if SIMD_HAS_ARM_NEON
  simd_nodiscard simd_inline simd_const uint8x16_t integer_bit_select(uint8x16_t m,uint8x16_t a,uint8x16_t b) noexcept { return vbslq_u8(m,a,b); }
#endif
} // namespace SIMD_BACKEND_NAMESPACE

#include <algorithm>
#include <cassert>
#include <span>
#include <limits>
#include <utility>

namespace simd {
  namespace detail::SIMD_BACKEND {
    template <std::size_t Bytes> struct integer_storage;
#if SIMD_HAS_AVX2
    template <> struct integer_storage<16> {
      using type = __m128i;
    };
    template <> struct integer_storage<32> {
      using type = __m256i;
    };
#endif
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ
    template <> struct integer_storage<64> {
      using type = __m512i;
    };
#endif
#if SIMD_HAS_ARM_NEON
    template <> struct integer_storage<16> {
      using type = uint8x16_t;
    };
#endif
    template <class T, std::size_t N>
    inline constexpr bool integer_shape =
        simd_integer_element<T> && (N == 1
#if SIMD_HAS_AVX2
                                    || sizeof(T) * N == 16 || sizeof(T) * N == 32
#endif
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ
                                    || (sizeof(T) * N == 64 && (sizeof(T) >= 4
#if SIMD_HAS_AVX512BW
                                                                || sizeof(T) <= 2
#endif
                                                                ))
#endif
#if SIMD_HAS_ARM_NEON
                                    || sizeof(T) * N == 16
#endif
                                   );
  } // namespace detail

  namespace detail::SIMD_BACKEND {
  template <simd_integer_element T, std::size_t N, std::size_t A = 1,
            simd_access Access = simd_access::ordinary>
    requires SIMD_BACKEND_NAMESPACE::integer_shape<T, N>
  simd_nodiscard simd_inline simd_pure vec<T, N,SIMD_ARCH> load_simd(T const *p, simd_memory<A, Access> = {}) noexcept;
  template <simd_integer_element T, std::size_t N, std::size_t A = 1,
            simd_access Access = simd_access::ordinary>
    requires SIMD_BACKEND_NAMESPACE::integer_shape<T, N>
  simd_inline void store_simd(T *p, vec<T, N,SIMD_ARCH> v, simd_memory<A, Access> = {}) noexcept;
  template <simd_integer_element T, std::size_t N, std::size_t A = 1,
            simd_access Access = simd_access::ordinary>
    requires SIMD_BACKEND_NAMESPACE::integer_shape<T, N>
  simd_nodiscard simd_inline simd_pure vec<T, N,SIMD_ARCH> load_simd_partial(T const *p, std::size_t count,
                                                                    simd_memory<A, Access> = {}) noexcept;
  template <simd_integer_element T, std::size_t N, std::size_t A = 1,
            simd_access Access = simd_access::ordinary>
    requires SIMD_BACKEND_NAMESPACE::integer_shape<T, N>
  simd_inline void store_simd_partial(T *p, vec<T, N,SIMD_ARCH> v, std::size_t count,
                                      simd_memory<A, Access> = {}) noexcept;

  }
  template <simd_integer_element T> struct vec<T, 1,SIMD_ARCH> : detail::swizzle_access<T,1,SIMD_ARCH> {
    using architecture = SIMD_ARCH;
    /// Select this architecture and forward arguments to the corresponding constructor.
    /// Exception behavior is exactly that of the forwarded construction.
    template<class... X> requires std::constructible_from<vec,X...>
    simd_inline constexpr vec(SIMD_ARCH, X &&... x)
        noexcept(std::is_nothrow_constructible_v<vec,X...>) : vec(std::forward<X>(x)...) {}
    template <class U> using rebind = vec<U,1,SIMD_ARCH>;
    template <std::size_t A = 1>
    /// Load logical lanes, assuming the template alignment in bytes.
    simd_nodiscard static simd_inline vec load_memory(T const * p) noexcept {
      return ::SIMD_BACKEND_NAMESPACE::load_simd<T,1>(p,simd_memory<A>{});
    }
    template <std::size_t A = 1>
    /// Store logical lanes, assuming the template alignment in bytes.
    simd_inline void store_memory(T * p) const noexcept {
      ::SIMD_BACKEND_NAMESPACE::store_simd(p,*this,simd_memory<A>{});
    }

    using value_type = T;
    using native_type = T;
    using mask_type = SIMD_BACKEND_NAMESPACE::comparison_mask<T, 1>;
    using mask = mask_type;
    using predicate_type = predicate<1,SIMD_ARCH>;
    using unsigned_register_tag = void;
    static constexpr std::size_t lanes = 1;
    T value{};
    /// Initialize the stored lane values to zero.
    constexpr vec() noexcept = default;
    /// Broadcast the integral input, retaining its low sizeof(T)*8 bits in each lane.
    template <simd_integer_element U> constexpr vec(U x) noexcept : value(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(x)) {}
    /// Copy one value per logical lane in array order.
    constexpr vec(std::array<T, 1> const &x) noexcept : value(x[0]) {}
    /// Return the native storage value without a numerical conversion.
    simd_nodiscard simd_inline constexpr operator T() const noexcept { return value; }
    /// Adopt native storage without numerical conversion.
    simd_nodiscard simd_inline static constexpr vec from_native(T x) noexcept { return vec(x); }
    /// Return the native storage representation.
    simd_nodiscard simd_inline constexpr T to_native() const noexcept { return value; }
    /// Add corresponding lanes modulo 2^(sizeof(T)*8).
    simd_nodiscard friend simd_inline simd_const constexpr vec operator+(vec a, vec b) noexcept {
      return from_native(SIMD_BACKEND_NAMESPACE::integer_scalar_add(a.value, b.value));
    }
    /// Subtract corresponding lanes modulo 2^(sizeof(T)*8).
    simd_nodiscard friend simd_inline simd_const constexpr vec operator-(vec a, vec b) noexcept {
      return from_native(SIMD_BACKEND_NAMESPACE::integer_scalar_sub(a.value, b.value));
    }
    /// Multiply corresponding lanes modulo 2^(sizeof(T)*8).
    simd_nodiscard friend simd_inline simd_const constexpr vec operator*(vec a, vec b) noexcept {
      return from_native(SIMD_BACKEND_NAMESPACE::integer_scalar_mul(a.value, b.value));
    }
    /// Bitwise AND of corresponding lane representations.
    simd_nodiscard friend simd_inline simd_const constexpr vec operator&(vec a, vec b) noexcept {
      return from_native(
          SIMD_BACKEND_NAMESPACE::integer_wrap<T>(SIMD_BACKEND_NAMESPACE::integer_word(a.value) & SIMD_BACKEND_NAMESPACE::integer_word(b.value)));
    }
    /// Bitwise OR of corresponding lane representations.
    simd_nodiscard friend simd_inline simd_const constexpr vec operator|(vec a, vec b) noexcept {
      return from_native(
          SIMD_BACKEND_NAMESPACE::integer_wrap<T>(SIMD_BACKEND_NAMESPACE::integer_word(a.value) | SIMD_BACKEND_NAMESPACE::integer_word(b.value)));
    }
    /// Bitwise XOR of corresponding lane representations.
    simd_nodiscard friend simd_inline simd_const constexpr vec operator^(vec a, vec b) noexcept {
      return from_native(
          SIMD_BACKEND_NAMESPACE::integer_wrap<T>(SIMD_BACKEND_NAMESPACE::integer_word(a.value) ^ SIMD_BACKEND_NAMESPACE::integer_word(b.value)));
    }
    /// Complement every bit in every lane.
    simd_nodiscard friend simd_inline simd_const constexpr vec operator~(vec a) noexcept {
      return a ^ vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(~std::make_unsigned_t<T>(0)));
    }
    /// Negate each lane modulo 2^(sizeof(T)*8).
    simd_nodiscard friend simd_inline simd_const constexpr vec operator-(vec a) noexcept { return vec(T(0)) - a; }
    /// Return the unchanged vector value.
    simd_nodiscard friend simd_inline simd_const constexpr vec operator+(vec a) noexcept { return a; }
    /// Return a mask whose lanes are true where `a == b` holds. Ordering follows the signedness of T.
    simd_nodiscard friend simd_inline simd_const constexpr mask_type operator==(vec a, vec b) noexcept {
      return mask_type(a.value == b.value);
    }
    /// Return a mask whose lanes are true where `a > b` holds. Ordering follows the signedness of T.
    simd_nodiscard friend simd_inline simd_const constexpr mask_type operator>(vec a, vec b) noexcept {
      return mask_type(a.value > b.value);
    }
    /// Return a mask whose lanes are true where `a != b` holds. Ordering follows the signedness of T.
    simd_nodiscard friend simd_inline simd_const constexpr mask_type operator!=(vec a, vec b) noexcept {
      return ~(a == b);
    }
    /// Return a mask whose lanes are true where `a < b` holds. Ordering follows the signedness of T.
    simd_nodiscard friend simd_inline simd_const constexpr mask_type operator<(vec a, vec b) noexcept {
      return b > a;
    }
    /// Return a mask whose lanes are true where `a <= b` holds. Ordering follows the signedness of T.
    simd_nodiscard friend simd_inline simd_const constexpr mask_type operator<=(vec a, vec b) noexcept {
      return ~(a > b);
    }
    /// Return a mask whose lanes are true where `a >= b` holds. Ordering follows the signedness of T.
    simd_nodiscard friend simd_inline simd_const constexpr mask_type operator>=(vec a, vec b) noexcept {
      return ~(b > a);
    }
    /// Add corresponding lanes modulo 2^(sizeof(T)*8). Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const constexpr vec operator+(vec a, U b) noexcept {
      return a + vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Add corresponding lanes modulo 2^(sizeof(T)*8). Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const constexpr vec operator+(U a, vec b) noexcept {
      return vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(a)) + b;
    }
    /// Subtract corresponding lanes modulo 2^(sizeof(T)*8). Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const constexpr vec operator-(vec a, U b) noexcept {
      return a - vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Subtract corresponding lanes modulo 2^(sizeof(T)*8). Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const constexpr vec operator-(U a, vec b) noexcept {
      return vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(a)) - b;
    }
    /// Multiply corresponding lanes modulo 2^(sizeof(T)*8). Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const constexpr vec operator*(vec a, U b) noexcept {
      return a * vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Multiply corresponding lanes modulo 2^(sizeof(T)*8). Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const constexpr vec operator*(U a, vec b) noexcept {
      return vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(a)) * b;
    }
    /// Bitwise AND of corresponding lane representations. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const constexpr vec operator&(vec a, U b) noexcept {
      return a & vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Bitwise AND of corresponding lane representations. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const constexpr vec operator&(U a, vec b) noexcept {
      return vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(a)) & b;
    }
    /// Bitwise OR of corresponding lane representations. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const constexpr vec operator|(vec a, U b) noexcept {
      return a | vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Bitwise OR of corresponding lane representations. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const constexpr vec operator|(U a, vec b) noexcept {
      return vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(a)) | b;
    }
    /// Bitwise XOR of corresponding lane representations. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const constexpr vec operator^(vec a, U b) noexcept {
      return a ^ vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Bitwise XOR of corresponding lane representations. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const constexpr vec operator^(U a, vec b) noexcept {
      return vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(a)) ^ b;
    }
    /// Return a mask whose lanes are true where `a == b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const constexpr mask_type operator==(vec a, U b) noexcept {
      return a == vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Return a mask whose lanes are true where `a == b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const constexpr mask_type operator==(U a, vec b) noexcept {
      return vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(a)) == b;
    }
    /// Return a mask whose lanes are true where `a != b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const constexpr mask_type operator!=(vec a, U b) noexcept {
      return a != vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Return a mask whose lanes are true where `a != b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const constexpr mask_type operator!=(U a, vec b) noexcept {
      return vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(a)) != b;
    }
    /// Return a mask whose lanes are true where `a < b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const constexpr mask_type operator<(vec a, U b) noexcept {
      return a < vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Return a mask whose lanes are true where `a < b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const constexpr mask_type operator<(U a, vec b) noexcept {
      return vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(a)) < b;
    }
    /// Return a mask whose lanes are true where `a > b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const constexpr mask_type operator>(vec a, U b) noexcept {
      return a > vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Return a mask whose lanes are true where `a > b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const constexpr mask_type operator>(U a, vec b) noexcept {
      return vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(a)) > b;
    }
    /// Return a mask whose lanes are true where `a <= b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const constexpr mask_type operator<=(vec a, U b) noexcept {
      return a <= vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Return a mask whose lanes are true where `a <= b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const constexpr mask_type operator<=(U a, vec b) noexcept {
      return vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(a)) <= b;
    }
    /// Return a mask whose lanes are true where `a >= b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const constexpr mask_type operator>=(vec a, U b) noexcept {
      return a >= vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Return a mask whose lanes are true where `a >= b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const constexpr mask_type operator>=(U a, vec b) noexcept {
      return vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(a)) >= b;
    }
    /// Apply the corresponding lane-wise add operation in place and return *this.
    simd_inline vec &operator+=(vec b) noexcept { return *this = *this + b; }
    /// Apply the corresponding lane-wise add operation in place and return *this. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U> simd_inline vec &operator+=(U b) noexcept { return *this = *this + b; }
    /// Apply the corresponding lane-wise subtract operation in place and return *this.
    simd_inline vec &operator-=(vec b) noexcept { return *this = *this - b; }
    /// Apply the corresponding lane-wise subtract operation in place and return *this. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U> simd_inline vec &operator-=(U b) noexcept { return *this = *this - b; }
    /// Apply the corresponding lane-wise multiply operation in place and return *this.
    simd_inline vec &operator*=(vec b) noexcept { return *this = *this * b; }
    /// Apply the corresponding lane-wise multiply operation in place and return *this. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U> simd_inline vec &operator*=(U b) noexcept { return *this = *this * b; }
    /// Apply the corresponding lane-wise AND operation in place and return *this.
    simd_inline vec &operator&=(vec b) noexcept { return *this = *this & b; }
    /// Apply the corresponding lane-wise AND operation in place and return *this. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U> simd_inline vec &operator&=(U b) noexcept { return *this = *this & b; }
    /// Apply the corresponding lane-wise OR operation in place and return *this.
    simd_inline vec &operator|=(vec b) noexcept { return *this = *this | b; }
    /// Apply the corresponding lane-wise OR operation in place and return *this. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U> simd_inline vec &operator|=(U b) noexcept { return *this = *this | b; }
    /// Apply the corresponding lane-wise XOR operation in place and return *this.
    simd_inline vec &operator^=(vec b) noexcept { return *this = *this ^ b; }
    /// Apply the corresponding lane-wise XOR operation in place and return *this. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U> simd_inline vec &operator^=(U b) noexcept { return *this = *this ^ b; }
    /// Shift each lane left by compile-time K, discarding high bits; require K below the lane bit width.
    template <unsigned K>
      requires(K < sizeof(T) * 8)
    simd_nodiscard simd_inline simd_const constexpr vec left() const noexcept {
      using W = SIMD_BACKEND_NAMESPACE::integer_work_word<T>;
      auto u = SIMD_BACKEND_NAMESPACE::integer_word(value);
      return from_native(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(W(u) << K));
    }
    /// Shift each lane right by compile-time K; signed lanes extend their sign. Require K below the lane bit width.
    template <unsigned K>
      requires(K < sizeof(T) * 8)
    simd_nodiscard simd_inline simd_const constexpr vec right() const noexcept {
      using U = std::make_unsigned_t<T>;
      using W = SIMD_BACKEND_NAMESPACE::integer_work_word<T>;
      auto u = SIMD_BACKEND_NAMESPACE::integer_word(value);
      if constexpr (K == 0)
        return *this;
      else {
        U result = U(W(u) >> K);
        if constexpr (std::is_signed_v<T>) {
          if (value < T(0))
            result = U(result | U(std::numeric_limits<U>::max() ^ (std::numeric_limits<U>::max() >> K)));
        }
        return from_native(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(result));
      }
    }
    /// Shift every lane left by K bits, discarding high bits. Require K smaller than the lane bit width.
    template <std::size_t K>
      requires(K < sizeof(T) * 8)
    simd_nodiscard friend simd_inline simd_const constexpr vec operator<<(vec a, imm_t<K>) noexcept {
      return a.template left<K>();
    }
    /// Shift every lane right by K; signed lanes extend the sign, unsigned lanes shift in zero. Require K smaller than the lane bit width.
    template <std::size_t K>
      requires(K < sizeof(T) * 8)
    simd_nodiscard friend simd_inline simd_const constexpr vec operator>>(vec a, imm_t<K>) noexcept {
      return a.template right<K>();
    }
    /// Read all logical lanes from an unaligned element pointer.
    simd_nodiscard simd_inline static simd_pure vec load(T const *p) noexcept {
      return SIMD_BACKEND_NAMESPACE::load_simd<T, lanes>(p);
    }
    /// Read all logical lanes without an extra alignment promise.
    simd_nodiscard simd_inline static simd_pure vec loadu(T const *p) noexcept {
      return SIMD_BACKEND_NAMESPACE::load_simd<T, lanes>(p);
    }
    /// Write all logical lanes to an unaligned element pointer.
    simd_inline void store(T *p) const noexcept { SIMD_BACKEND_NAMESPACE::store_simd(p, *this); }
    /// Write all logical lanes without an extra alignment promise.
    simd_inline void storeu(T *p) const noexcept { SIMD_BACKEND_NAMESPACE::store_simd(p, *this); }
    /// Read exactly n logical lanes and fill the remainder; require n <= lanes. For n == 0, p may be null.
    simd_nodiscard simd_inline static simd_pure vec load_partial(T const *p, std::size_t n,
                                                                  T fill = T(0)) noexcept {
      assert(n <= lanes);
      std::array<T, lanes> data;
      data.fill(fill);
      if (n)
        std::memcpy(data.data(), static_cast<void const *>(p), n * sizeof(T));
      return SIMD_BACKEND_NAMESPACE::load_simd<T, lanes>(data.data());
    }
    /// Write exactly n logical lanes; require n <= lanes. For n == 0, p may be null.
    simd_inline void store_partial(T *p, std::size_t n) const noexcept { SIMD_BACKEND_NAMESPACE::store_simd_partial(p, *this, n); }
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator+(vec, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator+(U, vec) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator+(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator-(vec, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator-(U, vec) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator-(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator*(vec, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator*(U, vec) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator*(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator/(vec, U) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator/(U, vec) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator/(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator%(vec, U) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator%(U, vec) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator%(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator&(vec, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator&(U, vec) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator&(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator|(vec, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator|(U, vec) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator|(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator^(vec, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator^(U, vec) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator^(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator==(vec, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator==(U, vec) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator==(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator!=(vec, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator!=(U, vec) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator!=(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator<(vec, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator<(U, vec) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator<(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator>(vec, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator>(U, vec) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator>(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator<=(vec, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator<=(U, vec) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator<=(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator>=(vec, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator>=(U, vec) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator>=(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    friend vec operator/(vec, vec) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    friend vec operator%(vec, vec) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <simd_integer_element U> friend vec operator/(vec, U) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <simd_integer_element U> friend vec operator/(U, vec) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <simd_integer_element U> friend vec operator%(vec, U) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <simd_integer_element U> friend vec operator%(U, vec) = delete;
    /// Reject runtime shift counts; use a compile-time imm<K> within the lane width.
    template <simd_integer_element U> friend vec operator<<(vec, U) = delete;
    /// Reject runtime shift counts; use a compile-time imm<K> within the lane width.
    template <simd_integer_element U> friend vec operator>>(vec, U) = delete;
  };

#if SIMD_HAS_AVX2 || SIMD_HAS_ARM_NEON || (SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ)
  // Qualified native helper names must exist even before constraints are checked.
  // A real integer specialization owns one intrinsic register. The storage trait
  // chooses representation only; every operation below has integer semantics.
  template <simd_integer_element T, std::size_t N>
    requires(N > 1 && SIMD_BACKEND_NAMESPACE::integer_shape<T, N>)
  struct vec<T, N,SIMD_ARCH> : detail::swizzle_access<T,N,SIMD_ARCH> {
    using architecture = SIMD_ARCH;
    /// Select this architecture and forward arguments to the corresponding constructor.
    /// Exception behavior is exactly that of the forwarded construction.
    template<class... X> requires std::constructible_from<vec,X...>
    simd_inline constexpr vec(SIMD_ARCH, X &&... x)
        noexcept(std::is_nothrow_constructible_v<vec,X...>) : vec(std::forward<X>(x)...) {}
    template <class U> using rebind = vec<U,N,SIMD_ARCH>;
    template <std::size_t A = 1>
    /// Load logical lanes, assuming the template alignment in bytes.
    simd_nodiscard static simd_inline vec load_memory(T const * p) noexcept {
      return ::SIMD_BACKEND_NAMESPACE::load_simd<T,N>(p,simd_memory<A>{});
    }
    template <std::size_t A = 1>
    /// Store logical lanes, assuming the template alignment in bytes.
    simd_inline void store_memory(T * p) const noexcept {
      ::SIMD_BACKEND_NAMESPACE::store_simd(p,*this,simd_memory<A>{});
    }

    using value_type = T;
    using native_type = typename SIMD_BACKEND_NAMESPACE::integer_storage<sizeof(T) * N>::type;
    using mask_type = SIMD_BACKEND_NAMESPACE::comparison_mask<T, N>;
    using mask = mask_type;
    using predicate_type = predicate<N,SIMD_ARCH>;
    using unsigned_register_tag = void;
    static constexpr std::size_t lanes = N;
    native_type value{};
    /// Initialize the stored lane values to zero.
    vec() noexcept = default;
    /// Broadcast the integral input, retaining its low sizeof(T)*8 bits in each lane.
    template <simd_integer_element U> simd_inline vec(U input) noexcept {
      T x = SIMD_BACKEND_NAMESPACE::integer_wrap<T>(input);
      if constexpr (sizeof(T) * N == 16)
        value = SIMD_BACKEND_NAMESPACE::integer_broadcast_16(x);
#if SIMD_HAS_AVX2
      else if constexpr (sizeof(T) * N == 32)
        value = SIMD_BACKEND_NAMESPACE::integer_broadcast_32(x);
#endif
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ
      else if constexpr (sizeof(T) * N == 64)
        value = SIMD_BACKEND_NAMESPACE::integer_broadcast_64(x);
#endif
    }
    /// Adopt native lane storage without numerical conversion.
    simd_inline vec(native_type x) noexcept : value(x) {}
    /// Construct logical lanes in argument order. Integral conversion retains the low lane-width bits.
    template <class... U>
      requires(sizeof...(U) == N && (simd_integer_element<U> && ...))
    simd_inline vec(U... xs) noexcept {
      std::array<T, N> data{SIMD_BACKEND_NAMESPACE::integer_wrap<T>(xs)...};
      std::memcpy(&value, data.data(), sizeof(value));
    }
    /// Copy one value per logical lane in array order.
    simd_inline vec(std::array<T, N> const &data) noexcept {
      std::memcpy(&value, data.data(), sizeof(value));
    }
    /// Return the native storage value without a numerical conversion.
    simd_nodiscard simd_inline simd_const operator native_type() const noexcept { return value; }
    /// Adopt native storage without numerical conversion.
    simd_nodiscard simd_inline static simd_const vec from_native(native_type x) noexcept { return vec(x); }
    /// Return the native storage representation.
    simd_nodiscard simd_inline simd_const native_type to_native() const noexcept { return value; }
    /// Add corresponding lanes modulo 2^(sizeof(T)*8).
    simd_nodiscard friend simd_inline simd_const vec operator+(vec a, vec b) noexcept {
      return from_native(SIMD_BACKEND_NAMESPACE::integer_add<T>(a.value, b.value));
    }
    /// Subtract corresponding lanes modulo 2^(sizeof(T)*8).
    simd_nodiscard friend simd_inline simd_const vec operator-(vec a, vec b) noexcept {
      return from_native(SIMD_BACKEND_NAMESPACE::integer_sub<T>(a.value, b.value));
    }
    /// Multiply corresponding lanes modulo 2^(sizeof(T)*8).
    simd_nodiscard friend simd_inline simd_const vec operator*(vec a, vec b) noexcept {
      return from_native(SIMD_BACKEND_NAMESPACE::integer_mul<T>(a.value, b.value));
    }
    /// Bitwise AND of corresponding lane representations.
    simd_nodiscard friend simd_inline simd_const vec operator&(vec a, vec b) noexcept {
      return from_native(SIMD_BACKEND_NAMESPACE::integer_and(a.value, b.value));
    }
    /// Bitwise OR of corresponding lane representations.
    simd_nodiscard friend simd_inline simd_const vec operator|(vec a, vec b) noexcept {
      return from_native(SIMD_BACKEND_NAMESPACE::integer_or(a.value, b.value));
    }
    /// Bitwise XOR of corresponding lane representations.
    simd_nodiscard friend simd_inline simd_const vec operator^(vec a, vec b) noexcept {
      return from_native(SIMD_BACKEND_NAMESPACE::integer_xor(a.value, b.value));
    }
    /// Complement every bit in every lane.
    simd_nodiscard friend simd_inline simd_const vec operator~(vec a) noexcept {
      return a ^ vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(~std::make_unsigned_t<T>(0)));
    }
    /// Negate each lane modulo 2^(sizeof(T)*8).
    simd_nodiscard friend simd_inline simd_const vec operator-(vec a) noexcept { return vec(T(0)) - a; }
    /// Return the unchanged vector value.
    simd_nodiscard friend simd_inline simd_const vec operator+(vec a) noexcept { return a; }
    /// Return a mask whose lanes are true where `a == b` holds. Ordering follows the signedness of T.
    simd_nodiscard friend simd_inline simd_const mask_type operator==(vec a, vec b) noexcept {
      return mask_type::unsafe_from_native(SIMD_BACKEND_NAMESPACE::integer_compare<T, false>(a.value, b.value));
    }
    /// Return a mask whose lanes are true where `a > b` holds. Ordering follows the signedness of T.
    simd_nodiscard friend simd_inline simd_const mask_type operator>(vec a, vec b) noexcept {
      return mask_type::unsafe_from_native(SIMD_BACKEND_NAMESPACE::integer_compare<T, true>(a.value, b.value));
    }
    /// Return a mask whose lanes are true where `a != b` holds. Ordering follows the signedness of T.
    simd_nodiscard friend simd_inline simd_const mask_type operator!=(vec a, vec b) noexcept {
      return ~(a == b);
    }
    /// Return a mask whose lanes are true where `a < b` holds. Ordering follows the signedness of T.
    simd_nodiscard friend simd_inline simd_const mask_type operator<(vec a, vec b) noexcept {
      return b > a;
    }
    /// Return a mask whose lanes are true where `a <= b` holds. Ordering follows the signedness of T.
    simd_nodiscard friend simd_inline simd_const mask_type operator<=(vec a, vec b) noexcept {
      return ~(a > b);
    }
    /// Return a mask whose lanes are true where `a >= b` holds. Ordering follows the signedness of T.
    simd_nodiscard friend simd_inline simd_const mask_type operator>=(vec a, vec b) noexcept {
      return ~(b > a);
    }
    /// Add corresponding lanes modulo 2^(sizeof(T)*8). Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const vec operator+(vec a, U b) noexcept {
      return a + vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Add corresponding lanes modulo 2^(sizeof(T)*8). Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const vec operator+(U a, vec b) noexcept {
      return vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(a)) + b;
    }
    /// Subtract corresponding lanes modulo 2^(sizeof(T)*8). Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const vec operator-(vec a, U b) noexcept {
      return a - vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Subtract corresponding lanes modulo 2^(sizeof(T)*8). Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const vec operator-(U a, vec b) noexcept {
      return vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(a)) - b;
    }
    /// Multiply corresponding lanes modulo 2^(sizeof(T)*8). Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const vec operator*(vec a, U b) noexcept {
      return a * vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Multiply corresponding lanes modulo 2^(sizeof(T)*8). Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const vec operator*(U a, vec b) noexcept {
      return vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(a)) * b;
    }
    /// Bitwise AND of corresponding lane representations. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const vec operator&(vec a, U b) noexcept {
      return a & vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Bitwise AND of corresponding lane representations. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const vec operator&(U a, vec b) noexcept {
      return vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(a)) & b;
    }
    /// Bitwise OR of corresponding lane representations. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const vec operator|(vec a, U b) noexcept {
      return a | vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Bitwise OR of corresponding lane representations. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const vec operator|(U a, vec b) noexcept {
      return vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(a)) | b;
    }
    /// Bitwise XOR of corresponding lane representations. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const vec operator^(vec a, U b) noexcept {
      return a ^ vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Bitwise XOR of corresponding lane representations. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const vec operator^(U a, vec b) noexcept {
      return vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(a)) ^ b;
    }
    /// Return a mask whose lanes are true where `a == b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const mask_type operator==(vec a, U b) noexcept {
      return a == vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Return a mask whose lanes are true where `a == b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const mask_type operator==(U a, vec b) noexcept {
      return vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(a)) == b;
    }
    /// Return a mask whose lanes are true where `a != b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const mask_type operator!=(vec a, U b) noexcept {
      return a != vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Return a mask whose lanes are true where `a != b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const mask_type operator!=(U a, vec b) noexcept {
      return vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(a)) != b;
    }
    /// Return a mask whose lanes are true where `a < b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const mask_type operator<(vec a, U b) noexcept {
      return a < vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Return a mask whose lanes are true where `a < b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const mask_type operator<(U a, vec b) noexcept {
      return vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(a)) < b;
    }
    /// Return a mask whose lanes are true where `a > b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const mask_type operator>(vec a, U b) noexcept {
      return a > vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Return a mask whose lanes are true where `a > b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const mask_type operator>(U a, vec b) noexcept {
      return vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(a)) > b;
    }
    /// Return a mask whose lanes are true where `a <= b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const mask_type operator<=(vec a, U b) noexcept {
      return a <= vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Return a mask whose lanes are true where `a <= b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const mask_type operator<=(U a, vec b) noexcept {
      return vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(a)) <= b;
    }
    /// Return a mask whose lanes are true where `a >= b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const mask_type operator>=(vec a, U b) noexcept {
      return a >= vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Return a mask whose lanes are true where `a >= b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    simd_nodiscard friend simd_inline simd_const mask_type operator>=(U a, vec b) noexcept {
      return vec(SIMD_BACKEND_NAMESPACE::integer_wrap<T>(a)) >= b;
    }
    /// Apply the corresponding lane-wise add operation in place and return *this.
    simd_inline vec &operator+=(vec b) noexcept { return *this = *this + b; }
    /// Apply the corresponding lane-wise add operation in place and return *this. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U> simd_inline vec &operator+=(U b) noexcept { return *this = *this + b; }
    /// Apply the corresponding lane-wise subtract operation in place and return *this.
    simd_inline vec &operator-=(vec b) noexcept { return *this = *this - b; }
    /// Apply the corresponding lane-wise subtract operation in place and return *this. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U> simd_inline vec &operator-=(U b) noexcept { return *this = *this - b; }
    /// Apply the corresponding lane-wise multiply operation in place and return *this.
    simd_inline vec &operator*=(vec b) noexcept { return *this = *this * b; }
    /// Apply the corresponding lane-wise multiply operation in place and return *this. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U> simd_inline vec &operator*=(U b) noexcept { return *this = *this * b; }
    /// Apply the corresponding lane-wise AND operation in place and return *this.
    simd_inline vec &operator&=(vec b) noexcept { return *this = *this & b; }
    /// Apply the corresponding lane-wise AND operation in place and return *this. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U> simd_inline vec &operator&=(U b) noexcept { return *this = *this & b; }
    /// Apply the corresponding lane-wise OR operation in place and return *this.
    simd_inline vec &operator|=(vec b) noexcept { return *this = *this | b; }
    /// Apply the corresponding lane-wise OR operation in place and return *this. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U> simd_inline vec &operator|=(U b) noexcept { return *this = *this | b; }
    /// Apply the corresponding lane-wise XOR operation in place and return *this.
    simd_inline vec &operator^=(vec b) noexcept { return *this = *this ^ b; }
    /// Apply the corresponding lane-wise XOR operation in place and return *this. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U> simd_inline vec &operator^=(U b) noexcept { return *this = *this ^ b; }
    /// Shift each lane left by compile-time K, discarding high bits; require K below the lane bit width.
    template <unsigned K>
      requires(K < sizeof(T) * 8)
    simd_nodiscard simd_inline simd_const vec left() const noexcept {
      return from_native(SIMD_BACKEND_NAMESPACE::integer_left<T, K>(value));
    }
    /// Shift each lane right by compile-time K; signed lanes extend their sign. Require K below the lane bit width.
    template <unsigned K>
      requires(K < sizeof(T) * 8)
    simd_nodiscard simd_inline simd_const vec right() const noexcept {
      return from_native(SIMD_BACKEND_NAMESPACE::integer_right<T, K>(value));
    }
    /// Shift every lane left by K bits, discarding high bits. Require K smaller than the lane bit width.
    template <std::size_t K>
      requires(K < sizeof(T) * 8)
    simd_nodiscard friend simd_inline simd_const vec operator<<(vec a, imm_t<K>) noexcept {
      return a.template left<K>();
    }
    /// Shift every lane right by K; signed lanes extend the sign, unsigned lanes shift in zero. Require K smaller than the lane bit width.
    template <std::size_t K>
      requires(K < sizeof(T) * 8)
    simd_nodiscard friend simd_inline simd_const vec operator>>(vec a, imm_t<K>) noexcept {
      return a.template right<K>();
    }
    /// Read all logical lanes from an unaligned element pointer.
    simd_nodiscard simd_inline static simd_pure vec load(T const *p) noexcept {
      return SIMD_BACKEND_NAMESPACE::load_simd<T, lanes>(p);
    }
    /// Read all logical lanes without an extra alignment promise.
    simd_nodiscard simd_inline static simd_pure vec loadu(T const *p) noexcept {
      return SIMD_BACKEND_NAMESPACE::load_simd<T, lanes>(p);
    }
    /// Write all logical lanes to an unaligned element pointer.
    simd_inline void store(T *p) const noexcept { SIMD_BACKEND_NAMESPACE::store_simd(p, *this); }
    /// Write all logical lanes without an extra alignment promise.
    simd_inline void storeu(T *p) const noexcept { SIMD_BACKEND_NAMESPACE::store_simd(p, *this); }
    /// Read exactly n logical lanes and fill the remainder; require n <= lanes. For n == 0, p may be null.
    simd_nodiscard simd_inline static simd_pure vec load_partial(T const *p, std::size_t n,
                                                                  T fill = T(0)) noexcept {
      assert(n <= lanes);
      if (n == lanes) return load(p);
      if (!n) return vec(fill);
      // Native 32/64-bit lane tails must not touch masked-off addresses. This
      // also accepts byte-unaligned sources; scalar tail loads use memcpy.
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ
      if constexpr (sizeof(T) == 4 || sizeof(T) == 8) {
        auto active = std::uint64_t((std::uint64_t(1) << n) - 1);
        if constexpr (sizeof(T) * N == 64) {
          if constexpr (sizeof(T) == 4)
            return from_native(_mm512_mask_loadu_epi32(vec(fill).value, __mmask16(active), p));
          else return from_native(_mm512_mask_loadu_epi64(vec(fill).value, __mmask8(active), p));
        }
#if SIMD_HAS_AVX512VL
        else if constexpr (sizeof(T) * N == 32) {
          if constexpr (sizeof(T) == 4)
            return from_native(_mm256_mask_loadu_epi32(vec(fill).value, __mmask8(active), p));
          else return from_native(_mm256_mask_loadu_epi64(vec(fill).value, __mmask8(active), p));
        } else {
          if constexpr (sizeof(T) == 4)
            return from_native(_mm_mask_loadu_epi32(vec(fill).value, __mmask8(active), p));
          else return from_native(_mm_mask_loadu_epi64(vec(fill).value, __mmask8(active), p));
        }
#endif
      }
#endif
#if SIMD_HAS_AVX2
      if constexpr ((sizeof(T) == 4 || sizeof(T) == 8) && !mask_type::compact && sizeof(T) * N <= 32) {
        native_type active, loaded;
        if constexpr (sizeof(T) * N == 32) {
          if constexpr (sizeof(T) == 4) {
            active = _mm256_cmpgt_epi32(_mm256_set1_epi32(int(n)), _mm256_setr_epi32(0,1,2,3,4,5,6,7));
            loaded = _mm256_maskload_epi32(reinterpret_cast<int const *>(p), active);
          } else {
            active = _mm256_cmpgt_epi64(_mm256_set1_epi64x(static_cast<long long>(n)), _mm256_setr_epi64x(0,1,2,3));
            loaded = _mm256_maskload_epi64(reinterpret_cast<long long const *>(p), active);
          }
        } else if constexpr (sizeof(T) * N == 16) {
          if constexpr (sizeof(T) == 4) {
            active = _mm_cmpgt_epi32(_mm_set1_epi32(int(n)), _mm_setr_epi32(0,1,2,3));
            loaded = _mm_maskload_epi32(reinterpret_cast<int const *>(p), active);
          } else {
            active = _mm_cmpgt_epi64(_mm_set1_epi64x(static_cast<long long>(n)), _mm_set_epi64x(1,0));
            loaded = _mm_maskload_epi64(reinterpret_cast<long long const *>(p), active);
          }
        }
        return from_native(SIMD_BACKEND_NAMESPACE::integer_bit_select(active, loaded, vec(fill).value));
      }
#endif
#if SIMD_HAS_ARM_NEON
      if constexpr (sizeof(T) == 4) {
        auto result = vreinterpretq_u32_u8(vec(fill).value);
        std::uint32_t word;
        auto bytes = reinterpret_cast<unsigned char const *>(p);
        switch (n) {
          case 3: std::memcpy(&word, bytes + 8, 4); result = vsetq_lane_u32(word, result, 2); [[fallthrough]];
          case 2: std::memcpy(&word, bytes + 4, 4); result = vsetq_lane_u32(word, result, 1); [[fallthrough]];
          case 1: std::memcpy(&word, bytes, 4); result = vsetq_lane_u32(word, result, 0);
        }
        return from_native(vreinterpretq_u8_u32(result));
      } else if constexpr (sizeof(T) == 8) {
        std::uint64_t word;
        std::memcpy(&word, static_cast<void const *>(p), 8);
        return from_native(vreinterpretq_u8_u64(vsetq_lane_u64(word, vreinterpretq_u64_u8(vec(fill).value), 0)));
      }
#endif
      std::array<T, lanes> data;
      data.fill(fill);
      if (n)
        std::memcpy(data.data(), static_cast<void const *>(p), n * sizeof(T));
      return SIMD_BACKEND_NAMESPACE::load_simd<T, lanes>(data.data());
    }
    /// Write exactly n logical lanes; require n <= lanes. For n == 0, p may be null.
    simd_inline void store_partial(T *p, std::size_t n) const noexcept { SIMD_BACKEND_NAMESPACE::store_simd_partial(p, *this, n); }
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator+(vec, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator+(U, vec) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator+(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator-(vec, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator-(U, vec) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator-(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator*(vec, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator*(U, vec) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator*(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator/(vec, U) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator/(U, vec) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator/(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator%(vec, U) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator%(U, vec) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator%(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator&(vec, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator&(U, vec) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator&(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator|(vec, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator|(U, vec) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator|(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator^(vec, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator^(U, vec) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator^(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator==(vec, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator==(U, vec) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator==(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator!=(vec, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator!=(U, vec) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator!=(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator<(vec, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator<(U, vec) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator<(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator>(vec, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator>(U, vec) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator>(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator<=(vec, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator<=(U, vec) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator<=(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator>=(vec, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator>=(U, vec) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<vec, ::simd::vec<U, M,SIMD_ARCH>>)
    friend void operator>=(vec, ::simd::vec<U, M,SIMD_ARCH>) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    friend vec operator/(vec, vec) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    friend vec operator%(vec, vec) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <simd_integer_element U> friend vec operator/(vec, U) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <simd_integer_element U> friend vec operator/(U, vec) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <simd_integer_element U> friend vec operator%(vec, U) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <simd_integer_element U> friend vec operator%(U, vec) = delete;
    /// Reject runtime shift counts; use a compile-time imm<K> within the lane width.
    template <simd_integer_element U> friend vec operator<<(vec, U) = delete;
    /// Reject runtime shift counts; use a compile-time imm<K> within the lane width.
    template <simd_integer_element U> friend vec operator>>(vec, U) = delete;
  };

#endif

  namespace detail::SIMD_BACKEND {
  template <simd_integer_element T, std::size_t N, std::size_t A, simd_access Access>
    requires SIMD_BACKEND_NAMESPACE::integer_shape<T, N>
  simd_nodiscard simd_inline simd_pure vec<T, N,SIMD_ARCH> load_simd(T const *p, simd_memory<A, Access>) noexcept {
    using V = vec<T, N,SIMD_ARCH>;
    typename V::native_type value;
    // Erase the element type at the byte-copy boundary: p need not satisfy
    // alignof(T). Only an explicit alignment promise may select an aligned
    // move. Streaming currently takes this ordinary fallback.
#if defined(__GNUC__) || defined(__clang__)
    if constexpr (A > 1)
      p = static_cast<T const *>(__builtin_assume_aligned(p, A));
#elif defined(_MSC_VER)
    if constexpr (A > 1)
      __assume((reinterpret_cast<std::uintptr_t>(p) & (A - 1)) == 0);
#endif
    std::memcpy(&value, static_cast<void const *>(p), sizeof(value));
    return V::from_native(value);
  }
  template <simd_integer_element T, std::size_t N, std::size_t A, simd_access Access>
    requires SIMD_BACKEND_NAMESPACE::integer_shape<T, N>
  simd_inline void store_simd(T *p, vec<T, N,SIMD_ARCH> v, simd_memory<A, Access>) noexcept {
#if defined(__GNUC__) || defined(__clang__)
    if constexpr (A > 1)
      p = static_cast<T *>(__builtin_assume_aligned(p, A));
#elif defined(_MSC_VER)
    if constexpr (A > 1)
      __assume((reinterpret_cast<std::uintptr_t>(p) & (A - 1)) == 0);
#endif
    std::memcpy(static_cast<void *>(p), &v.value, sizeof(v.value));
  }
  template <simd_integer_element T, std::size_t N, std::size_t A, simd_access Access>
    requires SIMD_BACKEND_NAMESPACE::integer_shape<T, N>
  simd_nodiscard simd_inline simd_pure vec<T, N,SIMD_ARCH> load_simd_partial(T const *p, std::size_t count,
                                                                    simd_memory<A, Access>) noexcept {
    assert(count <= N);
    std::array<T, N> data{};
    if (count)
      std::memcpy(data.data(), static_cast<void const *>(p), count * sizeof(T));
    return SIMD_BACKEND_NAMESPACE::load_simd<T, N>(data.data());
  }
  template <simd_integer_element T, std::size_t N, std::size_t A, simd_access Access>
    requires SIMD_BACKEND_NAMESPACE::integer_shape<T, N>
  simd_inline void store_simd_partial(T *p, vec<T, N,SIMD_ARCH> v, std::size_t count,
                                      simd_memory<A, Access>) noexcept {
    assert(count <= N);
    std::array<T, N> data;
    store_simd(data.data(), v);
    if (count)
      std::memcpy(static_cast<void *>(p), data.data(), count * sizeof(T));
  }
  template <simd_integer_element T, std::size_t N>
    requires SIMD_BACKEND_NAMESPACE::integer_shape<T, N>
  simd_nodiscard simd_inline simd_pure vec<T, N,SIMD_ARCH> load_simd(std::array<T, N> const &p) noexcept {
    return SIMD_BACKEND_NAMESPACE::load_simd<T, N>(p.data());
  }
  template <simd_integer_element T, std::size_t N>
    requires SIMD_BACKEND_NAMESPACE::integer_shape<T, N>
  simd_nodiscard simd_inline simd_pure vec<T, N,SIMD_ARCH> load_simd(std::span<T const, N> p) noexcept {
    return SIMD_BACKEND_NAMESPACE::load_simd<T, N>(p.data());
  }


  }
  namespace detail::SIMD_BACKEND {
  template <class M, class T, std::size_t N>
  concept integer_mask_for =
      std::same_as<M, typename vec<T, N,SIMD_ARCH>::mask_type> || std::same_as<M, vec<SIMD_BACKEND_NAMESPACE::mask_lane_for<T>, N,SIMD_ARCH>>;
  }
  /// \ingroup masks
  /// Choose a in true lanes and b in false lanes; both operands are evaluated.
  template <simd_integer_element T, std::size_t N, class M>
    requires(SIMD_BACKEND_NAMESPACE::integer_shape<T, N> && SIMD_BACKEND_NAMESPACE::integer_mask_for<M, T, N>)
  simd_nodiscard simd_inline simd_const vec<T, N,SIMD_ARCH> select(M m, vec<T, N,SIMD_ARCH> a, vec<T, N,SIMD_ARCH> b) noexcept {
    if constexpr (N == 1)
      return any(m) ? a : b;
#if SIMD_HAS_AVX2 || SIMD_HAS_ARM_NEON || (SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ)
    else
      return vec<T, N,SIMD_ARCH>::from_native(SIMD_BACKEND_NAMESPACE::integer_select<T>(m, a.value, b.value));
#endif
  }
  /// \ingroup masks
  /// Select individual bits: (bits & a) | (~bits & b); no mask canonicalization.
  template <simd_integer_element T, std::size_t N>
    requires SIMD_BACKEND_NAMESPACE::integer_shape<T, N>
  simd_nodiscard simd_inline simd_const vec<T, N,SIMD_ARCH> bit_select(vec<T, N,SIMD_ARCH> bits, vec<T, N,SIMD_ARCH> a,
                                                              vec<T, N,SIMD_ARCH> b) noexcept {
    if constexpr(N==1) return (bits & a) | (~bits & b);
#if SIMD_HAS_AVX2 || SIMD_HAS_ARM_NEON || (SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ)
    else return vec<T,N,SIMD_ARCH>::from_native(SIMD_BACKEND_NAMESPACE::integer_bit_select(bits.value,a.value,b.value));
#endif
  }
  // Compatibility: numeric masks select individual bits, never lane truth values.
  /// \ingroup masks
  /// Choose a in true lanes and b in false lanes; both operands are evaluated.
  template <simd_integer_element T, std::size_t N>
    requires SIMD_BACKEND_NAMESPACE::integer_shape<T, N>
  simd_nodiscard simd_inline simd_const vec<T, N,SIMD_ARCH> select(vec<T, N,SIMD_ARCH> bits, vec<T, N,SIMD_ARCH> a,
                                                          vec<T, N,SIMD_ARCH> b) noexcept {
    return bit_select(bits, a, b);
  }
  /// \ingroup masks
  /// Expand lane truth into unsigned integer zero/all-one words; preserve the lane count.
  template <simd_mask_element M, std::size_t N>
  simd_nodiscard simd_inline simd_const auto mask_bits(vec<M, N,SIMD_ARCH> m) noexcept {
    using U = typename M::storage_type;
    return vec<U, N,SIMD_ARCH>::from_native(m.to_native());
  }
  /// \ingroup masks
  /// Expand lane truth into unsigned integer zero/all-one words; preserve the lane count.
  template <simd_integer_element T, simd_mask_element M, std::size_t N>
    requires(sizeof(T) == sizeof(M))
  simd_nodiscard simd_inline simd_const vec<std::make_unsigned_t<T>, N,SIMD_ARCH> mask_bits(vec<M, N,SIMD_ARCH> m) noexcept {
    return mask_bits(m);
  }
  /// \ingroup masks
  /// Expand lane truth into unsigned integer zero/all-one words; preserve the lane count.
  template <simd_integer_element T, std::size_t N>
  simd_nodiscard simd_inline simd_const vec<std::make_unsigned_t<T>, N,SIMD_ARCH> mask_bits(predicate<N,SIMD_ARCH> m) noexcept {
    return mask_bits(to_vector_mask<SIMD_BACKEND_NAMESPACE::mask_lane_for<T>>(m));
  }
  /// \ingroup masks
  /// Add modulo the lane width in active lanes, retaining prior elsewhere.
  template <simd_integer_element T, std::size_t N, class M>
    requires(SIMD_BACKEND_NAMESPACE::integer_shape<T, N> && SIMD_BACKEND_NAMESPACE::integer_mask_for<M, T, N>)
  simd_nodiscard simd_inline simd_const vec<T, N,SIMD_ARCH> masked_add(M m, vec<T, N,SIMD_ARCH> prior, vec<T, N,SIMD_ARCH> a,
                                                              vec<T, N,SIMD_ARCH> b) noexcept {
    if constexpr (N == 1)
      return select(m, a + b, prior);
#if SIMD_HAS_AVX2 || SIMD_HAS_ARM_NEON || (SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ)
    else
      return vec<T, N,SIMD_ARCH>::from_native(SIMD_BACKEND_NAMESPACE::integer_masked_add<T>(m, prior.value, a.value, b.value));
#endif
  }
  /// \ingroup masks
  /// Add modulo the lane width in active lanes and zero inactive lanes.
  template <simd_integer_element T, std::size_t N, class M>
    requires(SIMD_BACKEND_NAMESPACE::integer_shape<T, N> && SIMD_BACKEND_NAMESPACE::integer_mask_for<M, T, N>)
  simd_nodiscard simd_inline simd_const vec<T, N,SIMD_ARCH> masked_add_zero(M m, vec<T, N,SIMD_ARCH> a, vec<T, N,SIMD_ARCH> b) noexcept {
    return masked_add(m, vec<T, N,SIMD_ARCH>(T(0)), a, b);
  }
  /// \ingroup masks
  /// Subtract modulo the lane width in active lanes, retaining prior elsewhere.
  template <simd_integer_element T, std::size_t N, class M>
    requires(SIMD_BACKEND_NAMESPACE::integer_shape<T, N> && SIMD_BACKEND_NAMESPACE::integer_mask_for<M, T, N>)
  simd_nodiscard simd_inline simd_const vec<T, N,SIMD_ARCH> masked_sub(M m, vec<T, N,SIMD_ARCH> prior, vec<T, N,SIMD_ARCH> a,
                                                              vec<T, N,SIMD_ARCH> b) noexcept {
    if constexpr (N == 1)
      return select(m, a - b, prior);
#if SIMD_HAS_AVX2 || SIMD_HAS_ARM_NEON || (SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ)
    else
      return vec<T, N,SIMD_ARCH>::from_native(SIMD_BACKEND_NAMESPACE::integer_masked_sub<T>(m, prior.value, a.value, b.value));
#endif
  }
  /// \ingroup masks
  /// Subtract modulo the lane width in active lanes and zero inactive lanes.
  template <simd_integer_element T, std::size_t N, class M>
    requires(SIMD_BACKEND_NAMESPACE::integer_shape<T, N> && SIMD_BACKEND_NAMESPACE::integer_mask_for<M, T, N>)
  simd_nodiscard simd_inline simd_const vec<T, N,SIMD_ARCH> masked_sub_zero(M m, vec<T, N,SIMD_ARCH> a, vec<T, N,SIMD_ARCH> b) noexcept {
    return masked_sub(m, vec<T, N,SIMD_ARCH>(T(0)), a, b);
  }
  /// \ingroup masks
  /// Multiply modulo the lane width in active lanes, retaining prior elsewhere.
  template <simd_integer_element T, std::size_t N, class M>
    requires(SIMD_BACKEND_NAMESPACE::integer_shape<T, N> && SIMD_BACKEND_NAMESPACE::integer_mask_for<M, T, N>)
  simd_nodiscard simd_inline simd_const vec<T, N,SIMD_ARCH> masked_mul(M m, vec<T, N,SIMD_ARCH> prior, vec<T, N,SIMD_ARCH> a,
                                                              vec<T, N,SIMD_ARCH> b) noexcept {
    if constexpr (N == 1)
      return select(m, a * b, prior);
#if SIMD_HAS_AVX2 || SIMD_HAS_ARM_NEON || (SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ)
    else
      return vec<T, N,SIMD_ARCH>::from_native(SIMD_BACKEND_NAMESPACE::integer_masked_mul<T>(m, prior.value, a.value, b.value));
#endif
  }
  /// \ingroup masks
  /// Multiply modulo the lane width in active lanes and zero inactive lanes.
  template <simd_integer_element T, std::size_t N, class M>
    requires(SIMD_BACKEND_NAMESPACE::integer_shape<T, N> && SIMD_BACKEND_NAMESPACE::integer_mask_for<M, T, N>)
  simd_nodiscard simd_inline simd_const vec<T, N,SIMD_ARCH> masked_mul_zero(M m, vec<T, N,SIMD_ARCH> a, vec<T, N,SIMD_ARCH> b) noexcept {
    return masked_mul(m, vec<T, N,SIMD_ARCH>(T(0)), a, b);
  }
} // namespace simd

// Reject invalid immediate widths before implicit native conversion can select
// a builtin scalar shift. The immediate tag retains its public size conversion.
namespace simd {
  /// Reject this unsupported operand combination instead of converting implicitly to a native register.
  template<simd_integer_element T, std::size_t N, std::size_t K>
    requires (K >= sizeof(T) * 8)
  void operator<<(vec<T, N,SIMD_ARCH>, imm_t<K>) = delete;
  /// Reject this unsupported operand combination instead of converting implicitly to a native register.
  template<simd_integer_element T, std::size_t N, std::size_t K>
    requires (K >= sizeof(T) * 8)
  void operator>>(vec<T, N,SIMD_ARCH>, imm_t<K>) = delete;
}

namespace SIMD_BACKEND_NAMESPACE::native {
  // Compatibility names point toward the public native class template.
  using uint32x1=::simd::vec<::simd::uint32_t,1,SIMD_ARCH>;
#if SIMD_HAS_AVX2 || SIMD_HAS_ARM_NEON
  using uint32x4=::simd::vec<::simd::uint32_t,4,SIMD_ARCH>;
#endif
#if SIMD_HAS_AVX2
  using uint32x8=::simd::vec<::simd::uint32_t,8,SIMD_ARCH>;
#endif
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ
  using uint32x16=::simd::vec<::simd::uint32_t,16,SIMD_ARCH>;
#endif
  template<class V> concept unsigned_register = requires {
    typename V::unsigned_register_tag;
    typename V::value_type;
  } && std::same_as<typename V::value_type,::simd::uint32_t>;
  template<unsigned S,unsigned_register V> requires(S<32)
  simd_nodiscard simd_inline simd_const V shift_left(V x) noexcept { return x.template left<S>(); }
  template<unsigned S,unsigned_register V> requires(S<32)
  simd_nodiscard simd_inline simd_const V shift_right(V x) noexcept { return x.template right<S>(); }
  using ::simd::mask_bits;
  using ::simd::bit_select;
  struct integer_target {
#if defined(SIMD_INTEGER_SCALAR)
    using unsigned_type=uint32x1;
#elif SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ
    using unsigned_type=uint32x16;
#elif SIMD_HAS_AVX2
    using unsigned_type=uint32x8;
#elif SIMD_HAS_ARM_NEON
    using unsigned_type=uint32x4;
#else
    using unsigned_type=uint32x1;
#endif
    static constexpr std::size_t lanes=unsigned_type::lanes;
  };
}

#include <cmath>
#if defined(SIMD_AVX2) && !SIMD_HAS_AVX2
#error SIMD_AVX2 requires compiler AVX2 flags
#endif
#if (defined(SIMD_AVX512) || SIMD_HAS_AVX512F) && !(SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ)
#error SIMD_AVX512 requires compiler AVX512F and AVX512DQ flags
#endif
#if SIMD_HAS_AVX2 && !defined(_MSC_VER) && !defined(__FMA__)
#error AVX2 register operations require -mfma as well as -mavx2
#endif
namespace simd {
  namespace detail::SIMD_BACKEND {
    template <class V,std::size_t Alignment> simd_nodiscard simd_artificial simd_inline simd_pure V simd_load_native(simd_noescape float const *) noexcept;
    template <class V,std::size_t Alignment> simd_artificial simd_inline void simd_store_native(simd_noescape float *,V) noexcept;
  }
  namespace detail::SIMD_BACKEND {
    template <class T> inline constexpr bool custom_argument = ::simd::simd_custom_element<std::remove_cvref_t<T>>;
    template <class T, std::size_t N> inline constexpr bool custom_argument<vec<T,N,SIMD_ARCH>> = ::simd::simd_custom_element<T>;
  }
  namespace detail::SIMD_BACKEND {

  using ::simd::detail::register_memory;
  }
  template <> struct simd_empty_bases vec<float, 1,SIMD_ARCH> : SIMD_BACKEND_NAMESPACE::register_memory<vec<float,1,SIMD_ARCH>, 1>, detail::swizzle_access<float,1,SIMD_ARCH> {
    using architecture = SIMD_ARCH;
    /// Select this architecture and forward arguments to the corresponding constructor.
    /// Exception behavior is exactly that of the forwarded construction.
    template<class... X> requires std::constructible_from<vec,X...>
    simd_inline constexpr vec(SIMD_ARCH, X &&... x)
        noexcept(std::is_nothrow_constructible_v<vec,X...>) : vec(std::forward<X>(x)...) {}
    template <class T> using rebind = vec<T,1,SIMD_ARCH>;
    using vector_mask_type=vec<mask32,1,SIMD_ARCH>;
    using mask_type=SIMD_BACKEND_NAMESPACE::comparison_mask<float,1>;
    using mask = mask_type;
    using predicate_type = predicate<1,SIMD_ARCH>;
    float value{};
    /// Initialize the stored lane values to zero.
    simd_inline vec() = default;
    /// Broadcast the supplied value to each logical lane.
    simd_inline constexpr vec(float x) : value(x) {}
    /// Read all logical lanes from an unaligned element pointer.
    simd_nodiscard static simd_inline simd_pure vec load(simd_noescape float const * p) { return SIMD_BACKEND_NAMESPACE::simd_load_native<vec,1>(p); }
    /// Write all logical lanes to an unaligned element pointer.
    simd_inline void store(simd_noescape float * p) const { SIMD_BACKEND_NAMESPACE::simd_store_native<vec,1>(p,*this); }
    /// Add corresponding floating-point lanes using the caller's rounding and denormal environment.
    simd_nodiscard friend simd_inline simd_pure vec operator+(vec a, vec b) { return vec(a.value + b.value); }
    /// Subtract corresponding floating-point lanes using the caller's rounding and denormal environment.
    simd_nodiscard friend simd_inline simd_pure vec operator-(vec a, vec b) { return vec(a.value - b.value); }
    /// Multiply corresponding floating-point lanes using the caller's rounding and denormal environment.
    simd_nodiscard friend simd_inline simd_pure vec operator*(vec a, vec b) { return vec(a.value * b.value); }
    /// Divide corresponding floating-point lanes using the caller's rounding and denormal environment.
    simd_nodiscard friend simd_inline simd_pure vec operator/(vec a, vec b) { return vec(a.value / b.value); }
    /// Negate every logical lane; floating-point lanes change sign.
    simd_nodiscard friend simd_inline simd_const vec operator-(vec a) { return vec(-a.value); }
    /// Return a mask whose lanes are true where `a < b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator<(vec a, vec b) { return mask_type::from_native(a.value < b.value ? ~std::uint32_t(0) : 0u); }
    /// Return a mask whose lanes are true where `a > b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator>(vec a, vec b) { return mask_type::from_native(a.value > b.value ? ~std::uint32_t(0) : 0u); }
    /// Return a mask whose lanes are true where `a == b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator==(vec a, vec b) { return mask_type::from_native(a.value == b.value ? ~std::uint32_t(0) : 0u); }
    /// Choose a in true mask lanes and b in false lanes; both values are already evaluated.
    template<class M> requires (std::same_as<M,mask_type> || std::same_as<M,vector_mask_type>)
    simd_nodiscard friend simd_inline simd_const vec select(M m,vec a,vec b) { return m.to_native()!=0 ? a : b; }
    /// Compute a*b+c with one fused rounding per lane.
    simd_nodiscard friend simd_inline vec fma(vec a, vec b, vec c) { return vec(std::fma(a.value, b.value, c.value)); }
    /// Take the native square root of each lane.
    simd_nodiscard friend simd_inline vec sqrt(vec a) { return vec(std::sqrt(a.value)); }
    /// Round each lane to nearest integral value, choosing even at a tie.
    simd_nodiscard friend simd_inline simd_pure vec round_even(vec a) {
      if (!std::isfinite(a.value) || std::abs(a.value) >= 0x1p23f) return a;
      float lo = std::floor(a.value), delta = a.value - lo;
      float r = lo + float(delta > .5f || (delta == .5f && std::fmod(lo, 2.f) != 0));
      return vec(r == 0 ? std::copysign(0.f, a.value) : r);
    }
    // Internal precondition: integral n in [-126,127].
    /// Construct normal powers of two; require integral exponents in [-126,127].
    simd_nodiscard friend simd_inline simd_const vec normal_pow2(vec n) { return vec(std::bit_cast<float>(std::uint32_t(int(n.value) + 127) << 23)); }

    template <std::size_t Alignment = 1>
    /// Load logical lanes, assuming the template alignment in bytes.
    simd_nodiscard static simd_inline simd_pure vec load_memory(float const * p) noexcept {
      return SIMD_BACKEND_NAMESPACE::simd_load_native<vec,Alignment>(p);
    }
    template <std::size_t Alignment = 1>
    /// Store logical lanes, assuming the template alignment in bytes.
    simd_inline void store_memory(float * p) const noexcept {
      SIMD_BACKEND_NAMESPACE::simd_store_native<vec,Alignment>(p,*this);
    }
    using value_type = float;
    using register_type = vec;
    using native_type = float;
    using bits_type = vec<uint32_t,1,SIMD_ARCH>;
    /// Return the native storage value without a numerical conversion.
    simd_nodiscard simd_inline simd_pure operator native_type() const noexcept { return value; }
    /// Return the native storage representation.
    simd_nodiscard simd_inline simd_pure native_type to_native() const noexcept { return value; }
    /// Project the exact lane representation into the corresponding unsigned vector.
    simd_nodiscard simd_inline simd_pure bits_type bits() const noexcept { return bits_type(std::bit_cast<std::uint32_t>(value)); }
    /// Return the exact binary32 lane representations in the unsigned vector.
    simd_nodiscard simd_inline simd_pure bits_type to_bits() const noexcept { return bits(); }
    /// Reinterpret binary words as lane values; do not normalize them.
    simd_nodiscard static simd_inline simd_const vec from_bits(bits_type bits) noexcept { return vec(std::bit_cast<float>(bits.value)); }
    /// Reinterpret binary words as lane values; do not normalize them.
    simd_nodiscard static simd_inline simd_const vec from_bits(std::uint32_t bits) noexcept { return from_bits(bits_type(bits)); }
    /// Broadcast the float value without adding an FTZ or other normalization policy.
    simd_nodiscard static simd_inline simd_const vec from_float(float x) noexcept { return vec(x); }
    /// Adopt native storage without numerical conversion.
    simd_nodiscard static simd_inline simd_const vec from_native(native_type x) noexcept { return vec(x); }
    /// Adopt raw float storage without numerical conversion or normalization.
    simd_nodiscard static simd_inline simd_const vec unsafe_from_float32(native_type x) noexcept { return vec(x); }
    /// Read all logical lanes without an extra alignment promise.
    simd_nodiscard static simd_inline simd_pure vec loadu(simd_noescape float const * p) { return SIMD_BACKEND_NAMESPACE::simd_load_native<vec,1>(p); }
    /// Write all logical lanes without an extra alignment promise.
    simd_inline void storeu(simd_noescape float * p) const { SIMD_BACKEND_NAMESPACE::simd_store_native<vec,1>(p,*this); }
    /// Load exactly the logical count of binary32 words without normalizing their representations.
    simd_nodiscard static simd_inline simd_pure vec load_bits(simd_noescape std::uint32_t const * p) noexcept { return from_bits(bits_type::load(p)); }
    /// Store the exact binary32 words for every logical lane.
    simd_inline void store_bits(simd_noescape std::uint32_t * p) const noexcept { bits().store(p); }
    /// Read n representation words and fill the remaining logical lanes; require n <= lanes. A zero count permits null.
    simd_nodiscard static simd_inline simd_pure vec load_bits_partial(simd_noescape std::uint32_t const * p,std::size_t n,std::uint32_t fill=0) noexcept { return from_bits(bits_type::load_partial(p,n,fill)); }
    /// Write n exact representation words; require n <= lanes. A zero count permits null.
    simd_inline void store_bits_partial(simd_noescape std::uint32_t * p,std::size_t n) const noexcept { bits().store_partial(p,n); }
    /// Read all logical lanes without an extra alignment promise.
    simd_inline vec(std::array<float,1> const & values) noexcept : vec(loadu(values.data())) {}
    /// Apply the corresponding lane-wise add operation in place and return *this.
    simd_inline vec & operator+=(vec b) noexcept { return *this=*this+b; }
    /// Apply the corresponding lane-wise subtract operation in place and return *this.
    simd_inline vec & operator-=(vec b) noexcept { return *this=*this-b; }
    /// Apply the corresponding lane-wise multiply operation in place and return *this.
    simd_inline vec & operator*=(vec b) noexcept { return *this=*this*b; }
    /// Apply the corresponding lane-wise divide operation in place and return *this.
    simd_inline vec & operator/=(vec b) noexcept { return *this=*this/b; }
    /// Return a mask whose lanes are true where `a != b` holds. NaN lanes compare unequal.
    simd_nodiscard friend simd_inline simd_const mask_type operator!=(vec a,vec b) noexcept { return ~(a==b); }
    /// Return a mask whose lanes are true where `a <= b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator<=(vec a,vec b) noexcept { return (a<b)|(a==b); }
    /// Return a mask whose lanes are true where `a >= b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator>=(vec a,vec b) noexcept { return (a>b)|(a==b); }
  };
#if SIMD_HAS_AVX2
}
#include "simd/simd/x86.h"
namespace simd {
#endif
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ
  template <> struct vec<float, 16,SIMD_ARCH> : SIMD_BACKEND_NAMESPACE::register_memory<vec<float,16,SIMD_ARCH>, 16> {
    using architecture = SIMD_ARCH;
    /// Select this architecture and forward arguments to the corresponding constructor.
    /// Exception behavior is exactly that of the forwarded construction.
    template<class... X> requires std::constructible_from<vec,X...>
    simd_inline constexpr vec(SIMD_ARCH, X &&... x)
        noexcept(std::is_nothrow_constructible_v<vec,X...>) : vec(std::forward<X>(x)...) {}
    template <class T> using rebind = vec<T,16,SIMD_ARCH>;
    using vector_mask_type=vec<mask32,16,SIMD_ARCH>;
    using mask_type=SIMD_BACKEND_NAMESPACE::comparison_mask<float,16>;
    using mask = mask_type;
    using predicate_type = predicate<16,SIMD_ARCH>;
    __m512 value;
    /// Default initialization leaves storage unspecified; value initialization with braces zero-initializes it.
    simd_inline vec() = default;
    /// Copy the stored value without arithmetic or normalization.
    simd_inline constexpr vec(vec const &) = default;
    /// Copy the stored value and return *this; no numerical conversion is performed.
    simd_reinitializes simd_inline constexpr vec & operator=(vec const &) = default;
    /// Broadcast the supplied value to each logical lane.
    simd_inline vec(float x) : value(_mm512_set1_ps(x)) {}
    /// Adopt native lane storage without numerical conversion.
    simd_inline constexpr vec(__m512 x) : value(x) {}
    /// Read all logical lanes from an unaligned element pointer.
    simd_nodiscard static simd_inline simd_pure vec load(simd_noescape float const * p) { return SIMD_BACKEND_NAMESPACE::simd_load_native<vec,1>(p); }
    /// Write all logical lanes to an unaligned element pointer.
    simd_inline void store(simd_noescape float * p) const { SIMD_BACKEND_NAMESPACE::simd_store_native<vec,1>(p,*this); }
    /// Add corresponding floating-point lanes using the caller's rounding and denormal environment.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec operator+(vec a, vec b) { return vec(_mm512_add_ps(a.value, b.value)); }
    /// Subtract corresponding floating-point lanes using the caller's rounding and denormal environment.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec operator-(vec a, vec b) { return vec(_mm512_sub_ps(a.value, b.value)); }
    /// Multiply corresponding floating-point lanes using the caller's rounding and denormal environment.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec operator*(vec a, vec b) { return vec(_mm512_mul_ps(a.value, b.value)); }
    /// Divide corresponding floating-point lanes using the caller's rounding and denormal environment.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec operator/(vec a, vec b) { return vec(_mm512_div_ps(a.value, b.value)); }
    /// Negate every logical lane; floating-point lanes change sign.
    simd_nodiscard friend simd_inline simd_const vec operator-(vec a) { return vec(_mm512_xor_ps(a.value, _mm512_set1_ps(-0.f))); }
    /// Return a mask whose lanes are true where `a < b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator<(vec a, vec b) { return mask_type::from_native(_mm512_cmp_ps_mask(a.value,b.value,_CMP_LT_OQ)); }
    /// Return a mask whose lanes are true where `a > b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator>(vec a, vec b) { return mask_type::from_native(_mm512_cmp_ps_mask(a.value,b.value,_CMP_GT_OQ)); }
    /// Return a mask whose lanes are true where `a == b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator==(vec a, vec b) { return mask_type::from_native(_mm512_cmp_ps_mask(a.value,b.value,_CMP_EQ_OQ)); }
    /// Choose a in true lanes and b in false lanes; both values are already evaluated.
    template<class M> requires (std::same_as<M,mask_type> || std::same_as<M,vector_mask_type>)
    simd_nodiscard friend simd_inline simd_const vec select(M m,vec a,vec b) {
      if constexpr(M::compact) return vec(_mm512_mask_blend_ps(m.to_native(),b.value,a.value));
      else return vec(_mm512_castsi512_ps(_mm512_or_si512(_mm512_and_si512(m.to_native(),_mm512_castps_si512(a.value)),
        _mm512_andnot_si512(m.to_native(),_mm512_castps_si512(b.value))))); }
    /// Compute a*b+c with one fused rounding per lane.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec fma(vec a, vec b, vec c) { return vec(_mm512_fmadd_ps(a.value, b.value, c.value)); }
    /// Take the native square root of each lane.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec sqrt(vec a) { return vec(_mm512_sqrt_ps(a.value)); }
    /// Round each lane to nearest integral value, choosing even at a tie.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec round_even(vec a) { return vec(_mm512_roundscale_ps(a.value, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC)); }
    /// Construct normal powers of two; require integral exponents in [-126,127].
    simd_nodiscard friend simd_inline simd_const vec normal_pow2(vec n) { return vec(_mm512_castsi512_ps(_mm512_slli_epi32(_mm512_add_epi32(_mm512_cvttps_epi32(n.value), _mm512_set1_epi32(127)), 23))); }

    template <std::size_t Alignment = 1>
    /// Load logical lanes, assuming the template alignment in bytes.
    simd_nodiscard static simd_inline simd_pure vec load_memory(float const * p) noexcept {
      return SIMD_BACKEND_NAMESPACE::simd_load_native<vec,Alignment>(p);
    }
    template <std::size_t Alignment = 1>
    /// Store logical lanes, assuming the template alignment in bytes.
    simd_inline void store_memory(float * p) const noexcept {
      SIMD_BACKEND_NAMESPACE::simd_store_native<vec,Alignment>(p,*this);
    }
    using value_type = float;
    using register_type = vec;
    using native_type = __m512;
    using bits_type = vec<uint32_t,16,SIMD_ARCH>;
    /// Return the native storage value without a numerical conversion.
    simd_nodiscard simd_inline simd_pure operator native_type() const noexcept { return value; }
    /// Return the native storage representation.
    simd_nodiscard simd_inline simd_pure native_type to_native() const noexcept { return value; }
    /// Project the exact lane representation into the corresponding unsigned vector.
    simd_artificial simd_nodiscard simd_inline simd_pure bits_type bits() const noexcept { return bits_type::from_native(_mm512_castps_si512(value)); }
    /// Return the exact binary32 lane representations in the unsigned vector.
    simd_nodiscard simd_inline simd_pure bits_type to_bits() const noexcept { return bits(); }
    /// Reinterpret binary words as lane values; do not normalize them.
    simd_artificial simd_nodiscard static simd_inline simd_const vec from_bits(bits_type bits) noexcept { return vec(_mm512_castsi512_ps(bits.value)); }
    /// Reinterpret binary words as lane values; do not normalize them.
    simd_nodiscard static simd_inline simd_const vec from_bits(std::uint32_t bits) noexcept { return from_bits(bits_type(bits)); }
    /// Broadcast the float value without adding an FTZ or other normalization policy.
    simd_nodiscard static simd_inline simd_const vec from_float(float x) noexcept { return vec(x); }
    /// Adopt native storage without numerical conversion.
    simd_nodiscard static simd_inline simd_const vec from_native(native_type x) noexcept { return vec(x); }
    /// Adopt raw float storage without numerical conversion or normalization.
    simd_nodiscard static simd_inline simd_const vec unsafe_from_float32(native_type x) noexcept { return vec(x); }
    /// Read all logical lanes without an extra alignment promise.
    simd_nodiscard static simd_inline simd_pure vec loadu(simd_noescape float const * p) { return SIMD_BACKEND_NAMESPACE::simd_load_native<vec,1>(p); }
    /// Write all logical lanes without an extra alignment promise.
    simd_inline void storeu(simd_noescape float * p) const { SIMD_BACKEND_NAMESPACE::simd_store_native<vec,1>(p,*this); }
    /// Load exactly the logical count of binary32 words without normalizing their representations.
    simd_nodiscard static simd_inline simd_pure vec load_bits(simd_noescape std::uint32_t const * p) noexcept { return from_bits(bits_type::load(p)); }
    /// Store the exact binary32 words for every logical lane.
    simd_inline void store_bits(simd_noescape std::uint32_t * p) const noexcept { bits().store(p); }
    /// Read n representation words and fill the remaining logical lanes; require n <= lanes. A zero count permits null.
    simd_nodiscard static simd_inline simd_pure vec load_bits_partial(simd_noescape std::uint32_t const * p,std::size_t n,std::uint32_t fill=0) noexcept { return from_bits(bits_type::load_partial(p,n,fill)); }
    /// Write n exact representation words; require n <= lanes. A zero count permits null.
    simd_inline void store_bits_partial(simd_noescape std::uint32_t * p,std::size_t n) const noexcept { bits().store_partial(p,n); }
    /// Read all logical lanes without an extra alignment promise.
    simd_inline vec(std::array<float,16> const & values) noexcept : vec(loadu(values.data())) {}
#if defined(__clang__)
    /// Construct logical lanes in argument order. Any element conversions determine the exception specification.
    template <class... X> requires (sizeof...(X)==16) && (std::convertible_to<X,float> && ...)
    simd_inline constexpr vec(X... x) noexcept((noexcept(static_cast<float>(x)) && ...)) : value{static_cast<float>(x)...} {}
#else
    /// Construct logical lanes in argument order. Any element conversions determine the exception specification.
    template <class... X> requires (sizeof...(X)==16) && (std::convertible_to<X,float> && ...)
    simd_inline vec(X... x) noexcept((noexcept(static_cast<float>(x)) && ...)) : vec(loadu(std::array<float,16>{static_cast<float>(x)...}.data())) {}
#endif
    /// Apply the corresponding lane-wise add operation in place and return *this.
    simd_inline vec & operator+=(vec b) noexcept { return *this=*this+b; }
    /// Apply the corresponding lane-wise subtract operation in place and return *this.
    simd_inline vec & operator-=(vec b) noexcept { return *this=*this-b; }
    /// Apply the corresponding lane-wise multiply operation in place and return *this.
    simd_inline vec & operator*=(vec b) noexcept { return *this=*this*b; }
    /// Apply the corresponding lane-wise divide operation in place and return *this.
    simd_inline vec & operator/=(vec b) noexcept { return *this=*this/b; }
    /// Return a mask whose lanes are true where `a != b` holds. NaN lanes compare unequal.
    simd_nodiscard friend simd_inline simd_const mask_type operator!=(vec a,vec b) noexcept { return ~(a==b); }
    /// Return a mask whose lanes are true where `a <= b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator<=(vec a,vec b) noexcept { return (a<b)|(a==b); }
    /// Return a mask whose lanes are true where `a >= b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator>=(vec a,vec b) noexcept { return (a>b)|(a==b); }
  };
#endif
#if SIMD_HAS_ARM_NEON
  template <> struct simd_empty_bases vec<float, 4,SIMD_ARCH> : SIMD_BACKEND_NAMESPACE::register_memory<vec<float,4,SIMD_ARCH>, 4>, detail::swizzle_access<float,4,SIMD_ARCH> {
    using architecture = SIMD_ARCH;
    /// Select this architecture and forward arguments to the corresponding constructor.
    /// Exception behavior is exactly that of the forwarded construction.
    template<class... X> requires std::constructible_from<vec,X...>
    simd_inline constexpr vec(SIMD_ARCH, X &&... x)
        noexcept(std::is_nothrow_constructible_v<vec,X...>) : vec(std::forward<X>(x)...) {}
    template <class T> using rebind = vec<T,4,SIMD_ARCH>;
    using vector_mask_type=vec<mask32,4,SIMD_ARCH>;
    using mask_type=SIMD_BACKEND_NAMESPACE::comparison_mask<float,4>;
    using mask = mask_type;
    using predicate_type = predicate<4,SIMD_ARCH>;
    float32x4_t value;
    /// Default initialization leaves storage unspecified; value initialization with braces zero-initializes it.
    simd_inline vec() = default;
    /// Copy the stored value without arithmetic or normalization.
    simd_inline constexpr vec(vec const &) = default;
    /// Copy the stored value and return *this; no numerical conversion is performed.
    simd_reinitializes simd_inline constexpr vec & operator=(vec const &) = default;
    /// Broadcast the supplied value to each logical lane.
    simd_inline vec(float x) : value(vdupq_n_f32(x)) {}
    /// Adopt native lane storage without numerical conversion.
    simd_inline constexpr vec(float32x4_t x) : value(x) {}
    /// Read all logical lanes from an unaligned element pointer.
    simd_nodiscard static simd_inline simd_pure vec load(simd_noescape float const * p) { return SIMD_BACKEND_NAMESPACE::simd_load_native<vec,1>(p); }
    /// Write all logical lanes to an unaligned element pointer.
    simd_inline void store(simd_noescape float * p) const { SIMD_BACKEND_NAMESPACE::simd_store_native<vec,1>(p,*this); }
    /// Add corresponding floating-point lanes using the caller's rounding and denormal environment.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec operator+(vec a, vec b) { return vec(vaddq_f32(a.value, b.value)); }
    /// Subtract corresponding floating-point lanes using the caller's rounding and denormal environment.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec operator-(vec a, vec b) { return vec(vsubq_f32(a.value, b.value)); }
    /// Multiply corresponding floating-point lanes using the caller's rounding and denormal environment.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec operator*(vec a, vec b) { return vec(vmulq_f32(a.value, b.value)); }
    /// Divide corresponding floating-point lanes using the caller's rounding and denormal environment.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec operator/(vec a, vec b) { return vec(vdivq_f32(a.value, b.value)); }
    /// Negate every logical lane; floating-point lanes change sign.
    simd_artificial simd_nodiscard friend simd_inline simd_const vec operator-(vec a) { return vec(vnegq_f32(a.value)); }
    /// Return a mask whose lanes are true where `a < b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator<(vec a, vec b) { return mask_type::unsafe_from_native(vreinterpretq_u8_u32(vcltq_f32(a.value,b.value))); }
    /// Return a mask whose lanes are true where `a > b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator>(vec a, vec b) { return mask_type::unsafe_from_native(vreinterpretq_u8_u32(vcgtq_f32(a.value,b.value))); }
    /// Return a mask whose lanes are true where `a == b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator==(vec a, vec b) { return mask_type::unsafe_from_native(vreinterpretq_u8_u32(vceqq_f32(a.value,b.value))); }
    /// Choose a in true mask lanes and b in false lanes; both values are already evaluated.
    template<class M> requires (std::same_as<M,mask_type> || std::same_as<M,vector_mask_type>)
    simd_nodiscard friend simd_inline simd_const vec select(M m,vec a,vec b) { return vec(vbslq_f32(vreinterpretq_u32_u8(m.to_native()),a.value,b.value)); }
    /// Compute a*b+c with one fused rounding per lane.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec fma(vec a, vec b, vec c) { return vec(vfmaq_f32(c.value, a.value, b.value)); }
    /// Take the native square root of each lane.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec sqrt(vec a) { return vec(vsqrtq_f32(a.value)); }
    /// Round each lane to nearest integral value, choosing even at a tie.
    simd_artificial simd_nodiscard friend simd_inline simd_const vec round_even(vec a) { return vec(vrndnq_f32(a.value)); }
    /// Construct normal powers of two; require integral exponents in [-126,127].
    simd_nodiscard friend simd_inline simd_const vec normal_pow2(vec n) { return vec(vreinterpretq_f32_s32(vshlq_n_s32(vaddq_s32(vcvtq_s32_f32(n.value), vdupq_n_s32(127)), 23))); }

    template <std::size_t Alignment = 1>
    /// Load logical lanes, assuming the template alignment in bytes.
    simd_nodiscard static simd_inline simd_pure vec load_memory(float const * p) noexcept {
      return SIMD_BACKEND_NAMESPACE::simd_load_native<vec,Alignment>(p);
    }
    template <std::size_t Alignment = 1>
    /// Store logical lanes, assuming the template alignment in bytes.
    simd_inline void store_memory(float * p) const noexcept {
      SIMD_BACKEND_NAMESPACE::simd_store_native<vec,Alignment>(p,*this);
    }
    using value_type = float;
    using register_type = vec;
    using native_type = float32x4_t;
    using bits_type = vec<uint32_t,4,SIMD_ARCH>;
    /// Return the native storage value without a numerical conversion.
    simd_nodiscard simd_inline simd_pure operator native_type() const noexcept { return value; }
    /// Return the native storage representation.
    simd_nodiscard simd_inline simd_pure native_type to_native() const noexcept { return value; }
    /// Project the exact lane representation into the corresponding unsigned vector.
    simd_artificial simd_nodiscard simd_inline simd_pure bits_type bits() const noexcept { return bits_type::from_native(vreinterpretq_u8_f32(value)); }
    /// Return the exact binary32 lane representations in the unsigned vector.
    simd_nodiscard simd_inline simd_pure bits_type to_bits() const noexcept { return bits(); }
    /// Reinterpret binary words as lane values; do not normalize them.
    simd_artificial simd_nodiscard static simd_inline simd_const vec from_bits(bits_type bits) noexcept { return vec(vreinterpretq_f32_u8(bits.value)); }
    /// Reinterpret binary words as lane values; do not normalize them.
    simd_nodiscard static simd_inline simd_const vec from_bits(std::uint32_t bits) noexcept { return from_bits(bits_type(bits)); }
    /// Broadcast the float value without adding an FTZ or other normalization policy.
    simd_nodiscard static simd_inline simd_const vec from_float(float x) noexcept { return vec(x); }
    /// Adopt native storage without numerical conversion.
    simd_nodiscard static simd_inline simd_const vec from_native(native_type x) noexcept { return vec(x); }
    /// Adopt raw float storage without numerical conversion or normalization.
    simd_nodiscard static simd_inline simd_const vec unsafe_from_float32(native_type x) noexcept { return vec(x); }
    /// Read all logical lanes without an extra alignment promise.
    simd_nodiscard static simd_inline simd_pure vec loadu(simd_noescape float const * p) { return SIMD_BACKEND_NAMESPACE::simd_load_native<vec,1>(p); }
    /// Write all logical lanes without an extra alignment promise.
    simd_inline void storeu(simd_noescape float * p) const { SIMD_BACKEND_NAMESPACE::simd_store_native<vec,1>(p,*this); }
    /// Load exactly the logical count of binary32 words without normalizing their representations.
    simd_nodiscard static simd_inline simd_pure vec load_bits(simd_noescape std::uint32_t const * p) noexcept { return from_bits(bits_type::load(p)); }
    /// Store the exact binary32 words for every logical lane.
    simd_inline void store_bits(simd_noescape std::uint32_t * p) const noexcept { bits().store(p); }
    /// Read n representation words and fill the remaining logical lanes; require n <= lanes. A zero count permits null.
    simd_nodiscard static simd_inline simd_pure vec load_bits_partial(simd_noescape std::uint32_t const * p,std::size_t n,std::uint32_t fill=0) noexcept { return from_bits(bits_type::load_partial(p,n,fill)); }
    /// Write n exact representation words; require n <= lanes. A zero count permits null.
    simd_inline void store_bits_partial(simd_noescape std::uint32_t * p,std::size_t n) const noexcept { bits().store_partial(p,n); }
    /// Read all logical lanes without an extra alignment promise.
    simd_inline vec(std::array<float,4> const & values) noexcept : vec(loadu(values.data())) {}
#if defined(__clang__)
    /// Construct logical lanes in argument order. Any element conversions determine the exception specification.
    template <class... X> requires (sizeof...(X)==4) && (std::convertible_to<X,float> && ...)
    simd_inline constexpr vec(X... x) noexcept((noexcept(static_cast<float>(x)) && ...)) : value{static_cast<float>(x)...} {}
#else
    template <class... X> requires (sizeof...(X)==4) && (std::convertible_to<X,float> && ...)
    /// Read all logical lanes without an extra alignment promise.
    simd_inline vec(X... x) noexcept((noexcept(static_cast<float>(x)) && ...)) : vec(loadu(std::array<float,4>{static_cast<float>(x)...}.data())) {}
#endif
    /// Apply the corresponding lane-wise add operation in place and return *this.
    simd_inline vec & operator+=(vec b) noexcept { return *this=*this+b; }
    /// Apply the corresponding lane-wise subtract operation in place and return *this.
    simd_inline vec & operator-=(vec b) noexcept { return *this=*this-b; }
    /// Apply the corresponding lane-wise multiply operation in place and return *this.
    simd_inline vec & operator*=(vec b) noexcept { return *this=*this*b; }
    /// Apply the corresponding lane-wise divide operation in place and return *this.
    simd_inline vec & operator/=(vec b) noexcept { return *this=*this/b; }
    /// Return a mask whose lanes are true where `a != b` holds. NaN lanes compare unequal.
    simd_nodiscard friend simd_inline simd_const mask_type operator!=(vec a,vec b) noexcept { return ~(a==b); }
    /// Return a mask whose lanes are true where `a <= b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator<=(vec a,vec b) noexcept { return (a<b)|(a==b); }
    /// Return a mask whose lanes are true where `a >= b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator>=(vec a,vec b) noexcept { return (a>b)|(a==b); }
  };
#endif
  namespace detail::SIMD_BACKEND {
    // VSCALEF uses floor, including negative fractions. Native rounding also
    // observes the caller's denormal-input mode before this operation.
    template <std::size_t N>
    simd_nodiscard simd_inline simd_pure vec<float,N,SIMD_ARCH> scaleb_floor(vec<float,N,SIMD_ARCH> x) noexcept {
#if SIMD_HAS_AVX2
      if constexpr(N==1) return vec<float,N,SIMD_ARCH>(_mm_cvtss_f32(_mm_floor_ss(_mm_setzero_ps(),_mm_set_ss(x.value))));
      else if constexpr(N==4) return vec<float,N,SIMD_ARCH>(_mm_floor_ps(x.value));
      else if constexpr(N==8) return vec<float,N,SIMD_ARCH>(_mm256_floor_ps(x.value));
      else
#endif
#if SIMD_HAS_ARM_NEON
      if constexpr(N==4) return vec<float,N,SIMD_ARCH>(vrndmq_f32(x.value));
      else
#endif
      return vec<float,N,SIMD_ARCH>(std::floor(x.value));
    }
    // Cold nonfinite-exponent table from Intel VSCALEFPD/SD/PS/SS. In particular,
    // quiet NaN scaled by +inf/-inf is +inf/+0, but signaling NaN stays NaN.
    // NaN payload/sign and matching active-lane exception flags are not promised.
    simd_nodiscard simd_inline simd_pure float scaleb_special(float x,float exponent) noexcept {
      auto a=std::bit_cast<std::uint32_t>(x),b=std::bit_cast<std::uint32_t>(exponent);
      auto aa=a&0x7fffffffu,bb=b&0x7fffffffu;
      if(aa>0x7f800000u && (a&0x00400000u)==0u) return x+x;
      if(bb>0x7f800000u) return x+exponent;
      bool negative=(b&0x80000000u)!=0u;
      if(aa>0x7f800000u) return std::bit_cast<float>(negative?0u:0x7f800000u);
      if(aa==0x7f800000u) return negative?x*0.0f:x;
      // A numerical zero comparison observes DAZ/FZ for a raw subnormal base.
      if(x==0.0f) return negative?std::bit_cast<float>(a&0x80000000u):x*exponent;
      return std::bit_cast<float>((a&0x80000000u)|(negative?0u:0x7f800000u));
    }
    template <std::size_t N,class M>
    simd_nodiscard simd_inline simd_pure vec<float,N,SIMD_ARCH> scaleb_fallback(M mask,
        vec<float,N,SIMD_ARCH> prior,vec<float,N,SIMD_ARCH> value,vec<float,N,SIMD_ARCH> exponent) noexcept {
      using V=vec<float,N,SIMD_ARCH>;using U=vec<uint32_t,N,SIMD_ARCH>;
      if(none(mask)) return prior;
      V x=select(mask,value,V(0)),n=select(mask,exponent,V(0));
      auto special=(n.bits()&U(0x7fffffffu))>=U(0x7f800000u);
      n=select(special,V(0),n);x=select(special,V(0),x);
      n=scaleb_floor(n);
      V result;
      if(any((n<V(-126))|(n>V(127)))) {
        // Three normal factors cover every binary32 input/exponent outcome.
        // Clamp before integer conversion, including very large finite n.
        n=select(n<V(-378),V(-378),select(n>V(381),V(381),n));
        V last=select(n<V(-126),V(-126),select(n>V(127),V(127),n));
        V remainder=n-last;
        V first=select(remainder<V(-126),V(-126),select(remainder>V(127),V(127),remainder));
        V middle=remainder-first;
        auto down=n<V(0);
        // Downscale extras first: an early tiny value must ultimately round to
        // zero. Upscale the largest factor first to normalize raw tiny inputs.
        V leading=select(down,first,last),trailing=select(down,last,middle);
        middle=select(down,middle,first);
        result=((x*normal_pow2(leading))*normal_pow2(middle))*normal_pow2(trailing);
      } else result=x*normal_pow2(n);
      if(any(special)) {
        std::array<float,N> a,b,r;
        value.store(a.data());exponent.store(b.data());result.store(r.data());
        auto bits=special.to_bitset();
        for(std::size_t i=0;i<N;++i) if((bits>>i)&1u) r[i]=scaleb_special(a[i],b[i]);
        result=V::load(r.data());
      }
      return select(mask,result,prior);
    }
  }
  // Full VSCALEFPS value semantics: floor(exponent), including nonfinite
  // operands. This raw operation follows the caller's FP environment.
  // Inactive lanes never execute scaling; merge preserves their exact bits.
  /// \ingroup vector_math
  /// Scale active lanes by 2^floor(exponent), retaining `prior` in other lanes.
  /// Inactive lanes are excluded from the scaling operation. The result follows
  /// the caller's floating-point environment, including denormal controls.
  template <std::size_t N,class M>
    requires(std::same_as<M,typename vec<float,N,SIMD_ARCH>::mask_type> ||
             std::same_as<M,typename vec<float,N,SIMD_ARCH>::vector_mask_type>)
  simd_nodiscard simd_inline simd_pure vec<float,N,SIMD_ARCH> masked_scaleb(M mask,
      vec<float,N,SIMD_ARCH> prior,vec<float,N,SIMD_ARCH> value,vec<float,N,SIMD_ARCH> exponent) noexcept {
    if constexpr(N==2 || N==3) {
      using V=vec<float,N,SIMD_ARCH>;
      using M4=typename V::storage_type::mask_type;
      return V::from_storage(masked_scaleb(M4::from_bitset(mask.to_bitset()),prior.to_storage(),value.to_storage(),exponent.to_storage()));
    } else {
#if SIMD_HAS_AVX512F
      auto native_mask=[&] {if constexpr(M::compact) return mask.to_native();else return to_predicate(mask).to_native();};
      if constexpr(N==1) return vec<float,N,SIMD_ARCH>(_mm_cvtss_f32(_mm_mask_scalef_ss(
        _mm_set_ss(prior.value),__mmask8(native_mask()),_mm_set_ss(value.value),_mm_set_ss(exponent.value))));
      else if constexpr(N==16) return vec<float,N,SIMD_ARCH>(_mm512_mask_scalef_ps(prior.value,native_mask(),value.value,exponent.value));
#if SIMD_HAS_AVX512VL
      else if constexpr(N==4) return vec<float,N,SIMD_ARCH>(_mm_mask_scalef_ps(prior.value,native_mask(),value.value,exponent.value));
      else if constexpr(N==8) return vec<float,N,SIMD_ARCH>(_mm256_mask_scalef_ps(prior.value,native_mask(),value.value,exponent.value));
#endif
      else
#endif
      return SIMD_BACKEND_NAMESPACE::scaleb_fallback(mask,prior,value,exponent);
    }
  }
  /// \ingroup vector_math
  /// Scale active lanes by 2^floor(exponent), writing positive zero elsewhere.
  template <std::size_t N,class M>
    requires(std::same_as<M,typename vec<float,N,SIMD_ARCH>::mask_type> ||
             std::same_as<M,typename vec<float,N,SIMD_ARCH>::vector_mask_type>)
  simd_nodiscard simd_inline simd_pure vec<float,N,SIMD_ARCH> masked_scaleb_zero(M mask,
      vec<float,N,SIMD_ARCH> value,vec<float,N,SIMD_ARCH> exponent) noexcept {
    return masked_scaleb(mask,vec<float,N,SIMD_ARCH>(0.f),value,exponent);
  }
  /// \ingroup vector_math
  /// Scale every lane by 2^floor(exponent), including fractional exponents.
  /// Unlike an integer ldexp exponent, the exponent argument is itself a vector.
  template <std::size_t N>
  simd_nodiscard simd_inline simd_pure vec<float,N,SIMD_ARCH> scaleb(vec<float,N,SIMD_ARCH> value,
      vec<float,N,SIMD_ARCH> exponent) noexcept {
    return masked_scaleb_zero(typename vec<float,N,SIMD_ARCH>::mask_type(true),value,exponent);
  }
  /// Add corresponding floating-point lanes using the caller's rounding and denormal environment.
  template <std::size_t N,class U> requires (!SIMD_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,vec<float,N,SIMD_ARCH>>) && std::convertible_to<U,vec<float,N,SIMD_ARCH>>
  simd_nodiscard simd_inline auto operator+(vec<float,N,SIMD_ARCH> a, U b) noexcept(noexcept(vec<float,N,SIMD_ARCH>(b))) { return a+vec<float,N,SIMD_ARCH>(b); }
  /// Add corresponding floating-point lanes using the caller's rounding and denormal environment.
  template <std::size_t N,class U> requires (!SIMD_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,vec<float,N,SIMD_ARCH>>) && std::convertible_to<U,vec<float,N,SIMD_ARCH>>
  simd_nodiscard simd_inline auto operator+(U a, vec<float,N,SIMD_ARCH> b) noexcept(noexcept(vec<float,N,SIMD_ARCH>(a))) { return vec<float,N,SIMD_ARCH>(a)+b; }
  /// Subtract corresponding floating-point lanes using the caller's rounding and denormal environment.
  template <std::size_t N,class U> requires (!SIMD_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,vec<float,N,SIMD_ARCH>>) && std::convertible_to<U,vec<float,N,SIMD_ARCH>>
  simd_nodiscard simd_inline auto operator-(vec<float,N,SIMD_ARCH> a, U b) noexcept(noexcept(vec<float,N,SIMD_ARCH>(b))) { return a-vec<float,N,SIMD_ARCH>(b); }
  /// Subtract corresponding floating-point lanes using the caller's rounding and denormal environment.
  template <std::size_t N,class U> requires (!SIMD_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,vec<float,N,SIMD_ARCH>>) && std::convertible_to<U,vec<float,N,SIMD_ARCH>>
  simd_nodiscard simd_inline auto operator-(U a, vec<float,N,SIMD_ARCH> b) noexcept(noexcept(vec<float,N,SIMD_ARCH>(a))) { return vec<float,N,SIMD_ARCH>(a)-b; }
  /// Multiply corresponding floating-point lanes using the caller's rounding and denormal environment.
  template <std::size_t N,class U> requires (!SIMD_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,vec<float,N,SIMD_ARCH>>) && std::convertible_to<U,vec<float,N,SIMD_ARCH>>
  simd_nodiscard simd_inline auto operator*(vec<float,N,SIMD_ARCH> a, U b) noexcept(noexcept(vec<float,N,SIMD_ARCH>(b))) { return a*vec<float,N,SIMD_ARCH>(b); }
  /// Multiply corresponding floating-point lanes using the caller's rounding and denormal environment.
  template <std::size_t N,class U> requires (!SIMD_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,vec<float,N,SIMD_ARCH>>) && std::convertible_to<U,vec<float,N,SIMD_ARCH>>
  simd_nodiscard simd_inline auto operator*(U a, vec<float,N,SIMD_ARCH> b) noexcept(noexcept(vec<float,N,SIMD_ARCH>(a))) { return vec<float,N,SIMD_ARCH>(a)*b; }
  /// Divide corresponding floating-point lanes using the caller's rounding and denormal environment.
  template <std::size_t N,class U> requires (!SIMD_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,vec<float,N,SIMD_ARCH>>) && std::convertible_to<U,vec<float,N,SIMD_ARCH>>
  simd_nodiscard simd_inline auto operator/(vec<float,N,SIMD_ARCH> a, U b) noexcept(noexcept(vec<float,N,SIMD_ARCH>(b))) { return a/vec<float,N,SIMD_ARCH>(b); }
  /// Divide corresponding floating-point lanes using the caller's rounding and denormal environment.
  template <std::size_t N,class U> requires (!SIMD_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,vec<float,N,SIMD_ARCH>>) && std::convertible_to<U,vec<float,N,SIMD_ARCH>>
  simd_nodiscard simd_inline auto operator/(U a, vec<float,N,SIMD_ARCH> b) noexcept(noexcept(vec<float,N,SIMD_ARCH>(a))) { return vec<float,N,SIMD_ARCH>(a)/b; }
  /// Return a mask whose lanes are true where `a < b` holds. NaN lanes yield false.
  template <std::size_t N,class U> requires (!SIMD_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,vec<float,N,SIMD_ARCH>>) && std::convertible_to<U,vec<float,N,SIMD_ARCH>>
  simd_nodiscard simd_inline auto operator<(vec<float,N,SIMD_ARCH> a, U b) noexcept(noexcept(vec<float,N,SIMD_ARCH>(b))) { return a<vec<float,N,SIMD_ARCH>(b); }
  /// Return a mask whose lanes are true where `a < b` holds. NaN lanes yield false.
  template <std::size_t N,class U> requires (!SIMD_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,vec<float,N,SIMD_ARCH>>) && std::convertible_to<U,vec<float,N,SIMD_ARCH>>
  simd_nodiscard simd_inline auto operator<(U a, vec<float,N,SIMD_ARCH> b) noexcept(noexcept(vec<float,N,SIMD_ARCH>(a))) { return vec<float,N,SIMD_ARCH>(a)<b; }
  /// Return a mask whose lanes are true where `a > b` holds. NaN lanes yield false.
  template <std::size_t N,class U> requires (!SIMD_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,vec<float,N,SIMD_ARCH>>) && std::convertible_to<U,vec<float,N,SIMD_ARCH>>
  simd_nodiscard simd_inline auto operator>(vec<float,N,SIMD_ARCH> a, U b) noexcept(noexcept(vec<float,N,SIMD_ARCH>(b))) { return a>vec<float,N,SIMD_ARCH>(b); }
  /// Return a mask whose lanes are true where `a > b` holds. NaN lanes yield false.
  template <std::size_t N,class U> requires (!SIMD_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,vec<float,N,SIMD_ARCH>>) && std::convertible_to<U,vec<float,N,SIMD_ARCH>>
  simd_nodiscard simd_inline auto operator>(U a, vec<float,N,SIMD_ARCH> b) noexcept(noexcept(vec<float,N,SIMD_ARCH>(a))) { return vec<float,N,SIMD_ARCH>(a)>b; }
  /// Return a mask whose lanes are true where `a <= b` holds. NaN lanes yield false.
  template <std::size_t N,class U> requires (!SIMD_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,vec<float,N,SIMD_ARCH>>) && std::convertible_to<U,vec<float,N,SIMD_ARCH>>
  simd_nodiscard simd_inline auto operator<=(vec<float,N,SIMD_ARCH> a, U b) noexcept(noexcept(vec<float,N,SIMD_ARCH>(b))) { return a<=vec<float,N,SIMD_ARCH>(b); }
  /// Return a mask whose lanes are true where `a <= b` holds. NaN lanes yield false.
  template <std::size_t N,class U> requires (!SIMD_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,vec<float,N,SIMD_ARCH>>) && std::convertible_to<U,vec<float,N,SIMD_ARCH>>
  simd_nodiscard simd_inline auto operator<=(U a, vec<float,N,SIMD_ARCH> b) noexcept(noexcept(vec<float,N,SIMD_ARCH>(a))) { return vec<float,N,SIMD_ARCH>(a)<=b; }
  /// Return a mask whose lanes are true where `a >= b` holds. NaN lanes yield false.
  template <std::size_t N,class U> requires (!SIMD_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,vec<float,N,SIMD_ARCH>>) && std::convertible_to<U,vec<float,N,SIMD_ARCH>>
  simd_nodiscard simd_inline auto operator>=(vec<float,N,SIMD_ARCH> a, U b) noexcept(noexcept(vec<float,N,SIMD_ARCH>(b))) { return a>=vec<float,N,SIMD_ARCH>(b); }
  /// Return a mask whose lanes are true where `a >= b` holds. NaN lanes yield false.
  template <std::size_t N,class U> requires (!SIMD_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,vec<float,N,SIMD_ARCH>>) && std::convertible_to<U,vec<float,N,SIMD_ARCH>>
  simd_nodiscard simd_inline auto operator>=(U a, vec<float,N,SIMD_ARCH> b) noexcept(noexcept(vec<float,N,SIMD_ARCH>(a))) { return vec<float,N,SIMD_ARCH>(a)>=b; }
  /// Return a mask whose lanes are true where `a == b` holds. NaN lanes yield false.
  template <std::size_t N,class U> requires (!SIMD_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,vec<float,N,SIMD_ARCH>>) && std::convertible_to<U,vec<float,N,SIMD_ARCH>>
  simd_nodiscard simd_inline auto operator==(vec<float,N,SIMD_ARCH> a, U b) noexcept(noexcept(vec<float,N,SIMD_ARCH>(b))) { return a==vec<float,N,SIMD_ARCH>(b); }
  /// Return a mask whose lanes are true where `a == b` holds. NaN lanes yield false.
  template <std::size_t N,class U> requires (!SIMD_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,vec<float,N,SIMD_ARCH>>) && std::convertible_to<U,vec<float,N,SIMD_ARCH>>
  simd_nodiscard simd_inline auto operator==(U a, vec<float,N,SIMD_ARCH> b) noexcept(noexcept(vec<float,N,SIMD_ARCH>(a))) { return vec<float,N,SIMD_ARCH>(a)==b; }
  /// Return a mask whose lanes are true where `a != b` holds. NaN lanes compare unequal.
  template <std::size_t N,class U> requires (!SIMD_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,vec<float,N,SIMD_ARCH>>) && std::convertible_to<U,vec<float,N,SIMD_ARCH>>
  simd_nodiscard simd_inline auto operator!=(vec<float,N,SIMD_ARCH> a, U b) noexcept(noexcept(vec<float,N,SIMD_ARCH>(b))) { return a!=vec<float,N,SIMD_ARCH>(b); }
  /// Return a mask whose lanes are true where `a != b` holds. NaN lanes compare unequal.
  template <std::size_t N,class U> requires (!SIMD_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,vec<float,N,SIMD_ARCH>>) && std::convertible_to<U,vec<float,N,SIMD_ARCH>>
  simd_nodiscard simd_inline auto operator!=(U a, vec<float,N,SIMD_ARCH> b) noexcept(noexcept(vec<float,N,SIMD_ARCH>(a))) { return vec<float,N,SIMD_ARCH>(a)!=b; }
  /// Compute a*b+c with one fused rounding per logical lane in the caller's floating-point environment.
  template <std::size_t N,class A,class B> requires (!SIMD_BACKEND_NAMESPACE::custom_argument<A> && !SIMD_BACKEND_NAMESPACE::custom_argument<B>) && std::convertible_to<A,vec<float,N,SIMD_ARCH>> && std::convertible_to<B,vec<float,N,SIMD_ARCH>>
  simd_nodiscard simd_inline auto fma(vec<float,N,SIMD_ARCH> a,A b,B c) noexcept(noexcept(vec<float,N,SIMD_ARCH>(b)) && noexcept(vec<float,N,SIMD_ARCH>(c))) { return fma(a,vec<float,N,SIMD_ARCH>(b),vec<float,N,SIMD_ARCH>(c)); }
  /// Compute a*b+c with one fused rounding per logical lane in the caller's floating-point environment.
  template <std::size_t N,class A,class B> requires (!SIMD_BACKEND_NAMESPACE::custom_argument<A> && !SIMD_BACKEND_NAMESPACE::custom_argument<B>) && (!std::same_as<A,vec<float,N,SIMD_ARCH>>) && std::convertible_to<A,vec<float,N,SIMD_ARCH>> && std::convertible_to<B,vec<float,N,SIMD_ARCH>>
  simd_nodiscard simd_inline auto fma(A a,vec<float,N,SIMD_ARCH> b,B c) noexcept(noexcept(vec<float,N,SIMD_ARCH>(a)) && noexcept(vec<float,N,SIMD_ARCH>(c))) { return fma(vec<float,N,SIMD_ARCH>(a),b,vec<float,N,SIMD_ARCH>(c)); }
  /// Compute a*b+c with one fused rounding per logical lane in the caller's floating-point environment.
  template <std::size_t N,class A,class B> requires (!SIMD_BACKEND_NAMESPACE::custom_argument<A> && !SIMD_BACKEND_NAMESPACE::custom_argument<B>) && (!std::same_as<A,vec<float,N,SIMD_ARCH>>) && (!std::same_as<B,vec<float,N,SIMD_ARCH>>) && std::convertible_to<A,vec<float,N,SIMD_ARCH>> && std::convertible_to<B,vec<float,N,SIMD_ARCH>>
  simd_nodiscard simd_inline auto fma(A a,B b,vec<float,N,SIMD_ARCH> c) noexcept(noexcept(vec<float,N,SIMD_ARCH>(a)) && noexcept(vec<float,N,SIMD_ARCH>(b))) { return fma(vec<float,N,SIMD_ARCH>(a),vec<float,N,SIMD_ARCH>(b),c); }
#if SIMD_HAS_AVX2
  /// Deduce a homogeneous mask vector from its lane arguments and explicit architecture.
  vec(SIMD_ARCH,__m128) -> vec<float,4,SIMD_ARCH>;
  /// Deduce a homogeneous mask vector from its lane arguments and explicit architecture.
  vec(SIMD_ARCH,__m256) -> vec<float,8,SIMD_ARCH>;
#endif
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ
  /// Deduce a homogeneous mask vector from its lane arguments and explicit architecture.
  vec(SIMD_ARCH,__m512) -> vec<float,16,SIMD_ARCH>;
#endif
#if SIMD_HAS_ARM_NEON
  /// Deduce four float lanes from native NEON storage and the explicit architecture tag.
  vec(SIMD_ARCH,float32x4_t) -> vec<float,4,SIMD_ARCH>;
#endif

  /// \ingroup vector_math
  /// Clear each binary32 sign bit, preserving the remaining payload bits.
  template <std::size_t N>
  simd_nodiscard simd_inline simd_const auto abs(vec<float,N,SIMD_ARCH> a) noexcept {
    return vec<float,N,SIMD_ARCH>::from_bits(a.bits() & typename vec<float,N,SIMD_ARCH>::bits_type(0x7fffffffu));
  }

  // Truncating float-to-integer conversion requires a representable result.
  /// \ingroup vector_math
  /// Convert floats to signed 32-bit integers by truncation toward zero.
  /// \pre Every truncated result is representable; NaNs and infinities are outside the contract.
  template <class To, std::size_t N> requires std::same_as<To,std::int32_t>
  simd_nodiscard simd_inline vec<To,N,SIMD_ARCH> convert(vec<float,N,SIMD_ARCH> x) noexcept {
    if constexpr(N==2 || N==3) return vec<To,N,SIMD_ARCH>::from_storage(convert<To>(x.to_storage()));
    else if constexpr (N==1) return vec<To,N,SIMD_ARCH>(static_cast<std::int32_t>(x.value));
#if SIMD_HAS_AVX2
    else if constexpr (N==4) return vec<To,N,SIMD_ARCH>::from_native(_mm_cvttps_epi32(x.value));
    else if constexpr (N==8) return vec<To,N,SIMD_ARCH>::from_native(_mm256_cvttps_epi32(x.value));
#endif
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ
    else if constexpr (N==16) return vec<To,N,SIMD_ARCH>::from_native(_mm512_cvttps_epi32(x.value));
#endif
#if SIMD_HAS_ARM_NEON
    else if constexpr (N==4) return vec<To,N,SIMD_ARCH>::from_native(vreinterpretq_u8_s32(vcvtq_s32_f32(x.value)));
#endif
  }
  /// \ingroup vector_math
  /// Numerically convert signed 32-bit integers to binary32.
  /// This is not a bit cast. Values outside binary32's exact integer range round.
  template <class To, std::size_t N> requires std::same_as<To,float>
  simd_nodiscard simd_inline vec<To,N,SIMD_ARCH> convert(vec<std::int32_t,N,SIMD_ARCH> x) noexcept {
    if constexpr(N==2 || N==3) return vec<To,N,SIMD_ARCH>::from_storage(convert<To>(x.to_storage()));
    else if constexpr (N==1) return vec<To,N,SIMD_ARCH>(static_cast<float>(x.value));
#if SIMD_HAS_AVX2
    else if constexpr (N==4) return vec<To,N,SIMD_ARCH>(_mm_cvtepi32_ps(x.value));
    else if constexpr (N==8) return vec<To,N,SIMD_ARCH>(_mm256_cvtepi32_ps(x.value));
#endif
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ
    else if constexpr (N==16) return vec<To,N,SIMD_ARCH>(_mm512_cvtepi32_ps(x.value));
#endif
#if SIMD_HAS_ARM_NEON
    else if constexpr (N==4) return vec<To,N,SIMD_ARCH>(vcvtq_f32_s32(vreinterpretq_s32_u8(x.value)));
#endif
  }

  namespace detail::SIMD_BACKEND {
    template <class V,std::size_t Alignment> simd_nodiscard simd_artificial simd_inline simd_pure V simd_load_native(simd_noescape float const * p) noexcept {
      if constexpr(V::lanes==1) return V(*p);
#if SIMD_HAS_AVX2
      else if constexpr(V::lanes==4) { if constexpr(Alignment>=16)return V(_mm_load_ps(p));else return V(_mm_loadu_ps(p)); }
      else if constexpr(V::lanes==8) { if constexpr(Alignment>=32)return V(_mm256_load_ps(p));else return V(_mm256_loadu_ps(p)); }
#endif
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ
      else if constexpr(V::lanes==16) { if constexpr(Alignment>=64)return V(_mm512_load_ps(p));else return V(_mm512_loadu_ps(p)); }
#endif
#if SIMD_HAS_ARM_NEON
      else if constexpr(V::lanes==4)return V(vld1q_f32(p));
#endif
    }
    template <class V,std::size_t Alignment> simd_artificial simd_inline void simd_store_native(simd_noescape float * p,V value) noexcept {
      if constexpr(V::lanes==1)*p=value.value;
#if SIMD_HAS_AVX2
      else if constexpr(V::lanes==4) {if constexpr(Alignment>=16)_mm_store_ps(p,value.value);else _mm_storeu_ps(p,value.value);}
      else if constexpr(V::lanes==8) {if constexpr(Alignment>=32)_mm256_store_ps(p,value.value);else _mm256_storeu_ps(p,value.value);}
#endif
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ
      else if constexpr(V::lanes==16) {if constexpr(Alignment>=64)_mm512_store_ps(p,value.value);else _mm512_storeu_ps(p,value.value);}
#endif
#if SIMD_HAS_ARM_NEON
      else if constexpr(V::lanes==4)vst1q_f32(p,value.value);
#endif
    }
  }
  namespace detail::SIMD_BACKEND {
  // Alignment is a caller promise. Access is a hint; streaming currently falls
  // back to ordinary moves on every backend and needs no completion fence.
  template <class T,std::size_t N,class U,std::size_t A=1,simd_access Access=simd_access::ordinary>
    requires (std::same_as<U,float> || ::simd::simd_custom_element<U>) &&
      (std::same_as<T,float> || ::simd::simd_custom_element<T>)
  simd_nodiscard simd_inline simd_pure vec<T,N,SIMD_ARCH> load_simd(simd_noescape U const * p,simd_memory<A,Access> = {}) noexcept {
    if constexpr (::simd::simd_custom_element<T>) return vec<T,N,SIMD_ARCH>::template load_memory<A>(p);
    else if constexpr (::simd::simd_custom_element<U>) return vec<U,N,SIMD_ARCH>::template load_memory<A>(p).to_native();
    else return SIMD_BACKEND_NAMESPACE::simd_load_native<vec<float,N,SIMD_ARCH>,A>(p);
  }
  template <class U,class T,std::size_t N,std::size_t A=1,simd_access Access=simd_access::ordinary>
    requires (std::same_as<U,float> || ::simd::simd_custom_element<U>) &&
      (std::same_as<T,float> || ::simd::simd_custom_element<T>)
  simd_inline void store_simd(simd_noescape U * p,vec<T,N,SIMD_ARCH> value,simd_memory<A,Access> = {}) noexcept {
    if constexpr (::simd::simd_custom_element<U>) vec<U,N,SIMD_ARCH>(value).template store_memory<A>(p);
    /// Return the native storage representation.
    else if constexpr (::simd::simd_custom_element<T>) value.to_native().template store_memory<A>(p);
    else SIMD_BACKEND_NAMESPACE::simd_store_native<vec<float,N,SIMD_ARCH>,A>(p,value);
  }
  template <class T,std::size_t N,class U,std::size_t A=1,simd_access Access=simd_access::ordinary>
    requires (std::same_as<U,float> || ::simd::simd_custom_element<U>) &&
      (std::same_as<T,float> || ::simd::simd_custom_element<T>)
  simd_nodiscard simd_inline simd_pure vec<T,N,SIMD_ARCH> load_simd_partial(simd_noescape U const * p,std::size_t count,
      T fill=T{},simd_memory<A,Access> = {}) noexcept {
    std::array<U,N> temporary;temporary.fill(U(fill));
    for(std::size_t i=0;i<count;++i)temporary[i]=p[i];
    return SIMD_BACKEND_NAMESPACE::load_simd<T,N>(temporary.data());
  }
  template <class U,class T,std::size_t N,std::size_t A=1,simd_access Access=simd_access::ordinary>
    requires (std::same_as<U,float> || ::simd::simd_custom_element<U>) &&
      (std::same_as<T,float> || ::simd::simd_custom_element<T>)
  simd_inline void store_simd_partial(simd_noescape U * p,vec<T,N,SIMD_ARCH> value,std::size_t count,simd_memory<A,Access> = {}) noexcept {
    std::array<U,N> temporary;store_simd(temporary.data(),value);
    for(std::size_t i=0;i<count;++i)p[i]=temporary[i];
  }
  template <class T,std::size_t N>
  simd_nodiscard simd_inline simd_pure auto load_simd(std::array<T,N> const & values) noexcept { return SIMD_BACKEND_NAMESPACE::load_simd<T,N>(values.data()); }
  template <class T,std::size_t N> requires (N!=std::dynamic_extent)
  simd_nodiscard simd_inline simd_pure auto load_simd(std::span<T,N> values) noexcept { return load_simd<std::remove_cv_t<T>,N>(values.data()); }
  }

}
namespace SIMD_BACKEND_NAMESPACE::native {
  template <class V,std::size_t N> using register_memory = ::SIMD_BACKEND_NAMESPACE::register_memory<V,N>;
  using fp32x1 = ::simd::vec<float,1,SIMD_ARCH>;
#if SIMD_HAS_AVX2 || SIMD_HAS_ARM_NEON
  using fp32x4 = ::simd::vec<float,4,SIMD_ARCH>;
#endif
#if SIMD_HAS_AVX2
  using fp32x8 = ::simd::vec<float,8,SIMD_ARCH>;
#endif
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ
  using fp32x16 = ::simd::vec<float,16,SIMD_ARCH>;
#endif
  template<class V> concept float_register = requires { V::lanes; };
  // Ordered comparison: second operand wins on equality or unordered, including
  // signed zero and NaNs, identically on each architecture.
  template<float_register V> simd_nodiscard simd_inline simd_const V min(V a, V b) { return select(a < b, a, b); }
  template<float_register V> simd_nodiscard simd_inline simd_const V max(V a, V b) { return select(a > b, a, b); }
  template<float_register V> simd_nodiscard simd_inline simd_pure float reduce_add(V v) {
    std::array<float, V::lanes> a; v.storeu(a.data()); float sum = 0;
    for (float x : a) sum += x; // Increasing lane order, FP32.
    return sum;
  }
  template<float_register V> simd_nodiscard simd_inline V acos_scalar_lanes(V v) {
    std::array<float, V::lanes> a; v.storeu(a.data());
    for (float & x : a) x = std::acos(x);
    return V::loadu(a.data());
  }
  struct native_target {
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ
    using float_type = fp32x16;
    static constexpr std::size_t preferred_registers = 6;
#elif SIMD_HAS_AVX2
    using float_type = fp32x8;
    static constexpr std::size_t preferred_registers = 6;
#elif SIMD_HAS_ARM_NEON
    using float_type = fp32x4;
    // Prior Mac wide-kernel tuning; remeasure when changing the kernel.
    static constexpr std::size_t preferred_registers = 15;
#else
    using float_type = fp32x1;
    static constexpr std::size_t preferred_registers = 1;
#endif
    // K is derived from the selected register, never a second ISA decision.
    static constexpr std::size_t lanes = float_type::lanes;
  };
}

#include "simd/simd/short.h"

namespace simd {
  namespace detail::SIMD_BACKEND {
    enum class rounding_direction { down, up, zero };

    template<rounding_direction Direction,std::size_t N>
    simd_inline vec<float,N,SIMD_ARCH> round_integral(vec<float,N,SIMD_ARCH> x) noexcept {
      using V = vec<float,N,SIMD_ARCH>;
      if constexpr (N == 2 || N == 3) {
        return V::from_storage(round_integral<Direction>(x.to_storage()));
      } else {
#if SIMD_HAS_AVX2
        constexpr int mode = (Direction == rounding_direction::down ? _MM_FROUND_TO_NEG_INF :
          Direction == rounding_direction::up ? _MM_FROUND_TO_POS_INF : _MM_FROUND_TO_ZERO) | _MM_FROUND_NO_EXC;
        if constexpr (N == 1)
          return V::from_native(_mm_cvtss_f32(_mm_round_ss(_mm_setzero_ps(),_mm_set_ss(x.to_native()),mode)));
        else if constexpr (N == 4) return V::from_native(_mm_round_ps(x.to_native(),mode));
        else if constexpr (N == 8) return V::from_native(_mm256_round_ps(x.to_native(),mode));
#if SIMD_HAS_AVX512F
        else if constexpr (N == 16) return V::from_native(_mm512_roundscale_ps(x.to_native(),mode));
#endif
#elif SIMD_HAS_ARM_NEON
        if constexpr (N == 1) {
          auto a = vdup_n_f32(x.to_native());
          if constexpr (Direction == rounding_direction::down) return V::from_native(vget_lane_f32(vrndm_f32(a),0));
          else if constexpr (Direction == rounding_direction::up) return V::from_native(vget_lane_f32(vrndp_f32(a),0));
          else return V::from_native(vget_lane_f32(vrnd_f32(a),0));
        } else {
          if constexpr (Direction == rounding_direction::down) return V::from_native(vrndmq_f32(x.to_native()));
          else if constexpr (Direction == rounding_direction::up) return V::from_native(vrndpq_f32(x.to_native()));
          else return V::from_native(vrndq_f32(x.to_native()));
        }
#else
        if constexpr (Direction == rounding_direction::down) return V::from_native(std::floor(x.to_native()));
        else if constexpr (Direction == rounding_direction::up) return V::from_native(std::ceil(x.to_native()));
        else return V::from_native(std::trunc(x.to_native()));
#endif
      }
    }
  }

  // Directions are encoded in the instruction, never taken from the ambient
  // rounding mode. As for other raw arithmetic, DAZ/FZ input handling follows
  // the configured floating-point environment.
  /// \ingroup vector_math
  /// Round each lane toward negative infinity, independently of ambient rounding.
  /// Signed zeros and infinities are preserved; NaNs remain NaNs. Raw denormal
  /// handling still follows the CPU environment. No NaN payload or FP-status promise is added.
  /// \snippet api.cc rounding
  template<std::size_t N> requires requires { typename vec<float,N,SIMD_ARCH>::native_type; }
  simd_nodiscard simd_inline vec<float,N,SIMD_ARCH> floor(vec<float,N,SIMD_ARCH> x) noexcept {
    return detail::SIMD_BACKEND::round_integral<detail::SIMD_BACKEND::rounding_direction::down>(x);
  }
  /// \ingroup vector_math
  /// Apply floor to each register in an array; an empty array performs no lane work.
  template<std::size_t N,std::size_t M> requires requires { typename vec<float,N,SIMD_ARCH>::native_type; }
  simd_nodiscard simd_inline std::array<vec<float,N,SIMD_ARCH>,M> floor(std::array<vec<float,N,SIMD_ARCH>,M> const & input) noexcept {
    auto const & [...x] = input;
    return {{floor(x)...}};
  }
  /// \ingroup vector_math
  /// Round each lane toward positive infinity, independently of ambient rounding.
  /// Signed zeros and infinities are preserved; NaNs remain NaNs. Raw denormal
  /// handling still follows the CPU environment. No NaN payload or FP-status promise is added.
  /// \snippet api.cc rounding
  template<std::size_t N> requires requires { typename vec<float,N,SIMD_ARCH>::native_type; }
  simd_nodiscard simd_inline vec<float,N,SIMD_ARCH> ceil(vec<float,N,SIMD_ARCH> x) noexcept {
    return detail::SIMD_BACKEND::round_integral<detail::SIMD_BACKEND::rounding_direction::up>(x);
  }
  /// \ingroup vector_math
  /// Apply ceil to each register in an array; an empty array performs no lane work.
  template<std::size_t N,std::size_t M> requires requires { typename vec<float,N,SIMD_ARCH>::native_type; }
  simd_nodiscard simd_inline std::array<vec<float,N,SIMD_ARCH>,M> ceil(std::array<vec<float,N,SIMD_ARCH>,M> const & input) noexcept {
    auto const & [...x] = input;
    return {{ceil(x)...}};
  }
  /// \ingroup vector_math
  /// Round each lane toward zero, independently of ambient rounding.
  /// Signed zeros and infinities are preserved; NaNs remain NaNs. Raw denormal
  /// handling still follows the CPU environment. No NaN payload or FP-status promise is added.
  /// \snippet api.cc rounding
  template<std::size_t N> requires requires { typename vec<float,N,SIMD_ARCH>::native_type; }
  simd_nodiscard simd_inline vec<float,N,SIMD_ARCH> trunc(vec<float,N,SIMD_ARCH> x) noexcept {
    return detail::SIMD_BACKEND::round_integral<detail::SIMD_BACKEND::rounding_direction::zero>(x);
  }
  /// \ingroup vector_math
  /// Apply trunc to each register in an array; an empty array performs no lane work.
  template<std::size_t N,std::size_t M> requires requires { typename vec<float,N,SIMD_ARCH>::native_type; }
  simd_nodiscard simd_inline std::array<vec<float,N,SIMD_ARCH>,M> trunc(std::array<vec<float,N,SIMD_ARCH>,M> const & input) noexcept {
    auto const & [...x] = input;
    return {{trunc(x)...}};
  }
}

// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0


namespace simd::detail::SIMD_BACKEND {
  template<std::size_t N,class M>
  simd_inline std::uint32_t compaction_mask_bits(M mask) noexcept {
#if SIMD_HAS_AVX2
    if constexpr (N > 1 && !M::compact) {
      if constexpr (sizeof(M) == 32)
        return std::uint32_t(_mm256_movemask_ps(std::bit_cast<__m256>(mask.to_native())));
      else return std::uint32_t(_mm_movemask_ps(std::bit_cast<__m128>(mask.to_native()))) & ((1u << N) - 1);
    } else
#elif SIMD_HAS_ARM_NEON
    if constexpr (N > 1) {
      constexpr std::array<std::uint32_t,4> weights{1,2,4,8};
      return vaddvq_u32(vandq_u32(std::bit_cast<uint32x4_t>(mask.to_native()),vld1q_u32(weights.data()))) & ((1u << N) - 1);
    } else
#endif
    return std::uint32_t(mask.to_bitset()) & ((1u << N) - 1);
  }

  // Four-lane shuffles use byte indices on both SSSE3 and NEON. The eight-lane
  // AVX2 permutation expands eight byte indices to dwords (2 KiB per table).
  template<bool Expand, std::size_t Width> inline constexpr auto compaction_indices = [] {
    constexpr auto bytes = Width == 4 ? 16 : 8;
    std::array<std::array<std::uint8_t,bytes>,std::size_t(1) << Width> table{};
    for (std::size_t mask = 0; mask != table.size(); ++mask) {
      std::size_t packed = 0;
      for (std::size_t lane = 0; lane != Width; ++lane) if (mask & (std::size_t(1) << lane)) {
        auto destination = Expand ? lane : packed;
        auto source = Expand ? packed : lane;
        if constexpr (Width == 4)
          for (std::size_t byte = 0; byte != 4; ++byte)
            table[mask][4 * destination + byte] = std::uint8_t(4 * source + byte);
        else table[mask][destination] = std::uint8_t(source);
        ++packed;
      }
    }
    return table;
  }();

  template<bool Expand, class V>
  simd_inline V compact_register(std::uint32_t mask, V input, V prior) noexcept {
    if constexpr (V::lanes == 1) return mask ? input : prior;
    else {
#if SIMD_HAS_AVX512F
      if constexpr (sizeof(V) == 64) {
        auto x = std::bit_cast<__m512i>(input), merge = std::bit_cast<__m512i>(prior);
        if constexpr (Expand) return std::bit_cast<V>(_mm512_mask_expand_epi32(merge, __mmask16(mask), x));
        else return std::bit_cast<V>(_mm512_mask_compress_epi32(merge, __mmask16(mask), x));
      }
#if SIMD_HAS_AVX512VL
      else if constexpr (sizeof(V) == 32) {
        auto x = std::bit_cast<__m256i>(input), merge = std::bit_cast<__m256i>(prior);
        if constexpr (Expand) return std::bit_cast<V>(_mm256_mask_expand_epi32(merge, __mmask8(mask), x));
        else return std::bit_cast<V>(_mm256_mask_compress_epi32(merge, __mmask8(mask), x));
      } else {
        auto x = std::bit_cast<__m128i>(input), merge = std::bit_cast<__m128i>(prior);
        if constexpr (Expand) return std::bit_cast<V>(_mm_mask_expand_epi32(merge, __mmask8(mask), x));
        else return std::bit_cast<V>(_mm_mask_compress_epi32(merge, __mmask8(mask), x));
      }
#else
      else
#endif
#endif
#if (!SIMD_HAS_AVX512F || !SIMD_HAS_AVX512VL) && (SIMD_HAS_AVX2 || SIMD_HAS_ARM_NEON)
      {
      V permuted;
#if SIMD_HAS_AVX2
      if constexpr (sizeof(V) == 32) {
        auto const & row = compaction_indices<Expand,8>[mask];
        auto indices = _mm256_cvtepu8_epi32(_mm_loadl_epi64(reinterpret_cast<__m128i const *>(row.data())));
        permuted = std::bit_cast<V>(_mm256_permutevar8x32_epi32(std::bit_cast<__m256i>(input), indices));
      } else {
        auto const & row = compaction_indices<Expand,4>[mask];
        auto indices = _mm_loadu_si128(reinterpret_cast<__m128i const *>(row.data()));
        permuted = std::bit_cast<V>(_mm_shuffle_epi8(std::bit_cast<__m128i>(input), indices));
      }
#else
      auto const & row = compaction_indices<Expand,4>[mask];
      permuted = std::bit_cast<V>(vqtbl1q_u8(std::bit_cast<uint8x16_t>(input), vld1q_u8(row.data())));
#endif
      // Build the destination predicate in registers instead of round-tripping
      // a packed mask through the generic byte-oriented mask representation.
#if SIMD_HAS_AVX2
      if constexpr (sizeof(V) == 32) {
        __m256i live;
        if constexpr (Expand) {
          auto bits = _mm256_setr_epi32(1,2,4,8,16,32,64,128);
          live = _mm256_cmpeq_epi32(_mm256_and_si256(_mm256_set1_epi32(int(mask)),bits),bits);
        } else live = _mm256_cmpgt_epi32(_mm256_set1_epi32(std::popcount(mask)),_mm256_setr_epi32(0,1,2,3,4,5,6,7));
        return std::bit_cast<V>(_mm256_blendv_epi8(std::bit_cast<__m256i>(prior),std::bit_cast<__m256i>(permuted),live));
      } else {
        __m128i live;
        if constexpr (Expand) {
          auto bits = _mm_setr_epi32(1,2,4,8);
          live = _mm_cmpeq_epi32(_mm_and_si128(_mm_set1_epi32(int(mask)),bits),bits);
        } else live = _mm_cmpgt_epi32(_mm_set1_epi32(std::popcount(mask)),_mm_setr_epi32(0,1,2,3));
        return std::bit_cast<V>(_mm_blendv_epi8(std::bit_cast<__m128i>(prior),std::bit_cast<__m128i>(permuted),live));
      }
#else
      uint32x4_t live;
      if constexpr (Expand) {
        constexpr std::array<std::uint32_t,4> weights{1,2,4,8};
        live = vtstq_u32(vdupq_n_u32(mask),vld1q_u32(weights.data()));
      } else {
        constexpr std::array<std::uint32_t,4> lanes{0,1,2,3};
        live = vcltq_u32(vld1q_u32(lanes.data()),vdupq_n_u32(std::uint32_t(std::popcount(mask))));
      }
      return std::bit_cast<V>(vbslq_u8(vreinterpretq_u8_u32(live),std::bit_cast<uint8x16_t>(permuted),std::bit_cast<uint8x16_t>(prior)));
#endif
      }
#endif
    }
  }
}

namespace simd {
  /// \ingroup vector_memory
  /// Pack selected logical lanes in increasing order; fill all remaining lanes.
  /// This rearranges bits, including NaN payloads and signed zeros. Short padding
  /// never contributes to count or output and is zero in the result.
  /// \snippet api.cc compaction
  template<class T, std::size_t N> requires
    (std::same_as<T,float> || std::same_as<T,std::int32_t> || std::same_as<T,std::uint32_t>) &&
    requires { typename vec<T,N,SIMD_ARCH>::native_type; }
  simd_nodiscard simd_inline compaction_result<vec<T,N,SIMD_ARCH>> compress(
      typename vec<T,N,SIMD_ARCH>::mask mask, vec<T,N,SIMD_ARCH> value, T fill = T{}) noexcept {
    using V = vec<T,N,SIMD_ARCH>;
    auto bits = detail::SIMD_BACKEND::compaction_mask_bits<N>(mask);
    // Construct fill through integer object representation, without FP arithmetic.
    using U = vec<std::uint32_t,N,SIMD_ARCH>;
    V prior = std::bit_cast<V>(U(std::bit_cast<std::uint32_t>(fill)));
    return {detail::SIMD_BACKEND::compact_register<false>(bits, value, prior), std::size_t(std::popcount(bits))};
  }

  /// \ingroup vector_memory
  /// Consume packed's first popcount(mask) lanes in order at selected positions.
  /// Unselected lanes retain prior bitwise. Short physical padding is zeroed.
  template<class T, std::size_t N> requires
    (std::same_as<T,float> || std::same_as<T,std::int32_t> || std::same_as<T,std::uint32_t>) &&
    requires { typename vec<T,N,SIMD_ARCH>::native_type; }
  simd_nodiscard simd_inline vec<T,N,SIMD_ARCH> expand(
      typename vec<T,N,SIMD_ARCH>::mask mask, vec<T,N,SIMD_ARCH> packed,
      vec<T,N,SIMD_ARCH> prior) noexcept {
    auto bits = detail::SIMD_BACKEND::compaction_mask_bits<N>(mask);
    auto result = detail::SIMD_BACKEND::compact_register<true>(bits, packed, prior);
    if constexpr (N == 2 || N == 3) return vec<T,N,SIMD_ARCH>::from_storage(result.to_storage());
    else return result;
  }

  /// \ingroup vector_memory
  /// Write the first min(capacity,popcount(mask)) selected lanes, in input order.
  /// Return the number WRITTEN, not the total selected count. No element beyond
  /// that prefix is accessed. Null is valid when capacity or the mask is zero.
  template<class T, std::size_t N> requires
    (std::same_as<T,float> || std::same_as<T,std::int32_t> || std::same_as<T,std::uint32_t>) &&
    requires { typename vec<T,N,SIMD_ARCH>::native_type; }
  simd_inline std::size_t compress_store(T * destination, std::size_t capacity,
      typename vec<T,N,SIMD_ARCH>::mask mask, vec<T,N,SIMD_ARCH> value) noexcept {
    auto bits = detail::SIMD_BACKEND::compaction_mask_bits<N>(mask);
    auto selected = std::size_t(std::popcount(bits));
    auto written = capacity < selected ? capacity : selected;
    if (!written) return 0;
#if SIMD_HAS_AVX512F
    if constexpr (N > 1 && (sizeof(value) == 64 || SIMD_HAS_AVX512VL)) {
      // Compress in registers, then store only the bounded contiguous prefix.
      // This also avoids trimming the source predicate when capacity is small.
      auto prefix = (std::uint32_t(1) << written) - 1;
      if constexpr (sizeof(value) == 64) {
        auto packed = _mm512_maskz_compress_epi32(__mmask16(bits),std::bit_cast<__m512i>(value));
        _mm512_mask_storeu_epi32(destination,__mmask16(prefix),packed);
      } else if constexpr (sizeof(value) == 32) {
        auto packed = _mm256_maskz_compress_epi32(__mmask8(bits),std::bit_cast<__m256i>(value));
        _mm256_mask_storeu_epi32(destination,__mmask8(prefix),packed);
      } else {
        auto packed = _mm_maskz_compress_epi32(__mmask8(bits),std::bit_cast<__m128i>(value));
        _mm_mask_storeu_epi32(destination,__mmask8(prefix),packed);
      }
      return written;
    }
#endif
    std::array<T,N> packed;
    compress(mask, value).value.store(packed.data());
    std::memcpy(destination, packed.data(), written * sizeof(T));
    return written;
  }
}
