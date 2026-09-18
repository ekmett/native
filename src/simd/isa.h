// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <concepts>
#include <cstdint>
#include <string_view>
#include <type_traits>
#include <utility>

namespace simd {
  using feature_set = std::uint64_t;

  /// Independent instruction features. Presets below are convenience aliases.
  enum class feature : feature_set {
    mmx=1ull<<0, sse=1ull<<1, sse2=1ull<<2, sse3=1ull<<3,
    ssse3=1ull<<4, sse41=1ull<<5, sse42=1ull<<6, popcnt=1ull<<7,
    avx=1ull<<8, avx2=1ull<<9, fma=1ull<<10, f16c=1ull<<11,
    bmi1=1ull<<12, bmi2=1ull<<13, avx512f=1ull<<14,
    avx512dq=1ull<<15, avx512bw=1ull<<16, avx512vl=1ull<<17,
    avx512_bf16=1ull<<18, avx512_fp16=1ull<<19,
    neon=1ull<<20, neon_fp16=1ull<<21, neon_bf16=1ull<<22,
    aes=1ull<<23, pclmul=1ull<<24, cx16=1ull<<25,
    avx512cd=1ull<<26, avx512ifma=1ull<<27,
    lzcnt=1ull<<28, movbe=1ull<<29, sahf=1ull<<30,
    arm_aes=1ull<<31, arm_sha2=1ull<<32, arm_sha3=1ull<<33,
    arm_crc=1ull<<34, arm_lse=1ull<<35, arm_rdm=1ull<<36,
    arm_fp16fml=1ull<<37, arm_dotprod=1ull<<38,
    arm_complxnum=1ull<<39, arm_jsconv=1ull<<40,
    arm_rcpc=1ull<<41, arm_pauth=1ull<<42
  };
  /// Combine two independent feature bits into a set.
  constexpr feature_set operator|(feature a, feature b) noexcept {
    return feature_set(a)|feature_set(b);
  }
  /// Add one feature to an existing set.
  constexpr feature_set operator|(feature_set a, feature b) noexcept {
    return a|feature_set(b);
  }
  /// Add one feature to an existing set, with the bit on the left.
  constexpr feature_set operator|(feature a, feature_set b) noexcept {
    return feature_set(a)|b;
  }

