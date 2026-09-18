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

namespace simd {
  /// Independent instruction features, represented by ordinal bit indices.
  enum class feature : std::uint64_t {
    mmx, sse, sse2, sse3, ssse3, sse41,
    sse42, popcnt, avx, avx2, fma, f16c,
    bmi1, bmi2, avx512f, avx512dq, avx512bw, avx512vl,
    avx512bf16, avx512fp16, neon, neon_fp16, neon_bf16, aes,
    pclmul, cx16, avx512cd, avx512ifma, lzcnt, movbe,
    sahf, arm_aes, arm_sha2, arm_sha3, arm_crc, arm_lse,
    arm_rdm, arm_fp16fml, arm_dotprod, arm_complxnum, arm_jsconv, arm_rcpc,
    arm_pauth,
    invalid_features // Reserved admission failure marker, never a registered instruction.
  };
  inline constexpr std::size_t feature_count=std::size_t(feature::invalid_features)+1;

  /// A structural instruction-feature set. Construction never adds prerequisites.
  struct isa {
    std::array<std::uint64_t,(feature_count+63)/64> flags{};

    constexpr isa() noexcept = default;
    constexpr isa(feature f) noexcept { set(f,true); }
    constexpr bool get(feature f) const noexcept {
      auto i=std::size_t(f);
      return (flags[i/64]>>(i%64))&1;
    }
    constexpr void set(feature f,bool value) noexcept {
      auto i=std::size_t(f);
      auto mask=std::uint64_t{1}<<(i%64);
      auto & word=flags[i/64];
      word=(word&~mask)|(value?mask:0);
    }
    constexpr bool has(feature f) const noexcept { return get(f); }
    constexpr bool has(isa other) const noexcept {
      for(std::size_t i=0;i<flags.size();++i)
        if((flags[i]&other.flags[i])!=other.flags[i]) return false;
      return true;
    }
    constexpr bool operator==(isa const &) const = default;

