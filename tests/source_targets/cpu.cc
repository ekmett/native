// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
import native.features;

// Both feature families are available through the CPU-only umbrella.
template<native::isa A> struct requirement {};
static_assert(!__is_same(requirement<native::avx2>, requirement<native::neon>));
static_assert(!__is_same(requirement<native::x86_feature::aes>, requirement<native::arm_feature::aes>));
static_assert(native::isa(native::x86_feature::aes).has(native::x86_feature::aes));
static_assert(native::isa(native::arm_feature::aes).has(native::arm_feature::aes));
static_assert(native::x86_feature_count>0 && native::arm_feature_count>0);

template<native::feature_set<native::x86_feature> A> struct x86_features {};
template<native::feature_set<native::arm_feature> A> struct arm_features {};
static_assert(!__is_same(x86_features<native::x86_feature::aes>,x86_features<native::x86_feature::avx>));
static_assert(!__is_same(arm_features<native::arm_feature::aes>,arm_features<native::arm_feature::neon>));
static_assert([] {
#if defined(__x86_64__) || defined(_M_X64)
  native::x86_capabilities cpu;
  constexpr auto feature=native::x86_feature::aes;
  cpu.xcr0_observed=true;cpu.xcr0=0xe6;
  for(unsigned i=0;i<native::x86_feature_count;++i) {
    auto f=static_cast<native::x86_feature>(i);cpu.present.set(f,true);cpu.observed.set(f,true);
  }
#elif defined(__aarch64__) || defined(_M_ARM64)
  native::arm_capabilities cpu;
  constexpr auto feature=native::arm_feature::aes;
  for(unsigned i=0;i<native::arm_feature_count;++i) {
    auto f=static_cast<native::arm_feature>(i);cpu.present.set(f,true);cpu.observed.set(f,true);
  }
#else
  return true;
#endif
#if defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__) || defined(_M_ARM64)
  // The raw diagnostics remain default; normalized sets are the authority.
  if(!native::classify_isa(cpu,feature).admitted()) return false;
  cpu.observed.set(feature,false);
  return !native::classify_isa(cpu,feature).admitted();
#endif
}());

int main() {
#if defined(__x86_64__) || defined(_M_X64)
  auto cpu=native::observe_x86_capabilities();
#elif defined(__aarch64__) || defined(_M_ARM64)
  auto cpu=native::observe_arm_capabilities();
#else
  return 0;
#endif
#if defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__) || defined(_M_ARM64)
  int calls=0;
  auto selected=native::with_isa(native::isa_list<native::scalar>{},cpu,
    [&]<native::isa A> { static_assert(A==native::scalar); ++calls; });
  return selected && calls==1 ? 0 : 1;
#endif
}
