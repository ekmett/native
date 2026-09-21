// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
namespace abi_lookup_test {
  using namespace native;
  constexpr isa<x86> x86_scalar{};
  constexpr isa<arm> arm_scalar{};
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
  template<class A,class M> concept accepts_minimum=requires(A a,M m) { target_entry{a,m}; };
  static_assert(accepts_minimum<isa<x86>,isa<x86>> && accepts_minimum<isa<arm>,isa<arm>>);
  static_assert(!accepts_minimum<isa<x86>,isa<arm>> && !accepts_minimum<isa<arm>,isa<x86>>);

  template<auto A,auto... P> concept has_target=requires { target<A,P...>; };
  static_assert(arch<isa<>> && arch<x86_feature> && arch<arm_feature> && !arch<int>);
  static_assert(!has_target<42,avx2> && !has_target<avx2,42>);
  static_assert(target<x86_scalar,bw,vl,base,avx2> == -1);
  static_assert(target<avx512> == -1);
  static_assert(!has_target<avx512,neon>);
  static_assert(!has_target<neon,avx2,arm_scalar>);
  static_assert(target<neon,arm_scalar> == 0);
  static_assert(target<both,vl,bw> == 0);
  static_assert(target<extra,bw,vl,base,avx2> == 0);
  static_assert(target<x86_feature::avx2,x86_feature::avx2> == 0);
  using missing=abi_lookup<x86_scalar,policies>;
  static_assert(!missing::matched && missing::index==-1);
  static_assert(!abi_lookup<avx512,isa_list<>>::matched);
  static_assert(abi_lookup<avx512,isa_list<>>::index==-1);

  // Scalar is an explicit empty fallback. Internal metadata lookup can retain
  // source order; the public target template rejects shadowed choices.
  using fallback=isa_list<avx2,x86_scalar>;
  static_assert(abi_lookup<x86_scalar,fallback>::index==1);
  static_assert(abi_lookup<neon,isa_list<arm_scalar>>::index==0);
  static_assert(abi_lookup<avx512,fallback>::index==0);
  static_assert(abi_lookup<avx512,isa_list<x86_scalar,avx512>>::index==0);
  constexpr auto arm_both=neon_bf16&arm_feature::neon_fp16;
  static_assert(abi_lookup<arm_both,isa_list<neon_fp16,neon_bf16>>::architecture==neon_fp16);

  template<isa<x86> A> constexpr int exp_target=target<A,bw,vl,base,avx2>;
  template<isa<x86> A> requires(exp_target<A> == 0)
  constexpr isa<x86> keep() { return A; }
  template<isa<x86> A> requires(exp_target<A> == 1)
  constexpr isa<x86> keep() { return A; }
  template<isa<x86> A> concept has_keep=requires { keep<A>(); };
  static_assert(keep<extra>()==extra && keep<vl>()==vl);
  static_assert(has_keep<both> && !has_keep<base> && !has_keep<x86_scalar>);
  template<isa<x86> A> struct partial { static constexpr int index=-1; };
  template<isa<x86> A> requires(exp_target<A> == 0)
  struct partial<A> { static constexpr int index=0; };
  static_assert(partial<extra>::index==0 && partial<avx2>::index==-1);

  // Independent extensions append after the existing public x86 ordinals.
  static_assert(static_cast<unsigned>(x86_feature::waitpkg)==29);
  static_assert(static_cast<unsigned>(x86_feature::crc32)==30);
  static_assert(static_cast<unsigned>(x86_feature::gfni)==31);
  static_assert(static_cast<unsigned>(x86_feature::avx512vpopcntdq)==32);
  static_assert(static_cast<unsigned>(x86_feature::vpclmulqdq)==33);
  static_assert(static_cast<unsigned>(x86_feature::avxvnni)==34);
  static_assert(static_cast<unsigned>(x86_feature::avx512vnni)==35);
  static_assert(static_cast<unsigned>(x86_feature::avxvnniint8)==36);
  static_assert(static_cast<unsigned>(x86_feature::avxvnniint16)==37);
  static_assert(x86_feature_count==38);
  static_assert([] {
    isa<x86> a{};
    a.crc32=true;
    a.gfni=true;
    a.avx512vpopcntdq=true;
    if(a!=(x86_feature::crc32&x86_feature::gfni&x86_feature::avx512vpopcntdq))
      return false;
    a.crc32=false;
    a.gfni=false;
    a.avx512vpopcntdq=false;
    return a==x86_scalar;
  }());

