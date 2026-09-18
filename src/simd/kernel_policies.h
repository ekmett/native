// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "simd/abi_refinement.h"

// Literal scopes shared by definition emitters and checked against the
// computed common refinement below.
#define SIMD_KERNEL_TARGET_0 ""
#define SIMD_KERNEL_TARGET_1 "avx2,fma,bmi2"
#define SIMD_KERNEL_TARGET_2 "avx2,fma,bmi2,avx512f,avx512dq"
#define SIMD_KERNEL_TARGET_3 "avx2,fma,bmi2,avx512f,avx512dq,avx512bw"
#define SIMD_KERNEL_TARGET_4 "avx2,fma,bmi2,avx512f,avx512dq,avx512vl"
#define SIMD_KERNEL_TARGET_5 "avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512vl"
#define SIMD_KERNEL_TARGET_7 "avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512bf16"
#define SIMD_KERNEL_TARGET_9 "avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512vl,avx512bf16"
#define SIMD_KERNEL_TARGET_11 "avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512fp16"
#define SIMD_KERNEL_TARGET_13 "avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512vl,avx512fp16"
#define SIMD_KERNEL_TARGET_15 "avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512bf16,avx512fp16"
#define SIMD_KERNEL_TARGET_17 "avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512vl,avx512bf16,avx512fp16"
#define SIMD_KERNEL_TARGET_20 "neon"
#define SIMD_KERNEL_TARGET_21 "neon,bf16"
#define SIMD_KERNEL_TARGET_22 "neon,fullfp16"
#define SIMD_KERNEL_TARGET_23 "neon,fullfp16,bf16"

// These are declaration-scope obligations, including the requirements of
// force-inline callees. A feature that does not change an instruction can still
// be required by its callee's target attribute. Keep those requirements until
// that callee is split into narrower scopes.
namespace simd::detail {
  // Type identity records available features; this optional member records the
  // requirements of the selected value implementation. Unknown custom domains
  // retain their architecture's conservative requirements.
  template<class T,class A,class=void> struct required_value_architecture {
    using type=A;
  };
  template<class T,class A> requires architecture<typename T::required_architecture>
  struct required_value_architecture<T,A,std::void_t<typename T::required_architecture>> {
    using type=typename T::required_architecture;
  };
  template<class T,class=void> struct value_architecture { using type=void; };
  template<class T> struct value_architecture<T,std::void_t<typename T::architecture>>
    : required_value_architecture<T,typename T::architecture> {};
  template<class T> using value_architecture_t=typename value_architecture<T>::type;
  using kernel_base=isa<avx2::features|feature::avx512f|feature::avx512dq>;
  using kernel_bw=isa<kernel_base::features|feature::avx512bw>;
  using kernel_vl=isa<kernel_base::features|feature::avx512vl>;
  using kernel_bw_bf16=isa<kernel_bw::features|feature::avx512_bf16>;
  using kernel_bw_fp16=isa<kernel_bw::features|feature::avx512_fp16>;
  using kernel_bw_half=isa<kernel_bw_bf16::features|feature::avx512_fp16>;
  using kernel_full_half=isa<avx512_bf16::features|feature::avx512_fp16>;
  using kernel_neon_half=isa<neon_bf16::features|feature::neon_fp16>;

  using x86_kernel_policies=isa_list<avx512,kernel_bw,kernel_vl,kernel_base,avx2>;
  using raw_kernel_policies=decltype(x86_kernel_policies{}+isa_list<neon,scalar>{});
  using x86_storage_policies=isa_list<kernel_full_half,kernel_bw_half,
    avx512_bf16,kernel_bw_bf16,avx512_fp16,kernel_bw_fp16,
    avx512,kernel_bw,kernel_vl,kernel_base,avx2>;
  using arm_storage_policies=isa_list<kernel_neon_half,neon_bf16,neon_fp16,neon>;
  using wide_storage_policies=decltype(x86_storage_policies{}+arm_storage_policies{}+isa_list<scalar>{});
  // Raw memory supports native-half storage at the full AVX-512 boundary.
  // No-VL float/integer paths retain their ordinary BW/base declarations.
  using memory_kernel_policies=decltype(isa_list<kernel_full_half,avx512_bf16,
    avx512_fp16,avx512,kernel_bw,kernel_vl,kernel_base,avx2>{}+
    arm_storage_policies{}+isa_list<scalar>{});

