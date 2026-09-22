// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/isa.h>
#include <native/targets.h>
using namespace native;
static_assert(unsigned(x86_feature::adx)==44);
static_assert(x86_feature_count==49);
constexpr bool metadata() {
  constexpr std::array features{x86_feature::sha512,x86_feature::sm3,x86_feature::sm4,x86_feature::avxneconvert};
  constexpr std::array names{"sha512","sm3","sm4","avxneconvert"};
  struct snapshot {
    std::uint32_t max_basic_leaf=7, max_leaf7_subleaf=1;
    std::uint32_t leaf1_ecx=~0u, leaf1_edx=~0u, leaf7_ebx=~0u;
    std::uint32_t leaf7_ecx=0, leaf7_edx=0, leaf7_1_eax=0, leaf7_1_edx=0;
    std::uint64_t xcr0=6;
    bool xcr0_observed=true;
  };
  for(unsigned i=0;i<4;++i) {
    auto requirement=target_features<x86>(names[i]);
    if(!requirement.has(features[i]) || !requirement.has(x86_feature::avx)) return false;
    if(requirement.has(x86_feature::avx2)!=(i!=1)) return false;
    for(unsigned bit=0;bit<32;++bit) {
      snapshot cpu;
      (i==3?cpu.leaf7_1_edx:cpu.leaf7_1_eax)=1u<<bit;
      if(classify_isa(cpu,requirement).admitted()!=(bit==(i==3?5:i))) return false;
    }
    snapshot cpu;
    (i==3?cpu.leaf7_1_edx:cpu.leaf7_1_eax)=1u<<(i==3?5:i);
    cpu.max_leaf7_subleaf=0;
    if(classify_isa(cpu,requirement).admitted()) return false;
    cpu.max_leaf7_subleaf=1; cpu.xcr0=2;
    if(classify_isa(cpu,requirement).admitted()) return false;
  }
  isa<x86> bits{};
  bits.sha512=true; bits.sm3=true; bits.sm4=true; bits.avxneconvert=true;
  return bits== (x86_feature::sha512&x86_feature::sm3&x86_feature::sm4&x86_feature::avxneconvert);
}
static_assert(metadata());
int main() {return !metadata();}
#ifdef NATIVE_CRYPTO_MINIMUM
static_assert(NATIVE_BASELINE.has(x86_feature::sha512));
static_assert(NATIVE_BASELINE.has(x86_feature::sm3));
static_assert(NATIVE_BASELINE.has(x86_feature::sm4));
static_assert(NATIVE_BASELINE.has(x86_feature::avxneconvert));
#endif