  namespace detail {
    enum class feature_register { leaf1_ecx, leaf1_edx, leaf7_ebx, leaf7_edx, leaf7_1_eax, extended1_ecx, arm };
    struct feature_record {
      feature value;
      std::string_view spelling;
      feature_set implies;
      feature_register location;
      unsigned bit;
    };
    // Clang target-feature dependencies, not an assertion that one CPU feature
    // bit alone guarantees another. Admission checks every bit in the closure.
    inline constexpr feature_record feature_registry[] = {
      {feature::mmx,"mmx",0,feature_register::leaf1_edx,23},
      {feature::sse,"sse",feature_set(feature::mmx),feature_register::leaf1_edx,25},
      {feature::sse2,"sse2",feature_set(feature::sse),feature_register::leaf1_edx,26},
      {feature::sse3,"sse3",feature_set(feature::sse2),feature_register::leaf1_ecx,0},
      {feature::ssse3,"ssse3",feature_set(feature::sse3),feature_register::leaf1_ecx,9},
      {feature::sse41,"sse4.1",feature_set(feature::ssse3),feature_register::leaf1_ecx,19},
      {feature::sse42,"sse4.2",feature::sse41|feature::popcnt,feature_register::leaf1_ecx,20},
      {feature::popcnt,"popcnt",0,feature_register::leaf1_ecx,23},
      {feature::avx,"avx",feature_set(feature::sse42),feature_register::leaf1_ecx,28},
      {feature::avx2,"avx2",feature_set(feature::avx),feature_register::leaf7_ebx,5},
      {feature::fma,"fma",feature_set(feature::avx),feature_register::leaf1_ecx,12},
      {feature::f16c,"f16c",feature_set(feature::avx),feature_register::leaf1_ecx,29},
      {feature::bmi1,"bmi",0,feature_register::leaf7_ebx,3},
      {feature::bmi2,"bmi2",0,feature_register::leaf7_ebx,8},
      {feature::avx512f,"avx512f",feature::avx2|feature::f16c|feature::fma,feature_register::leaf7_ebx,16},
      {feature::avx512dq,"avx512dq",feature_set(feature::avx512f),feature_register::leaf7_ebx,17},
      {feature::avx512bw,"avx512bw",feature_set(feature::avx512f),feature_register::leaf7_ebx,30},
      {feature::avx512vl,"avx512vl",feature_set(feature::avx512f),feature_register::leaf7_ebx,31},
      {feature::avx512_bf16,"avx512bf16",feature_set(feature::avx512bw),feature_register::leaf7_1_eax,5},
      {feature::avx512_fp16,"avx512fp16",feature_set(feature::avx512bw),feature_register::leaf7_edx,23},
      {feature::neon,"neon",0,feature_register::arm,0},
      {feature::neon_fp16,"fullfp16",feature_set(feature::neon),feature_register::arm,1},
      {feature::neon_bf16,"bf16",feature_set(feature::neon),feature_register::arm,2},
      {feature::aes,"aes",feature_set(feature::sse2),feature_register::leaf1_ecx,25},
      {feature::pclmul,"pclmul",feature_set(feature::sse2),feature_register::leaf1_ecx,1},
      {feature::cx16,"cx16",0,feature_register::leaf1_ecx,13},
      {feature::avx512cd,"avx512cd",feature_set(feature::avx512f),feature_register::leaf7_ebx,28},
      {feature::avx512ifma,"avx512ifma",feature_set(feature::avx512f),feature_register::leaf7_ebx,21},
      {feature::lzcnt,"lzcnt",0,feature_register::extended1_ecx,5},
      {feature::movbe,"movbe",0,feature_register::leaf1_ecx,22},
      {feature::sahf,"sahf",0,feature_register::extended1_ecx,0},
      {feature::arm_aes,"aes",feature_set(feature::neon),feature_register::arm,3},
      {feature::arm_sha2,"sha2",feature_set(feature::neon),feature_register::arm,4},
      {feature::arm_sha3,"sha3",feature_set(feature::arm_sha2),feature_register::arm,5},
      {feature::arm_crc,"crc",feature_set(feature::neon),feature_register::arm,6},
      {feature::arm_lse,"lse",feature_set(feature::neon),feature_register::arm,7},
      {feature::arm_rdm,"rdm",feature_set(feature::neon),feature_register::arm,8},
      {feature::arm_fp16fml,"fp16fml",feature_set(feature::neon_fp16),feature_register::arm,9},
      {feature::arm_dotprod,"dotprod",feature_set(feature::neon),feature_register::arm,10},
      {feature::arm_complxnum,"complxnum",feature_set(feature::neon),feature_register::arm,11},
      {feature::arm_jsconv,"jsconv",feature_set(feature::neon),feature_register::arm,12},
      {feature::arm_rcpc,"rcpc",feature_set(feature::neon),feature_register::arm,13},
      {feature::arm_pauth,"pauth",feature_set(feature::neon),feature_register::arm,14}
    };
    inline constexpr feature_set known_features=(1ull<<43)-1;
    inline constexpr feature_set arm_features=(feature::neon|feature::neon_fp16|feature::neon_bf16)|(((1ull<<43)-1)&~((1ull<<31)-1));
    inline constexpr feature_set x86_features=known_features&~arm_features;
    inline constexpr feature_set invalid_features=1ull<<63;
  }

  /// Canonical compiler-implied feature closure, shared by tags and admission.
  constexpr feature_set feature_closure(feature_set bits) noexcept {
    feature_set previous;
    do {
      previous=bits;
      for(auto const & entry:detail::feature_registry)
        if(bits&feature_set(entry.value)) bits|=entry.implies;
    } while(previous!=bits);
    return bits;
  }

  namespace detail {
    template<feature_set Bits> struct isa_tag {
      static_assert((Bits&~known_features)==0,"unknown SIMD ISA feature");
      static_assert(!(Bits&arm_features) || !(Bits&x86_features),"cannot combine x86 and ARM ISA features");
      static constexpr feature_set features=Bits;
    };
    template<class> inline constexpr bool is_isa=false;
    template<feature_set Bits> inline constexpr bool is_isa<isa_tag<Bits>> = true;
  }

