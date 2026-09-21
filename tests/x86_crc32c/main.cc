// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <native/attributes.h>
#include <native/targets.h>
#if NATIVE_TEST_INTERFACE == 0
#include <native/x86/crc32c.h>
import native.x86.features;
#elif NATIVE_TEST_INTERFACE == 1
import native.x86.crc32c;
#elif NATIVE_TEST_INTERFACE == 2
import native;
#elif NATIVE_TEST_INTERFACE == 3
import native.x86;
#else
#error Select the header, direct module, main hub or x86 umbrella test interface.
#endif
#include "checks.h"

int main(int argc, char**) {
  auto cpu = native::observe_x86_capabilities();
  bool crc32 = cpu.raw.max_basic_leaf >= 1 && (cpu.raw.leaf1_ecx & (1u << 20));
  bool gfni = cpu.raw.max_basic_leaf >= 7 && (cpu.raw.leaf7_ecx & (1u << 8));
  bool avx2 = cpu.raw.max_basic_leaf >= 7 && (cpu.raw.leaf7_ebx & (1u << 5));
  bool avx512f = cpu.raw.max_basic_leaf >= 7 && (cpu.raw.leaf7_ebx & (1u << 16));
  bool avx2_admitted = native::classify_isa(cpu, native::avx2).admitted();
  bool avx512_admitted = native::classify_isa(cpu, native::avx512).admitted();
  auto admission = native::classify_isa(cpu, crc32c_fixture::requirements, NATIVE_TARGET_MINIMUM);
  std::printf("CPUID: CRC32=%u GFNI=%u AVX2=%u AVX512F=%u; "
              "XCR0 observed=%u value=%016llx; admission CRC32=%u AVX2=%u AVX512=%u\n",
              unsigned(crc32), unsigned(gfni), unsigned(avx2), unsigned(avx512f),
              unsigned(cpu.xcr0_observed), static_cast<unsigned long long>(cpu.xcr0),
              unsigned(admission.admitted()), unsigned(avx2_admitted), unsigned(avx512_admitted));
  if (cpu.present.has(native::x86_feature::crc32) != crc32 ||
      cpu.observed.has(native::x86_feature::crc32) != (cpu.raw.max_basic_leaf >= 1)) {
    std::puts("CRC32 CPUID normalization mismatch");
    return 1;
  }
  constexpr std::uint32_t xsave = (1u << 26) | (1u << 27);
  bool may_read_xcr0 = cpu.raw.max_basic_leaf >= 1 && (cpu.raw.leaf1_ecx & xsave) == xsave;
  if (cpu.xcr0_observed != may_read_xcr0) {
    std::puts("XCR0 observation does not match XSAVE/OSXSAVE admission");
    return 2;
  }
  if (!admission.admitted()) {
    std::printf("CRC32C native execution skipped: %s; compile-time and synthetic admission checks passed\n",
                admission.reason());
    return 77;
  }
  std::puts("CRC32C native execution admitted");
  auto state = std::uint64_t{0x9e3779b97f4a7c15ull} ^ static_cast<unsigned>(argc);
  if (!crc32c_fixture::check_width<std::uint8_t>(state)) return 3;
  if (!crc32c_fixture::check_width<std::uint16_t>(state)) return 4;
  if (!crc32c_fixture::check_width<std::uint32_t>(state)) return 5;
#if defined(__x86_64__) || defined(_M_X64)
  if (!crc32c_fixture::check_width<std::uint64_t>(state)) return 6;
#endif
  if (!crc32c_fixture::check_vectors(state)) return 7;
  std::puts("CRC32C native execution passed");
  return 0;
}
