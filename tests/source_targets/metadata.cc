// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <concepts>
#include <cstdio>
#include <type_traits>
#include <native/targets.h>
#ifdef NATIVE_TARGETS_METADATA_ONLY
import native_target_metadata;
#else
import native;
#endif

namespace native {
  template<architecture Family> constexpr isa<Family> source_isa_adl(isa<Family> value) { return value; }
}
static_assert(source_isa_adl(native::avx2)==native::avx2);

template<native::isa<native::x86> A> requires(A==native::target_features<native::x86>(NATIVE_TARGET_STRING(avx2)))
constexpr int repeated();
template<native::isa<native::x86> A> requires(A==native::target_features<native::x86>(NATIVE_TARGET_STRING(avx2)))
constexpr int repeated() { return 7; }
static_assert(repeated<native::avx2>()==7);

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
    native::isa<native::arm> extra_observed=native::detail::arm_features,extra_features=native::detail::arm_features;
  };
  constexpr native::isa<native::x86> plain_avx2=native::x86_feature::avx2;
  constexpr auto combined=native::avx512_fp16&native::x86_feature::avx512bf16;
  static_assert(combined==(native::avx512_bf16&native::x86_feature::avx512fp16));
  static_assert(!plain_avx2.has(native::x86_feature::fma));
  static_assert(!plain_avx2.has(native::x86_feature::bmi1));
  static_assert(!plain_avx2.has(native::x86_feature::bmi2));
  static_assert(!plain_avx2.has(native::x86_feature::f16c));
  static_assert(!native::isa(native::x86_feature::avx512f).has(native::x86_feature::f16c));
  static_assert(native::feature_closure(native::x86_feature::avx512f).has(native::x86_feature::f16c));
  static_assert(native::feature_closure(native::x86_feature::avx512f).has(native::x86_feature::fma));
  static_assert(NATIVE_TARGET_ISA(scalar)==native::scalar);
#if defined(__x86_64__) || defined(_M_X64)
  static_assert(NATIVE_TARGET_ISA(avx2)==native::avx2);
  static_assert(NATIVE_TARGET_ISA(avx512)==native::avx512);
  static_assert(NATIVE_TARGET_ISA(avx512_bf16)==native::avx512_bf16);
  static_assert(NATIVE_TARGET_ISA(avx512_fp16)==native::avx512_fp16);
#elif defined(__aarch64__) || defined(_M_ARM64)
  static_assert(NATIVE_TARGET_ISA(neon)==native::neon);
  static_assert(NATIVE_TARGET_ISA(neon_fp16)==native::neon_fp16);
  static_assert(NATIVE_TARGET_ISA(neon_bf16)==native::neon_bf16);
#endif
  constexpr auto i8mm=native::isa(native::arm_feature::i8mm);
  static_assert(std::uint64_t(native::arm_feature::pauth)==14);
  static_assert(std::uint64_t(native::arm_feature::i8mm)==15);
  static_assert(native::arm_feature_count==22);
  static_assert(native::feature_closure(i8mm)==(native::neon&i8mm));
  static_assert(native::target_features<native::arm>("i8mm")==native::feature_closure(i8mm));
  static_assert([] {
    native::isa<native::arm> features;
    features.i8mm=true;
    if(!features.i8mm || features!=i8mm) return false;
    features.pauth=true;
    features.i8mm=false;
    return !features.i8mm && features==native::isa(native::arm_feature::pauth);
  }());
  static_assert(std::uint64_t(native::arm_feature::pmull)==16);
  static_assert(std::uint64_t(native::arm_feature::sha1)==17);
  static_assert(std::uint64_t(native::arm_feature::sha512)==18);
  static_assert(std::uint64_t(native::arm_feature::ebf16)==19);
  constexpr auto aes_bundle=native::neon&native::arm_feature::aes&native::arm_feature::pmull;
  constexpr auto sha2_bundle=native::neon&native::arm_feature::sha1&native::arm_feature::sha2;
  constexpr auto sha3_bundle=sha2_bundle&native::arm_feature::sha512&native::arm_feature::sha3;
  static_assert(native::target_features<native::arm>("neon,aes")==aes_bundle);
  static_assert(native::target_features<native::arm>("sha2")==sha2_bundle);
  static_assert(native::target_features<native::arm>("sha3")==sha3_bundle);
  static_assert([] {
    constexpr native::arm_feature crypto[]{native::arm_feature::aes,native::arm_feature::pmull,
      native::arm_feature::sha1,native::arm_feature::sha2,native::arm_feature::sha512,native::arm_feature::sha3};
    for(auto feature:crypto) {
      if(native::feature_closure(feature)!=(native::neon&feature)) return false;
      arm_snapshot cpu;
      cpu.extra_features=cpu.extra_observed=native::isa(feature);
      if(!native::classify_isa(cpu,feature).admitted()) return false;
      // One hardware capability never admits a whole compiler crypto bundle.
      if(native::classify_isa(cpu,aes_bundle).admitted() ||
         native::classify_isa(cpu,sha2_bundle).admitted() ||
         native::classify_isa(cpu,sha3_bundle).admitted()) return false;
      cpu.extra_observed={};
      if(native::classify_isa(cpu,feature).admitted()) return false;
    }
    // These names describe hardware, but LLVM has no corresponding target switch.
    constexpr char const * hardware_names[]{"pmull","sha1","sha512","ebf16"};
    for(auto spelling:hardware_names)
      if(native::target_features<native::arm>(spelling)<=native::detail::known_features<native::arm>) return false;
    native::isa<native::arm> properties;
    properties.pmull=true; properties.sha1=true;
    properties.sha512=true; properties.ebf16=true;
    return properties==(native::arm_feature::pmull&native::arm_feature::sha1&
      native::arm_feature::sha512&native::arm_feature::ebf16);
  }());
  static_assert(native::feature_closure(native::arm_feature::ebf16)==
    (native::neon_bf16&native::arm_feature::ebf16));
  static_assert(!native::classify_isa(arm_snapshot{},native::arm_feature::ebf16).admitted());
