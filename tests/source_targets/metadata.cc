// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <concepts>
#include <cstdio>
#include <type_traits>
#include <simd/targets.h>
#ifdef SIMD_TARGETS_METADATA_ONLY
import simd_target_metadata;
#else
import simd;
#endif

namespace simd {
  constexpr feature_set source_tag_adl(architecture auto tag) { return decltype(tag)::features; }
}
static_assert(source_tag_adl(simd::avx2{})==simd::avx2::features);

namespace {
  struct x86_snapshot {
    std::uint32_t max_basic_leaf=7,leaf1_ecx=~0u,leaf1_edx=~0u,leaf7_ebx=~0u;
    std::uint64_t xcr0=0xe6;
    bool xcr0_observed=true;
    std::uint32_t max_leaf7_subleaf=1,leaf7_1_eax=~0u,leaf7_edx=~0u;
    std::uint32_t max_extended_leaf=0x80000001u,extended1_ecx=~0u;
  };
  struct arm_snapshot {
    bool baseline_observed=true,fp=true,asimd=true;
    bool fp16_observed=true,scalar_fp16=true,vector_fp16=true;
    bool bf16_observed=true,bf16=true;
    std::uint64_t extra_observed=~0ull,extra_features=~0ull;
  };
  using plain_avx2=simd::isa<simd::feature::avx2>;
  using combined=simd::isa<simd::avx512_fp16::features|simd::feature::avx512_bf16>;
  static_assert(std::same_as<combined,simd::isa<simd::avx512_bf16::features|simd::feature::avx512_fp16>>);
  static_assert(!simd::has_feature<plain_avx2,simd::feature::fma>);
  static_assert(!simd::has_feature<plain_avx2,simd::feature::bmi1>);
  static_assert(!simd::has_feature<plain_avx2,simd::feature::bmi2>);
  static_assert(!simd::has_feature<plain_avx2,simd::feature::f16c>);
  static_assert(simd::has_feature<simd::isa<simd::feature::avx512f>,simd::feature::f16c>);
  static_assert(simd::has_feature<simd::isa<simd::feature::avx512f>,simd::feature::fma>);
  static_assert(std::same_as<SIMD_TARGET_TYPE(avx2),simd::avx2>);
  static_assert(std::same_as<SIMD_TARGET_TYPE(scalar),simd::scalar>);
  static_assert(std::same_as<SIMD_TARGET_TYPE(avx512),simd::avx512>);
  static_assert(std::same_as<SIMD_TARGET_TYPE(avx512_bf16),simd::avx512_bf16>);
  static_assert(std::same_as<SIMD_TARGET_TYPE(avx512_fp16),simd::avx512_fp16>);
  static_assert(std::same_as<SIMD_TARGET_TYPE(neon),simd::neon>);
  static_assert(std::same_as<SIMD_TARGET_TYPE(neon_fp16),simd::neon_fp16>);
  static_assert(std::same_as<SIMD_TARGET_TYPE(neon_bf16),simd::neon_bf16>);
  static_assert(simd::target_features("avx2,no-fma")&(1ull<<63));
  static_assert(simd::target_features("arch=skylake")&(1ull<<63));
  static_assert(simd::target_features("avx2,")&(1ull<<63));
  static_assert(simd::target_features("avx2,f16c")==simd::isa<simd::feature::avx2|simd::feature::f16c>::features);

