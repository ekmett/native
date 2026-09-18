// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <simd/isa.h>
#include <array>
#include <cstddef>
#include <utility>

// Internal ordered common refinement. Preserve each callee's first-match policy;
// no public policy-composition API or compiler retargeting is introduced.
namespace simd::detail {
  template<class List> struct abi_entries;
  template<class... E> struct abi_entries<simd::isa_list<E...>> {
    static constexpr std::array<simd::feature_set,sizeof...(E)> requirements{
      simd::detail::abi_match<E,0>::required_features...};
  };

  template<class... Lists> struct abi_refinement {
    static_assert(sizeof...(Lists)>0);
    static constexpr std::size_t dimensions=sizeof...(Lists);
    static constexpr std::size_t capacity=(std::size_t{1} * ... * abi_entries<Lists>::requirements.size());
    struct cell {
      simd::feature_set requirements{};
      std::array<std::size_t,dimensions> choices{};
      bool possible=false;
    };
    struct table {
      std::array<cell,capacity> records{};
      std::size_t size=0;
    };

    template<std::size_t K> static consteval auto choices() {
      constexpr std::array radices{abi_entries<Lists>::requirements.size()...};
      std::array<std::size_t,dimensions> result{};
      auto remaining=K;
      // Last coordinate varies fastest: preserve lexicographic policy priority.
      for(std::size_t i=dimensions;i>0;--i) {
        result[i-1]=remaining%radices[i-1];
        remaining/=radices[i-1];
      }
      return result;
    }
    template<std::size_t K,std::size_t... I>
    static consteval cell candidate(std::index_sequence<I...>) {
      constexpr auto selected=choices<K>();
      constexpr auto requirements=simd::feature_closure(
        (simd::feature_set{0} | ... | abi_entries<Lists>::requirements[selected[I]]));
      // Reject mixed-host unions before forming isa<requirements>, which is
      // intentionally ill-formed for an architecture spanning both hosts.
      if constexpr((requirements&simd::detail::x86_features) &&
                   (requirements&simd::detail::arm_features)) return {};
      else {
        using A=simd::isa<requirements>;
        constexpr bool possible=((simd::abi_lookup<A,Lists>::matched &&
          simd::abi_lookup<A,Lists>::index==selected[I]) && ...);
        return {requirements,selected,possible};
      }
    }
    template<std::size_t... K> static consteval table enumerate(std::index_sequence<K...>) {
      table result;
      auto append=[&](cell value) {
        if(value.possible) result.records[result.size++]=value;
      };
      (append(candidate<K>(std::index_sequence_for<Lists...>{})),...);
      return result;
    }
    static constexpr table cells=enumerate(std::make_index_sequence<capacity>{});
    template<std::size_t... I> static auto policies_for(std::index_sequence<I...>)
      -> simd::isa_list<simd::isa<cells.records[I].requirements>...>;
    using policies=decltype(policies_for(std::make_index_sequence<cells.size>{}));

    // Verify the realized cell against each original callee, rather than only
    // checking membership in the generated positive-requirement list.
    template<simd::architecture A> static consteval bool agrees() {
      using choice=simd::abi_lookup<A,policies>;
      constexpr bool all=(simd::abi_lookup<A,Lists>::matched && ...);
      if constexpr(!all) return !choice::matched;
      else if constexpr(!choice::matched) return false;
      else return cells.records[choice::index].choices==
        std::array<std::size_t,dimensions>{simd::abi_lookup<A,Lists>::index...};
    }
  };
}
