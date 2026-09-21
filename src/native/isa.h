// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <type_traits>
#include <utility>

namespace native {
  /// Architecture-specific instruction features, represented by local bit indices.
  enum class x86_feature : std::uint64_t {
    mmx, sse, sse2, sse3, ssse3, sse41,
    sse42, popcnt, avx, avx2, fma, f16c,
    bmi1, bmi2, avx512f, avx512dq, avx512bw, avx512vl,
    avx512bf16, avx512fp16, aes, pclmul, cx16, avx512cd,
    avx512ifma, lzcnt, movbe, sahf, mwaitx, waitpkg, crc32, gfni, avx512vpopcntdq,
    vpclmulqdq, avxvnni, avx512vnni, avxvnniint8, avxvnniint16
  };
  /// Independently observable ARM instruction features, using local bit indices.
  enum class arm_feature : std::uint64_t {
    neon, neon_fp16, neon_bf16, aes, sha2, sha3,
    crc, lse, rdm, fp16fml, dotprod, complxnum,
    jsconv, rcpc, pauth, i8mm, pmull, sha1, sha512, ebf16
  };
  /// Number of named x86 feature values.
  inline constexpr std::size_t x86_feature_count=std::size_t(x86_feature::avxvnniint16)+1;
  /// Number of named ARM feature values.
  inline constexpr std::size_t arm_feature_count=std::size_t(arm_feature::ebf16)+1;

  namespace detail {
    template<class T> concept instruction_feature=
      std::same_as<T,x86_feature> || std::same_as<T,arm_feature>;
    template<instruction_feature E> inline constexpr std::size_t feature_count=
      std::same_as<E,x86_feature> ? x86_feature_count : arm_feature_count;
    inline constexpr std::size_t invalid_feature_index=x86_feature_count+arm_feature_count;
    constexpr std::size_t feature_index(instruction_feature auto f) noexcept {
      auto i=std::uint64_t(f);
      if constexpr(std::same_as<decltype(f),x86_feature>)
        return i<x86_feature_count ? std::size_t(i) : invalid_feature_index;
      else
        return i<arm_feature_count ? x86_feature_count+std::size_t(i) : invalid_feature_index;
    }
  }

  /// A structural set of one architecture's features, without prerequisite closure.
  /// E must be x86_feature or arm_feature; the other family's values are rejected.
  template<detail::instruction_feature E> struct feature_set {
    /// Local feature bits and a private invalid marker; not a serialized ABI.
    std::array<std::uint64_t,(detail::feature_count<E>+1+63)/64> flags{};
    /// Construct an empty set.
    constexpr feature_set() noexcept = default;
    /// Construct exactly one feature; an invalid value records an invalid set.
    constexpr feature_set(E f) noexcept { set(f,true); }
    /// Read a feature; out-of-range values are absent.
    constexpr bool get(E f) const noexcept {
      auto i=std::uint64_t(f);
      return i<detail::feature_count<E> && ((flags[i/64]>>(i%64))&1);
    }
    /// Set or clear a feature. Requiring an invalid value records an invalid set;
    /// clearing one leaves the set unchanged. Neither operation indexes outside flags.
    constexpr void set(E f,bool value) noexcept {
      auto i=std::uint64_t(f);
      if(i>=detail::feature_count<E>) {
        if(!value) return;
        i=detail::feature_count<E>;
      }
      auto mask=std::uint64_t{1}<<(i%64);
      auto & word=flags[i/64];
      word=(word&~mask)|(value?mask:0);
    }
    /// True when the named feature is present.
    constexpr bool has(E f) const noexcept { return get(f); }
    /// True when every bit in other is present.
    constexpr bool has(feature_set other) const noexcept {
      for(std::size_t i=0;i<flags.size();++i)
        if((flags[i]&other.flags[i])!=other.flags[i]) return false;
      return true;
    }
    /// True when no invalid marker or unregistered padding bit is set.
    constexpr bool valid() const noexcept {
      constexpr auto used=detail::feature_count<E>%64;
      constexpr auto mask=used ? (std::uint64_t{1}<<used)-1 : 0;
      return (flags.back()&~mask)==0;
    }
    /// Compare all stored bits for exact equality.
    constexpr bool operator==(feature_set const &) const = default;
  };

  /// A structural instruction-feature set. Construction never adds prerequisites.
  /// Each registered feature has a mutable Boolean property, e.g. a.avx2.
  /// ARM extras retain their arm_ prefix to distinguish the two architectures.
  /// Properties read and update flags through their
  /// accessors; they occupy no additional storage. See docs/abi-lookup.md for
  /// feature conjunction, property assignment and subset selection.
  struct isa {
    /// Shared bit storage for both architectures and a private invalid marker.
    /// This representation is structural, not a persistent serialization format.
    std::array<std::uint64_t,(detail::invalid_feature_index+1+63)/64> flags{};

    /// Construct the empty requirement set.
    constexpr isa() noexcept = default;
    /// Require exactly one feature, without prerequisite closure.
    constexpr isa(detail::instruction_feature auto f) noexcept { set(f,true); }
    /// Convert a typed feature set exactly, preserving invalid requirements.
    template<detail::instruction_feature E>
    constexpr isa(feature_set<E> bits) noexcept {
      for(std::size_t i=0;i<detail::feature_count<E>;++i)
        if(bits.has(static_cast<E>(i))) set(static_cast<E>(i),true);
      if(!bits.valid()) set(static_cast<E>(detail::feature_count<E>),true);
    }
    /// Read one feature bit; out-of-range values are absent.
    constexpr bool get(detail::instruction_feature auto f) const noexcept {
      auto i=detail::feature_index(f);
      return i!=detail::invalid_feature_index && ((flags[i/64]>>(i%64))&1);
    }
    /// Set or clear one feature bit without changing any other requirement.
    /// Requiring an out-of-range value records an invalid requirement; clearing
    /// such a value does nothing. Neither operation indexes outside flags.
    constexpr void set(detail::instruction_feature auto f,bool value) noexcept {
      auto i=detail::feature_index(f);
      if(i==detail::invalid_feature_index && !value) return;
      auto mask=std::uint64_t{1}<<(i%64);
      auto & word=flags[i/64];
      word=(word&~mask)|(value?mask:0);
    }
    /// True when the single feature is present.
    constexpr bool has(detail::instruction_feature auto f) const noexcept { return get(f); }
    /// True when every feature in other is present, including the empty set.
    constexpr bool has(isa other) const noexcept {
      for(std::size_t i=0;i<flags.size();++i)
        if((flags[i]&other.flags[i])!=other.flags[i]) return false;
      return true;
    }
    /// Compare all stored bits for exact set equality.
    constexpr bool operator==(isa const &) const = default;

