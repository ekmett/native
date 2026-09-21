// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <initializer_list>
#include <native/attributes.h>
#if NATIVE_TEST_INTERFACE == 1
import native.x86.vbmi;
#elif NATIVE_TEST_INTERFACE == 2
import native;
#else
#error Select the module or hub interface
#endif
#include "checks.h"

extern "C" native_noinline unsigned long long
native_vbmi_baseline_import(unsigned long long value) noexcept {
  return (value >> 3) ^ (value + 17);
}

int main(int argc, char **) {
  auto probe = static_cast<unsigned long long>(argc);
  if (native_vbmi_baseline_import(probe) != ((probe >> 3) ^ (probe + 17))) {
    return 1;
  }
  auto cpu = native::observe_x86_capabilities();
  if (!native::classify_isa(cpu, full_width).admitted()) {
    std::puts("SKIP VBMI: CPU features or OS ZMM state unavailable; constexpr checks passed.");
    return 77;
  }
  std::uint64_t state = 0x9e3779b97f4a7c15ull;
  if (!check_vectors<64>(state)) {
    return 2;
  }
  if (native::classify_isa(cpu, short_width).admitted()) {
    if (!check_vectors<16>(state) || !check_vectors<32>(state)) {
      return 3;
    }
  } else {
    std::puts("SKIP VBMI narrow forms: AVX512VL unavailable.");
  }
  if (native::classify_isa(cpu, broad).admitted()) {
    if (!check_vectors<16, true>(state) || !check_vectors<32, true>(state) ||
        !check_vectors<64, true>(state)) {
      return 4;
    }
  } else {
    std::puts("SKIP VBMI broader-tag forms: AVX512DQ unavailable.");
  }
  std::puts("VBMI runtime permutation, multishift and mask checks passed.");
  return 0;
}
