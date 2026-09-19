// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
import simd.cpu;

// Both feature families are available through the CPU-only umbrella.
template<simd::isa A> struct requirement {};
static_assert(!__is_same(requirement<simd::avx2>, requirement<simd::neon>));

int main() {
#if defined(__x86_64__) || defined(_M_X64)
  auto cpu=simd::observe_x86_capabilities();
#elif defined(__aarch64__) || defined(_M_ARM64)
  auto cpu=simd::observe_arm_capabilities();
#else
  return 0;
#endif
#if defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__) || defined(_M_ARM64)
  int calls=0;
  auto selected=simd::with_isa(simd::isa_list<simd::scalar>{},cpu,
    [&]<simd::isa A> { static_assert(A==simd::scalar); ++calls; });
  return selected && calls==1 ? 0 : 1;
#endif
}