  // Transitive summaries for the existing raw kernels. The current source
  // driver puts these callees in identical scopes; compose that partition once.
  // Shape constraints still determine whether an operation is available. For
  // classification and extensible math these describe adapter scopes, not the
  // requirements of arbitrary user ADL functions or a promise of a raw overload.
  using arithmetic_policies=raw_kernel_policies;
  using compare_select_policies=raw_kernel_policies;
  using classification_policies=raw_kernel_policies;
  using rounding_policies=raw_kernel_policies;
  using scaling_policies=raw_kernel_policies;
  using integer_policies=raw_kernel_policies;
  using packing_policies=raw_kernel_policies;
  using compaction_policies=raw_kernel_policies;
  using array_math_policies=raw_kernel_policies;
  static_assert(std::same_as<arithmetic_policies,compare_select_policies> &&
    std::same_as<arithmetic_policies,classification_policies> &&
    std::same_as<arithmetic_policies,rounding_policies> &&
    std::same_as<arithmetic_policies,scaling_policies> &&
    std::same_as<arithmetic_policies,integer_policies> &&
    std::same_as<arithmetic_policies,packing_policies> &&
    std::same_as<arithmetic_policies,compaction_policies> &&
    std::same_as<arithmetic_policies,array_math_policies>);
  using wide_kernel_refinement=abi_refinement<raw_kernel_policies,wide_storage_policies>;
  using wide_kernel_policies=wide_kernel_refinement::policies;
  static_assert(wide_kernel_refinement::cells.size==16);
  // Generic memory helpers have a different partition, but every wide cell
  // already refines it. Check that fact without multiplying the body matrix.
  using wide_memory_refinement=abi_refinement<wide_kernel_policies,memory_kernel_policies>;
  static_assert(std::same_as<wide_kernel_policies,wide_memory_refinement::policies>);

  // Scope identities used by the repeated wide definitions. Ordinals belong
  // to the computed policy, never to a feature-bit count or a preset rank.
  inline constexpr std::array<unsigned,16> wide_kernel_scopes{
    17,9,13,5,15,7,11,3,4,2,1,23,21,22,20,0};
  template<unsigned Scope> consteval std::size_t wide_scope_index() {
    for(std::size_t i=0;i<wide_kernel_scopes.size();++i)
      if(wide_kernel_scopes[i]==Scope) return i;
    return abi_npos;
  }

  static_assert(wide_kernel_scopes.size()==wide_kernel_refinement::cells.size);
  static_assert(target_features(SIMD_KERNEL_TARGET_0)==
    wide_kernel_refinement::cells.records[wide_scope_index<0>()].requirements);
  static_assert(target_features(SIMD_KERNEL_TARGET_1)==
    wide_kernel_refinement::cells.records[wide_scope_index<1>()].requirements);
  static_assert(target_features(SIMD_KERNEL_TARGET_2)==
    wide_kernel_refinement::cells.records[wide_scope_index<2>()].requirements);
  static_assert(target_features(SIMD_KERNEL_TARGET_3)==
    wide_kernel_refinement::cells.records[wide_scope_index<3>()].requirements);
  static_assert(target_features(SIMD_KERNEL_TARGET_4)==
    wide_kernel_refinement::cells.records[wide_scope_index<4>()].requirements);
  static_assert(target_features(SIMD_KERNEL_TARGET_5)==
    wide_kernel_refinement::cells.records[wide_scope_index<5>()].requirements);
  static_assert(target_features(SIMD_KERNEL_TARGET_7)==
    wide_kernel_refinement::cells.records[wide_scope_index<7>()].requirements);
  static_assert(target_features(SIMD_KERNEL_TARGET_9)==
    wide_kernel_refinement::cells.records[wide_scope_index<9>()].requirements);
  static_assert(target_features(SIMD_KERNEL_TARGET_11)==
    wide_kernel_refinement::cells.records[wide_scope_index<11>()].requirements);
  static_assert(target_features(SIMD_KERNEL_TARGET_13)==
    wide_kernel_refinement::cells.records[wide_scope_index<13>()].requirements);
  static_assert(target_features(SIMD_KERNEL_TARGET_15)==
    wide_kernel_refinement::cells.records[wide_scope_index<15>()].requirements);
  static_assert(target_features(SIMD_KERNEL_TARGET_17)==
    wide_kernel_refinement::cells.records[wide_scope_index<17>()].requirements);
  static_assert(target_features(SIMD_KERNEL_TARGET_20)==
    wide_kernel_refinement::cells.records[wide_scope_index<20>()].requirements);
  static_assert(target_features(SIMD_KERNEL_TARGET_21)==
    wide_kernel_refinement::cells.records[wide_scope_index<21>()].requirements);
  static_assert(target_features(SIMD_KERNEL_TARGET_22)==
    wide_kernel_refinement::cells.records[wide_scope_index<22>()].requirements);
  static_assert(target_features(SIMD_KERNEL_TARGET_23)==
    wide_kernel_refinement::cells.records[wide_scope_index<23>()].requirements);

  template<class A> concept kernel_scalar_architecture=
    requires_abi<A,raw_kernel_policies,6> && A::features==0;
  template<class A> concept kernel_avx2_architecture=requires_abi<A,raw_kernel_policies,4>;
  template<class A> concept kernel_base_architecture=requires_abi<A,raw_kernel_policies,3>;
  template<class A> concept kernel_bw_architecture=requires_abi<A,raw_kernel_policies,1>;
  template<class A> concept kernel_vl_architecture=requires_abi<A,raw_kernel_policies,2>;
  template<class A> concept kernel_avx512_architecture=requires_abi<A,raw_kernel_policies,0>;
  template<class A> concept kernel_neon_architecture=requires_abi<A,raw_kernel_policies,5>;
}
