// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/abi_refinement.h"

// Literal scopes shared by definition emitters and checked against the
// computed common refinement below.
#define NATIVE_KERNEL_TARGET_0 ""
#define NATIVE_KERNEL_TARGET_1 "avx2,fma"
#define NATIVE_KERNEL_TARGET_2 "avx2,fma,avx512f,avx512dq"
#define NATIVE_KERNEL_TARGET_3 "avx2,fma,avx512f,avx512dq,avx512bw"
#define NATIVE_KERNEL_TARGET_4 "avx2,fma,avx512f,avx512dq,avx512vl"
#define NATIVE_KERNEL_TARGET_5 "avx2,fma,avx512f,avx512dq,avx512bw,avx512vl"
#define NATIVE_KERNEL_TARGET_7 "avx2,fma,avx512f,avx512dq,avx512bw,avx512bf16"
#define NATIVE_KERNEL_TARGET_9 "avx2,fma,avx512f,avx512dq,avx512bw,avx512vl,avx512bf16"
#define NATIVE_KERNEL_TARGET_11 "avx2,fma,avx512f,avx512dq,avx512bw,avx512fp16"
#define NATIVE_KERNEL_TARGET_13 "avx2,fma,avx512f,avx512dq,avx512bw,avx512vl,avx512fp16"
#define NATIVE_KERNEL_TARGET_15 "avx2,fma,avx512f,avx512dq,avx512bw,avx512bf16,avx512fp16"
#define NATIVE_KERNEL_TARGET_17 "avx2,fma,avx512f,avx512dq,avx512bw,avx512vl,avx512bf16,avx512fp16"
#define NATIVE_KERNEL_TARGET_20 "neon"
#define NATIVE_KERNEL_TARGET_21 "neon,bf16"
#define NATIVE_KERNEL_TARGET_22 "neon,fullfp16"
#define NATIVE_KERNEL_TARGET_23 "neon,fullfp16,bf16"

// These are declaration-scope obligations, including the requirements of
// force-inline callees. A feature that does not change an instruction can still
// be required by its callee's target attribute. Keep those requirements until
// that callee is split into narrower scopes.
namespace native::detail {
  inline constexpr isa<x86> kernel_base=feature_closure(avx2 & x86_feature::avx512f & x86_feature::avx512dq);
  inline constexpr isa<x86> kernel_bw=feature_closure(kernel_base & x86_feature::avx512bw);
  inline constexpr isa<x86> kernel_vl=feature_closure(kernel_base & x86_feature::avx512vl);
  inline constexpr isa<x86> kernel_bw_bf16=feature_closure(kernel_bw & x86_feature::avx512bf16);
  inline constexpr isa<x86> kernel_bw_fp16=feature_closure(kernel_bw & x86_feature::avx512fp16);
  inline constexpr isa<x86> kernel_bw_half=feature_closure(kernel_bw_bf16 & x86_feature::avx512fp16);
  inline constexpr isa<x86> kernel_full_half=feature_closure(avx512_bf16 & x86_feature::avx512fp16);
  inline constexpr isa<arm> kernel_neon_half=feature_closure(neon_bf16 & arm_feature::neon_fp16);

  template<architecture Family> struct kernel_profiles {
    using raw=isa_list<isa<Family>{}>;
    using storage=raw;
    using memory=raw;
    static constexpr std::array targets{NATIVE_KERNEL_TARGET_0};
    static constexpr std::array indices{15};
  };
  template<> struct kernel_profiles<x86> {
    using raw=isa_list<avx512,kernel_bw,kernel_vl,kernel_base,avx2,isa<x86>{}>;
    using storage=isa_list<kernel_full_half,kernel_bw_half,
      avx512_bf16,kernel_bw_bf16,avx512_fp16,kernel_bw_fp16,
      avx512,kernel_bw,kernel_vl,kernel_base,avx2,isa<x86>{}>;
    // Native-half memory operations carry the full AVX-512 declaration scope.
    using memory=isa_list<kernel_full_half,avx512_bf16,avx512_fp16,
      avx512,kernel_bw,kernel_vl,kernel_base,avx2,isa<x86>{}>;
    static constexpr std::array targets{
      NATIVE_KERNEL_TARGET_17,NATIVE_KERNEL_TARGET_9,NATIVE_KERNEL_TARGET_13,
      NATIVE_KERNEL_TARGET_5,NATIVE_KERNEL_TARGET_15,NATIVE_KERNEL_TARGET_7,
      NATIVE_KERNEL_TARGET_11,NATIVE_KERNEL_TARGET_3,NATIVE_KERNEL_TARGET_4,
      NATIVE_KERNEL_TARGET_2,NATIVE_KERNEL_TARGET_1,NATIVE_KERNEL_TARGET_0};
    static constexpr std::array indices{0,1,2,3,4,5,6,7,8,9,10,15};
  };
  template<> struct kernel_profiles<arm> {
    using raw=isa_list<neon,isa<arm>{}>;
    using storage=isa_list<kernel_neon_half,neon_bf16,neon_fp16,neon,isa<arm>{}>;
    using memory=storage;
    static constexpr std::array targets{NATIVE_KERNEL_TARGET_23,NATIVE_KERNEL_TARGET_21,
      NATIVE_KERNEL_TARGET_22,NATIVE_KERNEL_TARGET_20,NATIVE_KERNEL_TARGET_0};
    static constexpr std::array indices{11,12,13,14,15};
  };
  template<architecture Family> struct kernel_policy_family {
    using profiles=kernel_profiles<Family>;
    using refinement=abi_refinement<typename profiles::raw,typename profiles::storage>;
    using policies=typename refinement::policies;
    using memory_refinement=abi_refinement<policies,typename profiles::memory>;
    static_assert(std::same_as<policies,typename memory_refinement::policies>);
    static_assert(profiles::targets.size()==refinement::cells.size);
    static_assert(profiles::indices.size()==refinement::cells.size);
    static_assert([] {
      for(std::size_t i=0;i<profiles::targets.size();++i)
        if(target_features<Family>(profiles::targets[i])!=refinement::cells.records[i].requirements)
          return false;
      return true;
    }());
  };
  using host_kernel_profiles=kernel_profiles<target_arch>;
  using raw_kernel_policies=host_kernel_profiles::raw;
  using wide_storage_policies=host_kernel_profiles::storage;
  using memory_kernel_policies=host_kernel_profiles::memory;
  using wide_kernel_refinement=kernel_policy_family<target_arch>::refinement;
  using wide_kernel_policies=kernel_policy_family<target_arch>::policies;
  using wide_memory_refinement=kernel_policy_family<target_arch>::memory_refinement;
}
