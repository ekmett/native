// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <initializer_list>
#include <native/attributes.h>
#if NATIVE_TEST_INTERFACE == 1
import native.x86.bitalg;
#elif NATIVE_TEST_INTERFACE == 2
import native;
#else
#error Select the module or hub interface
#endif
#include "checks.h"

extern "C" native_noinline unsigned long long
native_bitalg_baseline_import(unsigned long long value) noexcept {
  return (value >> 3) ^ (value + 17);
}

int main(int argc, char **) {
  auto probe = static_cast<unsigned long long>(argc);
  if (native_bitalg_baseline_import(probe) != ((probe >> 3) ^ (probe + 17))) {
    return 1;
  }
  auto cpu = native::observe_x86_capabilities();
  if (!native::classify_isa(cpu, full_width).admitted()) {
    std::puts("SKIP BITALG: CPU features or OS ZMM state unavailable; constexpr checks passed.");
    return 77;
  }
  std::uint64_t state = 0x9e3779b97f4a7c15ull;
  if (!check_population<std::uint8_t, 64>(state) ||
      !check_population<std::uint16_t, 32>(state) || !check_shuffle<8>(state)) {
    return 2;
  }
  if (native::classify_isa(cpu, short_width).admitted()) {
    if (!check_population<std::uint8_t, 16>(state) ||
        !check_population<std::uint8_t, 32>(state) ||
        !check_population<std::uint16_t, 8>(state) ||
        !check_population<std::uint16_t, 16>(state) ||
        !check_shuffle<2>(state) || !check_shuffle<4>(state)) {
      return 3;
    }
  } else {
    std::puts("SKIP BITALG narrow forms: AVX512VL unavailable.");
  }
  if (native::classify_isa(cpu, broad).admitted()) {
    if (!check_population<std::uint8_t, 16, true>(state) ||
        !check_population<std::uint8_t, 32, true>(state) ||
        !check_population<std::uint8_t, 64, true>(state) ||
        !check_population<std::uint16_t, 8, true>(state) ||
        !check_population<std::uint16_t, 16, true>(state) ||
        !check_population<std::uint16_t, 32, true>(state) ||
        !check_shuffle<2, true>(state) || !check_shuffle<4, true>(state) ||
        !check_shuffle<8, true>(state)) {
      return 4;
    }
  } else {
    std::puts("SKIP BITALG broader-tag forms: AVX512DQ unavailable.");
  }
  std::puts("BITALG runtime lane, bit-selection and mask checks passed.");
  return 0;
}
