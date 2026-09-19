// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
import simd.cpu;

// Both feature families are available through the CPU-only umbrella.
template<simd::isa A> struct requirement {};
static_assert(!__is_same(requirement<simd::avx2>, requirement<simd::neon>));
static_assert(!__is_same(requirement<simd::x86_feature::aes>, requirement<simd::arm_feature::aes>));
static_assert(simd::isa(simd::x86_feature::aes).has(simd::x86_feature::aes));
static_assert(simd::isa(simd::arm_feature::aes).has(simd::arm_feature::aes));
static_assert(simd::x86_feature_count>0 && simd::arm_feature_count>0);

template<simd::feature_set<simd::x86_feature> A> struct x86_features {};
template<simd::feature_set<simd::arm_feature> A> struct arm_features {};
static_assert(!__is_same(x86_features<simd::x86_feature::aes>,x86_features<simd::x86_feature::avx>));
static_assert(!__is_same(arm_features<simd::arm_feature::aes>,arm_features<simd::arm_feature::neon>));
static_assert([] {
#if defined(__x86_64__) || defined(_M_X64)
  simd::x86_capabilities cpu;
  constexpr auto feature=simd::x86_feature::aes;
  cpu.xcr0_observed=true;cpu.xcr0=0xe6;
  for(unsigned i=0;i<simd::x86_feature_count;++i) {
    auto f=static_cast<simd::x86_feature>(i);cpu.present.set(f,true);cpu.observed.set(f,true);
  }
#elif defined(__aarch64__) || defined(_M_ARM64)
  simd::arm_capabilities cpu;
  constexpr auto feature=simd::arm_feature::aes;
  for(unsigned i=0;i<simd::arm_feature_count;++i) {
    auto f=static_cast<simd::arm_feature>(i);cpu.present.set(f,true);cpu.observed.set(f,true);
  }
#else
  return true;
#endif
#if defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__) || defined(_M_ARM64)
  // The raw diagnostics remain default; normalized sets are the authority.
  if(!simd::classify_isa(cpu,feature).admitted()) return false;
  cpu.observed.set(feature,false);
  return !simd::classify_isa(cpu,feature).admitted();
#endif
}());

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
