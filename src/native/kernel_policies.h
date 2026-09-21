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
  inline constexpr isa kernel_base=feature_closure(avx2 & x86_feature::avx512f & x86_feature::avx512dq);
  inline constexpr isa kernel_bw=feature_closure(kernel_base & x86_feature::avx512bw);
  inline constexpr isa kernel_vl=feature_closure(kernel_base & x86_feature::avx512vl);
  inline constexpr isa kernel_bw_bf16=feature_closure(kernel_bw & x86_feature::avx512bf16);
  inline constexpr isa kernel_bw_fp16=feature_closure(kernel_bw & x86_feature::avx512fp16);
  inline constexpr isa kernel_bw_half=feature_closure(kernel_bw_bf16 & x86_feature::avx512fp16);
  inline constexpr isa kernel_full_half=feature_closure(avx512_bf16 & x86_feature::avx512fp16);
  inline constexpr isa kernel_neon_half=feature_closure(neon_bf16 & arm_feature::neon_fp16);

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

  using wide_kernel_refinement=abi_refinement<raw_kernel_policies,wide_storage_policies>;
  using wide_kernel_policies=wide_kernel_refinement::policies;
  static_assert(wide_kernel_refinement::cells.size==16);
  // Generic memory helpers have a different partition, but every wide cell
  // already refines it. Check that fact without multiplying the body matrix.
  using wide_memory_refinement=abi_refinement<wide_kernel_policies,memory_kernel_policies>;
  static_assert(std::same_as<wide_kernel_policies,wide_memory_refinement::policies>);

  static_assert([] {
    constexpr std::array targets{
      NATIVE_KERNEL_TARGET_17, NATIVE_KERNEL_TARGET_9, NATIVE_KERNEL_TARGET_13,
      NATIVE_KERNEL_TARGET_5, NATIVE_KERNEL_TARGET_15, NATIVE_KERNEL_TARGET_7,
      NATIVE_KERNEL_TARGET_11, NATIVE_KERNEL_TARGET_3, NATIVE_KERNEL_TARGET_4,
      NATIVE_KERNEL_TARGET_2, NATIVE_KERNEL_TARGET_1, NATIVE_KERNEL_TARGET_23,
      NATIVE_KERNEL_TARGET_21, NATIVE_KERNEL_TARGET_22, NATIVE_KERNEL_TARGET_20,
      NATIVE_KERNEL_TARGET_0};
    static_assert(targets.size()==wide_kernel_refinement::cells.size);
    for(std::size_t i=0;i<targets.size();++i)
      if(target_features(targets[i])!=wide_kernel_refinement::cells.records[i].requirements)
        return false;
    return true;
  }());
}
