// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
namespace abi_lookup_test {
  using namespace native;
  constexpr auto base=feature_closure(avx2&x86_feature::avx512f&x86_feature::avx512dq);
  constexpr auto bw=base&x86_feature::avx512bw;
  constexpr auto vl=base&x86_feature::avx512vl;
  constexpr auto both=bw&x86_feature::avx512vl;
  constexpr auto extra=both&x86_feature::aes;
  using policies=isa_list<bw,vl,base,avx2>;
  static_assert(abi_lookup<bw,policies>::index==0);
  static_assert(abi_lookup<vl,policies>::index==1);
  static_assert(abi_lookup<base,policies>::index==2);
  static_assert(abi_lookup<avx2,policies>::index==3);
  static_assert(target<bw,bw,vl,base,avx2> == 0);
  static_assert(target<vl,bw,vl,base,avx2> == 1);
  static_assert(target<base,bw,vl,base,avx2> == 2);
  static_assert(target<avx2,bw,vl,base,avx2> == 3);
  static_assert(std::same_as<decltype(target<avx2,bw,vl,base,avx2>),int const>);
  // BW and VL are incomparable: list order decides their intersection.
  static_assert(abi_lookup<both,policies>::index==0);
  static_assert(abi_lookup<both,policies>::architecture==bw);
  static_assert(abi_lookup<both,isa_list<vl,bw>>::architecture==vl);
  static_assert(abi_lookup<both,isa_list<both,bw,vl>>::index==0);
  static_assert(abi_lookup<extra,policies>::index==0);
  static_assert(both==avx512);
  static_assert((x86_feature::avx512vl&bw)==both);
  static_assert(abi_lookup<extra,policies>::architecture==bw);
  static_assert(abi_lookup<extra,policies>::required_features==bw);

  // Metadata keeps the requested ISA and closes the compiler requirements.
  constexpr auto inherited=target_entry{avx2,x86_feature::avx512vl};
  using inherited_policies=isa_list<inherited,avx2>;
  static_assert(!abi_lookup<avx2,isa_list<inherited>>::matched);
  static_assert(abi_lookup<avx2,inherited_policies>::index==1);
  constexpr auto enough=feature_closure(avx2&x86_feature::avx512vl);
  using picked=abi_lookup<enough,inherited_policies>;
  static_assert(picked::matched && picked::index==0);
  static_assert(picked::architecture==avx2);
  static_assert(picked::minimum==isa(x86_feature::avx512vl));
  static_assert(picked::required_features==enough);
  constexpr auto wrong_host=target_entry{avx2,arm_feature::neon};
  static_assert(!abi_lookup<avx512,isa_list<wrong_host>>::matched);
  static_assert(!abi_lookup<neon,isa_list<wrong_host>>::matched);

  template<auto A,auto... P> concept has_target=requires { target<A,P...>; };
  static_assert(arch<isa> && arch<x86_feature> && arch<arm_feature> && !arch<int>);
  static_assert(!has_target<42,avx2> && !has_target<avx2,42>);
  static_assert(target<scalar,bw,vl,base,avx2> == -1);
  static_assert(target<avx512> == -1);
  static_assert(target<avx512,neon> == -1);
  static_assert(target<neon,avx2,scalar> == 1);
  static_assert(target<both,vl,bw> == 0);
  static_assert(target<extra,bw,vl,base,avx2> == 0);
  static_assert(target<x86_feature::avx2,x86_feature::avx2> == 0);
  using missing=abi_lookup<scalar,policies>;
  static_assert(!missing::matched && missing::index==-1);
  static_assert(!abi_lookup<avx512,isa_list<>>::matched);
  static_assert(abi_lookup<avx512,isa_list<>>::index==-1);

  // Scalar is an explicit empty fallback. Internal metadata lookup can retain
  // source order; the public target template rejects shadowed choices.
  using fallback=isa_list<avx2,scalar>;
  static_assert(abi_lookup<scalar,fallback>::index==1);
  static_assert(abi_lookup<neon,fallback>::index==1);
  static_assert(abi_lookup<avx512,fallback>::index==0);
  static_assert(abi_lookup<avx512,isa_list<scalar,avx512>>::index==0);
  static_assert(!abi_lookup<neon,policies>::matched);
  static_assert(!abi_lookup<avx512,isa_list<neon>>::matched);
  constexpr auto arm_both=neon_bf16&arm_feature::neon_fp16;
  static_assert(abi_lookup<arm_both,isa_list<neon_fp16,neon_bf16>>::architecture==neon_fp16);

