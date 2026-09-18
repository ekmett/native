// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "policies.h"
#include <bit>

namespace refinement_test {
  using namespace simd;
  using small=isa<feature::bmi1>;
  using large=isa<feature::aes>;
  using combined=isa<small::features|large::features>;
  static_assert(std::popcount(small::features)<std::popcount(large::features));
  using first=isa_list<small,large,scalar>;
  using second=isa_list<large,scalar>;
  using joined=refinement<first,second>;
  static_assert(joined::cells.size==4);
  // Preserve the original tuple priorities while filtering impossible cells.
  static_assert(joined::cells.records[0].choices==std::array<std::size_t,2>{0,0});
  static_assert(joined::cells.records[1].choices==std::array<std::size_t,2>{0,1});
  static_assert(joined::cells.records[2].choices==std::array<std::size_t,2>{1,0});
  static_assert(joined::cells.records[3].choices==std::array<std::size_t,2>{2,1});
  static_assert(joined::agrees<scalar>() && joined::agrees<small>() &&
    joined::agrees<large>() && joined::agrees<combined>());
  // One list retains its declared priority even when a lower-priority policy
  // has more prerequisites. Reversing that list reverses the winning type.
  using single=refinement<first>;
  using reverse=refinement<isa_list<large,small,scalar>>;
  static_assert(std::same_as<abi_lookup<combined,single::policies>::type,small>);
  static_assert(std::same_as<abi_lookup<combined,reverse::policies>::type,large>);
  static_assert(single::agrees<combined>() && reverse::agrees<combined>());

  using inherited=target_entry<small,feature_set(feature::aes)>;
  using minimum=refinement<isa_list<inherited,small>,isa_list<large,scalar>>;
  static_assert(minimum::agrees<small>() && minimum::agrees<combined>());
  static_assert(minimum::cells.size==2);
  static_assert(minimum::cells.records[0].requirements==combined::features);
  using mixed=refinement<isa_list<avx2,neon>,isa_list<neon,avx2>>;
  static_assert(mixed::cells.size==2);
  static_assert(mixed::agrees<avx2>() && mixed::agrees<neon>() && mixed::agrees<scalar>());
  using empty=refinement<isa_list<>,first>;
  static_assert(empty::cells.size==0 && empty::agrees<scalar>());
  static_assert(!requires_abi<scalar,empty::policies,abi_npos>);

  template<std::size_t K> consteval bool check_exp_boundary() {
    constexpr feature_set bits=avx2::features |
      ((K&1)?feature_set(feature::avx512f):0) |
      ((K&2)?feature_set(feature::avx512dq):0) |
      ((K&4)?feature_set(feature::avx512bw):0) |
      ((K&8)?feature_set(feature::avx512vl):0) |
      ((K&16)?feature_set(feature::avx512_bf16):0) |
      ((K&32)?feature_set(feature::avx512_fp16):0) |
      ((K&64)?feature_set(feature::aes):0);
    using A=isa<bits>;
    using selected=abi_lookup<A,exp_policies>;
    return exp_refinement::agrees<A>() && caller_refinement::agrees<A>() &&
      selected::index==abi_lookup<A,raw_exp_policies>::index &&
      (selected::type::features & (feature_set(feature::avx512_bf16)|
        feature_set(feature::avx512_fp16)))==0;
  }
  template<std::size_t... K> consteval bool exp_boundaries(std::index_sequence<K...>) {
    return (check_exp_boundary<K>() && ...);
  }
  static_assert(exp_refinement::cells.size==5 && exp_refinement::capacity==25);
  static_assert(std::same_as<exp_policies,raw_exp_policies>);
  static_assert(exp_boundaries(std::make_index_sequence<128>{}));
  static_assert(exp_refinement::agrees<scalar>() && exp_refinement::agrees<neon>());
}
int main() {}
