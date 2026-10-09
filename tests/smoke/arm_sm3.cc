// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cstdint>
#include <cstdio>
import native.arm.sm3;

namespace semantic = native::detail::arm_sm3_constant;

int main() noexcept {
  using words = std::array<std::uint32_t, 4>;
  using V = native::simd<std::uint32_t, 4>;
  // Distinct lanes, wrapping sums and mixed bits distinguish the four round
  // variants. Volatile input exercises the semantic bodies at runtime without requiring SM3 hardware.
  volatile std::uint32_t input[]{
    0xffffffff, 0x13579bdf, 0x2468ace0, 0x89abcdef,
    0x01234567, 0x76543210, 0xfedcba98, 0x76543210,
    0x80000001, 0x01234567, 0xfedcba98, 0x7fffffff
  };
  auto a = std::bit_cast<V>(words{input[0], input[1], input[2], input[3]});
  auto b = std::bit_cast<V>(words{input[4], input[5], input[6], input[7]});
  auto c = std::bit_cast<V>(words{input[8], input[9], input[10], input[11]});
  // Fixed answers from the independent truth-table reference in arm_sm_crypto.
  auto check = [](char const * name, V actual, words expected) noexcept {
    if (std::bit_cast<words>(actual) == expected) return true;
    std::printf("SM3 semantic check failed: %s\n", name);
    return false;
  };
  return !(check("ss1", semantic::sm3ss1(a, b, c), {0, 0, 0, 0x999554d9}) &&
    check("tt1a", semantic::sm3tt1a<0>(a, b, c), {0x13579bdf, 0xd159c048, 0x89abcdef, 0x091fc55a}) &&
    check("tt1b", semantic::sm3tt1b<1>(a, b, c), {0x13579bdf, 0xd159c048, 0x89abcdef, 0xcd199ddf}) &&
    check("tt2a", semantic::sm3tt2a<2>(a, b, c), {0x13579bdf, 0x67012345, 0x89abcdef, 0x76e56e9b}) &&
    check("tt2b", semantic::sm3tt2b<3>(a, b, c), {0x13579bdf, 0x67012345, 0x89abcdef, 0x088d3d4e}) &&
    check("partw1", semantic::sm3partw1(a, b, c), {0xc5c5a3a3, 0x83836d6d, 0x321a9ef6, 0xae379d04}) &&
    check("partw2", semantic::sm3partw2(a, b, c), {0xfedcba58, 0xf4a11a4f, 0xb4e95a07, 0x2a4a3878}));
}