    constexpr bool get_mmx() const noexcept { return get(x86_feature::mmx); }
    constexpr void set_mmx(bool value) noexcept { set(x86_feature::mmx,value); }
    __declspec(property(get=get_mmx,put=set_mmx)) bool mmx;
    constexpr bool get_sse() const noexcept { return get(x86_feature::sse); }
    constexpr void set_sse(bool value) noexcept { set(x86_feature::sse,value); }
    __declspec(property(get=get_sse,put=set_sse)) bool sse;
    constexpr bool get_sse2() const noexcept { return get(x86_feature::sse2); }
    constexpr void set_sse2(bool value) noexcept { set(x86_feature::sse2,value); }
    __declspec(property(get=get_sse2,put=set_sse2)) bool sse2;
    constexpr bool get_sse3() const noexcept { return get(x86_feature::sse3); }
    constexpr void set_sse3(bool value) noexcept { set(x86_feature::sse3,value); }
    __declspec(property(get=get_sse3,put=set_sse3)) bool sse3;
    constexpr bool get_ssse3() const noexcept { return get(x86_feature::ssse3); }
    constexpr void set_ssse3(bool value) noexcept { set(x86_feature::ssse3,value); }
    __declspec(property(get=get_ssse3,put=set_ssse3)) bool ssse3;
    constexpr bool get_sse41() const noexcept { return get(x86_feature::sse41); }
    constexpr void set_sse41(bool value) noexcept { set(x86_feature::sse41,value); }
    __declspec(property(get=get_sse41,put=set_sse41)) bool sse41;
    constexpr bool get_sse42() const noexcept { return get(x86_feature::sse42); }
    constexpr void set_sse42(bool value) noexcept { set(x86_feature::sse42,value); }
    __declspec(property(get=get_sse42,put=set_sse42)) bool sse42;
    constexpr bool get_popcnt() const noexcept { return get(x86_feature::popcnt); }
    constexpr void set_popcnt(bool value) noexcept { set(x86_feature::popcnt,value); }
    __declspec(property(get=get_popcnt,put=set_popcnt)) bool popcnt;
    constexpr bool get_avx() const noexcept { return get(x86_feature::avx); }
    constexpr void set_avx(bool value) noexcept { set(x86_feature::avx,value); }
    __declspec(property(get=get_avx,put=set_avx)) bool avx;
    constexpr bool get_avx2() const noexcept { return get(x86_feature::avx2); }
    constexpr void set_avx2(bool value) noexcept { set(x86_feature::avx2,value); }
    __declspec(property(get=get_avx2,put=set_avx2)) bool avx2;
    constexpr bool get_fma() const noexcept { return get(x86_feature::fma); }
    constexpr void set_fma(bool value) noexcept { set(x86_feature::fma,value); }
    __declspec(property(get=get_fma,put=set_fma)) bool fma;
    constexpr bool get_f16c() const noexcept { return get(x86_feature::f16c); }
    constexpr void set_f16c(bool value) noexcept { set(x86_feature::f16c,value); }
    __declspec(property(get=get_f16c,put=set_f16c)) bool f16c;
    constexpr bool get_bmi1() const noexcept { return get(x86_feature::bmi1); }
    constexpr void set_bmi1(bool value) noexcept { set(x86_feature::bmi1,value); }
    __declspec(property(get=get_bmi1,put=set_bmi1)) bool bmi1;
    constexpr bool get_bmi2() const noexcept { return get(x86_feature::bmi2); }
    constexpr void set_bmi2(bool value) noexcept { set(x86_feature::bmi2,value); }
    __declspec(property(get=get_bmi2,put=set_bmi2)) bool bmi2;
    constexpr bool get_avx512f() const noexcept { return get(x86_feature::avx512f); }
    constexpr void set_avx512f(bool value) noexcept { set(x86_feature::avx512f,value); }
    __declspec(property(get=get_avx512f,put=set_avx512f)) bool avx512f;
    constexpr bool get_avx512dq() const noexcept { return get(x86_feature::avx512dq); }
    constexpr void set_avx512dq(bool value) noexcept { set(x86_feature::avx512dq,value); }
    __declspec(property(get=get_avx512dq,put=set_avx512dq)) bool avx512dq;
    constexpr bool get_avx512bw() const noexcept { return get(x86_feature::avx512bw); }
    constexpr void set_avx512bw(bool value) noexcept { set(x86_feature::avx512bw,value); }
    __declspec(property(get=get_avx512bw,put=set_avx512bw)) bool avx512bw;
    constexpr bool get_avx512vl() const noexcept { return get(x86_feature::avx512vl); }
    constexpr void set_avx512vl(bool value) noexcept { set(x86_feature::avx512vl,value); }
    __declspec(property(get=get_avx512vl,put=set_avx512vl)) bool avx512vl;
    constexpr bool get_avx512bf16() const noexcept { return get(x86_feature::avx512bf16); }
    constexpr void set_avx512bf16(bool value) noexcept { set(x86_feature::avx512bf16,value); }
    __declspec(property(get=get_avx512bf16,put=set_avx512bf16)) bool avx512bf16;
    constexpr bool get_avx512fp16() const noexcept { return get(x86_feature::avx512fp16); }
    constexpr void set_avx512fp16(bool value) noexcept { set(x86_feature::avx512fp16,value); }
    __declspec(property(get=get_avx512fp16,put=set_avx512fp16)) bool avx512fp16;
    constexpr bool get_neon() const noexcept { return get(arm_feature::neon); }
    constexpr void set_neon(bool value) noexcept { set(arm_feature::neon,value); }
    __declspec(property(get=get_neon,put=set_neon)) bool neon;
    constexpr bool get_neon_fp16() const noexcept { return get(arm_feature::neon_fp16); }
    constexpr void set_neon_fp16(bool value) noexcept { set(arm_feature::neon_fp16,value); }
    __declspec(property(get=get_neon_fp16,put=set_neon_fp16)) bool neon_fp16;
    constexpr bool get_neon_bf16() const noexcept { return get(arm_feature::neon_bf16); }
    constexpr void set_neon_bf16(bool value) noexcept { set(arm_feature::neon_bf16,value); }
    __declspec(property(get=get_neon_bf16,put=set_neon_bf16)) bool neon_bf16;
    constexpr bool get_aes() const noexcept { return get(x86_feature::aes); }
    constexpr void set_aes(bool value) noexcept { set(x86_feature::aes,value); }
    __declspec(property(get=get_aes,put=set_aes)) bool aes;
    constexpr bool get_pclmul() const noexcept { return get(x86_feature::pclmul); }
    constexpr void set_pclmul(bool value) noexcept { set(x86_feature::pclmul,value); }
    __declspec(property(get=get_pclmul,put=set_pclmul)) bool pclmul;
    constexpr bool get_cx16() const noexcept { return get(x86_feature::cx16); }
    constexpr void set_cx16(bool value) noexcept { set(x86_feature::cx16,value); }
    __declspec(property(get=get_cx16,put=set_cx16)) bool cx16;
    constexpr bool get_avx512cd() const noexcept { return get(x86_feature::avx512cd); }
    constexpr void set_avx512cd(bool value) noexcept { set(x86_feature::avx512cd,value); }
    __declspec(property(get=get_avx512cd,put=set_avx512cd)) bool avx512cd;
    constexpr bool get_avx512ifma() const noexcept { return get(x86_feature::avx512ifma); }
    constexpr void set_avx512ifma(bool value) noexcept { set(x86_feature::avx512ifma,value); }
    __declspec(property(get=get_avx512ifma,put=set_avx512ifma)) bool avx512ifma;
    constexpr bool get_lzcnt() const noexcept { return get(x86_feature::lzcnt); }
    constexpr void set_lzcnt(bool value) noexcept { set(x86_feature::lzcnt,value); }
    __declspec(property(get=get_lzcnt,put=set_lzcnt)) bool lzcnt;
    constexpr bool get_movbe() const noexcept { return get(x86_feature::movbe); }
    constexpr void set_movbe(bool value) noexcept { set(x86_feature::movbe,value); }
    __declspec(property(get=get_movbe,put=set_movbe)) bool movbe;
    constexpr bool get_sahf() const noexcept { return get(x86_feature::sahf); }
    constexpr void set_sahf(bool value) noexcept { set(x86_feature::sahf,value); }
    __declspec(property(get=get_sahf,put=set_sahf)) bool sahf;
    constexpr bool get_mwaitx() const noexcept { return get(x86_feature::mwaitx); }
    constexpr void set_mwaitx(bool value) noexcept { set(x86_feature::mwaitx,value); }
    __declspec(property(get=get_mwaitx,put=set_mwaitx)) bool mwaitx;
    constexpr bool get_waitpkg() const noexcept { return get(x86_feature::waitpkg); }
    constexpr void set_waitpkg(bool value) noexcept { set(x86_feature::waitpkg,value); }
    __declspec(property(get=get_waitpkg,put=set_waitpkg)) bool waitpkg;
    constexpr bool get_crc32() const noexcept { return get(x86_feature::crc32); }
    constexpr void set_crc32(bool value) noexcept { set(x86_feature::crc32,value); }
    __declspec(property(get=get_crc32,put=set_crc32)) bool crc32;
    constexpr bool get_gfni() const noexcept { return get(x86_feature::gfni); }
    constexpr void set_gfni(bool value) noexcept { set(x86_feature::gfni,value); }
    __declspec(property(get=get_gfni,put=set_gfni)) bool gfni;
    constexpr bool get_vpclmulqdq() const noexcept { return get(x86_feature::vpclmulqdq); }
    constexpr void set_vpclmulqdq(bool value) noexcept { set(x86_feature::vpclmulqdq,value); }
    __declspec(property(get=get_vpclmulqdq,put=set_vpclmulqdq)) bool vpclmulqdq;
    constexpr bool get_avx512vpopcntdq() const noexcept { return get(x86_feature::avx512vpopcntdq); }
    constexpr void set_avx512vpopcntdq(bool value) noexcept { set(x86_feature::avx512vpopcntdq,value); }
    __declspec(property(get=get_avx512vpopcntdq,put=set_avx512vpopcntdq)) bool avx512vpopcntdq;
    constexpr bool get_avxvnni() const noexcept { return get(x86_feature::avxvnni); }
    constexpr void set_avxvnni(bool value) noexcept { set(x86_feature::avxvnni,value); }
    __declspec(property(get=get_avxvnni,put=set_avxvnni)) bool avxvnni;
    constexpr bool get_avx512vnni() const noexcept { return get(x86_feature::avx512vnni); }
    constexpr void set_avx512vnni(bool value) noexcept { set(x86_feature::avx512vnni,value); }
    __declspec(property(get=get_avx512vnni,put=set_avx512vnni)) bool avx512vnni;
    constexpr bool get_avxvnniint8() const noexcept { return get(x86_feature::avxvnniint8); }
    constexpr void set_avxvnniint8(bool value) noexcept { set(x86_feature::avxvnniint8,value); }
    __declspec(property(get=get_avxvnniint8,put=set_avxvnniint8)) bool avxvnniint8;
    constexpr bool get_avxvnniint16() const noexcept { return get(x86_feature::avxvnniint16); }
    constexpr void set_avxvnniint16(bool value) noexcept { set(x86_feature::avxvnniint16,value); }
    __declspec(property(get=get_avxvnniint16,put=set_avxvnniint16)) bool avxvnniint16;
    constexpr bool get_arm_pmull() const noexcept { return get(arm_feature::pmull); }
    constexpr void set_arm_pmull(bool value) noexcept { set(arm_feature::pmull,value); }
    __declspec(property(get=get_arm_pmull,put=set_arm_pmull)) bool arm_pmull;
    constexpr bool get_arm_sha1() const noexcept { return get(arm_feature::sha1); }
    constexpr void set_arm_sha1(bool value) noexcept { set(arm_feature::sha1,value); }
    __declspec(property(get=get_arm_sha1,put=set_arm_sha1)) bool arm_sha1;
    constexpr bool get_arm_sha512() const noexcept { return get(arm_feature::sha512); }
    constexpr void set_arm_sha512(bool value) noexcept { set(arm_feature::sha512,value); }
    __declspec(property(get=get_arm_sha512,put=set_arm_sha512)) bool arm_sha512;
    constexpr bool get_arm_ebf16() const noexcept { return get(arm_feature::ebf16); }
    constexpr void set_arm_ebf16(bool value) noexcept { set(arm_feature::ebf16,value); }
    __declspec(property(get=get_arm_ebf16,put=set_arm_ebf16)) bool arm_ebf16;
    constexpr bool get_arm_aes() const noexcept { return get(arm_feature::aes); }
    constexpr void set_arm_aes(bool value) noexcept { set(arm_feature::aes,value); }
    __declspec(property(get=get_arm_aes,put=set_arm_aes)) bool arm_aes;
    constexpr bool get_arm_sha2() const noexcept { return get(arm_feature::sha2); }
    constexpr void set_arm_sha2(bool value) noexcept { set(arm_feature::sha2,value); }
    __declspec(property(get=get_arm_sha2,put=set_arm_sha2)) bool arm_sha2;
    constexpr bool get_arm_sha3() const noexcept { return get(arm_feature::sha3); }
    constexpr void set_arm_sha3(bool value) noexcept { set(arm_feature::sha3,value); }
    __declspec(property(get=get_arm_sha3,put=set_arm_sha3)) bool arm_sha3;
    constexpr bool get_arm_crc() const noexcept { return get(arm_feature::crc); }
    constexpr void set_arm_crc(bool value) noexcept { set(arm_feature::crc,value); }
    __declspec(property(get=get_arm_crc,put=set_arm_crc)) bool arm_crc;
    constexpr bool get_arm_lse() const noexcept { return get(arm_feature::lse); }
    constexpr void set_arm_lse(bool value) noexcept { set(arm_feature::lse,value); }
    __declspec(property(get=get_arm_lse,put=set_arm_lse)) bool arm_lse;
    constexpr bool get_arm_rdm() const noexcept { return get(arm_feature::rdm); }
    constexpr void set_arm_rdm(bool value) noexcept { set(arm_feature::rdm,value); }
    __declspec(property(get=get_arm_rdm,put=set_arm_rdm)) bool arm_rdm;
    constexpr bool get_arm_fp16fml() const noexcept { return get(arm_feature::fp16fml); }
    constexpr void set_arm_fp16fml(bool value) noexcept { set(arm_feature::fp16fml,value); }
    __declspec(property(get=get_arm_fp16fml,put=set_arm_fp16fml)) bool arm_fp16fml;
    constexpr bool get_arm_dotprod() const noexcept { return get(arm_feature::dotprod); }
    constexpr void set_arm_dotprod(bool value) noexcept { set(arm_feature::dotprod,value); }
    __declspec(property(get=get_arm_dotprod,put=set_arm_dotprod)) bool arm_dotprod;
    constexpr bool get_arm_complxnum() const noexcept { return get(arm_feature::complxnum); }
    constexpr void set_arm_complxnum(bool value) noexcept { set(arm_feature::complxnum,value); }
    __declspec(property(get=get_arm_complxnum,put=set_arm_complxnum)) bool arm_complxnum;
    constexpr bool get_arm_jsconv() const noexcept { return get(arm_feature::jsconv); }
    constexpr void set_arm_jsconv(bool value) noexcept { set(arm_feature::jsconv,value); }
    __declspec(property(get=get_arm_jsconv,put=set_arm_jsconv)) bool arm_jsconv;
    constexpr bool get_arm_rcpc() const noexcept { return get(arm_feature::rcpc); }
    constexpr void set_arm_rcpc(bool value) noexcept { set(arm_feature::rcpc,value); }
    __declspec(property(get=get_arm_rcpc,put=set_arm_rcpc)) bool arm_rcpc;
    constexpr bool get_arm_pauth() const noexcept { return get(arm_feature::pauth); }
    constexpr void set_arm_pauth(bool value) noexcept { set(arm_feature::pauth,value); }
    __declspec(property(get=get_arm_pauth,put=set_arm_pauth)) bool arm_pauth;
    constexpr bool get_arm_i8mm() const noexcept { return get(arm_feature::i8mm); }
    constexpr void set_arm_i8mm(bool value) noexcept { set(arm_feature::i8mm,value); }
    __declspec(property(get=get_arm_i8mm,put=set_arm_i8mm)) bool arm_i8mm;
  };

