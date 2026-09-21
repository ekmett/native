// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"

import native;

namespace {
  constexpr auto requirements = native::target_features<native::x86>("aes");

  struct snapshot {
    std::uint32_t max_basic_leaf = 1;
    std::uint32_t leaf1_ecx = 0;
    std::uint32_t leaf1_edx = 0;
    std::uint32_t leaf7_ebx = 0;
    std::uint32_t max_leaf7_subleaf = 0;
    std::uint32_t leaf7_1_eax = 0;
    std::uint32_t leaf7_edx = 0;
    std::uint64_t xcr0 = 0;
    bool xcr0_observed = false;
  };
}

extern "C" native_noinline unsigned native_aes_baseline(unsigned a, unsigned b) noexcept {
  return (a ^ b) + a;
}

// Keep admission in a baseline translation unit, including its classifier body.
// Legacy AES needs no AVX/XSAVE state; it still needs observed SSE prerequisites.
extern "C" native_noinline bool native_aes_admission(
  std::uint32_t leaf1_ecx, std::uint32_t leaf1_edx, std::uint32_t max_basic_leaf) noexcept {
  snapshot cpu;
  cpu.max_basic_leaf = max_basic_leaf;
  cpu.leaf1_ecx = leaf1_ecx;
  cpu.leaf1_edx = leaf1_edx;
  return native::classify_isa(cpu, requirements).admitted();
}
