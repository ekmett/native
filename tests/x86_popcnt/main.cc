// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <bit>
#include <concepts>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <native/targets.h>
#if NATIVE_TEST_INTERFACE == 0
#include <native/x86/popcnt.h>
import native.x86.features;
#elif NATIVE_TEST_INTERFACE == 1
import native.x86.popcnt;
#elif NATIVE_TEST_INTERFACE == 2
import native;
#else
#error Select the header, direct module or main hub test interface.
#endif
#include "checks.h"

int main(int argc, char**) {
  auto cpu = native::observe_x86_capabilities();
  if (!native::classify_isa(cpu, requirements, NATIVE_TARGET_MINIMUM).admitted()) {
    std::puts("POPCNT unavailable; compile-time and synthetic admission checks passed.");
    return 77;
  }
  auto state = std::uint64_t{0x9e3779b97f4a7c15ull} ^ static_cast<unsigned>(argc);
  if (!check_width<std::uint16_t>(state)) return 1;
  if (!check_width<std::uint32_t>(state)) return 2;
  if (!check_width<std::uint64_t>(state)) return 3;
  return 0;
}