  template<class T> concept arch=detail::instruction_feature<T> || std::same_as<T,isa> ||
    std::same_as<T,feature_set<x86_feature>> || std::same_as<T,feature_set<arm_feature>>;

  /// Requirements compose by union: both operands must be available.
  constexpr isa operator&(arch auto left,arch auto right) noexcept {
    isa result=left, other=right;
    for(std::size_t i=0;i<result.flags.size();++i) result.flags[i]|=other.flags[i];
    return result;
  }
  // Concrete enum overloads prevent built-in ordinal comparisons from winning.
  /// A singleton feature is never a strict subset of another singleton.
  constexpr bool operator<(x86_feature,x86_feature) noexcept { return false; }
  /// A singleton feature is never a strict superset of another singleton.
  constexpr bool operator>(x86_feature,x86_feature) noexcept { return false; }
  /// Singleton inclusion holds exactly when both feature names are equal.
  constexpr bool operator<=(x86_feature a,x86_feature b) noexcept { return a==b; }
  /// Reverse singleton inclusion holds exactly when both features are equal.
  constexpr bool operator>=(x86_feature a,x86_feature b) noexcept { return a==b; }
  /// A singleton feature is never a strict subset of another singleton.
  constexpr bool operator<(arm_feature,arm_feature) noexcept { return false; }
  /// A singleton feature is never a strict superset of another singleton.
  constexpr bool operator>(arm_feature,arm_feature) noexcept { return false; }
  /// Singleton inclusion holds exactly when both feature names are equal.
  constexpr bool operator<=(arm_feature a,arm_feature b) noexcept { return a==b; }
  /// Reverse singleton inclusion holds exactly when both features are equal.
  constexpr bool operator>=(arm_feature a,arm_feature b) noexcept { return a==b; }
  /// True when every feature required by a is present in b.
  constexpr bool operator<=(arch auto a,arch auto b) noexcept { return isa(b).has(a); }
  /// True when a contains every feature required by b.
  constexpr bool operator>=(arch auto a,arch auto b) noexcept { return b<=a; }
  /// True when a is a subset of b and their feature sets differ.
  constexpr bool operator<(arch auto a,arch auto b) noexcept { return isa(a)!=isa(b) && a<=b; }
  /// True when a strictly contains b.
  constexpr bool operator>(arch auto a,arch auto b) noexcept { return b<a; }

