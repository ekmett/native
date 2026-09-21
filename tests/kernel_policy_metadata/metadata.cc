// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "native/value_traits.h"

namespace {
  using namespace native;
  using namespace native::detail;
  struct custom_element {};
  struct custom_value { static constexpr isa architecture=kernel_full_half; };
  struct malformed_value { using architecture=int; };
  struct malformed_constant { static constexpr int architecture=0; };
  struct nonstatic_architecture { isa architecture{}; };
  struct mutable_architecture { inline static isa architecture{}; };
  template<isa A,isa Expected> consteval bool raw_value_requirements() {
    return (value_architecture_v<simd<float,1,A>> == Expected) &&
      (value_architecture_v<simd<float,16,A> const> == Expected) &&
      (value_architecture_v<simd<std::uint16_t,32,A>> == Expected) &&
      (value_architecture_v<simd<mask_lane<std::uint32_t>,16,A>> == Expected) &&
      (value_architecture_v<predicate<16,A>> == Expected) &&
      (value_architecture_v<simd<custom_element,16,A>> == A);
  }
  static_assert(raw_value_requirements<kernel_full_half,avx512>());
  static_assert(raw_value_requirements<kernel_bw_half,kernel_bw>());
  static_assert(raw_value_requirements<kernel_neon_half,neon>());
  static_assert(raw_value_requirements<avx2,avx2>());
  static_assert(raw_value_requirements<scalar,scalar>());
  static_assert((value_architecture_v<custom_value> == kernel_full_half));
  static_assert(!value_traits<int>::known && !value_traits<malformed_value>::known &&
    !value_traits<malformed_constant>::known &&
    !value_traits<nonstatic_architecture>::known && !value_traits<mutable_architecture>::known);
  static_assert(value_traits<simd<float,1,kernel_full_half>>::aggregate_default);
  static_assert(!value_traits<simd<float,2,kernel_full_half>>::aggregate_default &&
    !value_traits<simd<std::uint32_t,1,kernel_full_half>>::aggregate_default &&
    !value_traits<simd<custom_element,1,kernel_full_half>>::aggregate_default &&
    !value_traits<custom_value>::aggregate_default);
  // Independent description of the original declaration boundaries. Checking
  // every subset catches priority mistakes hidden by the named presets.
  consteval unsigned original_scope(isa bits) {
    constexpr auto base=feature_closure(avx2&x86_feature::avx512f&x86_feature::avx512dq);
    if(bits.has(base))
      return 2+(bits.has(x86_feature::avx512bw)?1:0)+
        (bits.has(x86_feature::avx512vl)?2:0)+
        (bits.has(x86_feature::avx512bf16)?4:0)+
        (bits.has(x86_feature::avx512fp16)?8:0);
    if(bits.has(avx2)) return 1;
    if(bits.has(arm_feature::neon))
      return 20+(bits.has(arm_feature::neon_bf16)?1:0)+
        (bits.has(arm_feature::neon_fp16)?2:0);
    return 0;
  }
  constexpr std::array<unsigned,16> original_scopes{17,9,13,5,15,7,11,3,4,2,1,23,21,22,20,0};
  template<isa A> consteval bool check() {
    using choice=abi_lookup<A,wide_kernel_policies>;
    constexpr auto base=feature_closure(avx2&x86_feature::avx512f&x86_feature::avx512dq);
    constexpr auto raw=abi_lookup<A,raw_kernel_policies>::index;
    return wide_kernel_refinement::agrees<A>() && choice::matched &&
      original_scopes[choice::index]==original_scope(A) &&
      (raw==6 && A==scalar) == (A==scalar) &&
      (raw==4) == (A.has(avx2) && !A.has(base)) &&
      (raw==3) == (A.has(base) && !A.has(x86_feature::avx512bw) && !A.has(x86_feature::avx512vl)) &&
      (raw==1) == (A.has(base) && A.has(x86_feature::avx512bw) && !A.has(x86_feature::avx512vl)) &&
      (raw==2) == (A.has(base) && !A.has(x86_feature::avx512bw) && A.has(x86_feature::avx512vl)) &&
      (raw==0) == A.has(avx512) &&
      (raw==5) == A.has(neon);
  }
  template<std::size_t K> consteval bool x86_boundary() {
    constexpr isa A=feature_closure(((K&1)?avx2:scalar) &
      ((K&2)?isa(x86_feature::avx512f):scalar) &
      ((K&4)?isa(x86_feature::avx512dq):scalar) &
      ((K&8)?isa(x86_feature::avx512bw):scalar) &
      ((K&16)?isa(x86_feature::avx512vl):scalar) &
      ((K&32)?isa(x86_feature::avx512bf16):scalar) &
      ((K&64)?isa(x86_feature::avx512fp16):scalar) &
      ((K&128)?isa(x86_feature::aes):scalar));
    return check<A>();
  }
  // Evaluate each boundary once, with its own constexpr step budget.
  template<std::size_t I> constexpr bool x86_checked=x86_boundary<I>();
  template<std::size_t... I> consteval bool x86_boundaries(std::index_sequence<I...>) {
    return (x86_checked<I> && ...);
  }
  static_assert(x86_boundaries(std::make_index_sequence<256>{}));
  static_assert(check<scalar>() && check<neon>() && check<neon_bf16>() &&
    check<neon_fp16>() && check<kernel_neon_half>());
  static_assert(isa(x86_feature::bmi1)!=scalar);
  static_assert(abi_lookup<isa(x86_feature::bmi1),wide_kernel_policies>::index == 15);
  static_assert(abi_lookup<avx2,wide_kernel_policies>::index != -1);
}
int main() {}