  /// Feature order and redundant prerequisites do not create different types.
  template<auto Bits> using isa=detail::isa_tag<feature_closure(feature_set(Bits))>;
  template<class A> concept architecture=detail::is_isa<A>;
  template<class A, feature F> concept has_feature=architecture<A> && ((A::features&feature_set(F))!=0);
  template<class A, feature_set Bits> concept has_features=architecture<A> && ((A::features&Bits)==Bits);

  using scalar=isa<0>;
  using avx2=isa<feature::avx2|feature::fma|feature::bmi2>;
  using avx512=isa<avx2::features|feature::avx512f|feature::avx512dq|feature::avx512bw|feature::avx512vl>;
  using avx512_bf16=isa<avx512::features|feature::avx512_bf16>;
  using avx512_fp16=isa<avx512::features|feature::avx512_fp16>;
  using neon=isa<feature::neon>;
  using neon_fp16=isa<feature::neon_fp16>;
  using neon_bf16=isa<feature::neon_bf16>;
  namespace detail {
    template<class A> concept scalar_architecture=architecture<A> && A::features==0;
    template<class A> concept avx512_base_architecture=has_features<A,isa<avx2::features|feature::avx512f|feature::avx512dq>::features>;
    template<class A> concept avx512_architecture=has_features<A,avx512::features>;
    template<class A> concept avx512_nobw_novl_architecture=avx512_base_architecture<A> && !has_feature<A,feature::avx512bw> && !has_feature<A,feature::avx512vl>;
    template<class A> concept avx512_bw_novl_architecture=avx512_base_architecture<A> && has_feature<A,feature::avx512bw> && !has_feature<A,feature::avx512vl>;
    template<class A> concept avx512_nobw_vl_architecture=avx512_base_architecture<A> && !has_feature<A,feature::avx512bw> && has_feature<A,feature::avx512vl>;
    template<class A> concept avx2_architecture=has_features<A,avx2::features> && !avx512_base_architecture<A>;
    template<class A> concept neon_architecture=has_feature<A,feature::neon>;
    template<class A> concept avx512_bf16_architecture=avx512_architecture<A> && has_feature<A,feature::avx512_bf16>;
    template<class A> concept avx512_fp16_architecture=avx512_architecture<A> && has_feature<A,feature::avx512_fp16>;
    template<class A> concept avx512_half_architecture=avx512_bf16_architecture<A> && avx512_fp16_architecture<A>;
    template<class A> concept neon_bf16_architecture=neon_architecture<A> && has_feature<A,feature::neon_bf16>;
    template<class A> concept neon_fp16_architecture=neon_architecture<A> && has_feature<A,feature::neon_fp16>;
    template<class A> concept neon_half_architecture=neon_bf16_architecture<A> && neon_fp16_architecture<A>;
  }

  /// Parse a registered literal target feature list. CPU names, negative
  /// features and unknown features fail closed rather than guessing admission.
  constexpr feature_set target_features(std::string_view text) noexcept {
    if(text.empty() || text=="default") return 0;
#if defined(__aarch64__) || defined(_M_ARM64)
    bool arm_target=true;
#else
    bool arm_target=false;
#endif
    // Explicit ARM profile strings are also useful in synthetic metadata tests
    // on an x86 host. AES is spelled identically by both compiler backends.
    if(text.find("neon")!=std::string_view::npos || text.find("fullfp16")!=std::string_view::npos)
      arm_target=true;
    feature_set bits=0;
    while(!text.empty()) {
      auto comma=text.find(',');
      auto token=text.substr(0,comma);
      bool found=false;
      for(auto const & entry:detail::feature_registry) if(token==entry.spelling &&
          (entry.spelling!="aes" || (entry.location==detail::feature_register::arm)==arm_target)) {
        bits|=feature_set(entry.value); found=true; break;
      }
      if(!found) return detail::invalid_features;
      if(comma==std::string_view::npos) break;
      text.remove_prefix(comma+1);
      if(text.empty()) return detail::invalid_features;
    }
    return feature_closure(bits);
  }