  /// First matching requirement, with every later choice checked for shadowing.
  template<arch auto A,arch auto... Choices>
  inline constexpr int target=[]() consteval {
    constexpr std::array<isa,sizeof...(Choices)> choices{isa(Choices)...};
    static_assert([&] {
      for(std::size_t i=0;i<choices.size();++i)
        for(std::size_t j=i+1;j<choices.size();++j)
          if(choices[i]<=choices[j]) return false;
      return true;
    }(),"a later target is shadowed by an earlier one");
    for(std::size_t i=0;i<choices.size();++i)
      if(choices[i]<=isa(A)) return int(i);
    return -1;
  }();

  namespace detail {
    constexpr isa intersection(isa a,isa b) noexcept {
      for(std::size_t i=0;i<a.flags.size();++i) a.flags[i]&=b.flags[i];
      return a;
    }
    enum class feature_register { leaf1_ecx, leaf1_edx, leaf7_ebx, leaf7_ecx, leaf7_edx, leaf7_1_eax, leaf7_1_edx, extended1_ecx, arm };
    struct feature_record {
      isa value;
      std::string_view spelling;
      isa implies;
      feature_register location;
      unsigned bit;
      std::size_t index;
      isa target_implies;
      bool targetable;
      template<instruction_feature E>
      constexpr feature_record(E f,std::string_view spelling,isa implies,
          feature_register location,unsigned bit,isa target_implies={},bool targetable=true) noexcept:
        value(f),spelling(spelling),implies(implies),location(location),bit(bit),index(std::size_t(f)),
        target_implies(target_implies),targetable(targetable) {}
    };
    // Register and instruction prerequisites. X86 retains compiler feature dependencies.
    // ARM crypto bits are independent; target_implies records Clang bundles separately.
    // A hardware-only flag is not necessarily a valid compiler target spelling.
    inline constexpr feature_record feature_registry[] = {
      {x86_feature::mmx,"mmx",{},feature_register::leaf1_edx,23},
      {x86_feature::sse,"sse",isa(x86_feature::mmx),feature_register::leaf1_edx,25},
      {x86_feature::sse2,"sse2",isa(x86_feature::sse),feature_register::leaf1_edx,26},
      {x86_feature::sse3,"sse3",isa(x86_feature::sse2),feature_register::leaf1_ecx,0},
      {x86_feature::ssse3,"ssse3",isa(x86_feature::sse3),feature_register::leaf1_ecx,9},
      {x86_feature::sse41,"sse4.1",isa(x86_feature::ssse3),feature_register::leaf1_ecx,19},
      {x86_feature::sse42,"sse4.2",x86_feature::sse41&x86_feature::popcnt&x86_feature::crc32,feature_register::leaf1_ecx,20},
      {x86_feature::popcnt,"popcnt",{},feature_register::leaf1_ecx,23},
      {x86_feature::avx,"avx",isa(x86_feature::sse42),feature_register::leaf1_ecx,28},
      {x86_feature::avx2,"avx2",isa(x86_feature::avx),feature_register::leaf7_ebx,5},
      {x86_feature::fma,"fma",isa(x86_feature::avx),feature_register::leaf1_ecx,12},
      {x86_feature::f16c,"f16c",isa(x86_feature::avx),feature_register::leaf1_ecx,29},
      {x86_feature::bmi1,"bmi",{},feature_register::leaf7_ebx,3},
      {x86_feature::bmi2,"bmi2",{},feature_register::leaf7_ebx,8},
      {x86_feature::mwaitx,"mwaitx",{},feature_register::extended1_ecx,29},
      {x86_feature::waitpkg,"waitpkg",{},feature_register::leaf7_ecx,5},
      {x86_feature::crc32,"crc32",{},feature_register::leaf1_ecx,20},
      {x86_feature::gfni,"gfni",isa(x86_feature::sse2),feature_register::leaf7_ecx,8},
      {x86_feature::avx512f,"avx512f",x86_feature::avx2&x86_feature::f16c&x86_feature::fma,feature_register::leaf7_ebx,16},
      {x86_feature::avx512dq,"avx512dq",isa(x86_feature::avx512f),feature_register::leaf7_ebx,17},
      {x86_feature::avx512bw,"avx512bw",isa(x86_feature::avx512f),feature_register::leaf7_ebx,30},
      {x86_feature::avx512vl,"avx512vl",isa(x86_feature::avx512f),feature_register::leaf7_ebx,31},
      {x86_feature::avx512bf16,"avx512bf16",isa(x86_feature::avx512bw),feature_register::leaf7_1_eax,5},
      {x86_feature::avx512fp16,"avx512fp16",isa(x86_feature::avx512bw),feature_register::leaf7_edx,23},
      {x86_feature::avx512vpopcntdq,"avx512vpopcntdq",isa(x86_feature::avx512f),feature_register::leaf7_ecx,14},
      {x86_feature::avxvnni,"avxvnni",isa(x86_feature::avx2),feature_register::leaf7_1_eax,4},
      {x86_feature::avx512vnni,"avx512vnni",isa(x86_feature::avx512f),feature_register::leaf7_ecx,11},
      {x86_feature::avxvnniint8,"avxvnniint8",isa(x86_feature::avx2),feature_register::leaf7_1_edx,4},
      {x86_feature::avxvnniint16,"avxvnniint16",isa(x86_feature::avx2),feature_register::leaf7_1_edx,10},
      {arm_feature::neon,"neon",{},feature_register::arm,0},
      {arm_feature::neon_fp16,"fullfp16",isa(arm_feature::neon),feature_register::arm,1},
      {arm_feature::neon_bf16,"bf16",isa(arm_feature::neon),feature_register::arm,2},
      {x86_feature::aes,"aes",isa(x86_feature::sse2),feature_register::leaf1_ecx,25},
      {x86_feature::pclmul,"pclmul",isa(x86_feature::sse2),feature_register::leaf1_ecx,1},
      {x86_feature::vpclmulqdq,"vpclmulqdq",x86_feature::avx&x86_feature::pclmul,feature_register::leaf7_ecx,10},
      {x86_feature::cx16,"cx16",{},feature_register::leaf1_ecx,13},
      {x86_feature::avx512cd,"avx512cd",isa(x86_feature::avx512f),feature_register::leaf7_ebx,28},
      {x86_feature::avx512ifma,"avx512ifma",isa(x86_feature::avx512f),feature_register::leaf7_ebx,21},
      {x86_feature::lzcnt,"lzcnt",{},feature_register::extended1_ecx,5},
      {x86_feature::movbe,"movbe",{},feature_register::leaf1_ecx,22},
      {x86_feature::sahf,"sahf",{},feature_register::extended1_ecx,0},
      {arm_feature::aes,"aes",isa(arm_feature::neon),feature_register::arm,3,isa(arm_feature::pmull)},
      {arm_feature::sha2,"sha2",isa(arm_feature::neon),feature_register::arm,4,isa(arm_feature::sha1)},
      {arm_feature::sha3,"sha3",isa(arm_feature::neon),feature_register::arm,5,arm_feature::sha1&arm_feature::sha2&arm_feature::sha512},
      {arm_feature::crc,"crc",{},feature_register::arm,6},
      {arm_feature::lse,"lse",isa(arm_feature::neon),feature_register::arm,7},
      {arm_feature::rdm,"rdm",isa(arm_feature::neon),feature_register::arm,8},
      {arm_feature::fp16fml,"fp16fml",isa(arm_feature::neon_fp16),feature_register::arm,9},
      {arm_feature::dotprod,"dotprod",isa(arm_feature::neon),feature_register::arm,10},
      {arm_feature::complxnum,"complxnum",isa(arm_feature::neon),feature_register::arm,11},
      {arm_feature::jsconv,"jsconv",isa(arm_feature::neon),feature_register::arm,12},
      {arm_feature::rcpc,"rcpc",isa(arm_feature::neon),feature_register::arm,13},
      {arm_feature::pauth,"pauth",isa(arm_feature::neon),feature_register::arm,14},
      {arm_feature::i8mm,"i8mm",isa(arm_feature::neon),feature_register::arm,15},
      {arm_feature::pmull,"pmull",isa(arm_feature::neon),feature_register::arm,16,{},false},
      {arm_feature::sha1,"sha1",isa(arm_feature::neon),feature_register::arm,17,{},false},
      {arm_feature::sha512,"sha512",isa(arm_feature::neon),feature_register::arm,18,{},false},
      {arm_feature::ebf16,"ebf16",isa(arm_feature::neon_bf16),feature_register::arm,19,{},false}
    };
    inline constexpr isa arm_features=[] {
      isa result;
      for(auto const & entry:feature_registry)
        if(entry.location==feature_register::arm) result=result&entry.value;
      return result;
    }();
    inline constexpr isa x86_features=[] {
      isa result;
      for(auto const & entry:feature_registry)
        if(entry.location!=feature_register::arm) result=result&entry.value;
      return result;
    }();
    inline constexpr isa known_features=arm_features&x86_features;
    inline constexpr isa invalid_features=static_cast<x86_feature>(-1);
    // A shared value keeps repeated source constraints equivalent across
    // declarations; an immediately invoked macro lambda would not.
    template<isa A> inline constexpr isa source_isa=[]() consteval {
      static_assert(A<=known_features,"source target contains an unregistered ISA feature");
      static_assert(A<=x86_features || A<=arm_features,"source target combines x86 and ARM features");
      return A;
    }();
  }

