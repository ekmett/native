#pragma once
#include "simd/attributes.h"
#include <array>
#include <span>
#include <concepts>
#include <utility>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <limits>

namespace simd {
  static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559 &&
    std::numeric_limits<float>::radix == 2 && std::numeric_limits<float>::digits == 24 &&
    std::numeric_limits<float>::min_exponent == -125 && std::numeric_limits<float>::max_exponent == 128,
    "simd math requires IEEE 754 binary32 float");

  using std::int8_t;
  using std::int16_t;
  using std::int32_t;
  using std::int64_t;
  using std::uint8_t;
  using std::uint16_t;
  using std::uint32_t;
  using std::uint64_t;

  template <class T> concept simd_integer_element =
    std::same_as<T,int8_t> || std::same_as<T,int16_t> ||
    std::same_as<T,int32_t> || std::same_as<T,int64_t> ||
    std::same_as<T,uint8_t> || std::same_as<T,uint16_t> ||
    std::same_as<T,uint32_t> || std::same_as<T,uint64_t>;

  template <std::size_t K> struct imm_t {
    static constexpr std::size_t value = K;
    simd_nodiscard simd_inline simd_const consteval operator std::size_t() const noexcept { return K; }
  };
  template <std::size_t K> inline constexpr imm_t<K> imm{};

  // These scalar values are a Boolean lane domain, not integers. Their object
  // representations are exactly zero or all ones; numeric arithmetic is absent.
  template<class U> requires (std::is_unsigned_v<U> && !std::same_as<U,bool> && (sizeof(U)==1 || sizeof(U)==2 || sizeof(U)==4 || sizeof(U)==8))
  struct mask_lane {
    using storage_type=U;
    simd_inline constexpr mask_lane() noexcept = default;
    explicit simd_inline constexpr mask_lane(bool value) noexcept : value_(value?U(~U(0)):U(0)) {}
    simd_nodiscard static simd_inline simd_const constexpr mask_lane from_bits(U value) noexcept { return mask_lane(value!=0); }
    simd_nodiscard simd_inline simd_pure constexpr U to_bits() const noexcept { return value_; }
    simd_nodiscard simd_inline simd_pure constexpr bool to_bool() const noexcept { return value_!=0; }
    simd_nodiscard friend simd_inline simd_const constexpr mask_lane operator!(mask_lane a) noexcept { return mask_lane(!a.to_bool()); }
    simd_nodiscard friend simd_inline simd_const constexpr mask_lane operator~(mask_lane a) noexcept { return !a; }
    simd_nodiscard friend simd_inline simd_const constexpr mask_lane operator&(mask_lane a,mask_lane b) noexcept { return mask_lane(a.to_bool()&&b.to_bool()); }
    simd_nodiscard friend simd_inline simd_const constexpr mask_lane operator|(mask_lane a,mask_lane b) noexcept { return mask_lane(a.to_bool()||b.to_bool()); }
    simd_nodiscard friend simd_inline simd_const constexpr mask_lane operator^(mask_lane a,mask_lane b) noexcept { return mask_lane(a.to_bool()!=b.to_bool()); }
    simd_nodiscard friend simd_inline simd_const constexpr bool operator==(mask_lane,mask_lane) noexcept = default;
  private:
    U value_=0;
  };
  using mask8=mask_lane<std::uint8_t>;
  using mask16=mask_lane<std::uint16_t>;
  using mask32=mask_lane<std::uint32_t>;
  using mask64=mask_lane<std::uint64_t>;
  namespace detail {
    template<class T> inline constexpr bool is_mask_lane=false;
    template<class U> inline constexpr bool is_mask_lane<mask_lane<U>> = true;
  }
  template<class T> concept simd_mask_element=detail::is_mask_lane<T>;

  // Custom elements supply their storage register and their own value semantics.
  // The core never lifts raw arithmetic into a user-defined element implicitly.
  template <class T> struct simd_traits;
  template <class T, class Raw, class Self> struct simd_customization;
  template <class T> concept simd_custom_element = requires {
    typename simd_traits<T>::storage_type;
  };
  namespace detail {
    template <class... X> struct simd_deduced_element { using type = float; };
    template <class X, class... Rest> struct simd_deduced_element<X, Rest...> {
      using type = std::conditional_t<simd_custom_element<std::remove_cvref_t<X>>,
        std::remove_cvref_t<X>, typename simd_deduced_element<Rest...>::type>;
    };
    template <class... X> using simd_deduced_element_t = typename simd_deduced_element<X...>::type;
  }

  namespace detail {
  // Native registers are trivial values: default initialization leaves lanes
  // unspecified; broadcast, load, or value initialization supplies every lane.
  // Partial operations touch exactly n lanes (0 <= n <= lanes).
  template<class V, std::size_t L> struct register_memory {
    static constexpr std::size_t lanes = L;
    simd_nodiscard static simd_inline simd_pure V load_partial(simd_noescape float const * p, std::size_t n, float fill = 0) noexcept {
      alignas(64) std::array<float, L> a; a.fill(fill);
      for (std::size_t i = 0; i < n; ++i) a[i] = p[i];
      return V::load(a.data());
    }
    simd_inline void store_partial(simd_noescape float * p, std::size_t n) const noexcept {
      alignas(64) std::array<float, L> a; static_cast<V const &>(*this).store(a.data());
      for (std::size_t i = 0; i < n; ++i) p[i] = a[i];
    }
  };
  }
  struct scalar {};
  struct avx2 {};
  struct avx512 {};
  struct neon {};
  template<class A> concept architecture = std::same_as<A,scalar> || std::same_as<A,avx2> ||
    std::same_as<A,avx512> || std::same_as<A,neon>;
  template<class T, std::size_t N, architecture Arch> struct vec;
  template<std::size_t N, architecture Arch> struct predicate;
  namespace detail { template<class T,std::size_t N,class Arch> struct swizzle_access; }
  template<simd_custom_element T, std::size_t N, architecture Arch>
  struct simd_empty_bases vec<T,N,Arch> : simd_customization<T,
      vec<typename simd_traits<T>::storage_type,N,Arch>,vec<T,N,Arch>>, detail::swizzle_access<T,N,Arch> {
    using base = simd_customization<T,vec<typename simd_traits<T>::storage_type,N,Arch>,vec<T,N,Arch>>;
    using base::base;
    simd_inline constexpr vec() = default;
    using architecture = Arch;
    using mask = typename vec<typename simd_traits<T>::storage_type,N,Arch>::mask;
    using predicate_type = predicate<N,Arch>;
    template<class U> using rebind = vec<U,N,Arch>;
    template<class... X> requires std::constructible_from<base,X...>
    simd_inline constexpr vec(Arch, X &&... x)
      noexcept(std::is_nothrow_constructible_v<base,X...>) : base(std::forward<X>(x)...) {}
  };
  template<architecture Arch, class T, std::size_t N> vec(Arch,std::array<T,N> const &) -> vec<T,N,Arch>;
  template<architecture Arch, class... T> requires(sizeof...(T)>0) && (std::integral<T> && ...)
  vec(Arch,T...) -> vec<std::common_type_t<T...>,sizeof...(T),Arch>;
  template<architecture Arch, class T, class... X> requires simd_mask_element<T> && (std::same_as<T,X> && ...)
  vec(Arch,T,X...) -> vec<T,1+sizeof...(X),Arch>;
  template<architecture Arch, class... X> requires(sizeof...(X)>0) &&
    (!(std::integral<X> && ...)) && (!(simd_mask_element<X> && ...)) &&
    (std::convertible_to<X,detail::simd_deduced_element_t<X...>> && ...)
  vec(Arch,X...) -> vec<detail::simd_deduced_element_t<X...>,sizeof...(X),Arch>;

  // Native-register conversions must not make different architectures mix.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator+(vec<T,N,A>,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator-(vec<T,N,A>,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator*(vec<T,N,A>,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator/(vec<T,N,A>,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator%(vec<T,N,A>,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator&(vec<T,N,A>,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator|(vec<T,N,A>,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator^(vec<T,N,A>,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator<<(vec<T,N,A>,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator>>(vec<T,N,A>,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator==(vec<T,N,A>,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator!=(vec<T,N,A>,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator<(vec<T,N,A>,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator<=(vec<T,N,A>,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator>(vec<T,N,A>,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator>=(vec<T,N,A>,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator+=(vec<T,N,A> &,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator-=(vec<T,N,A> &,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator*=(vec<T,N,A> &,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator/=(vec<T,N,A> &,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator%=(vec<T,N,A> &,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator&=(vec<T,N,A> &,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator|=(vec<T,N,A> &,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator^=(vec<T,N,A> &,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator<<=(vec<T,N,A> &,vec<U,M,B>) = delete;
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator>>=(vec<T,N,A> &,vec<U,M,B>) = delete;

  enum class simd_access { ordinary, streaming };
  template <std::size_t Alignment = 1, simd_access Access = simd_access::ordinary>
  struct simd_memory {
    static_assert(Alignment > 0 && (Alignment & (Alignment - 1)) == 0);
    static constexpr std::size_t alignment = Alignment;
    static constexpr simd_access requested_access = Access;
    // Streaming is currently an ordinary-access fallback on every backend.
    static constexpr bool non_temporal = false;
  };
  template <class V, class U, std::size_t A=1, simd_access Access=simd_access::ordinary>
    requires requires(U const * p) { V::template load_memory<A>(p); }
  simd_nodiscard simd_inline V load_simd(U const * p, simd_memory<A,Access> = {})
      noexcept(noexcept(V::template load_memory<A>(p))) {
    return V::template load_memory<A>(p);
  }
  template <class V, simd_custom_element U, std::size_t A=1, simd_access Access=simd_access::ordinary>
    requires std::same_as<typename V::value_type,typename simd_traits<U>::storage_type>
  simd_nodiscard simd_inline V load_simd(U const * p, simd_memory<A,Access> = {})
      noexcept(noexcept(V::template rebind<U>::template load_memory<A>(p).to_native())) {
    return V::template rebind<U>::template load_memory<A>(p).to_native();
  }
  template <class U, class V, std::size_t A=1, simd_access Access=simd_access::ordinary>
    requires requires(V value,U * p) { value.template store_memory<A>(p); }
  simd_inline void store_simd(U * p,V value,simd_memory<A,Access> = {})
      noexcept(noexcept(value.template store_memory<A>(p))) {
    value.template store_memory<A>(p);
  }
  template <simd_custom_element U, class V, std::size_t A=1, simd_access Access=simd_access::ordinary>
    requires std::same_as<typename V::value_type,typename simd_traits<U>::storage_type>
  simd_inline void store_simd(U * p,V value,simd_memory<A,Access> = {})
      noexcept(noexcept(typename V::template rebind<U>(value).template store_memory<A>(p))) {
    typename V::template rebind<U>(value).template store_memory<A>(p);
  }
  template<class V,class U,std::size_t N> requires(N==V::lanes) &&
    requires(U const * p) { ::simd::load_simd<V>(p); }
  simd_nodiscard simd_inline V load_simd(std::array<U,N> const & values)
      noexcept(noexcept(::simd::load_simd<V>(values.data()))) {
    return ::simd::load_simd<V>(values.data());
  }
  template<class V,class U,std::size_t N> requires(N==V::lanes) &&
    requires(U * p) { ::simd::load_simd<V>(p); }
  simd_nodiscard simd_inline V load_simd(std::span<U,N> values)
      noexcept(noexcept(::simd::load_simd<V>(values.data()))) {
    return ::simd::load_simd<V>(values.data());
  }
  template <class V, class U, std::size_t A=1, simd_access Access=simd_access::ordinary>
    requires std::default_initializable<U> &&
      std::constructible_from<U,typename V::value_type &> && std::is_copy_assignable_v<U> &&
      requires(U const * p) { ::simd::load_simd<V>(p); }
  simd_nodiscard simd_inline V load_simd_partial(U const * p,std::size_t count,
      typename V::value_type fill={},simd_memory<A,Access> = {})
      noexcept(std::is_nothrow_default_constructible_v<U> &&
        std::is_nothrow_constructible_v<U,typename V::value_type &> &&
        std::is_nothrow_copy_assignable_v<U> && noexcept(::simd::load_simd<V>(p))) {
    std::array<U,V::lanes> temporary; temporary.fill(U(fill));
    for(std::size_t i=0;i<count;++i) temporary[i]=p[i];
    return ::simd::load_simd<V>(temporary.data());
  }
  template <class U, class V, std::size_t A=1, simd_access Access=simd_access::ordinary>
    requires std::default_initializable<U> && std::is_copy_assignable_v<U> &&
      requires(U * p,V value) { ::simd::store_simd(p,value); }
  simd_inline void store_simd_partial(U * p,V value,std::size_t count,simd_memory<A,Access> = {})
      noexcept(std::is_nothrow_default_constructible_v<U> &&
        std::is_nothrow_copy_assignable_v<U> && noexcept(::simd::store_simd(p,value))) {
    std::array<U,V::lanes> temporary; ::simd::store_simd(temporary.data(),value);
    for(std::size_t i=0;i<count;++i) p[i]=temporary[i];
  }

}

#include "simd/simd/swizzle.h"

/**
 * \file
 * \license
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
 * SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
 * \endlicense
 * \author Edward Kmett <ekmett@gmail.com>
 * \brief Declares SIMD element types and pointer access policies.
 */