    constexpr bool get_mmx() const noexcept { return get(feature::mmx); }
    constexpr void set_mmx(bool value) noexcept { set(feature::mmx,value); }
    __declspec(property(get=get_mmx,put=set_mmx)) bool mmx;
    constexpr bool get_sse() const noexcept { return get(feature::sse); }
    constexpr void set_sse(bool value) noexcept { set(feature::sse,value); }
    __declspec(property(get=get_sse,put=set_sse)) bool sse;
    constexpr bool get_sse2() const noexcept { return get(feature::sse2); }
    constexpr void set_sse2(bool value) noexcept { set(feature::sse2,value); }
    __declspec(property(get=get_sse2,put=set_sse2)) bool sse2;
    constexpr bool get_sse3() const noexcept { return get(feature::sse3); }
    constexpr void set_sse3(bool value) noexcept { set(feature::sse3,value); }
    __declspec(property(get=get_sse3,put=set_sse3)) bool sse3;
    constexpr bool get_ssse3() const noexcept { return get(feature::ssse3); }
    constexpr void set_ssse3(bool value) noexcept { set(feature::ssse3,value); }
    __declspec(property(get=get_ssse3,put=set_ssse3)) bool ssse3;
    constexpr bool get_sse41() const noexcept { return get(feature::sse41); }
    constexpr void set_sse41(bool value) noexcept { set(feature::sse41,value); }
    __declspec(property(get=get_sse41,put=set_sse41)) bool sse41;
    constexpr bool get_sse42() const noexcept { return get(feature::sse42); }
    constexpr void set_sse42(bool value) noexcept { set(feature::sse42,value); }
    __declspec(property(get=get_sse42,put=set_sse42)) bool sse42;
    constexpr bool get_popcnt() const noexcept { return get(feature::popcnt); }
    constexpr void set_popcnt(bool value) noexcept { set(feature::popcnt,value); }
    __declspec(property(get=get_popcnt,put=set_popcnt)) bool popcnt;
    constexpr bool get_avx() const noexcept { return get(feature::avx); }
    constexpr void set_avx(bool value) noexcept { set(feature::avx,value); }
    __declspec(property(get=get_avx,put=set_avx)) bool avx;
    constexpr bool get_avx2() const noexcept { return get(feature::avx2); }
    constexpr void set_avx2(bool value) noexcept { set(feature::avx2,value); }
    __declspec(property(get=get_avx2,put=set_avx2)) bool avx2;
    constexpr bool get_fma() const noexcept { return get(feature::fma); }
    constexpr void set_fma(bool value) noexcept { set(feature::fma,value); }
    __declspec(property(get=get_fma,put=set_fma)) bool fma;
    constexpr bool get_f16c() const noexcept { return get(feature::f16c); }
    constexpr void set_f16c(bool value) noexcept { set(feature::f16c,value); }
    __declspec(property(get=get_f16c,put=set_f16c)) bool f16c;
    constexpr bool get_bmi1() const noexcept { return get(feature::bmi1); }
    constexpr void set_bmi1(bool value) noexcept { set(feature::bmi1,value); }
    __declspec(property(get=get_bmi1,put=set_bmi1)) bool bmi1;
    constexpr bool get_bmi2() const noexcept { return get(feature::bmi2); }
    constexpr void set_bmi2(bool value) noexcept { set(feature::bmi2,value); }
    __declspec(property(get=get_bmi2,put=set_bmi2)) bool bmi2;
    constexpr bool get_avx512f() const noexcept { return get(feature::avx512f); }
    constexpr void set_avx512f(bool value) noexcept { set(feature::avx512f,value); }
    __declspec(property(get=get_avx512f,put=set_avx512f)) bool avx512f;
    constexpr bool get_avx512dq() const noexcept { return get(feature::avx512dq); }
    constexpr void set_avx512dq(bool value) noexcept { set(feature::avx512dq,value); }
    __declspec(property(get=get_avx512dq,put=set_avx512dq)) bool avx512dq;
    constexpr bool get_avx512bw() const noexcept { return get(feature::avx512bw); }
    constexpr void set_avx512bw(bool value) noexcept { set(feature::avx512bw,value); }
    __declspec(property(get=get_avx512bw,put=set_avx512bw)) bool avx512bw;
    constexpr bool get_avx512vl() const noexcept { return get(feature::avx512vl); }
    constexpr void set_avx512vl(bool value) noexcept { set(feature::avx512vl,value); }
    __declspec(property(get=get_avx512vl,put=set_avx512vl)) bool avx512vl;
    constexpr bool get_avx512bf16() const noexcept { return get(feature::avx512bf16); }
    constexpr void set_avx512bf16(bool value) noexcept { set(feature::avx512bf16,value); }
    __declspec(property(get=get_avx512bf16,put=set_avx512bf16)) bool avx512bf16;
    constexpr bool get_avx512fp16() const noexcept { return get(feature::avx512fp16); }
    constexpr void set_avx512fp16(bool value) noexcept { set(feature::avx512fp16,value); }
    __declspec(property(get=get_avx512fp16,put=set_avx512fp16)) bool avx512fp16;
    constexpr bool get_neon() const noexcept { return get(feature::neon); }
    constexpr void set_neon(bool value) noexcept { set(feature::neon,value); }
    __declspec(property(get=get_neon,put=set_neon)) bool neon;
    constexpr bool get_neon_fp16() const noexcept { return get(feature::neon_fp16); }
    constexpr void set_neon_fp16(bool value) noexcept { set(feature::neon_fp16,value); }
    __declspec(property(get=get_neon_fp16,put=set_neon_fp16)) bool neon_fp16;
    constexpr bool get_neon_bf16() const noexcept { return get(feature::neon_bf16); }
    constexpr void set_neon_bf16(bool value) noexcept { set(feature::neon_bf16,value); }
    __declspec(property(get=get_neon_bf16,put=set_neon_bf16)) bool neon_bf16;
    constexpr bool get_aes() const noexcept { return get(feature::aes); }
    constexpr void set_aes(bool value) noexcept { set(feature::aes,value); }
    __declspec(property(get=get_aes,put=set_aes)) bool aes;
    constexpr bool get_pclmul() const noexcept { return get(feature::pclmul); }
    constexpr void set_pclmul(bool value) noexcept { set(feature::pclmul,value); }
    __declspec(property(get=get_pclmul,put=set_pclmul)) bool pclmul;
    constexpr bool get_cx16() const noexcept { return get(feature::cx16); }
    constexpr void set_cx16(bool value) noexcept { set(feature::cx16,value); }
    __declspec(property(get=get_cx16,put=set_cx16)) bool cx16;
    constexpr bool get_avx512cd() const noexcept { return get(feature::avx512cd); }
    constexpr void set_avx512cd(bool value) noexcept { set(feature::avx512cd,value); }
    __declspec(property(get=get_avx512cd,put=set_avx512cd)) bool avx512cd;
    constexpr bool get_avx512ifma() const noexcept { return get(feature::avx512ifma); }
    constexpr void set_avx512ifma(bool value) noexcept { set(feature::avx512ifma,value); }
    __declspec(property(get=get_avx512ifma,put=set_avx512ifma)) bool avx512ifma;
    constexpr bool get_lzcnt() const noexcept { return get(feature::lzcnt); }
    constexpr void set_lzcnt(bool value) noexcept { set(feature::lzcnt,value); }
    __declspec(property(get=get_lzcnt,put=set_lzcnt)) bool lzcnt;
    constexpr bool get_movbe() const noexcept { return get(feature::movbe); }
    constexpr void set_movbe(bool value) noexcept { set(feature::movbe,value); }
    __declspec(property(get=get_movbe,put=set_movbe)) bool movbe;
    constexpr bool get_sahf() const noexcept { return get(feature::sahf); }
    constexpr void set_sahf(bool value) noexcept { set(feature::sahf,value); }
    __declspec(property(get=get_sahf,put=set_sahf)) bool sahf;
    constexpr bool get_arm_aes() const noexcept { return get(feature::arm_aes); }
    constexpr void set_arm_aes(bool value) noexcept { set(feature::arm_aes,value); }
    __declspec(property(get=get_arm_aes,put=set_arm_aes)) bool arm_aes;
    constexpr bool get_arm_sha2() const noexcept { return get(feature::arm_sha2); }
    constexpr void set_arm_sha2(bool value) noexcept { set(feature::arm_sha2,value); }
    __declspec(property(get=get_arm_sha2,put=set_arm_sha2)) bool arm_sha2;
    constexpr bool get_arm_sha3() const noexcept { return get(feature::arm_sha3); }
    constexpr void set_arm_sha3(bool value) noexcept { set(feature::arm_sha3,value); }
    __declspec(property(get=get_arm_sha3,put=set_arm_sha3)) bool arm_sha3;
    constexpr bool get_arm_crc() const noexcept { return get(feature::arm_crc); }
    constexpr void set_arm_crc(bool value) noexcept { set(feature::arm_crc,value); }
    __declspec(property(get=get_arm_crc,put=set_arm_crc)) bool arm_crc;
    constexpr bool get_arm_lse() const noexcept { return get(feature::arm_lse); }
    constexpr void set_arm_lse(bool value) noexcept { set(feature::arm_lse,value); }
    __declspec(property(get=get_arm_lse,put=set_arm_lse)) bool arm_lse;
    constexpr bool get_arm_rdm() const noexcept { return get(feature::arm_rdm); }
    constexpr void set_arm_rdm(bool value) noexcept { set(feature::arm_rdm,value); }
    __declspec(property(get=get_arm_rdm,put=set_arm_rdm)) bool arm_rdm;
    constexpr bool get_arm_fp16fml() const noexcept { return get(feature::arm_fp16fml); }
    constexpr void set_arm_fp16fml(bool value) noexcept { set(feature::arm_fp16fml,value); }
    __declspec(property(get=get_arm_fp16fml,put=set_arm_fp16fml)) bool arm_fp16fml;
    constexpr bool get_arm_dotprod() const noexcept { return get(feature::arm_dotprod); }
    constexpr void set_arm_dotprod(bool value) noexcept { set(feature::arm_dotprod,value); }
    __declspec(property(get=get_arm_dotprod,put=set_arm_dotprod)) bool arm_dotprod;
    constexpr bool get_arm_complxnum() const noexcept { return get(feature::arm_complxnum); }
    constexpr void set_arm_complxnum(bool value) noexcept { set(feature::arm_complxnum,value); }
    __declspec(property(get=get_arm_complxnum,put=set_arm_complxnum)) bool arm_complxnum;
    constexpr bool get_arm_jsconv() const noexcept { return get(feature::arm_jsconv); }
    constexpr void set_arm_jsconv(bool value) noexcept { set(feature::arm_jsconv,value); }
    __declspec(property(get=get_arm_jsconv,put=set_arm_jsconv)) bool arm_jsconv;
    constexpr bool get_arm_rcpc() const noexcept { return get(feature::arm_rcpc); }
    constexpr void set_arm_rcpc(bool value) noexcept { set(feature::arm_rcpc,value); }
    __declspec(property(get=get_arm_rcpc,put=set_arm_rcpc)) bool arm_rcpc;
    constexpr bool get_arm_pauth() const noexcept { return get(feature::arm_pauth); }
    constexpr void set_arm_pauth(bool value) noexcept { set(feature::arm_pauth,value); }
    __declspec(property(get=get_arm_pauth,put=set_arm_pauth)) bool arm_pauth;
  };

