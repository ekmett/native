// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <concepts>
#include <cstdint>
#include <initializer_list>
#include <utility>
import native;
#include "cases.h"

namespace abi_lookup_test {
  template<native::isa A,std::size_t L,std::size_t N>
    requires (exp_target<A> == 0)
  native::wide<native::vec<float,L,A>,N> preserve_shape(
    native::wide<native::vec<float,L,A>,N> const &);
  // Deduction retains the full caller ISA in a vector pack, including extra
  // features ignored by this operation's selected policy.
#if defined(__x86_64__) || defined(_M_X64)
  using pack=native::wide<native::vec<float,4,extra>,3>;
  static_assert(std::same_as<decltype(preserve_shape(std::declval<pack const &>())),pack>);
#endif
}
int main() {}