  constexpr bool synthetic() {
    x86_snapshot cpu;
    if(!simd::classify_isa(cpu,combined{}).admitted()) return false;
    if(simd::classify_isa(cpu,simd::neon{}).admitted()) return false;
    // All recorded CPU requirements and every OS state component are necessary.
    constexpr auto need=combined::features;
    constexpr std::uint32_t ecx=(1u<<0)|(1u<<9)|(1u<<12)|(1u<<19)|(1u<<20)|(1u<<23)|(1u<<26)|(1u<<27)|(1u<<28)|(1u<<29);
    constexpr std::uint32_t edx=(1u<<23)|(1u<<25)|(1u<<26);
    constexpr std::uint32_t ebx=(1u<<5)|(1u<<8)|(1u<<16)|(1u<<17)|(1u<<30)|(1u<<31);
    for(unsigned bit=0;bit<32;++bit) {
      cpu={}; cpu.leaf1_ecx&=~(1u<<bit);
      if(simd::classify_isa(cpu,need).admitted()==bool(ecx&(1u<<bit))) return false;
      cpu={}; cpu.leaf1_edx&=~(1u<<bit);
      if(simd::classify_isa(cpu,need).admitted()==bool(edx&(1u<<bit))) return false;
      cpu={}; cpu.leaf7_ebx&=~(1u<<bit);
      if(simd::classify_isa(cpu,need).admitted()==bool(ebx&(1u<<bit))) return false;
      cpu={}; cpu.leaf7_edx&=~(1u<<bit);
      if(simd::classify_isa(cpu,need).admitted()==(bit==23)) return false;
      cpu={}; cpu.leaf7_1_eax&=~(1u<<bit);
      if(simd::classify_isa(cpu,need).admitted()==(bit==5)) return false;
    }
    for(unsigned bit=0;bit<64;++bit) {
      cpu={};cpu.xcr0&=~(1ull<<bit);
      if(simd::classify_isa(cpu,need).admitted()==bool(0xe6ull&(1ull<<bit))) return false;
    }
    cpu={};cpu.max_basic_leaf=0;
    if(simd::classify_isa(cpu,need).admitted()) return false;
    cpu={};cpu.max_basic_leaf=6;
    if(simd::classify_isa(cpu,need).admitted()) return false;
    cpu={};cpu.max_leaf7_subleaf=0;
    if(simd::classify_isa(cpu,need).admitted()) return false;
    cpu={};cpu.xcr0_observed=false;
    if(simd::classify_isa(cpu,need).admitted()) return false;
    cpu={};cpu.max_extended_leaf=0x80000000u;
    if(simd::classify_isa(cpu,simd::isa<simd::feature::lzcnt>{}).admitted()) return false;
    cpu={};cpu.extended1_ecx=0;
    if(simd::classify_isa(cpu,simd::isa<simd::feature::sahf>{}).admitted()) return false;

    arm_snapshot arm;
    using arm_all=simd::isa<simd::neon_fp16::features|simd::feature::neon_bf16|simd::feature::arm_dotprod>;
    if(!simd::classify_isa(arm,arm_all{}).admitted()) return false;
    if(simd::classify_isa(arm,simd::avx2{}).admitted()) return false;
    for(unsigned i=0;i<8;++i) {
      arm={};
      switch(i) {
        case 0:arm.baseline_observed=false;break;
        case 1:arm.fp=false;break;
        case 2:arm.asimd=false;break;
        case 3:arm.fp16_observed=false;break;
        case 4:arm.scalar_fp16=false;break;
        case 5:arm.vector_fp16=false;break;
        case 6:arm.bf16_observed=false;break;
        case 7:arm.bf16=false;break;
      }
      if(simd::classify_isa(arm,arm_all{}).admitted()) return false;
    }
    arm={};arm.extra_observed=0;
    if(simd::classify_isa(arm,arm_all{}).admitted()) return false;
    arm={};arm.extra_features=0;
    if(simd::classify_isa(arm,arm_all{}).admitted()) return false;

    int calls=0;simd::feature_set selected=0;
    auto callback=[&]<class A>(A) { ++calls;selected=A::features; };
    cpu={};
    if(!simd::with_isa(simd::isa_list<combined,simd::avx2>{},cpu,callback) || calls!=1 || selected!=combined::features) return false;
    calls=0;cpu.xcr0=6;
    if(!simd::with_isa(simd::isa_list<combined,simd::avx2>{},cpu,callback) || calls!=1 || selected!=simd::avx2::features) return false;
    calls=0;
    if(simd::with_isa(simd::isa_list<combined>{},cpu,callback) || calls) return false;
    if(simd::with_isa(simd::isa_list<>{},cpu,callback) || calls) return false;
    // An AVX2-looking variant can inherit a stronger project minimum.
    using inherited=simd::target_entry<simd::avx2,simd::avx512::features>;
    if(simd::with_isa(simd::isa_list<inherited>{},cpu,callback) || calls) return false;
    cpu={};cpu.leaf7_1_eax=0;
    if(simd::with_isa(simd::isa_list<simd::avx2>{},cpu,callback,simd::avx512_bf16::features) || calls) return false;
    return true;
  }
  static_assert(synthetic());
}
int main() {
  if(!synthetic()) return 1;
  std::puts("Canonical tags, CPU/OS admission, inherited minima, ordered selection and no-match passed.");
}