#ifdef __ARM_FEATURE_AES
  static_assert(NATIVE_TARGET_MINIMUM.has(aes_bundle));
#endif
#ifdef __ARM_FEATURE_SHA2
  static_assert(NATIVE_TARGET_MINIMUM.has(sha2_bundle));
#endif
#ifdef __ARM_FEATURE_SHA3
  static_assert(NATIVE_TARGET_MINIMUM.has(sha3_bundle));
#endif
  static_assert(!(native::target_features<native::x86>("avx2,no-fma")<=native::detail::known_features<native::x86>));
  static_assert(!(native::target_features<native::x86>("default")<=native::detail::known_features<native::x86>));
  static_assert(native::target_features("")==native::scalar);
  static_assert(!(native::target_features<native::x86>("arch=skylake")<=native::detail::known_features<native::x86>));
  static_assert(!(native::target_features<native::x86>("avx2,")<=native::detail::known_features<native::x86>));
  static_assert(native::target_features<native::x86>("avx2,f16c")==native::feature_closure(native::x86_feature::avx2&native::x86_feature::f16c));

  constexpr bool synthetic_x86_features() {
    x86_snapshot cpu;
    if(!native::classify_isa(cpu,combined).admitted()) return false;
    // All recorded CPU requirements and every OS state component are necessary.
    constexpr auto need=combined;
    constexpr std::uint32_t ecx=(1u<<0)|(1u<<9)|(1u<<12)|(1u<<19)|(1u<<20)|(1u<<23)|(1u<<26)|(1u<<27)|(1u<<28)|(1u<<29);
    constexpr std::uint32_t edx=(1u<<23)|(1u<<25)|(1u<<26);
    constexpr std::uint32_t ebx=(1u<<5)|(1u<<16)|(1u<<17)|(1u<<30)|(1u<<31);
    for(unsigned bit=0;bit<32;++bit) {
      cpu={}; cpu.leaf1_ecx&=~(1u<<bit);
      if(native::classify_isa(cpu,need).admitted()==bool(ecx&(1u<<bit))) return false;
      cpu={}; cpu.leaf1_edx&=~(1u<<bit);
      if(native::classify_isa(cpu,need).admitted()==bool(edx&(1u<<bit))) return false;
      cpu={}; cpu.leaf7_ebx&=~(1u<<bit);
      if(native::classify_isa(cpu,need).admitted()==bool(ebx&(1u<<bit))) return false;
      cpu={}; cpu.leaf7_edx&=~(1u<<bit);
      if(native::classify_isa(cpu,need).admitted()==(bit==23)) return false;
      cpu={}; cpu.leaf7_1_eax&=~(1u<<bit);
      if(native::classify_isa(cpu,need).admitted()==(bit==5)) return false;
    }
    return true;
  }
  constexpr bool synthetic_x86_state() {
    x86_snapshot cpu;
    constexpr auto need=combined;
    for(unsigned bit=0;bit<64;++bit) {
      cpu={};cpu.xcr0&=~(1ull<<bit);
      if(native::classify_isa(cpu,need).admitted()==bool(0xe6ull&(1ull<<bit))) return false;
    }
    cpu={};cpu.max_basic_leaf=0;
    if(native::classify_isa(cpu,need).admitted()) return false;
    cpu={};cpu.max_basic_leaf=6;
    if(native::classify_isa(cpu,need).admitted()) return false;
    cpu={};cpu.max_leaf7_subleaf=0;
    if(native::classify_isa(cpu,need).admitted()) return false;
    cpu={};cpu.xcr0_observed=false;
    if(native::classify_isa(cpu,need).admitted()) return false;
    cpu={};cpu.max_extended_leaf=0x80000000u;
    if(native::classify_isa(cpu,native::x86_feature::lzcnt).admitted()) return false;
    cpu={};cpu.extended1_ecx=0;
    if(native::classify_isa(cpu,native::x86_feature::sahf).admitted()) return false;

    return true;
  }
  constexpr bool synthetic_arm() {
    arm_snapshot arm;
    constexpr auto arm_all=native::neon_fp16&native::arm_feature::neon_bf16&native::arm_feature::dotprod;
    if(!native::classify_isa(arm,arm_all).admitted()) return false;
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
      if(native::classify_isa(arm,arm_all).admitted()) return false;
    }
    arm={};arm.extra_observed={};
    if(native::classify_isa(arm,arm_all).admitted()) return false;
    arm={};arm.extra_features={};
    if(native::classify_isa(arm,arm_all).admitted()) return false;

    // I8MM requires the NEON state, but no DotProd, FP16 or BF16 feature.
    arm={};arm.fp16_observed=arm.scalar_fp16=arm.vector_fp16=false;
    arm.bf16_observed=arm.bf16=false;
    arm.extra_observed=arm.extra_features=i8mm;
    if(!native::classify_isa(arm,i8mm).admitted()) return false;
    arm.extra_observed={};
    if(native::classify_isa(arm,i8mm).missing_features!=i8mm) return false;
    arm.extra_observed=i8mm;arm.extra_features={};
    if(native::classify_isa(arm,i8mm).missing_features!=i8mm) return false;
    arm.extra_features=i8mm;arm.baseline_observed=false;
    if(native::classify_isa(arm,i8mm).missing_features!=native::neon) return false;

    return true;
  }
  constexpr bool synthetic_selection() {
    x86_snapshot cpu;
    int calls=0;native::isa<native::x86> selected{};
    auto callback=[&]<native::isa<native::x86> A> { ++calls;selected=A; };
    cpu={};
    if(!native::with_isa(native::isa_list<combined,native::avx2>{},cpu,callback) || calls!=1 || selected!=combined) return false;
    calls=0;cpu.xcr0=6;
    if(!native::with_isa(native::isa_list<combined,native::avx2>{},cpu,callback) || calls!=1 || selected!=native::avx2) return false;
    calls=0;
    if(native::with_isa(native::isa_list<combined>{},cpu,callback) || calls) return false;
    if(native::with_isa(native::isa_list<>{},cpu,callback) || calls) return false;
    // An AVX2-looking variant can inherit a stronger project minimum.
    constexpr auto inherited=native::target_entry{native::avx2,native::avx512};
    if(native::with_isa(native::isa_list<inherited>{},cpu,callback) || calls) return false;
    cpu={};cpu.leaf7_1_eax=0;
    if(native::with_isa(native::isa_list<native::avx2>{},cpu,callback,native::avx512_bf16) || calls) return false;
    return true;
  }
  template<class C,class R> concept compatible = requires(C c,R r) { native::classify_isa(c,r); };
  static_assert(!compatible<x86_snapshot,native::isa<native::arm>>);
  static_assert(!compatible<arm_snapshot,native::isa<native::x86>>);
  // Keep each independent oracle within the default constexpr step budget.
  static_assert(synthetic_x86_features());
  static_assert(synthetic_x86_state());
  static_assert(synthetic_arm());
  static_assert(synthetic_selection());
}
int main() {
  if(!synthetic_x86_features() || !synthetic_x86_state() ||
      !synthetic_arm() || !synthetic_selection()) return 1;
  std::puts("ISA values, CPU/OS admission, inherited minima, ordered selection and no-match passed.");
}
