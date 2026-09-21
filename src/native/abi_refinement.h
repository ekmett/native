// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <native/isa.h>
#include <array>
#include <cstddef>
#include <utility>
#include <type_traits>

// Internal ordered common refinement. Preserve each callee's first-match policy;
// no public policy-composition API or compiler retargeting is introduced.
namespace native::detail {
  template<class List> struct abi_entries;
  template<> struct abi_entries<::native::isa_list<>> {
    using isa_type=::native::isa<>;
    static constexpr std::array<isa_type,0> requirements{};
  };
  template<auto First,auto... E> struct abi_entries<::native::isa_list<First,E...>> {
    using isa_type=std::remove_cv_t<decltype(::native::detail::abi_match<First,0>::required_features)>;
    static constexpr std::array<isa_type,1+sizeof...(E)> requirements{
      ::native::detail::abi_match<First,0>::required_features,
      ::native::detail::abi_match<E,0>::required_features...};
  };

  template<class... Lists> struct refinement_isa { using type=::native::isa<>; };
  template<class First,class... Rest> struct refinement_isa<First,Rest...> {
    using type=std::conditional_t<abi_entries<First>::requirements.empty(),
      typename refinement_isa<Rest...>::type,typename abi_entries<First>::isa_type>;
  };

  template<class... Lists> struct abi_refinement {
    static_assert(sizeof...(Lists)>0);
    using isa_type=typename refinement_isa<Lists...>::type;
    static_assert(((abi_entries<Lists>::requirements.empty() ||
      std::same_as<isa_type,typename abi_entries<Lists>::isa_type>) && ...),
      "ABI refinement requires one architecture family");
    static constexpr std::size_t dimensions=sizeof...(Lists);
    static constexpr std::size_t capacity=(std::size_t{1} * ... * abi_entries<Lists>::requirements.size());
    struct cell {
      isa_type requirements{};
      std::array<int,dimensions> choices{};
      bool possible=false;
    };
    struct table {
      std::array<cell,capacity> records{};
      std::size_t size=0;
    };

    template<std::size_t K> static consteval auto choices() {
      constexpr std::array radices{abi_entries<Lists>::requirements.size()...};
      std::array<int,dimensions> result{};
      auto remaining=K;
      // Last coordinate varies fastest: preserve lexicographic policy priority.
      for(std::size_t i=dimensions;i>0;--i) {
        result[i-1]=int(remaining%radices[i-1]);
        remaining/=radices[i-1];
      }
      return result;
    }
    template<std::size_t K,std::size_t... I>
    static consteval cell candidate(std::index_sequence<I...>) {
      constexpr auto selected=choices<K>();
      constexpr auto requirements=::native::feature_closure(
        (isa_type{} & ... & abi_entries<Lists>::requirements[selected[I]]));
      constexpr auto A=requirements;
      constexpr bool possible=((::native::abi_lookup<A,Lists>::matched &&
        ::native::abi_lookup<A,Lists>::index==selected[I]) && ...);
      return {requirements,selected,possible};
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
      -> ::native::isa_list<cells.records[I].requirements...>;
    using policies=decltype(policies_for(std::make_index_sequence<cells.size>{}));

    // Verify the realized cell against each original callee, rather than only
    // checking membership in the generated positive-requirement list.
    template<isa_type A> static consteval bool agrees() {
      using choice=::native::abi_lookup<A,policies>;
      constexpr bool all=(::native::abi_lookup<A,Lists>::matched && ...);
      if constexpr(!all) return !choice::matched;
      else if constexpr(!choice::matched) return false;
      else return cells.records[choice::index].choices==
        std::array<int,dimensions>{::native::abi_lookup<A,Lists>::index...};
    }
  };
}
