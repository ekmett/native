// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "simd/kernel_policies.h"

namespace {
  using namespace simd;
  using namespace simd::detail;
  // Independent description of the original declaration boundaries. Checking
  // every subset catches priority mistakes hidden by the named presets.
  consteval unsigned original_scope(feature_set bits) {
    auto base=avx2::features|feature::avx512f|feature::avx512dq;
    if((bits&base)==base)
      return 2+((bits&feature_set(feature::avx512bw))?1:0)+
        ((bits&feature_set(feature::avx512vl))?2:0)+
        ((bits&feature_set(feature::avx512_bf16))?4:0)+
        ((bits&feature_set(feature::avx512_fp16))?8:0);
    if((bits&avx2::features)==avx2::features) return 1;
    if(bits&feature_set(feature::neon))
      return 20+((bits&feature_set(feature::neon_bf16))?1:0)+
        ((bits&feature_set(feature::neon_fp16))?2:0);
    return 0;
  }
  template<architecture A> consteval bool check() {
    using choice=abi_lookup<A,wide_kernel_policies>;
    return wide_kernel_refinement::agrees<A>() && choice::matched &&
      wide_kernel_scopes[choice::index]==original_scope(A::features) &&
      kernel_scalar_architecture<A> == scalar_architecture<A> &&
      kernel_avx2_architecture<A> == avx2_architecture<A> &&
      kernel_base_architecture<A> == avx512_nobw_novl_architecture<A> &&
      kernel_bw_architecture<A> == avx512_bw_novl_architecture<A> &&
      kernel_vl_architecture<A> == avx512_nobw_vl_architecture<A> &&
      kernel_avx512_architecture<A> == avx512_architecture<A> &&
      kernel_neon_architecture<A> == neon_architecture<A>;
  }
  template<std::size_t K> consteval bool x86_boundary() {
    constexpr auto bits=((K&1)?avx2::features:0) |
      ((K&2)?feature_set(feature::avx512f):0) |
      ((K&4)?feature_set(feature::avx512dq):0) |
      ((K&8)?feature_set(feature::avx512bw):0) |
      ((K&16)?feature_set(feature::avx512vl):0) |
      ((K&32)?feature_set(feature::avx512_bf16):0) |
      ((K&64)?feature_set(feature::avx512_fp16):0) |
      ((K&128)?feature_set(feature::aes):0);
    return check<isa<bits>>();
  }
  template<std::size_t... I> consteval bool x86_boundaries(std::index_sequence<I...>) {
    return (x86_boundary<I>() && ...);
  }
  static_assert(x86_boundaries(std::make_index_sequence<256>{}));
  static_assert(check<scalar>() && check<neon>() && check<neon_bf16>() &&
    check<neon_fp16>() && check<kernel_neon_half>());
  static_assert(!kernel_scalar_architecture<isa<feature::bmi1>>);
  static_assert(requires_abi<isa<feature::bmi1>,wide_kernel_policies,15>);
  static_assert(wide_scope_index<6>()==abi_npos);
  static_assert(!requires_abi<avx2,wide_kernel_policies,abi_npos>);
}
int main() {}
