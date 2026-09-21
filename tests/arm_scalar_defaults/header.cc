// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/crc.h>
#include <cstdint>
template<class T> concept default_crc32 = requires(std::uint32_t c,T x) { native::crc32(c,x); };
template<class T> concept default_crc32c = requires(std::uint32_t c,T x) { native::crc32c(c,x); };
template<class T> consteval bool contract() {
  constexpr auto crc = native::feature_closure(native::arm_feature::crc);
  return !default_crc32<T> && !default_crc32c<T> && requires(std::uint32_t c,T x) {
    native::crc32<crc>(c,x); native::crc32c<crc>(c,x);
  };
}
static_assert(contract<std::uint8_t>() && contract<std::uint16_t>()
  && contract<std::uint32_t>() && contract<std::uint64_t>());