  /// Add register and instruction prerequisites, shared by presets and admission.
  /// ARM crypto siblings remain independent; target_features adds compiler bundles.
  constexpr isa feature_closure(isa bits) noexcept {
    isa previous;
    do {
      previous=bits;
      for(auto const & entry:detail::feature_registry)
        if(bits.has(entry.value)) bits=bits&entry.implies;
    } while(previous!=bits);
    return bits;
  }

  inline constexpr isa scalar{};
  inline constexpr isa avx2=feature_closure(x86_feature::avx2&x86_feature::fma);
  inline constexpr isa avx512=feature_closure(avx2&x86_feature::avx512f&x86_feature::avx512dq&x86_feature::avx512bw&x86_feature::avx512vl);
  inline constexpr isa avx512_bf16=feature_closure(avx512&x86_feature::avx512bf16);
  inline constexpr isa avx512_fp16=feature_closure(avx512&x86_feature::avx512fp16);
  inline constexpr isa neon=feature_closure(arm_feature::neon);
  inline constexpr isa neon_fp16=feature_closure(arm_feature::neon_fp16);
  inline constexpr isa neon_bf16=feature_closure(arm_feature::neon_bf16);

  /// Parse a registered literal target feature list. CPU names, negative
  /// features and unknown features fail closed rather than guessing admission.
  constexpr isa target_features(std::string_view text) noexcept {
    if(text.empty()) return {};
#if defined(__aarch64__) || defined(_M_ARM64)
    bool arm_target=true;
#else
    bool arm_target=false;
#endif
    // Explicit ARM profile strings are also useful in synthetic metadata tests
    // on an x86 host. AES is spelled identically by both compiler backends.
    if(text.find("neon")!=std::string_view::npos || text.find("fullfp16")!=std::string_view::npos)
      arm_target=true;
    isa bits;
    while(!text.empty()) {
      auto comma=text.find(',');
      auto token=text.substr(0,comma);
      bool found=false;
      for(auto const & entry:detail::feature_registry) if(entry.targetable && token==entry.spelling &&
          (entry.spelling!="aes" || (entry.location==detail::feature_register::arm)==arm_target)) {
        bits=bits&entry.value&entry.target_implies; found=true; break;
      }
      if(!found) return detail::invalid_features;
      if(comma==std::string_view::npos) break;
      text.remove_prefix(comma+1);
      if(text.empty()) return detail::invalid_features;
    }
    return feature_closure(bits);
  }

