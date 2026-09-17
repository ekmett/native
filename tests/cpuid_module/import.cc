// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <cpuid.h>
#endif
import simd.cpuid;
import simd.wait;
#if defined(__AVX__) || defined(__AVX2__) || defined(__AVX512F__) || defined(__WAITPKG__) || defined(__MWAITX__)
#error "CPUID and generic wait consumers must compile for baseline x86-64"
#endif
namespace {
  auto native(std::uint32_t leaf, std::uint32_t subleaf = 0) {
    std::array<int, 4> r;
#if defined(_MSC_VER)
    __cpuidex(r.data(), static_cast<int>(leaf), static_cast<int>(subleaf));
#else
    __cpuid_count(leaf, subleaf, r[0], r[1], r[2], r[3]);
#endif
    return r;
  }
  bool same(std::uint32_t leaf, std::uint32_t subleaf = 0) {
    for (unsigned attempt = 0; attempt != 100; ++attempt) {
      auto before = native(leaf, subleaf);
      auto r = simd::cpuid(static_cast<std::int32_t>(leaf), static_cast<std::int32_t>(subleaf));
      auto after = native(leaf, subleaf);
      // Retry an observed CPU migration or other varying native observation.
      if (before != after) continue;
      return before == std::array<int, 4>{r.eax, r.ebx, r.ecx, r.edx};
    }
    return false;
  }
}
int main() {
  static_assert(noexcept(simd::cpuid(0, 0)));
  auto basic = native(0);
  auto maximum = static_cast<std::uint32_t>(basic[0]);
  auto extended = static_cast<std::uint32_t>(native(0x80000000u)[0]);
  if (!same(0) || !same(0x80000000u)) return 1;
  if (maximum >= 1 && !same(1)) return 2;
  if (maximum >= 7 && !same(7)) return 3;
  if (extended >= 0x80000001u && !same(0x80000001u)) return 4;
  // Unsupported leaves are passed through, without manufacturing zero registers.
  if (maximum < 0x7fffffffu && !same(maximum + 1)) return 5;
  char vendor[13]{};
  std::memcpy(vendor, &basic[1], 4); std::memcpy(vendor + 4, &basic[3], 4); std::memcpy(vendor + 8, &basic[2], 4);
  auto expected = std::strcmp(vendor, "GenuineIntel") == 0 ? simd::cpu_vendor::intel :
    std::strcmp(vendor, "AuthenticAMD") == 0 ? simd::cpu_vendor::amd : simd::cpu_vendor::unknown;
  if (simd::cpu_vendor != expected) return 6;
  bool mwaitx = extended >= 0x80000001u && (native(0x80000001u)[2] & (1 << 29)) != 0;
  bool waitpkg = maximum >= 7 && (native(7)[2] & (1 << 5)) != 0;
  if (simd::mwaitx::supported != mwaitx || simd::umwait::supported != waitpkg) return 7;
  std::printf("vendor=%s max=%08x extended=%08x mwaitx=%d waitpkg=%d; no wait instructions executed\n", vendor, maximum, extended, mwaitx, waitpkg);
}
