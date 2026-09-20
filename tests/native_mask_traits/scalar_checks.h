// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <array>
#include <concepts>
#include <cstddef>
#include <type_traits>

namespace mask_fixture {
  template<class T> concept has_mask = requires { typename native::mask<T>; };
  struct unsupported {};
  struct incomplete;
  struct member_only { using mask_type = bool; };
  enum class enumeration { value };

  struct custom_mask { bool value; };
  struct custom_scalar {
    int value;
    friend constexpr custom_mask operator<(custom_scalar a, custom_scalar b) {
      return {a.value < b.value};
    }
  };
}

// A downstream specialization must work after a named-module import as well as
// after a textual include. The alias removes qualifiers before customization.
template<> struct native::mask_traits<mask_fixture::custom_scalar> {
  using type = mask_fixture::custom_mask;
};

namespace mask_fixture {
  template<class... T> constexpr bool scalar_masks =
    (std::same_as<native::mask<T>, bool> && ...);
  static_assert(scalar_masks<bool, char, signed char, unsigned char, wchar_t,
    char8_t, char16_t, char32_t, short, unsigned short, int, unsigned int,
    long, unsigned long, long long, unsigned long long, float, double, long double>);
  static_assert(std::same_as<native::mask<float const volatile&>, bool>);
  static_assert(std::same_as<native::mask<int&&>, bool>);
  static_assert(std::same_as<native::mask_traits<custom_scalar>::type, custom_mask>);
  static_assert(std::same_as<native::mask<custom_scalar const&>, custom_mask>);
  static_assert(!has_mask<unsupported> && !has_mask<incomplete>);
  static_assert(!has_mask<member_only> && !has_mask<enumeration>);
  static_assert(!has_mask<void> && !has_mask<int*> && !has_mask<void()>);
  static_assert(std::is_empty_v<native::mask_traits<unsupported>>);
  static_assert(!has_mask<std::array<unsupported, 2>>);
  static_assert(!has_mask<std::array<unsupported, 0>>);

  static_assert(std::same_as<native::mask<std::array<float, 0>>, std::array<bool, 0>>);
  static_assert(std::same_as<native::mask<std::array<float, 3> const&>, std::array<bool, 3>>);
  static_assert(std::same_as<native::mask<std::array<custom_scalar, 2>>, std::array<custom_mask, 2>>);
  static_assert(std::same_as<native::mask<std::array<std::array<int, 2>, 3>>,
    std::array<std::array<bool, 2>, 3>>);

  template<class T, std::size_t N>
  native::mask<std::array<T, N>> compare(std::array<T, N> const& a, std::array<T, N> const& b) {
    native::mask<std::array<T, N>> result{};
    for (std::size_t i = 0; i != N; ++i) result[i] = a[i] < b[i];
    return result;
  }

  inline int check_scalar_masks(int seed) {
    auto arithmetic = compare(std::array{seed - 1, seed, seed + 1},
                              std::array{seed, seed, seed});
    if (!arithmetic[0] || arithmetic[1] || arithmetic[2]) return 1;
    auto custom = compare(std::array{custom_scalar{seed - 1}, custom_scalar{seed + 1}},
                          std::array{custom_scalar{seed}, custom_scalar{seed}});
    if (!custom[0].value || custom[1].value) return 2;
    return 0;
  }
}
