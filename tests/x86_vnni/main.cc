// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <immintrin.h>
#include <native/attributes.h>
#include <native/targets.h>

#if NATIVE_TEST_INTERFACE == 1
import native.x86.vnni;
import native.x86.features;
#elif NATIVE_TEST_INTERFACE == 2
import native.x86;
#elif NATIVE_TEST_INTERFACE == 3
import native;
#else
#error Select the granular module, x86 umbrella or main hub interface.
#endif

#include "checks.h"

extern "C" native_noinline unsigned long long native_vnni_baseline_import(
    unsigned long long value) noexcept {
  return (value >> 3) ^ (value + 17);
}

int main(int argc, char** argv) {
  auto value = static_cast<unsigned long long>(argc);
  if (native_vnni_baseline_import(value) != ((value >> 3) ^ (value + 17))) return 2;
  return vnni_fixture::run(argc > 1 ? argv[1] : "core");
}