  struct isa_admission {
    isa missing_features{};
    std::uint64_t missing_xcr0=0;
    bool invalid_features=false;
    bool missing_xcr0_observation=false;
    /// True only when every requested feature and OS state component is present.
    constexpr bool admitted() const noexcept {
      return missing_features==scalar && !missing_xcr0 && !invalid_features && !missing_xcr0_observation;
    }
    /// First missing feature's target spelling, or a missing OS-state description.
    /// The feature set and state mask retain all missing requirements. A feature
    /// is unavailable both when its query failed and when it was observed absent.
    constexpr char const * reason() const noexcept {
      if(invalid_features) return "invalid ISA features";
      for(auto const & entry:detail::feature_registry)
        if(missing_features.has(entry.value)) return entry.spelling.data();
      if(missing_xcr0_observation) return "XCR0 unavailable";
      if(missing_xcr0 & (1ull<<1)) return "XMM state unavailable";
      if(missing_xcr0 & (1ull<<2)) return "YMM state unavailable";
      if(missing_xcr0 & (1ull<<5)) return "opmask state unavailable";
      if(missing_xcr0 & (1ull<<6)) return "ZMM high state unavailable";
      if(missing_xcr0 & (1ull<<7)) return "high ZMM registers unavailable";
      return admitted() ? "admitted" : "ISA requirements unavailable";
    }
  };

