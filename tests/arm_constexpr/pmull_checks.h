// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
namespace polynomial_constant_fixture {
  constexpr auto strong=native::feature_closure(native::arm_feature::pmull);
  constexpr auto weak=native::neon;
  template<native::isa A> constexpr auto words(native::simd<std::uint64_t,2,A> value) {
    return std::bit_cast<std::array<std::uint64_t,2>>(value.to_native());
  }
  template<unsigned I> consteval bool row() {
    for(unsigned j=0;j<64;++j) {
      // The product of x^I and x^j is exactly x^(I+j), on either side of lane 64.
      std::array<std::uint64_t,2> expected{};
      expected[(I+j)/64]=std::uint64_t{1}<<((I+j)%64);
      auto a=std::uint64_t{1}<<I,b=std::uint64_t{1}<<j;
      if(words(native::pmull<strong>(a,b))!=expected || words(native::pmull<weak>(a,b))!=expected)
        return false;
    }
    return true;
  }
  template<unsigned I> struct basis_row { static_assert(row<I>()); };
  template<unsigned... I> consteval bool basis(std::integer_sequence<unsigned,I...>) {
    return ((sizeof(basis_row<I>)>0) && ...);
  }
  static_assert(basis(std::make_integer_sequence<unsigned,64>{}));
  // (1+x+...+x^63)^2 = 1+x^2+...+x^126 over GF(2).
  constexpr std::uint64_t all=~std::uint64_t{};
  constexpr std::array<std::uint64_t,2> alternating{0x5555555555555555,0x5555555555555555};
  static_assert(words(native::pmull<strong>(all,all))==alternating);
  static_assert(words(native::pmull<weak>(all,all))==alternating);
  static_assert(words(native::pmull<weak>(std::uint64_t{},all))==std::array<std::uint64_t,2>{});
  static_assert(words(native::pmull<weak>(all,std::uint64_t{}))==std::array<std::uint64_t,2>{});
  static_assert(words(native::pmull(std::uint64_t{3},std::uint64_t{3}))==std::array<std::uint64_t,2>{5,0});
  static_assert(std::same_as<decltype(native::pmull<weak>(all,all)),native::simd<std::uint64_t,2,weak>>);
  static_assert(std::same_as<decltype(native::pmull<strong>(all,all)),native::simd<std::uint64_t,2,strong>>);
  static_assert(std::same_as<decltype(native::pmull(all,all)),native::simd<std::uint64_t,2,weak>>);
  template<native::isa A> concept shape = requires { native::pmull<A>(std::uint64_t{},std::uint64_t{}); };
  static_assert(!shape<native::scalar>);
}
