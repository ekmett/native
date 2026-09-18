// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
namespace abi_lookup_test {
  using namespace simd;
  using base=isa<avx2::features|feature::avx512f|feature::avx512dq>;
  using bw=isa<base::features|feature::avx512bw>;
  using vl=isa<base::features|feature::avx512vl>;
  using both=isa<bw::features|feature::avx512vl>;
  using extra=isa<both::features|feature::aes>;
  using policies=isa_list<bw,vl,base,avx2>;
  static_assert(abi_lookup<bw,policies>::index==0);
  static_assert(abi_lookup<vl,policies>::index==1);
  static_assert(abi_lookup<base,policies>::index==2);
  static_assert(abi_lookup<avx2,policies>::index==3);
  static_assert(target<bw,bw,vl,base,avx2> == 0);
  static_assert(target<vl,bw,vl,base,avx2> == 1);
  static_assert(target<base,bw,vl,base,avx2> == 2);
  static_assert(target<avx2,bw,vl,base,avx2> == 3);
  static_assert(target<avx2,policies> == target<avx2,bw,vl,base,avx2>);
  // BW and VL are incomparable: list order decides their intersection.
  static_assert(abi_lookup<both,policies>::index==0);
  static_assert(std::same_as<abi_lookup<both,policies>::type,bw>);
  static_assert(std::same_as<abi_lookup<both,isa_list<vl,bw>>::type,vl>);
  static_assert(abi_lookup<both,isa_list<both,bw,vl>>::index==0);
  static_assert(abi_lookup<extra,policies>::index==0);
  static_assert(std::same_as<both,avx512>);
  static_assert(std::same_as<isa<feature::avx512vl|bw::features>,both>);
  static_assert(abi_lookup<both,isa_list<avx512,both>>::index==0);
  static_assert(std::same_as<abi_lookup<extra,policies>::architecture,bw>);
  static_assert(abi_lookup<extra,policies>::required_features==bw::features);

  // Matching must include closure of the inherited compiler minimum, without
  // replacing the requested tag or losing the original target_entry metadata.
  using inherited=target_entry<avx2,feature_set(feature::avx512vl)>;
  using inherited_policies=isa_list<inherited,avx2>;
  static_assert(!abi_lookup<avx2,isa_list<inherited>>::matched);
  static_assert(abi_lookup<avx2,inherited_policies>::index==1);
  using enough=isa<avx2::features|feature::avx512vl>;
  using picked=abi_lookup<enough,inherited_policies>;
  static_assert(picked::matched && picked::index==0);
  static_assert(std::same_as<picked::type,inherited>);
  static_assert(std::same_as<picked::architecture,avx2>);
  static_assert(picked::minimum==feature_set(feature::avx512vl));
  static_assert(picked::required_features==enough::features);
  using wrong_host=target_entry<avx2,feature_set(feature::neon)>;
  static_assert(!abi_lookup<avx512,isa_list<wrong_host>>::matched);
  static_assert(!abi_lookup<neon,isa_list<wrong_host>>::matched);

  template<class A,class... P> concept has_abi=requires { target<A,P...>; };
  template<class A,class P> concept has_lookup=requires { typename abi_lookup<A,P>::type; };
  static_assert(!has_abi<int,avx2> && !has_abi<avx2,int>);
  static_assert(!has_abi<avx2,isa_list<int>>);
  static_assert(!has_abi<avx2,isa_list<isa_list<avx2>>>);
  static_assert(!has_abi<avx2,isa_list<avx2>,avx512>);
  static_assert(!has_abi<avx2 const,avx2> && !has_abi<avx2,avx2 const>);
  static_assert(!has_lookup<int,policies> && !has_lookup<avx2,isa_list<int>>);
  static_assert(target<scalar,policies> == target_npos);
  static_assert(target<avx512> == target_npos && target<avx512,isa_list<>> == target_npos);
  static_assert(target<avx512,neon> == target_npos);
  static_assert(target<neon,avx2,scalar> == 1);
  static_assert(target<both,vl,bw> == 0);
  static_assert(target<extra,policies> == 0);
  static_assert(target<avx512,avx2,avx512> == 0);
  static_assert(target<avx2,inherited,avx2> == 1);
  static_assert(target<enough,inherited,avx2> == 0);
  static_assert(target<avx512,wrong_host> == target_npos);
  using missing=abi_lookup<scalar,policies>;
  static_assert(!missing::matched && missing::index==target_npos);
  static_assert(std::same_as<missing::type,void>);
  static_assert(std::same_as<missing::architecture,void>);
  static_assert(!requires_target<scalar,0,policies>);
  static_assert(!requires_target<scalar,target_npos,policies>);
  static_assert(!abi_lookup<avx512,isa_list<>>::matched);
  static_assert(abi_lookup<avx512,isa_list<>>::index==target_npos);
  static_assert(!requires_target<avx512,target_npos,isa_list<>>);
  static_assert(!requires_target<avx512,99,policies>);
  static_assert(!requires_target<int,0,policies>);
  static_assert(!requires_target<avx512,0,int>);

  // Scalar is an explicit zero-requirement fallback; it also matches stronger
  // tags, and placing it first intentionally shadows every later entry.
  using fallback=isa_list<avx2,scalar>;
  static_assert(abi_lookup<scalar,fallback>::index==1);
  static_assert(abi_lookup<neon,fallback>::index==1);
  static_assert(abi_lookup<avx512,fallback>::index==0);
  static_assert(abi_lookup<avx512,isa_list<scalar,avx512>>::index==0);
  static_assert(!abi_lookup<neon,policies>::matched);
  static_assert(!abi_lookup<avx512,isa_list<neon>>::matched);
  using arm_both=isa<neon_bf16::features|feature::neon_fp16>;
  static_assert(std::same_as<abi_lookup<arm_both,isa_list<neon_fp16,neon_bf16>>::type,neon_fp16>);

  // The selected ordinal constrains overloads without changing caller identity.
  template<architecture A> requires (target<A,policies> == 0)
  constexpr A keep_tag(A tag) { return tag; }
  template<architecture A> requires (target<A,policies> == 1)
  constexpr A keep_tag(A tag) { return tag; }
  template<class A> concept has_keep_tag=architecture<A> && requires(A tag) { keep_tag(tag); };
  static_assert(std::same_as<decltype(keep_tag(extra{})),extra>);
  static_assert(std::same_as<decltype(keep_tag(vl{})),vl>);
  static_assert(has_keep_tag<both> && !has_keep_tag<base> && !has_keep_tag<scalar>);
  // Keep the architecture atomic constraint so this exactly repeated pattern
  // is more constrained than its primary, as real predicate specializations are.
  template<architecture A> struct partial { static constexpr auto index=target_npos; };
  template<requires_target<0,policies> A> struct partial<A> { static constexpr auto index=0u; };
  static_assert(partial<extra>::index==0 && partial<avx2>::index==target_npos);
  template<requires_target<3,bw,vl,base,avx2> A>
  constexpr A keep_fourth(A tag) { return tag; }
  template<requires_target<0,policies> A>
  constexpr A keep_first(A tag) { return tag; }
  static_assert(std::same_as<decltype(keep_fourth(avx2{})),avx2>);
  static_assert(std::same_as<decltype(keep_first(extra{})),extra>);
  static_assert(!requires_target<int,0,avx2> && !requires_target<avx2,0,int>);
  static_assert(!requires_target<scalar,target_npos,avx2>);
  static_assert(requires_target<both,0,policies> && !requires_target<both,1,policies>);
}
