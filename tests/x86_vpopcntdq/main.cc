// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <initializer_list>
#include <limits>
#include <type_traits>
#include <immintrin.h>
#include <native/attributes.h>
#include <native/targets.h>
#if NATIVE_TEST_INTERFACE == 0
#include <native/x86/vpopcntdq.h>
import native.x86.features;
#elif NATIVE_TEST_INTERFACE == 1
import native.x86.vpopcntdq;
#elif NATIVE_TEST_INTERFACE == 2
import native;
#else
#error Select the header, direct module or main hub test interface.
#endif
#include "checks.h"

extern "C" native_noinline unsigned long long native_vpopcntdq_baseline_import(
    unsigned long long value) noexcept {
  return (value >> 3) ^ (value + 17);
}

int main(int argc, char**) {
  auto probe = static_cast<unsigned long long>(argc);
  if (native_vpopcntdq_baseline_import(probe) != ((probe >> 3) ^ (probe + 17))) return 8;
  if (!check_scalar_reference()) return 1;
  auto cpu = native::observe_x86_capabilities();
  std::printf("AVX512F=%u AVX512VL=%u AVX512VPOPCNTDQ=%u XCR0=0x%llx (observed=%u)\n",
    unsigned(cpu.present.has(native::x86_feature::avx512f)),
    unsigned(cpu.present.has(native::x86_feature::avx512vl)),
    unsigned(cpu.present.has(native::x86_feature::avx512vpopcntdq)),
    static_cast<unsigned long long>(cpu.xcr0), unsigned(cpu.xcr0_observed));
  if (!native::classify_isa(cpu, requirements_512, NATIVE_TARGET_MINIMUM).admitted()) {
    std::puts("AVX512VPOPCNTDQ execution skipped: AVX512F, AVX512VPOPCNTDQ or OS ZMM state unavailable; baseline oracle and feature constraints passed.");
    return 77;
  }
  auto state = std::uint64_t{0x9e3779b97f4a7c15ull} ^ static_cast<unsigned>(argc);
  if (!check_vectors<std::uint32_t, 16>(state)) return 2;
  if (!check_vectors<std::uint64_t, 8>(state)) return 3;
  std::puts("AVX512VPOPCNTDQ 512-bit execution passed: plain, merge and zero forms for 32/64-bit lanes.");
  if (!native::classify_isa(cpu, requirements_vl, NATIVE_TARGET_MINIMUM).admitted()) {
    std::puts("AVX512VPOPCNTDQ 128/256-bit execution skipped: AVX512VL unavailable.");
    return 0;
  }
  if (!check_vectors<std::uint32_t, 4>(state)) return 4;
  if (!check_vectors<std::uint64_t, 2>(state)) return 5;
  if (!check_vectors<std::uint32_t, 8>(state)) return 6;
  if (!check_vectors<std::uint64_t, 4>(state)) return 7;
  std::puts("AVX512VPOPCNTDQ 128/256-bit execution passed: plain, merge and zero forms for 32/64-bit lanes.");
  return 0;
}