  // Equal local indices cannot enable a foreign family's operation.
  template<class A,class B> concept comparable=requires(A a,B b) { a==b; a<=b; a>=b; };
  template<class A,class B> concept conjunction=requires(A a,B b) { a&b; };
  static_assert(!comparable<isa<x86>,isa<arm>> && !comparable<x86_feature,arm_feature>);
  static_assert(!conjunction<isa<x86>,isa<arm>> && !conjunction<x86_feature,arm_feature>);
  static_assert(!conjunction<isa<arm>,wasm_feature> && !conjunction<isa<wasm>,x86_feature>);
  static_assert(!(arm_feature::neon<arm_feature::aes));
  static_assert(!(arm_feature::aes<arm_feature::neon));
  static_assert(arm_feature::aes<=arm_feature::aes);
  static_assert(arm_feature::aes>=arm_feature::aes);
  static_assert([] {
    for(std::size_t i=0;i<x86_feature_count;++i) {
      auto f=static_cast<x86_feature>(i);
      if(!isa(f).has(f)) return false;
    }
    for(std::size_t i=0;i<arm_feature_count;++i) {
      auto f=static_cast<arm_feature>(i);
      if(!isa(f).has(f)) return false;
    }
    return true;
  }());
  constexpr bool invalid_enum_bounds() {
    auto check=[](auto invalid) {
      constexpr auto first=static_cast<decltype(invalid)>(0);
      isa a=first;
      if(a.get(invalid)) return false;
      a.set(invalid,false);
      if(a!=isa(first)) return false;
      a.set(invalid,true);
      return !a.get(invalid) && a.has(first) &&
        !(a<=detail::known_features<decltype(isa(invalid))::family>) &&
        !(feature_closure(isa(invalid))<=detail::known_features<decltype(isa(invalid))::family>);
    };
    return check(static_cast<x86_feature>(x86_feature_count)) &&
      check(static_cast<arm_feature>(arm_feature_count)) &&
      check(static_cast<x86_feature>(-1)) && check(static_cast<arm_feature>(-1)) &&
      check(static_cast<x86_feature>(std::uint64_t{1}<<63)) &&
      check(static_cast<arm_feature>(std::uint64_t{1}<<63));
  }
  static_assert(invalid_enum_bounds());
  template<auto A> struct enum_requirement {};
  static_assert(!std::same_as<enum_requirement<x86_feature::aes>,enum_requirement<arm_feature::aes>>);

  template<class S,class F> concept accepts_feature=requires(S s,F f) {
    S(f); s.get(f); s.has(f); s.set(f,true);
  };
  static_assert(accepts_feature<isa<x86>,x86_feature>);
  static_assert(accepts_feature<isa<arm>,arm_feature>);
  static_assert(!accepts_feature<isa<x86>,arm_feature>);
  static_assert(!accepts_feature<isa<arm>,x86_feature>);
  static_assert(!std::constructible_from<isa<x86>,isa<arm>>);
  template<isa<x86> A> struct x86_set_requirement {};
  template<isa<arm> A> struct arm_set_requirement {};
  static_assert(!std::same_as<x86_set_requirement<x86_feature::aes>,x86_set_requirement<x86_feature::avx>>);
  static_assert(!std::same_as<arm_set_requirement<arm_feature::aes>,arm_set_requirement<arm_feature::neon>>);
  static_assert(!std::same_as<enum_requirement<isa<x86>{x86_feature::aes}>,
    enum_requirement<isa<arm>{arm_feature::aes}>>);
  static_assert(arch<isa<x86>> && arch<isa<arm>> && arch<isa<wasm>>);
  template<class S> concept x86_properties=requires(S s) { s.avx2; s.fma; s.bmi2; };
  template<class S> concept arm_properties=requires(S s) { s.neon; s.i8mm; s.pmull; };
  template<class S> concept wasm_properties=requires(S s) { s.simd128; s.relaxed_simd; };
  static_assert(x86_properties<isa<x86>> && !x86_properties<isa<arm>> && !x86_properties<isa<wasm>>);
  static_assert(arm_properties<isa<arm>> && !arm_properties<isa<x86>> && !arm_properties<isa<wasm>>);
  static_assert(wasm_properties<isa<wasm>> && !wasm_properties<isa<arm>> && !wasm_properties<isa<x86>>);
  static_assert(!std::constructible_from<isa<arm>,isa<x86>>);
  static_assert(!std::constructible_from<isa<wasm>,isa<arm>> && !std::constructible_from<isa<arm>,isa<wasm>>);
  constexpr isa<x86> typed_avx2=x86_feature::avx2;
  constexpr isa<arm> typed_neon=arm_feature::neon;
  static_assert(target<typed_avx2,x86_feature::avx2,x86_scalar> == 0);
  static_assert(target<typed_neon,arm_feature::neon,arm_scalar> == 0);
  static_assert(typed_avx2<=avx2 && !comparable<decltype(typed_avx2),decltype(typed_neon)>);
  static_assert((typed_avx2&x86_feature::fma)==(x86_feature::avx2&x86_feature::fma));
  static_assert(isa(isa<x86>{x86_feature::avx2})==isa(x86_feature::avx2));
  static_assert(!isa(isa<x86>{x86_feature::avx2}).has(x86_feature::avx));
  template<class E> constexpr bool typed_set_bounds(std::size_t count) {
    decltype(isa(E{})) empty;
    if(!empty.valid() || isa(empty)!=decltype(isa(E{})){}) return false;
    for(std::size_t i=0;i<count;++i) {
      auto f=static_cast<E>(i);
      decltype(isa(E{})) s=f;
      if(!s.valid() || !s.has(f) || !s.has(empty) || isa(s)!=isa(f)) return false;
      s.set(f,false);
      if(s!=empty) return false;
    }
    for(auto invalid:{std::uint64_t(count),~std::uint64_t{},std::uint64_t{1}<<63}) {
      auto f=static_cast<E>(invalid);
      decltype(isa(E{})) s;
      if(s.has(f)) return false;
      s.set(f,false);
      if(s!=empty) return false;
      s.set(f,true);
      if(s.valid() || s.has(f) || isa(s)<=detail::known_features<decltype(isa(E{}))::family>) return false;
      auto saved=s;s.set(f,false);
      if(s!=saved) return false;
    }
    decltype(isa(E{})) padding;
    padding.flags.back()=std::uint64_t{1}<<63;
    return !padding.valid() && !(isa(padding)<=detail::known_features<decltype(isa(E{}))::family>);
  }
  static_assert(typed_set_bounds<x86_feature>(x86_feature_count));
  static_assert(typed_set_bounds<arm_feature>(arm_feature_count));