  template<isa A> constexpr int exp_target=target<A,bw,vl,base,avx2>;
  template<isa A> requires(exp_target<A> == 0)
  constexpr isa keep() { return A; }
  template<isa A> requires(exp_target<A> == 1)
  constexpr isa keep() { return A; }
  template<isa A> concept has_keep=requires { keep<A>(); };
  static_assert(keep<extra>()==extra && keep<vl>()==vl);
  static_assert(has_keep<both> && !has_keep<base> && !has_keep<scalar>);
  template<isa A> struct partial { static constexpr int index=-1; };
  template<isa A> requires(exp_target<A> == 0)
  struct partial<A> { static constexpr int index=0; };
  static_assert(partial<extra>::index==0 && partial<avx2>::index==-1);

  // Independent extensions append after the existing public x86 ordinals.
  static_assert(static_cast<unsigned>(x86_feature::waitpkg)==29);
  static_assert(static_cast<unsigned>(x86_feature::crc32)==30);
  static_assert(static_cast<unsigned>(x86_feature::gfni)==31);
  static_assert(static_cast<unsigned>(x86_feature::avx512vpopcntdq)==32);
  static_assert(x86_feature_count==33);
  static_assert([] {
    isa a=arm_feature::neon;
    a.crc32=true;
    a.gfni=true;
    a.avx512vpopcntdq=true;
    if(a!=(arm_feature::neon&x86_feature::crc32&x86_feature::gfni&x86_feature::avx512vpopcntdq))
      return false;
    a.crc32=false;
    a.gfni=false;
    a.avx512vpopcntdq=false;
    return a==isa(arm_feature::neon);
  }());

  // Architecture-local ordinals must never alias in shared ISA storage.
  static_assert(isa(x86_feature::aes)!=isa(arm_feature::aes));
  static_assert(!(x86_feature::aes<=arm_feature::aes));
  static_assert(!(arm_feature::aes>=x86_feature::aes));
  static_assert(!(arm_feature::neon<arm_feature::aes));
  static_assert(!(arm_feature::aes<arm_feature::neon));
  static_assert(arm_feature::aes<=arm_feature::aes);
  static_assert(arm_feature::aes>=arm_feature::aes);
  constexpr auto both_aes=x86_feature::aes&arm_feature::aes;
  static_assert(both_aes.has(x86_feature::aes) && both_aes.has(arm_feature::aes));
  static_assert([] {
    for(std::size_t i=0;i<x86_feature_count;++i) {
      auto f=static_cast<x86_feature>(i);
      if(!isa(f).has(f)) return false;
      for(std::size_t j=0;j<arm_feature_count;++j)
        if(isa(f).has(static_cast<arm_feature>(j))) return false;
    }
    for(std::size_t i=0;i<arm_feature_count;++i) {
      auto f=static_cast<arm_feature>(i);
      if(!isa(f).has(f)) return false;
    }
    return true;
  }());
  constexpr bool invalid_enum_bounds() {
    auto check=[](auto invalid) {
      isa a=x86_feature::aes&arm_feature::aes;
      if(a.get(invalid)) return false;
      a.set(invalid,false);
      if(a!=both_aes) return false;
      a.set(invalid,true);
      return !a.get(invalid) && a.has(both_aes) &&
        !(a<=detail::known_features) &&
        !(feature_closure(isa(invalid))<=detail::known_features);
    };
    return check(static_cast<x86_feature>(x86_feature_count)) &&
      check(static_cast<arm_feature>(arm_feature_count)) &&
      check(static_cast<x86_feature>(-1)) && check(static_cast<arm_feature>(-1)) &&
      check(static_cast<x86_feature>(std::uint64_t{1}<<63)) &&
      check(static_cast<arm_feature>(std::uint64_t{1}<<63));
  }
  static_assert(invalid_enum_bounds());
  template<isa A> struct enum_requirement {};
  static_assert(!std::same_as<enum_requirement<x86_feature::aes>,enum_requirement<arm_feature::aes>>);

