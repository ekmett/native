#pragma once
#include "simd/simd/common.h"
#include <bit>
#include <cstring>
#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#elif defined(__aarch64__) || defined(_M_ARM64)
#include <arm_neon.h>
#endif

namespace simd {
  namespace detail {
    template<class Arch> concept native_short_arch = std::same_as<Arch,avx2> ||
      std::same_as<Arch,avx512> || std::same_as<Arch,neon>;
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
  template<detail::short_element T,std::size_t N,detail::native_short_arch Arch>
    requires(N==2 || N==3) && requires { typename vec<T,4,Arch>::native_type; }
  struct vec<T,N,Arch> : detail::swizzle_access<T,N,Arch> {
    using value_type=T;
    using architecture=Arch;
    using storage_type=vec<T,4,Arch>;
    using native_type=detail::short_native<detail::short_lane<T>>;
    using register_type=vec;
    using unsigned_register_tag=void;
    using bits_type=vec<std::uint32_t,N,Arch>;
    using vector_mask_type=vec<mask32,N,Arch>;
    using mask_type=std::conditional_t<simd_mask_element<T>,vec,
      std::conditional_t<std::same_as<Arch,avx512>,predicate<N,Arch>,vector_mask_type>>;
    using mask=mask_type;
    using predicate_type=predicate<N,Arch>;
    template<class U> using rebind=vec<U,N,Arch>;
    static constexpr std::size_t lanes=N;
    static constexpr std::size_t storage_lanes=4;
    static constexpr bool compact=false;
    static constexpr std::uint64_t lane_mask=(std::uint64_t(1)<<N)-1;
    native_type value;

    simd_inline constexpr vec() = default;
    simd_inline constexpr vec(vec const &) = default;
    simd_inline constexpr vec & operator=(vec const &) = default;
    simd_inline constexpr vec(T x) noexcept : value{detail::short_word(x),detail::short_word(x),
      N==3?detail::short_word(x):detail::short_lane<T>(0),0} {}
    explicit simd_inline constexpr vec(bool x) noexcept requires simd_mask_element<T> : vec(T(x)) {}
    simd_inline constexpr vec(native_type x) noexcept
      : value(__builtin_shufflevector(x,native_type{},0,1,N==3?2:4,4)) {}
    template<class... X> requires(sizeof...(X)==N) && (std::convertible_to<X,T> && ...)
    simd_inline constexpr vec(X... x) noexcept((noexcept(static_cast<T>(x)) && ...))
      : value{detail::short_word(static_cast<T>(x))...} {}
    template<class... X> requires std::constructible_from<vec,X...>
    simd_inline constexpr vec(Arch,X &&... x) noexcept(std::is_nothrow_constructible_v<vec,X...>)
      : vec(std::forward<X>(x)...) {}
    simd_inline vec(std::array<T,N> const & values) noexcept : vec(load(values.data())) {}

