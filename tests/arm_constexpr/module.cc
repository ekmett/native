// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <concepts>
#include <utility>
#include <cstdint>
#if NATIVE_ARM_CONSTEXPR_HUB
import native;
#else
import native.arm.crc;
import native.arm.sha;
import native.arm.pmull;
#endif
#include "crc_checks.h"
constexpr auto sha1=native::isa<native::arm>(native::arm_feature::sha1);
consteval bool rotation_checks() {
  for(unsigned bit=0;bit<32;++bit) {
    auto word=std::uint32_t{1}<<bit;
    if(native::sha1h<sha1>(word)!=std::rotr(word,2)
      || native::sha1h<native::isa<native::arm>{}>(word)!=std::rotr(word,2)) return false;
  }
  return native::sha1h<sha1>(std::uint32_t{0x12345678})==0x048d159e
    && native::sha1h<native::isa<native::arm>{}>(std::uint32_t{0x12345678})==0x048d159e;
}
static_assert(rotation_checks());
// These modules were compiled without CRC/SHA. Literal calls still evaluate
// immediately and keep the owning module's default ISA.
static_assert(native::crc32(std::uint32_t{},std::uint8_t{1})==0x77073096);
static_assert(native::crc32c(std::uint32_t{},std::uint8_t{1})==0xf26b8303);
static_assert(native::sha1h(std::uint32_t{3})==0xc0000000);
template<class T> concept exact_sha_word = requires(T x) { native::sha1h<native::isa<native::arm>{}>(x); };
static_assert(!exact_sha_word<std::int32_t> && !exact_sha_word<std::uint16_t>
  && !exact_sha_word<std::uint64_t> && !exact_sha_word<float>);
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
namespace polynomial_constant_fixture {
  constexpr auto strong=native::feature_closure(native::arm_feature::pmull);
  constexpr auto weak=native::neon;
  template<native::isa<native::arm> A> constexpr auto words(native::simd<std::uint64_t,2,A> value) {
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
  template<native::isa<native::arm> A> concept shape = requires { native::pmull<A>(std::uint64_t{},std::uint64_t{}); };
  static_assert(!shape<native::scalar>);
}
// Family identity remains part of the requirement, including an empty set.
template<auto Arch> concept crc_family = requires { native::crc32<Arch>(std::uint32_t{},std::uint8_t{1}); };
template<auto Arch> concept sha_family = requires { native::sha1h<Arch>(std::uint32_t{1}); };
template<auto Arch> concept pmull_family = requires { native::pmull<Arch>(std::uint64_t{1},std::uint64_t{1}); };
static_assert(crc_family<native::isa<native::arm>{}> && sha_family<native::isa<native::arm>{}>);
static_assert(pmull_family<native::neon>);
static_assert(!crc_family<native::isa<native::x86>{}> && !crc_family<native::isa<native::wasm>{}>);
static_assert(!sha_family<native::isa<native::x86>{}> && !sha_family<native::isa<native::wasm>{}>);
static_assert(!pmull_family<native::isa<native::x86>{}> && !pmull_family<native::isa<native::wasm>{}>);
static_assert(std::same_as<decltype(native::arm_capabilities{}.present),native::isa<native::arm>>);
static_assert(std::same_as<decltype(native::arm_capabilities{}.observed),native::isa<native::arm>>);
template<class R> concept arm_admission = requires(native::arm_capabilities cpu,R requirement) {
  native::classify_isa(cpu,requirement);
};
static_assert(arm_admission<native::isa<native::arm>>);
static_assert(!arm_admission<native::isa<native::x86>> && !arm_admission<native::isa<native::wasm>>);
int main() { return 0; }
