// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <array>
#include <cstddef>
#include <type_traits>

namespace native {
  /// \ingroup types
  /// Specialize with `type` to associate a value with its logical mask type.
  /// Unsupported types have no default mask.
  template<class T> struct mask_traits {};

  /// \ingroup types
  /// Arithmetic scalar comparisons have a Boolean mask.
  template<class T> requires std::is_arithmetic_v<T>
  struct mask_traits<T> { using type = bool; };

  /// \ingroup types
  /// The logical mask associated with T, ignoring its cv/ref qualifiers.
  template<class T>
  using mask = typename mask_traits<std::remove_cvref_t<T>>::type;

  /// \ingroup types
  /// Preserve the shape of an array while mapping each element to its mask.
  /// This describes pointwise masks; it does not change std::array comparisons.
  template<class T, std::size_t N> requires requires { typename mask<T>; }
  struct mask_traits<std::array<T, N>> { using type = std::array<mask<T>, N>; };
}
