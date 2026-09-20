// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <array>
#include <cstddef>
#include <type_traits>

// Preserve the former storage and default constructor as an independent oracle.
template<class T,std::size_t N> struct old_wide {
  alignas(T) std::array<T,N> registers;
  constexpr old_wide() = default;
};
template<class T,std::size_t N> consteval bool same_traits() {
  using W=native::wide<T,N>;
  using O=old_wide<T,N>;
  static_assert(sizeof(W)==sizeof(O) && alignof(W)==alignof(O));
  static_assert(offsetof(W,registers)==offsetof(O,registers));
#define CHECK_TRAIT(name) static_assert(std::name<W> == std::name<O>);
  CHECK_TRAIT(is_default_constructible_v)
  CHECK_TRAIT(is_nothrow_default_constructible_v)
  CHECK_TRAIT(is_trivially_default_constructible_v)
  CHECK_TRAIT(is_copy_constructible_v)
  CHECK_TRAIT(is_nothrow_copy_constructible_v)
  CHECK_TRAIT(is_trivially_copy_constructible_v)
  CHECK_TRAIT(is_move_constructible_v)
  CHECK_TRAIT(is_nothrow_move_constructible_v)
  CHECK_TRAIT(is_trivially_move_constructible_v)
  CHECK_TRAIT(is_copy_assignable_v)
  CHECK_TRAIT(is_nothrow_copy_assignable_v)
  CHECK_TRAIT(is_trivially_copy_assignable_v)
  CHECK_TRAIT(is_move_assignable_v)
  CHECK_TRAIT(is_nothrow_move_assignable_v)
  CHECK_TRAIT(is_trivially_move_assignable_v)
  CHECK_TRAIT(is_destructible_v)
  CHECK_TRAIT(is_nothrow_destructible_v)
  CHECK_TRAIT(is_trivially_destructible_v)
  CHECK_TRAIT(is_trivially_copyable_v)
  CHECK_TRAIT(is_standard_layout_v)
  CHECK_TRAIT(is_aggregate_v)
#undef CHECK_TRAIT
  // wide has additional converting constructors, so is_trivial need not equal
  // this storage-only oracle. A nontrivial default must remain nontrivial.
  static_assert(std::is_trivially_default_constructible_v<O> || !std::is_trivial_v<W>);
  return true;
}
