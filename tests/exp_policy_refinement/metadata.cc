// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "policies.h"
#include <bit>

namespace refinement_test {
  using namespace native;
  constexpr isa<x86> empty_x86{};
  constexpr isa small=feature_closure(x86_feature::bmi1);
  constexpr isa large=feature_closure(x86_feature::aes);
  constexpr isa combined=small&large;
  constexpr auto count=[](isa<x86> value) { unsigned n=0;for(auto word:value.flags)n+=std::popcount(word);return n; };
  static_assert(count(small)<count(large));
  using first=isa_list<small,large,empty_x86>;
  using second=isa_list<large,empty_x86>;
  using joined=refinement<first,second>;
  static_assert(joined::cells.size==4);
  // Preserve the original tuple priorities while filtering impossible cells.
  static_assert(joined::cells.records[0].choices==std::array<int,2>{0,0});
  static_assert(joined::cells.records[1].choices==std::array<int,2>{0,1});
  static_assert(joined::cells.records[2].choices==std::array<int,2>{1,0});
  static_assert(joined::cells.records[3].choices==std::array<int,2>{2,1});
  static_assert(joined::agrees<empty_x86>() && joined::agrees<small>() &&
    joined::agrees<large>() && joined::agrees<combined>());
  // One list retains its declared priority even when a lower-priority policy
  // has more prerequisites. Reversing that list reverses the winning requirement.
  using single=refinement<first>;
  using reverse=refinement<isa_list<large,small,empty_x86>>;
  static_assert(abi_lookup<combined,single::policies>::architecture==small);
  static_assert(abi_lookup<combined,reverse::policies>::architecture==large);
  static_assert(single::agrees<combined>() && reverse::agrees<combined>());

  constexpr auto inherited=target_entry{small,x86_feature::aes};
  using minimum=refinement<isa_list<inherited,small>,isa_list<large,empty_x86>>;
  static_assert(minimum::agrees<small>() && minimum::agrees<combined>());
  static_assert(minimum::cells.size==2);
  static_assert(minimum::cells.records[0].requirements==combined);
  // Each platform has its own refinement; foreign requirements cannot form a cell.
  using arm_joined=refinement<isa_list<neon_fp16,neon,isa<arm>{}>,
    isa_list<neon_bf16,neon,isa<arm>{}>>;
  static_assert(arm_joined::agrees<neon>() && arm_joined::agrees<neon_fp16>() &&
    arm_joined::agrees<neon_bf16>() && arm_joined::agrees<isa<arm>{}>());
  using empty=refinement<isa_list<>,first>;
  static_assert(empty::cells.size==0 && empty::agrees<empty_x86>());
  static_assert(abi_lookup<empty_x86,empty::policies>::index == -1);

  template<std::size_t K> consteval bool check_exp_boundary() {
    constexpr isa A=feature_closure(avx2 &
      ((K&1)?isa(x86_feature::avx512f):empty_x86) &
      ((K&2)?isa(x86_feature::avx512dq):empty_x86) &
      ((K&4)?isa(x86_feature::avx512bw):empty_x86) &
      ((K&8)?isa(x86_feature::avx512vl):empty_x86) &
      ((K&16)?isa(x86_feature::avx512bf16):empty_x86) &
      ((K&32)?isa(x86_feature::avx512fp16):empty_x86) &
      ((K&64)?isa(x86_feature::aes):empty_x86));
    using selected=abi_lookup<A,exp_policies>;
    return exp_target<A> == selected::index &&
      !selected::architecture.has(x86_feature::avx512bf16) &&
      !selected::architecture.has(x86_feature::avx512fp16);
  }
  template<std::size_t... K> consteval bool exp_boundaries(std::index_sequence<K...>) {
    return (check_exp_boundary<K>() && ...);
  }
  static_assert(exp_boundaries(std::make_index_sequence<128>{}));
  static_assert(exp_target<empty_x86> == -1);
}
int main() {}