  struct isa_admission {
    feature_set missing_features=0;
    std::uint64_t missing_xcr0=0;
    bool invalid_features=false;
    bool missing_xcr0_observation=false;
    /// True only when every requested feature and OS state component is present.
    constexpr bool admitted() const noexcept {
      return !missing_features && !missing_xcr0 && !invalid_features && !missing_xcr0_observation;
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
  constexpr isa_admission classify_isa(C const & cpu, feature_set requested) noexcept {
    auto bits=feature_closure(requested);
    isa_admission result;
    result.invalid_features=(bits&~detail::x86_features)!=0;
    for(auto const & entry:detail::feature_registry) {
      if(!(bits&feature_set(entry.value))) continue;
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
      if(!(observed&(std::uint32_t(1)<<entry.bit))) result.missing_features|=feature_set(entry.value);
    }
    if(bits&feature_set(feature::avx)) {
      bool readable=cpu.max_basic_leaf>=1 && (cpu.leaf1_ecx&(1u<<26)) &&
        (cpu.leaf1_ecx&(1u<<27)) && cpu.xcr0_observed;
      result.missing_xcr0_observation=!readable;
      result.missing_xcr0=((bits&feature_set(feature::avx512f))?0xe6ull:0x6ull)&~(readable?cpu.xcr0:0ull);
    }
    return result;
  }

  /// Pure classification of the existing simd.arm observation record.
  template<detail::arm_observation C>
  constexpr isa_admission classify_isa(C const & cpu, feature_set requested) noexcept {
    auto bits=feature_closure(requested);
    isa_admission result;
    result.invalid_features=(bits&~detail::arm_features)!=0;
    if((bits&feature_set(feature::neon)) && !(cpu.baseline_observed && cpu.fp && cpu.asimd))
      result.missing_features|=feature_set(feature::neon);
    if((bits&feature_set(feature::neon_fp16)) && !(cpu.fp16_observed && cpu.scalar_fp16 && cpu.vector_fp16))
      result.missing_features|=feature_set(feature::neon_fp16);
    if((bits&feature_set(feature::neon_bf16)) && !(cpu.bf16_observed && cpu.bf16))
      result.missing_features|=feature_set(feature::neon_bf16);
    constexpr auto baseline=feature::neon|feature::neon_fp16|feature::neon_bf16;
    auto extra=bits&detail::arm_features&~baseline;
    if constexpr(requires { cpu.extra_observed; cpu.extra_features; })
      result.missing_features|=extra&~(cpu.extra_observed&cpu.extra_features);
    else result.missing_features|=extra;
    return result;
  }

  /// Admit a type tag together with an explicit inherited project minimum.
  template<architecture A, class C>
  constexpr isa_admission classify_isa(C const & cpu, A, feature_set minimum=0) noexcept {
    return classify_isa(cpu,A::features|minimum);
  }

  /// A source variant retains its requested type and inherited compiler minimum.
  template<architecture A, feature_set Minimum> struct target_entry {
    static_assert((Minimum&~detail::known_features)==0,"source minimum contains an unregistered ISA feature");
    using architecture=A;
    static constexpr feature_set minimum=Minimum;
  };
  template<class... A> struct isa_list {};
  /// Concatenate ordered target lists without sorting or changing type identity.
  template<class... A, class... B>
  constexpr isa_list<A...,B...> operator+(isa_list<A...>,isa_list<B...>) noexcept { return {}; }
  namespace detail {
    template<class A> struct entry_traits { using architecture=A; static constexpr feature_set minimum=0; };
    template<architecture A, feature_set M> struct entry_traits<target_entry<A,M>> {
      using architecture=A; static constexpr feature_set minimum=M;
    };
  }

  /// Ordered first-match admission. Returns false without invoking the callback
  /// when no entry is admitted. This does not compile or target-attribute code.
  /// The callback receives only the selected type tag; capture results explicitly.
  template<class... Entries, class C, class F>
  constexpr bool with_isa(isa_list<Entries...>, C const & cpu, F && callback, feature_set minimum=0) {
    bool selected=false;
    auto try_entry=[&]<class E>() {
      using traits=detail::entry_traits<E>;
      using A=typename traits::architecture;
      static_assert(architecture<A>);
      if(!selected && classify_isa(cpu,A{},minimum|traits::minimum).admitted()) {
        selected=true;
        std::forward<F>(callback)(A{});
      }
    };
    (try_entry.template operator()<Entries>(),...);
    return selected;
  }
}