    simd_nodiscard simd_inline constexpr native_type to_native() const noexcept { return value; }
    simd_nodiscard simd_inline constexpr operator native_type() const noexcept requires(!simd_mask_element<T>) { return value; }
    simd_nodiscard static simd_inline vec from_native(native_type x) noexcept {
      if constexpr(simd_mask_element<T>) return from_storage(storage_type::from_native(std::bit_cast<typename storage_type::native_type>(x)));
      else return vec(x);
    }
    simd_nodiscard static simd_inline constexpr vec unsafe_from_native(native_type x) noexcept { return vec(x); }
    simd_nodiscard simd_inline storage_type to_storage() const noexcept {
      if constexpr(simd_mask_element<T>) return storage_type::unsafe_from_native(std::bit_cast<typename storage_type::native_type>(value));
      else return storage_type::from_native(std::bit_cast<typename storage_type::native_type>(value));
    }
    simd_nodiscard static simd_inline vec from_storage(storage_type x) noexcept {
      return vec(std::bit_cast<native_type>(x.to_native()));
    }
    // The caller supplies exactly N logical lanes; alignment never grants a
    // readable fourth lane. Three-lane x86 transfers use native masked memory.
    template<std::size_t Alignment=1>
    simd_nodiscard static simd_inline vec load_memory(T const * p) noexcept {
#if defined(__x86_64__) || defined(_M_X64)
      if constexpr(std::same_as<Arch,avx2> || std::same_as<Arch,avx512>) {
        if constexpr(N==2) return vec(unchecked{},std::bit_cast<native_type>(_mm_loadl_epi64(reinterpret_cast<__m128i const *>(p))));
        else if constexpr(std::same_as<Arch,avx512>) {
          if constexpr(std::same_as<T,float>) return vec(unchecked{},std::bit_cast<native_type>(_mm_maskz_loadu_ps(7,p)));
          else return vec(unchecked{},std::bit_cast<native_type>(_mm_maskz_loadu_epi32(7,p)));
        } else {
          auto active=_mm_set_epi32(0,-1,-1,-1);
          if constexpr(std::same_as<T,float>) return vec(unchecked{},std::bit_cast<native_type>(_mm_maskload_ps(p,active)));
          else return vec(unchecked{},std::bit_cast<native_type>(_mm_maskload_epi32(reinterpret_cast<int const *>(p),active)));
        }
      }
#elif defined(__aarch64__) || defined(_M_ARM64)
      if constexpr(std::same_as<Arch,neon>) {
        if constexpr(std::same_as<T,float>) {
          auto x=vcombine_f32(vld1_f32(p),vdup_n_f32(0.f));
          if constexpr(N==3) x=vld1q_lane_f32(p+2,x,2);
          return vec(unchecked{},std::bit_cast<native_type>(x));
        } else {
          auto q=reinterpret_cast<std::uint32_t const *>(p);
          auto x=vcombine_u32(vld1_u32(q),vdup_n_u32(0));
          if constexpr(N==3) x=vld1q_lane_u32(q+2,x,2);
          return vec(unchecked{},std::bit_cast<native_type>(x));
        }
      }
#endif
    }
    template<std::size_t Alignment=1>
    simd_inline void store_memory(T * p) const noexcept {
#if defined(__x86_64__) || defined(_M_X64)
      if constexpr(std::same_as<Arch,avx2> || std::same_as<Arch,avx512>) {
        if constexpr(N==2) _mm_storel_epi64(reinterpret_cast<__m128i *>(p),std::bit_cast<__m128i>(value));
        else if constexpr(std::same_as<Arch,avx512>) {
          if constexpr(std::same_as<T,float>) _mm_mask_storeu_ps(p,7,std::bit_cast<__m128>(value));
          else _mm_mask_storeu_epi32(p,7,std::bit_cast<__m128i>(value));
        } else {
          auto active=_mm_set_epi32(0,-1,-1,-1);
          if constexpr(std::same_as<T,float>) _mm_maskstore_ps(p,active,std::bit_cast<__m128>(value));
          else _mm_maskstore_epi32(reinterpret_cast<int *>(p),active,std::bit_cast<__m128i>(value));
        }
      }
#elif defined(__aarch64__) || defined(_M_ARM64)
      if constexpr(std::same_as<Arch,neon>) {
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
    simd_nodiscard static simd_inline vec load(T const * p) noexcept { return load_memory(p); }
    simd_nodiscard static simd_inline vec loadu(T const * p) noexcept { return load_memory(p); }
    simd_inline void store(T * p) const noexcept { store_memory(p); }
    simd_inline void storeu(T * p) const noexcept { store_memory(p); }
    simd_nodiscard static simd_inline vec load_partial(T const * p,std::size_t n,T fill={}) noexcept {
      std::array<T,N> values;values.fill(fill);
      if(n) std::memcpy(values.data(),p,n*sizeof(T));
      return load(values.data());
    }
    simd_inline void store_partial(T * p,std::size_t n) const noexcept {
      std::array<T,N> values;store(values.data());
      if(n) std::memcpy(p,values.data(),n*sizeof(T));
    }
    simd_nodiscard simd_inline bits_type bits() const noexcept requires std::same_as<T,float> {
      return bits_type::from_storage(to_storage().bits());
    }
    simd_nodiscard simd_inline bits_type to_bits() const noexcept requires std::same_as<T,float> { return bits(); }
    simd_nodiscard static simd_inline vec from_bits(bits_type x) noexcept requires std::same_as<T,float> {
      return from_storage(storage_type::from_bits(x.to_storage()));
    }
    simd_nodiscard static simd_inline vec from_bits(std::uint32_t x) noexcept requires std::same_as<T,float> { return from_bits(bits_type(x)); }
    simd_nodiscard static simd_inline vec from_float(float x) noexcept requires std::same_as<T,float> { return vec(x); }
    simd_nodiscard static simd_inline vec unsafe_from_float32(native_type x) noexcept requires std::same_as<T,float> { return vec(x); }
    simd_nodiscard static simd_inline vec load_bits(std::uint32_t const * p) noexcept requires std::same_as<T,float> { return from_bits(bits_type::load(p)); }
    simd_inline void store_bits(std::uint32_t * p) const noexcept requires std::same_as<T,float> { bits().store(p); }
    simd_nodiscard static simd_inline vec load_bits_partial(std::uint32_t const * p,std::size_t n,std::uint32_t fill=0) noexcept requires std::same_as<T,float> { return from_bits(bits_type::load_partial(p,n,fill)); }
    simd_inline void store_bits_partial(std::uint32_t * p,std::size_t n) const noexcept requires std::same_as<T,float> { bits().store_partial(p,n); }
    simd_nodiscard static simd_inline vec from_bitset(std::uint64_t bits) noexcept requires simd_mask_element<T> { return from_storage(storage_type::from_bitset(bits&lane_mask)); }
    simd_nodiscard simd_inline std::uint64_t to_bitset() const noexcept requires simd_mask_element<T> { return to_storage().to_bitset()&lane_mask; }
    simd_nodiscard friend simd_inline bool any(vec x) noexcept requires simd_mask_element<T> { return x.to_bitset()!=0; }
    simd_nodiscard friend simd_inline bool all(vec x) noexcept requires simd_mask_element<T> { return x.to_bitset()==lane_mask; }
    simd_nodiscard friend simd_inline bool none(vec x) noexcept requires simd_mask_element<T> { return !any(x); }

    simd_nodiscard friend simd_inline vec operator+(vec a,vec b) noexcept requires(!simd_mask_element<T>) { return clean(a.to_storage()+b.to_storage()); }
    simd_nodiscard friend simd_inline vec operator-(vec a,vec b) noexcept requires(!simd_mask_element<T>) { return clean(a.to_storage()-b.to_storage()); }
    simd_nodiscard friend simd_inline vec operator*(vec a,vec b) noexcept requires(!simd_mask_element<T>) { return clean(a.to_storage()*b.to_storage()); }
    simd_nodiscard friend simd_inline vec operator/(vec a,vec b) noexcept requires std::same_as<T,float> {
      using M4=typename storage_type::vector_mask_type;
      auto divisor=select(M4::from_bitset(lane_mask),b.to_storage(),storage_type(1.f));
      return clean(a.to_storage()/divisor);
    }
    simd_nodiscard friend simd_inline vec operator-(vec a) noexcept requires(!simd_mask_element<T>) { return from_storage(-a.to_storage()); }
    simd_nodiscard friend simd_inline vec operator&(vec a,vec b) noexcept requires(!std::same_as<T,float>) { return from_storage(a.to_storage()&b.to_storage()); }
    simd_nodiscard friend simd_inline vec operator|(vec a,vec b) noexcept requires(!std::same_as<T,float>) { return from_storage(a.to_storage()|b.to_storage()); }
    simd_nodiscard friend simd_inline vec operator^(vec a,vec b) noexcept requires(!std::same_as<T,float>) { return from_storage(a.to_storage()^b.to_storage()); }
    simd_nodiscard friend simd_inline vec operator~(vec a) noexcept requires(!std::same_as<T,float>) { return from_storage(~a.to_storage()); }
    simd_nodiscard friend simd_inline vec operator!(vec a) noexcept requires simd_mask_element<T> { return ~a; }
    simd_nodiscard friend simd_inline mask_type operator==(vec a,vec b) noexcept { return comparison(a.to_storage()==b.to_storage()); }
    simd_nodiscard friend simd_inline mask_type operator!=(vec a,vec b) noexcept { return ~(a==b); }
    simd_nodiscard friend simd_inline mask_type operator<(vec a,vec b) noexcept requires(!simd_mask_element<T>) { return comparison(a.to_storage()<b.to_storage()); }
    simd_nodiscard friend simd_inline mask_type operator>(vec a,vec b) noexcept requires(!simd_mask_element<T>) { return comparison(a.to_storage()>b.to_storage()); }
    simd_nodiscard friend simd_inline mask_type operator<=(vec a,vec b) noexcept requires(!simd_mask_element<T>) { return (a<b)|(a==b); }
    simd_nodiscard friend simd_inline mask_type operator>=(vec a,vec b) noexcept requires(!simd_mask_element<T>) { return (a>b)|(a==b); }
    template<class M> requires(std::same_as<M,mask_type> || std::same_as<M,vector_mask_type>)
    simd_nodiscard friend simd_inline vec select(M mask,vec a,vec b) noexcept {
      using FM=typename storage_type::mask_type;
      return clean(select(FM::from_bitset(mask.to_bitset()),a.to_storage(),b.to_storage()));
    }
    simd_nodiscard friend simd_inline vec fma(vec a,vec b,vec c) noexcept requires std::same_as<T,float> { return clean(fma(a.to_storage(),b.to_storage(),c.to_storage())); }
    simd_nodiscard friend simd_inline vec sqrt(vec a) noexcept requires std::same_as<T,float> { return clean(sqrt(a.to_storage())); }
    simd_nodiscard friend simd_inline vec round_even(vec a) noexcept requires std::same_as<T,float> { return clean(round_even(a.to_storage())); }
    simd_nodiscard friend simd_inline vec normal_pow2(vec a) noexcept requires std::same_as<T,float> { return from_storage(normal_pow2(a.to_storage())); }
    template<std::size_t K> requires(K<32)
    simd_nodiscard friend simd_inline vec operator<<(vec a,imm_t<K>) noexcept requires simd_integer_element<T> { return from_storage(a.to_storage()<<imm<K>); }
    template<std::size_t K> requires(K<32)
    simd_nodiscard friend simd_inline vec operator>>(vec a,imm_t<K>) noexcept requires simd_integer_element<T> { return from_storage(a.to_storage()>>imm<K>); }
    template<unsigned K> requires(K<32) && simd_integer_element<T>
    simd_nodiscard simd_inline vec left() const noexcept { return *this<<imm<K>; }
    template<unsigned K> requires(K<32) && simd_integer_element<T>
    simd_nodiscard simd_inline vec right() const noexcept { return *this>>imm<K>; }
    // Match native integer registers: division and run-time shifts are absent.
    friend vec operator<<(vec,vec) requires simd_integer_element<T> = delete;
    friend vec operator>>(vec,vec) requires simd_integer_element<T> = delete;
    friend vec operator/(vec,vec) requires simd_integer_element<T> = delete;
    friend vec operator%(vec,vec) requires simd_integer_element<T> = delete;
    template<simd_integer_element U> friend vec operator<<(vec,U) requires simd_integer_element<T> = delete;
    template<simd_integer_element U> friend vec operator>>(vec,U) requires simd_integer_element<T> = delete;
    template<simd_integer_element U> friend vec operator/(vec,U) requires simd_integer_element<T> = delete;
    template<simd_integer_element U> friend vec operator/(U,vec) requires simd_integer_element<T> = delete;
    template<simd_integer_element U> friend vec operator%(vec,U) requires simd_integer_element<T> = delete;
    template<simd_integer_element U> friend vec operator%(U,vec) requires simd_integer_element<T> = delete;
    simd_inline vec & operator+=(vec b) noexcept requires(!simd_mask_element<T>) { return *this=*this+b; }
    simd_inline vec & operator-=(vec b) noexcept requires(!simd_mask_element<T>) { return *this=*this-b; }
    simd_inline vec & operator*=(vec b) noexcept requires(!simd_mask_element<T>) { return *this=*this*b; }
    simd_inline vec & operator/=(vec b) noexcept requires std::same_as<T,float> { return *this=*this/b; }
    simd_inline vec & operator&=(vec b) noexcept requires(!std::same_as<T,float>) { return *this=*this&b; }
    simd_inline vec & operator|=(vec b) noexcept requires(!std::same_as<T,float>) { return *this=*this|b; }
    simd_inline vec & operator^=(vec b) noexcept requires(!std::same_as<T,float>) { return *this=*this^b; }
  private:
    struct unchecked {};
    simd_inline constexpr vec(unchecked,native_type x) noexcept : value(x) {}
    simd_nodiscard static simd_inline vec clean(storage_type x) noexcept { return vec(unchecked{},std::bit_cast<native_type>(x.to_native())); }
    template<class M> simd_nodiscard static simd_inline mask_type comparison(M x) noexcept {
      if constexpr(mask_type::compact) return mask_type::from_native(x.to_native());
      else return mask_type::from_storage(x);
    }
  };

  template<simd_integer_element T,std::size_t N,detail::native_short_arch Arch>
    requires(N==2 || N==3) && (sizeof(T)==4)
  simd_nodiscard simd_inline vec<T,N,Arch> bit_select(vec<T,N,Arch> bits,vec<T,N,Arch> a,vec<T,N,Arch> b) noexcept { return (bits&a)|(~bits&b); }
  template<simd_integer_element T,std::size_t N,detail::native_short_arch Arch,class M>
    requires(N==2 || N==3) && (sizeof(T)==4) &&
      (std::same_as<M,typename vec<T,N,Arch>::mask> || std::same_as<M,vec<mask32,N,Arch>>)
  simd_nodiscard simd_inline vec<T,N,Arch> masked_add(M m,vec<T,N,Arch> prior,vec<T,N,Arch> a,vec<T,N,Arch> b) noexcept { return select(m,a+b,prior); }
  template<simd_integer_element T,std::size_t N,detail::native_short_arch Arch,class M>
    requires(N==2 || N==3) && (sizeof(T)==4) &&
      (std::same_as<M,typename vec<T,N,Arch>::mask> || std::same_as<M,vec<mask32,N,Arch>>)
  simd_nodiscard simd_inline vec<T,N,Arch> masked_sub(M m,vec<T,N,Arch> prior,vec<T,N,Arch> a,vec<T,N,Arch> b) noexcept { return select(m,a-b,prior); }
  template<simd_integer_element T,std::size_t N,detail::native_short_arch Arch,class M>
    requires(N==2 || N==3) && (sizeof(T)==4) &&
      (std::same_as<M,typename vec<T,N,Arch>::mask> || std::same_as<M,vec<mask32,N,Arch>>)
  simd_nodiscard simd_inline vec<T,N,Arch> masked_mul(M m,vec<T,N,Arch> prior,vec<T,N,Arch> a,vec<T,N,Arch> b) noexcept { return select(m,a*b,prior); }
}

// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
