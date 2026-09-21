// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <cstdint>
import native;

constexpr auto storage=native::target_features<native::x86>("sse2");
static_assert([] consteval {
  using words=native::simd<std::uint64_t,2,storage>;
  using bytes=native::simd<std::uint8_t,16,storage>;
  using signed_bytes=native::simd<std::int8_t,16,storage>;
  using dwords=native::simd<std::int32_t,4,storage>;
  std::array<std::uint64_t,2> q{};
  native::pclmulqdq<storage,0>(words(std::uint64_t{3}),words(std::uint64_t{3})).store(q.data());
  if(q[0]!=5 || q[1]!=0) return false;
  native::vpclmulqdq<storage,0>(words(std::uint64_t{3}),words(std::uint64_t{3})).store(q.data());
  if(q[0]!=5 || q[1]!=0) return false;
  native::vpopcntq<storage>(words(~std::uint64_t{0})).store(q.data());
  if(q[0]!=64 || q[1]!=64) return false;
  std::array<std::uint8_t,16> b{};
  native::gf2p8mulb<storage>(bytes(std::uint8_t{0x57}),bytes(std::uint8_t{0x83})).store(b.data());
  for(auto value:b) if(value!=0xc1) return false;
  std::array<std::int32_t,4> d{};
  native::dpbusd<storage>(dwords(std::int32_t{1}),bytes(std::uint8_t{255}),signed_bytes(std::int8_t{-128})).store(d.data());
  for(auto value:d) if(value!=1-4*255*128) return false;
  return true;
}());
