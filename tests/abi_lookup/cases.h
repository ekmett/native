// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
namespace abi_lookup_test {
  using namespace simd;
  constexpr auto base=feature_closure(avx2&feature::avx512f&feature::avx512dq);
  constexpr auto bw=base&feature::avx512bw;
  constexpr auto vl=base&feature::avx512vl;
  constexpr auto both=bw&feature::avx512vl;
  constexpr auto extra=both&feature::aes;
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
  static_assert((feature::avx512vl&bw)==both);
  static_assert(abi_lookup<extra,policies>::architecture==bw);
  static_assert(abi_lookup<extra,policies>::required_features==bw);

  // Metadata keeps the requested ISA and closes the compiler requirements.
  constexpr auto inherited=target_entry{avx2,feature::avx512vl};
  using inherited_policies=isa_list<inherited,avx2>;
  static_assert(!abi_lookup<avx2,isa_list<inherited>>::matched);
  static_assert(abi_lookup<avx2,inherited_policies>::index==1);
  constexpr auto enough=feature_closure(avx2&feature::avx512vl);
  using picked=abi_lookup<enough,inherited_policies>;
  static_assert(picked::matched && picked::index==0);
  static_assert(picked::architecture==avx2);
  static_assert(picked::minimum==isa(feature::avx512vl));
  static_assert(picked::required_features==enough);
  constexpr auto wrong_host=target_entry{avx2,feature::neon};
  static_assert(!abi_lookup<avx512,isa_list<wrong_host>>::matched);
  static_assert(!abi_lookup<neon,isa_list<wrong_host>>::matched);

  template<auto A,auto... P> concept has_target=requires { target<A,P...>; };
  static_assert(arch<isa> && arch<feature> && !arch<int>);
  static_assert(!has_target<42,avx2> && !has_target<avx2,42>);
  static_assert(target<scalar,bw,vl,base,avx2> == -1);
  static_assert(target<avx512> == -1);
  static_assert(target<avx512,neon> == -1);
  static_assert(target<neon,avx2,scalar> == 1);
  static_assert(target<both,vl,bw> == 0);
  static_assert(target<extra,bw,vl,base,avx2> == 0);
  static_assert(target<feature::avx2,feature::avx2> == 0);
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
  constexpr auto arm_both=neon_bf16&feature::neon_fp16;
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

  // Singleton construction is exact, unlike explicit compiler-feature closure.
  constexpr isa single=feature::avx2;
  static_assert(single.has(feature::avx2) && !single.has(feature::avx));
  static_assert(single<avx2 && !(avx2<single));
  static_assert(isa{}.has(isa{}) && isa{}<single);
  static_assert(feature::avx2<=feature::avx2 && !(feature::avx2<feature::avx2));
  static_assert(!(feature::avx2<feature::fma) && !(feature::fma<feature::avx2));
  static_assert(!(feature::avx2<=feature::fma) && !(feature::fma>=feature::avx2));
  constexpr auto pair=feature::avx2&feature::fma;
  static_assert(pair==(single&feature::fma));
  static_assert(pair==(feature::fma&single) && pair==(single&isa(feature::fma)));
  static_assert(pair>feature::avx2 && feature::fma<pair && pair>=single);
  static_assert(feature::avx2<=single && single>=feature::avx2);
  constexpr isa edited=[] {
    isa a=feature::avx2;
    a.fma=true;
    a.avx2=false;
    return a;
  }();
  static_assert(edited==isa(feature::fma) && single==isa(feature::avx2));
  template<isa A> requires(A.avx2 && A.fma)
  constexpr bool fields() { return A.has(feature::avx2&feature::fma); }
  static_assert(fields<pair>());
  template<class T> concept alternatives=requires(T a) { a|a; };
  static_assert(!alternatives<isa> && !alternatives<feature>);
}
