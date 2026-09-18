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

  using missing=abi_lookup<scalar,policies>;
  static_assert(!missing::matched && missing::index==abi_npos);
  static_assert(std::same_as<missing::type,void>);
  static_assert(std::same_as<missing::architecture,void>);
  static_assert(!requires_abi<scalar,policies,0>);
  static_assert(!requires_abi<scalar,policies,abi_npos>);
  static_assert(!abi_lookup<avx512,isa_list<>>::matched);
  static_assert(abi_lookup<avx512,isa_list<>>::index==abi_npos);
  static_assert(!requires_abi<avx512,isa_list<>,abi_npos>);
  static_assert(!requires_abi<avx512,policies,99>);
  static_assert(!requires_abi<int,policies,0>);
  static_assert(!requires_abi<avx512,int,0>);

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
  template<architecture A> requires requires_abi<A,policies,0>
  constexpr A keep_tag(A tag) { return tag; }
  template<architecture A> requires requires_abi<A,policies,1>
  constexpr A keep_tag(A tag) { return tag; }
  template<class A> concept has_keep_tag=architecture<A> && requires(A tag) { keep_tag(tag); };
  static_assert(std::same_as<decltype(keep_tag(extra{})),extra>);
  static_assert(std::same_as<decltype(keep_tag(vl{})),vl>);
  static_assert(has_keep_tag<both> && !has_keep_tag<base> && !has_keep_tag<scalar>);
  static_assert(requires_abi<both,policies,0> && !requires_abi<both,policies,1>);
}