  template<class S,class F> concept accepts_feature=requires(S s,F f) {
    S(f); s.get(f); s.has(f); s.set(f,true);
  };
  static_assert(accepts_feature<feature_set<x86_feature>,x86_feature>);
  static_assert(accepts_feature<feature_set<arm_feature>,arm_feature>);
  static_assert(!accepts_feature<feature_set<x86_feature>,arm_feature>);
  static_assert(!accepts_feature<feature_set<arm_feature>,x86_feature>);
  static_assert(!std::constructible_from<feature_set<x86_feature>,feature_set<arm_feature>>);
  template<feature_set<x86_feature> A> struct x86_set_requirement {};
  template<feature_set<arm_feature> A> struct arm_set_requirement {};
  static_assert(!std::same_as<x86_set_requirement<x86_feature::aes>,x86_set_requirement<x86_feature::avx>>);
  static_assert(!std::same_as<arm_set_requirement<arm_feature::aes>,arm_set_requirement<arm_feature::neon>>);
  static_assert(!std::same_as<enum_requirement<feature_set<x86_feature>{x86_feature::aes}>,
    enum_requirement<feature_set<arm_feature>{arm_feature::aes}>>);
  static_assert(arch<feature_set<x86_feature>> && arch<feature_set<arm_feature>>);
  constexpr feature_set<x86_feature> typed_avx2=x86_feature::avx2;
  constexpr feature_set<arm_feature> typed_neon=arm_feature::neon;
  static_assert(target<typed_avx2,x86_feature::avx2,scalar> == 0);
  static_assert(target<typed_neon,arm_feature::neon,scalar> == 0);
  static_assert(typed_avx2<=avx2 && !(typed_avx2<=typed_neon));
  static_assert((typed_avx2&x86_feature::fma)==(x86_feature::avx2&x86_feature::fma));
  static_assert(isa(feature_set<x86_feature>{x86_feature::avx2})==isa(x86_feature::avx2));
  static_assert(!isa(feature_set<x86_feature>{x86_feature::avx2}).has(x86_feature::avx));
  template<class E> constexpr bool typed_set_bounds(std::size_t count) {
    feature_set<E> empty;
    if(!empty.valid() || isa(empty)!=scalar) return false;
    for(std::size_t i=0;i<count;++i) {
      auto f=static_cast<E>(i);
      feature_set<E> s=f;
      if(!s.valid() || !s.has(f) || !s.has(empty) || isa(s)!=isa(f)) return false;
      s.set(f,false);
      if(s!=empty) return false;
    }
    for(auto invalid:{std::uint64_t(count),~std::uint64_t{},std::uint64_t{1}<<63}) {
      auto f=static_cast<E>(invalid);
      feature_set<E> s;
      if(s.has(f)) return false;
      s.set(f,false);
      if(s!=empty) return false;
      s.set(f,true);
      if(s.valid() || s.has(f) || isa(s)<=detail::known_features) return false;
      auto saved=s;s.set(f,false);
      if(s!=saved) return false;
    }
    feature_set<E> padding;
    padding.flags.back()=std::uint64_t{1}<<63;
    return !padding.valid() && !(isa(padding)<=detail::known_features);
  }
  static_assert(typed_set_bounds<x86_feature>(x86_feature_count));
  static_assert(typed_set_bounds<arm_feature>(arm_feature_count));

