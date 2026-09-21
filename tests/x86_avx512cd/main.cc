// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <initializer_list>
#include <immintrin.h>
#include <native/attributes.h>
#if NATIVE_TEST_INTERFACE == 1
import native.x86.avx512cd;
#elif NATIVE_TEST_INTERFACE == 2
import native;
#else
#error Select the module or hub interface
#endif
#include "checks.h"

extern "C" native_noinline unsigned long long native_avx512cd_baseline_import(unsigned long long value) noexcept {
  return (value >> 3) ^ (value + 17);
}
int main(int argc,char**) {
  auto probe=static_cast<unsigned long long>(argc);
  if(native_avx512cd_baseline_import(probe)!=((probe>>3)^(probe+17))) return 1;
  auto cpu=native::observe_x86_capabilities();
  if(!native::classify_isa(cpu,cd512).admitted()) {
    std::puts("SKIP AVX512CD: CPU features or OS ZMM state unavailable; constexpr/shape checks passed."); return 77;
  }
  std::uint64_t state=0x9e3779b97f4a7c15ull;
  if(!check_vectors<std::uint32_t,16>(state) || !check_vectors<std::uint64_t,8>(state)) return 2;
  if(native::classify_isa(cpu,cdvl).admitted()) {
    if(!check_vectors<std::uint32_t,4>(state) || !check_vectors<std::uint32_t,8>(state) ||
       !check_vectors<std::uint64_t,2>(state) || !check_vectors<std::uint64_t,4>(state)) return 3;
  } else std::puts("SKIP AVX512CD narrow forms: AVX512VL unavailable.");
  std::puts("AVX512CD runtime lane and mask checks passed.");
  return 0;
}
