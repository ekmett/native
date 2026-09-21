#include "native/simd/common_body.h"

namespace native::detail::NATIVE_BACKEND {
  template<class T> using swizzle_word = std::conditional_t<sizeof(T)==1,std::uint8_t,
    std::conditional_t<sizeof(T)==2,std::uint16_t,std::conditional_t<sizeof(T)==4,std::uint32_t,std::uint64_t>>>;
  template<class T> using swizzle_native = swizzle_word<T> __attribute__((ext_vector_type(4)));
  template<std::size_t... I> struct swizzle {
    static constexpr std::size_t size=sizeof...(I);
    template<class Self> using result = std::conditional_t<size==1,typename Self::value_type,
      simd<typename Self::value_type,size,Self::architecture>>;
    static constexpr bool unique=[] {
      constexpr std::size_t indices[]{I...};
      for(std::size_t i=0;i<size;++i)
        for(std::size_t j=0;j<i;++j) if(indices[i]==indices[j]) return false;
      return true;
    }();
    template<std::size_t D> static consteval int slot() {
      constexpr std::size_t indices[]{I...};
      for(std::size_t i=0;i<size;++i) if(indices[i]==D) return int(4+i);
      return int(D);
    }
    template<class Self> static consteval bool readable() {
      using T=typename Self::value_type;
      if constexpr(!((I<Self::lanes)&&...) || !std::is_trivially_copyable_v<T> ||
          !std::default_initializable<T> || !(sizeof(T)==1 || sizeof(T)==2 || sizeof(T)==4 || sizeof(T)==8)) return false;
      else if constexpr(!requires(Self const & self,T * p) { self.template store_memory<1>(p); }) return false;
      else if constexpr(size==1) return std::copy_constructible<T>;
      else return requires(T const * p) {
        sizeof(result<Self>);
        { result<Self>::template load_memory<1>(p) } -> std::same_as<result<Self>>;
      };
    }
    template<class Self> static consteval bool writable() {
      if constexpr(std::is_const_v<Self> || !unique || !readable<Self>()) return false;
      else if constexpr(size>1 && !requires(result<Self> const & rhs,typename Self::value_type * p) { rhs.template store_memory<1>(p); }) return false;
      else return std::is_copy_assignable_v<typename Self::value_type> &&
        std::is_copy_constructible_v<result<Self>> && requires(Self & self,typename Self::value_type const * p) {
          { Self::template load_memory<1>(p) } -> std::same_as<Self>;
          self=Self::template load_memory<1>(p);
        };
    }
    template<class V> static constexpr bool native_words=[] {
      using T=typename V::value_type;
      if constexpr(simd_custom_element<T>) return false;
      else if constexpr(requires { typename V::native_type; }) return sizeof(typename V::native_type)==sizeof(swizzle_native<T>);
      else return false;
    }();
    template<class Self> static native_inline constexpr auto words(Self const & self) {
      using T=typename Self::value_type;
      if constexpr(native_words<Self>) {
        return std::bit_cast<swizzle_native<T>>(self.to_native());
      } else {
        std::array<T,4> values{};
        self.template store_memory<1>(values.data());
        return std::bit_cast<swizzle_native<T>>(values);
      }
    }
    template<class V> static native_inline constexpr V from_words(swizzle_native<typename V::value_type> words) {
      using T=typename V::value_type;
      if constexpr(native_words<V>) {
        return V::from_native(std::bit_cast<typename V::native_type>(words));
      } else {
        auto values=std::bit_cast<std::array<T,4>>(words);
        return V::template load_memory<1>(values.data());
      }
    }
    template<class Self>
    native_nodiscard static native_inline constexpr result<Self> read(Self const & self) {
      using T=typename Self::value_type;
      auto value=words(self);
      if constexpr(size==1) {
        constexpr std::size_t indices[]{I...};
        return std::bit_cast<T>(swizzle_word<T>(value[indices[0]]));
      } else {
        constexpr int indices[]{int(I)...};
        auto shuffled=__builtin_shufflevector(value,value,
          indices[0],indices[1],indices[2%size],indices[3%size]);
        return from_words<result<Self>>(shuffled);
      }
    }
    template<class Self>
    native_inline constexpr static result<Self> write(Self & self,result<Self> rhs) {
      using T=typename Self::value_type;
      auto before=words(self);
      auto replacement=[&] {
        if constexpr(size==1) return swizzle_native<T>{std::bit_cast<swizzle_word<T>>(rhs),0,0,0};
        else return words(rhs);
      }();
      auto shuffled=__builtin_shufflevector(before,replacement,slot<0>(),slot<1>(),slot<2>(),slot<3>());
      self=from_words<Self>(shuffled);
      return rhs;
    }
  };
}
namespace native::detail {
  // Properties are compiler accessors, not proxy objects: a read owns its lanes,
  // and assignment materializes the complete right side before any scatter.
  template<class T,std::size_t N,::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) &&(N<=4)
  struct swizzle_access<T,N,Arch> {
    template<std::size_t K> using result = std::conditional_t<K==1,T,simd<T,K,Arch>>;
#define NATIVE_SWIZZLE_FIELD(NAME,K,...) \
    template<class Self> requires(::NATIVE_BACKEND_NAMESPACE::swizzle<__VA_ARGS__>::template readable<Self>()) \
    native_nodiscard native_inline constexpr result<K> get_##NAME(this Self const & self) { return ::NATIVE_BACKEND_NAMESPACE::swizzle<__VA_ARGS__>::read(self); } \
    template<class Self> requires(::NATIVE_BACKEND_NAMESPACE::swizzle<__VA_ARGS__>::template writable<Self>()) \
    native_inline constexpr result<K> set_##NAME(this Self & self,result<K> rhs) { return ::NATIVE_BACKEND_NAMESPACE::swizzle<__VA_ARGS__>::write(self,rhs); } \
    __declspec(property(get=get_##NAME,put=set_##NAME)) result<K> NAME;
#define NATIVE_SWIZZLE_ROW2(A,I,X,Y,Z,W) \
    NATIVE_SWIZZLE_FIELD(A##X,2,I,0) NATIVE_SWIZZLE_FIELD(A##Y,2,I,1) \
    NATIVE_SWIZZLE_FIELD(A##Z,2,I,2) NATIVE_SWIZZLE_FIELD(A##W,2,I,3)
#define NATIVE_SWIZZLE_ROW3(A,I,B,J,X,Y,Z,W) \
    NATIVE_SWIZZLE_FIELD(A##B##X,3,I,J,0) NATIVE_SWIZZLE_FIELD(A##B##Y,3,I,J,1) \
    NATIVE_SWIZZLE_FIELD(A##B##Z,3,I,J,2) NATIVE_SWIZZLE_FIELD(A##B##W,3,I,J,3)
#define NATIVE_SWIZZLE_PLANE3(A,I,X,Y,Z,W) \
    NATIVE_SWIZZLE_ROW3(A,I,X,0,X,Y,Z,W) NATIVE_SWIZZLE_ROW3(A,I,Y,1,X,Y,Z,W) \
    NATIVE_SWIZZLE_ROW3(A,I,Z,2,X,Y,Z,W) NATIVE_SWIZZLE_ROW3(A,I,W,3,X,Y,Z,W)
#define NATIVE_SWIZZLE_ROW4(A,I,B,J,C,K,X,Y,Z,W) \
    NATIVE_SWIZZLE_FIELD(A##B##C##X,4,I,J,K,0) NATIVE_SWIZZLE_FIELD(A##B##C##Y,4,I,J,K,1) \
    NATIVE_SWIZZLE_FIELD(A##B##C##Z,4,I,J,K,2) NATIVE_SWIZZLE_FIELD(A##B##C##W,4,I,J,K,3)
#define NATIVE_SWIZZLE_PLANE4(A,I,B,J,X,Y,Z,W) \
    NATIVE_SWIZZLE_ROW4(A,I,B,J,X,0,X,Y,Z,W) NATIVE_SWIZZLE_ROW4(A,I,B,J,Y,1,X,Y,Z,W) \
    NATIVE_SWIZZLE_ROW4(A,I,B,J,Z,2,X,Y,Z,W) NATIVE_SWIZZLE_ROW4(A,I,B,J,W,3,X,Y,Z,W)
#define NATIVE_SWIZZLE_CUBE4(A,I,X,Y,Z,W) \
    NATIVE_SWIZZLE_PLANE4(A,I,X,0,X,Y,Z,W) NATIVE_SWIZZLE_PLANE4(A,I,Y,1,X,Y,Z,W) \
    NATIVE_SWIZZLE_PLANE4(A,I,Z,2,X,Y,Z,W) NATIVE_SWIZZLE_PLANE4(A,I,W,3,X,Y,Z,W)
#define NATIVE_SWIZZLE4(X,Y,Z,W) \
    NATIVE_SWIZZLE_FIELD(X,1,0) NATIVE_SWIZZLE_FIELD(Y,1,1) NATIVE_SWIZZLE_FIELD(Z,1,2) NATIVE_SWIZZLE_FIELD(W,1,3) \
    NATIVE_SWIZZLE_ROW2(X,0,X,Y,Z,W) NATIVE_SWIZZLE_ROW2(Y,1,X,Y,Z,W) NATIVE_SWIZZLE_ROW2(Z,2,X,Y,Z,W) NATIVE_SWIZZLE_ROW2(W,3,X,Y,Z,W) \
    NATIVE_SWIZZLE_PLANE3(X,0,X,Y,Z,W) NATIVE_SWIZZLE_PLANE3(Y,1,X,Y,Z,W) NATIVE_SWIZZLE_PLANE3(Z,2,X,Y,Z,W) NATIVE_SWIZZLE_PLANE3(W,3,X,Y,Z,W) \
    NATIVE_SWIZZLE_CUBE4(X,0,X,Y,Z,W) NATIVE_SWIZZLE_CUBE4(Y,1,X,Y,Z,W) NATIVE_SWIZZLE_CUBE4(Z,2,X,Y,Z,W) NATIVE_SWIZZLE_CUBE4(W,3,X,Y,Z,W)
    NATIVE_SWIZZLE4(x,y,z,w)
#undef NATIVE_SWIZZLE4
#undef NATIVE_SWIZZLE_CUBE4
#undef NATIVE_SWIZZLE_PLANE4
#undef NATIVE_SWIZZLE_ROW4
#undef NATIVE_SWIZZLE_PLANE3
#undef NATIVE_SWIZZLE_ROW3
#undef NATIVE_SWIZZLE_ROW2
#undef NATIVE_SWIZZLE_FIELD
  };
}

// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Included once per disjoint backend family under its function target scope.
// Raw SIMD types and operations for the selected compile-time ISA profile.

#if NATIVE_HAS_AVX2 || NATIVE_HAS_AVX512F
#endif
#if NATIVE_HAS_ARM_NEON
#endif

namespace native {
  namespace detail::NATIVE_BACKEND {
    // Register storage can exist without this backend's arithmetic operations.
    template<std::size_t N> inline constexpr bool float_shape = N==1
#if NATIVE_HAS_AVX2 || NATIVE_HAS_ARM_NEON
      || N==2 || N==3 || N==4
#endif
#if NATIVE_HAS_AVX2
      || N==8
#endif
#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ
      || N==16
#endif
      ;
    template <class T> inline constexpr std::size_t mask_lane_bytes = [] {
      if constexpr (::native::simd_custom_element<T>) return sizeof(typename ::native::simd_traits<T>::storage_type);
      else return sizeof(T);
    }();
    template <class T> concept mask_element = ::native::simd_custom_element<T> ||
      (std::is_arithmetic_v<T> && !std::is_const_v<T> && !std::is_volatile_v<T> &&
       (sizeof(T)==1 || sizeof(T)==2 || sizeof(T)==4 || sizeof(T)==8));
    template <std::size_t B,std::size_t N> inline constexpr bool mask_compact =
#if NATIVE_HAS_AVX512F
      N>1 && (B>=4
#if NATIVE_HAS_AVX512BW
      || B==1 || B==2
#endif
      ) && (B*N==64
#if NATIVE_HAS_AVX512VL
      || B*N==16 || B*N==32
#endif
      );
#else
      false;
#endif
    template <std::size_t B,std::size_t N> inline constexpr bool mask_shape = N==1 || (B*N==64 && mask_compact<B,N>)
#if NATIVE_HAS_AVX2
      || B*N==16 || B*N==32
#endif
#if NATIVE_HAS_ARM_NEON
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
      static native_inline native_const constexpr native_type normalize(native_type x) noexcept { return x?native_type(~U(0)):U(0); }
      static native_inline native_const constexpr native_type broadcast(bool x) noexcept { return normalize(U(x)); }
      static native_inline native_const constexpr native_type bit_and(native_type a,native_type b) noexcept { return U(a&b); }
      static native_inline native_const constexpr native_type bit_or(native_type a,native_type b) noexcept { return U(a|b); }
      static native_inline native_const constexpr native_type bit_xor(native_type a,native_type b) noexcept { return U(a^b); }
      static native_inline native_const constexpr native_type bit_not(native_type a) noexcept { return U(~a); }
      static native_inline native_const constexpr bool any(native_type a) noexcept { return a!=0; }
      static native_inline native_const constexpr bool all(native_type a) noexcept { return a==U(~U(0)); }
      static native_inline native_const constexpr std::uint64_t bits(native_type a) noexcept { return a?1:0; }
      static native_inline native_const constexpr native_type from_bits(std::uint64_t a) noexcept { return broadcast((a&1)!=0); }
    };
    template <std::size_t N> struct mask_compact_ops {
      using native_type = std::conditional_t<(N<=8),std::uint8_t,std::conditional_t<(N<=16),std::uint16_t,
        std::conditional_t<(N<=32),std::uint32_t,std::uint64_t>>>;
#if NATIVE_HOST_X86
      native_target("sse2")
#endif
      static native_inline native_const constexpr native_type normalize(native_type a) noexcept {
        if constexpr(N==8 || N==16 || N==32 || N==64) return a;
        else return native_type(std::uint64_t(a)&mask_low_bits<N>);
      }
      static native_inline native_const constexpr native_type broadcast(bool a) noexcept { return a?native_type(mask_low_bits<N>):native_type(0); }
      static native_inline native_const constexpr native_type bit_and(native_type a,native_type b) noexcept {
        if (std::is_constant_evaluated()) return native_type(a&b);
#if NATIVE_HAS_AVX512F
#if NATIVE_HAS_AVX512DQ
        if constexpr(N<=8) return _kand_mask8(a,b);
        else
#endif
        if constexpr(N<=16) return native_type(_kand_mask16(a,b));
#if NATIVE_HAS_AVX512BW
        else if constexpr(N<=32) return _kand_mask32(a,b);
        else return _kand_mask64(a,b);
#else
        else return native_type(a&b);
#endif
#else
        return native_type(a&b);
#endif
      }
      static native_inline native_const constexpr native_type bit_or(native_type a,native_type b) noexcept {
        if (std::is_constant_evaluated()) return native_type(a|b);
#if NATIVE_HAS_AVX512F
#if NATIVE_HAS_AVX512DQ
        if constexpr(N<=8) return _kor_mask8(a,b);
        else
#endif
        if constexpr(N<=16) return native_type(_kor_mask16(a,b));
#if NATIVE_HAS_AVX512BW
        else if constexpr(N<=32) return _kor_mask32(a,b);
        else return _kor_mask64(a,b);
#else
        else return native_type(a|b);
#endif
#else
        return native_type(a|b);
#endif
      }
      static native_inline native_const constexpr native_type bit_xor(native_type a,native_type b) noexcept {
        if (std::is_constant_evaluated()) return native_type(a^b);
#if NATIVE_HAS_AVX512F
#if NATIVE_HAS_AVX512DQ
        if constexpr(N<=8) return _kxor_mask8(a,b);
        else
#endif
        if constexpr(N<=16) return native_type(_kxor_mask16(a,b));
#if NATIVE_HAS_AVX512BW
        else if constexpr(N<=32) return _kxor_mask32(a,b);
        else return _kxor_mask64(a,b);
#else
        else return native_type(a^b);
#endif
#else
        return native_type(a^b);
#endif
      }
      static native_inline native_const constexpr native_type bit_not(native_type a) noexcept {
        if(std::is_constant_evaluated()) return native_type((~std::uint64_t(a))&mask_low_bits<N>);
        // Partial mask8 values complement only their N logical lanes.
        if constexpr(N<8) return bit_xor(a,native_type(mask_low_bits<N>));
#if NATIVE_HAS_AVX512F
#if NATIVE_HAS_AVX512DQ
        else if constexpr(N==8) return _knot_mask8(a);
#endif
        else if constexpr(N<=16) return native_type(_knot_mask16(a));
#if NATIVE_HAS_AVX512BW
        else if constexpr(N<=32) return _knot_mask32(a);
        else return _knot_mask64(a);
#else
        else return native_type(~a);
#endif
#else
        else return native_type(~a);
#endif
      }
      static native_inline native_const constexpr bool any(native_type a) noexcept { return a!=0; }
      static native_inline native_const constexpr bool all(native_type a) noexcept { return a==native_type(mask_low_bits<N>); }
      static native_inline native_const constexpr std::uint64_t bits(native_type a) noexcept { return a; }
      static native_inline native_const constexpr native_type from_bits(std::uint64_t a) noexcept { return normalize(native_type(a)); }
    };
    template <std::size_t B,std::size_t N> struct mask_vector_ops {
#if NATIVE_HAS_AVX2
      using native_type = std::conditional_t<B*N==16,__m128i,__m256i>;
      static native_inline constexpr native_const native_type broadcast(bool a) noexcept {
        if consteval {
          std::array<mask_word<B>,N> lanes{};
          lanes.fill(a?mask_word<B>(~mask_word<B>(0)):0);
          return __builtin_bit_cast(native_type,lanes);
        }
        if constexpr (B*N==16) return _mm_set1_epi32(a?-1:0);
        else return _mm256_set1_epi32(a?-1:0);
      }
      static native_inline constexpr native_const native_type bit_and(native_type a,native_type b) noexcept {
        if consteval {
            auto x=__builtin_bit_cast(std::array<mask_word<B>,N>,a);
            auto y=__builtin_bit_cast(std::array<mask_word<B>,N>,b);
            for(std::size_t i=0;i<N;++i) x[i]=mask_word<B>(x[i]&y[i]);
            return __builtin_bit_cast(native_type,x);
        }
        if constexpr (B*N==16) return _mm_and_si128(a,b);
        else return _mm256_and_si256(a,b);
      }
      static native_inline constexpr native_const native_type bit_or(native_type a,native_type b) noexcept {
        if consteval {
            auto x=__builtin_bit_cast(std::array<mask_word<B>,N>,a);
            auto y=__builtin_bit_cast(std::array<mask_word<B>,N>,b);
            for(std::size_t i=0;i<N;++i) x[i]=mask_word<B>(x[i]|y[i]);
            return __builtin_bit_cast(native_type,x);
        }
        if constexpr (B*N==16) return _mm_or_si128(a,b);
        else return _mm256_or_si256(a,b);
      }
      static native_inline constexpr native_const native_type bit_xor(native_type a,native_type b) noexcept {
        if consteval {
            auto x=__builtin_bit_cast(std::array<mask_word<B>,N>,a);
            auto y=__builtin_bit_cast(std::array<mask_word<B>,N>,b);
            for(std::size_t i=0;i<N;++i) x[i]=mask_word<B>(x[i]^y[i]);
            return __builtin_bit_cast(native_type,x);
        }
        if constexpr (B*N==16) return _mm_xor_si128(a,b);
        else return _mm256_xor_si256(a,b);
      }
      static native_inline constexpr native_const native_type bit_not(native_type a) noexcept { return bit_xor(a,broadcast(true)); }
      static native_inline constexpr native_const native_type normalize(native_type a) noexcept {
        if consteval {
          auto lanes=__builtin_bit_cast(std::array<mask_word<B>,N>,a);
          for(auto & lane:lanes) lane=lane?mask_word<B>(~mask_word<B>(0)):0;
          return __builtin_bit_cast(native_type,lanes);
        }
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
      static native_inline constexpr native_const bool any(native_type a) noexcept {
        if consteval {
          auto lanes=__builtin_bit_cast(std::array<mask_word<B>,N>,a);
          for(auto lane:lanes) if(lane!=0) return true;
          return false;
        }
        if constexpr (B*N==16) return _mm_testz_si128(a,a)==0;
        else return _mm256_testz_si256(a,a)==0;
      }
      static native_inline constexpr native_const bool all(native_type a) noexcept {
        if consteval {
          auto lanes=__builtin_bit_cast(std::array<mask_word<B>,N>,a);
          for(auto lane:lanes) if(lane!=mask_word<B>(~mask_word<B>(0))) return false;
          return true;
        }
        if constexpr (B*N==16) return _mm_movemask_epi8(a)==0xffff;
        else return _mm256_movemask_epi8(a)==-1;
      }
      static native_inline constexpr native_const std::uint64_t bits(native_type a) noexcept {
        if consteval {
          auto lanes=__builtin_bit_cast(std::array<mask_word<B>,N>,a);
          std::uint64_t result=0;
          for(std::size_t i=0;i<N;++i) result|=std::uint64_t(lanes[i]!=0)<<i;
          return result;
        }
        std::uint32_t bytes;
        if constexpr (B*N==16) bytes=std::uint32_t(_mm_movemask_epi8(a));
        else bytes=std::uint32_t(_mm256_movemask_epi8(a));
        std::uint64_t result=0;
        for(std::size_t i=0;i<N;++i) result|=std::uint64_t((bytes>>(i*B))&1)<<i;
        return result;
      }
      static native_inline constexpr native_const native_type from_bits(std::uint64_t bits) noexcept {
        if consteval {
            std::array<mask_word<B>,N> lanes{};
            for(std::size_t i=0;i<N;++i) lanes[i]=((bits>>i)&1)?mask_word<B>(~mask_word<B>(0)):0;
            return __builtin_bit_cast(native_type,lanes);
        }
        std::array<mask_word<B>,N> a{};
        for(std::size_t i=0;i<N;++i) a[i]=((bits>>i)&1)?mask_word<B>(~mask_word<B>(0)):mask_word<B>(0);
        if constexpr (B*N==16) return _mm_loadu_si128(reinterpret_cast<__m128i const *>(a.data()));
        else return _mm256_loadu_si256(reinterpret_cast<__m256i const *>(a.data()));
      }
#elif NATIVE_HAS_ARM_NEON
      using native_type = uint8x16_t;
      static native_inline constexpr native_const native_type broadcast(bool a) noexcept {
        if consteval {
          std::array<mask_word<B>,N> lanes{};
          lanes.fill(a?mask_word<B>(~mask_word<B>(0)):0);
          return __builtin_bit_cast(native_type,lanes);
        }
        return vdupq_n_u8(a?0xff:0);
      }
      static native_inline constexpr native_const native_type bit_and(native_type a,native_type b) noexcept {
        if consteval {
            auto x=__builtin_bit_cast(std::array<mask_word<B>,N>,a);
            auto y=__builtin_bit_cast(std::array<mask_word<B>,N>,b);
            for(std::size_t i=0;i<N;++i) x[i]=mask_word<B>(x[i]&y[i]);
            return __builtin_bit_cast(native_type,x);
        }
        return vandq_u8(a,b);
      }
      static native_inline constexpr native_const native_type bit_or(native_type a,native_type b) noexcept {
        if consteval {
            auto x=__builtin_bit_cast(std::array<mask_word<B>,N>,a);
            auto y=__builtin_bit_cast(std::array<mask_word<B>,N>,b);
            for(std::size_t i=0;i<N;++i) x[i]=mask_word<B>(x[i]|y[i]);
            return __builtin_bit_cast(native_type,x);
        }
        return vorrq_u8(a,b);
      }
      static native_inline constexpr native_const native_type bit_xor(native_type a,native_type b) noexcept {
        if consteval {
            auto x=__builtin_bit_cast(std::array<mask_word<B>,N>,a);
            auto y=__builtin_bit_cast(std::array<mask_word<B>,N>,b);
            for(std::size_t i=0;i<N;++i) x[i]=mask_word<B>(x[i]^y[i]);
            return __builtin_bit_cast(native_type,x);
        }
        return veorq_u8(a,b);
      }
      static native_inline constexpr native_const native_type bit_not(native_type a) noexcept { if consteval { return bit_xor(a,broadcast(true)); } return vmvnq_u8(a); }
      static native_inline constexpr native_const native_type normalize(native_type a) noexcept {
        if consteval {
          auto lanes=__builtin_bit_cast(std::array<mask_word<B>,N>,a);
          for(auto & lane:lanes) lane=lane?mask_word<B>(~mask_word<B>(0)):0;
          return __builtin_bit_cast(native_type,lanes);
        }
        if constexpr(B==1) return bit_not(vceqq_u8(a,vdupq_n_u8(0)));
        else if constexpr(B==2) return bit_not(vreinterpretq_u8_u16(vceqq_u16(vreinterpretq_u16_u8(a),vdupq_n_u16(0))));
        else if constexpr(B==4) return bit_not(vreinterpretq_u8_u32(vceqq_u32(vreinterpretq_u32_u8(a),vdupq_n_u32(0))));
        else return bit_not(vreinterpretq_u8_u64(vceqq_u64(vreinterpretq_u64_u8(a),vdupq_n_u64(0))));
      }
      static native_inline constexpr native_const bool any(native_type a) noexcept {
        if consteval {
          auto lanes=__builtin_bit_cast(std::array<mask_word<B>,N>,a);
          for(auto lane:lanes) if(lane!=0) return true;
          return false;
        }
        return vmaxvq_u8(a)!=0;
      }
      static native_inline constexpr native_const bool all(native_type a) noexcept {
        if consteval {
          auto lanes=__builtin_bit_cast(std::array<mask_word<B>,N>,a);
          for(auto lane:lanes) if(lane!=mask_word<B>(~mask_word<B>(0))) return false;
          return true;
        }
        return vminvq_u8(a)==0xff;
      }
      static native_inline constexpr native_const std::uint64_t bits(native_type a) noexcept {
        if consteval {
          auto lanes=__builtin_bit_cast(std::array<mask_word<B>,N>,a);
          std::uint64_t result=0;
          for(std::size_t i=0;i<N;++i) result|=std::uint64_t(lanes[i]!=0)<<i;
          return result;
        }
        std::array<std::uint8_t,16> bytes{};vst1q_u8(bytes.data(),a);
        std::uint64_t result=0;
        for(std::size_t i=0;i<N;++i) result|=std::uint64_t(bytes[i*B]!=0)<<i;
        return result;
      }
      static native_inline constexpr native_const native_type from_bits(std::uint64_t bits) noexcept {
        if consteval {
            std::array<mask_word<B>,N> lanes{};
            for(std::size_t i=0;i<N;++i) lanes[i]=((bits>>i)&1)?mask_word<B>(~mask_word<B>(0)):0;
            return __builtin_bit_cast(native_type,lanes);
        }
        std::array<std::uint8_t,16> a{};
        for(std::size_t i=0;i<N;++i) for(std::size_t j=0;j<B;++j) a[i*B+j]=((bits>>i)&1)?0xff:0;
        return vld1q_u8(a.data());
      }
#endif
    };
    template<std::size_t B,std::size_t N> struct mask_vector512_ops {
#if NATIVE_HAS_AVX512F
      using native_type=__m512i;
      static native_inline constexpr native_const native_type broadcast(bool a) noexcept {
        if consteval {
          std::array<mask_word<B>,N> lanes{};
          lanes.fill(a?mask_word<B>(~mask_word<B>(0)):0);
          return __builtin_bit_cast(native_type,lanes);
        }
        return _mm512_set1_epi32(a?-1:0);
      }
      static native_inline constexpr native_const native_type bit_and(native_type a,native_type b) noexcept {
        if consteval {
            auto x=__builtin_bit_cast(std::array<mask_word<B>,N>,a);
            auto y=__builtin_bit_cast(std::array<mask_word<B>,N>,b);
            for(std::size_t i=0;i<N;++i) x[i]=mask_word<B>(x[i]&y[i]);
            return __builtin_bit_cast(native_type,x);
        }
        return _mm512_and_si512(a,b);
      }
      static native_inline constexpr native_const native_type bit_or(native_type a,native_type b) noexcept {
        if consteval {
            auto x=__builtin_bit_cast(std::array<mask_word<B>,N>,a);
            auto y=__builtin_bit_cast(std::array<mask_word<B>,N>,b);
            for(std::size_t i=0;i<N;++i) x[i]=mask_word<B>(x[i]|y[i]);
            return __builtin_bit_cast(native_type,x);
        }
        return _mm512_or_si512(a,b);
      }
      static native_inline constexpr native_const native_type bit_xor(native_type a,native_type b) noexcept {
        if consteval {
            auto x=__builtin_bit_cast(std::array<mask_word<B>,N>,a);
            auto y=__builtin_bit_cast(std::array<mask_word<B>,N>,b);
            for(std::size_t i=0;i<N;++i) x[i]=mask_word<B>(x[i]^y[i]);
            return __builtin_bit_cast(native_type,x);
        }
        return _mm512_xor_si512(a,b);
      }
      static native_inline constexpr native_const native_type bit_not(native_type a) noexcept { return bit_xor(a,broadcast(true)); }
      static native_inline constexpr native_const std::uint64_t bits(native_type a) noexcept {
        if consteval {
          auto lanes=__builtin_bit_cast(std::array<mask_word<B>,N>,a);
          std::uint64_t result=0;
          for(std::size_t i=0;i<N;++i) result|=std::uint64_t(lanes[i]!=0)<<i;
          return result;
        }
        if constexpr(B==1) return _mm512_cmpneq_epi8_mask(a,_mm512_setzero_si512());
        else if constexpr(B==2) return _mm512_cmpneq_epi16_mask(a,_mm512_setzero_si512());
        else if constexpr(B==4) return _mm512_cmpneq_epi32_mask(a,_mm512_setzero_si512());
        else return _mm512_cmpneq_epi64_mask(a,_mm512_setzero_si512());
      }
      static native_inline constexpr native_const native_type from_bits(std::uint64_t a) noexcept {
        if consteval {
            std::array<mask_word<B>,N> lanes{};
            for(std::size_t i=0;i<N;++i) lanes[i]=((a>>i)&1)?mask_word<B>(~mask_word<B>(0)):0;
            return __builtin_bit_cast(native_type,lanes);
        }
        if constexpr(B==1) return _mm512_maskz_set1_epi8(__mmask64(a),-1);
        else if constexpr(B==2) return _mm512_maskz_set1_epi16(__mmask32(a),-1);
        else if constexpr(B==4) return _mm512_maskz_set1_epi32(__mmask16(a),-1);
        else return _mm512_maskz_set1_epi64(__mmask8(a),-1);
      }
      static native_inline constexpr native_const native_type normalize(native_type a) noexcept {
        if consteval {
          auto lanes=__builtin_bit_cast(std::array<mask_word<B>,N>,a);
          for(auto & lane:lanes) lane=lane?mask_word<B>(~mask_word<B>(0)):0;
          return __builtin_bit_cast(native_type,lanes);
        }
        return from_bits(bits(a));
      }
      static native_inline constexpr native_const bool any(native_type a) noexcept {
        if consteval {
          auto lanes=__builtin_bit_cast(std::array<mask_word<B>,N>,a);
          for(auto lane:lanes) if(lane!=0) return true;
          return false;
        }
        return bits(a)!=0;
      }
      static native_inline constexpr native_const bool all(native_type a) noexcept {
        if consteval {
          auto lanes=__builtin_bit_cast(std::array<mask_word<B>,N>,a);
          for(auto lane:lanes) if(lane!=mask_word<B>(~mask_word<B>(0))) return false;
          return true;
        }
        return bits(a)==mask_low_bits<N>;
      }
#endif
    };
    template<std::size_t B,std::size_t N> using mask_full_ops=std::conditional_t<N==1,mask_scalar_ops<mask_word<B>>,
      std::conditional_t<B*N==64,mask_vector512_ops<B,N>,mask_vector_ops<B,N>>>;
    template<std::size_t N> inline constexpr bool predicate_shape=
#if NATIVE_HAS_AVX512F
      (N==1 || N==2 || N==3 || N==4 || N==8 || N==16
#if NATIVE_HAS_AVX512BW
       || N==32 || N==64
#endif
      );
#else
      false;
#endif
  }

  template<class U,std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::mask_shape<sizeof(U),N>
  /// \ingroup masks
  /// A full-vector mask whose lanes are canonical zero/all-one values.
  /// `from_native` normalizes whole lanes; `unsafe_from_native` requires that
  /// invariant already. `to_bitset` places logical lane i in bit i.
  struct alignas(typename ::NATIVE_BACKEND_NAMESPACE::mask_full_ops<sizeof(U),N>::native_type) simd<mask_lane<U>,N,Arch> : detail::swizzle_access<mask_lane<U>,N,Arch> {
    static constexpr isa<> architecture=Arch;
    template <class X> using rebind = simd<X,N,Arch>;
    /// Load exactly the logical lanes; the template alignment is a caller promise, never permission to read padding.
    template <std::size_t A = 1>
    native_nodiscard static native_inline constexpr simd load_memory(mask_lane<U> const * p) noexcept { return load(p); }
    /// Store exactly the logical lanes; the template alignment is a caller promise, never permission to write padding.
    template <std::size_t A = 1>
    native_inline constexpr void store_memory(mask_lane<U> * p) const noexcept { store(p); }

    using value_type=mask_lane<U>;
    using storage_type=U;
    using ops=::NATIVE_BACKEND_NAMESPACE::mask_full_ops<sizeof(U),N>;
    using native_type=typename ops::native_type;
    using mask_type=simd;
    using mask = mask_type;
    using predicate_type = predicate<N,Arch>;
    static constexpr std::size_t lanes=N;
    static constexpr bool compact=false;
    /// Initialize every logical lane to false.
    native_inline constexpr simd() noexcept : value_(ops::broadcast(false)) {}
    /// Broadcast the supplied truth value to every logical lane.
    explicit native_inline constexpr simd(bool value) noexcept : value_(ops::broadcast(value)) {}
    /// Broadcast the supplied truth value to every logical lane.
    native_inline constexpr simd(value_type value) noexcept : simd(value.to_bool()) {}
    /// Construct logical lanes in argument order. Any element conversions determine the exception specification.
    template<class... X> requires(N>1 && sizeof...(X)==N) && (std::same_as<X,value_type>&&...)
    native_inline constexpr simd(X... values) noexcept : simd(std::array<value_type,N>{values...}) {}
    /// Read all logical lanes from an unaligned element pointer.
    explicit native_inline constexpr simd(std::array<value_type,N> const & values) noexcept : simd(load(values.data())) {}
    // Safe native import interprets each whole U-sized lane as nonzero truth.
    /// Import native mask storage, normalizing nonzero lanes to true.
    native_nodiscard static native_inline native_const constexpr simd from_native(native_type value) noexcept { return simd(raw{},ops::normalize(value)); }
    // Caller promises zero/all-ones for EVERY lane. Arbitrary bitselect masks
    // must use bit_select instead, never this canonical predicate domain.
    /// Adopt storage with the precondition that every mask lane is canonical zero or all ones.
    native_nodiscard static native_inline native_const constexpr simd unsafe_from_native(native_type value) noexcept { return simd(raw{},value); }
    /// Return the native storage representation.
    native_nodiscard native_inline native_pure constexpr native_type to_native() const noexcept { return value_; }
    /// Import lane i from bit i, clearing bits above the logical lane count.
    native_nodiscard static native_inline native_const constexpr simd from_bitset(std::uint64_t value) noexcept { return simd(raw{},ops::from_bits(value)); }
    /// Pack each logical lane truth value into bit i; higher bits are zero.
    native_nodiscard native_inline native_pure constexpr std::uint64_t to_bitset() const noexcept { return ops::bits(value_); }
    /// Read all logical lanes from an unaligned element pointer.
    native_nodiscard static native_inline constexpr native_pure simd load(value_type const * p) noexcept {
      if consteval {
        std::uint64_t bits=0;
        for(std::size_t i=0;i<N;++i) bits|=std::uint64_t(p[i].to_bool())<<i;
        return from_bitset(bits);
      }
      native_type value;std::memcpy(&value,static_cast<void const *>(p),sizeof(value));return unsafe_from_native(value);
    }
    /// Write all logical lanes to an unaligned element pointer.
    native_inline constexpr void store(value_type * p) const noexcept {
      if consteval {
        auto bits=to_bitset();
        for(std::size_t i=0;i<N;++i) p[i]=value_type(((bits>>i)&1)!=0);
        return;
      }
      std::memcpy(static_cast<void *>(p),&value_,sizeof(value_));
    }
    /// Invert each lane truth value, preserving the mask representation.
    native_nodiscard friend native_inline native_const constexpr simd operator~(simd a) noexcept { return simd(raw{},ops::bit_not(a.value_)); }
    /// Return the lane-wise logical complement, retaining this mask type.
    native_nodiscard friend native_inline native_const constexpr simd operator!(simd a) noexcept { return ~a; }
    /// Bitwise AND of corresponding lane representations.
    native_nodiscard friend native_inline native_const constexpr simd operator&(simd a,simd b) noexcept { return simd(raw{},ops::bit_and(a.value_,b.value_)); }
    /// Bitwise OR of corresponding lane representations.
    native_nodiscard friend native_inline native_const constexpr simd operator|(simd a,simd b) noexcept { return simd(raw{},ops::bit_or(a.value_,b.value_)); }
    /// Bitwise XOR of corresponding lane representations.
    native_nodiscard friend native_inline native_const constexpr simd operator^(simd a,simd b) noexcept { return simd(raw{},ops::bit_xor(a.value_,b.value_)); }
    /// Return a mask whose lanes are true where `a == b` holds.
    native_nodiscard friend native_inline native_const constexpr simd operator==(simd a,simd b) noexcept { return ~(a^b); }
    /// Return a mask whose lanes are true where `a != b` holds.
    native_nodiscard friend native_inline native_const constexpr simd operator!=(simd a,simd b) noexcept { return a^b; }
    /// Apply the corresponding lane-wise AND operation in place and return *this.
    native_inline constexpr simd & operator&=(simd b) noexcept { return *this=*this&b; }
    /// Apply the corresponding lane-wise OR operation in place and return *this.
    native_inline constexpr simd & operator|=(simd b) noexcept { return *this=*this|b; }
    /// Apply the corresponding lane-wise XOR operation in place and return *this.
    native_inline constexpr simd & operator^=(simd b) noexcept { return *this=*this^b; }
    /// Return whether at least one logical lane is true.
    native_nodiscard friend native_inline native_const constexpr bool any(simd a) noexcept { return ops::any(a.value_); }
    /// Return whether every logical lane is true.
    native_nodiscard friend native_inline native_const constexpr bool all(simd a) noexcept { return ops::all(a.value_); }
    /// Return true exactly when no logical lane is true.
    native_nodiscard friend native_inline native_const constexpr bool none(simd a) noexcept { return !any(a); }
    /// Choose a in true lanes and b in false lanes; both values are already evaluated.
    native_nodiscard friend native_inline native_const constexpr simd select(simd predicate,simd a,simd b) noexcept { return (predicate&a)|(~predicate&b); }
  private:
    struct raw {};
    native_inline constexpr simd(raw,native_type value) noexcept : value_(value) {}
    native_type value_;
  };

  /// \ingroup masks
  /// Compact comparison storage. Every import clears bits above the lane count,
  /// including the compatibility `unsafe_from_native` spelling.

  template<std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::predicate_shape<N>
  struct predicate<N,Arch> {
    static constexpr isa<> architecture=Arch;
    using ops=::NATIVE_BACKEND_NAMESPACE::mask_compact_ops<N>;
    using native_type=typename ops::native_type;
    static constexpr std::size_t lanes=N;
    static constexpr bool compact=true;
    /// Initialize every logical lane to false.
    native_inline constexpr predicate() noexcept = default;
    /// Broadcast the supplied truth value to every logical lane.
    explicit native_inline constexpr predicate(bool value) noexcept : value_(ops::broadcast(value)) {}
    /// Import compact mask bits and clear bits above the lane count.
#if NATIVE_HOST_X86
    native_target("sse2")
#endif
    native_nodiscard static native_inline native_const constexpr predicate from_native(native_type value) noexcept { return predicate(raw{},ops::normalize(value)); }
    /// Import compact bits and clear bits above the lane count, just like from_native.
#if NATIVE_HOST_X86
    native_target("sse2")
#endif
    native_nodiscard static native_inline native_const constexpr predicate unsafe_from_native(native_type value) noexcept { return from_native(value); }
    /// Return the native storage representation.
#if NATIVE_HOST_X86
    native_target("sse2")
#endif
    native_nodiscard native_inline native_pure constexpr native_type to_native() const noexcept { return value_; }
    /// Import lane truth from the low logical-lane bits.
#if NATIVE_HOST_X86
    native_target("sse2")
#endif
    native_nodiscard static native_inline native_const constexpr predicate from_bitset(std::uint64_t value) noexcept { return from_native(native_type(value)); }
    /// Pack lane truth into low bits, with lane zero in bit zero.
#if NATIVE_HOST_X86
    // Compact masks hold scalar bits even when their vector profile is stronger.
    native_target("sse2")
#endif
    native_nodiscard native_inline native_pure constexpr std::uint64_t to_bitset() const noexcept { return value_; }
    /// Invert each lane truth value, preserving the mask representation.
    native_nodiscard friend native_inline native_const constexpr predicate operator~(predicate a) noexcept { return predicate(raw{},ops::bit_not(a.value_)); }
    /// Return the lane-wise logical complement, retaining this mask type.
    native_nodiscard friend native_inline native_const constexpr predicate operator!(predicate a) noexcept { return ~a; }
    /// Bitwise AND of corresponding lane representations.
    native_nodiscard friend native_inline native_const constexpr predicate operator&(predicate a,predicate b) noexcept { return predicate(raw{},ops::bit_and(a.value_,b.value_)); }
    /// Bitwise OR of corresponding lane representations.
    native_nodiscard friend native_inline native_const constexpr predicate operator|(predicate a,predicate b) noexcept { return predicate(raw{},ops::bit_or(a.value_,b.value_)); }
    /// Bitwise XOR of corresponding lane representations.
    native_nodiscard friend native_inline native_const constexpr predicate operator^(predicate a,predicate b) noexcept { return predicate(raw{},ops::bit_xor(a.value_,b.value_)); }
    /// Return a mask whose lanes are true where `a == b` holds.
    native_nodiscard friend native_inline native_const constexpr predicate operator==(predicate a,predicate b) noexcept { return ~(a^b); }
    /// Return a mask whose lanes are true where `a != b` holds.
    native_nodiscard friend native_inline native_const constexpr predicate operator!=(predicate a,predicate b) noexcept { return a^b; }
    /// Apply the corresponding lane-wise AND operation in place and return *this.
    native_inline constexpr predicate & operator&=(predicate b) noexcept { return *this=*this&b; }
    /// Apply the corresponding lane-wise OR operation in place and return *this.
    native_inline constexpr predicate & operator|=(predicate b) noexcept { return *this=*this|b; }
    /// Apply the corresponding lane-wise XOR operation in place and return *this.
    native_inline constexpr predicate & operator^=(predicate b) noexcept { return *this=*this^b; }
    /// Return whether at least one logical lane is true.
    native_nodiscard friend native_inline native_const constexpr bool any(predicate a) noexcept { return a.value_!=0; }
    /// Return whether every logical lane is true.
    native_nodiscard friend native_inline native_const constexpr bool all(predicate a) noexcept { return ops::all(a.value_); }
    /// Return true exactly when no logical lane is true.
    native_nodiscard friend native_inline native_const constexpr bool none(predicate a) noexcept { return !any(a); }
    /// Choose a in true lanes and b in false lanes; both values are already evaluated.
    native_nodiscard friend native_inline native_const constexpr predicate select(predicate p,predicate a,predicate b) noexcept { return (p&a)|(~p&b); }
  private:
    struct raw {};
#if NATIVE_HOST_X86
    native_target("sse2")
#endif
    native_inline constexpr predicate(raw,native_type value) noexcept : value_(value) {}
    native_type value_=0;
  };

  namespace detail::NATIVE_BACKEND {
    template<class T> using mask_lane_for=mask_lane<mask_word<mask_lane_bytes<T>>>;
    template<class T,std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) using comparison_mask=std::conditional_t<mask_compact<mask_lane_bytes<T>,N>,
      predicate<N,Arch>,simd<mask_lane_for<T>,N,Arch>>;
  }
  namespace detail::NATIVE_BACKEND {
  template<::native::isa<> Arch, class T,std::size_t N,class U,std::size_t A=1,simd_access Access=simd_access::ordinary>
    requires NATIVE_ARCH_REQUIRES(Arch) && simd_mask_element<T> && std::same_as<T,U> && requires { typename simd<T,N,Arch>::native_type; }
  native_nodiscard native_inline constexpr native_pure simd<T,N,Arch> load_simd(U const * p,simd_memory<A,Access> = {}) noexcept { return simd<T,N,Arch>::load(p); }
  template<class U,class T,std::size_t N,std::size_t A=1,simd_access Access=simd_access::ordinary, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && simd_mask_element<T> && std::same_as<T,U> && requires { typename simd<T,N,Arch>::native_type; }
  native_inline constexpr void store_simd(U * p,simd<T,N,Arch> value,simd_memory<A,Access> = {}) noexcept { value.store(p); }
  template<::native::isa<> Arch, class T,std::size_t N,class U,std::size_t A=1,simd_access Access=simd_access::ordinary>
    requires NATIVE_ARCH_REQUIRES(Arch) && simd_mask_element<T> && std::same_as<T,U> && requires { typename simd<T,N,Arch>::native_type; }
  native_nodiscard native_inline constexpr native_pure simd<T,N,Arch> load_simd_partial(U const * p,std::size_t count,T fill=T{},simd_memory<A,Access> = {}) noexcept native_diagnose_if(count > N,"partial SIMD count exceeds the lane count") {
    std::array<T,N> a;a.fill(fill);for(std::size_t i=0;i<count;++i)a[i]=p[i];return simd<T,N,Arch>::load(a.data());
  }
  template<class U,class T,std::size_t N,std::size_t A=1,simd_access Access=simd_access::ordinary, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && simd_mask_element<T> && std::same_as<T,U> && requires { typename simd<T,N,Arch>::native_type; }
  native_inline constexpr void store_simd_partial(U * p,simd<T,N,Arch> value,std::size_t count,simd_memory<A,Access> = {}) noexcept native_diagnose_if(count > N,"partial SIMD count exceeds the lane count") {
    std::array<T,N> a;value.store(a.data());for(std::size_t i=0;i<count;++i)p[i]=a[i];
  }
  }
  /// \ingroup masks
  /// Change full-mask lane width without changing lane truth or lane count.
  template<class U,class T,std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && simd_mask_element<U> && simd_mask_element<T> &&
    requires { typename simd<U,N,Arch>::native_type; typename simd<T,N,Arch>::native_type; }
  native_nodiscard native_inline constexpr native_const simd<U,N,Arch> mask_cast(simd<T,N,Arch> value) noexcept {
    if constexpr(sizeof(U)==sizeof(T)) return simd<U,N,Arch>::unsafe_from_native(value.to_native());
    else return simd<U,N,Arch>::from_bitset(value.to_bitset());
  }
  /// \ingroup masks
  /// Compress full-vector truth into a supported compact predicate.
  template<class T,std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && simd_mask_element<T> && ::NATIVE_BACKEND_NAMESPACE::predicate_shape<N> && requires { typename simd<T,N,Arch>::native_type; }
  native_nodiscard native_inline constexpr native_const predicate<N,Arch> to_predicate(simd<T,N,Arch> value) noexcept {
    if consteval { return predicate<N,Arch>::from_bitset(value.to_bitset()); }
#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512VL
    if constexpr(sizeof(T)*N==16 && ::NATIVE_BACKEND_NAMESPACE::mask_compact<sizeof(T),N>) {
      auto x=value.to_native();auto z=_mm_setzero_si128();
      if constexpr(sizeof(T)==1) return predicate<N,Arch>::from_native(_mm_cmpneq_epi8_mask(x,z));
      else if constexpr(sizeof(T)==2) return predicate<N,Arch>::from_native(_mm_cmpneq_epi16_mask(x,z));
      else if constexpr(sizeof(T)==4) return predicate<N,Arch>::from_native(_mm_cmpneq_epi32_mask(x,z));
      else return predicate<N,Arch>::from_native(_mm_cmpneq_epi64_mask(x,z));
    } else if constexpr(sizeof(T)*N==32 && ::NATIVE_BACKEND_NAMESPACE::mask_compact<sizeof(T),N>) {
      auto x=value.to_native();auto z=_mm256_setzero_si256();
      if constexpr(sizeof(T)==1) return predicate<N,Arch>::from_native(_mm256_cmpneq_epi8_mask(x,z));
      else if constexpr(sizeof(T)==2) return predicate<N,Arch>::from_native(_mm256_cmpneq_epi16_mask(x,z));
      else if constexpr(sizeof(T)==4) return predicate<N,Arch>::from_native(_mm256_cmpneq_epi32_mask(x,z));
      else return predicate<N,Arch>::from_native(_mm256_cmpneq_epi64_mask(x,z));
    } else
#endif
    return predicate<N,Arch>::from_bitset(value.to_bitset());
  }
  /// \ingroup masks
  /// Expand a predicate to canonical zero/all-one lanes of mask element `T`.
  template<class T,std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && simd_mask_element<T> && ::NATIVE_BACKEND_NAMESPACE::predicate_shape<N> && requires { typename simd<T,N,Arch>::native_type; }
  native_nodiscard native_inline constexpr native_const simd<T,N,Arch> to_vector_mask(predicate<N,Arch> value) noexcept {
    if consteval { return simd<T,N,Arch>::from_bitset(value.to_bitset()); }
#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512VL
    if constexpr(sizeof(T)*N==16 && ::NATIVE_BACKEND_NAMESPACE::mask_compact<sizeof(T),N>) {
      auto k=value.to_native();
      if constexpr(sizeof(T)==1) return simd<T,N,Arch>::unsafe_from_native(_mm_maskz_set1_epi8(k,-1));
      else if constexpr(sizeof(T)==2) return simd<T,N,Arch>::unsafe_from_native(_mm_maskz_set1_epi16(k,-1));
      else if constexpr(sizeof(T)==4) return simd<T,N,Arch>::unsafe_from_native(_mm_maskz_set1_epi32(k,-1));
      else return simd<T,N,Arch>::unsafe_from_native(_mm_maskz_set1_epi64(k,-1));
    } else if constexpr(sizeof(T)*N==32 && ::NATIVE_BACKEND_NAMESPACE::mask_compact<sizeof(T),N>) {
      auto k=value.to_native();
      if constexpr(sizeof(T)==1) return simd<T,N,Arch>::unsafe_from_native(_mm256_maskz_set1_epi8(k,-1));
      else if constexpr(sizeof(T)==2) return simd<T,N,Arch>::unsafe_from_native(_mm256_maskz_set1_epi16(k,-1));
      else if constexpr(sizeof(T)==4) return simd<T,N,Arch>::unsafe_from_native(_mm256_maskz_set1_epi32(k,-1));
      else return simd<T,N,Arch>::unsafe_from_native(_mm256_maskz_set1_epi64(k,-1));
    } else
#endif
    return simd<T,N,Arch>::from_bitset(value.to_bitset());
  }
}


namespace native {
  namespace detail::NATIVE_BACKEND {
    template<std::size_t N> native_nodiscard native_inline native_const constexpr auto bool_ones() noexcept {
      if consteval {
        using V=typename mask_full_ops<1,N>::native_type;
        std::array<std::uint8_t,N> lanes{}; lanes.fill(1);
        return __builtin_bit_cast(V,lanes);
      }
      if constexpr(N==1) return std::uint8_t(1);
#if NATIVE_HAS_AVX2
      else if constexpr(N==16) return _mm_set1_epi8(1);
      else if constexpr(N==32) return _mm256_set1_epi8(1);
#endif
#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512BW
      else if constexpr(N==64) return _mm512_set1_epi8(1);
#endif
#if NATIVE_HAS_ARM_NEON
      else return vdupq_n_u8(1);
#endif
    }
  }
  /// \ingroup masks
  /// Boolean data stores one byte, zero or one, per lane.
  /// Use explicit conversions to cross into full masks or compact predicates.
  template<std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::mask_shape<1,N>
  struct simd<bool,N,Arch> : detail::swizzle_access<bool,N,Arch> {
    static constexpr isa<> architecture=Arch;
    template <class X> using rebind = simd<X,N,Arch>;
    /// Load exactly the logical lanes; the template alignment is a caller promise, never permission to read padding.
    template <std::size_t A = 1>
    native_nodiscard static native_inline constexpr simd load_memory(bool const * p) noexcept { return load(p); }
    /// Store exactly the logical lanes; the template alignment is a caller promise, never permission to write padding.
    template <std::size_t A = 1>
    native_inline constexpr void store_memory(bool * p) const noexcept { store(p); }

    using value_type=bool;
    using storage_type=std::uint8_t;
    using ops=::NATIVE_BACKEND_NAMESPACE::mask_full_ops<1,N>;
    using native_type=typename ops::native_type;
    using vector_mask_type=simd<mask8,N,Arch>;
    using mask_type=::NATIVE_BACKEND_NAMESPACE::comparison_mask<bool,N,Arch>;
    using mask = mask_type;
    using predicate_type = predicate<N,Arch>;
    static constexpr std::size_t lanes=N;
    /// Initialize every logical lane to false.
    native_inline constexpr simd() noexcept : value_(ops::broadcast(false)) {}
    /// Broadcast the supplied truth value to every logical lane.
    explicit native_inline constexpr simd(bool value) noexcept : value_(value?::NATIVE_BACKEND_NAMESPACE::bool_ones<N>():ops::broadcast(false)) {}
    /// Construct logical lanes in argument order. Any element conversions determine the exception specification.
    template<class... X> requires(N>1 && sizeof...(X)==N) && (std::same_as<X,bool>&&...)
    native_inline constexpr simd(X... value) noexcept : simd(std::array<bool,N>{value...}) {}
    /// Read all logical lanes from an unaligned element pointer.
    explicit native_inline constexpr simd(std::array<bool,N> const & value) noexcept : simd(load(value.data())) {}
    /// Import native byte lanes, converting each nonzero byte to Boolean one.
    native_nodiscard static native_inline native_const constexpr simd from_native(native_type value) noexcept {
      return simd(raw{},ops::bit_and(ops::normalize(value),::NATIVE_BACKEND_NAMESPACE::bool_ones<N>()));
    }
    // Caller promises EVERY byte is 0 or 1, never an all-ones mask byte.
    /// Adopt storage with the precondition that every Boolean byte is zero or one.
    native_nodiscard static native_inline native_const constexpr simd unsafe_from_native(native_type value) noexcept { return simd(raw{},value); }
    /// Return the native storage representation.
    native_nodiscard native_inline native_pure constexpr native_type to_native() const noexcept { return value_; }
    /// Read all logical lanes from an unaligned element pointer.
    native_nodiscard static native_inline constexpr native_pure simd load(bool const * p) noexcept {
      std::array<std::uint8_t,N> bytes{};
      for(std::size_t i=0;i<N;++i) bytes[i]=p[i]?1:0;
      if consteval { return unsafe_from_native(__builtin_bit_cast(native_type,bytes)); }
      native_type value;std::memcpy(&value,bytes.data(),sizeof(value));return unsafe_from_native(value);
    }
    /// Write all logical lanes to an unaligned element pointer.
    native_inline constexpr void store(bool * p) const noexcept {
      std::array<std::uint8_t,N> bytes{};
      if consteval { bytes=__builtin_bit_cast(decltype(bytes),value_); }
      else { std::memcpy(bytes.data(),&value_,sizeof(value_)); }
      for(std::size_t i=0;i<N;++i) p[i]=bytes[i]!=0;
    }
    // The caller supplies 0 <= count <= N. Zero touches no pointer, even null.
    /// Read exactly n logical lanes and fill the remainder; require n <= lanes. For n == 0, p may be null.
    native_nodiscard static native_inline constexpr native_pure simd load_partial(bool const * p,std::size_t count,bool fill=false) noexcept native_diagnose_if(count > simd::lanes,"partial SIMD count exceeds the lane count") {
      std::array<bool,N> values;values.fill(fill);
      for(std::size_t i=0;i<count;++i) values[i]=p[i];
      return load(values.data());
    }
    /// Write exactly n logical lanes; require n <= lanes. For n == 0, p may be null.
    native_inline constexpr void store_partial(bool * p,std::size_t count) const noexcept native_diagnose_if(count > simd::lanes,"partial SIMD count exceeds the lane count") {
      std::array<bool,N> values;store(values.data());
      for(std::size_t i=0;i<count;++i) p[i]=values[i];
    }
    /// Return the lane-wise logical complement, retaining this mask type.
    native_nodiscard friend native_inline native_const constexpr simd operator!(simd a) noexcept { return simd(raw{},ops::bit_xor(a.value_,::NATIVE_BACKEND_NAMESPACE::bool_ones<N>())); }
    /// Invert each lane truth value, preserving the mask representation.
    native_nodiscard friend native_inline native_const constexpr simd operator~(simd a) noexcept { return !a; }
    /// Bitwise AND of corresponding lane representations.
    native_nodiscard friend native_inline native_const constexpr simd operator&(simd a,simd b) noexcept { return simd(raw{},ops::bit_and(a.value_,b.value_)); }
    /// Bitwise OR of corresponding lane representations.
    native_nodiscard friend native_inline native_const constexpr simd operator|(simd a,simd b) noexcept { return simd(raw{},ops::bit_or(a.value_,b.value_)); }
    /// Bitwise XOR of corresponding lane representations.
    native_nodiscard friend native_inline native_const constexpr simd operator^(simd a,simd b) noexcept { return simd(raw{},ops::bit_xor(a.value_,b.value_)); }
    /// Return a mask whose lanes are true where `a != b` holds.
    native_nodiscard friend native_inline native_const constexpr mask_type operator!=(simd a,simd b) noexcept {
      auto x=ops::bit_xor(a.value_,b.value_);
      if consteval { return mask_type::from_bitset(vector_mask_type::from_native(x).to_bitset()); }
#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512BW
      if constexpr(N==64) return mask_type::from_native(_mm512_cmpneq_epi8_mask(x,_mm512_setzero_si512()));
#if NATIVE_HAS_AVX512VL
      else if constexpr(N==32) return mask_type::from_native(_mm256_cmpneq_epi8_mask(x,_mm256_setzero_si256()));
      else if constexpr(N==16) return mask_type::from_native(_mm_cmpneq_epi8_mask(x,_mm_setzero_si128()));
#endif
      else
#endif
      return vector_mask_type::from_native(x);
    }
    /// Return a mask whose lanes are true where `a == b` holds.
    native_nodiscard friend native_inline native_const constexpr mask_type operator==(simd a,simd b) noexcept { return ~(a!=b); }
    /// Apply the corresponding lane-wise AND operation in place and return *this.
    native_inline constexpr simd & operator&=(simd b) noexcept { return *this=*this&b; }
    /// Apply the corresponding lane-wise OR operation in place and return *this.
    native_inline constexpr simd & operator|=(simd b) noexcept { return *this=*this|b; }
    /// Apply the corresponding lane-wise XOR operation in place and return *this.
    native_inline constexpr simd & operator^=(simd b) noexcept { return *this=*this^b; }
    /// Return whether at least one logical lane is true.
    native_nodiscard friend native_inline native_const constexpr bool any(simd a) noexcept { return ops::any(a.value_); }
    /// Return whether every logical lane is true.
    native_nodiscard friend native_inline native_const constexpr bool all(simd a) noexcept { return none(!a); }
    /// Return true exactly when no logical lane is true.
    native_nodiscard friend native_inline native_const constexpr bool none(simd a) noexcept { return !any(a); }
    /// Choose a in true lanes and b in false lanes; both values are already evaluated.
    template<class M> requires(std::same_as<M,mask_type> || std::same_as<M,vector_mask_type>)
    native_nodiscard friend native_inline native_const constexpr simd select(M m,simd a,simd b) noexcept {
      if consteval {
        auto mask=vector_mask_type::from_bitset(m.to_bitset()).to_native();
        return simd(raw{},ops::bit_or(ops::bit_and(mask,a.value_),ops::bit_and(ops::bit_not(mask),b.value_)));
      }
#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512BW
      if constexpr(M::compact) {
        if constexpr(N==64) return simd(raw{},_mm512_mask_blend_epi8(m.to_native(),b.value_,a.value_));
#if NATIVE_HAS_AVX512VL
        else if constexpr(N==32) return simd(raw{},_mm256_mask_blend_epi8(m.to_native(),b.value_,a.value_));
        else return simd(raw{},_mm_mask_blend_epi8(m.to_native(),b.value_,a.value_));
#endif
      } else
#endif
      return simd(raw{},ops::bit_or(ops::bit_and(m.to_native(),a.value_),ops::bit_and(ops::bit_not(m.to_native()),b.value_)));
    }
  private:
    struct raw {};
    native_inline constexpr simd(raw,native_type value) noexcept : value_(value) {}
    native_type value_;
  };

  namespace detail::NATIVE_BACKEND {
  template<::native::isa<> Arch, class T,std::size_t N,class U,std::size_t A=1,simd_access Access=simd_access::ordinary>
    requires NATIVE_ARCH_REQUIRES(Arch) && std::same_as<T,bool> && std::same_as<U,bool> && requires { typename simd<T,N,Arch>::native_type; }
  native_nodiscard native_inline constexpr native_pure simd<T,N,Arch> load_simd(U const * p,simd_memory<A,Access> = {}) noexcept { return simd<T,N,Arch>::load(p); }
  template<class U,class T,std::size_t N,std::size_t A=1,simd_access Access=simd_access::ordinary, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && std::same_as<T,bool> && std::same_as<U,bool> && requires { typename simd<T,N,Arch>::native_type; }
  native_inline constexpr void store_simd(U * p,simd<T,N,Arch> value,simd_memory<A,Access> = {}) noexcept { value.store(p); }
  template<::native::isa<> Arch, class T,std::size_t N,class U,std::size_t A=1,simd_access Access=simd_access::ordinary>
    requires NATIVE_ARCH_REQUIRES(Arch) && std::same_as<T,bool> && std::same_as<U,bool> && requires { typename simd<T,N,Arch>::native_type; }
  native_nodiscard native_inline constexpr native_pure simd<T,N,Arch> load_simd_partial(U const * p,std::size_t count,T fill=false,simd_memory<A,Access> = {}) noexcept native_diagnose_if(count > N,"partial SIMD count exceeds the lane count") {
    return simd<T,N,Arch>::load_partial(p,count,fill);
  }
  template<class U,class T,std::size_t N,std::size_t A=1,simd_access Access=simd_access::ordinary, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && std::same_as<T,bool> && std::same_as<U,bool> && requires { typename simd<T,N,Arch>::native_type; }
  native_inline constexpr void store_simd_partial(U * p,simd<T,N,Arch> value,std::size_t count,simd_memory<A,Access> = {}) noexcept native_diagnose_if(count > N,"partial SIMD count exceeds the lane count") { value.store_partial(p,count); }
  }
  /// Expand lane truth into canonical zero/all-one full-vector mask lanes.
  template<std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && requires { typename simd<bool,N,Arch>::native_type; }
  native_nodiscard native_inline constexpr native_const simd<mask8,N,Arch> to_vector_mask(simd<bool,N,Arch> value) noexcept {
    return simd<mask8,N,Arch>::from_native(value.to_native());
  }
  /// Convert lane truth into Boolean data lanes represented as zero or one, preserving the lane count.
  template<class T,std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && simd_mask_element<T> && requires { typename simd<bool,N,Arch>::native_type; typename simd<T,N,Arch>::native_type; }
  native_nodiscard native_inline constexpr native_const simd<bool,N,Arch> to_bool(simd<T,N,Arch> value) noexcept {
    auto byte_mask=mask_cast<mask8>(value);
    return simd<bool,N,Arch>::unsafe_from_native(simd<bool,N,Arch>::ops::bit_and(byte_mask.to_native(),::NATIVE_BACKEND_NAMESPACE::bool_ones<N>()));
  }
  /// Convert lane truth into Boolean data lanes represented as zero or one, preserving the lane count.
  template<std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::predicate_shape<N> && requires { typename simd<bool,N,Arch>::native_type; }
  native_nodiscard native_inline constexpr native_const simd<bool,N,Arch> to_bool(predicate<N,Arch> value) noexcept { return to_bool(to_vector_mask<mask8>(value)); }
  /// Compress lane truth into the supported compact predicate representation.
  template<std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::predicate_shape<N> && requires { typename simd<bool,N,Arch>::native_type; }
  native_nodiscard native_inline constexpr native_const predicate<N,Arch> to_predicate(simd<bool,N,Arch> value) noexcept {
    auto result=value!=simd<bool,N,Arch>(false);
    if constexpr(decltype(result)::compact) return result;
    else return to_predicate(result);
  }
}


namespace NATIVE_BACKEND_NAMESPACE {
  template <simd_integer_element T> native_nodiscard native_inline constexpr auto integer_word(T value) noexcept {
    return std::bit_cast<std::make_unsigned_t<T>>(value);
  }
  template <simd_integer_element T, class U>
  native_nodiscard native_inline constexpr T integer_wrap(U value) noexcept {
    return std::bit_cast<T>(static_cast<std::make_unsigned_t<T>>(value));
  }
  template <simd_integer_element T>
  using integer_work_word = std::conditional_t<(sizeof(T) < 4), uint32_t, std::make_unsigned_t<T>>;
  template <simd_integer_element T>
  native_nodiscard native_inline constexpr T integer_scalar_add(T a, T b) noexcept {
    using U = integer_work_word<T>;
    return integer_wrap<T>(U(integer_word(a)) + U(integer_word(b)));
  }
  template <simd_integer_element T>
  native_nodiscard native_inline constexpr T integer_scalar_sub(T a, T b) noexcept {
    using U = integer_work_word<T>;
    return integer_wrap<T>(U(integer_word(a)) - U(integer_word(b)));
  }
  template <simd_integer_element T>
  native_nodiscard native_inline constexpr T integer_scalar_mul(T a, T b) noexcept {
    using U = integer_work_word<T>;
    return integer_wrap<T>(U(integer_word(a)) * U(integer_word(b)));
  }

#if NATIVE_HAS_AVX2
  template <simd_integer_element T>
  native_nodiscard native_inline native_const __m128i integer_broadcast_16(T x) noexcept {
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
  native_nodiscard native_inline native_const __m128i integer_add(__m128i a, __m128i b) noexcept {
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
  native_nodiscard native_inline native_const __m128i integer_sub(__m128i a, __m128i b) noexcept {
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
  native_nodiscard native_inline native_const __m128i integer_mul(__m128i a, __m128i b) noexcept {
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
#if NATIVE_HAS_AVX512DQ && NATIVE_HAS_AVX512VL
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
  native_nodiscard native_inline native_const __m128i integer_left(__m128i a) noexcept {
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
  native_nodiscard native_inline native_const __m128i integer_right(__m128i a) noexcept {
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
#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512VL
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

#if NATIVE_HAS_AVX2
  template <simd_integer_element T>
  native_nodiscard native_inline native_const __m256i integer_broadcast_32(T x) noexcept {
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
  native_nodiscard native_inline native_const __m256i integer_add(__m256i a, __m256i b) noexcept {
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
  native_nodiscard native_inline native_const __m256i integer_sub(__m256i a, __m256i b) noexcept {
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
  native_nodiscard native_inline native_const __m256i integer_mul(__m256i a, __m256i b) noexcept {
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
#if NATIVE_HAS_AVX512DQ && NATIVE_HAS_AVX512VL
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
  native_nodiscard native_inline native_const __m256i integer_left(__m256i a) noexcept {
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
  native_nodiscard native_inline native_const __m256i integer_right(__m256i a) noexcept {
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
#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512VL
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

#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ
  template <simd_integer_element T>
  native_nodiscard native_inline native_const __m512i integer_broadcast_64(T x) noexcept {
#if NATIVE_HAS_AVX512BW
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
  native_nodiscard native_inline native_const __m512i integer_add(__m512i a, __m512i b) noexcept {
#if NATIVE_HAS_AVX512BW
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
  native_nodiscard native_inline native_const __m512i integer_sub(__m512i a, __m512i b) noexcept {
#if NATIVE_HAS_AVX512BW
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
  native_nodiscard native_inline native_const __m512i integer_mul(__m512i a, __m512i b) noexcept {
#if NATIVE_HAS_AVX512BW
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
  native_nodiscard native_inline native_const __m512i integer_left(__m512i a) noexcept {
    if constexpr (S == 0)
      return a;
    else {
#if NATIVE_HAS_AVX512BW
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
  native_nodiscard native_inline native_const __m512i integer_right(__m512i a) noexcept {
    if constexpr (S == 0)
      return a;
    else {
#if NATIVE_HAS_AVX512BW
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
#if NATIVE_HAS_AVX2
  native_nodiscard native_inline native_const __m128i integer_and(__m128i a, __m128i b) noexcept {
    return _mm_and_si128(a, b);
  }
  native_nodiscard native_inline native_const __m128i integer_or(__m128i a, __m128i b) noexcept {
    return _mm_or_si128(a, b);
  }
  native_nodiscard native_inline native_const __m128i integer_xor(__m128i a, __m128i b) noexcept {
    return _mm_xor_si128(a, b);
  }
  template <simd_integer_element T, bool Greater>
  native_nodiscard native_inline native_const auto integer_compare(__m128i a, __m128i b) noexcept {
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
  native_nodiscard native_inline native_const __m128i integer_select(M m, __m128i a, __m128i b) noexcept {
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
  native_nodiscard native_inline native_const __m128i integer_masked_add(M m, __m128i prior, __m128i a,
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
  native_nodiscard native_inline native_const __m128i integer_masked_sub(M m, __m128i prior, __m128i a,
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

#if NATIVE_HAS_AVX2
  native_nodiscard native_inline native_const __m256i integer_and(__m256i a, __m256i b) noexcept {
    return _mm256_and_si256(a, b);
  }
  native_nodiscard native_inline native_const __m256i integer_or(__m256i a, __m256i b) noexcept {
    return _mm256_or_si256(a, b);
  }
  native_nodiscard native_inline native_const __m256i integer_xor(__m256i a, __m256i b) noexcept {
    return _mm256_xor_si256(a, b);
  }
  template <simd_integer_element T, bool Greater>
  native_nodiscard native_inline native_const auto integer_compare(__m256i a, __m256i b) noexcept {
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
  native_nodiscard native_inline native_const __m256i integer_select(M m, __m256i a, __m256i b) noexcept {
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
  native_nodiscard native_inline native_const __m256i integer_masked_add(M m, __m256i prior, __m256i a,
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
  native_nodiscard native_inline native_const __m256i integer_masked_sub(M m, __m256i prior, __m256i a,
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

#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ
  native_nodiscard native_inline native_const __m512i integer_and(__m512i a, __m512i b) noexcept {
    return _mm512_and_si512(a, b);
  }
  native_nodiscard native_inline native_const __m512i integer_or(__m512i a, __m512i b) noexcept {
    return _mm512_or_si512(a, b);
  }
  native_nodiscard native_inline native_const __m512i integer_xor(__m512i a, __m512i b) noexcept {
    return _mm512_xor_si512(a, b);
  }
  template <simd_integer_element T, bool Greater>
  native_nodiscard native_inline native_const auto integer_compare(__m512i a, __m512i b) noexcept {
    if constexpr (mask_compact<sizeof(T), 64 / sizeof(T)>) {
#if NATIVE_HAS_AVX512BW
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
  native_nodiscard native_inline native_const __m512i integer_select(M m, __m512i a, __m512i b) noexcept {
    if constexpr (M::compact) {
#if NATIVE_HAS_AVX512BW
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
  native_nodiscard native_inline native_const __m512i integer_masked_add(M m, __m512i prior, __m512i a,
                                                                   __m512i b) noexcept {
    if constexpr (M::compact) {
#if NATIVE_HAS_AVX512BW
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
  native_nodiscard native_inline native_const __m512i integer_masked_sub(M m, __m512i prior, __m512i a,
                                                                   __m512i b) noexcept {
    if constexpr (M::compact) {
#if NATIVE_HAS_AVX512BW
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

#if NATIVE_HAS_ARM_NEON
  native_nodiscard native_inline native_const uint8x16_t integer_and(uint8x16_t a, uint8x16_t b) noexcept {
    return vandq_u8(a, b);
  }
  native_nodiscard native_inline native_const uint8x16_t integer_or(uint8x16_t a, uint8x16_t b) noexcept {
    return vorrq_u8(a, b);
  }
  native_nodiscard native_inline native_const uint8x16_t integer_xor(uint8x16_t a, uint8x16_t b) noexcept {
    return veorq_u8(a, b);
  }
  template <simd_integer_element T>
  native_nodiscard native_inline native_const uint8x16_t integer_add(uint8x16_t a, uint8x16_t b) noexcept {
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
  native_nodiscard native_inline native_const uint8x16_t integer_sub(uint8x16_t a, uint8x16_t b) noexcept {
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
  native_nodiscard native_inline native_const uint8x16_t integer_mul(uint8x16_t a, uint8x16_t b) noexcept {
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
  native_nodiscard native_inline native_const uint8x16_t integer_broadcast_16(T x) noexcept {
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
  native_nodiscard native_inline native_const uint8x16_t integer_left(uint8x16_t a) noexcept {
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
  native_nodiscard native_inline native_const uint8x16_t integer_right(uint8x16_t a) noexcept {
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
  native_nodiscard native_inline native_const uint8x16_t integer_compare(uint8x16_t a, uint8x16_t b) noexcept {
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
  native_nodiscard native_inline native_const uint8x16_t integer_select(M m, uint8x16_t a, uint8x16_t b) noexcept {
    return vbslq_u8(m.to_native(), a, b);
  }
  template <simd_integer_element T, class M>
  native_nodiscard native_inline native_const uint8x16_t integer_masked_add(M m, uint8x16_t prior, uint8x16_t a,
                                                                      uint8x16_t b) noexcept {
    return integer_select<T>(m, integer_add<T>(a, b), prior);
  }
  template <simd_integer_element T, class M>
  native_nodiscard native_inline native_const uint8x16_t integer_masked_sub(M m, uint8x16_t prior, uint8x16_t a,
                                                                      uint8x16_t b) noexcept {
    return integer_select<T>(m, integer_sub<T>(a, b), prior);
  }
#endif

#if NATIVE_HAS_AVX2
  template <simd_integer_element T, class M>
  native_nodiscard native_inline native_const __m128i integer_masked_mul(M m, __m128i prior, __m128i a,
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
#if NATIVE_HAS_AVX2
  template <simd_integer_element T, class M>
  native_nodiscard native_inline native_const __m256i integer_masked_mul(M m, __m256i prior, __m256i a,
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
#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ
  template <simd_integer_element T, class M>
  native_nodiscard native_inline native_const __m512i integer_masked_mul(M m, __m512i prior, __m512i a,
                                                                   __m512i b) noexcept {
    if constexpr (M::compact && sizeof(T) > 1) {
#if NATIVE_HAS_AVX512BW
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
#if NATIVE_HAS_ARM_NEON
  template <simd_integer_element T, class M>
  native_nodiscard native_inline native_const uint8x16_t integer_masked_mul(M m, uint8x16_t prior, uint8x16_t a,
                                                                      uint8x16_t b) noexcept {
    return integer_select<T>(m, integer_mul<T>(a, b), prior);
  }
#endif
#if NATIVE_HAS_AVX2
  native_nodiscard native_inline native_const __m128i integer_bit_select(__m128i m,__m128i a,__m128i b) noexcept {
    return _mm_or_si128(_mm_and_si128(m,a),_mm_andnot_si128(m,b));
  }
#endif
#if NATIVE_HAS_AVX2
  native_nodiscard native_inline native_const __m256i integer_bit_select(__m256i m,__m256i a,__m256i b) noexcept {
    return _mm256_or_si256(_mm256_and_si256(m,a),_mm256_andnot_si256(m,b));
  }
#endif
#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ
  native_nodiscard native_inline native_const __m512i integer_bit_select(__m512i m,__m512i a,__m512i b) noexcept {
    return _mm512_or_si512(_mm512_and_si512(m,a),_mm512_andnot_si512(m,b));
  }
#endif
#if NATIVE_HAS_ARM_NEON
  native_nodiscard native_inline native_const uint8x16_t integer_bit_select(uint8x16_t m,uint8x16_t a,uint8x16_t b) noexcept { return vbslq_u8(m,a,b); }
#endif
} // namespace NATIVE_BACKEND_NAMESPACE


namespace native {
  namespace detail::NATIVE_BACKEND {
    template <std::size_t Bytes> struct integer_storage;
#if NATIVE_HAS_AVX2
    template <> struct integer_storage<16> {
      using type = __m128i;
    };
    template <> struct integer_storage<32> {
      using type = __m256i;
    };
#endif
#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ
    template <> struct integer_storage<64> {
      using type = __m512i;
    };
#endif
#if NATIVE_HAS_ARM_NEON
    template <> struct integer_storage<16> {
      using type = uint8x16_t;
    };
#endif
    template <class T, std::size_t N>
    inline constexpr bool integer_shape =
        simd_integer_element<T> && (N == 1
#if NATIVE_HAS_AVX2
                                    || sizeof(T) * N == 16 || sizeof(T) * N == 32
#endif
#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ
                                    || (sizeof(T) * N == 64 && (sizeof(T) >= 4
#if NATIVE_HAS_AVX512BW
                                                                || sizeof(T) <= 2
#endif
                                                                ))
#endif
#if NATIVE_HAS_ARM_NEON
                                    || sizeof(T) * N == 16
#endif
                                   );
  } // namespace detail

  namespace detail::NATIVE_BACKEND {
  template <::native::isa<> Arch, simd_integer_element T, std::size_t N, std::size_t A = 1,
            simd_access Access = simd_access::ordinary>
    requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::integer_shape<T, N>
  native_nodiscard native_inline native_pure constexpr simd<T, N,Arch> load_simd(T const *p, simd_memory<A, Access> = {}) noexcept;
  template <simd_integer_element T, std::size_t N, std::size_t A = 1,
            simd_access Access = simd_access::ordinary, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::integer_shape<T, N>
  native_inline constexpr void store_simd(T *p, simd<T, N,Arch> v, simd_memory<A, Access> = {}) noexcept;
  template <::native::isa<> Arch, simd_integer_element T, std::size_t N, std::size_t A = 1,
            simd_access Access = simd_access::ordinary>
    requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::integer_shape<T, N>
  native_nodiscard native_inline constexpr native_pure simd<T, N,Arch> load_simd_partial(T const *p, std::size_t count,
                                                                    simd_memory<A, Access> = {}) noexcept native_diagnose_if(count > N,"partial SIMD count exceeds the lane count");
  template <simd_integer_element T, std::size_t N, std::size_t A = 1,
            simd_access Access = simd_access::ordinary, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::integer_shape<T, N>
  native_inline constexpr void store_simd_partial(T *p, simd<T, N,Arch> v, std::size_t count,
                                      simd_memory<A, Access> = {}) noexcept native_diagnose_if(count > N,"partial SIMD count exceeds the lane count");

  }
  template <simd_integer_element T, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) struct simd<T, 1,Arch> : detail::swizzle_access<T,1,Arch> {
    static constexpr isa<> architecture=Arch;
    template <class U> using rebind = simd<U,1,Arch>;
    template <std::size_t A = 1>
    /// Load logical lanes, assuming the template alignment in bytes.
    native_nodiscard static native_inline constexpr simd load_memory(T const * p) noexcept {
      return ::NATIVE_BACKEND_NAMESPACE::load_simd<Arch,T,1>(p,simd_memory<A>{});
    }
    template <std::size_t A = 1>
    /// Store logical lanes, assuming the template alignment in bytes.
    native_inline constexpr void store_memory(T * p) const noexcept {
      ::NATIVE_BACKEND_NAMESPACE::store_simd(p,*this,simd_memory<A>{});
    }

    using value_type = T;
    using native_type = T;
    using mask_type = ::NATIVE_BACKEND_NAMESPACE::comparison_mask<T, 1,Arch>;
    using mask = mask_type;
    using predicate_type = predicate<1,Arch>;
    using unsigned_register_tag = void;
    static constexpr std::size_t lanes = 1;
    T value{};
    /// Initialize the stored lane values to zero.
    constexpr simd() noexcept = default;
    /// Broadcast the integral input, retaining its low sizeof(T)*8 bits in each lane.
    template <simd_integer_element U> constexpr simd(U x) noexcept : value(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(x)) {}
    /// Copy one value per logical lane in array order.
    constexpr simd(std::array<T, 1> const &x) noexcept : value(x[0]) {}
    /// Return the native storage value without a numerical conversion.
    native_nodiscard native_inline constexpr operator T() const noexcept { return value; }
    /// Adopt native storage without numerical conversion.
    native_nodiscard native_inline static constexpr simd from_native(T x) noexcept { return simd(x); }
    /// Return the native storage representation.
    native_nodiscard native_inline constexpr T to_native() const noexcept { return value; }
    /// Add corresponding lanes modulo 2^(sizeof(T)*8).
    native_nodiscard friend native_inline native_const constexpr simd operator+(simd a, simd b) noexcept {
      return from_native(::NATIVE_BACKEND_NAMESPACE::integer_scalar_add(a.value, b.value));
    }
    /// Subtract corresponding lanes modulo 2^(sizeof(T)*8).
    native_nodiscard friend native_inline native_const constexpr simd operator-(simd a, simd b) noexcept {
      return from_native(::NATIVE_BACKEND_NAMESPACE::integer_scalar_sub(a.value, b.value));
    }
    /// Multiply corresponding lanes modulo 2^(sizeof(T)*8).
    native_nodiscard friend native_inline native_const constexpr simd operator*(simd a, simd b) noexcept {
      return from_native(::NATIVE_BACKEND_NAMESPACE::integer_scalar_mul(a.value, b.value));
    }
    /// Bitwise AND of corresponding lane representations.
    native_nodiscard friend native_inline native_const constexpr simd operator&(simd a, simd b) noexcept {
      return from_native(
          ::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(::NATIVE_BACKEND_NAMESPACE::integer_word(a.value) & ::NATIVE_BACKEND_NAMESPACE::integer_word(b.value)));
    }
    /// Bitwise OR of corresponding lane representations.
    native_nodiscard friend native_inline native_const constexpr simd operator|(simd a, simd b) noexcept {
      return from_native(
          ::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(::NATIVE_BACKEND_NAMESPACE::integer_word(a.value) | ::NATIVE_BACKEND_NAMESPACE::integer_word(b.value)));
    }
    /// Bitwise XOR of corresponding lane representations.
    native_nodiscard friend native_inline native_const constexpr simd operator^(simd a, simd b) noexcept {
      return from_native(
          ::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(::NATIVE_BACKEND_NAMESPACE::integer_word(a.value) ^ ::NATIVE_BACKEND_NAMESPACE::integer_word(b.value)));
    }
    /// Complement every bit in every lane.
    native_nodiscard friend native_inline native_const constexpr simd operator~(simd a) noexcept {
      return a ^ simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(~std::make_unsigned_t<T>(0)));
    }
    /// Negate each lane modulo 2^(sizeof(T)*8).
    native_nodiscard friend native_inline native_const constexpr simd operator-(simd a) noexcept { return simd(T(0)) - a; }
    /// Return the unchanged vector value.
    native_nodiscard friend native_inline native_const constexpr simd operator+(simd a) noexcept { return a; }
    /// Return a mask whose lanes are true where `a == b` holds. Ordering follows the signedness of T.
    native_nodiscard friend native_inline native_const constexpr mask_type operator==(simd a, simd b) noexcept {
      return mask_type(a.value == b.value);
    }
    /// Return a mask whose lanes are true where `a > b` holds. Ordering follows the signedness of T.
    native_nodiscard friend native_inline native_const constexpr mask_type operator>(simd a, simd b) noexcept {
      return mask_type(a.value > b.value);
    }
    /// Return a mask whose lanes are true where `a != b` holds. Ordering follows the signedness of T.
    native_nodiscard friend native_inline native_const constexpr mask_type operator!=(simd a, simd b) noexcept {
      return ~(a == b);
    }
    /// Return a mask whose lanes are true where `a < b` holds. Ordering follows the signedness of T.
    native_nodiscard friend native_inline native_const constexpr mask_type operator<(simd a, simd b) noexcept {
      return b > a;
    }
    /// Return a mask whose lanes are true where `a <= b` holds. Ordering follows the signedness of T.
    native_nodiscard friend native_inline native_const constexpr mask_type operator<=(simd a, simd b) noexcept {
      return ~(a > b);
    }
    /// Return a mask whose lanes are true where `a >= b` holds. Ordering follows the signedness of T.
    native_nodiscard friend native_inline native_const constexpr mask_type operator>=(simd a, simd b) noexcept {
      return ~(b > a);
    }
    /// Add corresponding lanes modulo 2^(sizeof(T)*8). Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline native_const constexpr simd operator+(simd a, U b) noexcept {
      return a + simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Add corresponding lanes modulo 2^(sizeof(T)*8). Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline native_const constexpr simd operator+(U a, simd b) noexcept {
      return simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(a)) + b;
    }
    /// Subtract corresponding lanes modulo 2^(sizeof(T)*8). Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline native_const constexpr simd operator-(simd a, U b) noexcept {
      return a - simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Subtract corresponding lanes modulo 2^(sizeof(T)*8). Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline native_const constexpr simd operator-(U a, simd b) noexcept {
      return simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(a)) - b;
    }
    /// Multiply corresponding lanes modulo 2^(sizeof(T)*8). Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline native_const constexpr simd operator*(simd a, U b) noexcept {
      return a * simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Multiply corresponding lanes modulo 2^(sizeof(T)*8). Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline native_const constexpr simd operator*(U a, simd b) noexcept {
      return simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(a)) * b;
    }
    /// Bitwise AND of corresponding lane representations. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline native_const constexpr simd operator&(simd a, U b) noexcept {
      return a & simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Bitwise AND of corresponding lane representations. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline native_const constexpr simd operator&(U a, simd b) noexcept {
      return simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(a)) & b;
    }
    /// Bitwise OR of corresponding lane representations. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline native_const constexpr simd operator|(simd a, U b) noexcept {
      return a | simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Bitwise OR of corresponding lane representations. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline native_const constexpr simd operator|(U a, simd b) noexcept {
      return simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(a)) | b;
    }
    /// Bitwise XOR of corresponding lane representations. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline native_const constexpr simd operator^(simd a, U b) noexcept {
      return a ^ simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Bitwise XOR of corresponding lane representations. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline native_const constexpr simd operator^(U a, simd b) noexcept {
      return simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(a)) ^ b;
    }
    /// Return a mask whose lanes are true where `a == b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline native_const constexpr mask_type operator==(simd a, U b) noexcept {
      return a == simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Return a mask whose lanes are true where `a == b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline native_const constexpr mask_type operator==(U a, simd b) noexcept {
      return simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(a)) == b;
    }
    /// Return a mask whose lanes are true where `a != b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline native_const constexpr mask_type operator!=(simd a, U b) noexcept {
      return a != simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Return a mask whose lanes are true where `a != b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline native_const constexpr mask_type operator!=(U a, simd b) noexcept {
      return simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(a)) != b;
    }
    /// Return a mask whose lanes are true where `a < b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline native_const constexpr mask_type operator<(simd a, U b) noexcept {
      return a < simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Return a mask whose lanes are true where `a < b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline native_const constexpr mask_type operator<(U a, simd b) noexcept {
      return simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(a)) < b;
    }
    /// Return a mask whose lanes are true where `a > b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline native_const constexpr mask_type operator>(simd a, U b) noexcept {
      return a > simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Return a mask whose lanes are true where `a > b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline native_const constexpr mask_type operator>(U a, simd b) noexcept {
      return simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(a)) > b;
    }
    /// Return a mask whose lanes are true where `a <= b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline native_const constexpr mask_type operator<=(simd a, U b) noexcept {
      return a <= simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Return a mask whose lanes are true where `a <= b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline native_const constexpr mask_type operator<=(U a, simd b) noexcept {
      return simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(a)) <= b;
    }
    /// Return a mask whose lanes are true where `a >= b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline native_const constexpr mask_type operator>=(simd a, U b) noexcept {
      return a >= simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Return a mask whose lanes are true where `a >= b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline native_const constexpr mask_type operator>=(U a, simd b) noexcept {
      return simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(a)) >= b;
    }
    /// Apply the corresponding lane-wise add operation in place and return *this.
    native_inline constexpr simd &operator+=(simd b) noexcept { return *this = *this + b; }
    /// Apply the corresponding lane-wise add operation in place and return *this. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U> native_inline constexpr simd &operator+=(U b) noexcept { return *this = *this + b; }
    /// Apply the corresponding lane-wise subtract operation in place and return *this.
    native_inline constexpr simd &operator-=(simd b) noexcept { return *this = *this - b; }
    /// Apply the corresponding lane-wise subtract operation in place and return *this. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U> native_inline constexpr simd &operator-=(U b) noexcept { return *this = *this - b; }
    /// Apply the corresponding lane-wise multiply operation in place and return *this.
    native_inline constexpr simd &operator*=(simd b) noexcept { return *this = *this * b; }
    /// Apply the corresponding lane-wise multiply operation in place and return *this. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U> native_inline constexpr simd &operator*=(U b) noexcept { return *this = *this * b; }
    /// Apply the corresponding lane-wise AND operation in place and return *this.
    native_inline constexpr simd &operator&=(simd b) noexcept { return *this = *this & b; }
    /// Apply the corresponding lane-wise AND operation in place and return *this. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U> native_inline constexpr simd &operator&=(U b) noexcept { return *this = *this & b; }
    /// Apply the corresponding lane-wise OR operation in place and return *this.
    native_inline constexpr simd &operator|=(simd b) noexcept { return *this = *this | b; }
    /// Apply the corresponding lane-wise OR operation in place and return *this. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U> native_inline constexpr simd &operator|=(U b) noexcept { return *this = *this | b; }
    /// Apply the corresponding lane-wise XOR operation in place and return *this.
    native_inline constexpr simd &operator^=(simd b) noexcept { return *this = *this ^ b; }
    /// Apply the corresponding lane-wise XOR operation in place and return *this. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U> native_inline constexpr simd &operator^=(U b) noexcept { return *this = *this ^ b; }
    /// Shift each lane left by compile-time K, discarding high bits; require K below the lane bit width.
    template <unsigned K>
      requires(K < sizeof(T) * 8)
    native_nodiscard native_inline native_const constexpr simd left() const noexcept {
      using W = ::NATIVE_BACKEND_NAMESPACE::integer_work_word<T>;
      auto u = ::NATIVE_BACKEND_NAMESPACE::integer_word(value);
      return from_native(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(W(u) << K));
    }
    /// Shift each lane right by compile-time K; signed lanes extend their sign. Require K below the lane bit width.
    template <unsigned K>
      requires(K < sizeof(T) * 8)
    native_nodiscard native_inline native_const constexpr simd right() const noexcept {
      using U = std::make_unsigned_t<T>;
      using W = ::NATIVE_BACKEND_NAMESPACE::integer_work_word<T>;
      auto u = ::NATIVE_BACKEND_NAMESPACE::integer_word(value);
      if constexpr (K == 0)
        return *this;
      else {
        U result = U(W(u) >> K);
        if constexpr (std::is_signed_v<T>) {
          if (value < T(0))
            result = U(result | U(std::numeric_limits<U>::max() ^ (std::numeric_limits<U>::max() >> K)));
        }
        return from_native(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(result));
      }
    }
    /// Shift every lane left by K bits, discarding high bits. Require K smaller than the lane bit width.
    template <std::size_t K>
      requires(K < sizeof(T) * 8)
    native_nodiscard friend native_inline native_const constexpr simd operator<<(simd a, imm_t<K>) noexcept {
      return a.template left<K>();
    }
    /// Shift every lane right by K; signed lanes extend the sign, unsigned lanes shift in zero. Require K smaller than the lane bit width.
    template <std::size_t K>
      requires(K < sizeof(T) * 8)
    native_nodiscard friend native_inline native_const constexpr simd operator>>(simd a, imm_t<K>) noexcept {
      return a.template right<K>();
    }
    /// Read all logical lanes from an unaligned element pointer.
    native_nodiscard native_inline static native_pure constexpr simd load(T const *p) noexcept {
      return ::NATIVE_BACKEND_NAMESPACE::load_simd<Arch,T, lanes>(p);
    }
    /// Read all logical lanes without an extra alignment promise.
    native_nodiscard native_inline static native_pure constexpr simd loadu(T const *p) noexcept {
      return ::NATIVE_BACKEND_NAMESPACE::load_simd<Arch,T, lanes>(p);
    }
    /// Write all logical lanes to an unaligned element pointer.
    native_inline constexpr void store(T *p) const noexcept { ::NATIVE_BACKEND_NAMESPACE::store_simd(p, *this); }
    /// Write all logical lanes without an extra alignment promise.
    native_inline constexpr void storeu(T *p) const noexcept { ::NATIVE_BACKEND_NAMESPACE::store_simd(p, *this); }
    /// Read exactly n logical lanes and fill the remainder; require n <= lanes. For n == 0, p may be null.
    native_nodiscard native_inline constexpr static native_pure simd load_partial(T const *p, std::size_t n,
                                                                  T fill = T(0)) noexcept native_diagnose_if(n > simd::lanes,"partial SIMD count exceeds the lane count") {
      assert(n <= lanes);
      std::array<T, lanes> data;
      data.fill(fill);
      if consteval { for(std::size_t i=0;i<n;++i) data[i]=p[i]; }
      else { if (n) std::memcpy(data.data(), static_cast<void const *>(p), n * sizeof(T)); }
      return ::NATIVE_BACKEND_NAMESPACE::load_simd<Arch,T, lanes>(data.data());
    }
    /// Write exactly n logical lanes; require n <= lanes. For n == 0, p may be null.
    native_inline constexpr void store_partial(T *p, std::size_t n) const noexcept native_diagnose_if(n > simd::lanes,"partial SIMD count exceeds the lane count") { ::NATIVE_BACKEND_NAMESPACE::store_simd_partial(p, *this, n); }
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator+(simd, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator+(U, simd) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator+(simd, ::native::simd<U, M,Arch>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator-(simd, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator-(U, simd) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator-(simd, ::native::simd<U, M,Arch>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator*(simd, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator*(U, simd) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator*(simd, ::native::simd<U, M,Arch>) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator/(simd, U) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator/(U, simd) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator/(simd, ::native::simd<U, M,Arch>) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator%(simd, U) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator%(U, simd) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator%(simd, ::native::simd<U, M,Arch>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator&(simd, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator&(U, simd) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator&(simd, ::native::simd<U, M,Arch>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator|(simd, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator|(U, simd) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator|(simd, ::native::simd<U, M,Arch>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator^(simd, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator^(U, simd) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator^(simd, ::native::simd<U, M,Arch>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator==(simd, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator==(U, simd) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator==(simd, ::native::simd<U, M,Arch>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator!=(simd, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator!=(U, simd) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator!=(simd, ::native::simd<U, M,Arch>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator<(simd, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator<(U, simd) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator<(simd, ::native::simd<U, M,Arch>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator>(simd, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator>(U, simd) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator>(simd, ::native::simd<U, M,Arch>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator<=(simd, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator<=(U, simd) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator<=(simd, ::native::simd<U, M,Arch>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator>=(simd, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator>=(U, simd) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator>=(simd, ::native::simd<U, M,Arch>) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    friend simd operator/(simd, simd) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    friend simd operator%(simd, simd) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <simd_integer_element U> friend simd operator/(simd, U) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <simd_integer_element U> friend simd operator/(U, simd) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <simd_integer_element U> friend simd operator%(simd, U) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <simd_integer_element U> friend simd operator%(U, simd) = delete;
    /// Reject runtime shift counts; use a compile-time imm<K> within the lane width.
    template <simd_integer_element U> friend simd operator<<(simd, U) = delete;
    /// Reject runtime shift counts; use a compile-time imm<K> within the lane width.
    template <simd_integer_element U> friend simd operator>>(simd, U) = delete;
  };

#if NATIVE_HAS_AVX2 || NATIVE_HAS_ARM_NEON || (NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ)
  // Qualified native helper names must exist even before constraints are checked.
  // A real integer specialization owns one intrinsic register. The storage trait
  // chooses representation only; every operation below has integer semantics.
  template <simd_integer_element T, std::size_t N, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) &&(N > 1 && ::NATIVE_BACKEND_NAMESPACE::integer_shape<T, N>)
  struct simd<T, N,Arch> : detail::swizzle_access<T,N,Arch> {
    static constexpr isa<> architecture=Arch;
    template <class U> using rebind = simd<U,N,Arch>;
    template <std::size_t A = 1>
    /// Load logical lanes, assuming the template alignment in bytes.
    native_nodiscard static native_inline constexpr simd load_memory(T const * p) noexcept {
      return ::NATIVE_BACKEND_NAMESPACE::load_simd<Arch,T,N>(p,simd_memory<A>{});
    }
    template <std::size_t A = 1>
    /// Store logical lanes, assuming the template alignment in bytes.
    native_inline constexpr void store_memory(T * p) const noexcept {
      ::NATIVE_BACKEND_NAMESPACE::store_simd(p,*this,simd_memory<A>{});
    }

    using value_type = T;
    using native_type = typename ::NATIVE_BACKEND_NAMESPACE::integer_storage<sizeof(T) * N>::type;
    using mask_type = ::NATIVE_BACKEND_NAMESPACE::comparison_mask<T, N,Arch>;
    using mask = mask_type;
    using predicate_type = predicate<N,Arch>;
    using unsigned_register_tag = void;
    static constexpr std::size_t lanes = N;
    native_type value{};
    /// Initialize the stored lane values to zero.
    constexpr simd() noexcept = default;
    /// Broadcast the integral input, retaining its low sizeof(T)*8 bits in each lane.
    template <simd_integer_element U> native_inline constexpr simd(U input) noexcept {
      T x = ::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(input);
      if consteval {
        std::array<T,N> data;
        data.fill(x);
        value=std::bit_cast<native_type>(data);
      } else {
        if constexpr (sizeof(T) * N == 16)
          value = ::NATIVE_BACKEND_NAMESPACE::integer_broadcast_16(x);
#if NATIVE_HAS_AVX2
        else if constexpr (sizeof(T) * N == 32)
          value = ::NATIVE_BACKEND_NAMESPACE::integer_broadcast_32(x);
#endif
#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ
        else if constexpr (sizeof(T) * N == 64)
          value = ::NATIVE_BACKEND_NAMESPACE::integer_broadcast_64(x);
#endif
      }
    }
#if NATIVE_HOST_X86
    /// Adopt native lane storage under its minimal register ABI.
    native_inline constexpr native_target("sse2") simd(native_type x) noexcept requires(sizeof(native_type)==16) : value(x) {}
    /// Adopt native lane storage under its minimal register ABI.
    native_inline constexpr native_target("avx") simd(native_type x) noexcept requires(sizeof(native_type)==32) : value(x) {}
    /// Adopt native lane storage under its minimal register ABI.
    native_inline constexpr native_target("avx512f") simd(native_type x) noexcept requires(sizeof(native_type)==64) : value(x) {}
#else
    /// Adopt native lane storage without numerical conversion.
    native_inline constexpr simd(native_type x) noexcept : value(x) {}
#endif
    /// Construct logical lanes in argument order. Integral conversion retains the low lane-width bits.
    template <class... U>
      requires(sizeof...(U) == N && (simd_integer_element<U> && ...))
    native_inline constexpr simd(U... xs) noexcept {
      std::array<T, N> data{::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(xs)...};
      if consteval { value=std::bit_cast<native_type>(data); }
      else { std::memcpy(&value, data.data(), sizeof(value)); }
    }
    /// Copy one value per logical lane in array order.
    native_inline constexpr simd(std::array<T, N> const &data) noexcept {
      if consteval { value=std::bit_cast<native_type>(data); }
      else { std::memcpy(&value, data.data(), sizeof(value)); }
    }
#if NATIVE_HOST_X86
    /// Return the native storage value under its minimal register ABI.
    native_nodiscard native_inline constexpr native_const native_target("sse2")
    operator native_type() const noexcept requires(sizeof(native_type)==16) { return value; }
    /// Adopt native storage without numerical conversion.
    native_nodiscard static native_inline constexpr native_const native_target("sse2")
    simd from_native(native_type x) noexcept requires(sizeof(native_type)==16) { return simd(x); }
    /// Return the native storage representation under its minimal register ABI.
    native_nodiscard native_inline constexpr native_const native_target("sse2")
    native_type to_native() const noexcept requires(sizeof(native_type)==16) { return value; }
    /// Return the native storage value under its minimal register ABI.
    native_nodiscard native_inline constexpr native_const native_target("avx")
    operator native_type() const noexcept requires(sizeof(native_type)==32) { return value; }
    /// Adopt native storage without numerical conversion.
    native_nodiscard static native_inline constexpr native_const native_target("avx")
    simd from_native(native_type x) noexcept requires(sizeof(native_type)==32) { return simd(x); }
    /// Return the native storage representation under its minimal register ABI.
    native_nodiscard native_inline constexpr native_const native_target("avx")
    native_type to_native() const noexcept requires(sizeof(native_type)==32) { return value; }
    /// Return the native storage value under its minimal register ABI.
    native_nodiscard native_inline constexpr native_const native_target("avx512f")
    operator native_type() const noexcept requires(sizeof(native_type)==64) { return value; }
    /// Adopt native storage without numerical conversion.
    native_nodiscard static native_inline constexpr native_const native_target("avx512f")
    simd from_native(native_type x) noexcept requires(sizeof(native_type)==64) { return simd(x); }
    /// Return the native storage representation under its minimal register ABI.
    native_nodiscard native_inline constexpr native_const native_target("avx512f")
    native_type to_native() const noexcept requires(sizeof(native_type)==64) { return value; }
#else
    /// Return the native storage value without a numerical conversion.
    native_nodiscard native_inline constexpr native_const operator native_type() const noexcept { return value; }
    /// Adopt native storage without numerical conversion.
    native_nodiscard native_inline constexpr static native_const simd from_native(native_type x) noexcept { return simd(x); }
    /// Return the native storage representation.
    native_nodiscard native_inline constexpr native_const native_type to_native() const noexcept { return value; }
#endif
    /// Add corresponding lanes modulo 2^(sizeof(T)*8).
    native_nodiscard friend native_inline constexpr native_const simd operator+(simd a, simd b) noexcept {
      if consteval {
        using U=std::make_unsigned_t<T>;
        using V=U __attribute__((ext_vector_type(N)));
        return from_native(__builtin_bit_cast(native_type,
          __builtin_bit_cast(V,a.value) + __builtin_bit_cast(V,b.value)));
      }
      return from_native(::NATIVE_BACKEND_NAMESPACE::integer_add<T>(a.value, b.value));
    }
    /// Subtract corresponding lanes modulo 2^(sizeof(T)*8).
    native_nodiscard friend native_inline constexpr native_const simd operator-(simd a, simd b) noexcept {
      if consteval {
        using U=std::make_unsigned_t<T>;
        using V=U __attribute__((ext_vector_type(N)));
        return from_native(__builtin_bit_cast(native_type,
          __builtin_bit_cast(V,a.value) - __builtin_bit_cast(V,b.value)));
      }
      return from_native(::NATIVE_BACKEND_NAMESPACE::integer_sub<T>(a.value, b.value));
    }
    /// Multiply corresponding lanes modulo 2^(sizeof(T)*8).
    native_nodiscard friend native_inline constexpr native_const simd operator*(simd a, simd b) noexcept {
      if consteval {
        using U=std::make_unsigned_t<T>;
        using V=U __attribute__((ext_vector_type(N)));
        return from_native(__builtin_bit_cast(native_type,
          __builtin_bit_cast(V,a.value) * __builtin_bit_cast(V,b.value)));
      }
      return from_native(::NATIVE_BACKEND_NAMESPACE::integer_mul<T>(a.value, b.value));
    }
    /// Bitwise AND of corresponding lane representations.
    native_nodiscard friend native_inline constexpr native_const simd operator&(simd a, simd b) noexcept {
      if consteval {
        using U=std::make_unsigned_t<T>;
        using V=U __attribute__((ext_vector_type(N)));
        return from_native(__builtin_bit_cast(native_type,
          __builtin_bit_cast(V,a.value) & __builtin_bit_cast(V,b.value)));
      }
      return from_native(::NATIVE_BACKEND_NAMESPACE::integer_and(a.value, b.value));
    }
    /// Bitwise OR of corresponding lane representations.
    native_nodiscard friend native_inline constexpr native_const simd operator|(simd a, simd b) noexcept {
      if consteval {
        using U=std::make_unsigned_t<T>;
        using V=U __attribute__((ext_vector_type(N)));
        return from_native(__builtin_bit_cast(native_type,
          __builtin_bit_cast(V,a.value) | __builtin_bit_cast(V,b.value)));
      }
      return from_native(::NATIVE_BACKEND_NAMESPACE::integer_or(a.value, b.value));
    }
    /// Bitwise XOR of corresponding lane representations.
    native_nodiscard friend native_inline constexpr native_const simd operator^(simd a, simd b) noexcept {
      if consteval {
        using U=std::make_unsigned_t<T>;
        using V=U __attribute__((ext_vector_type(N)));
        return from_native(__builtin_bit_cast(native_type,
          __builtin_bit_cast(V,a.value) ^ __builtin_bit_cast(V,b.value)));
      }
      return from_native(::NATIVE_BACKEND_NAMESPACE::integer_xor(a.value, b.value));
    }
    /// Complement every bit in every lane.
    native_nodiscard friend native_inline constexpr native_const simd operator~(simd a) noexcept {
      return a ^ simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(~std::make_unsigned_t<T>(0)));
    }
    /// Negate each lane modulo 2^(sizeof(T)*8).
    native_nodiscard friend native_inline constexpr native_const simd operator-(simd a) noexcept { return simd(T(0)) - a; }
    /// Return the unchanged vector value.
    native_nodiscard friend native_inline constexpr native_const simd operator+(simd a) noexcept { return a; }
    /// Return a mask whose lanes are true where `a == b` holds. Ordering follows the signedness of T.
    native_nodiscard friend native_inline constexpr native_const mask_type operator==(simd a, simd b) noexcept {
      if consteval {
        auto x=__builtin_bit_cast(std::array<T,N>,a.value);
        auto y=__builtin_bit_cast(std::array<T,N>,b.value);
        std::uint64_t bits=0;
        for(std::size_t i=0;i<N;++i) bits|=std::uint64_t(x[i] == y[i])<<i;
        return mask_type::from_bitset(bits);
      }
      return mask_type::unsafe_from_native(::NATIVE_BACKEND_NAMESPACE::integer_compare<T, false>(a.value, b.value));
    }
    /// Return a mask whose lanes are true where `a > b` holds. Ordering follows the signedness of T.
    native_nodiscard friend native_inline constexpr native_const mask_type operator>(simd a, simd b) noexcept {
      if consteval {
        auto x=__builtin_bit_cast(std::array<T,N>,a.value);
        auto y=__builtin_bit_cast(std::array<T,N>,b.value);
        std::uint64_t bits=0;
        for(std::size_t i=0;i<N;++i) bits|=std::uint64_t(x[i] > y[i])<<i;
        return mask_type::from_bitset(bits);
      }
      return mask_type::unsafe_from_native(::NATIVE_BACKEND_NAMESPACE::integer_compare<T, true>(a.value, b.value));
    }
    /// Return a mask whose lanes are true where `a != b` holds. Ordering follows the signedness of T.
    native_nodiscard friend native_inline constexpr native_const mask_type operator!=(simd a, simd b) noexcept {
      return ~(a == b);
    }
    /// Return a mask whose lanes are true where `a < b` holds. Ordering follows the signedness of T.
    native_nodiscard friend native_inline constexpr native_const mask_type operator<(simd a, simd b) noexcept {
      return b > a;
    }
    /// Return a mask whose lanes are true where `a <= b` holds. Ordering follows the signedness of T.
    native_nodiscard friend native_inline constexpr native_const mask_type operator<=(simd a, simd b) noexcept {
      return ~(a > b);
    }
    /// Return a mask whose lanes are true where `a >= b` holds. Ordering follows the signedness of T.
    native_nodiscard friend native_inline constexpr native_const mask_type operator>=(simd a, simd b) noexcept {
      return ~(b > a);
    }
    /// Add corresponding lanes modulo 2^(sizeof(T)*8). Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline constexpr native_const simd operator+(simd a, U b) noexcept {
      return a + simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Add corresponding lanes modulo 2^(sizeof(T)*8). Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline constexpr native_const simd operator+(U a, simd b) noexcept {
      return simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(a)) + b;
    }
    /// Subtract corresponding lanes modulo 2^(sizeof(T)*8). Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline constexpr native_const simd operator-(simd a, U b) noexcept {
      return a - simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Subtract corresponding lanes modulo 2^(sizeof(T)*8). Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline constexpr native_const simd operator-(U a, simd b) noexcept {
      return simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(a)) - b;
    }
    /// Multiply corresponding lanes modulo 2^(sizeof(T)*8). Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline constexpr native_const simd operator*(simd a, U b) noexcept {
      return a * simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Multiply corresponding lanes modulo 2^(sizeof(T)*8). Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline constexpr native_const simd operator*(U a, simd b) noexcept {
      return simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(a)) * b;
    }
    /// Bitwise AND of corresponding lane representations. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline constexpr native_const simd operator&(simd a, U b) noexcept {
      return a & simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Bitwise AND of corresponding lane representations. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline constexpr native_const simd operator&(U a, simd b) noexcept {
      return simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(a)) & b;
    }
    /// Bitwise OR of corresponding lane representations. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline constexpr native_const simd operator|(simd a, U b) noexcept {
      return a | simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Bitwise OR of corresponding lane representations. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline constexpr native_const simd operator|(U a, simd b) noexcept {
      return simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(a)) | b;
    }
    /// Bitwise XOR of corresponding lane representations. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline constexpr native_const simd operator^(simd a, U b) noexcept {
      return a ^ simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Bitwise XOR of corresponding lane representations. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline constexpr native_const simd operator^(U a, simd b) noexcept {
      return simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(a)) ^ b;
    }
    /// Return a mask whose lanes are true where `a == b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline constexpr native_const mask_type operator==(simd a, U b) noexcept {
      return a == simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Return a mask whose lanes are true where `a == b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline constexpr native_const mask_type operator==(U a, simd b) noexcept {
      return simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(a)) == b;
    }
    /// Return a mask whose lanes are true where `a != b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline constexpr native_const mask_type operator!=(simd a, U b) noexcept {
      return a != simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Return a mask whose lanes are true where `a != b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline constexpr native_const mask_type operator!=(U a, simd b) noexcept {
      return simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(a)) != b;
    }
    /// Return a mask whose lanes are true where `a < b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline constexpr native_const mask_type operator<(simd a, U b) noexcept {
      return a < simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Return a mask whose lanes are true where `a < b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline constexpr native_const mask_type operator<(U a, simd b) noexcept {
      return simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(a)) < b;
    }
    /// Return a mask whose lanes are true where `a > b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline constexpr native_const mask_type operator>(simd a, U b) noexcept {
      return a > simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Return a mask whose lanes are true where `a > b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline constexpr native_const mask_type operator>(U a, simd b) noexcept {
      return simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(a)) > b;
    }
    /// Return a mask whose lanes are true where `a <= b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline constexpr native_const mask_type operator<=(simd a, U b) noexcept {
      return a <= simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Return a mask whose lanes are true where `a <= b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline constexpr native_const mask_type operator<=(U a, simd b) noexcept {
      return simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(a)) <= b;
    }
    /// Return a mask whose lanes are true where `a >= b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline constexpr native_const mask_type operator>=(simd a, U b) noexcept {
      return a >= simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(b));
    }
    /// Return a mask whose lanes are true where `a >= b` holds. Ordering follows the signedness of T. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U>
    native_nodiscard friend native_inline constexpr native_const mask_type operator>=(U a, simd b) noexcept {
      return simd(::NATIVE_BACKEND_NAMESPACE::integer_wrap<T>(a)) >= b;
    }
    /// Apply the corresponding lane-wise add operation in place and return *this.
    native_inline constexpr simd &operator+=(simd b) noexcept { return *this = *this + b; }
    /// Apply the corresponding lane-wise add operation in place and return *this. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U> native_inline constexpr simd &operator+=(U b) noexcept { return *this = *this + b; }
    /// Apply the corresponding lane-wise subtract operation in place and return *this.
    native_inline constexpr simd &operator-=(simd b) noexcept { return *this = *this - b; }
    /// Apply the corresponding lane-wise subtract operation in place and return *this. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U> native_inline constexpr simd &operator-=(U b) noexcept { return *this = *this - b; }
    /// Apply the corresponding lane-wise multiply operation in place and return *this.
    native_inline constexpr simd &operator*=(simd b) noexcept { return *this = *this * b; }
    /// Apply the corresponding lane-wise multiply operation in place and return *this. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U> native_inline constexpr simd &operator*=(U b) noexcept { return *this = *this * b; }
    /// Apply the corresponding lane-wise AND operation in place and return *this.
    native_inline constexpr simd &operator&=(simd b) noexcept { return *this = *this & b; }
    /// Apply the corresponding lane-wise AND operation in place and return *this. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U> native_inline constexpr simd &operator&=(U b) noexcept { return *this = *this & b; }
    /// Apply the corresponding lane-wise OR operation in place and return *this.
    native_inline constexpr simd &operator|=(simd b) noexcept { return *this = *this | b; }
    /// Apply the corresponding lane-wise OR operation in place and return *this. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U> native_inline constexpr simd &operator|=(U b) noexcept { return *this = *this | b; }
    /// Apply the corresponding lane-wise XOR operation in place and return *this.
    native_inline constexpr simd &operator^=(simd b) noexcept { return *this = *this ^ b; }
    /// Apply the corresponding lane-wise XOR operation in place and return *this. Scalar operands are reduced to the lane width and broadcast first.
    template <simd_integer_element U> native_inline constexpr simd &operator^=(U b) noexcept { return *this = *this ^ b; }
    /// Shift each lane left by compile-time K, discarding high bits; require K below the lane bit width.
    template <unsigned K>
      requires(K < sizeof(T) * 8)
    native_nodiscard native_inline constexpr native_const simd left() const noexcept {
      if consteval {
        using E=std::make_unsigned_t<T>;
        using V=E __attribute__((ext_vector_type(N)));
        return from_native(__builtin_bit_cast(native_type,__builtin_bit_cast(V,value) << K));
      }
      return from_native(::NATIVE_BACKEND_NAMESPACE::integer_left<T, K>(value));
    }
    /// Shift each lane right by compile-time K; signed lanes extend their sign. Require K below the lane bit width.
    template <unsigned K>
      requires(K < sizeof(T) * 8)
    native_nodiscard native_inline constexpr native_const simd right() const noexcept {
      if consteval {
        using E=T;
        using V=E __attribute__((ext_vector_type(N)));
        return from_native(__builtin_bit_cast(native_type,__builtin_bit_cast(V,value) >> K));
      }
      return from_native(::NATIVE_BACKEND_NAMESPACE::integer_right<T, K>(value));
    }
    /// Shift every lane left by K bits, discarding high bits. Require K smaller than the lane bit width.
    template <std::size_t K>
      requires(K < sizeof(T) * 8)
    native_nodiscard friend native_inline constexpr native_const simd operator<<(simd a, imm_t<K>) noexcept {
      return a.template left<K>();
    }
    /// Shift every lane right by K; signed lanes extend the sign, unsigned lanes shift in zero. Require K smaller than the lane bit width.
    template <std::size_t K>
      requires(K < sizeof(T) * 8)
    native_nodiscard friend native_inline constexpr native_const simd operator>>(simd a, imm_t<K>) noexcept {
      return a.template right<K>();
    }
    /// Read all logical lanes from an unaligned element pointer.
    native_nodiscard native_inline static native_pure constexpr simd load(T const *p) noexcept {
      return ::NATIVE_BACKEND_NAMESPACE::load_simd<Arch,T, lanes>(p);
    }
    /// Read all logical lanes without an extra alignment promise.
    native_nodiscard native_inline static native_pure constexpr simd loadu(T const *p) noexcept {
      return ::NATIVE_BACKEND_NAMESPACE::load_simd<Arch,T, lanes>(p);
    }
    /// Write all logical lanes to an unaligned element pointer.
    native_inline constexpr void store(T *p) const noexcept { ::NATIVE_BACKEND_NAMESPACE::store_simd(p, *this); }
    /// Write all logical lanes without an extra alignment promise.
    native_inline constexpr void storeu(T *p) const noexcept { ::NATIVE_BACKEND_NAMESPACE::store_simd(p, *this); }
    /// Read exactly n logical lanes and fill the remainder; require n <= lanes. For n == 0, p may be null.
    native_nodiscard native_inline constexpr static native_pure simd load_partial(T const *p, std::size_t n,
                                                                  T fill = T(0)) noexcept native_diagnose_if(n > simd::lanes,"partial SIMD count exceeds the lane count") {
      assert(n <= lanes);
      if consteval {
        std::array<T,N> data; data.fill(fill);
        for(std::size_t i=0;i<n;++i) data[i]=p[i];
        return simd(data);
      }
      if (n == lanes) return load(p);
      if (!n) return simd(fill);
      // Native 32/64-bit lane tails must not touch masked-off addresses. This
      // also accepts byte-unaligned sources; scalar tail loads use memcpy.
#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ
      if constexpr (sizeof(T) == 4 || sizeof(T) == 8) {
        auto active = std::uint64_t((std::uint64_t(1) << n) - 1);
        if constexpr (sizeof(T) * N == 64) {
          if constexpr (sizeof(T) == 4)
            return from_native(_mm512_mask_loadu_epi32(simd(fill).value, __mmask16(active), p));
          else return from_native(_mm512_mask_loadu_epi64(simd(fill).value, __mmask8(active), p));
        }
#if NATIVE_HAS_AVX512VL
        else if constexpr (sizeof(T) * N == 32) {
          if constexpr (sizeof(T) == 4)
            return from_native(_mm256_mask_loadu_epi32(simd(fill).value, __mmask8(active), p));
          else return from_native(_mm256_mask_loadu_epi64(simd(fill).value, __mmask8(active), p));
        } else {
          if constexpr (sizeof(T) == 4)
            return from_native(_mm_mask_loadu_epi32(simd(fill).value, __mmask8(active), p));
          else return from_native(_mm_mask_loadu_epi64(simd(fill).value, __mmask8(active), p));
        }
#endif
      }
#endif
#if NATIVE_HAS_AVX2
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
        return from_native(::NATIVE_BACKEND_NAMESPACE::integer_bit_select(active, loaded, simd(fill).value));
      }
#endif
#if NATIVE_HAS_ARM_NEON
      if constexpr (sizeof(T) == 4) {
        auto result = vreinterpretq_u32_u8(simd(fill).value);
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
        return from_native(vreinterpretq_u8_u64(vsetq_lane_u64(word, vreinterpretq_u64_u8(simd(fill).value), 0)));
      }
#endif
      std::array<T, lanes> data;
      data.fill(fill);
      if (n)
        std::memcpy(data.data(), static_cast<void const *>(p), n * sizeof(T));
      return ::NATIVE_BACKEND_NAMESPACE::load_simd<Arch,T, lanes>(data.data());
    }
    /// Write exactly n logical lanes; require n <= lanes. For n == 0, p may be null.
    native_inline constexpr void store_partial(T *p, std::size_t n) const noexcept native_diagnose_if(n > simd::lanes,"partial SIMD count exceeds the lane count") {
      if consteval {
        assert(n<=N);
        auto data=__builtin_bit_cast(std::array<T,N>,value);
        for(std::size_t i=0;i<n;++i) p[i]=data[i];
        return;
      } ::NATIVE_BACKEND_NAMESPACE::store_simd_partial(p, *this, n); }
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator+(simd, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator+(U, simd) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator+(simd, ::native::simd<U, M,Arch>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator-(simd, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator-(U, simd) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator-(simd, ::native::simd<U, M,Arch>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator*(simd, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator*(U, simd) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator*(simd, ::native::simd<U, M,Arch>) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator/(simd, U) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator/(U, simd) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator/(simd, ::native::simd<U, M,Arch>) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator%(simd, U) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator%(U, simd) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator%(simd, ::native::simd<U, M,Arch>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator&(simd, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator&(U, simd) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator&(simd, ::native::simd<U, M,Arch>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator|(simd, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator|(U, simd) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator|(simd, ::native::simd<U, M,Arch>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator^(simd, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator^(U, simd) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator^(simd, ::native::simd<U, M,Arch>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator==(simd, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator==(U, simd) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator==(simd, ::native::simd<U, M,Arch>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator!=(simd, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator!=(U, simd) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator!=(simd, ::native::simd<U, M,Arch>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator<(simd, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator<(U, simd) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator<(simd, ::native::simd<U, M,Arch>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator>(simd, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator>(U, simd) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator>(simd, ::native::simd<U, M,Arch>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator<=(simd, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator<=(U, simd) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator<=(simd, ::native::simd<U, M,Arch>) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator>=(simd, U) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <class U> requires (std::is_arithmetic_v<U> && !simd_integer_element<U>) friend void operator>=(U, simd) = delete;
    /// Reject this unsupported operand combination instead of converting implicitly to a native register.
    template <simd_integer_element U, std::size_t M>
      requires(!std::same_as<simd, ::native::simd<U, M,Arch>>)
    friend void operator>=(simd, ::native::simd<U, M,Arch>) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    friend simd operator/(simd, simd) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    friend simd operator%(simd, simd) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <simd_integer_element U> friend simd operator/(simd, U) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <simd_integer_element U> friend simd operator/(U, simd) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <simd_integer_element U> friend simd operator%(simd, U) = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template <simd_integer_element U> friend simd operator%(U, simd) = delete;
    /// Reject runtime shift counts; use a compile-time imm<K> within the lane width.
    template <simd_integer_element U> friend simd operator<<(simd, U) = delete;
    /// Reject runtime shift counts; use a compile-time imm<K> within the lane width.
    template <simd_integer_element U> friend simd operator>>(simd, U) = delete;
  };

#endif

  namespace detail::NATIVE_BACKEND {
  template <::native::isa<> Arch, simd_integer_element T, std::size_t N, std::size_t A, simd_access Access>
    requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::integer_shape<T, N>
  native_nodiscard native_inline native_pure constexpr simd<T, N,Arch> load_simd(T const *p, simd_memory<A, Access>) noexcept {
    using V = simd<T, N,Arch>;
    if consteval {
      if constexpr(N==1) return V(*p);
      else {
        std::array<T,N> data;
        for(std::size_t i=0;i<N;++i) data[i]=p[i];
        return V(data);
      }
    }
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
  template <simd_integer_element T, std::size_t N, std::size_t A, simd_access Access, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::integer_shape<T, N>
  native_inline constexpr void store_simd(T *p, simd<T, N,Arch> v, simd_memory<A, Access>) noexcept {
    if consteval {
      if constexpr(N==1) *p=v.value;
      else {
        auto data=std::bit_cast<std::array<T,N>>(v.value);
        for(std::size_t i=0;i<N;++i) p[i]=data[i];
      }
      return;
    }
#if defined(__GNUC__) || defined(__clang__)
    if constexpr (A > 1)
      p = static_cast<T *>(__builtin_assume_aligned(p, A));
#elif defined(_MSC_VER)
    if constexpr (A > 1)
      __assume((reinterpret_cast<std::uintptr_t>(p) & (A - 1)) == 0);
#endif
    std::memcpy(static_cast<void *>(p), &v.value, sizeof(v.value));
  }
  template <::native::isa<> Arch, simd_integer_element T, std::size_t N, std::size_t A, simd_access Access>
    requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::integer_shape<T, N>
  native_nodiscard native_inline constexpr native_pure simd<T, N,Arch> load_simd_partial(T const *p, std::size_t count,
                                                                    simd_memory<A, Access>) noexcept native_diagnose_if(count > N,"partial SIMD count exceeds the lane count") {
    assert(count <= N);
    std::array<T, N> data{};
    if consteval { for(std::size_t i=0;i<count;++i) data[i]=p[i]; }
    else { if (count) std::memcpy(data.data(), static_cast<void const *>(p), count * sizeof(T)); }
    return ::NATIVE_BACKEND_NAMESPACE::load_simd<Arch,T, N>(data.data());
  }
  template <simd_integer_element T, std::size_t N, std::size_t A, simd_access Access, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::integer_shape<T, N>
  native_inline constexpr void store_simd_partial(T *p, simd<T, N,Arch> v, std::size_t count,
                                      simd_memory<A, Access>) noexcept native_diagnose_if(count > N,"partial SIMD count exceeds the lane count") {
    assert(count <= N);
    std::array<T, N> data;
    store_simd(data.data(), v);
    if consteval { for(std::size_t i=0;i<count;++i) p[i]=data[i]; }
    else { if (count) std::memcpy(static_cast<void *>(p), data.data(), count * sizeof(T)); }
  }
  template <::native::isa<> Arch, simd_integer_element T, std::size_t N>
    requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::integer_shape<T, N>
  native_nodiscard native_inline constexpr native_pure simd<T, N,Arch> load_simd(std::array<T, N> const &p) noexcept {
    return ::NATIVE_BACKEND_NAMESPACE::load_simd<Arch,T, N>(p.data());
  }
  template <::native::isa<> Arch, simd_integer_element T, std::size_t N>
    requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::integer_shape<T, N>
  native_nodiscard native_inline constexpr native_pure simd<T, N,Arch> load_simd(std::span<T const, N> p) noexcept {
    return ::NATIVE_BACKEND_NAMESPACE::load_simd<Arch,T, N>(p.data());
  }


  }
  namespace detail::NATIVE_BACKEND {
  template <class M, class T, std::size_t N, isa<> Arch>
  concept integer_mask_for =
      std::same_as<M, typename simd<T, N,Arch>::mask_type> || std::same_as<M, simd<::NATIVE_BACKEND_NAMESPACE::mask_lane_for<T>, N,Arch>>;
  }
  /// \ingroup masks
  /// Choose a in true lanes and b in false lanes; both operands are evaluated.
  template <simd_integer_element T, std::size_t N, class M, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) &&(::NATIVE_BACKEND_NAMESPACE::integer_shape<T, N> && ::NATIVE_BACKEND_NAMESPACE::integer_mask_for<M, T, N, Arch>)
  native_nodiscard native_inline constexpr native_const simd<T, N,Arch> select(M m, simd<T, N,Arch> a, simd<T, N,Arch> b) noexcept {
    if consteval {
      std::array<T,N> first{},second{};
      a.store(first.data()); b.store(second.data());
      auto bits=m.to_bitset();
      for(std::size_t i=0;i<N;++i) if(!((bits>>i)&1)) first[i]=second[i];
      return simd<T,N,Arch>(first);
    }
    if constexpr (N == 1)
      return any(m) ? a : b;
#if NATIVE_HAS_AVX2 || NATIVE_HAS_ARM_NEON || (NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ)
    else
      return simd<T, N,Arch>::from_native(::NATIVE_BACKEND_NAMESPACE::integer_select<T>(m, a.value, b.value));
#endif
  }
  /// \ingroup masks
  /// Select individual bits: (bits & a) | (~bits & b); no mask canonicalization.
  template <simd_integer_element T, std::size_t N, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::integer_shape<T, N>
  native_nodiscard native_inline constexpr native_const simd<T, N,Arch> bit_select(simd<T, N,Arch> bits, simd<T, N,Arch> a,
                                                              simd<T, N,Arch> b) noexcept {
    if consteval { return (bits & a) | (~bits & b); }
    if constexpr(N==1) return (bits & a) | (~bits & b);
#if NATIVE_HAS_AVX2 || NATIVE_HAS_ARM_NEON || (NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ)
    else return simd<T,N,Arch>::from_native(::NATIVE_BACKEND_NAMESPACE::integer_bit_select(bits.value,a.value,b.value));
#endif
  }
  // Compatibility: numeric masks select individual bits, never lane truth values.
  /// \ingroup masks
  /// Choose a in true lanes and b in false lanes; both operands are evaluated.
  template <simd_integer_element T, std::size_t N, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::integer_shape<T, N>
  native_nodiscard native_inline constexpr native_const simd<T, N,Arch> select(simd<T, N,Arch> bits, simd<T, N,Arch> a,
                                                          simd<T, N,Arch> b) noexcept {
    return bit_select(bits, a, b);
  }
  /// \ingroup masks
  /// Expand lane truth into unsigned integer zero/all-one words; preserve the lane count.
  template <simd_mask_element M, std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch)
  native_nodiscard native_inline constexpr native_const auto mask_bits(simd<M, N,Arch> m) noexcept {
    using U = typename M::storage_type;
    return simd<U, N,Arch>::from_native(m.to_native());
  }
  /// \ingroup masks
  /// Expand lane truth into unsigned integer zero/all-one words; preserve the lane count.
  template <simd_integer_element T, simd_mask_element M, std::size_t N, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) &&(sizeof(T) == sizeof(M))
  native_nodiscard native_inline constexpr native_const simd<std::make_unsigned_t<T>, N,Arch> mask_bits(simd<M, N,Arch> m) noexcept {
    return mask_bits(m);
  }
  /// \ingroup masks
  /// Expand lane truth into unsigned integer zero/all-one words; preserve the lane count.
  template <simd_integer_element T, std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) &&
    requires(predicate<N,Arch> m) { to_vector_mask<::NATIVE_BACKEND_NAMESPACE::mask_lane_for<T>>(m); }
  native_nodiscard native_inline constexpr native_const simd<std::make_unsigned_t<T>, N,Arch> mask_bits(predicate<N,Arch> m) noexcept {
    return mask_bits(to_vector_mask<::NATIVE_BACKEND_NAMESPACE::mask_lane_for<T>>(m));
  }
  /// \ingroup masks
  /// Add modulo the lane width in active lanes, retaining prior elsewhere.
  template <simd_integer_element T, std::size_t N, class M, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) &&(::NATIVE_BACKEND_NAMESPACE::integer_shape<T, N> && ::NATIVE_BACKEND_NAMESPACE::integer_mask_for<M, T, N, Arch>)
  native_nodiscard native_inline constexpr native_const simd<T, N,Arch> masked_add(M m, simd<T, N,Arch> prior, simd<T, N,Arch> a,
                                                              simd<T, N,Arch> b) noexcept {
    if consteval { return select(m,a + b,prior); }
    if constexpr (N == 1)
      return select(m, a + b, prior);
#if NATIVE_HAS_AVX2 || NATIVE_HAS_ARM_NEON || (NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ)
    else
      return simd<T, N,Arch>::from_native(::NATIVE_BACKEND_NAMESPACE::integer_masked_add<T>(m, prior.value, a.value, b.value));
#endif
  }
  /// \ingroup masks
  /// Add modulo the lane width in active lanes and zero inactive lanes.
  template <simd_integer_element T, std::size_t N, class M, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) &&(::NATIVE_BACKEND_NAMESPACE::integer_shape<T, N> && ::NATIVE_BACKEND_NAMESPACE::integer_mask_for<M, T, N, Arch>)
  native_nodiscard native_inline constexpr native_const simd<T, N,Arch> masked_add_zero(M m, simd<T, N,Arch> a, simd<T, N,Arch> b) noexcept {
    return masked_add(m, simd<T, N,Arch>(T(0)), a, b);
  }
  /// \ingroup masks
  /// Subtract modulo the lane width in active lanes, retaining prior elsewhere.
  template <simd_integer_element T, std::size_t N, class M, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) &&(::NATIVE_BACKEND_NAMESPACE::integer_shape<T, N> && ::NATIVE_BACKEND_NAMESPACE::integer_mask_for<M, T, N, Arch>)
  native_nodiscard native_inline constexpr native_const simd<T, N,Arch> masked_sub(M m, simd<T, N,Arch> prior, simd<T, N,Arch> a,
                                                              simd<T, N,Arch> b) noexcept {
    if consteval { return select(m,a - b,prior); }
    if constexpr (N == 1)
      return select(m, a - b, prior);
#if NATIVE_HAS_AVX2 || NATIVE_HAS_ARM_NEON || (NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ)
    else
      return simd<T, N,Arch>::from_native(::NATIVE_BACKEND_NAMESPACE::integer_masked_sub<T>(m, prior.value, a.value, b.value));
#endif
  }
  /// \ingroup masks
  /// Subtract modulo the lane width in active lanes and zero inactive lanes.
  template <simd_integer_element T, std::size_t N, class M, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) &&(::NATIVE_BACKEND_NAMESPACE::integer_shape<T, N> && ::NATIVE_BACKEND_NAMESPACE::integer_mask_for<M, T, N, Arch>)
  native_nodiscard native_inline constexpr native_const simd<T, N,Arch> masked_sub_zero(M m, simd<T, N,Arch> a, simd<T, N,Arch> b) noexcept {
    return masked_sub(m, simd<T, N,Arch>(T(0)), a, b);
  }
  /// \ingroup masks
  /// Multiply modulo the lane width in active lanes, retaining prior elsewhere.
  template <simd_integer_element T, std::size_t N, class M, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) &&(::NATIVE_BACKEND_NAMESPACE::integer_shape<T, N> && ::NATIVE_BACKEND_NAMESPACE::integer_mask_for<M, T, N, Arch>)
  native_nodiscard native_inline constexpr native_const simd<T, N,Arch> masked_mul(M m, simd<T, N,Arch> prior, simd<T, N,Arch> a,
                                                              simd<T, N,Arch> b) noexcept {
    if consteval { return select(m,a * b,prior); }
    if constexpr (N == 1)
      return select(m, a * b, prior);
#if NATIVE_HAS_AVX2 || NATIVE_HAS_ARM_NEON || (NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ)
    else
      return simd<T, N,Arch>::from_native(::NATIVE_BACKEND_NAMESPACE::integer_masked_mul<T>(m, prior.value, a.value, b.value));
#endif
  }
  /// \ingroup masks
  /// Multiply modulo the lane width in active lanes and zero inactive lanes.
  template <simd_integer_element T, std::size_t N, class M, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) &&(::NATIVE_BACKEND_NAMESPACE::integer_shape<T, N> && ::NATIVE_BACKEND_NAMESPACE::integer_mask_for<M, T, N, Arch>)
  native_nodiscard native_inline constexpr native_const simd<T, N,Arch> masked_mul_zero(M m, simd<T, N,Arch> a, simd<T, N,Arch> b) noexcept {
    return masked_mul(m, simd<T, N,Arch>(T(0)), a, b);
  }
} // namespace native

// Reject invalid immediate widths before implicit native conversion can select
// a builtin scalar shift. The immediate tag retains its public size conversion.
namespace native {
  /// Reject this unsupported operand combination instead of converting implicitly to a native register.
  template<simd_integer_element T, std::size_t N, std::size_t K, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && (K >= sizeof(T) * 8)
  void operator<<(simd<T, N,Arch>, imm_t<K>) = delete;
  /// Reject this unsupported operand combination instead of converting implicitly to a native register.
  template<simd_integer_element T, std::size_t N, std::size_t K, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && (K >= sizeof(T) * 8)
  void operator>>(simd<T, N,Arch>, imm_t<K>) = delete;
}

namespace NATIVE_BACKEND_NAMESPACE::native {
  // Compatibility names point toward the public native class template.
  using uint32x1=::native::simd<::native::uint32_t,1,NATIVE_DEFAULT_ARCH>;
#if NATIVE_HAS_AVX2 || NATIVE_HAS_ARM_NEON
  using uint32x4=::native::simd<::native::uint32_t,4,NATIVE_DEFAULT_ARCH>;
#endif
#if NATIVE_HAS_AVX2
  using uint32x8=::native::simd<::native::uint32_t,8,NATIVE_DEFAULT_ARCH>;
#endif
#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ
  using uint32x16=::native::simd<::native::uint32_t,16,NATIVE_DEFAULT_ARCH>;
#endif
  template<class V> concept unsigned_register = requires {
    typename V::unsigned_register_tag;
    typename V::value_type;
  } && std::same_as<typename V::value_type,::native::uint32_t>;
  template<unsigned S,unsigned_register V> requires(S<32)
  native_nodiscard native_inline native_const V shift_left(V x) noexcept { return x.template left<S>(); }
  template<unsigned S,unsigned_register V> requires(S<32)
  native_nodiscard native_inline native_const V shift_right(V x) noexcept { return x.template right<S>(); }
  using ::native::mask_bits;
  using ::native::bit_select;
  struct integer_target {
#if defined(NATIVE_INTEGER_SCALAR)
    using unsigned_type=uint32x1;
#elif NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ
    using unsigned_type=uint32x16;
#elif NATIVE_HAS_AVX2
    using unsigned_type=uint32x8;
#elif NATIVE_HAS_ARM_NEON
    using unsigned_type=uint32x4;
#else
    using unsigned_type=uint32x1;
#endif
    static constexpr std::size_t lanes=unsigned_type::lanes;
  };
}

namespace native {
  namespace detail::NATIVE_BACKEND {
    template <class V,std::size_t Alignment> native_nodiscard native_artificial native_inline constexpr native_pure V simd_load_native(native_noescape float const *) noexcept;
    template <class V,std::size_t Alignment> native_artificial native_inline constexpr void simd_store_native(native_noescape float *,V) noexcept;
  }
  namespace detail::NATIVE_BACKEND {
    template <class T> inline constexpr bool custom_argument = ::native::simd_custom_element<std::remove_cvref_t<T>>;
    template <class T, std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) inline constexpr bool custom_argument<simd<T,N,Arch>> = ::native::simd_custom_element<T>;
  }
  namespace detail::NATIVE_BACKEND {

  using ::native::detail::register_memory;
  }
  template <::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) struct native_empty_bases simd<float, 1,Arch> : ::NATIVE_BACKEND_NAMESPACE::register_memory<simd<float,1,Arch>, 1>, detail::swizzle_access<float,1,Arch> {
    static constexpr isa<> architecture=Arch;
    template <class T> using rebind = simd<T,1,Arch>;
    using vector_mask_type=simd<mask32,1,Arch>;
    using mask_type=::NATIVE_BACKEND_NAMESPACE::comparison_mask<float,1,Arch>;
    using mask = mask_type;
    using predicate_type = predicate<1,Arch>;
    float value{};
    /// Initialize the stored lane values to zero.
    native_inline simd() = default;
    /// Broadcast the supplied value to each logical lane.
    native_inline constexpr simd(float x) : value(x) {}
    /// Read all logical lanes from an unaligned element pointer.
    native_nodiscard static native_inline constexpr native_pure simd load(native_noescape float const * p) { return ::NATIVE_BACKEND_NAMESPACE::simd_load_native<simd,1>(p); }
    /// Write all logical lanes to an unaligned element pointer.
    native_inline constexpr void store(native_noescape float * p) const { ::NATIVE_BACKEND_NAMESPACE::simd_store_native<simd,1>(p,*this); }
    /// Add corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_nodiscard friend native_inline constexpr native_pure simd operator+(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::add,a,b); }
      return simd(a.value + b.value);
    }
    /// Subtract corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_nodiscard friend native_inline constexpr native_pure simd operator-(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::subtract,a,b); }
      return simd(a.value - b.value);
    }
    /// Multiply corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_nodiscard friend native_inline constexpr native_pure simd operator*(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::multiply,a,b); }
      return simd(a.value * b.value);
    }
    /// Divide corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_nodiscard friend native_inline constexpr native_pure simd operator/(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::divide,a,b); }
      return simd(a.value / b.value);
    }
    /// Negate every logical lane; floating-point lanes change sign.
    native_nodiscard friend native_inline constexpr native_const simd operator-(simd a) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::negate,a); }
      return simd(-a.value);
    }
    /// Return a mask whose lanes are true where `a < b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator<(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::compare(::native::detail::float_constant::less,a,b); }
      return mask_type::from_native(a.value < b.value ? ~std::uint32_t(0) : 0u);
    }
    /// Return a mask whose lanes are true where `a > b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator>(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::compare(::native::detail::float_constant::less,b,a); }
      return mask_type::from_native(a.value > b.value ? ~std::uint32_t(0) : 0u);
    }
    /// Return a mask whose lanes are true where `a == b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator==(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::compare(::native::detail::float_constant::equal,a,b); }
      return mask_type::from_native(a.value == b.value ? ~std::uint32_t(0) : 0u);
    }
    /// Choose a in true mask lanes and b in false lanes; both values are already evaluated.
    template<class M> requires (std::same_as<M,mask_type> || std::same_as<M,vector_mask_type>)
    native_nodiscard friend native_inline constexpr native_const simd select(M m,simd a,simd b) {
      if consteval { return ::native::detail::float_constant::select(m,a,b); }
      return m.to_native()!=0 ? a : b;
    }
    /// Compute a*b+c with one fused rounding per lane.
    native_nodiscard friend native_inline constexpr simd fma(simd a, simd b, simd c) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::multiply_add,a,b,c); }
      return simd(std::fma(a.value, b.value, c.value));
    }
    /// Take the native square root of each lane.
    native_nodiscard friend native_inline constexpr simd sqrt(simd a) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::square_root,a); }
      return simd(std::sqrt(a.value));
    }
    /// Round each lane to nearest integral value, choosing even at a tie.
    native_nodiscard friend native_inline constexpr native_pure simd round_even(simd a) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::nearest,a); }
      if (!std::isfinite(a.value) || std::abs(a.value) >= 0x1p23f) return a;
      float lo = std::floor(a.value), delta = a.value - lo;
      float r = lo + float(delta > .5f || (delta == .5f && std::fmod(lo, 2.f) != 0));
      return simd(r == 0 ? std::copysign(0.f, a.value) : r);
    }
    // Internal precondition: integral n in [-126,127].
    /// Construct normal powers of two; require integral exponents in [-126,127].
    native_nodiscard friend native_inline constexpr native_const simd normal_pow2(simd n) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::power_of_two,n); }
      return simd(std::bit_cast<float>(std::uint32_t(int(n.value) + 127) << 23));
    }

    template <std::size_t Alignment = 1>
    /// Load logical lanes, assuming the template alignment in bytes.
    native_nodiscard static native_inline constexpr native_pure simd load_memory(float const * p) noexcept {
      return ::NATIVE_BACKEND_NAMESPACE::simd_load_native<simd,Alignment>(p);
    }
    template <std::size_t Alignment = 1>
    /// Store logical lanes, assuming the template alignment in bytes.
    native_inline constexpr void store_memory(float * p) const noexcept {
      ::NATIVE_BACKEND_NAMESPACE::simd_store_native<simd,Alignment>(p,*this);
    }
    using value_type = float;
    using register_type = simd;
    using native_type = float;
    using bits_type = simd<uint32_t,1,Arch>;
    /// Return the native storage value without a numerical conversion.
    native_nodiscard native_inline constexpr native_pure operator native_type() const noexcept { return value; }
    /// Return the native storage representation.
    native_nodiscard native_inline constexpr native_pure native_type to_native() const noexcept { return value; }
    /// Project the exact lane representation into the corresponding unsigned vector.
    native_nodiscard native_inline constexpr native_pure bits_type bits() const noexcept { return bits_type(std::bit_cast<std::uint32_t>(value)); }
    /// Return the exact binary32 lane representations in the unsigned vector.
    native_nodiscard native_inline constexpr native_pure bits_type to_bits() const noexcept { return bits(); }
    /// Reinterpret binary words as lane values; do not normalize them.
    native_nodiscard static native_inline constexpr native_const simd from_bits(bits_type bits) noexcept { return simd(std::bit_cast<float>(bits.value)); }
    /// Reinterpret binary words as lane values; do not normalize them.
    native_nodiscard static native_inline constexpr native_const simd from_bits(std::uint32_t bits) noexcept { return from_bits(bits_type(bits)); }
    /// Broadcast the float value without adding an FTZ or other normalization policy.
    native_nodiscard static native_inline constexpr native_const simd from_float(float x) noexcept { return simd(x); }
    /// Adopt native storage without numerical conversion.
    native_nodiscard static native_inline constexpr native_const simd from_native(native_type x) noexcept { return simd(x); }
    /// Adopt raw float storage without numerical conversion or normalization.
    native_nodiscard static native_inline constexpr native_const simd unsafe_from_float32(native_type x) noexcept { return simd(x); }
    /// Read all logical lanes without an extra alignment promise.
    native_nodiscard static native_inline constexpr native_pure simd loadu(native_noescape float const * p) { return ::NATIVE_BACKEND_NAMESPACE::simd_load_native<simd,1>(p); }
    /// Write all logical lanes without an extra alignment promise.
    native_inline constexpr void storeu(native_noescape float * p) const { ::NATIVE_BACKEND_NAMESPACE::simd_store_native<simd,1>(p,*this); }
    /// Load exactly the logical count of binary32 words without normalizing their representations.
    native_nodiscard static native_inline constexpr native_pure simd load_bits(native_noescape std::uint32_t const * p) noexcept { return from_bits(bits_type::load(p)); }
    /// Store the exact binary32 words for every logical lane.
    native_inline constexpr void store_bits(native_noescape std::uint32_t * p) const noexcept { bits().store(p); }
    /// Read n representation words and fill the remaining logical lanes; require n <= lanes. A zero count permits null.
    native_nodiscard static native_inline constexpr native_pure simd load_bits_partial(native_noescape std::uint32_t const * p,std::size_t n,std::uint32_t fill=0) noexcept native_diagnose_if(n > simd::lanes,"partial SIMD count exceeds the lane count") { return from_bits(bits_type::load_partial(p,n,fill)); }
    /// Write n exact representation words; require n <= lanes. A zero count permits null.
    native_inline constexpr void store_bits_partial(native_noescape std::uint32_t * p,std::size_t n) const noexcept native_diagnose_if(n > simd::lanes,"partial SIMD count exceeds the lane count") { bits().store_partial(p,n); }
    /// Read all logical lanes without an extra alignment promise.
    native_inline constexpr simd(std::array<float,1> const & values) noexcept : simd(loadu(values.data())) {}
    /// Apply the corresponding lane-wise add operation in place and return *this.
    native_inline constexpr simd & operator+=(simd b) noexcept { return *this=*this+b; }
    /// Apply the corresponding lane-wise subtract operation in place and return *this.
    native_inline constexpr simd & operator-=(simd b) noexcept { return *this=*this-b; }
    /// Apply the corresponding lane-wise multiply operation in place and return *this.
    native_inline constexpr simd & operator*=(simd b) noexcept { return *this=*this*b; }
    /// Apply the corresponding lane-wise divide operation in place and return *this.
    native_inline constexpr simd & operator/=(simd b) noexcept { return *this=*this/b; }
    /// Return a mask whose lanes are true where `a != b` holds. NaN lanes compare unequal.
    native_nodiscard friend native_inline constexpr native_const mask_type operator!=(simd a,simd b) noexcept { return ~(a==b); }
    /// Return a mask whose lanes are true where `a <= b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator<=(simd a,simd b) noexcept { return (a<b)|(a==b); }
    /// Return a mask whose lanes are true where `a >= b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator>=(simd a,simd b) noexcept { return (a>b)|(a==b); }
  };
#if NATIVE_HAS_AVX2
}

namespace native {
  /// \ingroup vectors
  /// Raw x86 float storage; the Arch argument fixes comparison-mask representation.
  template <::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) struct native_empty_bases simd<float, 4,Arch> : detail::register_memory<simd<float,4,Arch>, 4>, detail::swizzle_access<float,4,Arch> {
    static constexpr isa<> architecture=Arch;
    template <class T> using rebind = simd<T,4,Arch>;
    using vector_mask_type=simd<mask32,4,Arch>;
    using mask_type=std::conditional_t<bool(NATIVE_HAS_AVX512VL),predicate<4,Arch>,simd<mask32,4,Arch>>;
    using mask = mask_type;
    using predicate_type = predicate<4,Arch>;
    __m128 value;
    /// Default initialization leaves storage unspecified; value initialization with braces zero-initializes it.
    native_inline simd() = default;
    /// Copy the stored value without arithmetic or normalization.
    native_inline constexpr simd(simd const &) = default;
    /// Copy the stored value and return *this; no numerical conversion is performed.
    native_reinitializes native_inline constexpr simd & operator=(simd const &) = default;
    /// Broadcast x to all lanes.
    native_inline constexpr simd(float x) {
      if consteval { std::array<float,sizeof(native_type)/sizeof(float)> values{}; values.fill(x); value=__builtin_bit_cast(native_type,values); }
      else { value=_mm_set1_ps(x); }
    }
    /// Adopt native lane storage without numerical conversion.
    native_inline native_target("sse") constexpr simd(__m128 x) : value(x) {}
    /// Load every logical lane; no extra alignment is required.
    native_nodiscard static native_inline constexpr native_pure simd load(native_noescape float const * p) { return load_memory<1>(p); }
    /// Store every logical lane; no extra alignment is required.
    native_inline constexpr void store(native_noescape float * p) const { store_memory<1>(p); }
    /// Add corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd operator+(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::add,a,b); }
      return simd(_mm_add_ps(a.value, b.value));
    }
    /// Subtract corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd operator-(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::subtract,a,b); }
      return simd(_mm_sub_ps(a.value, b.value));
    }
    /// Multiply corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd operator*(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::multiply,a,b); }
      return simd(_mm_mul_ps(a.value, b.value));
    }
    /// Divide corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd operator/(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::divide,a,b); }
      return simd(_mm_div_ps(a.value, b.value));
    }
    /// Negate every logical lane; floating-point lanes change sign.
    native_nodiscard friend native_inline constexpr native_const simd operator-(simd a) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::negate,a); }
      return simd(_mm_xor_ps(a.value, _mm_set1_ps(-0.f)));
    }
    /// Return a mask whose lanes are true where `a < b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator<(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::compare(::native::detail::float_constant::less,a,b); }
      if constexpr(bool(NATIVE_HAS_AVX512VL))
        return mask_type::from_native(_mm_cmp_ps_mask(a.value,b.value,_CMP_LT_OQ));
      else
        return mask_type::unsafe_from_native(_mm_castps_si128(_mm_cmp_ps(a.value,b.value,_CMP_LT_OQ)));
    }
    /// Return a mask whose lanes are true where `a > b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator>(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::compare(::native::detail::float_constant::less,b,a); }
      if constexpr(bool(NATIVE_HAS_AVX512VL))
        return mask_type::from_native(_mm_cmp_ps_mask(a.value,b.value,_CMP_GT_OQ));
      else
        return mask_type::unsafe_from_native(_mm_castps_si128(_mm_cmp_ps(a.value,b.value,_CMP_GT_OQ)));
    }
    /// Return a mask whose lanes are true where `a == b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator==(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::compare(::native::detail::float_constant::equal,a,b); }
      if constexpr(bool(NATIVE_HAS_AVX512VL))
        return mask_type::from_native(_mm_cmp_ps_mask(a.value,b.value,_CMP_EQ_OQ));
      else
        return mask_type::unsafe_from_native(_mm_castps_si128(_mm_cmp_ps(a.value,b.value,_CMP_EQ_OQ)));
    }
    /// Choose a where the canonical mask is true, otherwise b; both operands are evaluated.
    template<class M> requires (std::same_as<M,mask_type> || std::same_as<M,vector_mask_type>)
    native_nodiscard friend native_inline constexpr native_const simd select(M m,simd a,simd b) {
      if consteval { return ::native::detail::float_constant::select(m,a,b); }
      if constexpr(M::compact) return simd(_mm_mask_blend_ps(m.to_native(),b.value,a.value));
      else
        return simd(_mm_blendv_ps(b.value,a.value,_mm_castsi128_ps(m.to_native()))); }
    /// Compute a*b+c with one fused rounding per lane.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd fma(simd a, simd b, simd c) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::multiply_add,a,b,c); }
      return simd(_mm_fmadd_ps(a.value, b.value, c.value));
    }
    /// Compute the native square root in every lane.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd sqrt(simd a) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::square_root,a); }
      return simd(_mm_sqrt_ps(a.value));
    }
    /// Round to an integral value, ties to even, independent of ambient direction.
    native_artificial native_nodiscard friend native_inline constexpr native_const simd round_even(simd a) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::nearest,a); }
      return simd(_mm_round_ps(a.value, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC));
    }
    /// Construct normal powers of two; require integral exponents in [-126,127].
    native_nodiscard friend native_inline constexpr native_const simd normal_pow2(simd n) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::power_of_two,n); }
      return simd(_mm_castsi128_ps(_mm_slli_epi32(_mm_add_epi32(_mm_cvttps_epi32(n.value), _mm_set1_epi32(127)), 23)));
    }

    template <std::size_t Alignment = 1>
    /// Load full lanes, assuming Alignment-byte pointer alignment.
    native_nodiscard static native_inline constexpr native_pure simd load_memory(float const * p) noexcept {
      if consteval {
        std::array<float,sizeof(native_type)/sizeof(float)> values{};
        for (std::size_t i=0;i<sizeof(native_type)/sizeof(float);++i) values[i]=p[i];
        return from_native(__builtin_bit_cast(native_type,values));
      }
      if constexpr(Alignment>=16) return simd(_mm_load_ps(p));
      else return simd(_mm_loadu_ps(p));
    }
    template <std::size_t Alignment = 1>
    /// Store full lanes, assuming Alignment-byte pointer alignment.
    native_inline constexpr void store_memory(float * p) const noexcept {
      if consteval {
        auto values=__builtin_bit_cast(std::array<float,sizeof(native_type)/sizeof(float)>,value);
        for (std::size_t i=0;i<sizeof(native_type)/sizeof(float);++i) p[i]=values[i];
        return;
      }
      if constexpr(Alignment>=16) _mm_store_ps(p,value);
      else _mm_storeu_ps(p,value);
    }
    using value_type = float;
    using register_type = simd;
    using native_type = __m128;
    using bits_type = simd<uint32_t,4,Arch>;
    /// Return the native storage value without a numerical conversion.
    native_nodiscard native_inline constexpr native_pure native_target("sse") operator native_type() const noexcept { return value; }
    /// Project native register storage without a numerical conversion.
    native_nodiscard native_inline constexpr native_pure native_target("sse") native_type to_native() const noexcept { return value; }
    /// Project exact binary32 lane words into the unsigned vector.
    native_artificial native_nodiscard native_inline constexpr native_pure bits_type bits() const noexcept { if consteval { return bits_type::from_native(__builtin_bit_cast(typename bits_type::native_type,value)); } return bits_type::from_native(_mm_castps_si128(value)); }
    /// Synonym for bits(): preserve all binary32 representation bits.
    native_nodiscard native_inline constexpr native_pure bits_type to_bits() const noexcept { return bits(); }
    /// Reinterpret unsigned lane words as binary32, without normalization.
    native_artificial native_nodiscard static native_inline constexpr native_const simd from_bits(bits_type bits) noexcept { if consteval { return simd(__builtin_bit_cast(native_type,bits.to_native())); } return simd(_mm_castsi128_ps(bits.value)); }
    /// Reinterpret unsigned lane words as binary32, without normalization.
    native_nodiscard static native_inline constexpr native_const simd from_bits(std::uint32_t bits) noexcept { return from_bits(bits_type(bits)); }
    /// Broadcast one binary32 value to every lane.
    native_nodiscard static native_inline constexpr native_const simd from_float(float x) noexcept { return simd(x); }
    /// Adopt native register storage without changing its bits.
    native_nodiscard static native_inline constexpr native_const native_target("sse") simd from_native(native_type x) noexcept { return simd(x); }
    /// Adopt native raw float storage; this raw type adds no normalization.
    native_nodiscard static native_inline constexpr native_const native_target("sse") simd unsafe_from_float32(native_type x) noexcept { return simd(x); }
    /// Synonym for an unaligned full-vector load.
    native_nodiscard static native_inline constexpr native_pure simd loadu(native_noescape float const * p) { return load_memory<1>(p); }
    /// Synonym for an unaligned full-vector store.
    native_inline constexpr void storeu(native_noescape float * p) const { store_memory<1>(p); }
    /// Load exact binary32 representations from uint32_t words.
    native_nodiscard static native_inline constexpr native_pure simd load_bits(native_noescape std::uint32_t const * p) noexcept { return from_bits(bits_type::load(p)); }
    /// Store exact binary32 representations as uint32_t words.
    native_inline constexpr void store_bits(native_noescape std::uint32_t * p) const noexcept { bits().store(p); }
    /// Load n words and fill the remaining lanes; require n <= lanes.
    native_nodiscard static native_inline constexpr native_pure simd load_bits_partial(native_noescape std::uint32_t const * p,std::size_t n,std::uint32_t fill=0) noexcept native_diagnose_if(n > simd::lanes,"partial SIMD count exceeds the lane count") { return from_bits(bits_type::load_partial(p,n,fill)); }
    /// Store exactly n representation words; require n <= lanes.
    native_inline constexpr void store_bits_partial(native_noescape std::uint32_t * p,std::size_t n) const noexcept native_diagnose_if(n > simd::lanes,"partial SIMD count exceeds the lane count") { bits().store_partial(p,n); }
    /// Load one lane from each array element, in array order.
    native_inline constexpr simd(std::array<float,4> const & values) noexcept : simd(loadu(values.data())) {}
#if defined(__clang__)
    /// Convert one argument per lane; exceptions follow those named-lvalue conversions.
    template <class... X> requires (sizeof...(X)==4) && (std::convertible_to<X,float> && ...)
    native_inline constexpr simd(X... x) noexcept((noexcept(static_cast<float>(x)) && ...)) : value{static_cast<float>(x)...} {}
#else
    /// Convert one argument per lane; exceptions follow those named-lvalue conversions.
    template <class... X> requires (sizeof...(X)==4) && (std::convertible_to<X,float> && ...)
    native_inline constexpr simd(X... x) noexcept((noexcept(static_cast<float>(x)) && ...)) : simd(loadu(std::array<float,4>{static_cast<float>(x)...}.data())) {}
#endif
    /// Apply the corresponding lane-wise add operation in place and return *this.
    native_inline constexpr simd & operator+=(simd b) noexcept { return *this=*this+b; }
    /// Apply the corresponding lane-wise subtract operation in place and return *this.
    native_inline constexpr simd & operator-=(simd b) noexcept { return *this=*this-b; }
    /// Apply the corresponding lane-wise multiply operation in place and return *this.
    native_inline constexpr simd & operator*=(simd b) noexcept { return *this=*this*b; }
    /// Apply the corresponding lane-wise divide operation in place and return *this.
    native_inline constexpr simd & operator/=(simd b) noexcept { return *this=*this/b; }
    /// Return a mask whose lanes are true where `a != b` holds. NaN lanes compare unequal.
    native_nodiscard friend native_inline constexpr native_const mask_type operator!=(simd a,simd b) noexcept { return ~(a==b); }
    /// Return a mask whose lanes are true where `a <= b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator<=(simd a,simd b) noexcept { return (a<b)|(a==b); }
    /// Return a mask whose lanes are true where `a >= b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator>=(simd a,simd b) noexcept { return (a>b)|(a==b); }
  };

  /// \ingroup vectors
  /// Raw x86 float storage; the Arch argument fixes comparison-mask representation.
  template <::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) struct simd<float, 8,Arch> : detail::register_memory<simd<float,8,Arch>, 8> {
    static constexpr isa<> architecture=Arch;
    template <class T> using rebind = simd<T,8,Arch>;
    using vector_mask_type=simd<mask32,8,Arch>;
    using mask_type=std::conditional_t<bool(NATIVE_HAS_AVX512VL),predicate<8,Arch>,simd<mask32,8,Arch>>;
    using mask = mask_type;
    using predicate_type = predicate<8,Arch>;
    __m256 value;
    /// Default initialization leaves storage unspecified; value initialization with braces zero-initializes it.
    native_inline simd() = default;
    /// Copy the stored value without arithmetic or normalization.
    native_inline constexpr simd(simd const &) = default;
    /// Copy the stored value and return *this; no numerical conversion is performed.
    native_reinitializes native_inline constexpr simd & operator=(simd const &) = default;
    /// Broadcast x to all lanes.
    native_inline constexpr simd(float x) {
      if consteval { std::array<float,sizeof(native_type)/sizeof(float)> values{}; values.fill(x); value=__builtin_bit_cast(native_type,values); }
      else { value=_mm256_set1_ps(x); }
    }
    /// Adopt native lane storage without numerical conversion.
    native_inline native_target("avx") constexpr simd(__m256 x) : value(x) {}
    /// Load every logical lane; no extra alignment is required.
    native_nodiscard static native_inline constexpr native_pure simd load(native_noescape float const * p) { return load_memory<1>(p); }
    /// Store every logical lane; no extra alignment is required.
    native_inline constexpr void store(native_noescape float * p) const { store_memory<1>(p); }
    /// Add corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd operator+(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::add,a,b); }
      return simd(_mm256_add_ps(a.value, b.value));
    }
    /// Subtract corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd operator-(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::subtract,a,b); }
      return simd(_mm256_sub_ps(a.value, b.value));
    }
    /// Multiply corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd operator*(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::multiply,a,b); }
      return simd(_mm256_mul_ps(a.value, b.value));
    }
    /// Divide corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd operator/(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::divide,a,b); }
      return simd(_mm256_div_ps(a.value, b.value));
    }
    /// Negate every logical lane; floating-point lanes change sign.
    native_nodiscard friend native_inline constexpr native_const simd operator-(simd a) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::negate,a); }
      return simd(_mm256_xor_ps(a.value, _mm256_set1_ps(-0.f)));
    }
    /// Return a mask whose lanes are true where `a < b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator<(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::compare(::native::detail::float_constant::less,a,b); }
      if constexpr(bool(NATIVE_HAS_AVX512VL))
        return mask_type::from_native(_mm256_cmp_ps_mask(a.value,b.value,_CMP_LT_OQ));
      else
        return mask_type::unsafe_from_native(_mm256_castps_si256(_mm256_cmp_ps(a.value,b.value,_CMP_LT_OQ)));
    }
    /// Return a mask whose lanes are true where `a > b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator>(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::compare(::native::detail::float_constant::less,b,a); }
      if constexpr(bool(NATIVE_HAS_AVX512VL))
        return mask_type::from_native(_mm256_cmp_ps_mask(a.value,b.value,_CMP_GT_OQ));
      else
        return mask_type::unsafe_from_native(_mm256_castps_si256(_mm256_cmp_ps(a.value,b.value,_CMP_GT_OQ)));
    }
    /// Return a mask whose lanes are true where `a == b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator==(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::compare(::native::detail::float_constant::equal,a,b); }
      if constexpr(bool(NATIVE_HAS_AVX512VL))
        return mask_type::from_native(_mm256_cmp_ps_mask(a.value,b.value,_CMP_EQ_OQ));
      else
        return mask_type::unsafe_from_native(_mm256_castps_si256(_mm256_cmp_ps(a.value,b.value,_CMP_EQ_OQ)));
    }
    /// Choose a where the canonical mask is true, otherwise b; both operands are evaluated.
    template<class M> requires (std::same_as<M,mask_type> || std::same_as<M,vector_mask_type>)
    native_nodiscard friend native_inline constexpr native_const simd select(M m,simd a,simd b) {
      if consteval { return ::native::detail::float_constant::select(m,a,b); }
      if constexpr(M::compact) return simd(_mm256_mask_blend_ps(m.to_native(),b.value,a.value));
      else
        return simd(_mm256_blendv_ps(b.value,a.value,_mm256_castsi256_ps(m.to_native()))); }
    /// Compute a*b+c with one fused rounding per lane.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd fma(simd a, simd b, simd c) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::multiply_add,a,b,c); }
      return simd(_mm256_fmadd_ps(a.value, b.value, c.value));
    }
    /// Compute the native square root in every lane.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd sqrt(simd a) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::square_root,a); }
      return simd(_mm256_sqrt_ps(a.value));
    }
    /// Round to an integral value, ties to even, independent of ambient direction.
    native_artificial native_nodiscard friend native_inline constexpr native_const simd round_even(simd a) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::nearest,a); }
      return simd(_mm256_round_ps(a.value, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC));
    }
    /// Construct normal powers of two; require integral exponents in [-126,127].
    native_nodiscard friend native_inline constexpr native_const simd normal_pow2(simd n) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::power_of_two,n); }
      return simd(_mm256_castsi256_ps(_mm256_slli_epi32(_mm256_add_epi32(_mm256_cvttps_epi32(n.value), _mm256_set1_epi32(127)), 23)));
    }

    template <std::size_t Alignment = 1>
    /// Load full lanes, assuming Alignment-byte pointer alignment.
    native_nodiscard static native_inline constexpr native_pure simd load_memory(float const * p) noexcept {
      if consteval {
        std::array<float,sizeof(native_type)/sizeof(float)> values{};
        for (std::size_t i=0;i<sizeof(native_type)/sizeof(float);++i) values[i]=p[i];
        return from_native(__builtin_bit_cast(native_type,values));
      }
      if constexpr(Alignment>=32) return simd(_mm256_load_ps(p));
      else return simd(_mm256_loadu_ps(p));
    }
    template <std::size_t Alignment = 1>
    /// Store full lanes, assuming Alignment-byte pointer alignment.
    native_inline constexpr void store_memory(float * p) const noexcept {
      if consteval {
        auto values=__builtin_bit_cast(std::array<float,sizeof(native_type)/sizeof(float)>,value);
        for (std::size_t i=0;i<sizeof(native_type)/sizeof(float);++i) p[i]=values[i];
        return;
      }
      if constexpr(Alignment>=32) _mm256_store_ps(p,value);
      else _mm256_storeu_ps(p,value);
    }
    using value_type = float;
    using register_type = simd;
    using native_type = __m256;
    using bits_type = simd<uint32_t,8,Arch>;
    /// Return the native storage value without a numerical conversion.
    native_nodiscard native_inline constexpr native_pure native_target("avx") operator native_type() const noexcept { return value; }
    /// Project native register storage without a numerical conversion.
    native_nodiscard native_inline constexpr native_pure native_target("avx") native_type to_native() const noexcept { return value; }
    /// Project exact binary32 lane words into the unsigned vector.
    native_artificial native_nodiscard native_inline constexpr native_pure bits_type bits() const noexcept { if consteval { return bits_type::from_native(__builtin_bit_cast(typename bits_type::native_type,value)); } return bits_type::from_native(_mm256_castps_si256(value)); }
    /// Synonym for bits(): preserve all binary32 representation bits.
    native_nodiscard native_inline constexpr native_pure bits_type to_bits() const noexcept { return bits(); }
    /// Reinterpret unsigned lane words as binary32, without normalization.
    native_artificial native_nodiscard static native_inline constexpr native_const simd from_bits(bits_type bits) noexcept { if consteval { return simd(__builtin_bit_cast(native_type,bits.to_native())); } return simd(_mm256_castsi256_ps(bits.value)); }
    /// Reinterpret unsigned lane words as binary32, without normalization.
    native_nodiscard static native_inline constexpr native_const simd from_bits(std::uint32_t bits) noexcept { return from_bits(bits_type(bits)); }
    /// Broadcast one binary32 value to every lane.
    native_nodiscard static native_inline constexpr native_const simd from_float(float x) noexcept { return simd(x); }
    /// Adopt native register storage without changing its bits.
    native_nodiscard static native_inline constexpr native_const native_target("avx") simd from_native(native_type x) noexcept { return simd(x); }
    /// Adopt native raw float storage; this raw type adds no normalization.
    native_nodiscard static native_inline constexpr native_const native_target("avx") simd unsafe_from_float32(native_type x) noexcept { return simd(x); }
    /// Synonym for an unaligned full-vector load.
    native_nodiscard static native_inline constexpr native_pure simd loadu(native_noescape float const * p) { return load_memory<1>(p); }
    /// Synonym for an unaligned full-vector store.
    native_inline constexpr void storeu(native_noescape float * p) const { store_memory<1>(p); }
    /// Load exact binary32 representations from uint32_t words.
    native_nodiscard static native_inline constexpr native_pure simd load_bits(native_noescape std::uint32_t const * p) noexcept { return from_bits(bits_type::load(p)); }
    /// Store exact binary32 representations as uint32_t words.
    native_inline constexpr void store_bits(native_noescape std::uint32_t * p) const noexcept { bits().store(p); }
    /// Load n words and fill the remaining lanes; require n <= lanes.
    native_nodiscard static native_inline constexpr native_pure simd load_bits_partial(native_noescape std::uint32_t const * p,std::size_t n,std::uint32_t fill=0) noexcept native_diagnose_if(n > simd::lanes,"partial SIMD count exceeds the lane count") { return from_bits(bits_type::load_partial(p,n,fill)); }
    /// Store exactly n representation words; require n <= lanes.
    native_inline constexpr void store_bits_partial(native_noescape std::uint32_t * p,std::size_t n) const noexcept native_diagnose_if(n > simd::lanes,"partial SIMD count exceeds the lane count") { bits().store_partial(p,n); }
    /// Load one lane from each array element, in array order.
    native_inline constexpr simd(std::array<float,8> const & values) noexcept : simd(loadu(values.data())) {}
#if defined(__clang__)
    /// Convert one argument per lane; exceptions follow those named-lvalue conversions.
    template <class... X> requires (sizeof...(X)==8) && (std::convertible_to<X,float> && ...)
    native_inline constexpr simd(X... x) noexcept((noexcept(static_cast<float>(x)) && ...)) : value{static_cast<float>(x)...} {}
#else
    /// Convert one argument per lane; exceptions follow those named-lvalue conversions.
    template <class... X> requires (sizeof...(X)==8) && (std::convertible_to<X,float> && ...)
    native_inline constexpr simd(X... x) noexcept((noexcept(static_cast<float>(x)) && ...)) : simd(loadu(std::array<float,8>{static_cast<float>(x)...}.data())) {}
#endif
    /// Apply the corresponding lane-wise add operation in place and return *this.
    native_inline constexpr simd & operator+=(simd b) noexcept { return *this=*this+b; }
    /// Apply the corresponding lane-wise subtract operation in place and return *this.
    native_inline constexpr simd & operator-=(simd b) noexcept { return *this=*this-b; }
    /// Apply the corresponding lane-wise multiply operation in place and return *this.
    native_inline constexpr simd & operator*=(simd b) noexcept { return *this=*this*b; }
    /// Apply the corresponding lane-wise divide operation in place and return *this.
    native_inline constexpr simd & operator/=(simd b) noexcept { return *this=*this/b; }
    /// Return a mask whose lanes are true where `a != b` holds. NaN lanes compare unequal.
    native_nodiscard friend native_inline constexpr native_const mask_type operator!=(simd a,simd b) noexcept { return ~(a==b); }
    /// Return a mask whose lanes are true where `a <= b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator<=(simd a,simd b) noexcept { return (a<b)|(a==b); }
    /// Return a mask whose lanes are true where `a >= b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator>=(simd a,simd b) noexcept { return (a>b)|(a==b); }
  };

}

// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
namespace native {
#endif
#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ
  template <::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) struct simd<float, 16,Arch> : ::NATIVE_BACKEND_NAMESPACE::register_memory<simd<float,16,Arch>, 16> {
    static constexpr isa<> architecture=Arch;
    template <class T> using rebind = simd<T,16,Arch>;
    using vector_mask_type=simd<mask32,16,Arch>;
    using mask_type=::NATIVE_BACKEND_NAMESPACE::comparison_mask<float,16,Arch>;
    using mask = mask_type;
    using predicate_type = predicate<16,Arch>;
    __m512 value;
    /// Default initialization leaves storage unspecified; value initialization with braces zero-initializes it.
    native_inline simd() = default;
    /// Copy the stored value without arithmetic or normalization.
    native_inline constexpr simd(simd const &) = default;
    /// Copy the stored value and return *this; no numerical conversion is performed.
    native_reinitializes native_inline constexpr simd & operator=(simd const &) = default;
    /// Broadcast the supplied value to each logical lane.
    native_inline constexpr simd(float x) {
      if consteval { std::array<float,sizeof(native_type)/sizeof(float)> values{}; values.fill(x); value=__builtin_bit_cast(native_type,values); }
      else { value=_mm512_set1_ps(x); }
    }
    /// Adopt native lane storage without numerical conversion.
    native_inline native_target("avx512f") constexpr simd(__m512 x) : value(x) {}
    /// Read all logical lanes from an unaligned element pointer.
    native_nodiscard static native_inline constexpr native_pure simd load(native_noescape float const * p) { return ::NATIVE_BACKEND_NAMESPACE::simd_load_native<simd,1>(p); }
    /// Write all logical lanes to an unaligned element pointer.
    native_inline constexpr void store(native_noescape float * p) const { ::NATIVE_BACKEND_NAMESPACE::simd_store_native<simd,1>(p,*this); }
    /// Add corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd operator+(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::add,a,b); }
      return simd(_mm512_add_ps(a.value, b.value));
    }
    /// Subtract corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd operator-(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::subtract,a,b); }
      return simd(_mm512_sub_ps(a.value, b.value));
    }
    /// Multiply corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd operator*(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::multiply,a,b); }
      return simd(_mm512_mul_ps(a.value, b.value));
    }
    /// Divide corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd operator/(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::divide,a,b); }
      return simd(_mm512_div_ps(a.value, b.value));
    }
    /// Negate every logical lane; floating-point lanes change sign.
    native_nodiscard friend native_inline constexpr native_const simd operator-(simd a) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::negate,a); }
      return simd(_mm512_xor_ps(a.value, _mm512_set1_ps(-0.f)));
    }
    /// Return a mask whose lanes are true where `a < b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator<(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::compare(::native::detail::float_constant::less,a,b); }
      return mask_type::from_native(_mm512_cmp_ps_mask(a.value,b.value,_CMP_LT_OQ));
    }
    /// Return a mask whose lanes are true where `a > b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator>(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::compare(::native::detail::float_constant::less,b,a); }
      return mask_type::from_native(_mm512_cmp_ps_mask(a.value,b.value,_CMP_GT_OQ));
    }
    /// Return a mask whose lanes are true where `a == b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator==(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::compare(::native::detail::float_constant::equal,a,b); }
      return mask_type::from_native(_mm512_cmp_ps_mask(a.value,b.value,_CMP_EQ_OQ));
    }
    /// Choose a in true lanes and b in false lanes; both values are already evaluated.
    template<class M> requires (std::same_as<M,mask_type> || std::same_as<M,vector_mask_type>)
    native_nodiscard friend native_inline constexpr native_const simd select(M m,simd a,simd b) {
      if consteval { return ::native::detail::float_constant::select(m,a,b); }
      if constexpr(M::compact) return simd(_mm512_mask_blend_ps(m.to_native(),b.value,a.value));
      else return simd(_mm512_castsi512_ps(_mm512_or_si512(_mm512_and_si512(m.to_native(),_mm512_castps_si512(a.value)),
        _mm512_andnot_si512(m.to_native(),_mm512_castps_si512(b.value))))); }
    /// Compute a*b+c with one fused rounding per lane.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd fma(simd a, simd b, simd c) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::multiply_add,a,b,c); }
      return simd(_mm512_fmadd_ps(a.value, b.value, c.value));
    }
    /// Take the native square root of each lane.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd sqrt(simd a) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::square_root,a); }
      return simd(_mm512_sqrt_ps(a.value));
    }
    /// Round each lane to nearest integral value, choosing even at a tie.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd round_even(simd a) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::nearest,a); }
      return simd(_mm512_roundscale_ps(a.value, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC));
    }
    /// Construct normal powers of two; require integral exponents in [-126,127].
    native_nodiscard friend native_inline constexpr native_const simd normal_pow2(simd n) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::power_of_two,n); }
      return simd(_mm512_castsi512_ps(_mm512_slli_epi32(_mm512_add_epi32(_mm512_cvttps_epi32(n.value), _mm512_set1_epi32(127)), 23)));
    }

    template <std::size_t Alignment = 1>
    /// Load logical lanes, assuming the template alignment in bytes.
    native_nodiscard static native_inline constexpr native_pure simd load_memory(float const * p) noexcept {
      return ::NATIVE_BACKEND_NAMESPACE::simd_load_native<simd,Alignment>(p);
    }
    template <std::size_t Alignment = 1>
    /// Store logical lanes, assuming the template alignment in bytes.
    native_inline constexpr void store_memory(float * p) const noexcept {
      ::NATIVE_BACKEND_NAMESPACE::simd_store_native<simd,Alignment>(p,*this);
    }
    using value_type = float;
    using register_type = simd;
    using native_type = __m512;
    using bits_type = simd<uint32_t,16,Arch>;
    /// Return the native storage value without a numerical conversion.
    native_nodiscard native_inline constexpr native_pure native_target("avx512f") operator native_type() const noexcept { return value; }
    /// Return the native storage representation.
    native_nodiscard native_inline constexpr native_pure native_target("avx512f") native_type to_native() const noexcept { return value; }
    /// Project the exact lane representation into the corresponding unsigned vector.
    native_artificial native_nodiscard native_inline constexpr native_pure bits_type bits() const noexcept { if consteval { return bits_type::from_native(__builtin_bit_cast(typename bits_type::native_type,value)); } return bits_type::from_native(_mm512_castps_si512(value)); }
    /// Return the exact binary32 lane representations in the unsigned vector.
    native_nodiscard native_inline constexpr native_pure bits_type to_bits() const noexcept { return bits(); }
    /// Reinterpret binary words as lane values; do not normalize them.
    native_artificial native_nodiscard static native_inline constexpr native_const simd from_bits(bits_type bits) noexcept { if consteval { return simd(__builtin_bit_cast(native_type,bits.to_native())); } return simd(_mm512_castsi512_ps(bits.value)); }
    /// Reinterpret binary words as lane values; do not normalize them.
    native_nodiscard static native_inline constexpr native_const simd from_bits(std::uint32_t bits) noexcept { return from_bits(bits_type(bits)); }
    /// Broadcast the float value without adding an FTZ or other normalization policy.
    native_nodiscard static native_inline constexpr native_const simd from_float(float x) noexcept { return simd(x); }
    /// Adopt native storage without numerical conversion.
    native_nodiscard static native_inline constexpr native_const native_target("avx512f") simd from_native(native_type x) noexcept { return simd(x); }
    /// Adopt raw float storage without numerical conversion or normalization.
    native_nodiscard static native_inline constexpr native_const native_target("avx512f") simd unsafe_from_float32(native_type x) noexcept { return simd(x); }
    /// Read all logical lanes without an extra alignment promise.
    native_nodiscard static native_inline constexpr native_pure simd loadu(native_noescape float const * p) { return ::NATIVE_BACKEND_NAMESPACE::simd_load_native<simd,1>(p); }
    /// Write all logical lanes without an extra alignment promise.
    native_inline constexpr void storeu(native_noescape float * p) const { ::NATIVE_BACKEND_NAMESPACE::simd_store_native<simd,1>(p,*this); }
    /// Load exactly the logical count of binary32 words without normalizing their representations.
    native_nodiscard static native_inline constexpr native_pure simd load_bits(native_noescape std::uint32_t const * p) noexcept { return from_bits(bits_type::load(p)); }
    /// Store the exact binary32 words for every logical lane.
    native_inline constexpr void store_bits(native_noescape std::uint32_t * p) const noexcept { bits().store(p); }
    /// Read n representation words and fill the remaining logical lanes; require n <= lanes. A zero count permits null.
    native_nodiscard static native_inline constexpr native_pure simd load_bits_partial(native_noescape std::uint32_t const * p,std::size_t n,std::uint32_t fill=0) noexcept native_diagnose_if(n > simd::lanes,"partial SIMD count exceeds the lane count") { return from_bits(bits_type::load_partial(p,n,fill)); }
    /// Write n exact representation words; require n <= lanes. A zero count permits null.
    native_inline constexpr void store_bits_partial(native_noescape std::uint32_t * p,std::size_t n) const noexcept native_diagnose_if(n > simd::lanes,"partial SIMD count exceeds the lane count") { bits().store_partial(p,n); }
    /// Read all logical lanes without an extra alignment promise.
    native_inline constexpr simd(std::array<float,16> const & values) noexcept : simd(loadu(values.data())) {}
#if defined(__clang__)
    /// Construct logical lanes in argument order. Any element conversions determine the exception specification.
    template <class... X> requires (sizeof...(X)==16) && (std::convertible_to<X,float> && ...)
    native_inline constexpr simd(X... x) noexcept((noexcept(static_cast<float>(x)) && ...)) : value{static_cast<float>(x)...} {}
#else
    /// Construct logical lanes in argument order. Any element conversions determine the exception specification.
    template <class... X> requires (sizeof...(X)==16) && (std::convertible_to<X,float> && ...)
    native_inline constexpr simd(X... x) noexcept((noexcept(static_cast<float>(x)) && ...)) : simd(loadu(std::array<float,16>{static_cast<float>(x)...}.data())) {}
#endif
    /// Apply the corresponding lane-wise add operation in place and return *this.
    native_inline constexpr simd & operator+=(simd b) noexcept { return *this=*this+b; }
    /// Apply the corresponding lane-wise subtract operation in place and return *this.
    native_inline constexpr simd & operator-=(simd b) noexcept { return *this=*this-b; }
    /// Apply the corresponding lane-wise multiply operation in place and return *this.
    native_inline constexpr simd & operator*=(simd b) noexcept { return *this=*this*b; }
    /// Apply the corresponding lane-wise divide operation in place and return *this.
    native_inline constexpr simd & operator/=(simd b) noexcept { return *this=*this/b; }
    /// Return a mask whose lanes are true where `a != b` holds. NaN lanes compare unequal.
    native_nodiscard friend native_inline constexpr native_const mask_type operator!=(simd a,simd b) noexcept { return ~(a==b); }
    /// Return a mask whose lanes are true where `a <= b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator<=(simd a,simd b) noexcept { return (a<b)|(a==b); }
    /// Return a mask whose lanes are true where `a >= b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator>=(simd a,simd b) noexcept { return (a>b)|(a==b); }
  };
#endif
#if NATIVE_HAS_ARM_NEON
  template <::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) struct alignas(float32x4_t) native_empty_bases simd<float, 4,Arch> : ::NATIVE_BACKEND_NAMESPACE::register_memory<simd<float,4,Arch>, 4>, detail::swizzle_access<float,4,Arch> {
    static constexpr isa<> architecture=Arch;
    template <class T> using rebind = simd<T,4,Arch>;
    using vector_mask_type=simd<mask32,4,Arch>;
    using mask_type=::NATIVE_BACKEND_NAMESPACE::comparison_mask<float,4,Arch>;
    using mask = mask_type;
    using predicate_type = predicate<4,Arch>;
    float32x4_t value;
    /// Default initialization leaves storage unspecified; value initialization with braces zero-initializes it.
    native_inline simd() = default;
    /// Copy the stored value without arithmetic or normalization.
    native_inline constexpr simd(simd const &) = default;
    /// Copy the stored value and return *this; no numerical conversion is performed.
    native_reinitializes native_inline constexpr simd & operator=(simd const &) = default;
    /// Broadcast the supplied value to each logical lane.
    native_inline constexpr simd(float x) {
      if consteval { std::array<float,sizeof(native_type)/sizeof(float)> values{}; values.fill(x); value=__builtin_bit_cast(native_type,values); }
      else { value=vdupq_n_f32(x); }
    }
    /// Adopt native lane storage without numerical conversion.
    native_inline constexpr simd(float32x4_t x) : value(x) {}
    /// Read all logical lanes from an unaligned element pointer.
    native_nodiscard static native_inline constexpr native_pure simd load(native_noescape float const * p) { return ::NATIVE_BACKEND_NAMESPACE::simd_load_native<simd,1>(p); }
    /// Write all logical lanes to an unaligned element pointer.
    native_inline constexpr void store(native_noescape float * p) const { ::NATIVE_BACKEND_NAMESPACE::simd_store_native<simd,1>(p,*this); }
    /// Add corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd operator+(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::add,a,b); }
      return simd(vaddq_f32(a.value, b.value));
    }
    /// Subtract corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd operator-(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::subtract,a,b); }
      return simd(vsubq_f32(a.value, b.value));
    }
    /// Multiply corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd operator*(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::multiply,a,b); }
      return simd(vmulq_f32(a.value, b.value));
    }
    /// Divide corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd operator/(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::divide,a,b); }
      return simd(vdivq_f32(a.value, b.value));
    }
    /// Negate every logical lane; floating-point lanes change sign.
    native_artificial native_nodiscard friend native_inline constexpr native_const simd operator-(simd a) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::negate,a); }
      return simd(vnegq_f32(a.value));
    }
    /// Return a mask whose lanes are true where `a < b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator<(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::compare(::native::detail::float_constant::less,a,b); }
      return mask_type::unsafe_from_native(vreinterpretq_u8_u32(vcltq_f32(a.value,b.value)));
    }
    /// Return a mask whose lanes are true where `a > b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator>(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::compare(::native::detail::float_constant::less,b,a); }
      return mask_type::unsafe_from_native(vreinterpretq_u8_u32(vcgtq_f32(a.value,b.value)));
    }
    /// Return a mask whose lanes are true where `a == b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator==(simd a, simd b) {
      if consteval { return ::native::detail::float_constant::compare(::native::detail::float_constant::equal,a,b); }
      return mask_type::unsafe_from_native(vreinterpretq_u8_u32(vceqq_f32(a.value,b.value)));
    }
    /// Choose a in true mask lanes and b in false lanes; both values are already evaluated.
    template<class M> requires (std::same_as<M,mask_type> || std::same_as<M,vector_mask_type>)
    native_nodiscard friend native_inline constexpr native_const simd select(M m,simd a,simd b) {
      if consteval { return ::native::detail::float_constant::select(m,a,b); }
      return simd(vbslq_f32(vreinterpretq_u32_u8(m.to_native()),a.value,b.value));
    }
    /// Compute a*b+c with one fused rounding per lane.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd fma(simd a, simd b, simd c) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::multiply_add,a,b,c); }
      return simd(vfmaq_f32(c.value, a.value, b.value));
    }
    /// Take the native square root of each lane.
    native_artificial native_nodiscard friend native_inline constexpr native_pure simd sqrt(simd a) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::square_root,a); }
      return simd(vsqrtq_f32(a.value));
    }
    /// Round each lane to nearest integral value, choosing even at a tie.
    native_artificial native_nodiscard friend native_inline constexpr native_const simd round_even(simd a) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::nearest,a); }
      return simd(vrndnq_f32(a.value));
    }
    /// Construct normal powers of two; require integral exponents in [-126,127].
    native_nodiscard friend native_inline constexpr native_const simd normal_pow2(simd n) {
      if consteval { return ::native::detail::float_constant::map(::native::detail::float_constant::power_of_two,n); }
      return simd(vreinterpretq_f32_s32(vshlq_n_s32(vaddq_s32(vcvtq_s32_f32(n.value), vdupq_n_s32(127)), 23)));
    }

    template <std::size_t Alignment = 1>
    /// Load logical lanes, assuming the template alignment in bytes.
    native_nodiscard static native_inline constexpr native_pure simd load_memory(float const * p) noexcept {
      return ::NATIVE_BACKEND_NAMESPACE::simd_load_native<simd,Alignment>(p);
    }
    template <std::size_t Alignment = 1>
    /// Store logical lanes, assuming the template alignment in bytes.
    native_inline constexpr void store_memory(float * p) const noexcept {
      ::NATIVE_BACKEND_NAMESPACE::simd_store_native<simd,Alignment>(p,*this);
    }
    using value_type = float;
    using register_type = simd;
    using native_type = float32x4_t;
    using bits_type = simd<uint32_t,4,Arch>;
    /// Return the native storage value without a numerical conversion.
    native_nodiscard native_inline constexpr native_pure operator native_type() const noexcept { return value; }
    /// Return the native storage representation.
    native_nodiscard native_inline constexpr native_pure native_type to_native() const noexcept { return value; }
    /// Project the exact lane representation into the corresponding unsigned vector.
    native_artificial native_nodiscard native_inline constexpr native_pure bits_type bits() const noexcept { if consteval { return bits_type::from_native(__builtin_bit_cast(typename bits_type::native_type,value)); } return bits_type::from_native(vreinterpretq_u8_f32(value)); }
    /// Return the exact binary32 lane representations in the unsigned vector.
    native_nodiscard native_inline constexpr native_pure bits_type to_bits() const noexcept { return bits(); }
    /// Reinterpret binary words as lane values; do not normalize them.
    native_artificial native_nodiscard static native_inline constexpr native_const simd from_bits(bits_type bits) noexcept { if consteval { return simd(__builtin_bit_cast(native_type,bits.to_native())); } return simd(vreinterpretq_f32_u8(bits.value)); }
    /// Reinterpret binary words as lane values; do not normalize them.
    native_nodiscard static native_inline constexpr native_const simd from_bits(std::uint32_t bits) noexcept { return from_bits(bits_type(bits)); }
    /// Broadcast the float value without adding an FTZ or other normalization policy.
    native_nodiscard static native_inline constexpr native_const simd from_float(float x) noexcept { return simd(x); }
    /// Adopt native storage without numerical conversion.
    native_nodiscard static native_inline constexpr native_const simd from_native(native_type x) noexcept { return simd(x); }
    /// Adopt raw float storage without numerical conversion or normalization.
    native_nodiscard static native_inline constexpr native_const simd unsafe_from_float32(native_type x) noexcept { return simd(x); }
    /// Read all logical lanes without an extra alignment promise.
    native_nodiscard static native_inline constexpr native_pure simd loadu(native_noescape float const * p) { return ::NATIVE_BACKEND_NAMESPACE::simd_load_native<simd,1>(p); }
    /// Write all logical lanes without an extra alignment promise.
    native_inline constexpr void storeu(native_noescape float * p) const { ::NATIVE_BACKEND_NAMESPACE::simd_store_native<simd,1>(p,*this); }
    /// Load exactly the logical count of binary32 words without normalizing their representations.
    native_nodiscard static native_inline constexpr native_pure simd load_bits(native_noescape std::uint32_t const * p) noexcept { return from_bits(bits_type::load(p)); }
    /// Store the exact binary32 words for every logical lane.
    native_inline constexpr void store_bits(native_noescape std::uint32_t * p) const noexcept { bits().store(p); }
    /// Read n representation words and fill the remaining logical lanes; require n <= lanes. A zero count permits null.
    native_nodiscard static native_inline constexpr native_pure simd load_bits_partial(native_noescape std::uint32_t const * p,std::size_t n,std::uint32_t fill=0) noexcept native_diagnose_if(n > simd::lanes,"partial SIMD count exceeds the lane count") { return from_bits(bits_type::load_partial(p,n,fill)); }
    /// Write n exact representation words; require n <= lanes. A zero count permits null.
    native_inline constexpr void store_bits_partial(native_noescape std::uint32_t * p,std::size_t n) const noexcept native_diagnose_if(n > simd::lanes,"partial SIMD count exceeds the lane count") { bits().store_partial(p,n); }
    /// Read all logical lanes without an extra alignment promise.
    native_inline constexpr simd(std::array<float,4> const & values) noexcept : simd(loadu(values.data())) {}
#if defined(__clang__)
    /// Construct logical lanes in argument order. Any element conversions determine the exception specification.
    template <class... X> requires (sizeof...(X)==4) && (std::convertible_to<X,float> && ...)
    native_inline constexpr simd(X... x) noexcept((noexcept(static_cast<float>(x)) && ...)) : value{static_cast<float>(x)...} {}
#else
    template <class... X> requires (sizeof...(X)==4) && (std::convertible_to<X,float> && ...)
    /// Read all logical lanes without an extra alignment promise.
    native_inline constexpr simd(X... x) noexcept((noexcept(static_cast<float>(x)) && ...)) : simd(loadu(std::array<float,4>{static_cast<float>(x)...}.data())) {}
#endif
    /// Apply the corresponding lane-wise add operation in place and return *this.
    native_inline constexpr simd & operator+=(simd b) noexcept { return *this=*this+b; }
    /// Apply the corresponding lane-wise subtract operation in place and return *this.
    native_inline constexpr simd & operator-=(simd b) noexcept { return *this=*this-b; }
    /// Apply the corresponding lane-wise multiply operation in place and return *this.
    native_inline constexpr simd & operator*=(simd b) noexcept { return *this=*this*b; }
    /// Apply the corresponding lane-wise divide operation in place and return *this.
    native_inline constexpr simd & operator/=(simd b) noexcept { return *this=*this/b; }
    /// Return a mask whose lanes are true where `a != b` holds. NaN lanes compare unequal.
    native_nodiscard friend native_inline constexpr native_const mask_type operator!=(simd a,simd b) noexcept { return ~(a==b); }
    /// Return a mask whose lanes are true where `a <= b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator<=(simd a,simd b) noexcept { return (a<b)|(a==b); }
    /// Return a mask whose lanes are true where `a >= b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline constexpr native_const mask_type operator>=(simd a,simd b) noexcept { return (a>b)|(a==b); }
  };
#endif
  namespace detail::NATIVE_BACKEND {
    // VSCALEF uses floor, including negative fractions. Native rounding also
    // observes the caller's denormal-input mode before this operation.
    template <std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch)
    native_nodiscard native_inline native_pure simd<float,N,Arch> scaleb_floor(simd<float,N,Arch> x) noexcept {
#if NATIVE_HAS_AVX2
      if constexpr(N==1) return simd<float,N,Arch>(_mm_cvtss_f32(_mm_floor_ss(_mm_setzero_ps(),_mm_set_ss(x.value))));
      else if constexpr(N==4) return simd<float,N,Arch>(_mm_floor_ps(x.value));
      else if constexpr(N==8) return simd<float,N,Arch>(_mm256_floor_ps(x.value));
      else
#endif
#if NATIVE_HAS_ARM_NEON
      if constexpr(N==4) return simd<float,N,Arch>(__builtin_elementwise_floor(x.value));
      else
#endif
      return simd<float,N,Arch>(std::floor(x.value));
    }
    // Cold nonfinite-exponent table from Intel VSCALEFPD/SD/PS/SS. In particular,
    // quiet NaN scaled by +inf/-inf is +inf/+0, but signaling NaN stays NaN.
    // NaN payload/sign and matching active-lane exception flags are not promised.
    native_nodiscard native_inline native_pure float scaleb_special(float x,float exponent) noexcept {
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
    template <std::size_t N,class M, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch)
    native_nodiscard native_inline native_pure simd<float,N,Arch> scaleb_fallback(M mask,
        simd<float,N,Arch> prior,simd<float,N,Arch> value,simd<float,N,Arch> exponent) noexcept {
      using V=simd<float,N,Arch>;using U=simd<uint32_t,N,Arch>;
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
  template <std::size_t N,class M, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> &&(std::same_as<M,typename simd<float,N,Arch>::mask_type> ||
             std::same_as<M,typename simd<float,N,Arch>::vector_mask_type>)
  native_nodiscard native_inline constexpr native_pure simd<float,N,Arch> masked_scaleb(M mask,
      simd<float,N,Arch> prior,simd<float,N,Arch> value,simd<float,N,Arch> exponent) noexcept {
    if consteval {
      auto a=::native::detail::float_constant::words(value),b=::native::detail::float_constant::words(exponent);
      auto result=::native::detail::float_constant::words(prior);
      auto active=mask.to_bitset();
      for(std::size_t i=0;i<N;++i) if((active>>i)&1) result[i]=::native::detail::float_constant::scale(a[i],b[i]);
      return simd<float,N,Arch>::load_bits(result.data());
    }
    if constexpr(N==2 || N==3) {
      using V=simd<float,N,Arch>;
      using M4=typename V::storage_type::mask_type;
      return V::from_storage(masked_scaleb(M4::from_bitset(mask.to_bitset()),prior.to_storage(),value.to_storage(),exponent.to_storage()));
    } else {
#if NATIVE_HAS_AVX512F
      auto native_mask=[&] {if constexpr(M::compact) return mask.to_native();else return to_predicate(mask).to_native();};
      if constexpr(N==1) return simd<float,N,Arch>(_mm_cvtss_f32(_mm_mask_scalef_ss(
        _mm_set_ss(prior.value),__mmask8(native_mask()),_mm_set_ss(value.value),_mm_set_ss(exponent.value))));
      else if constexpr(N==16) return simd<float,N,Arch>(_mm512_mask_scalef_ps(prior.value,native_mask(),value.value,exponent.value));
#if NATIVE_HAS_AVX512VL
      else if constexpr(N==4) return simd<float,N,Arch>(_mm_mask_scalef_ps(prior.value,native_mask(),value.value,exponent.value));
      else if constexpr(N==8) return simd<float,N,Arch>(_mm256_mask_scalef_ps(prior.value,native_mask(),value.value,exponent.value));
#endif
      else
#endif
      return ::NATIVE_BACKEND_NAMESPACE::scaleb_fallback(mask,prior,value,exponent);
    }
  }
  /// \ingroup vector_math
  /// Scale active lanes by 2^floor(exponent), writing positive zero elsewhere.
  template <std::size_t N,class M, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> &&(std::same_as<M,typename simd<float,N,Arch>::mask_type> ||
             std::same_as<M,typename simd<float,N,Arch>::vector_mask_type>)
  native_nodiscard native_inline constexpr native_pure simd<float,N,Arch> masked_scaleb_zero(M mask,
      simd<float,N,Arch> value,simd<float,N,Arch> exponent) noexcept {
    return masked_scaleb(mask,simd<float,N,Arch>(0.f),value,exponent);
  }
  /// \ingroup vector_math
  /// Scale every lane by 2^floor(exponent), including fractional exponents.
  /// Unlike an integer ldexp exponent, the exponent argument is itself a vector.
  template <std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N>
  native_nodiscard native_inline constexpr native_pure simd<float,N,Arch> scaleb(simd<float,N,Arch> value,
      simd<float,N,Arch> exponent) noexcept {
    return masked_scaleb_zero(typename simd<float,N,Arch>::mask_type(true),value,exponent);
  }
  /// Add corresponding floating-point lanes using the caller's rounding and denormal environment.
  template <std::size_t N,class U, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && (!::NATIVE_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,simd<float,N,Arch>>) && std::convertible_to<U,simd<float,N,Arch>>
  native_nodiscard native_inline constexpr auto operator+(simd<float,N,Arch> a, U b) noexcept(noexcept(simd<float,N,Arch>(b))) { return a+simd<float,N,Arch>(b); }
  /// Add corresponding floating-point lanes using the caller's rounding and denormal environment.
  template <std::size_t N,class U, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && (!::NATIVE_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,simd<float,N,Arch>>) && std::convertible_to<U,simd<float,N,Arch>>
  native_nodiscard native_inline constexpr auto operator+(U a, simd<float,N,Arch> b) noexcept(noexcept(simd<float,N,Arch>(a))) { return simd<float,N,Arch>(a)+b; }
  /// Subtract corresponding floating-point lanes using the caller's rounding and denormal environment.
  template <std::size_t N,class U, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && (!::NATIVE_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,simd<float,N,Arch>>) && std::convertible_to<U,simd<float,N,Arch>>
  native_nodiscard native_inline constexpr auto operator-(simd<float,N,Arch> a, U b) noexcept(noexcept(simd<float,N,Arch>(b))) { return a-simd<float,N,Arch>(b); }
  /// Subtract corresponding floating-point lanes using the caller's rounding and denormal environment.
  template <std::size_t N,class U, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && (!::NATIVE_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,simd<float,N,Arch>>) && std::convertible_to<U,simd<float,N,Arch>>
  native_nodiscard native_inline constexpr auto operator-(U a, simd<float,N,Arch> b) noexcept(noexcept(simd<float,N,Arch>(a))) { return simd<float,N,Arch>(a)-b; }
  /// Multiply corresponding floating-point lanes using the caller's rounding and denormal environment.
  template <std::size_t N,class U, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && (!::NATIVE_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,simd<float,N,Arch>>) && std::convertible_to<U,simd<float,N,Arch>>
  native_nodiscard native_inline constexpr auto operator*(simd<float,N,Arch> a, U b) noexcept(noexcept(simd<float,N,Arch>(b))) { return a*simd<float,N,Arch>(b); }
  /// Multiply corresponding floating-point lanes using the caller's rounding and denormal environment.
  template <std::size_t N,class U, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && (!::NATIVE_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,simd<float,N,Arch>>) && std::convertible_to<U,simd<float,N,Arch>>
  native_nodiscard native_inline constexpr auto operator*(U a, simd<float,N,Arch> b) noexcept(noexcept(simd<float,N,Arch>(a))) { return simd<float,N,Arch>(a)*b; }
  /// Divide corresponding floating-point lanes using the caller's rounding and denormal environment.
  template <std::size_t N,class U, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && (!::NATIVE_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,simd<float,N,Arch>>) && std::convertible_to<U,simd<float,N,Arch>>
  native_nodiscard native_inline constexpr auto operator/(simd<float,N,Arch> a, U b) noexcept(noexcept(simd<float,N,Arch>(b))) { return a/simd<float,N,Arch>(b); }
  /// Divide corresponding floating-point lanes using the caller's rounding and denormal environment.
  template <std::size_t N,class U, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && (!::NATIVE_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,simd<float,N,Arch>>) && std::convertible_to<U,simd<float,N,Arch>>
  native_nodiscard native_inline constexpr auto operator/(U a, simd<float,N,Arch> b) noexcept(noexcept(simd<float,N,Arch>(a))) { return simd<float,N,Arch>(a)/b; }
  /// Return a mask whose lanes are true where `a < b` holds. NaN lanes yield false.
  template <std::size_t N,class U, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && (!::NATIVE_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,simd<float,N,Arch>>) && std::convertible_to<U,simd<float,N,Arch>>
  native_nodiscard native_inline constexpr auto operator<(simd<float,N,Arch> a, U b) noexcept(noexcept(simd<float,N,Arch>(b))) { return a<simd<float,N,Arch>(b); }
  /// Return a mask whose lanes are true where `a < b` holds. NaN lanes yield false.
  template <std::size_t N,class U, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && (!::NATIVE_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,simd<float,N,Arch>>) && std::convertible_to<U,simd<float,N,Arch>>
  native_nodiscard native_inline constexpr auto operator<(U a, simd<float,N,Arch> b) noexcept(noexcept(simd<float,N,Arch>(a))) { return simd<float,N,Arch>(a)<b; }
  /// Return a mask whose lanes are true where `a > b` holds. NaN lanes yield false.
  template <std::size_t N,class U, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && (!::NATIVE_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,simd<float,N,Arch>>) && std::convertible_to<U,simd<float,N,Arch>>
  native_nodiscard native_inline constexpr auto operator>(simd<float,N,Arch> a, U b) noexcept(noexcept(simd<float,N,Arch>(b))) { return a>simd<float,N,Arch>(b); }
  /// Return a mask whose lanes are true where `a > b` holds. NaN lanes yield false.
  template <std::size_t N,class U, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && (!::NATIVE_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,simd<float,N,Arch>>) && std::convertible_to<U,simd<float,N,Arch>>
  native_nodiscard native_inline constexpr auto operator>(U a, simd<float,N,Arch> b) noexcept(noexcept(simd<float,N,Arch>(a))) { return simd<float,N,Arch>(a)>b; }
  /// Return a mask whose lanes are true where `a <= b` holds. NaN lanes yield false.
  template <std::size_t N,class U, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && (!::NATIVE_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,simd<float,N,Arch>>) && std::convertible_to<U,simd<float,N,Arch>>
  native_nodiscard native_inline constexpr auto operator<=(simd<float,N,Arch> a, U b) noexcept(noexcept(simd<float,N,Arch>(b))) { return a<=simd<float,N,Arch>(b); }
  /// Return a mask whose lanes are true where `a <= b` holds. NaN lanes yield false.
  template <std::size_t N,class U, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && (!::NATIVE_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,simd<float,N,Arch>>) && std::convertible_to<U,simd<float,N,Arch>>
  native_nodiscard native_inline constexpr auto operator<=(U a, simd<float,N,Arch> b) noexcept(noexcept(simd<float,N,Arch>(a))) { return simd<float,N,Arch>(a)<=b; }
  /// Return a mask whose lanes are true where `a >= b` holds. NaN lanes yield false.
  template <std::size_t N,class U, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && (!::NATIVE_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,simd<float,N,Arch>>) && std::convertible_to<U,simd<float,N,Arch>>
  native_nodiscard native_inline constexpr auto operator>=(simd<float,N,Arch> a, U b) noexcept(noexcept(simd<float,N,Arch>(b))) { return a>=simd<float,N,Arch>(b); }
  /// Return a mask whose lanes are true where `a >= b` holds. NaN lanes yield false.
  template <std::size_t N,class U, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && (!::NATIVE_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,simd<float,N,Arch>>) && std::convertible_to<U,simd<float,N,Arch>>
  native_nodiscard native_inline constexpr auto operator>=(U a, simd<float,N,Arch> b) noexcept(noexcept(simd<float,N,Arch>(a))) { return simd<float,N,Arch>(a)>=b; }
  /// Return a mask whose lanes are true where `a == b` holds. NaN lanes yield false.
  template <std::size_t N,class U, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && (!::NATIVE_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,simd<float,N,Arch>>) && std::convertible_to<U,simd<float,N,Arch>>
  native_nodiscard native_inline constexpr auto operator==(simd<float,N,Arch> a, U b) noexcept(noexcept(simd<float,N,Arch>(b))) { return a==simd<float,N,Arch>(b); }
  /// Return a mask whose lanes are true where `a == b` holds. NaN lanes yield false.
  template <std::size_t N,class U, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && (!::NATIVE_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,simd<float,N,Arch>>) && std::convertible_to<U,simd<float,N,Arch>>
  native_nodiscard native_inline constexpr auto operator==(U a, simd<float,N,Arch> b) noexcept(noexcept(simd<float,N,Arch>(a))) { return simd<float,N,Arch>(a)==b; }
  /// Return a mask whose lanes are true where `a != b` holds. NaN lanes compare unequal.
  template <std::size_t N,class U, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && (!::NATIVE_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,simd<float,N,Arch>>) && std::convertible_to<U,simd<float,N,Arch>>
  native_nodiscard native_inline constexpr auto operator!=(simd<float,N,Arch> a, U b) noexcept(noexcept(simd<float,N,Arch>(b))) { return a!=simd<float,N,Arch>(b); }
  /// Return a mask whose lanes are true where `a != b` holds. NaN lanes compare unequal.
  template <std::size_t N,class U, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && (!::NATIVE_BACKEND_NAMESPACE::custom_argument<U>) && (!std::same_as<U,simd<float,N,Arch>>) && std::convertible_to<U,simd<float,N,Arch>>
  native_nodiscard native_inline constexpr auto operator!=(U a, simd<float,N,Arch> b) noexcept(noexcept(simd<float,N,Arch>(a))) { return simd<float,N,Arch>(a)!=b; }
  /// Compute a*b+c with one fused rounding per logical lane in the caller's floating-point environment.
  template <std::size_t N,class A,class B, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && (!::NATIVE_BACKEND_NAMESPACE::custom_argument<A> && !::NATIVE_BACKEND_NAMESPACE::custom_argument<B>) && std::convertible_to<A,simd<float,N,Arch>> && std::convertible_to<B,simd<float,N,Arch>>
  native_nodiscard native_inline constexpr auto fma(simd<float,N,Arch> a,A b,B c) noexcept(noexcept(simd<float,N,Arch>(b)) && noexcept(simd<float,N,Arch>(c))) { return fma(a,simd<float,N,Arch>(b),simd<float,N,Arch>(c)); }
  /// Compute a*b+c with one fused rounding per logical lane in the caller's floating-point environment.
  template <std::size_t N,class A,class B, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && (!::NATIVE_BACKEND_NAMESPACE::custom_argument<A> && !::NATIVE_BACKEND_NAMESPACE::custom_argument<B>) && (!std::same_as<A,simd<float,N,Arch>>) && std::convertible_to<A,simd<float,N,Arch>> && std::convertible_to<B,simd<float,N,Arch>>
  native_nodiscard native_inline constexpr auto fma(A a,simd<float,N,Arch> b,B c) noexcept(noexcept(simd<float,N,Arch>(a)) && noexcept(simd<float,N,Arch>(c))) { return fma(simd<float,N,Arch>(a),b,simd<float,N,Arch>(c)); }
  /// Compute a*b+c with one fused rounding per logical lane in the caller's floating-point environment.
  template <std::size_t N,class A,class B, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && (!::NATIVE_BACKEND_NAMESPACE::custom_argument<A> && !::NATIVE_BACKEND_NAMESPACE::custom_argument<B>) && (!std::same_as<A,simd<float,N,Arch>>) && (!std::same_as<B,simd<float,N,Arch>>) && std::convertible_to<A,simd<float,N,Arch>> && std::convertible_to<B,simd<float,N,Arch>>
  native_nodiscard native_inline constexpr auto fma(A a,B b,simd<float,N,Arch> c) noexcept(noexcept(simd<float,N,Arch>(a)) && noexcept(simd<float,N,Arch>(b))) { return fma(simd<float,N,Arch>(a),simd<float,N,Arch>(b),c); }
  /// \ingroup vector_math
  /// Clear each binary32 sign bit, preserving the remaining payload bits.
  template <std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N>
  native_nodiscard native_inline constexpr native_const auto abs(simd<float,N,Arch> a) noexcept {
    return simd<float,N,Arch>::from_bits(a.bits() & typename simd<float,N,Arch>::bits_type(0x7fffffffu));
  }

  // Truncating float-to-integer conversion requires a representable result.
  /// \ingroup vector_math
  /// Convert floats to signed 32-bit integers by truncation toward zero.
  /// \pre Every truncated result is representable; NaNs and infinities are outside the contract.
  template <class To, std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && std::same_as<To,std::int32_t>
  native_nodiscard native_inline constexpr simd<To,N,Arch> convert(simd<float,N,Arch> x) noexcept {
    if consteval {
      std::array<float,N> a{}; std::array<To,N> b{}; x.store(a.data());
      for(std::size_t i=0;i<N;++i) b[i]=static_cast<To>(a[i]);
      return simd<To,N,Arch>::load(b.data());
    }
    if constexpr(N==2 || N==3) return simd<To,N,Arch>::from_storage(convert<To>(x.to_storage()));
    else if constexpr (N==1) return simd<To,N,Arch>(static_cast<std::int32_t>(x.value));
#if NATIVE_HAS_AVX2
    else if constexpr (N==4) return simd<To,N,Arch>::from_native(_mm_cvttps_epi32(x.value));
    else if constexpr (N==8) return simd<To,N,Arch>::from_native(_mm256_cvttps_epi32(x.value));
#endif
#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ
    else if constexpr (N==16) return simd<To,N,Arch>::from_native(_mm512_cvttps_epi32(x.value));
#endif
#if NATIVE_HAS_ARM_NEON
    else if constexpr (N==4) return simd<To,N,Arch>::from_native(vreinterpretq_u8_s32(vcvtq_s32_f32(x.value)));
#endif
  }
  /// \ingroup vector_math
  /// Numerically convert signed 32-bit integers to binary32.
  /// This is not a bit cast. Values outside binary32's exact integer range round.
  template <class To, std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && std::same_as<To,float>
  native_nodiscard native_inline constexpr simd<To,N,Arch> convert(simd<std::int32_t,N,Arch> x) noexcept {
    if consteval {
      std::array<std::int32_t,N> a{}; std::array<To,N> b{}; x.store(a.data());
      for(std::size_t i=0;i<N;++i) b[i]=static_cast<To>(a[i]);
      return simd<To,N,Arch>::load(b.data());
    }
    if constexpr(N==2 || N==3) return simd<To,N,Arch>::from_storage(convert<To>(x.to_storage()));
    else if constexpr (N==1) return simd<To,N,Arch>(static_cast<float>(x.value));
#if NATIVE_HAS_AVX2
    else if constexpr (N==4) return simd<To,N,Arch>(_mm_cvtepi32_ps(x.value));
    else if constexpr (N==8) return simd<To,N,Arch>(_mm256_cvtepi32_ps(x.value));
#endif
#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ
    else if constexpr (N==16) return simd<To,N,Arch>(_mm512_cvtepi32_ps(x.value));
#endif
#if NATIVE_HAS_ARM_NEON
    else if constexpr (N==4) return simd<To,N,Arch>(vcvtq_f32_s32(vreinterpretq_s32_u8(x.value)));
#endif
  }

  namespace detail::NATIVE_BACKEND {
    template <class V,std::size_t Alignment> native_nodiscard native_artificial native_inline constexpr native_pure V simd_load_native(native_noescape float const * p) noexcept {
      if consteval {
        std::array<float,V::lanes> values{};
        for (std::size_t i=0;i<V::lanes;++i) values[i]=p[i];
        return V::from_native(__builtin_bit_cast(typename V::native_type,values));
      }
      if constexpr(V::lanes==1) return V(*p);
#if NATIVE_HAS_AVX2
      else if constexpr(V::lanes==4) { if constexpr(Alignment>=16)return V(_mm_load_ps(p));else return V(_mm_loadu_ps(p)); }
      else if constexpr(V::lanes==8) { if constexpr(Alignment>=32)return V(_mm256_load_ps(p));else return V(_mm256_loadu_ps(p)); }
#endif
#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ
      else if constexpr(V::lanes==16) { if constexpr(Alignment>=64)return V(_mm512_load_ps(p));else return V(_mm512_loadu_ps(p)); }
#endif
#if NATIVE_HAS_ARM_NEON
      else if constexpr(V::lanes==4)return V(vld1q_f32(p));
#endif
    }
    template <class V,std::size_t Alignment> native_artificial native_inline constexpr void simd_store_native(native_noescape float * p,V value) noexcept {
      if consteval {
        auto values=__builtin_bit_cast(std::array<float,V::lanes>,value.to_native());
        for (std::size_t i=0;i<V::lanes;++i) p[i]=values[i];
        return;
      }
      if constexpr(V::lanes==1)*p=value.value;
#if NATIVE_HAS_AVX2
      else if constexpr(V::lanes==4) {if constexpr(Alignment>=16)_mm_store_ps(p,value.value);else _mm_storeu_ps(p,value.value);}
      else if constexpr(V::lanes==8) {if constexpr(Alignment>=32)_mm256_store_ps(p,value.value);else _mm256_storeu_ps(p,value.value);}
#endif
#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ
      else if constexpr(V::lanes==16) {if constexpr(Alignment>=64)_mm512_store_ps(p,value.value);else _mm512_storeu_ps(p,value.value);}
#endif
#if NATIVE_HAS_ARM_NEON
      else if constexpr(V::lanes==4)vst1q_f32(p,value.value);
#endif
    }
  }
  namespace detail::NATIVE_BACKEND {
  // Alignment is a caller promise. Access is a hint; streaming currently falls
  // back to ordinary moves on every backend and needs no completion fence.
  template <::native::isa<> Arch, class T,std::size_t N,class U,std::size_t A=1,simd_access Access=simd_access::ordinary>
    requires NATIVE_ARCH_REQUIRES(Arch) && (std::same_as<U,float> || ::native::simd_custom_element<U>) &&
      (std::same_as<T,float> || ::native::simd_custom_element<T>)
  native_nodiscard native_inline constexpr native_pure simd<T,N,Arch> load_simd(native_noescape U const * p,simd_memory<A,Access> = {}) noexcept {
    if constexpr (::native::simd_custom_element<T>) return simd<T,N,Arch>::template load_memory<A>(p);
    else if constexpr (::native::simd_custom_element<U>) return simd<U,N,Arch>::template load_memory<A>(p).to_native();
    else return ::NATIVE_BACKEND_NAMESPACE::simd_load_native<simd<float,N,Arch>,A>(p);
  }
  template <class U,class T,std::size_t N,std::size_t A=1,simd_access Access=simd_access::ordinary, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && (std::same_as<U,float> || ::native::simd_custom_element<U>) &&
      (std::same_as<T,float> || ::native::simd_custom_element<T>)
  native_inline constexpr void store_simd(native_noescape U * p,simd<T,N,Arch> value,simd_memory<A,Access> = {}) noexcept {
    if constexpr (::native::simd_custom_element<U>) simd<U,N,Arch>(value).template store_memory<A>(p);
    /// Return the native storage representation.
    else if constexpr (::native::simd_custom_element<T>) value.to_native().template store_memory<A>(p);
    else ::NATIVE_BACKEND_NAMESPACE::simd_store_native<simd<float,N,Arch>,A>(p,value);
  }
  template <::native::isa<> Arch, class T,std::size_t N,class U,std::size_t A=1,simd_access Access=simd_access::ordinary>
    requires NATIVE_ARCH_REQUIRES(Arch) && (std::same_as<U,float> || ::native::simd_custom_element<U>) &&
      (std::same_as<T,float> || ::native::simd_custom_element<T>)
  native_nodiscard native_inline constexpr native_pure simd<T,N,Arch> load_simd_partial(native_noescape U const * p,std::size_t count,
      T fill=T{},simd_memory<A,Access> = {}) noexcept native_diagnose_if(count > N,"partial SIMD count exceeds the lane count") {
    std::array<U,N> temporary;temporary.fill(U(fill));
    for(std::size_t i=0;i<count;++i)temporary[i]=p[i];
    return ::NATIVE_BACKEND_NAMESPACE::load_simd<Arch,T,N>(temporary.data());
  }
  template <class U,class T,std::size_t N,std::size_t A=1,simd_access Access=simd_access::ordinary, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && (std::same_as<U,float> || ::native::simd_custom_element<U>) &&
      (std::same_as<T,float> || ::native::simd_custom_element<T>)
  native_inline constexpr void store_simd_partial(native_noescape U * p,simd<T,N,Arch> value,std::size_t count,simd_memory<A,Access> = {}) noexcept native_diagnose_if(count > N,"partial SIMD count exceeds the lane count") {
    std::array<U,N> temporary;store_simd(temporary.data(),value);
    for(std::size_t i=0;i<count;++i)p[i]=temporary[i];
  }
  template <::native::isa<> Arch,class T,std::size_t N> requires NATIVE_ARCH_REQUIRES(Arch)
  native_nodiscard native_inline constexpr native_pure auto load_simd(std::array<T,N> const & values) noexcept { return ::NATIVE_BACKEND_NAMESPACE::load_simd<Arch,T,N>(values.data()); }
  template <::native::isa<> Arch,class T,std::size_t N> requires NATIVE_ARCH_REQUIRES(Arch) && (N!=std::dynamic_extent)
  native_nodiscard native_inline constexpr native_pure auto load_simd(std::span<T,N> values) noexcept { return load_simd<Arch,std::remove_cv_t<T>,N>(values.data()); }
  }

}
namespace NATIVE_BACKEND_NAMESPACE::native {
  template <class V,std::size_t N> using register_memory = ::NATIVE_BACKEND_NAMESPACE::register_memory<V,N>;
  using fp32x1 = ::native::simd<float,1,NATIVE_DEFAULT_ARCH>;
#if NATIVE_HAS_AVX2 || NATIVE_HAS_ARM_NEON
  using fp32x4 = ::native::simd<float,4,NATIVE_DEFAULT_ARCH>;
#endif
#if NATIVE_HAS_AVX2
  using fp32x8 = ::native::simd<float,8,NATIVE_DEFAULT_ARCH>;
#endif
#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ
  using fp32x16 = ::native::simd<float,16,NATIVE_DEFAULT_ARCH>;
#endif
  template<class V> concept float_register = requires { V::lanes; V::architecture; typename V::value_type; } &&
    std::same_as<typename V::value_type,float> && ::NATIVE_BACKEND_NAMESPACE::float_shape<V::lanes> && (::native::abi_lookup<V::architecture,::native::detail::raw_kernel_policies>::index == NATIVE_RAW_TARGET);
  // Ordered comparison: second operand wins on equality or unordered, including
  // signed zero and NaNs, identically on each architecture.
  template<float_register V> native_nodiscard native_inline constexpr native_const V min(V a, V b) { return select(a < b, a, b); }
  template<float_register V> native_nodiscard native_inline constexpr native_const V max(V a, V b) { return select(a > b, a, b); }
  template<float_register V> native_nodiscard native_inline native_pure float reduce_add(V v) {
    std::array<float, V::lanes> a; v.storeu(a.data()); float sum = 0;
    for (float x : a) sum += x; // Increasing lane order, FP32.
    return sum;
  }
  template<float_register V> native_nodiscard native_inline V acos_scalar_lanes(V v) {
    std::array<float, V::lanes> a; v.storeu(a.data());
    for (float & x : a) x = std::acos(x);
    return V::loadu(a.data());
  }
  struct native_target {
#if NATIVE_HAS_AVX512F && NATIVE_HAS_AVX512DQ
    using float_type = fp32x16;
    static constexpr std::size_t preferred_registers = 6;
#elif NATIVE_HAS_AVX2
    using float_type = fp32x8;
    static constexpr std::size_t preferred_registers = 6;
#elif NATIVE_HAS_ARM_NEON
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



#if NATIVE_HAS_AVX2 || NATIVE_HAS_ARM_NEON
#if defined(__x86_64__) || defined(_M_X64)
#elif defined(__aarch64__) || defined(_M_ARM64)
#endif

namespace native {
  namespace detail::NATIVE_BACKEND {
    template<class T> concept short_element = std::same_as<T,float> ||
      std::same_as<T,std::int32_t> || std::same_as<T,std::uint32_t> || std::same_as<T,mask32>;
    template<class T> using short_lane = std::conditional_t<simd_mask_element<T>,std::uint32_t,T>;
    template<class T> using short_native = T __attribute__((ext_vector_type(4)));
    template<class T> constexpr short_lane<T> short_word(T value) noexcept {
      if constexpr(simd_mask_element<T>) return value.to_bits();
      else return value;
    }
  }

  // Logical short vectors use one physical four-lane register. Floating padding
  // stays zero; division supplies one in the unused denominator lanes.
  /// \ingroup vectors
  /// Two or three logical lanes in the corresponding four-lane native storage.
  /// Loads and stores touch only logical lanes; mask reductions ignore padding.
  /// Division supplies harmless inactive operands before using the full register.
  /// \snippet api.cc swizzles
  template<::NATIVE_BACKEND_NAMESPACE::short_element T,std::size_t N,::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) &&(N==2 || N==3) && requires { typename simd<T,4,Arch>::native_type; }
  // Make the register alignment explicit: MSVC's packed standard-library
  // aggregates can otherwise cap an ext_vector_type member's implicit alignment.
  struct alignas(typename simd<T,4,Arch>::native_type) simd<T,N,Arch> : detail::swizzle_access<T,N,Arch> {
    using value_type=T;
    static constexpr isa<> architecture=Arch;
    using storage_type=simd<T,4,Arch>;
    using native_type=::NATIVE_BACKEND_NAMESPACE::short_native<::NATIVE_BACKEND_NAMESPACE::short_lane<T>>;
    using register_type=simd;
    using unsigned_register_tag=void;
    using bits_type=simd<std::uint32_t,N,Arch>;
    using vector_mask_type=simd<mask32,N,Arch>;
    using mask_type=std::conditional_t<simd_mask_element<T>,simd,
      std::conditional_t<bool(NATIVE_HAS_AVX512VL),predicate<N,Arch>,vector_mask_type>>;
    using mask=mask_type;
    using predicate_type=predicate<N,Arch>;
    template<class U> using rebind=simd<U,N,Arch>;
    static constexpr std::size_t lanes=N;
    static constexpr std::size_t storage_lanes=4;
    static constexpr bool compact=false;
    static constexpr std::uint64_t lane_mask=(std::uint64_t(1)<<N)-1;
    native_type value;

    /// Default initialization leaves storage unspecified; value initialization with braces zero-initializes it.
    native_inline constexpr simd() = default;
    /// Copy the stored value without arithmetic or normalization.
    native_inline constexpr simd(simd const &) = default;
    /// Copy the stored value and return *this; no numerical conversion is performed.
    native_inline constexpr simd & operator=(simd const &) = default;
    /// Broadcast the supplied value to each logical lane. Unused physical lanes are zero.
    native_inline constexpr simd(T x) noexcept : value{::NATIVE_BACKEND_NAMESPACE::short_word(x),::NATIVE_BACKEND_NAMESPACE::short_word(x),
      N==3?::NATIVE_BACKEND_NAMESPACE::short_word(x):NATIVE_BACKEND_NAMESPACE::short_lane<T>(0),0} {}
    /// Broadcast the supplied value to each logical lane. Unused physical lanes are zero.
    explicit native_inline constexpr simd(bool x) noexcept requires simd_mask_element<T> : simd(T(x)) {}
    /// Adopt native lane storage and clear physical padding. Mask elements must already be canonical.
    native_inline constexpr simd(native_type x) noexcept
      : value(__builtin_shufflevector(x,native_type{},0,1,N==3?2:4,4)) {}
    /// Construct logical lanes in argument order. Any element conversions determine the exception specification.
    template<class... X> requires(sizeof...(X)==N) && (std::convertible_to<X,T> && ...)
    native_inline constexpr simd(X... x) noexcept((noexcept(static_cast<T>(x)) && ...))
      : value{::NATIVE_BACKEND_NAMESPACE::short_word(static_cast<T>(x))...} {}
    /// Copy one value per logical lane in array order.
    native_inline constexpr simd(std::array<T,N> const & values) noexcept : simd(load(values.data())) {}

    /// Return the native storage representation, including physical padding when present.
    native_nodiscard native_inline constexpr native_type to_native() const noexcept { return value; }
    /// Return the native storage value without a numerical conversion.
    native_nodiscard native_inline constexpr operator native_type() const noexcept requires(!simd_mask_element<T>) { return value; }
    /// Import native lanes, normalizing mask elements; other elements retain their bits. Clear physical padding.
    native_nodiscard static native_inline constexpr simd from_native(native_type x) noexcept {
      if constexpr(simd_mask_element<T>) return from_storage(storage_type::from_native(std::bit_cast<typename storage_type::native_type>(x)));
      else return simd(x);
    }
    /// Adopt native bits and clear physical padding. If T is a mask element, every logical lane must already be canonical.
    native_nodiscard static native_inline constexpr simd unsafe_from_native(native_type x) noexcept { return simd(x); }
    /// Return the corresponding four-lane storage vector without changing logical lane bits.
    native_nodiscard native_inline constexpr storage_type to_storage() const noexcept {
      if constexpr(simd_mask_element<T>) return storage_type::unsafe_from_native(std::bit_cast<typename storage_type::native_type>(value));
      else return storage_type::from_native(std::bit_cast<typename storage_type::native_type>(value));
    }
    /// Copy the first N lanes from a four-lane storage value and clear physical padding.
    native_nodiscard static native_inline constexpr simd from_storage(storage_type x) noexcept {
      return simd(std::bit_cast<native_type>(x.to_native()));
    }
    // The caller supplies exactly N logical lanes; alignment never grants a
    // readable fourth lane. Three-lane x86 transfers use native masked memory.
    /// Load exactly the logical lanes; the template alignment is a caller promise, never permission to read padding.
    template<std::size_t Alignment=1>
    native_nodiscard static native_inline constexpr simd load_memory(T const * p) noexcept {
      if consteval {
        native_type words{};
        for (std::size_t i=0;i<N;++i) words[i]=::NATIVE_BACKEND_NAMESPACE::short_word(p[i]);
        return simd(unchecked{},words);
      }
#if defined(__x86_64__) || defined(_M_X64)
      if constexpr(bool(NATIVE_HAS_AVX2)) {
        if constexpr(N==2) return simd(unchecked{},std::bit_cast<native_type>(_mm_loadl_epi64(reinterpret_cast<__m128i const *>(p))));
        else if constexpr(bool(NATIVE_HAS_AVX512VL)) {
          if constexpr(std::same_as<T,float>) return simd(unchecked{},std::bit_cast<native_type>(_mm_maskz_loadu_ps(7,p)));
          else return simd(unchecked{},std::bit_cast<native_type>(_mm_maskz_loadu_epi32(7,p)));
        } else {
          auto active=_mm_set_epi32(0,-1,-1,-1);
          if constexpr(std::same_as<T,float>) return simd(unchecked{},std::bit_cast<native_type>(_mm_maskload_ps(p,active)));
          else return simd(unchecked{},std::bit_cast<native_type>(_mm_maskload_epi32(reinterpret_cast<int const *>(p),active)));
        }
      }
#elif defined(__aarch64__) || defined(_M_ARM64)
      if constexpr((::native::arm_feature::neon <= Arch)) {
        if constexpr(std::same_as<T,float>) {
          auto x=vcombine_f32(vld1_f32(p),vdup_n_f32(0.f));
          if constexpr(N==3) x=vld1q_lane_f32(p+2,x,2);
          return simd(unchecked{},std::bit_cast<native_type>(x));
        } else {
          auto q=reinterpret_cast<std::uint32_t const *>(p);
          auto x=vcombine_u32(vld1_u32(q),vdup_n_u32(0));
          if constexpr(N==3) x=vld1q_lane_u32(q+2,x,2);
          return simd(unchecked{},std::bit_cast<native_type>(x));
        }
      }
#endif
    }
    /// Store exactly the logical lanes; the template alignment is a caller promise, never permission to write padding.
    template<std::size_t Alignment=1>
    native_inline constexpr void store_memory(T * p) const noexcept {
      if consteval {
        for (std::size_t i=0;i<N;++i) {
          if constexpr(simd_mask_element<T>) p[i]=T::from_bits(value[i]);
          else p[i]=value[i];
        }
        return;
      }
#if defined(__x86_64__) || defined(_M_X64)
      if constexpr(bool(NATIVE_HAS_AVX2)) {
        if constexpr(N==2) _mm_storel_epi64(reinterpret_cast<__m128i *>(p),std::bit_cast<__m128i>(value));
        else if constexpr(bool(NATIVE_HAS_AVX512VL)) {
          if constexpr(std::same_as<T,float>) _mm_mask_storeu_ps(p,7,std::bit_cast<__m128>(value));
          else _mm_mask_storeu_epi32(p,7,std::bit_cast<__m128i>(value));
        } else {
          auto active=_mm_set_epi32(0,-1,-1,-1);
          if constexpr(std::same_as<T,float>) _mm_maskstore_ps(p,active,std::bit_cast<__m128>(value));
          else _mm_maskstore_epi32(reinterpret_cast<int *>(p),active,std::bit_cast<__m128i>(value));
        }
      }
#elif defined(__aarch64__) || defined(_M_ARM64)
      if constexpr((::native::arm_feature::neon <= Arch)) {
        if constexpr(std::same_as<T,float>) {
          auto x=std::bit_cast<float32x4_t>(value);vst1_f32(p,vget_low_f32(x));
          if constexpr(N==3) vst1q_lane_f32(p+2,x,2);
        } else {
          auto q=reinterpret_cast<std::uint32_t *>(p);auto x=std::bit_cast<uint32x4_t>(value);
          vst1_u32(q,vget_low_u32(x));
          if constexpr(N==3) vst1q_lane_u32(q+2,x,2);
        }
      }
#endif
    }
    /// Load exactly the logical lanes; no extra pointer alignment is required.
    native_nodiscard static native_inline constexpr simd load(T const * p) noexcept { return load_memory(p); }
    /// Load exactly the logical lanes without an additional alignment requirement.
    native_nodiscard static native_inline constexpr simd loadu(T const * p) noexcept { return load_memory(p); }
    /// Store exactly the logical lanes; no extra pointer alignment is required.
    native_inline constexpr void store(T * p) const noexcept { store_memory(p); }
    /// Store exactly the logical lanes without an additional alignment requirement.
    native_inline constexpr void storeu(T * p) const noexcept { store_memory(p); }
    /// Read exactly n logical lanes and fill the remainder; require n <= lanes. For n == 0, p may be null.
    native_nodiscard static native_inline constexpr simd load_partial(T const * p,std::size_t n,T fill={}) noexcept native_diagnose_if(n > simd::lanes,"partial SIMD count exceeds the lane count") {
      std::array<T,N> values;values.fill(fill);
      if consteval { for (std::size_t i=0;i<n;++i) values[i]=p[i]; }
      else { if(n) std::memcpy(values.data(),p,n*sizeof(T)); }
      return load(values.data());
    }
    /// Write exactly n logical lanes; require n <= lanes. For n == 0, p may be null.
    native_inline constexpr void store_partial(T * p,std::size_t n) const noexcept native_diagnose_if(n > simd::lanes,"partial SIMD count exceeds the lane count") {
      std::array<T,N> values;store(values.data());
      if consteval { for (std::size_t i=0;i<n;++i) p[i]=values[i]; }
      else { if(n) std::memcpy(p,values.data(),n*sizeof(T)); }
    }
    /// Return the exact binary32 lane representations in the unsigned vector.
    native_nodiscard native_inline constexpr bits_type bits() const noexcept requires std::same_as<T,float> {
      return bits_type::from_storage(to_storage().bits());
    }
    /// Return the exact binary32 lane representations in the unsigned vector.
    native_nodiscard native_inline constexpr bits_type to_bits() const noexcept requires std::same_as<T,float> { return bits(); }
    /// Reinterpret binary32 words as lane values without normalization; a scalar word is broadcast.
    native_nodiscard static native_inline constexpr simd from_bits(bits_type x) noexcept requires std::same_as<T,float> {
      return from_storage(storage_type::from_bits(x.to_storage()));
    }
    /// Reinterpret binary32 words as lane values without normalization; a scalar word is broadcast.
    native_nodiscard static native_inline constexpr simd from_bits(std::uint32_t x) noexcept requires std::same_as<T,float> { return from_bits(bits_type(x)); }
    /// Broadcast the float value without adding an FTZ or other normalization policy.
    native_nodiscard static native_inline constexpr simd from_float(float x) noexcept requires std::same_as<T,float> { return simd(x); }
    /// Adopt raw float storage without numerical conversion or normalization.
    native_nodiscard static native_inline constexpr simd unsafe_from_float32(native_type x) noexcept requires std::same_as<T,float> { return simd(x); }
    /// Load exactly the logical count of binary32 words without normalizing their representations.
    native_nodiscard static native_inline constexpr simd load_bits(std::uint32_t const * p) noexcept requires std::same_as<T,float> { return from_bits(bits_type::load(p)); }
    /// Store the exact binary32 words for every logical lane.
    native_inline constexpr void store_bits(std::uint32_t * p) const noexcept requires std::same_as<T,float> { bits().store(p); }
    /// Read n representation words and fill the remaining logical lanes; require n <= lanes. A zero count permits null.
    native_nodiscard static native_inline constexpr simd load_bits_partial(std::uint32_t const * p,std::size_t n,std::uint32_t fill=0) noexcept requires std::same_as<T,float> native_diagnose_if(n > simd::lanes,"partial SIMD count exceeds the lane count") { return from_bits(bits_type::load_partial(p,n,fill)); }
    /// Write n exact representation words; require n <= lanes. A zero count permits null.
    native_inline constexpr void store_bits_partial(std::uint32_t * p,std::size_t n) const noexcept requires std::same_as<T,float> native_diagnose_if(n > simd::lanes,"partial SIMD count exceeds the lane count") { bits().store_partial(p,n); }
    /// Import lane i from bit i, clearing bits above the logical lane count.
    native_nodiscard static native_inline constexpr simd from_bitset(std::uint64_t bits) noexcept requires simd_mask_element<T> { return from_storage(storage_type::from_bitset(bits&lane_mask)); }
    /// Pack each logical lane truth value into bit i; higher bits are zero.
    native_nodiscard native_inline constexpr std::uint64_t to_bitset() const noexcept requires simd_mask_element<T> { return to_storage().to_bitset()&lane_mask; }
    /// Return true when at least one logical lane is true.
    native_nodiscard friend native_inline constexpr bool any(simd x) noexcept requires simd_mask_element<T> { return x.to_bitset()!=0; }
    /// Return true exactly when every logical lane is true.
    native_nodiscard friend native_inline constexpr bool all(simd x) noexcept requires simd_mask_element<T> { return x.to_bitset()==lane_mask; }
    /// Return true exactly when no logical lane is true.
    native_nodiscard friend native_inline constexpr bool none(simd x) noexcept requires simd_mask_element<T> { return !any(x); }

    /// Add corresponding logical lanes using the full-register operation.
    /// Integer results wrap at the lane width; floating results follow the caller's environment.
    native_nodiscard friend native_inline constexpr simd operator+(simd a,simd b) noexcept requires(!simd_mask_element<T>) { return clean(a.to_storage()+b.to_storage()); }
    /// Subtract corresponding logical lanes using the full-register operation.
    /// Integer results wrap at the lane width; floating results follow the caller's environment.
    native_nodiscard friend native_inline constexpr simd operator-(simd a,simd b) noexcept requires(!simd_mask_element<T>) { return clean(a.to_storage()-b.to_storage()); }
    /// Multiply corresponding logical lanes using the full-register operation.
    /// Integer results wrap at the lane width; floating results follow the caller's environment.
    native_nodiscard friend native_inline constexpr simd operator*(simd a,simd b) noexcept requires(!simd_mask_element<T>) { return clean(a.to_storage()*b.to_storage()); }
    /// Divide corresponding logical lanes using the full-register operation.
    /// Inactive denominator lanes are set to one so padding does not introduce division by zero.
    native_nodiscard friend native_inline constexpr simd operator/(simd a,simd b) noexcept requires std::same_as<T,float> {
      auto padded=__builtin_shufflevector(b.value,native_type{1.f,1.f,1.f,1.f},0,1,N==3?2:4,4);
      auto divisor=storage_type::from_native(std::bit_cast<typename storage_type::native_type>(padded));
      return clean(a.to_storage()/divisor);
    }
    /// Negate every logical lane; floating-point lanes change sign.
    native_nodiscard friend native_inline constexpr simd operator-(simd a) noexcept requires(!simd_mask_element<T>) { return from_storage(-a.to_storage()); }
    /// Bitwise AND of corresponding lane representations.
    native_nodiscard friend native_inline constexpr simd operator&(simd a,simd b) noexcept requires(!std::same_as<T,float>) { return from_storage(a.to_storage()&b.to_storage()); }
    /// Bitwise OR of corresponding lane representations.
    native_nodiscard friend native_inline constexpr simd operator|(simd a,simd b) noexcept requires(!std::same_as<T,float>) { return from_storage(a.to_storage()|b.to_storage()); }
    /// Bitwise XOR of corresponding lane representations.
    native_nodiscard friend native_inline constexpr simd operator^(simd a,simd b) noexcept requires(!std::same_as<T,float>) { return from_storage(a.to_storage()^b.to_storage()); }
    /// Complement every bit in every lane.
    native_nodiscard friend native_inline constexpr simd operator~(simd a) noexcept requires(!std::same_as<T,float>) { return from_storage(~a.to_storage()); }
    /// Return the lane-wise logical complement, retaining this mask type.
    native_nodiscard friend native_inline constexpr simd operator!(simd a) noexcept requires simd_mask_element<T> { return ~a; }
    /// Return a mask whose lanes are true where `a == b` holds.
    native_nodiscard friend native_inline constexpr mask_type operator==(simd a,simd b) noexcept { return comparison(a.to_storage()==b.to_storage()); }
    /// Return a mask whose lanes are true where `a != b` holds.
    native_nodiscard friend native_inline constexpr mask_type operator!=(simd a,simd b) noexcept { return ~(a==b); }
    /// Return a mask whose lanes are true where `a < b` holds.
    native_nodiscard friend native_inline constexpr mask_type operator<(simd a,simd b) noexcept requires(!simd_mask_element<T>) { return comparison(a.to_storage()<b.to_storage()); }
    /// Return a mask whose lanes are true where `a > b` holds.
    native_nodiscard friend native_inline constexpr mask_type operator>(simd a,simd b) noexcept requires(!simd_mask_element<T>) { return comparison(a.to_storage()>b.to_storage()); }
    /// Return a mask whose lanes are true where `a <= b` holds.
    native_nodiscard friend native_inline constexpr mask_type operator<=(simd a,simd b) noexcept requires(!simd_mask_element<T>) { return (a<b)|(a==b); }
    /// Return a mask whose lanes are true where `a >= b` holds.
    native_nodiscard friend native_inline constexpr mask_type operator>=(simd a,simd b) noexcept requires(!simd_mask_element<T>) { return (a>b)|(a==b); }
    /// Choose a in true mask lanes and b in false lanes; both values are already evaluated.
    template<class M> requires(std::same_as<M,mask_type> || std::same_as<M,vector_mask_type>)
    native_nodiscard friend native_inline constexpr simd select(M mask,simd a,simd b) noexcept {
      using FM=typename storage_type::mask_type;
      return clean(select(FM::from_bitset(mask.to_bitset()),a.to_storage(),b.to_storage()));
    }
    /// Compute a*b+c with one fused rounding per logical lane in the caller's floating-point environment.
    native_nodiscard friend native_inline constexpr simd fma(simd a,simd b,simd c) noexcept requires std::same_as<T,float> { return clean(fma(a.to_storage(),b.to_storage(),c.to_storage())); }
    /// Compute the native square root of each logical lane in the caller's floating-point environment.
    native_nodiscard friend native_inline constexpr simd sqrt(simd a) noexcept requires std::same_as<T,float> { return clean(sqrt(a.to_storage())); }
    /// Round each logical lane to an integral value, ties to even, independently of the ambient rounding direction.
    native_nodiscard friend native_inline constexpr simd round_even(simd a) noexcept requires std::same_as<T,float> { return clean(round_even(a.to_storage())); }
    /// Construct normal powers of two; require each input to be an integral exponent in [-126,127].
    native_nodiscard friend native_inline constexpr simd normal_pow2(simd a) noexcept requires std::same_as<T,float> { return from_storage(normal_pow2(a.to_storage())); }
    /// Shift every lane left by K bits, discarding high bits. Require K smaller than the lane bit width.
    template<std::size_t K> requires(K<32)
    native_nodiscard friend native_inline constexpr simd operator<<(simd a,imm_t<K>) noexcept requires simd_integer_element<T> { return from_storage(a.to_storage()<<imm<K>); }
    /// Shift every lane right by K; signed lanes extend the sign, unsigned lanes shift in zero. Require K smaller than the lane bit width.
    template<std::size_t K> requires(K<32)
    native_nodiscard friend native_inline constexpr simd operator>>(simd a,imm_t<K>) noexcept requires simd_integer_element<T> { return from_storage(a.to_storage()>>imm<K>); }
    /// Shift each lane left by compile-time K, discarding high bits; require K below the lane bit width.
    template<unsigned K> requires(K<32) && simd_integer_element<T>
    native_nodiscard native_inline constexpr simd left() const noexcept { return *this<<imm<K>; }
    /// Shift each lane right by compile-time K; signed lanes extend their sign. Require K below the lane bit width.
    template<unsigned K> requires(K<32) && simd_integer_element<T>
    native_nodiscard native_inline constexpr simd right() const noexcept { return *this>>imm<K>; }
    // Match native integer registers: division and run-time shifts are absent.
    /// Reject runtime shift counts; use a compile-time imm<K> within the lane width.
    friend simd operator<<(simd,simd) requires simd_integer_element<T> = delete;
    /// Reject runtime shift counts; use a compile-time imm<K> within the lane width.
    friend simd operator>>(simd,simd) requires simd_integer_element<T> = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    friend simd operator/(simd,simd) requires simd_integer_element<T> = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    friend simd operator%(simd,simd) requires simd_integer_element<T> = delete;
    /// Reject runtime shift counts; use a compile-time imm<K> within the lane width.
    template<simd_integer_element U> friend simd operator<<(simd,U) requires simd_integer_element<T> = delete;
    /// Reject runtime shift counts; use a compile-time imm<K> within the lane width.
    template<simd_integer_element U> friend simd operator>>(simd,U) requires simd_integer_element<T> = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template<simd_integer_element U> friend simd operator/(simd,U) requires simd_integer_element<T> = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template<simd_integer_element U> friend simd operator/(U,simd) requires simd_integer_element<T> = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template<simd_integer_element U> friend simd operator%(simd,U) requires simd_integer_element<T> = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template<simd_integer_element U> friend simd operator%(U,simd) requires simd_integer_element<T> = delete;
    /// Apply the corresponding lane-wise add operation in place and return *this.
    native_inline constexpr simd & operator+=(simd b) noexcept requires(!simd_mask_element<T>) { return *this=*this+b; }
    /// Apply the corresponding lane-wise subtract operation in place and return *this.
    native_inline constexpr simd & operator-=(simd b) noexcept requires(!simd_mask_element<T>) { return *this=*this-b; }
    /// Apply the corresponding lane-wise multiply operation in place and return *this.
    native_inline constexpr simd & operator*=(simd b) noexcept requires(!simd_mask_element<T>) { return *this=*this*b; }
    /// Apply the corresponding lane-wise divide operation in place and return *this.
    native_inline constexpr simd & operator/=(simd b) noexcept requires std::same_as<T,float> { return *this=*this/b; }
    /// Apply the corresponding lane-wise AND operation in place and return *this.
    native_inline constexpr simd & operator&=(simd b) noexcept requires(!std::same_as<T,float>) { return *this=*this&b; }
    /// Apply the corresponding lane-wise OR operation in place and return *this.
    native_inline constexpr simd & operator|=(simd b) noexcept requires(!std::same_as<T,float>) { return *this=*this|b; }
    /// Apply the corresponding lane-wise XOR operation in place and return *this.
    native_inline constexpr simd & operator^=(simd b) noexcept requires(!std::same_as<T,float>) { return *this=*this^b; }
  private:
    struct unchecked {};
    native_inline constexpr simd(unchecked,native_type x) noexcept : value(x) {}
    native_nodiscard static native_inline constexpr simd clean(storage_type x) noexcept { return simd(unchecked{},std::bit_cast<native_type>(x.to_native())); }
    template<class M> native_nodiscard static native_inline constexpr mask_type comparison(M x) noexcept {
      if constexpr(mask_type::compact) return mask_type::from_native(x.to_native());
      else return mask_type::from_storage(x);
    }
  };

  /// Choose each bit from a where the corresponding mask bit is one, otherwise from b; arbitrary bit masks are permitted.
  template<simd_integer_element T,std::size_t N,::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) &&(N==2 || N==3) && (sizeof(T)==4)
  native_nodiscard native_inline constexpr simd<T,N,Arch> bit_select(simd<T,N,Arch> bits,simd<T,N,Arch> a,simd<T,N,Arch> b) noexcept { return (bits&a)|(~bits&b); }
  /// Add matching integer lanes, retaining prior in inactive mask lanes; arithmetic wraps at the lane width.
  template<simd_integer_element T,std::size_t N,::native::isa<> Arch,class M>
    requires NATIVE_ARCH_REQUIRES(Arch) &&(N==2 || N==3) && (sizeof(T)==4) &&
      (std::same_as<M,typename simd<T,N,Arch>::mask> || std::same_as<M,simd<mask32,N,Arch>>)
  native_nodiscard native_inline constexpr simd<T,N,Arch> masked_add(M m,simd<T,N,Arch> prior,simd<T,N,Arch> a,simd<T,N,Arch> b) noexcept { return select(m,a+b,prior); }
  /// Subtract matching integer lanes, retaining prior in inactive mask lanes; arithmetic wraps at the lane width.
  template<simd_integer_element T,std::size_t N,::native::isa<> Arch,class M>
    requires NATIVE_ARCH_REQUIRES(Arch) &&(N==2 || N==3) && (sizeof(T)==4) &&
      (std::same_as<M,typename simd<T,N,Arch>::mask> || std::same_as<M,simd<mask32,N,Arch>>)
  native_nodiscard native_inline constexpr simd<T,N,Arch> masked_sub(M m,simd<T,N,Arch> prior,simd<T,N,Arch> a,simd<T,N,Arch> b) noexcept { return select(m,a-b,prior); }
  /// Multiply matching integer lanes, retaining prior in inactive mask lanes; arithmetic wraps at the lane width.
  template<simd_integer_element T,std::size_t N,::native::isa<> Arch,class M>
    requires NATIVE_ARCH_REQUIRES(Arch) &&(N==2 || N==3) && (sizeof(T)==4) &&
      (std::same_as<M,typename simd<T,N,Arch>::mask> || std::same_as<M,simd<mask32,N,Arch>>)
  native_nodiscard native_inline constexpr simd<T,N,Arch> masked_mul(M m,simd<T,N,Arch> prior,simd<T,N,Arch> a,simd<T,N,Arch> b) noexcept { return select(m,a*b,prior); }
}

// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#endif
namespace native {
  namespace detail::NATIVE_BACKEND {
    enum class rounding_direction { down, up, zero };

    template<rounding_direction Direction,std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch)
    native_inline constexpr simd<float,N,Arch> round_integral(simd<float,N,Arch> x) noexcept {
      using V = simd<float,N,Arch>;
      if consteval {
        if constexpr(Direction==rounding_direction::down) return ::native::detail::float_constant::map(::native::detail::float_constant::floor,x);
        else if constexpr(Direction==rounding_direction::up) return ::native::detail::float_constant::map(::native::detail::float_constant::ceil,x);
        else return ::native::detail::float_constant::map(::native::detail::float_constant::trunc,x);
      }
      if constexpr (N == 2 || N == 3) {
        return V::from_storage(round_integral<Direction>(x.to_storage()));
      } else {
#if NATIVE_HAS_AVX2
        constexpr int mode = (Direction == rounding_direction::down ? _MM_FROUND_TO_NEG_INF :
          Direction == rounding_direction::up ? _MM_FROUND_TO_POS_INF : _MM_FROUND_TO_ZERO) | _MM_FROUND_NO_EXC;
        if constexpr (N == 1)
          return V::from_native(_mm_cvtss_f32(_mm_round_ss(_mm_setzero_ps(),_mm_set_ss(x.to_native()),mode)));
        else if constexpr (N == 4) return V::from_native(_mm_round_ps(x.to_native(),mode));
        else if constexpr (N == 8) return V::from_native(_mm256_round_ps(x.to_native(),mode));
#if NATIVE_HAS_AVX512F
        else if constexpr (N == 16) return V::from_native(_mm512_roundscale_ps(x.to_native(),mode));
#endif
#elif NATIVE_HAS_ARM_NEON
        if constexpr (N == 1) {
          auto a = vdup_n_f32(x.to_native());
          if constexpr (Direction == rounding_direction::down) return V::from_native(vget_lane_f32(vrndm_f32(a),0));
          else if constexpr (Direction == rounding_direction::up) return V::from_native(vget_lane_f32(vrndp_f32(a),0));
          else return V::from_native(vget_lane_f32(vrnd_f32(a),0));
        } else {
          if constexpr (Direction == rounding_direction::down) return V::from_native(__builtin_elementwise_floor(x.to_native()));
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
  template<std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N>
  native_nodiscard native_inline constexpr simd<float,N,Arch> floor(simd<float,N,Arch> x) noexcept {
    return detail::NATIVE_BACKEND::round_integral<detail::NATIVE_BACKEND::rounding_direction::down>(x);
  }
  /// \ingroup vector_math
  /// Apply floor to each register in an array; an empty array performs no lane work.
  template<std::size_t N,std::size_t M, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N>
  native_nodiscard native_inline constexpr std::array<simd<float,N,Arch>,M> floor(std::array<simd<float,N,Arch>,M> const & input) noexcept {
    auto const & [...x] = input;
    return {{floor(x)...}};
  }
  /// \ingroup vector_math
  /// Round each lane toward positive infinity, independently of ambient rounding.
  /// Signed zeros and infinities are preserved; NaNs remain NaNs. Raw denormal
  /// handling still follows the CPU environment. No NaN payload or FP-status promise is added.
  /// \snippet api.cc rounding
  template<std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N>
  native_nodiscard native_inline constexpr simd<float,N,Arch> ceil(simd<float,N,Arch> x) noexcept {
    return detail::NATIVE_BACKEND::round_integral<detail::NATIVE_BACKEND::rounding_direction::up>(x);
  }
  /// \ingroup vector_math
  /// Apply ceil to each register in an array; an empty array performs no lane work.
  template<std::size_t N,std::size_t M, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N>
  native_nodiscard native_inline constexpr std::array<simd<float,N,Arch>,M> ceil(std::array<simd<float,N,Arch>,M> const & input) noexcept {
    auto const & [...x] = input;
    return {{ceil(x)...}};
  }
  /// \ingroup vector_math
  /// Round each lane toward zero, independently of ambient rounding.
  /// Signed zeros and infinities are preserved; NaNs remain NaNs. Raw denormal
  /// handling still follows the CPU environment. No NaN payload or FP-status promise is added.
  /// \snippet api.cc rounding
  template<std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N>
  native_nodiscard native_inline constexpr simd<float,N,Arch> trunc(simd<float,N,Arch> x) noexcept {
    return detail::NATIVE_BACKEND::round_integral<detail::NATIVE_BACKEND::rounding_direction::zero>(x);
  }
  /// \ingroup vector_math
  /// Apply trunc to each register in an array; an empty array performs no lane work.
  template<std::size_t N,std::size_t M, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) && ::NATIVE_BACKEND_NAMESPACE::float_shape<N>
  native_nodiscard native_inline constexpr std::array<simd<float,N,Arch>,M> trunc(std::array<simd<float,N,Arch>,M> const & input) noexcept {
    auto const & [...x] = input;
    return {{trunc(x)...}};
  }
}

// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0


namespace native::detail::NATIVE_BACKEND {
  template<std::size_t N,class M>
  native_inline std::uint32_t compaction_mask_bits(M mask) noexcept {
#if NATIVE_HAS_AVX2
    if constexpr (N > 1 && !M::compact) {
      if constexpr (sizeof(M) == 32)
        return std::uint32_t(_mm256_movemask_ps(std::bit_cast<__m256>(mask.to_native())));
      else return std::uint32_t(_mm_movemask_ps(std::bit_cast<__m128>(mask.to_native()))) & ((1u << N) - 1);
    } else
#elif NATIVE_HAS_ARM_NEON
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
  native_inline V compact_register(std::uint32_t mask, V input, V prior) noexcept {
    if constexpr (V::lanes == 1) return mask ? input : prior;
    else {
#if NATIVE_HAS_AVX512F
      if constexpr (sizeof(V) == 64) {
        auto x = std::bit_cast<__m512i>(input), merge = std::bit_cast<__m512i>(prior);
        if constexpr (Expand) return std::bit_cast<V>(_mm512_mask_expand_epi32(merge, __mmask16(mask), x));
        else return std::bit_cast<V>(_mm512_mask_compress_epi32(merge, __mmask16(mask), x));
      }
#if NATIVE_HAS_AVX512VL
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
#if (!NATIVE_HAS_AVX512F || !NATIVE_HAS_AVX512VL) && (NATIVE_HAS_AVX2 || NATIVE_HAS_ARM_NEON)
      {
      V permuted;
#if NATIVE_HAS_AVX2
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
#if NATIVE_HAS_AVX2
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

namespace native {
  /// \ingroup vector_memory
  /// Pack selected logical lanes in increasing order; fill all remaining lanes.
  /// This rearranges bits, including NaN payloads and signed zeros. Short padding
  /// never contributes to count or output and is zero in the result.
  /// \snippet api.cc compaction
  template<class T, std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) &&
    (std::same_as<T,float> || std::same_as<T,std::int32_t> || std::same_as<T,std::uint32_t>) &&
    ::NATIVE_BACKEND_NAMESPACE::float_shape<N>
  native_nodiscard native_inline constexpr compaction_result<simd<T,N,Arch>> compress(
      typename simd<T,N,Arch>::mask mask, simd<T,N,Arch> value, T fill = T{}) noexcept {
    if consteval {
      std::array<T,N> input{},output{}; value.store(input.data()); output.fill(fill);
      auto bits=mask.to_bitset(); std::size_t count=0;
      for(std::size_t i=0;i<N;++i) if((bits>>i)&1) output[count++]=input[i];
      return {simd<T,N,Arch>::load(output.data()),count};
    }
    using V = simd<T,N,Arch>;
    auto bits = detail::NATIVE_BACKEND::compaction_mask_bits<N>(mask);
    // Construct fill through integer object representation, without FP arithmetic.
    using U = simd<std::uint32_t,N,Arch>;
    V prior = std::bit_cast<V>(U(std::bit_cast<std::uint32_t>(fill)));
    return {detail::NATIVE_BACKEND::compact_register<false>(bits, value, prior), std::size_t(std::popcount(bits))};
  }

  /// \ingroup vector_memory
  /// Consume packed's first popcount(mask) lanes in order at selected positions.
  /// Unselected lanes retain prior bitwise. Short physical padding is zeroed.
  template<class T, std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) &&
    (std::same_as<T,float> || std::same_as<T,std::int32_t> || std::same_as<T,std::uint32_t>) &&
    ::NATIVE_BACKEND_NAMESPACE::float_shape<N>
  native_nodiscard native_inline constexpr simd<T,N,Arch> expand(
      typename simd<T,N,Arch>::mask mask, simd<T,N,Arch> packed,
      simd<T,N,Arch> prior) noexcept {
    if consteval {
      std::array<T,N> input{},output{}; packed.store(input.data()); prior.store(output.data());
      auto bits=mask.to_bitset(); std::size_t count=0;
      for(std::size_t i=0;i<N;++i) if((bits>>i)&1) output[i]=input[count++];
      return simd<T,N,Arch>::load(output.data());
    }
    auto bits = detail::NATIVE_BACKEND::compaction_mask_bits<N>(mask);
    auto result = detail::NATIVE_BACKEND::compact_register<true>(bits, packed, prior);
    if constexpr (N == 2 || N == 3) return simd<T,N,Arch>::from_storage(result.to_storage());
    else return result;
  }

  /// \ingroup vector_memory
  /// Write the first min(capacity,popcount(mask)) selected lanes, in input order.
  /// Return the number WRITTEN, not the total selected count. No element beyond
  /// that prefix is accessed. Null is valid when capacity or the mask is zero.
  template<class T, std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) &&
    (std::same_as<T,float> || std::same_as<T,std::int32_t> || std::same_as<T,std::uint32_t>) &&
    ::NATIVE_BACKEND_NAMESPACE::float_shape<N>
  native_inline constexpr std::size_t compress_store(T * destination, std::size_t capacity,
      typename simd<T,N,Arch>::mask mask, simd<T,N,Arch> value) noexcept {
    if consteval {
      std::array<T,N> input{}; value.store(input.data());
      auto bits=mask.to_bitset(); std::size_t written=0;
      for(std::size_t i=0;i<N && written<capacity;++i)
        if((bits>>i)&1) destination[written++]=input[i];
      return written;
    }
    auto bits = detail::NATIVE_BACKEND::compaction_mask_bits<N>(mask);
    auto selected = std::size_t(std::popcount(bits));
    auto written = capacity < selected ? capacity : selected;
    if (!written) return 0;
#if NATIVE_HAS_AVX512F
    if constexpr (N > 1 && (sizeof(value) == 64 || NATIVE_HAS_AVX512VL)) {
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

#if NATIVE_HAS_WASM_SIMD128 || defined(NATIVE_DOXYGEN)
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Included only in the SIMD128 backend target scope.
namespace native {
  namespace detail {
    template<class T>
    concept wasm_number =
      simd_integer_element<T> || std::same_as<T, float> || std::same_as<T, double>;
    template<class T>
    using wasm_word = std::conditional_t<
      sizeof(T) == 1, std::uint8_t,
      std::conditional_t<sizeof(T) == 2, std::uint16_t,
                         std::conditional_t<sizeof(T) == 4, std::uint32_t, std::uint64_t>>>;
    template<class T>
    using wasm_format =
      std::conditional_t<sizeof(T) == 4, constexpr_float::binary32, constexpr_float::binary64>;

    template<class V>
    constexpr auto wasm_lanes(V v) noexcept {
      std::array<typename V::value_type, V::lanes> a{};
      v.store(a.data());
      return a;
    }

    template<class V, class F, class... W>
    constexpr V wasm_map(F f, V v, W... w) noexcept {
      auto inputs = std::tuple{wasm_lanes(v), wasm_lanes(w)...};
      std::array<typename V::value_type, V::lanes> r{};
      for (std::size_t i = 0; i < V::lanes; ++i) {
        r[i] = std::apply([&](auto const &... a) { return f(a[i]...); }, inputs);
      }
      return V::load(r.data());
    }

    template<class T, std::size_t N, isa<> A>
    requires ordinary_simd_element<T> && (A.has(wasm_feature::simd128)) &&
             (wasm_number<T> || simd_mask_element<T>) && (sizeof(T) * N == 16)
    struct value_traits<simd<T, N, A>> {
      static constexpr isa<> value = A;
      static constexpr bool known = true;
      static constexpr bool aggregate_default = false;
    };
  } // namespace detail

  /// Canonical zero/all-one SIMD128 comparison lanes.
  template<class U, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(U) * N == 16)
  struct alignas(16) simd<mask_lane<U>, N, A> {
    using value_type = mask_lane<U>;
    using native_type = v128_t;
    using mask_type = simd;
    using mask = simd;
    template<class T>
    using rebind = simd<T, N, A>;
    static constexpr auto architecture = A;
    static constexpr std::size_t lanes = N;
    static constexpr bool compact = false;

  private:
    native_type value_{};

  public:
    /// Initialize every lane to zero.
    constexpr simd() noexcept = default;

    /// Broadcast one truth value as canonical zero/all-one lanes.
    explicit native_inline constexpr simd(bool x) noexcept
      : simd(from_bitset(x ? ~std::uint64_t{} : 0)) {}

    /// Return the implementation register without changing its bits.
    native_inline constexpr native_type to_native() const noexcept {
      return value_;
    }

    /// Adopt implementation bits; mask callers must supply canonical lanes.
    static native_inline constexpr simd unsafe_from_native(native_type x) noexcept {
      simd r;
      r.value_ = x;
      return r;
    }

    /// Adopt implementation storage; mask specializations normalize nonzero lanes.
    static native_inline constexpr simd from_native(native_type x) noexcept {
      if consteval {
        auto words = __builtin_bit_cast(std::array<U, N>, x);
        for (auto & w : words) {
          w = w ? U(~U(0)) : U(0);
        }
        return unsafe_from_native(__builtin_bit_cast(native_type, words));
      } else {
        if constexpr (sizeof(U) == 1) {
          return unsafe_from_native(wasm_i8x16_ne(x, wasm_i32x4_splat(0)));
        } else if constexpr (sizeof(U) == 2) {
          return unsafe_from_native(wasm_i16x8_ne(x, wasm_i32x4_splat(0)));
        } else if constexpr (sizeof(U) == 4) {
          return unsafe_from_native(wasm_i32x4_ne(x, wasm_i32x4_splat(0)));
        } else if constexpr (sizeof(U) == 8) {
          return unsafe_from_native(wasm_i64x2_ne(x, wasm_i32x4_splat(0)));
        }
      }
    }

    /// Expand low scalar bits to canonical mask lanes, ignoring excess bits.
    static native_inline constexpr simd from_bitset(std::uint64_t bits) noexcept {
      std::array<U, N> a{};
      for (std::size_t i = 0; i < N; ++i) {
        a[i] = ((bits >> i) & 1) ? U(~U(0)) : U(0);
      }
      return unsafe_from_native(__builtin_bit_cast(native_type, a));
    }

    /// Gather lane truth values into low scalar bits.
    native_inline constexpr std::uint64_t to_bitset() const noexcept {
      if consteval {
        auto a = __builtin_bit_cast(std::array<U, N>, value_);
        std::uint64_t r = 0;
        for (std::size_t i = 0; i < N; ++i) {
          r |= std::uint64_t(a[i] != 0) << i;
        }
        return r;
      } else {
        if constexpr (sizeof(U) == 1) {
          return wasm_i8x16_bitmask(value_);
        } else if constexpr (sizeof(U) == 2) {
          return wasm_i16x8_bitmask(value_);
        } else if constexpr (sizeof(U) == 4) {
          return wasm_i32x4_bitmask(value_);
        } else if constexpr (sizeof(U) == 8) {
          return wasm_i64x2_bitmask(value_);
        }
      }
    }

    /// Return the compact lane truth bitset.
    native_inline constexpr std::uint64_t bits() const noexcept {
      return to_bitset();
    }

    /// Read exactly N canonical mask lane objects.
    static native_inline constexpr simd load(value_type const * p) noexcept {
      std::array<U, N> a{};
      for (std::size_t i = 0; i < N; ++i) {
        a[i] = p[i].to_bits();
      }
      return unsafe_from_native(__builtin_bit_cast(native_type, a));
    }

    /// Write exactly N canonical mask lane objects.
    native_inline constexpr void store(value_type * p) const noexcept {
      auto a = __builtin_bit_cast(std::array<U, N>, value_);
      for (std::size_t i = 0; i < N; ++i) {
        p[i] = value_type::from_bits(a[i]);
      }
    }

    /// Load all lanes; Align is the caller-provided pointer alignment.
    template<std::size_t Align>
    static native_inline constexpr simd load_memory(value_type const * p) noexcept {
      return load(p);
    }

    /// Store all lanes; Align is the caller-provided pointer alignment.
    template<std::size_t Align>
    native_inline constexpr void store_memory(value_type * p) const noexcept {
      store(p);
    }

    /// Test whether at least one lane is true.
    friend native_inline constexpr bool any(simd v) noexcept {
      if consteval {
        return v.to_bitset() != 0;
      } else {
        return wasm_v128_any_true(v.value_);
      }
    }

    /// Test whether every lane is true.
    friend native_inline constexpr bool all(simd v) noexcept {
      if consteval {
        return v.to_bitset() == ((std::uint64_t{1} << N) - 1);
      } else {
        if constexpr (sizeof(U) == 1) {
          return wasm_i8x16_all_true(v.value_);
        } else if constexpr (sizeof(U) == 2) {
          return wasm_i16x8_all_true(v.value_);
        } else if constexpr (sizeof(U) == 4) {
          return wasm_i32x4_all_true(v.value_);
        } else {
          return wasm_i64x2_all_true(v.value_);
        }
      }
    }

    /// Test whether no lane is true.
    friend native_inline constexpr bool none(simd v) noexcept {
      return !any(v);
    }

    /// Intersect representation bits.
    friend native_inline constexpr simd operator&(simd a, simd b) noexcept {
      if consteval {
        return from_bitset(a.to_bitset() & b.to_bitset());
      } else {
        return unsafe_from_native(wasm_v128_and(a.value_, b.value_));
      }
    }

    /// Apply the corresponding lane operation and update this value.
    native_inline constexpr simd & operator&=(simd b) noexcept {
      return *this = *this & b;
    }

    /// Unite representation bits.
    friend native_inline constexpr simd operator|(simd a, simd b) noexcept {
      if consteval {
        return from_bitset(a.to_bitset() | b.to_bitset());
      } else {
        return unsafe_from_native(wasm_v128_or(a.value_, b.value_));
      }
    }

    /// Apply the corresponding lane operation and update this value.
    native_inline constexpr simd & operator|=(simd b) noexcept {
      return *this = *this | b;
    }

    /// Exclusive-or representation bits.
    friend native_inline constexpr simd operator^(simd a, simd b) noexcept {
      if consteval {
        return from_bitset(a.to_bitset() ^ b.to_bitset());
      } else {
        return unsafe_from_native(wasm_v128_xor(a.value_, b.value_));
      }
    }

    /// Apply the corresponding lane operation and update this value.
    native_inline constexpr simd & operator^=(simd b) noexcept {
      return *this = *this ^ b;
    }

    /// Complement every representation bit (or lane truth for masks).
    friend native_inline constexpr simd operator~(simd a) noexcept {
      if consteval {
        return from_bitset(~a.to_bitset());
      } else {
        return unsafe_from_native(wasm_v128_not(a.value_));
      }
    }

    /// Complement every mask lane.
    friend native_inline constexpr simd operator!(simd a) noexcept {
      return ~a;
    }

    /// Compare lane equality and return canonical mask lanes.
    friend native_inline constexpr simd operator==(simd a, simd b) noexcept {
      return ~(a ^ b);
    }

    /// Compare lane inequality and return canonical mask lanes.
    friend native_inline constexpr simd operator!=(simd a, simd b) noexcept {
      return a ^ b;
    }

    /// Choose mask lanes from a where m is true, otherwise from b.
    friend native_inline constexpr simd select(simd m, simd a, simd b) noexcept {
      return (m & a) | (~m & b);
    }
  };

  /// One complete WebAssembly SIMD128 register with semantic element lanes.
  template<detail::wasm_number T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16)
  struct alignas(16) simd<T, N, A> {
    using value_type = T;
    using native_type = v128_t;
    using word_type = detail::wasm_word<T>;
    using mask_type = simd<mask_lane<word_type>, N, A>;
    using mask = mask_type;
    using bits_type = simd<word_type, N, A>;
    using register_type = simd;
    template<class U>
    using rebind = simd<U, N, A>;
    static constexpr auto architecture = A;
    static constexpr std::size_t lanes = N;

  private:
    native_type value_{};

  public:
    /// Initialize every lane to zero.
    constexpr simd() noexcept = default;

    /// Broadcast a floating element value to every lane.
    native_inline constexpr simd(T x) noexcept
    requires std::is_floating_point_v<T>
    {
      if consteval {
        std::array<T, N> a{};
        a.fill(x);
        value_ = __builtin_bit_cast(native_type, a);
      } else {
        if constexpr (std::same_as<T, float>) {
          value_ = wasm_f32x4_splat(x);
        } else {
          value_ = wasm_f64x2_splat(x);
        }
      }
    }

    /// Broadcast an integer, retaining its low lane-width bits in every lane.
    template<simd_integer_element U>
    requires simd_integer_element<T>
    native_inline constexpr simd(U input) noexcept {
      auto x = static_cast<T>(input);
      if consteval {
        std::array<T, N> a{};
        a.fill(x);
        value_ = __builtin_bit_cast(native_type, a);
      } else {
        if constexpr (std::same_as<T, std::int8_t>) {
          value_ = wasm_i8x16_splat(x);
        } else if constexpr (std::same_as<T, std::uint8_t>) {
          value_ = wasm_u8x16_splat(x);
        } else if constexpr (std::same_as<T, std::int16_t>) {
          value_ = wasm_i16x8_splat(x);
        } else if constexpr (std::same_as<T, std::uint16_t>) {
          value_ = wasm_u16x8_splat(x);
        } else if constexpr (std::same_as<T, std::int32_t>) {
          value_ = wasm_i32x4_splat(x);
        } else if constexpr (std::same_as<T, std::uint32_t>) {
          value_ = wasm_u32x4_splat(x);
        } else if constexpr (std::same_as<T, std::int64_t>) {
          value_ = wasm_i64x2_splat(x);
        } else if constexpr (std::same_as<T, std::uint64_t>) {
          value_ = wasm_u64x2_splat(x);
        }
      }
    }

    /// Copy the array in lane order.
    native_inline constexpr explicit simd(std::array<T, N> const & a) noexcept
      : simd(load(a.data())) {}

    /// Construct N lanes; integer arguments retain their low lane-width bits.
    template<class... U>
    requires(sizeof...(U) == N && ((std::same_as<U, T> && ...) ||
                                   (simd_integer_element<T> && (simd_integer_element<U> && ...))))
    native_inline constexpr simd(U... x) noexcept : simd(std::array<T, N>{static_cast<T>(x)...}) {}

    /// Return the implementation register without changing its bits.
    native_inline constexpr native_type to_native() const noexcept {
      return value_;
    }

    /// Adopt implementation storage; mask specializations normalize nonzero lanes.
    static native_inline constexpr simd from_native(native_type x) noexcept {
      simd r;
      r.value_ = x;
      return r;
    }

    /// Adopt implementation bits; mask callers must supply canonical lanes.
    static native_inline constexpr simd unsafe_from_native(native_type x) noexcept {
      return from_native(x);
    }

    /// Load exactly 16 unaligned bytes as N element objects.
    static native_inline constexpr simd load(T const * p) noexcept {
      if consteval {
        std::array<T, N> a{};
        for (std::size_t i = 0; i < N; ++i) {
          a[i] = p[i];
        }
        return from_native(__builtin_bit_cast(native_type, a));
      } else {
        return from_native(wasm_v128_load(p));
      }
    }

    /// Store exactly 16 unaligned bytes in lane order.
    native_inline constexpr void store(T * p) const noexcept {
      if consteval {
        auto a = __builtin_bit_cast(std::array<T, N>, value_);
        for (std::size_t i = 0; i < N; ++i) {
          p[i] = a[i];
        }
      } else {
        wasm_v128_store(p, value_);
      }
    }

    /// Load all lanes; Align is the caller-provided pointer alignment.
    template<std::size_t Align>
    static native_inline constexpr simd load_memory(T const * p) noexcept {
      if consteval {
        return load(p);
      } else {
        return load(static_cast<T const *>(__builtin_assume_aligned(p, Align)));
      }
    }

    /// Store all lanes; Align is the caller-provided pointer alignment.
    template<std::size_t Align>
    native_inline constexpr void store_memory(T * p) const noexcept {
      if consteval {
        store(p);
      } else {
        store(static_cast<T *>(__builtin_assume_aligned(p, Align)));
      }
    }

    /// Read n lanes and fill the rest; require n <= N. Null is valid when n is zero.
    static native_inline constexpr simd load_partial(T const * p, std::size_t n,
                                                     T fill = {}) noexcept {
      std::array<T, N> a{};
      a.fill(fill);
      for (std::size_t i = 0; i < n; ++i) {
        a[i] = p[i];
      }
      return load(a.data());
    }

    /// Write only the first n lanes; require n <= N. Null is valid when n is zero.
    native_inline constexpr void store_partial(T * p, std::size_t n) const noexcept {
      auto a = detail::wasm_lanes(*this);
      for (std::size_t i = 0; i < n; ++i) {
        p[i] = a[i];
      }
    }

    /// Load lane representations from corresponding unsigned words.
    static native_inline constexpr simd load_bits(word_type const * p) noexcept {
      return from_native(bits_type::load(p).to_native());
    }

    /// Store lane representations as corresponding unsigned words.
    native_inline constexpr void store_bits(word_type * p) const noexcept {
      bits().store(p);
    }

    /// Reinterpret each lane as an unsigned word of the same width.
    native_inline constexpr bits_type bits() const noexcept {
      return bits_type::from_native(value_);
    }

    /// Adopt unsigned lane representations without numerical conversion.
    static native_inline constexpr simd from_bits(bits_type x) noexcept {
      return from_native(x.to_native());
    }

    /// Extract the compile-time-selected lane as its scalar element type.
    template<std::size_t I>
    requires(I < N)
    native_inline constexpr T get() const noexcept {
      if consteval {
        return __builtin_bit_cast(std::array<T, N>, value_)[I];
      } else {
        if constexpr (std::same_as<T, float>) {
          return wasm_f32x4_extract_lane(value_, I);
        } else if constexpr (std::same_as<T, double>) {
          return wasm_f64x2_extract_lane(value_, I);
        } else if constexpr (std::same_as<T, std::int8_t>) {
          return wasm_i8x16_extract_lane(value_, I);
        } else if constexpr (std::same_as<T, std::uint8_t>) {
          return wasm_u8x16_extract_lane(value_, I);
        } else if constexpr (std::same_as<T, std::int16_t>) {
          return wasm_i16x8_extract_lane(value_, I);
        } else if constexpr (std::same_as<T, std::uint16_t>) {
          return wasm_u16x8_extract_lane(value_, I);
        } else if constexpr (std::same_as<T, std::int32_t>) {
          return wasm_i32x4_extract_lane(value_, I);
        } else if constexpr (std::same_as<T, std::uint32_t>) {
          return wasm_u32x4_extract_lane(value_, I);
        } else if constexpr (std::same_as<T, std::int64_t>) {
          return wasm_i64x2_extract_lane(value_, I);
        } else if constexpr (std::same_as<T, std::uint64_t>) {
          return wasm_u64x2_extract_lane(value_, I);
        }
      }
    }

    /// Replace one compile-time-selected lane and preserve every other lane.
    template<std::size_t I>
    requires(I < N)
    native_inline constexpr simd replace(T x) const noexcept {
      if consteval {
        auto a = __builtin_bit_cast(std::array<T, N>, value_);
        a[I] = x;
        return load(a.data());
      } else {
        if constexpr (std::same_as<T, float>) {
          return from_native(wasm_f32x4_replace_lane(value_, I, x));
        } else if constexpr (std::same_as<T, double>) {
          return from_native(wasm_f64x2_replace_lane(value_, I, x));
        } else if constexpr (std::same_as<T, std::int8_t>) {
          return from_native(wasm_i8x16_replace_lane(value_, I, x));
        } else if constexpr (std::same_as<T, std::uint8_t>) {
          return from_native(wasm_u8x16_replace_lane(value_, I, x));
        } else if constexpr (std::same_as<T, std::int16_t>) {
          return from_native(wasm_i16x8_replace_lane(value_, I, x));
        } else if constexpr (std::same_as<T, std::uint16_t>) {
          return from_native(wasm_u16x8_replace_lane(value_, I, x));
        } else if constexpr (std::same_as<T, std::int32_t>) {
          return from_native(wasm_i32x4_replace_lane(value_, I, x));
        } else if constexpr (std::same_as<T, std::uint32_t>) {
          return from_native(wasm_u32x4_replace_lane(value_, I, x));
        } else if constexpr (std::same_as<T, std::int64_t>) {
          return from_native(wasm_i64x2_replace_lane(value_, I, x));
        } else if constexpr (std::same_as<T, std::uint64_t>) {
          return from_native(wasm_u64x2_replace_lane(value_, I, x));
        }
      }
    }

    /// Add lanes; integer results wrap and floating results round to nearest-even.
    friend native_inline constexpr simd operator+(simd a, simd b) noexcept {
      if consteval {
        return detail::wasm_map(
          [](T x, T y) {
            if constexpr (std::is_floating_point_v<T>) {
              using format_type = detail::wasm_format<T>;
              return std::bit_cast<T>(detail::constexpr_float::add_bits<format_type>(
                std::bit_cast<word_type>(x), std::bit_cast<word_type>(y)));
            } else {
              return std::bit_cast<T>(
                word_type(std::uint64_t(word_type(x)) + std::uint64_t(word_type(y))));
            }
          },
          a, b);
      } else {
        if constexpr (std::same_as<T, float>) {
          return from_native(wasm_f32x4_add(a.value_, b.value_));
        } else if constexpr (std::same_as<T, double>) {
          return from_native(wasm_f64x2_add(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int8_t>) {
          return from_native(wasm_i8x16_add(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint8_t>) {
          return from_native(wasm_i8x16_add(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int16_t>) {
          return from_native(wasm_i16x8_add(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint16_t>) {
          return from_native(wasm_i16x8_add(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int32_t>) {
          return from_native(wasm_i32x4_add(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint32_t>) {
          return from_native(wasm_i32x4_add(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int64_t>) {
          return from_native(wasm_i64x2_add(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint64_t>) {
          return from_native(wasm_i64x2_add(a.value_, b.value_));
        }
      }
    }

    /// Apply the corresponding lane operation and update this value.
    native_inline constexpr simd & operator+=(simd b) noexcept {
      return *this = *this + b;
    }

    /// Subtract or negate lanes; integer results wrap and floating signs are preserved.
    friend native_inline constexpr simd operator-(simd a, simd b) noexcept {
      if consteval {
        return detail::wasm_map(
          [](T x, T y) {
            if constexpr (std::is_floating_point_v<T>) {
              using format_type = detail::wasm_format<T>;
              return std::bit_cast<T>(detail::constexpr_float::sub_bits<format_type>(
                std::bit_cast<word_type>(x), std::bit_cast<word_type>(y)));
            } else {
              return std::bit_cast<T>(
                word_type(std::uint64_t(word_type(x)) - std::uint64_t(word_type(y))));
            }
          },
          a, b);
      } else {
        if constexpr (std::same_as<T, float>) {
          return from_native(wasm_f32x4_sub(a.value_, b.value_));
        } else if constexpr (std::same_as<T, double>) {
          return from_native(wasm_f64x2_sub(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int8_t>) {
          return from_native(wasm_i8x16_sub(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint8_t>) {
          return from_native(wasm_i8x16_sub(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int16_t>) {
          return from_native(wasm_i16x8_sub(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint16_t>) {
          return from_native(wasm_i16x8_sub(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int32_t>) {
          return from_native(wasm_i32x4_sub(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint32_t>) {
          return from_native(wasm_i32x4_sub(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int64_t>) {
          return from_native(wasm_i64x2_sub(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint64_t>) {
          return from_native(wasm_i64x2_sub(a.value_, b.value_));
        }
      }
    }

    /// Apply the corresponding lane operation and update this value.
    native_inline constexpr simd & operator-=(simd b) noexcept {
      return *this = *this - b;
    }

    /// Multiply lanes; integer results wrap and floating results round to nearest-even.
    friend native_inline constexpr simd operator*(simd a, simd b) noexcept
    requires(sizeof(T) > 1)
    {
      if consteval {
        return detail::wasm_map(
          [](T x, T y) {
            if constexpr (std::is_floating_point_v<T>) {
              using format_type = detail::wasm_format<T>;
              return std::bit_cast<T>(detail::constexpr_float::mul_bits<format_type>(
                std::bit_cast<word_type>(x), std::bit_cast<word_type>(y)));
            } else {
              return std::bit_cast<T>(
                word_type(std::uint64_t(word_type(x)) * std::uint64_t(word_type(y))));
            }
          },
          a, b);
      } else {
        if constexpr (std::same_as<T, float>) {
          return from_native(wasm_f32x4_mul(a.value_, b.value_));
        } else if constexpr (std::same_as<T, double>) {
          return from_native(wasm_f64x2_mul(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int16_t>) {
          return from_native(wasm_i16x8_mul(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint16_t>) {
          return from_native(wasm_i16x8_mul(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int32_t>) {
          return from_native(wasm_i32x4_mul(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint32_t>) {
          return from_native(wasm_i32x4_mul(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int64_t>) {
          return from_native(wasm_i64x2_mul(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint64_t>) {
          return from_native(wasm_i64x2_mul(a.value_, b.value_));
        }
      }
    }

    /// Apply the corresponding lane operation and update this value.
    native_inline constexpr simd & operator*=(simd b) noexcept
    requires(sizeof(T) > 1)
    {
      return *this = *this * b;
    }

    /// Divide floating lanes with WebAssembly IEEE semantics.
    friend native_inline constexpr simd operator/(simd a, simd b) noexcept
    requires std::is_floating_point_v<T>
    {
      if consteval {
        return detail::wasm_map(
          [](T x, T y) {
            using format_type = detail::wasm_format<T>;
            return std::bit_cast<T>(detail::constexpr_float::div_bits<format_type>(
              std::bit_cast<word_type>(x), std::bit_cast<word_type>(y)));
          },
          a, b);
      } else {
        if constexpr (std::same_as<T, float>) {
          return from_native(wasm_f32x4_div(a.value_, b.value_));
        } else if constexpr (std::same_as<T, double>) {
          return from_native(wasm_f64x2_div(a.value_, b.value_));
        }
      }
    }

    /// Apply the corresponding lane operation and update this value.
    native_inline constexpr simd & operator/=(simd b) noexcept
    requires std::is_floating_point_v<T>
    {
      return *this = *this / b;
    }

    /// Intersect representation bits.
    friend native_inline constexpr simd operator&(simd a, simd b) noexcept {
      if consteval {
        auto x = __builtin_bit_cast(std::array<word_type, N>, a.value_);
        auto y = __builtin_bit_cast(std::array<word_type, N>, b.value_);
        for (std::size_t i = 0; i < N; ++i) {
          x[i] &= y[i];
        }
        return from_native(__builtin_bit_cast(native_type, x));
      } else {
        return from_native(wasm_v128_and(a.value_, b.value_));
      }
    }

    /// Apply the corresponding lane operation and update this value.
    native_inline constexpr simd & operator&=(simd b) noexcept {
      return *this = *this & b;
    }

    /// Unite representation bits.
    friend native_inline constexpr simd operator|(simd a, simd b) noexcept {
      if consteval {
        auto x = __builtin_bit_cast(std::array<word_type, N>, a.value_);
        auto y = __builtin_bit_cast(std::array<word_type, N>, b.value_);
        for (std::size_t i = 0; i < N; ++i) {
          x[i] |= y[i];
        }
        return from_native(__builtin_bit_cast(native_type, x));
      } else {
        return from_native(wasm_v128_or(a.value_, b.value_));
      }
    }

    /// Apply the corresponding lane operation and update this value.
    native_inline constexpr simd & operator|=(simd b) noexcept {
      return *this = *this | b;
    }

    /// Exclusive-or representation bits.
    friend native_inline constexpr simd operator^(simd a, simd b) noexcept {
      if consteval {
        auto x = __builtin_bit_cast(std::array<word_type, N>, a.value_);
        auto y = __builtin_bit_cast(std::array<word_type, N>, b.value_);
        for (std::size_t i = 0; i < N; ++i) {
          x[i] ^= y[i];
        }
        return from_native(__builtin_bit_cast(native_type, x));
      } else {
        return from_native(wasm_v128_xor(a.value_, b.value_));
      }
    }

    /// Apply the corresponding lane operation and update this value.
    native_inline constexpr simd & operator^=(simd b) noexcept {
      return *this = *this ^ b;
    }

    /// Complement every representation bit (or lane truth for masks).
    friend native_inline constexpr simd operator~(simd a) noexcept {
      if consteval {
        auto x = __builtin_bit_cast(std::array<word_type, N>, a.value_);
        for (auto & w : x) {
          w = word_type(~w);
        }
        return from_native(__builtin_bit_cast(native_type, x));
      } else {
        return from_native(wasm_v128_not(a.value_));
      }
    }

    /// Subtract or negate lanes; integer results wrap and floating signs are preserved.
    friend native_inline constexpr simd operator-(simd a) noexcept {
      if consteval {
        auto x = __builtin_bit_cast(std::array<word_type, N>, a.value_);
        for (auto & w : x) {
          if constexpr (std::is_floating_point_v<T>) {
            w ^= word_type{1} << (sizeof(T) * 8 - 1);
          } else {
            w = word_type(0 - w);
          }
        }
        return from_native(__builtin_bit_cast(native_type, x));
      } else {
        if constexpr (std::same_as<T, float>) {
          return from_native(wasm_f32x4_neg(a.value_));
        } else if constexpr (std::same_as<T, double>) {
          return from_native(wasm_f64x2_neg(a.value_));
        } else if constexpr (std::same_as<T, std::int8_t>) {
          return from_native(wasm_i8x16_neg(a.value_));
        } else if constexpr (std::same_as<T, std::uint8_t>) {
          return from_native(wasm_i8x16_neg(a.value_));
        } else if constexpr (std::same_as<T, std::int16_t>) {
          return from_native(wasm_i16x8_neg(a.value_));
        } else if constexpr (std::same_as<T, std::uint16_t>) {
          return from_native(wasm_i16x8_neg(a.value_));
        } else if constexpr (std::same_as<T, std::int32_t>) {
          return from_native(wasm_i32x4_neg(a.value_));
        } else if constexpr (std::same_as<T, std::uint32_t>) {
          return from_native(wasm_i32x4_neg(a.value_));
        } else if constexpr (std::same_as<T, std::int64_t>) {
          return from_native(wasm_i64x2_neg(a.value_));
        } else if constexpr (std::same_as<T, std::uint64_t>) {
          return from_native(wasm_i64x2_neg(a.value_));
        }
      }
    }

    /// Compare lane equality and return canonical mask lanes.
    friend native_inline constexpr mask_type operator==(simd a, simd b) noexcept {
      if consteval {
        auto x = detail::wasm_lanes(a), y = detail::wasm_lanes(b);
        std::uint64_t bits = 0;
        for (std::size_t i = 0; i < N; ++i) {
          bits |= std::uint64_t(x[i] == y[i]) << i;
        }
        return mask_type::from_bitset(bits);
      } else {
        if constexpr (std::same_as<T, float>) {
          return mask_type::unsafe_from_native(wasm_f32x4_eq(a.value_, b.value_));
        } else if constexpr (std::same_as<T, double>) {
          return mask_type::unsafe_from_native(wasm_f64x2_eq(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int8_t>) {
          return mask_type::unsafe_from_native(wasm_i8x16_eq(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint8_t>) {
          return mask_type::unsafe_from_native(wasm_i8x16_eq(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int16_t>) {
          return mask_type::unsafe_from_native(wasm_i16x8_eq(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint16_t>) {
          return mask_type::unsafe_from_native(wasm_i16x8_eq(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int32_t>) {
          return mask_type::unsafe_from_native(wasm_i32x4_eq(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint32_t>) {
          return mask_type::unsafe_from_native(wasm_i32x4_eq(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int64_t>) {
          return mask_type::unsafe_from_native(wasm_i64x2_eq(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint64_t>) {
          return mask_type::unsafe_from_native(wasm_i64x2_eq(a.value_, b.value_));
        }
      }
    }

    /// Compare lane inequality and return canonical mask lanes.
    friend native_inline constexpr mask_type operator!=(simd a, simd b) noexcept {
      if consteval {
        auto x = detail::wasm_lanes(a), y = detail::wasm_lanes(b);
        std::uint64_t bits = 0;
        for (std::size_t i = 0; i < N; ++i) {
          bits |= std::uint64_t(x[i] != y[i]) << i;
        }
        return mask_type::from_bitset(bits);
      } else {
        if constexpr (std::same_as<T, float>) {
          return mask_type::unsafe_from_native(wasm_f32x4_ne(a.value_, b.value_));
        } else if constexpr (std::same_as<T, double>) {
          return mask_type::unsafe_from_native(wasm_f64x2_ne(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int8_t>) {
          return mask_type::unsafe_from_native(wasm_i8x16_ne(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint8_t>) {
          return mask_type::unsafe_from_native(wasm_i8x16_ne(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int16_t>) {
          return mask_type::unsafe_from_native(wasm_i16x8_ne(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint16_t>) {
          return mask_type::unsafe_from_native(wasm_i16x8_ne(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int32_t>) {
          return mask_type::unsafe_from_native(wasm_i32x4_ne(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint32_t>) {
          return mask_type::unsafe_from_native(wasm_i32x4_ne(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int64_t>) {
          return mask_type::unsafe_from_native(wasm_i64x2_ne(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint64_t>) {
          return mask_type::unsafe_from_native(wasm_i64x2_ne(a.value_, b.value_));
        }
      }
    }

    /// Compare lanes using their signedness; unordered floating lanes are false.
    friend native_inline constexpr mask_type operator<(simd a, simd b) noexcept {
      if consteval {
        auto x = detail::wasm_lanes(a), y = detail::wasm_lanes(b);
        std::uint64_t bits = 0;
        for (std::size_t i = 0; i < N; ++i) {
          bits |= std::uint64_t(x[i] < y[i]) << i;
        }
        return mask_type::from_bitset(bits);
      } else {
        if constexpr (std::same_as<T, float>) {
          return mask_type::unsafe_from_native(wasm_f32x4_lt(a.value_, b.value_));
        } else if constexpr (std::same_as<T, double>) {
          return mask_type::unsafe_from_native(wasm_f64x2_lt(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int8_t>) {
          return mask_type::unsafe_from_native(wasm_i8x16_lt(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint8_t>) {
          return mask_type::unsafe_from_native(wasm_u8x16_lt(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int16_t>) {
          return mask_type::unsafe_from_native(wasm_i16x8_lt(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint16_t>) {
          return mask_type::unsafe_from_native(wasm_u16x8_lt(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int32_t>) {
          return mask_type::unsafe_from_native(wasm_i32x4_lt(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint32_t>) {
          return mask_type::unsafe_from_native(wasm_u32x4_lt(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int64_t>) {
          return mask_type::unsafe_from_native(wasm_i64x2_lt(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint64_t>) {
          return mask_type::unsafe_from_native(
            wasm_i64x2_lt(wasm_v128_xor(a.value_, wasm_i64x2_splat(INT64_MIN)),
                          wasm_v128_xor(b.value_, wasm_i64x2_splat(INT64_MIN))));
        }
      }
    }

    /// Compare lanes using their signedness; unordered floating lanes are false.
    friend native_inline constexpr mask_type operator<=(simd a, simd b) noexcept {
      if consteval {
        auto x = detail::wasm_lanes(a), y = detail::wasm_lanes(b);
        std::uint64_t bits = 0;
        for (std::size_t i = 0; i < N; ++i) {
          bits |= std::uint64_t(x[i] <= y[i]) << i;
        }
        return mask_type::from_bitset(bits);
      } else {
        if constexpr (std::same_as<T, float>) {
          return mask_type::unsafe_from_native(wasm_f32x4_le(a.value_, b.value_));
        } else if constexpr (std::same_as<T, double>) {
          return mask_type::unsafe_from_native(wasm_f64x2_le(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int8_t>) {
          return mask_type::unsafe_from_native(wasm_i8x16_le(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint8_t>) {
          return mask_type::unsafe_from_native(wasm_u8x16_le(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int16_t>) {
          return mask_type::unsafe_from_native(wasm_i16x8_le(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint16_t>) {
          return mask_type::unsafe_from_native(wasm_u16x8_le(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int32_t>) {
          return mask_type::unsafe_from_native(wasm_i32x4_le(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint32_t>) {
          return mask_type::unsafe_from_native(wasm_u32x4_le(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int64_t>) {
          return mask_type::unsafe_from_native(wasm_i64x2_le(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint64_t>) {
          return mask_type::unsafe_from_native(
            wasm_i64x2_le(wasm_v128_xor(a.value_, wasm_i64x2_splat(INT64_MIN)),
                          wasm_v128_xor(b.value_, wasm_i64x2_splat(INT64_MIN))));
        }
      }
    }

    /// Compare lanes using their signedness; unordered floating lanes are false.
    friend native_inline constexpr mask_type operator>(simd a, simd b) noexcept {
      if consteval {
        auto x = detail::wasm_lanes(a), y = detail::wasm_lanes(b);
        std::uint64_t bits = 0;
        for (std::size_t i = 0; i < N; ++i) {
          bits |= std::uint64_t(x[i] > y[i]) << i;
        }
        return mask_type::from_bitset(bits);
      } else {
        if constexpr (std::same_as<T, float>) {
          return mask_type::unsafe_from_native(wasm_f32x4_gt(a.value_, b.value_));
        } else if constexpr (std::same_as<T, double>) {
          return mask_type::unsafe_from_native(wasm_f64x2_gt(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int8_t>) {
          return mask_type::unsafe_from_native(wasm_i8x16_gt(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint8_t>) {
          return mask_type::unsafe_from_native(wasm_u8x16_gt(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int16_t>) {
          return mask_type::unsafe_from_native(wasm_i16x8_gt(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint16_t>) {
          return mask_type::unsafe_from_native(wasm_u16x8_gt(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int32_t>) {
          return mask_type::unsafe_from_native(wasm_i32x4_gt(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint32_t>) {
          return mask_type::unsafe_from_native(wasm_u32x4_gt(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int64_t>) {
          return mask_type::unsafe_from_native(wasm_i64x2_gt(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint64_t>) {
          return mask_type::unsafe_from_native(
            wasm_i64x2_gt(wasm_v128_xor(a.value_, wasm_i64x2_splat(INT64_MIN)),
                          wasm_v128_xor(b.value_, wasm_i64x2_splat(INT64_MIN))));
        }
      }
    }

    /// Compare lanes using their signedness; unordered floating lanes are false.
    friend native_inline constexpr mask_type operator>=(simd a, simd b) noexcept {
      if consteval {
        auto x = detail::wasm_lanes(a), y = detail::wasm_lanes(b);
        std::uint64_t bits = 0;
        for (std::size_t i = 0; i < N; ++i) {
          bits |= std::uint64_t(x[i] >= y[i]) << i;
        }
        return mask_type::from_bitset(bits);
      } else {
        if constexpr (std::same_as<T, float>) {
          return mask_type::unsafe_from_native(wasm_f32x4_ge(a.value_, b.value_));
        } else if constexpr (std::same_as<T, double>) {
          return mask_type::unsafe_from_native(wasm_f64x2_ge(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int8_t>) {
          return mask_type::unsafe_from_native(wasm_i8x16_ge(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint8_t>) {
          return mask_type::unsafe_from_native(wasm_u8x16_ge(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int16_t>) {
          return mask_type::unsafe_from_native(wasm_i16x8_ge(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint16_t>) {
          return mask_type::unsafe_from_native(wasm_u16x8_ge(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int32_t>) {
          return mask_type::unsafe_from_native(wasm_i32x4_ge(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint32_t>) {
          return mask_type::unsafe_from_native(wasm_u32x4_ge(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::int64_t>) {
          return mask_type::unsafe_from_native(wasm_i64x2_ge(a.value_, b.value_));
        } else if constexpr (std::same_as<T, std::uint64_t>) {
          return mask_type::unsafe_from_native(
            wasm_i64x2_ge(wasm_v128_xor(a.value_, wasm_i64x2_splat(INT64_MIN)),
                          wasm_v128_xor(b.value_, wasm_i64x2_splat(INT64_MIN))));
        }
      }
    }

    /// WebAssembly shifts reduce the count modulo the lane width.
    native_inline constexpr simd left(unsigned count) const noexcept
    requires simd_integer_element<T>
    {
      count %= sizeof(T) * 8;
      if consteval {
        return detail::wasm_map(
          [&](T x) { return std::bit_cast<T>(word_type(std::uint64_t(word_type(x)) << count)); },
          *this);
      } else {
        if constexpr (std::same_as<T, std::int8_t>) {
          return from_native(wasm_i8x16_shl(value_, count));
        } else if constexpr (std::same_as<T, std::uint8_t>) {
          return from_native(wasm_i8x16_shl(value_, count));
        } else if constexpr (std::same_as<T, std::int16_t>) {
          return from_native(wasm_i16x8_shl(value_, count));
        } else if constexpr (std::same_as<T, std::uint16_t>) {
          return from_native(wasm_i16x8_shl(value_, count));
        } else if constexpr (std::same_as<T, std::int32_t>) {
          return from_native(wasm_i32x4_shl(value_, count));
        } else if constexpr (std::same_as<T, std::uint32_t>) {
          return from_native(wasm_i32x4_shl(value_, count));
        } else if constexpr (std::same_as<T, std::int64_t>) {
          return from_native(wasm_i64x2_shl(value_, count));
        } else if constexpr (std::same_as<T, std::uint64_t>) {
          return from_native(wasm_i64x2_shl(value_, count));
        }
      }
    }

    /// Shift lanes left by K modulo the lane width.
    template<std::size_t K>
    native_inline constexpr simd left() const noexcept
    requires simd_integer_element<T>
    {
      return left(K);
    }

    /// Shift integer lanes left, reducing the count modulo the lane width.
    friend native_inline constexpr simd operator<<(simd a, unsigned n) noexcept
    requires simd_integer_element<T>
    {
      return a.left(n);
    }

    /// Shift integer lanes left, reducing the count modulo the lane width.
    template<std::size_t K>
    friend native_inline constexpr simd operator<<(simd a, imm_t<K>) noexcept
    requires simd_integer_element<T>
    {
      return a.left(K);
    }

    /// WebAssembly shifts reduce the count modulo the lane width.
    native_inline constexpr simd right(unsigned count) const noexcept
    requires simd_integer_element<T>
    {
      count %= sizeof(T) * 8;
      if consteval {
        return detail::wasm_map([&](T x) { return T(x >> count); }, *this);
      } else {
        if constexpr (std::same_as<T, std::int8_t>) {
          return from_native(wasm_i8x16_shr(value_, count));
        } else if constexpr (std::same_as<T, std::uint8_t>) {
          return from_native(wasm_u8x16_shr(value_, count));
        } else if constexpr (std::same_as<T, std::int16_t>) {
          return from_native(wasm_i16x8_shr(value_, count));
        } else if constexpr (std::same_as<T, std::uint16_t>) {
          return from_native(wasm_u16x8_shr(value_, count));
        } else if constexpr (std::same_as<T, std::int32_t>) {
          return from_native(wasm_i32x4_shr(value_, count));
        } else if constexpr (std::same_as<T, std::uint32_t>) {
          return from_native(wasm_u32x4_shr(value_, count));
        } else if constexpr (std::same_as<T, std::int64_t>) {
          return from_native(wasm_i64x2_shr(value_, count));
        } else if constexpr (std::same_as<T, std::uint64_t>) {
          return from_native(wasm_u64x2_shr(value_, count));
        }
      }
    }

    /// Shift lanes right by K modulo the lane width.
    template<std::size_t K>
    native_inline constexpr simd right() const noexcept
    requires simd_integer_element<T>
    {
      return right(K);
    }

    /// Shift integer lanes right, extending signed lanes and reducing the count modulo their width.
    friend native_inline constexpr simd operator>>(simd a, unsigned n) noexcept
    requires simd_integer_element<T>
    {
      return a.right(n);
    }

    /// Shift integer lanes right, extending signed lanes and reducing the count modulo their width.
    template<std::size_t K>
    friend native_inline constexpr simd operator>>(simd a, imm_t<K>) noexcept
    requires simd_integer_element<T>
    {
      return a.right(K);
    }
  };

  /// Select full lanes from a where m is true, otherwise from b.
  template<detail::wasm_number T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16)
  native_inline constexpr simd<T, N, A> select(typename simd<T, N, A>::mask_type m, simd<T, N, A> a,
                                               simd<T, N, A> b) noexcept {
    if consteval {
      auto x = detail::wasm_lanes(a), y = detail::wasm_lanes(b);
      auto bits = m.to_bitset();
      for (std::size_t i = 0; i < N; ++i) {
        if (!((bits >> i) & 1)) {
          x[i] = y[i];
        }
      }
      return simd<T, N, A>::load(x.data());
    } else {
      return simd<T, N, A>::from_native(
        wasm_v128_bitselect(a.to_native(), b.to_native(), m.to_native()));
    }
  }

  /// Select individual representation bits using the unsigned mask words.
  template<detail::wasm_number T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16)
  native_inline constexpr simd<T, N, A> bit_select(simd<detail::wasm_word<T>, N, A> m,
                                                   simd<T, N, A> a, simd<T, N, A> b) noexcept {
    return simd<T, N, A>::from_bits((m & a.bits()) | (~m & b.bits()));
  }

  /// Reinterpret canonical mask lanes as corresponding integer words.
  template<class To, class U, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(U) * N == 16) && (sizeof(To) == sizeof(U)) &&
          simd_integer_element<To>
  native_inline constexpr simd<To, N, A> mask_bits(simd<mask_lane<U>, N, A> m) noexcept {
    return simd<To, N, A>::from_native(m.to_native());
  }

  /// Broadcast one compile-time-selected lane.
  template<std::size_t I, detail::wasm_number T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16) && (I < N)
  native_inline constexpr simd<T, N, A> broadcast(simd<T, N, A> v, imm_t<I>) noexcept {
    return simd<T, N, A>(v.template get<I>());
  }

  /// Select N lanes from the concatenation of two inputs, using constant indices.
  template<std::size_t... I, detail::wasm_number T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16) && (sizeof...(I) == N) &&
          ((I < 2 * N) && ...)
  native_inline constexpr simd<T, N, A> shuffle(simd<T, N, A> a, simd<T, N, A> b) noexcept {
    if consteval {
      auto x = detail::wasm_lanes(a), y = detail::wasm_lanes(b);
      std::array<T, N> r{(I < N ? x[I] : y[I - N])...};
      return simd<T, N, A>::load(r.data());
    } else {
      using vector_type = T __attribute__((ext_vector_type(N)));
      return simd<T, N, A>::from_native(__builtin_bit_cast(
        v128_t, __builtin_shufflevector(__builtin_bit_cast(vector_type, a.to_native()),
                                        __builtin_bit_cast(vector_type, b.to_native()), I...)));
    }
  }

  /// Look up byte indices 0..15; every other index produces zero.
  template<isa<> A>
  requires(A.has(wasm_feature::simd128))
  native_inline constexpr simd<std::uint8_t, 16, A>
  swizzle(simd<std::uint8_t, 16, A> v, simd<std::uint8_t, 16, A> indices) noexcept {
    if consteval {
      auto x = detail::wasm_lanes(v), i = detail::wasm_lanes(indices);
      for (auto & n : i) {
        n = n < 16 ? x[n] : 0;
      }
      return simd<std::uint8_t, 16, A>::load(i.data());
    } else {
      return simd<std::uint8_t, 16, A>::from_native(
        wasm_i8x16_swizzle(v.to_native(), indices.to_native()));
    }
  }

  /// Compute correctly rounded square roots; negative finite lanes produce NaN.
  template<std::floating_point T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16)
  native_inline constexpr simd<T, N, A> sqrt(simd<T, N, A> v) noexcept {
    if consteval {
      return detail::wasm_map(
        [](T x) {
          using format_type = detail::wasm_format<T>;
          using bits_type = typename format_type::bits_type;
          auto w = std::bit_cast<bits_type>(x);
          return std::bit_cast<T>(detail::constexpr_float::sqrt_bits<format_type>(w));
        },
        v);
    } else {
      if constexpr (sizeof(T) == 4) {
        return simd<T, N, A>::from_native(wasm_f32x4_sqrt(v.to_native()));
      } else {
        return simd<T, N, A>::from_native(wasm_f64x2_sqrt(v.to_native()));
      }
    }
  }

  /// Round floating lanes toward negative infinity.
  template<std::floating_point T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16)
  native_inline constexpr simd<T, N, A> floor(simd<T, N, A> v) noexcept {
    if consteval {
      return detail::wasm_map(
        [](T x) {
          using format_type = detail::wasm_format<T>;
          using bits_type = typename format_type::bits_type;
          auto w = std::bit_cast<bits_type>(x);
          return std::bit_cast<T>(detail::constexpr_float::round_integral_bits<format_type>(
            w, detail::constexpr_float::rounding::downward));
        },
        v);
    } else {
      if constexpr (sizeof(T) == 4) {
        return simd<T, N, A>::from_native(wasm_f32x4_floor(v.to_native()));
      } else {
        return simd<T, N, A>::from_native(wasm_f64x2_floor(v.to_native()));
      }
    }
  }

  /// Round floating lanes toward positive infinity.
  template<std::floating_point T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16)
  native_inline constexpr simd<T, N, A> ceil(simd<T, N, A> v) noexcept {
    if consteval {
      return detail::wasm_map(
        [](T x) {
          using format_type = detail::wasm_format<T>;
          using bits_type = typename format_type::bits_type;
          auto w = std::bit_cast<bits_type>(x);
          return std::bit_cast<T>(detail::constexpr_float::round_integral_bits<format_type>(
            w, detail::constexpr_float::rounding::upward));
        },
        v);
    } else {
      if constexpr (sizeof(T) == 4) {
        return simd<T, N, A>::from_native(wasm_f32x4_ceil(v.to_native()));
      } else {
        return simd<T, N, A>::from_native(wasm_f64x2_ceil(v.to_native()));
      }
    }
  }

  /// Round floating lanes toward zero.
  template<std::floating_point T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16)
  native_inline constexpr simd<T, N, A> trunc(simd<T, N, A> v) noexcept {
    if consteval {
      return detail::wasm_map(
        [](T x) {
          using format_type = detail::wasm_format<T>;
          using bits_type = typename format_type::bits_type;
          auto w = std::bit_cast<bits_type>(x);
          return std::bit_cast<T>(detail::constexpr_float::round_integral_bits<format_type>(
            w, detail::constexpr_float::rounding::toward_zero));
        },
        v);
    } else {
      if constexpr (sizeof(T) == 4) {
        return simd<T, N, A>::from_native(wasm_f32x4_trunc(v.to_native()));
      } else {
        return simd<T, N, A>::from_native(wasm_f64x2_trunc(v.to_native()));
      }
    }
  }

  /// Round floating lanes to nearest integers, choosing even at ties.
  template<std::floating_point T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16)
  native_inline constexpr simd<T, N, A> round_even(simd<T, N, A> v) noexcept {
    if consteval {
      return detail::wasm_map(
        [](T x) {
          using format_type = detail::wasm_format<T>;
          using bits_type = typename format_type::bits_type;
          auto w = std::bit_cast<bits_type>(x);
          return std::bit_cast<T>(detail::constexpr_float::round_integral_bits<format_type>(w));
        },
        v);
    } else {
      if constexpr (sizeof(T) == 4) {
        return simd<T, N, A>::from_native(wasm_f32x4_nearest(v.to_native()));
      } else {
        return simd<T, N, A>::from_native(wasm_f64x2_nearest(v.to_native()));
      }
    }
  }

  /// Clear the floating sign bit without changing payload bits.
  template<std::floating_point T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16)
  native_inline constexpr simd<T, N, A> abs(simd<T, N, A> v) noexcept {
    if consteval {
      return detail::wasm_map(
        [](T x) {
          using format_type = detail::wasm_format<T>;
          using bits_type = typename format_type::bits_type;
          auto w = std::bit_cast<bits_type>(x);
          return std::bit_cast<T>(bits_type(w & ~format_type::sign_mask));
        },
        v);
    } else {
      if constexpr (sizeof(T) == 4) {
        return simd<T, N, A>::from_native(wasm_f32x4_abs(v.to_native()));
      } else {
        return simd<T, N, A>::from_native(wasm_f64x2_abs(v.to_native()));
      }
    }
  }
} // namespace native
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// SIMD128 instruction families, included in the SIMD128 target scope.
namespace native {
  namespace detail {
    template<class T>
    constexpr T wasm_saturate(std::int64_t x) noexcept {
      if (x < std::int64_t(std::numeric_limits<T>::min())) {
        return std::numeric_limits<T>::min();
      }
      if (x > std::int64_t(std::numeric_limits<T>::max())) {
        return std::numeric_limits<T>::max();
      }
      return T(x);
    }

    template<class To, class From>
    constexpr To wasm_trunc_sat(From x) noexcept {
      if (x != x) {
        return 0;
      }
      if (x <= double(std::numeric_limits<To>::min())) {
        return std::numeric_limits<To>::min();
      }
      if (x >= double(std::numeric_limits<To>::max())) {
        return std::numeric_limits<To>::max();
      }
      return To(x);
    }
  } // namespace detail

  /// Saturate signed or unsigned byte/halfword arithmetic at the lane limits.
  template<simd_integer_element T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16 && sizeof(T) <= 2)
  native_inline constexpr simd<T, N, A> add_sat(simd<T, N, A> a, simd<T, N, A> b) noexcept {
    using vector_type = simd<T, N, A>;
    if consteval {
      return detail::wasm_map(
        [](T x, T y) { return detail::wasm_saturate<T>(std::int64_t(x) + std::int64_t(y)); }, a, b);
    } else {
      if constexpr (std::same_as<T, std::int8_t>) {
        return vector_type::from_native(wasm_i8x16_add_sat(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<T, std::uint8_t>) {
        return vector_type::from_native(wasm_u8x16_add_sat(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<T, std::int16_t>) {
        return vector_type::from_native(wasm_i16x8_add_sat(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<T, std::uint16_t>) {
        return vector_type::from_native(wasm_u16x8_add_sat(a.to_native(), b.to_native()));
      }
    }
  }

  /// Saturate signed or unsigned byte/halfword arithmetic at the lane limits.
  template<simd_integer_element T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16 && sizeof(T) <= 2)
  native_inline constexpr simd<T, N, A> sub_sat(simd<T, N, A> a, simd<T, N, A> b) noexcept {
    using vector_type = simd<T, N, A>;
    if consteval {
      return detail::wasm_map(
        [](T x, T y) { return detail::wasm_saturate<T>(std::int64_t(x) - std::int64_t(y)); }, a, b);
    } else {
      if constexpr (std::same_as<T, std::int8_t>) {
        return vector_type::from_native(wasm_i8x16_sub_sat(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<T, std::uint8_t>) {
        return vector_type::from_native(wasm_u8x16_sub_sat(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<T, std::int16_t>) {
        return vector_type::from_native(wasm_i16x8_sub_sat(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<T, std::uint16_t>) {
        return vector_type::from_native(wasm_u16x8_sub_sat(a.to_native(), b.to_native()));
      }
    }
  }

  /// Rounded unsigned average: (a+b+1)/2 without intermediate overflow.
  template<simd_integer_element T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) &&
          (std::is_unsigned_v<T> && sizeof(T) * N == 16 && sizeof(T) <= 2)
  native_inline constexpr simd<T, N, A> average_round(simd<T, N, A> a, simd<T, N, A> b) noexcept {
    if consteval {
      return detail::wasm_map([](T x, T y) { return T((unsigned(x) + y + 1) / 2); }, a, b);
    } else {
      if constexpr (sizeof(T) == 1) {
        return simd<T, N, A>::from_native(wasm_u8x16_avgr(a.to_native(), b.to_native()));
      } else {
        return simd<T, N, A>::from_native(wasm_u16x8_avgr(a.to_native(), b.to_native()));
      }
    }
  }

  /// Absolute value with the minimum signed lane retaining its representation.
  template<simd_integer_element T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (std::is_signed_v<T> && sizeof(T) * N == 16)
  native_inline constexpr simd<T, N, A> abs(simd<T, N, A> a) noexcept {
    using vector_type = simd<T, N, A>;
    using unsigned_type = std::make_unsigned_t<T>;
    if consteval {
      return detail::wasm_map(
        [](T x) { return x < 0 ? std::bit_cast<T>(unsigned_type(0 - unsigned_type(x))) : x; }, a);
    } else {
      if constexpr (sizeof(T) == 1) {
        return vector_type::from_native(wasm_i8x16_abs(a.to_native()));
      } else if constexpr (sizeof(T) == 2) {
        return vector_type::from_native(wasm_i16x8_abs(a.to_native()));
      } else if constexpr (sizeof(T) == 4) {
        return vector_type::from_native(wasm_i32x4_abs(a.to_native()));
      } else if constexpr (sizeof(T) == 8) {
        return vector_type::from_native(wasm_i64x2_abs(a.to_native()));
      }
    }
  }

  /// Minimum/maximum; floating NaNs propagate and signed zeros follow WebAssembly rules.
  template<detail::wasm_number T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16)
  native_inline constexpr simd<T, N, A> min(simd<T, N, A> a, simd<T, N, A> b) noexcept {
    using vector_type = simd<T, N, A>;
    if consteval {
      return detail::wasm_map(
        [](T x, T y) {
          if constexpr (std::is_floating_point_v<T>) {
            using format_type = detail::wasm_format<T>;
            using bits_type = typename format_type::bits_type;
            auto xx = std::bit_cast<bits_type>(x), yy = std::bit_cast<bits_type>(y);
            if (detail::constexpr_float::is_nan<format_type>(xx) ||
                detail::constexpr_float::is_nan<format_type>(yy)) {
              return std::bit_cast<T>(detail::constexpr_float::default_nan<format_type>({}));
            }
            if (detail::constexpr_float::is_zero<format_type>(xx) &&
                detail::constexpr_float::is_zero<format_type>(yy)) {
              return std::bit_cast<T>(bits_type(xx | yy));
            }
          }
          return y < x ? y : x;
        },
        a, b);
    } else {
      if constexpr (std::same_as<T, float>) {
        return vector_type::from_native(wasm_f32x4_min(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<T, double>) {
        return vector_type::from_native(wasm_f64x2_min(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<T, std::int8_t>) {
        return vector_type::from_native(wasm_i8x16_min(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<T, std::uint8_t>) {
        return vector_type::from_native(wasm_u8x16_min(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<T, std::int16_t>) {
        return vector_type::from_native(wasm_i16x8_min(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<T, std::uint16_t>) {
        return vector_type::from_native(wasm_u16x8_min(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<T, std::int32_t>) {
        return vector_type::from_native(wasm_i32x4_min(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<T, std::uint32_t>) {
        return vector_type::from_native(wasm_u32x4_min(a.to_native(), b.to_native()));
      } else {
        return select(a < b, a, b);
      }
    }
  }

  /// Minimum/maximum; floating NaNs propagate and signed zeros follow WebAssembly rules.
  template<detail::wasm_number T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16)
  native_inline constexpr simd<T, N, A> max(simd<T, N, A> a, simd<T, N, A> b) noexcept {
    using vector_type = simd<T, N, A>;
    if consteval {
      return detail::wasm_map(
        [](T x, T y) {
          if constexpr (std::is_floating_point_v<T>) {
            using format_type = detail::wasm_format<T>;
            using bits_type = typename format_type::bits_type;
            auto xx = std::bit_cast<bits_type>(x), yy = std::bit_cast<bits_type>(y);
            if (detail::constexpr_float::is_nan<format_type>(xx) ||
                detail::constexpr_float::is_nan<format_type>(yy)) {
              return std::bit_cast<T>(detail::constexpr_float::default_nan<format_type>({}));
            }
            if (detail::constexpr_float::is_zero<format_type>(xx) &&
                detail::constexpr_float::is_zero<format_type>(yy)) {
              return std::bit_cast<T>(bits_type(xx & yy));
            }
          }
          return y > x ? y : x;
        },
        a, b);
    } else {
      if constexpr (std::same_as<T, float>) {
        return vector_type::from_native(wasm_f32x4_max(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<T, double>) {
        return vector_type::from_native(wasm_f64x2_max(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<T, std::int8_t>) {
        return vector_type::from_native(wasm_i8x16_max(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<T, std::uint8_t>) {
        return vector_type::from_native(wasm_u8x16_max(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<T, std::int16_t>) {
        return vector_type::from_native(wasm_i16x8_max(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<T, std::uint16_t>) {
        return vector_type::from_native(wasm_u16x8_max(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<T, std::int32_t>) {
        return vector_type::from_native(wasm_i32x4_max(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<T, std::uint32_t>) {
        return vector_type::from_native(wasm_u32x4_max(a.to_native(), b.to_native()));
      } else {
        return select(a > b, a, b);
      }
    }
  }

  /// Pseudo minimum/maximum selects the first operand for unordered or equal lanes.
  template<detail::wasm_number T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16 && std::is_floating_point_v<T>)
  native_inline constexpr simd<T, N, A> pmin(simd<T, N, A> a, simd<T, N, A> b) noexcept {
    using vector_type = simd<T, N, A>;
    if consteval {
      return detail::wasm_map([](T x, T y) { return y < x ? y : x; }, a, b);
    } else {
      if constexpr (std::same_as<T, float>) {
        return vector_type::from_native(wasm_f32x4_pmin(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<T, double>) {
        return vector_type::from_native(wasm_f64x2_pmin(a.to_native(), b.to_native()));
      }
    }
  }

  /// Pseudo minimum/maximum selects the first operand for unordered or equal lanes.
  template<detail::wasm_number T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16 && std::is_floating_point_v<T>)
  native_inline constexpr simd<T, N, A> pmax(simd<T, N, A> a, simd<T, N, A> b) noexcept {
    using vector_type = simd<T, N, A>;
    if consteval {
      return detail::wasm_map([](T x, T y) { return y > x ? y : x; }, a, b);
    } else {
      if constexpr (std::same_as<T, float>) {
        return vector_type::from_native(wasm_f32x4_pmax(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<T, double>) {
        return vector_type::from_native(wasm_f64x2_pmax(a.to_native(), b.to_native()));
      }
    }
  }

  /// Widen the lower half of integer lanes, preserving signedness.
  template<simd_integer_element T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16 && sizeof(T) <= 4)
  native_inline constexpr auto extend_low(simd<T, N, A> a) noexcept {
    using wide_unsigned_type =
      std::conditional_t<sizeof(T) == 1, std::uint16_t,
                         std::conditional_t<sizeof(T) == 2, std::uint32_t, std::uint64_t>>;
    using wide_type =
      std::conditional_t<std::is_signed_v<T>, std::make_signed_t<wide_unsigned_type>,
                         wide_unsigned_type>;
    using vector_type = simd<wide_type, N / 2, A>;
    if consteval {
      auto x = detail::wasm_lanes(a);
      std::array<wide_type, N / 2> r{};
      for (std::size_t i = 0; i < N / 2; ++i) {
        r[i] = x[i];
      }
      return vector_type::load(r.data());
    } else {
      if constexpr (std::same_as<T, std::int8_t>) {
        return vector_type::from_native(wasm_i16x8_extend_low_i8x16(a.to_native()));
      } else if constexpr (std::same_as<T, std::uint8_t>) {
        return vector_type::from_native(wasm_u16x8_extend_low_u8x16(a.to_native()));
      } else if constexpr (std::same_as<T, std::int16_t>) {
        return vector_type::from_native(wasm_i32x4_extend_low_i16x8(a.to_native()));
      } else if constexpr (std::same_as<T, std::uint16_t>) {
        return vector_type::from_native(wasm_u32x4_extend_low_u16x8(a.to_native()));
      } else if constexpr (std::same_as<T, std::int32_t>) {
        return vector_type::from_native(wasm_i64x2_extend_low_i32x4(a.to_native()));
      } else if constexpr (std::same_as<T, std::uint32_t>) {
        return vector_type::from_native(wasm_u64x2_extend_low_u32x4(a.to_native()));
      }
    }
  }

  /// Multiply widened lower integer lanes; the complete product fits.
  template<simd_integer_element T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16 && sizeof(T) <= 4)
  native_inline constexpr auto multiply_widened_low(simd<T, N, A> a, simd<T, N, A> b) noexcept {
    return extend_low(a) * extend_low(b);
  }

  /// Widen the upper half of integer lanes, preserving signedness.
  template<simd_integer_element T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16 && sizeof(T) <= 4)
  native_inline constexpr auto extend_high(simd<T, N, A> a) noexcept {
    using wide_unsigned_type =
      std::conditional_t<sizeof(T) == 1, std::uint16_t,
                         std::conditional_t<sizeof(T) == 2, std::uint32_t, std::uint64_t>>;
    using wide_type =
      std::conditional_t<std::is_signed_v<T>, std::make_signed_t<wide_unsigned_type>,
                         wide_unsigned_type>;
    using vector_type = simd<wide_type, N / 2, A>;
    if consteval {
      auto x = detail::wasm_lanes(a);
      std::array<wide_type, N / 2> r{};
      for (std::size_t i = 0; i < N / 2; ++i) {
        r[i] = x[i + N / 2];
      }
      return vector_type::load(r.data());
    } else {
      if constexpr (std::same_as<T, std::int8_t>) {
        return vector_type::from_native(wasm_i16x8_extend_high_i8x16(a.to_native()));
      } else if constexpr (std::same_as<T, std::uint8_t>) {
        return vector_type::from_native(wasm_u16x8_extend_high_u8x16(a.to_native()));
      } else if constexpr (std::same_as<T, std::int16_t>) {
        return vector_type::from_native(wasm_i32x4_extend_high_i16x8(a.to_native()));
      } else if constexpr (std::same_as<T, std::uint16_t>) {
        return vector_type::from_native(wasm_u32x4_extend_high_u16x8(a.to_native()));
      } else if constexpr (std::same_as<T, std::int32_t>) {
        return vector_type::from_native(wasm_i64x2_extend_high_i32x4(a.to_native()));
      } else if constexpr (std::same_as<T, std::uint32_t>) {
        return vector_type::from_native(wasm_u64x2_extend_high_u32x4(a.to_native()));
      }
    }
  }

  /// Multiply widened upper integer lanes; the complete product fits.
  template<simd_integer_element T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16 && sizeof(T) <= 4)
  native_inline constexpr auto multiply_widened_high(simd<T, N, A> a, simd<T, N, A> b) noexcept {
    return extend_high(a) * extend_high(b);
  }

  /// Saturating concatenate from signed source lanes, including unsigned destinations.
  template<simd_integer_element To, simd_integer_element From, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) &&
          (sizeof(From) * N == 16 && sizeof(From) == 2 * sizeof(To) && sizeof(To) <= 2 &&
           std::is_signed_v<From>)
  native_inline constexpr simd<To, N * 2, A> narrow_sat(simd<From, N, A> a,
                                                        simd<From, N, A> b) noexcept {
    using vector_type = simd<To, N * 2, A>;
    if consteval {
      auto x = detail::wasm_lanes(a), y = detail::wasm_lanes(b);
      std::array<To, N * 2> r{};
      for (std::size_t i = 0; i < N; ++i) {
        r[i] = detail::wasm_saturate<To>(x[i]);
        r[i + N] = detail::wasm_saturate<To>(y[i]);
      }
      return vector_type::load(r.data());
    } else {
      if constexpr (std::same_as<To, std::int8_t>) {
        return vector_type::from_native(wasm_i8x16_narrow_i16x8(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<To, std::uint8_t>) {
        return vector_type::from_native(wasm_u8x16_narrow_i16x8(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<To, std::int16_t>) {
        return vector_type::from_native(wasm_i16x8_narrow_i32x4(a.to_native(), b.to_native()));
      } else if constexpr (std::same_as<To, std::uint16_t>) {
        return vector_type::from_native(wasm_u16x8_narrow_i32x4(a.to_native(), b.to_native()));
      }
    }
  }

  /// Multiply signed Q15 lanes, round by adding 2^14, shift by 15 and saturate.
  template<isa<> A>
  requires(A.has(wasm_feature::simd128))
  native_inline constexpr simd<std::int16_t, 8, A>
  q15mulr_sat(simd<std::int16_t, 8, A> a, simd<std::int16_t, 8, A> b) noexcept {
    if consteval {
      return detail::wasm_map(
        [](std::int16_t x, std::int16_t y) {
          return detail::wasm_saturate<std::int16_t>((std::int64_t(x) * y + 16384) >> 15);
        },
        a, b);
    } else {
      return simd<std::int16_t, 8, A>::from_native(
        wasm_i16x8_q15mulr_sat(a.to_native(), b.to_native()));
    }
  }

  /// Sum adjacent signed halfword products modulo 2^32.
  template<isa<> A>
  requires(A.has(wasm_feature::simd128))
  native_inline constexpr simd<std::int32_t, 4, A> dot(simd<std::int16_t, 8, A> a,
                                                       simd<std::int16_t, 8, A> b) noexcept {
    using vector_type = simd<std::int32_t, 4, A>;
    if consteval {
      auto x = detail::wasm_lanes(a), y = detail::wasm_lanes(b);
      std::array<std::int32_t, 4> r{};
      for (std::size_t i = 0; i < 4; ++i) {
        r[i] = std::bit_cast<std::int32_t>(std::uint32_t(
          std::int64_t(x[2 * i]) * y[2 * i] + std::int64_t(x[2 * i + 1]) * y[2 * i + 1]));
      }
      return vector_type::load(r.data());
    } else {
      return vector_type::from_native(wasm_i32x4_dot_i16x8(a.to_native(), b.to_native()));
    }
  }

  /// Saturating floating-to-integer conversion; NaN becomes zero. Binary64 leaves the upper two
  /// result lanes zero.
  template<simd_integer_element To, std::floating_point From, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(From) * N == 16 && sizeof(To) == 4)
  native_inline constexpr simd<To, 4, A> trunc_sat(simd<From, N, A> a) noexcept {
    using vector_type = simd<To, 4, A>;
    if consteval {
      auto x = detail::wasm_lanes(a);
      std::array<To, 4> r{};
      for (std::size_t i = 0; i < N; ++i) {
        r[i] = detail::wasm_trunc_sat<To>(x[i]);
      }
      return vector_type::load(r.data());
    } else {
      if constexpr (sizeof(From) == 4 && std::is_signed_v<To>) {
        return vector_type::from_native(wasm_i32x4_trunc_sat_f32x4(a.to_native()));
      } else if constexpr (sizeof(From) == 4) {
        return vector_type::from_native(wasm_u32x4_trunc_sat_f32x4(a.to_native()));
      } else if constexpr (std::is_signed_v<To>) {
        return vector_type::from_native(wasm_i32x4_trunc_sat_f64x2_zero(a.to_native()));
      } else {
        return vector_type::from_native(wasm_u32x4_trunc_sat_f64x2_zero(a.to_native()));
      }
    }
  }

  /// Native integer-to-float or float-width conversion. Double results consume the low two source
  /// lanes; float demotion zeroes its upper two lanes.
  template<std::floating_point To, detail::wasm_number From, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) &&
          (sizeof(From) * N == 16 && (sizeof(From) == 4 || std::is_floating_point_v<From>) &&
           !std::same_as<To, From>)
  native_inline constexpr simd<To, 16 / sizeof(To), A> convert(simd<From, N, A> a) noexcept {
    using vector_type = simd<To, 16 / sizeof(To), A>;
    if consteval {
      auto x = detail::wasm_lanes(a);
      std::array<To, vector_type::lanes> r{};
      for (std::size_t i = 0; i < std::min(N, vector_type::lanes); ++i) {
        if constexpr (std::is_floating_point_v<From>) {
          using format_type = detail::wasm_format<From>;
          using result_format = detail::wasm_format<To>;
          r[i] =
            std::bit_cast<To>(detail::constexpr_float::convert_bits<result_format, format_type>(
              std::bit_cast<typename format_type::bits_type>(x[i])));
        } else {
          r[i] = To(x[i]);
        }
      }
      return vector_type::load(r.data());
    } else {
      if constexpr (std::same_as<From, float>) {
        return vector_type::from_native(wasm_f64x2_promote_low_f32x4(a.to_native()));
      } else if constexpr (std::same_as<From, double>) {
        return vector_type::from_native(wasm_f32x4_demote_f64x2_zero(a.to_native()));
      } else if constexpr (sizeof(To) == 4 && std::is_signed_v<From>) {
        return vector_type::from_native(wasm_f32x4_convert_i32x4(a.to_native()));
      } else if constexpr (sizeof(To) == 4) {
        return vector_type::from_native(wasm_f32x4_convert_u32x4(a.to_native()));
      } else if constexpr (std::is_signed_v<From>) {
        return vector_type::from_native(wasm_f64x2_convert_low_i32x4(a.to_native()));
      } else {
        return vector_type::from_native(wasm_f64x2_convert_low_u32x4(a.to_native()));
      }
    }
  }

  /// Read one scalar and broadcast it; the access is exactly sizeof(T) bytes.
  template<class V>
  requires detail::wasm_number<typename V::value_type> &&
           (V::architecture.has(wasm_feature::simd128)) &&
           (V::lanes * sizeof(typename V::value_type) == 16)
  native_inline constexpr V load_splat(typename V::value_type const * p) noexcept {
    return V(*p);
  }

  /// Read one scalar into lane I, preserving the other lanes.
  template<std::size_t I, detail::wasm_number T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16 && I < N)
  native_inline constexpr simd<T, N, A> load_lane(T const * p, simd<T, N, A> a) noexcept {
    return a.template replace<I>(*p);
  }

  /// Write only lane I to one scalar object.
  template<std::size_t I, detail::wasm_number T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16 && I < N)
  native_inline constexpr void store_lane(T * p, simd<T, N, A> a) noexcept {
    *p = a.template get<I>();
  }

  /// Read one 32- or 64-bit lane and zero the other lanes.
  template<class V>
  requires(V::architecture.has(wasm_feature::simd128)) &&
          (V::lanes * sizeof(typename V::value_type) == 16 && sizeof(typename V::value_type) >= 4)
  native_inline constexpr V load_zero(typename V::value_type const * p) noexcept {
    if consteval {
      std::array<typename V::value_type, V::lanes> a{};
      a[0] = *p;
      return V::load(a.data());
    } else {
      if constexpr (sizeof(typename V::value_type) == 4) {
        return V::from_native(wasm_v128_load32_zero(p));
      } else {
        return V::from_native(wasm_v128_load64_zero(p));
      }
    }
  }

  /// Read exactly eight bytes of source lanes and widen them with their signedness.
  template<simd_integer_element To, simd_integer_element From, isa<> A>
  requires(A.has(wasm_feature::simd128)) &&
          (sizeof(To) == 2 * sizeof(From) && std::is_signed_v<To> == std::is_signed_v<From>)
  native_inline constexpr simd<To, 16 / sizeof(To), A> load_widened(From const * p) noexcept {
    using vector_type = simd<To, 16 / sizeof(To), A>;
    if consteval {
      std::array<To, vector_type::lanes> a{};
      for (std::size_t i = 0; i < vector_type::lanes; ++i) {
        a[i] = p[i];
      }
      return vector_type::load(a.data());
    } else {
      if constexpr (std::same_as<To, std::int16_t>) {
        return vector_type::from_native(wasm_i16x8_load8x8(p));
      } else if constexpr (std::same_as<To, std::uint16_t>) {
        return vector_type::from_native(wasm_u16x8_load8x8(p));
      } else if constexpr (std::same_as<To, std::int32_t>) {
        return vector_type::from_native(wasm_i32x4_load16x4(p));
      } else if constexpr (std::same_as<To, std::uint32_t>) {
        return vector_type::from_native(wasm_u32x4_load16x4(p));
      } else if constexpr (std::same_as<To, std::int64_t>) {
        return vector_type::from_native(wasm_i64x2_load32x2(p));
      } else if constexpr (std::same_as<To, std::uint64_t>) {
        return vector_type::from_native(wasm_u64x2_load32x2(p));
      }
    }
  }

  /// Signed widening pairwise sums; unsigned overloads are shared with the integer API.
  template<simd_integer_element T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) &&
          (std::is_signed_v<T> && sizeof(T) * N == 16 && sizeof(T) <= 2)
  native_inline constexpr auto pairwise_add_widened(simd<T, N, A> a) noexcept {
    using wide_type = std::conditional_t<sizeof(T) == 1, std::int16_t, std::int32_t>;
    using vector_type = simd<wide_type, N / 2, A>;
    if consteval {
      auto x = detail::wasm_lanes(a);
      std::array<wide_type, N / 2> r{};
      for (std::size_t i = 0; i < N / 2; ++i) {
        r[i] = wide_type(x[2 * i]) + x[2 * i + 1];
      }
      return vector_type::load(r.data());
    } else {
      if constexpr (sizeof(T) == 1) {
        return vector_type::from_native(wasm_i16x8_extadd_pairwise_i8x16(a.to_native()));
      } else {
        return vector_type::from_native(wasm_i32x4_extadd_pairwise_i16x8(a.to_native()));
      }
    }
  }

  /// Gather each integer lane's sign bit into bit i of the scalar result.
  template<simd_integer_element T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16)
  native_inline constexpr std::uint32_t bitmask(simd<T, N, A> a) noexcept {
    if consteval {
      auto x = detail::wasm_lanes(a);
      std::uint32_t r = 0;
      for (std::size_t i = 0; i < N; ++i) {
        r |= std::uint32_t(std::make_unsigned_t<T>(x[i]) >> (sizeof(T) * 8 - 1)) << i;
      }
      return r;
    } else {
      if constexpr (sizeof(T) == 1) {
        return wasm_i8x16_bitmask(a.to_native());
      } else if constexpr (sizeof(T) == 2) {
        return wasm_i16x8_bitmask(a.to_native());
      } else if constexpr (sizeof(T) == 4) {
        return wasm_i32x4_bitmask(a.to_native());
      } else {
        return wasm_i64x2_bitmask(a.to_native());
      }
    }
  }

  /// Test whether at least one integer lane is nonzero.
  template<simd_integer_element T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16)
  native_inline constexpr bool any(simd<T, N, A> a) noexcept {
    if consteval {
      for (auto x : detail::wasm_lanes(a)) {
        if (x) {
          return true;
        }
      }
      return false;
    } else {
      return wasm_v128_any_true(a.to_native());
    }
  }

  /// Test whether every integer lane is nonzero.
  template<simd_integer_element T, std::size_t N, isa<> A>
  requires(A.has(wasm_feature::simd128)) && (sizeof(T) * N == 16)
  native_inline constexpr bool all(simd<T, N, A> a) noexcept {
    if consteval {
      for (auto x : detail::wasm_lanes(a)) {
        if (!x) {
          return false;
        }
      }
      return true;
    } else {
      if constexpr (sizeof(T) == 1) {
        return wasm_i8x16_all_true(a.to_native());
      } else if constexpr (sizeof(T) == 2) {
        return wasm_i16x8_all_true(a.to_native());
      } else if constexpr (sizeof(T) == 4) {
        return wasm_i32x4_all_true(a.to_native());
      } else {
        return wasm_i64x2_all_true(a.to_native());
      }
    }
  }

} // namespace native
#endif
