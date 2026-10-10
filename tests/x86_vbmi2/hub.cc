// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <cstdint>
#include <hint.h>
import native;

constexpr auto arch = native::target_features<native::x86>("avx512vbmi2");
using V = native::simd<std::uint16_t, 32, arch>;

// The family module owns the full corpus; this checks the omnibus export path.
hint_noinline hint_target("avx512vbmi2") constexpr bool hub_values(unsigned first) {
  std::array<std::uint16_t, 32> input{};
  for (unsigned i = 0; i < input.size(); ++i) input[i] = static_cast<std::uint16_t>(i + first);
  auto mask = native::predicate<32, arch>::from_bitset(0x80010001u);
  auto packed = native::maskz_vpcompressw<arch>(mask, V::load(input.data()));
  std::array<std::uint16_t, 32> output{};
  packed.store(output.data());
  if (output[0] != input[0] || output[1] != input[16] || output[2] != input[31] || output[3] != 0) return false;
  native::maskz_vpexpandw<arch>(mask, packed).store(output.data());
  for (unsigned i = 0; i < output.size(); ++i)
    if (output[i] != (((0x80010001u >> i) & 1) ? input[i] : 0)) return false;
  return true;
}
static_assert(hub_values(1));

extern "C" hint_noinline unsigned long long
native_vbmi2_baseline_import(unsigned long long value) noexcept { return (value >> 3) ^ (value + 17); }
int main(int argc, char **) {
  auto value = static_cast<unsigned long long>(argc);
  if (native_vbmi2_baseline_import(value) != ((value >> 3) ^ (value + 17))) return 1;
  if (!native::classify_isa(native::observe_x86_capabilities(), arch).admitted()) return 77;
  return hub_values(static_cast<unsigned>(argc)) ? 0 : 1;
}
