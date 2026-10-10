// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <cstdint>
#include <hint.h>
import native;

constexpr auto arch = native::target_features<native::x86>("avx512bitalg,avx512vl");

// The family module owns the semantic cases; this checks the omnibus exports.
hint_noinline hint_target("avx512bitalg,avx512vl") constexpr bool hub_values(unsigned first) {
  std::array<std::uint16_t, 8> input{static_cast<std::uint16_t>(first & 1), 0xffff, 0x100, 0x80};
  using W = native::simd<std::uint16_t, 8, arch>;
  std::array<std::uint16_t, 8> output{};
  native::vpopcntw<arch>(W::load(input.data())).store(output.data());
  if (output != std::array<std::uint16_t, 8>{static_cast<std::uint16_t>(first & 1), 16, 1, 1}) return false;
  using B = native::simd<std::uint8_t, 16, arch>;
  std::array<std::uint8_t, 16> bytes{};
  native::vpopcntb<arch>(B{std::uint8_t{0xff}}).store(bytes.data());
  for (auto value : bytes) if (value != 8) return false;
  using Q = native::simd<std::uint64_t, 2, arch>;
  return native::vpshufbitqmb<arch>(Q{std::uint64_t{1}}, B{std::uint8_t{0}}).to_bitset() == 0xffff;
}
static_assert(hub_values(1));

extern "C" hint_noinline unsigned long long
native_bitalg_baseline_import(unsigned long long value) noexcept { return (value >> 3) ^ (value + 17); }
int main(int argc, char **) {
  auto value = static_cast<unsigned long long>(argc);
  if (native_bitalg_baseline_import(value) != ((value >> 3) ^ (value + 17))) return 1;
  if (!native::classify_isa(native::observe_x86_capabilities(), arch).admitted()) return 77;
  return hub_values(static_cast<unsigned>(argc)) ? 0 : 1;
}