  namespace detail {
    template<class C> concept x86_observation=requires(C const & c) {
      c.max_basic_leaf; c.leaf1_ecx; c.leaf1_edx; c.leaf7_ebx;
      c.max_leaf7_subleaf; c.leaf7_1_eax; c.leaf7_edx;
    };
    template<class C> concept arm_observation=requires(C const & c) {
      c.baseline_observed; c.fp; c.asimd;
      c.fp16_observed; c.scalar_fp16; c.vector_fp16;
      c.bf16_observed; c.bf16;
    };
    template<class C,class E> concept normalized_features=requires(C const & c) {
      { c.present } -> std::same_as<feature_set<E> const &>;
      { c.observed } -> std::same_as<feature_set<E> const &>;
    };
    template<instruction_feature E> struct feature_observation {
      feature_set<E> present{},observed{};
    };
    template<x86_observation C>
    constexpr feature_observation<x86_feature> decode_x86_features(C const & cpu) noexcept {
      feature_observation<x86_feature> result;
      for(auto const & entry:feature_registry) {
        std::uint32_t word=0;
        bool observed=false;
        switch(entry.location) {
          case feature_register::leaf1_ecx: observed=cpu.max_basic_leaf>=1; word=cpu.leaf1_ecx; break;
          case feature_register::leaf1_edx: observed=cpu.max_basic_leaf>=1; word=cpu.leaf1_edx; break;
          case feature_register::leaf7_ebx: observed=cpu.max_basic_leaf>=7; word=cpu.leaf7_ebx; break;
          case feature_register::leaf7_ecx:
            if constexpr(requires { cpu.leaf7_ecx; }) {
              observed=cpu.max_basic_leaf>=7; word=cpu.leaf7_ecx;
            }
            break;
          case feature_register::leaf7_edx: observed=cpu.max_basic_leaf>=7; word=cpu.leaf7_edx; break;
          case feature_register::leaf7_1_eax:
            observed=cpu.max_basic_leaf>=7 && cpu.max_leaf7_subleaf>=1; word=cpu.leaf7_1_eax; break;
          case feature_register::leaf7_1_edx:
            if constexpr(requires { cpu.leaf7_1_edx; }) {
              observed=cpu.max_basic_leaf>=7 && cpu.max_leaf7_subleaf>=1; word=cpu.leaf7_1_edx;
            }
            break;
          case feature_register::extended1_ecx:
            if constexpr(requires { cpu.max_extended_leaf; cpu.extended1_ecx; }) {
              observed=cpu.max_extended_leaf>=0x80000001u; word=cpu.extended1_ecx;
            }
            break;
          case feature_register::arm: continue;
        }
        auto f=static_cast<x86_feature>(entry.index);
        result.observed.set(f,observed);
        result.present.set(f,observed && (word&(std::uint32_t{1}<<entry.bit)));
      }
      return result;
    }
    template<arm_observation C>
    constexpr feature_observation<arm_feature> decode_arm_features(C const & cpu) noexcept {
      feature_observation<arm_feature> result;
      result.observed.set(arm_feature::neon,cpu.baseline_observed);
      result.present.set(arm_feature::neon,cpu.baseline_observed && cpu.fp && cpu.asimd);
      result.observed.set(arm_feature::neon_fp16,cpu.fp16_observed);
      result.present.set(arm_feature::neon_fp16,cpu.fp16_observed && cpu.scalar_fp16 && cpu.vector_fp16);
      result.observed.set(arm_feature::neon_bf16,cpu.bf16_observed);
      result.present.set(arm_feature::neon_bf16,cpu.bf16_observed && cpu.bf16);
      if constexpr(requires { cpu.ebf16_observed; cpu.ebf16; }) {
        result.observed.set(arm_feature::ebf16,cpu.ebf16_observed);
        result.present.set(arm_feature::ebf16,cpu.ebf16_observed && cpu.ebf16);
      }
      constexpr auto baseline=arm_feature::neon&arm_feature::neon_fp16&arm_feature::neon_bf16&arm_feature::ebf16;
      if constexpr(requires { cpu.extra_observed; cpu.extra_features; })
        for(auto const & entry:feature_registry) {
          if(entry.location!=feature_register::arm || baseline.has(entry.value)) continue;
          auto f=static_cast<arm_feature>(entry.index);
          auto observed=cpu.extra_observed.has(f);
          result.observed.set(f,observed);
          result.present.set(f,observed && cpu.extra_features.has(f));
        }
      return result;
    }
    template<instruction_feature E>
    constexpr isa_admission classify_features(feature_set<E> present,feature_set<E> observed,isa bits) noexcept {
      isa_admission result;
      auto known=std::same_as<E,x86_feature> ? x86_features : arm_features;
      result.invalid_features=!present.valid() || !observed.valid() || !(bits<=known);
      auto available=intersection(isa(present),isa(observed));
      result.missing_features=intersection(bits,known);
      for(std::size_t i=0;i<available.flags.size();++i)
        result.missing_features.flags[i]&=~available.flags[i];
      return result;
    }
    constexpr isa_admission classify_x86_features(feature_set<x86_feature> present,
        feature_set<x86_feature> observed,std::uint64_t xcr0,bool readable,isa bits) noexcept {
      auto result=classify_features(present,observed,bits);
      if(bits.has(x86_feature::avx)) {
        result.missing_xcr0_observation=!readable;
        result.missing_xcr0=(bits.has(x86_feature::avx512f)?0xe6ull:0x6ull)&~(readable?xcr0:0ull);
      }
      return result;
    }
  }

