// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "simd/value_traits.h"

namespace {
  using namespace simd;
  using namespace simd::detail;
  struct custom_element {};
  struct custom_value { using architecture=kernel_full_half; };
  struct malformed_value { using architecture=int; };
  template<class A,class Expected> consteval bool raw_value_requirements() {
    return std::same_as<value_architecture_t<vec<float,1,A>>,Expected> &&
      std::same_as<value_architecture_t<vec<float,16,A> const>,Expected> &&
      std::same_as<value_architecture_t<vec<std::uint16_t,32,A>>,Expected> &&
      std::same_as<value_architecture_t<vec<mask_lane<std::uint32_t>,16,A>>,Expected> &&
      std::same_as<value_architecture_t<predicate<16,A>>,Expected> &&
      std::same_as<value_architecture_t<vec<custom_element,16,A>>,A>;
  }
  static_assert(raw_value_requirements<kernel_full_half,avx512>());
  static_assert(raw_value_requirements<kernel_bw_half,kernel_bw>());
  static_assert(raw_value_requirements<kernel_neon_half,neon>());
  static_assert(raw_value_requirements<avx2,avx2>());
  static_assert(raw_value_requirements<scalar,scalar>());
  static_assert(std::same_as<value_architecture_t<custom_value>,kernel_full_half>);
  static_assert(std::same_as<value_architecture_t<int>,void> &&
    std::same_as<value_architecture_t<malformed_value>,void>);
  static_assert(value_traits<vec<float,1,kernel_full_half>>::aggregate_default);
  static_assert(!value_traits<vec<float,2,kernel_full_half>>::aggregate_default &&
    !value_traits<vec<std::uint32_t,1,kernel_full_half>>::aggregate_default &&
    !value_traits<vec<custom_element,1,kernel_full_half>>::aggregate_default &&
    !value_traits<custom_value>::aggregate_default);
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
  constexpr std::array<unsigned,16> original_scopes{17,9,13,5,15,7,11,3,4,2,1,23,21,22,20,0};
  template<architecture A> consteval bool check() {
    using choice=abi_lookup<A,wide_kernel_policies>;
    return wide_kernel_refinement::agrees<A>() && choice::matched &&
      original_scopes[choice::index]==original_scope(A::features) &&
      (target<A,raw_kernel_policies> == 6 && A::features == 0) == scalar_architecture<A> &&
      (target<A,raw_kernel_policies> == 4) == avx2_architecture<A> &&
      (target<A,raw_kernel_policies> == 3) == avx512_nobw_novl_architecture<A> &&
      (target<A,raw_kernel_policies> == 1) == avx512_bw_novl_architecture<A> &&
      (target<A,raw_kernel_policies> == 2) == avx512_nobw_vl_architecture<A> &&
      (target<A,raw_kernel_policies> == 0) == avx512_architecture<A> &&
      (target<A,raw_kernel_policies> == 5) == neon_architecture<A>;
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
  static_assert(!std::same_as<isa<feature::bmi1>,scalar>);
  static_assert(target<isa<feature::bmi1>,wide_kernel_policies> == 15);
  static_assert(target<avx2,wide_kernel_policies> != target_npos);
}
int main() {}