  struct normalized_x86 {
    feature_set<x86_feature> present{},observed{};
    std::uint64_t xcr0=0xe6;
    bool xcr0_observed=true;
  };
  struct normalized_arm {
    feature_set<arm_feature> present{},observed{};
  };
  template<class E,class C> constexpr bool normalized_missing(std::size_t count,isa profile) {
    C full;
    for(std::size_t i=0;i<count;++i) {
      auto f=static_cast<E>(i);
      full.present.set(f,true);full.observed.set(f,true);
    }
    if(!classify_isa(full,profile).admitted()) return false;
    for(std::size_t i=0;i<count;++i) {
      auto f=static_cast<E>(i);
      auto cpu=full;cpu.observed.set(f,false);
      auto result=classify_isa(cpu,profile);
      if(result.admitted()==profile.has(f)) return false;
      if(result.missing_features!=(profile.has(f)?isa(f):scalar)) return false;
      cpu=full;cpu.present.set(f,false);
      result=classify_isa(cpu,profile);
      if(result.admitted()==profile.has(f)) return false;
    }
    if(classify_isa(full,isa(static_cast<E>(-1))).admitted()) return false;
    auto invalid=full;invalid.present.set(static_cast<E>(-1),true);
    if(!classify_isa(invalid,profile).invalid_features) return false;
    invalid=full;invalid.observed.flags.back()|=std::uint64_t{1}<<63;
    return classify_isa(invalid,profile).invalid_features &&
      !classify_isa(invalid,scalar).admitted();
  }
  static_assert(normalized_missing<x86_feature,normalized_x86>(x86_feature_count,avx512_fp16));
  static_assert(normalized_missing<arm_feature,normalized_arm>(arm_feature_count,neon_fp16&neon_bf16));
  static_assert([] {
    normalized_x86 cpu;
    if(!classify_isa(cpu,scalar).admitted() || classify_isa(cpu,avx2).admitted()) return false;
    // Scalar BMI2 (for example PDEP) does not require vector register state.
    cpu.present=x86_feature::bmi2;cpu.observed=x86_feature::bmi2;
    cpu.xcr0=0;cpu.xcr0_observed=false;
    if(!classify_isa(cpu,x86_feature::bmi2).admitted()) return false;
    if(classify_isa(cpu,x86_feature::avx).admitted()) return false;
    for(std::size_t i=0;i<x86_feature_count;++i) {
      auto f=static_cast<x86_feature>(i);
      cpu.present.set(f,true);cpu.observed.set(f,true);
    }
    cpu.xcr0_observed=false;
    auto missing=classify_isa(cpu,avx512);
    if(missing.admitted() || !missing.missing_xcr0_observation || missing.missing_xcr0!=0xe6) return false;
    cpu.xcr0_observed=true;cpu.xcr0=0;
    missing=classify_isa(cpu,avx2);
    return !missing.admitted() && !missing.missing_xcr0_observation && missing.missing_xcr0==6;
  }());

  // Singleton construction is exact, unlike explicit compiler-feature closure.
  constexpr isa single=x86_feature::avx2;
  static_assert(single.has(x86_feature::avx2) && !single.has(x86_feature::avx));
  static_assert(single<avx2 && !(avx2<single));
  static_assert(isa{}.has(isa{}) && isa{}<single);
  static_assert(x86_feature::avx2<=x86_feature::avx2 && !(x86_feature::avx2<x86_feature::avx2));
  static_assert(!(x86_feature::avx2<x86_feature::fma) && !(x86_feature::fma<x86_feature::avx2));
  static_assert(!(x86_feature::avx2<=x86_feature::fma) && !(x86_feature::fma>=x86_feature::avx2));
  constexpr auto pair=x86_feature::avx2&x86_feature::fma;
  static_assert(pair==(single&x86_feature::fma));
  static_assert(pair==(x86_feature::fma&single) && pair==(single&isa(x86_feature::fma)));
  static_assert(pair>x86_feature::avx2 && x86_feature::fma<pair && pair>=single);
  static_assert(x86_feature::avx2<=single && single>=x86_feature::avx2);
  constexpr isa edited=[] {
    isa a=x86_feature::avx2;
    a.fma=true;
    a.avx2=false;
    return a;
  }();
  static_assert(edited==isa(x86_feature::fma) && single==isa(x86_feature::avx2));
  // Clang's Itanium mangler cannot encode properties directly in a function
  // constraint. Keep property evaluation here and exercise the portable form.
  template<isa A> concept has_fields=A.avx2 && A.fma;
  template<isa A> requires has_fields<A>
  constexpr bool fields() { return A.has(x86_feature::avx2&x86_feature::fma); }
  static_assert(fields<pair>());
  static_assert(!has_fields<single> && !has_fields<isa{}>);
  template<isa A> requires(A.has(x86_feature::avx2&x86_feature::fma))
  constexpr bool flags();
  template<isa A> requires(A.has(x86_feature::avx2&x86_feature::fma))
  constexpr bool flags() { return A.avx2 && A.fma; }
  static_assert(flags<pair>());
  template<class T> concept alternatives=requires(T a) { a|a; };
  static_assert(!alternatives<isa> && !alternatives<x86_feature> && !alternatives<arm_feature>);
}