  struct normalized_x86 {
    isa<x86> present{},observed{};
    std::uint64_t xcr0=0xe6;
    bool xcr0_observed=true;
  };
  struct normalized_arm {
    isa<arm> present{},observed{};
  };
  template<class E,class C> constexpr bool normalized_missing(std::size_t count,decltype(isa(E{})) profile) {
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
      if(result.missing_features!=(profile.has(f)?isa(f):decltype(isa(E{})){})) return false;
      cpu=full;cpu.present.set(f,false);
      result=classify_isa(cpu,profile);
      if(result.admitted()==profile.has(f)) return false;
    }
    if(classify_isa(full,isa(static_cast<E>(-1))).admitted()) return false;
    auto invalid=full;invalid.present.set(static_cast<E>(-1),true);
    if(!classify_isa(invalid,profile).invalid_features) return false;
    invalid=full;invalid.observed.flags.back()|=std::uint64_t{1}<<63;
    return classify_isa(invalid,profile).invalid_features &&
      !classify_isa(invalid,decltype(isa(E{})){}).admitted();
  }
  static_assert(normalized_missing<x86_feature,normalized_x86>(x86_feature_count,avx512_fp16));
  static_assert(normalized_missing<arm_feature,normalized_arm>(arm_feature_count,neon_fp16&neon_bf16));
  static_assert(normalized_missing<arm_feature,normalized_arm>(arm_feature_count,neon&arm_feature::i8mm));
  static_assert([] {
    normalized_x86 cpu;
    if(!classify_isa(cpu,x86_scalar).admitted() || classify_isa(cpu,avx2).admitted()) return false;
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
  constexpr isa<x86> single=x86_feature::avx2;
  static_assert(single.has(x86_feature::avx2) && !single.has(x86_feature::avx));
  static_assert(single<avx2 && !(avx2<single));
  static_assert(isa<x86>{}.has(isa<x86>{}) && isa<x86>{}<single);
  static_assert(x86_feature::avx2<=x86_feature::avx2 && !(x86_feature::avx2<x86_feature::avx2));
  static_assert(!(x86_feature::avx2<x86_feature::fma) && !(x86_feature::fma<x86_feature::avx2));
  static_assert(!(x86_feature::avx2<=x86_feature::fma) && !(x86_feature::fma>=x86_feature::avx2));
  constexpr auto pair=x86_feature::avx2&x86_feature::fma;
  static_assert(pair==(single&x86_feature::fma));
  static_assert(pair==(x86_feature::fma&single) && pair==(single&isa(x86_feature::fma)));
  static_assert(pair>x86_feature::avx2 && x86_feature::fma<pair && pair>=single);
  static_assert(x86_feature::avx2<=single && single>=x86_feature::avx2);
  constexpr isa<x86> edited=[] {
    isa a=x86_feature::avx2;
    a.fma=true;
    a.avx2=false;
    return a;
  }();
  static_assert(edited==isa(x86_feature::fma) && single==isa(x86_feature::avx2));
  // Clang's Itanium mangler cannot encode properties directly in a function
  // constraint. Keep property evaluation here and exercise the portable form.
  template<isa<x86> A> concept has_fields=A.avx2 && A.fma;
  template<isa<x86> A> requires has_fields<A>
  constexpr bool fields() { return A.has(x86_feature::avx2&x86_feature::fma); }
  static_assert(fields<pair>());
  static_assert(!has_fields<single> && !has_fields<isa<x86>{}>);
  template<isa<x86> A> requires(A.has(x86_feature::avx2&x86_feature::fma))
  constexpr bool flags();
  template<isa<x86> A> requires(A.has(x86_feature::avx2&x86_feature::fma))
  constexpr bool flags() { return A.avx2 && A.fma; }
  static_assert(flags<pair>());
  template<class T> concept alternatives=requires(T a) { a|a; };
  static_assert(!alternatives<isa<>> && !alternatives<x86_feature> && !alternatives<arm_feature>);
}