  template<class T> concept arch=std::same_as<T,feature> || std::same_as<T,isa>;

  /// Requirements compose by union: both operands must be available.
  constexpr isa operator&(arch auto left,arch auto right) noexcept {
    isa result=left, other=right;
    for(std::size_t i=0;i<result.flags.size();++i) result.flags[i]|=other.flags[i];
    return result;
  }
  // Concrete enum overloads prevent the built-in ordinal comparisons from winning.
  constexpr bool operator<(feature,feature) noexcept { return false; }
  constexpr bool operator>(feature,feature) noexcept { return false; }
  constexpr bool operator<=(feature a,feature b) noexcept { return a==b; }
  constexpr bool operator>=(feature a,feature b) noexcept { return a==b; }
  constexpr bool operator<=(arch auto a,arch auto b) noexcept { return isa(b).has(a); }
  constexpr bool operator>=(arch auto a,arch auto b) noexcept { return b<=a; }
  constexpr bool operator<(arch auto a,arch auto b) noexcept { return isa(a)!=isa(b) && a<=b; }
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
    enum class feature_register { leaf1_ecx, leaf1_edx, leaf7_ebx, leaf7_edx, leaf7_1_eax, extended1_ecx, arm };
    struct feature_record {
      feature value;
      std::string_view spelling;
      isa implies;
      feature_register location;
      unsigned bit;
    };
    // Clang target-feature dependencies, not an assertion that one CPU feature
    // bit alone guarantees another. Admission checks every bit in the closure.
    inline constexpr feature_record feature_registry[] = {
      {feature::mmx,"mmx",{},feature_register::leaf1_edx,23},
      {feature::sse,"sse",isa(feature::mmx),feature_register::leaf1_edx,25},
      {feature::sse2,"sse2",isa(feature::sse),feature_register::leaf1_edx,26},
      {feature::sse3,"sse3",isa(feature::sse2),feature_register::leaf1_ecx,0},
      {feature::ssse3,"ssse3",isa(feature::sse3),feature_register::leaf1_ecx,9},
      {feature::sse41,"sse4.1",isa(feature::ssse3),feature_register::leaf1_ecx,19},
      {feature::sse42,"sse4.2",feature::sse41&feature::popcnt,feature_register::leaf1_ecx,20},
      {feature::popcnt,"popcnt",{},feature_register::leaf1_ecx,23},
      {feature::avx,"avx",isa(feature::sse42),feature_register::leaf1_ecx,28},
      {feature::avx2,"avx2",isa(feature::avx),feature_register::leaf7_ebx,5},
      {feature::fma,"fma",isa(feature::avx),feature_register::leaf1_ecx,12},
      {feature::f16c,"f16c",isa(feature::avx),feature_register::leaf1_ecx,29},
      {feature::bmi1,"bmi",{},feature_register::leaf7_ebx,3},
      {feature::bmi2,"bmi2",{},feature_register::leaf7_ebx,8},
      {feature::avx512f,"avx512f",feature::avx2&feature::f16c&feature::fma,feature_register::leaf7_ebx,16},
      {feature::avx512dq,"avx512dq",isa(feature::avx512f),feature_register::leaf7_ebx,17},
      {feature::avx512bw,"avx512bw",isa(feature::avx512f),feature_register::leaf7_ebx,30},
      {feature::avx512vl,"avx512vl",isa(feature::avx512f),feature_register::leaf7_ebx,31},
      {feature::avx512bf16,"avx512bf16",isa(feature::avx512bw),feature_register::leaf7_1_eax,5},
      {feature::avx512fp16,"avx512fp16",isa(feature::avx512bw),feature_register::leaf7_edx,23},
      {feature::neon,"neon",{},feature_register::arm,0},
      {feature::neon_fp16,"fullfp16",isa(feature::neon),feature_register::arm,1},
      {feature::neon_bf16,"bf16",isa(feature::neon),feature_register::arm,2},
      {feature::aes,"aes",isa(feature::sse2),feature_register::leaf1_ecx,25},
      {feature::pclmul,"pclmul",isa(feature::sse2),feature_register::leaf1_ecx,1},
      {feature::cx16,"cx16",{},feature_register::leaf1_ecx,13},
      {feature::avx512cd,"avx512cd",isa(feature::avx512f),feature_register::leaf7_ebx,28},
      {feature::avx512ifma,"avx512ifma",isa(feature::avx512f),feature_register::leaf7_ebx,21},
      {feature::lzcnt,"lzcnt",{},feature_register::extended1_ecx,5},
      {feature::movbe,"movbe",{},feature_register::leaf1_ecx,22},
      {feature::sahf,"sahf",{},feature_register::extended1_ecx,0},
      {feature::arm_aes,"aes",isa(feature::neon),feature_register::arm,3},
      {feature::arm_sha2,"sha2",isa(feature::neon),feature_register::arm,4},
      {feature::arm_sha3,"sha3",isa(feature::arm_sha2),feature_register::arm,5},
      {feature::arm_crc,"crc",isa(feature::neon),feature_register::arm,6},
      {feature::arm_lse,"lse",isa(feature::neon),feature_register::arm,7},
      {feature::arm_rdm,"rdm",isa(feature::neon),feature_register::arm,8},
      {feature::arm_fp16fml,"fp16fml",isa(feature::neon_fp16),feature_register::arm,9},
      {feature::arm_dotprod,"dotprod",isa(feature::neon),feature_register::arm,10},
      {feature::arm_complxnum,"complxnum",isa(feature::neon),feature_register::arm,11},
      {feature::arm_jsconv,"jsconv",isa(feature::neon),feature_register::arm,12},
      {feature::arm_rcpc,"rcpc",isa(feature::neon),feature_register::arm,13},
      {feature::arm_pauth,"pauth",isa(feature::neon),feature_register::arm,14}
    };
    inline constexpr isa arm_features=[] {
      isa result;
      for(auto const & entry:feature_registry)
        if(entry.location==feature_register::arm) result.set(entry.value,true);
      return result;
    }();
    inline constexpr isa x86_features=[] {
      isa result;
      for(auto const & entry:feature_registry)
        if(entry.location!=feature_register::arm) result.set(entry.value,true);
      return result;
    }();
    inline constexpr isa known_features=arm_features&x86_features;
    inline constexpr isa invalid_features=feature::invalid_features;
  }

  /// Explicit compiler-implied closure, shared by presets and admission.
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
  inline constexpr isa avx2=feature_closure(feature::avx2&feature::fma&feature::bmi2);
  inline constexpr isa avx512=feature_closure(avx2&feature::avx512f&feature::avx512dq&feature::avx512bw&feature::avx512vl);
  inline constexpr isa avx512_bf16=feature_closure(avx512&feature::avx512bf16);
  inline constexpr isa avx512_fp16=feature_closure(avx512&feature::avx512fp16);
  inline constexpr isa neon=feature_closure(feature::neon);
  inline constexpr isa neon_fp16=feature_closure(feature::neon_fp16);
  inline constexpr isa neon_bf16=feature_closure(feature::neon_bf16);

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
      for(auto const & entry:detail::feature_registry) if(token==entry.spelling &&
          (entry.spelling!="aes" || (entry.location==detail::feature_register::arm)==arm_target)) {
        bits.set(entry.value,true); found=true; break;
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
  };

  namespace detail {
    template<class C> concept x86_observation=requires(C const & c) {
      c.max_basic_leaf; c.leaf1_ecx; c.leaf1_edx; c.leaf7_ebx;
      c.max_leaf7_subleaf; c.leaf7_1_eax; c.leaf7_edx;
      c.xcr0; c.xcr0_observed;
    };
    template<class C> concept arm_observation=requires(C const & c) {
      c.baseline_observed; c.fp; c.asimd;
      c.fp16_observed; c.scalar_fp16; c.vector_fp16;
      c.bf16_observed; c.bf16;
    };
  }

  /// Pure classification of the existing simd.cpuid observation record.
  template<detail::x86_observation C>
  constexpr isa_admission classify_isa(C const & cpu, isa requested,isa minimum={}) noexcept {
    auto bits=feature_closure(requested&minimum);
    isa_admission result;
    result.invalid_features=!(bits<=detail::x86_features);
    for(auto const & entry:detail::feature_registry) {
      if(!bits.has(entry.value)) continue;
      std::uint32_t observed=0;
      switch(entry.location) {
        case detail::feature_register::leaf1_ecx: if(cpu.max_basic_leaf>=1) observed=cpu.leaf1_ecx; break;
        case detail::feature_register::leaf1_edx: if(cpu.max_basic_leaf>=1) observed=cpu.leaf1_edx; break;
        case detail::feature_register::leaf7_ebx: if(cpu.max_basic_leaf>=7) observed=cpu.leaf7_ebx; break;
        case detail::feature_register::leaf7_edx: if(cpu.max_basic_leaf>=7) observed=cpu.leaf7_edx; break;
        case detail::feature_register::leaf7_1_eax: if(cpu.max_basic_leaf>=7 && cpu.max_leaf7_subleaf>=1) observed=cpu.leaf7_1_eax; break;
        case detail::feature_register::extended1_ecx:
          if constexpr(requires { cpu.max_extended_leaf; cpu.extended1_ecx; })
            if(cpu.max_extended_leaf>=0x80000001u) observed=cpu.extended1_ecx;
          break;
        case detail::feature_register::arm: break;
      }
      if(!(observed&(std::uint32_t(1)<<entry.bit))) result.missing_features.set(entry.value,true);
    }
    if(bits.has(feature::avx)) {
      bool readable=cpu.max_basic_leaf>=1 && (cpu.leaf1_ecx&(1u<<26)) &&
        (cpu.leaf1_ecx&(1u<<27)) && cpu.xcr0_observed;
      result.missing_xcr0_observation=!readable;
      result.missing_xcr0=((bits.has(feature::avx512f))?0xe6ull:0x6ull)&~(readable?cpu.xcr0:0ull);
    }
    return result;
  }

  /// Pure classification of the existing simd.arm observation record.
  template<detail::arm_observation C>
  constexpr isa_admission classify_isa(C const & cpu,isa requested,isa minimum={}) noexcept {
    auto bits=feature_closure(requested&minimum);
    isa_admission result;
    result.invalid_features=!(bits<=detail::arm_features);
    if(bits.neon && !(cpu.baseline_observed && cpu.fp && cpu.asimd))
      result.missing_features.neon=true;
    if(bits.neon_fp16 && !(cpu.fp16_observed && cpu.scalar_fp16 && cpu.vector_fp16))
      result.missing_features.neon_fp16=true;
    if(bits.neon_bf16 && !(cpu.bf16_observed && cpu.bf16))
      result.missing_features.neon_bf16=true;
    constexpr auto baseline=feature::neon&feature::neon_fp16&feature::neon_bf16;
    for(auto const & entry:detail::feature_registry) {
      if(entry.location!=detail::feature_register::arm || baseline.has(entry.value) || !bits.has(entry.value)) continue;
      bool available=false;
      if constexpr(requires { cpu.extra_observed; cpu.extra_features; })
        available=cpu.extra_observed.has(entry.value) && cpu.extra_features.has(entry.value);
      if(!available) result.missing_features.set(entry.value,true);
    }
    return result;
  }

  /// A source variant retains its requested ISA and inherited compiler minimum.
  struct target_entry {
    isa architecture;
    isa minimum{};
    constexpr bool operator==(target_entry const &) const = default;
  };
  template<auto... Entries> struct isa_list {};
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