  /// Classify normalized x86 features and independently observed OS vector state.
  /// Both present and observed must contain each required feature. Raw diagnostics
  /// are not read; changing them does not change the normalized observation.
  template<class C> requires detail::normalized_features<C,x86_feature> && requires(C const & c) { c.xcr0; c.xcr0_observed; }
  constexpr isa_admission classify_isa(C const & cpu,isa requested,isa minimum={}) noexcept {
    return detail::classify_x86_features(cpu.present,cpu.observed,cpu.xcr0,cpu.xcr0_observed,
      feature_closure(requested&minimum));
  }

  /// Classify normalized ARM features. A feature must be both present and observed.
  /// Raw query diagnostics do not override the normalized observation.
  template<class C> requires detail::normalized_features<C,arm_feature>
  constexpr isa_admission classify_isa(C const & cpu,isa requested,isa minimum={}) noexcept {
    return detail::classify_features(cpu.present,cpu.observed,feature_closure(requested&minimum));
  }

  /// Decode a structural raw x86 snapshot before applying the same admission rules.
  /// Unsupported leaves and unread XCR0 cannot authorize stale positive values.
  template<detail::x86_observation C> requires (!detail::normalized_features<C,x86_feature>) && requires(C const & c) { c.xcr0; c.xcr0_observed; }
  constexpr isa_admission classify_isa(C const & cpu,isa requested,isa minimum={}) noexcept {
    auto features=detail::decode_x86_features(cpu);
    bool readable=cpu.max_basic_leaf>=1 && (cpu.leaf1_ecx&(1u<<26)) &&
      (cpu.leaf1_ecx&(1u<<27)) && cpu.xcr0_observed;
    return detail::classify_x86_features(features.present,features.observed,cpu.xcr0,readable,
      feature_closure(requested&minimum));
  }

  /// Decode a structural raw ARM snapshot before applying the same admission rules.
  /// Failed OS queries cannot authorize stale positive values.
  template<detail::arm_observation C> requires (!detail::normalized_features<C,arm_feature>)
  constexpr isa_admission classify_isa(C const & cpu,isa requested,isa minimum={}) noexcept {
    auto features=detail::decode_arm_features(cpu);
    return detail::classify_features(features.present,features.observed,feature_closure(requested&minimum));
  }

  /// A source variant retains its requested ISA and inherited compiler minimum.
  struct target_entry {
    isa architecture;
    isa minimum{};
    /// Compare the requested ISA and inherited compiler minimum exactly.
    constexpr bool operator==(target_entry const &) const = default;
  };
  template<auto... Entries> struct isa_list {};
  /// Concatenate ordered source metadata without sorting or deduplicating it.
  template<auto... A,auto... B>
  constexpr isa_list<A...,B...> operator+(isa_list<A...>,isa_list<B...>) noexcept { return {}; }

  namespace detail {
    template<auto E,int I> struct abi_match {
      static constexpr target_entry entry=[] {
        if constexpr(arch<std::remove_cv_t<decltype(E)>>) return target_entry{E};
        else return E;
      }();
      static constexpr isa architecture=entry.architecture;
      static constexpr isa minimum=entry.minimum;
      static_assert(architecture<=known_features,"ABI policy contains an unregistered ISA feature");
      static_assert(minimum<=known_features,"ABI policy minimum contains an unregistered ISA feature");
      static constexpr isa required_features=feature_closure(architecture&minimum);
      static constexpr bool matched=true;
      static constexpr int index=I;
    };
    template<isa A,int I,auto... Entries> struct abi_lookup_impl {
      static constexpr bool matched=false;
      static constexpr int index=-1;
      static constexpr isa architecture{},minimum{},required_features{};
    };
    template<isa A,int I,auto E,auto... Rest>
    struct abi_lookup_impl<A,I,E,Rest...> : std::conditional_t<
      abi_match<E,I>::required_features<=A,
      abi_match<E,I>,abi_lookup_impl<A,I+1,Rest...>> {};
  }
  /// Internal value-list selection retaining compiler-implied requirements.
  /// Unlike target's exact set selection, this includes prerequisite closure.
  template<isa A,class List> struct abi_lookup;
  template<isa A,auto... Entries>
  struct abi_lookup<A,isa_list<Entries...>> : detail::abi_lookup_impl<A,0,Entries...> {};

  /// Admit the first available source variant and invoke callback.operator()<A>().
  template<auto... Entries,class C,class F>
  constexpr bool with_isa(isa_list<Entries...>,C const & cpu,F && callback,isa minimum={}) {
    bool selected=false;
    auto try_entry=[&]<auto E>() {
      using entry=detail::abi_match<E,0>;
      if(!selected && classify_isa(cpu,entry::architecture,minimum&entry::minimum).admitted()) {
        selected=true;
        std::forward<F>(callback).template operator()<entry::architecture>();
      }
    };
    (try_entry.template operator()<Entries>(),...);
    return selected;
  }
}
