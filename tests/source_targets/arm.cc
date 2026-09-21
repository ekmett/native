// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdio>
#include <string_view>
import native.arm.features;

namespace {
  constexpr native::arm_feature extras[]{
    native::arm_feature::aes, native::arm_feature::sha2, native::arm_feature::sha3,
    native::arm_feature::crc, native::arm_feature::lse, native::arm_feature::rdm,
    native::arm_feature::fp16fml, native::arm_feature::dotprod,
    native::arm_feature::complxnum, native::arm_feature::jsconv,
    native::arm_feature::rcpc, native::arm_feature::pauth
  };
  constexpr auto full=[] {
    native::arm_capabilities::raw_observations cpu;
    cpu.baseline_observed=cpu.fp=cpu.asimd=true;
    cpu.fp16_observed=cpu.scalar_fp16=cpu.vector_fp16=true;
    cpu.bf16_observed=cpu.bf16=true;
    for(auto f:extras) {
      cpu.extra_observed.set(f,true);
      cpu.extra_features.set(f,true);
    }
    return cpu;
  }();
  constexpr auto combined=native::neon_fp16&native::neon_bf16&native::arm_feature::dotprod;

  constexpr bool synthetic() {
    if(!native::classify_isa(full,combined).admitted()) return false;
    // BF16 is independent of FP16 and enhanced BF16; neither implies admission.
    auto cpu=full;
    cpu.fp16_observed=cpu.scalar_fp16=cpu.vector_fp16=false;
    if(!native::classify_isa(cpu,native::neon_bf16).admitted()) return false;
    for(unsigned bit=0;bit!=8;++bit) {
      cpu=full;
      switch(bit) {
        case 0:cpu.baseline_observed=false;break;
        case 1:cpu.fp=false;break;
        case 2:cpu.asimd=false;break;
        case 3:cpu.fp16_observed=false;break;
        case 4:cpu.scalar_fp16=false;break;
        case 5:cpu.vector_fp16=false;break;
        case 6:cpu.bf16_observed=false;break;
        case 7:cpu.bf16=false;break;
      }
      auto result=native::classify_isa(cpu,combined);
      auto missing=bit<3 ? native::arm_feature::neon : bit<6 ? native::arm_feature::neon_fp16 : native::arm_feature::neon_bf16;
      if(result.admitted() || result.missing_features!=native::isa(missing)) return false;
    }
    for(auto f:extras) {
      cpu=full;cpu.extra_observed.set(f,false);
      auto result=native::classify_isa(cpu,native::isa(f));
      if(result.admitted() || result.missing_features!=native::isa(f)) return false;
      cpu=full;cpu.extra_features.set(f,false);
      result=native::classify_isa(cpu,native::isa(f));
      if(result.admitted() || result.missing_features!=native::isa(f)) return false;
    }
    struct old_snapshot {
      bool baseline_observed=true,fp=true,asimd=true;
      bool fp16_observed=true,scalar_fp16=true,vector_fp16=true;
      bool bf16_observed=true,bf16=true;
    };
    if(native::classify_isa(old_snapshot{},native::arm_feature::dotprod).admitted()) return false;
    auto invalid=native::isa(static_cast<native::x86_feature>(-1));
    if(!native::classify_isa(full,invalid).invalid_features) return false;
    invalid={};invalid.flags[0]=1ull<<63;
    if(!native::classify_isa(full,invalid).invalid_features) return false;
    native::isa_admission unknown;
    unknown.missing_features=invalid;
    if(unknown.admitted() || std::string_view(unknown.reason())=="admitted") return false;
    unknown={};unknown.missing_xcr0=1ull<<63;
    if(unknown.admitted() || std::string_view(unknown.reason())=="admitted") return false;
    if(!native::classify_isa(full,native::avx2&native::neon).invalid_features) return false;
    if(!native::classify_isa(native::arm_capabilities{},native::scalar).admitted()) return false;
    if(native::classify_isa(native::arm_capabilities{},native::scalar,native::neon).admitted()) return false;

    int calls=0;native::isa selected{};
    auto callback=[&]<native::isa A> { ++calls;selected=A; };
    cpu=full;
    if(!native::with_isa(native::isa_list<combined,native::neon>{},cpu,callback) || calls!=1 || selected!=combined) return false;
    calls=0;cpu.bf16_observed=false;
    if(!native::with_isa(native::isa_list<combined,native::neon>{},cpu,callback) || calls!=1 || selected!=native::neon) return false;
    calls=0;
    if(native::with_isa(native::isa_list<combined>{},cpu,callback) || calls) return false;
    if(native::with_isa(native::isa_list<>{},cpu,callback) || calls) return false;
    constexpr auto inherited=native::target_entry{native::neon,native::neon_bf16};
    if(native::with_isa(native::isa_list<inherited>{},cpu,callback) || calls) return false;
    if(native::with_isa(native::isa_list<native::neon>{},cpu,callback,native::neon_bf16) || calls) return false;
    cpu=full;
    if(!native::with_isa(native::isa_list<inherited>{},cpu,callback) || calls!=1 || selected!=native::neon) return false;
    calls=0;
    if(!native::with_isa(native::isa_list<native::neon,combined>{},cpu,callback) || calls!=1 || selected!=native::neon) return false;
    cpu=full;cpu.fp16_observed=false;
    if(std::string_view(native::classify_isa(cpu,native::neon_fp16).reason())!="fullfp16") return false;
    return true;
  }
  static_assert(synthetic());
}

int main() {
  if(!synthetic()) return 1;
  auto cpu=native::observe_arm_capabilities();
  // A real failed observation cannot be repaired by stale feature booleans.
  if(!cpu.observed.has(native::arm_feature::neon) && native::classify_isa(cpu,native::neon).admitted()) return 2;
  if(!cpu.observed.has(native::arm_feature::neon_fp16) && native::classify_isa(cpu,native::neon_fp16).admitted()) return 3;
  if(!cpu.observed.has(native::arm_feature::neon_bf16) && native::classify_isa(cpu,native::neon_bf16).admitted()) return 4;
  std::printf("ARM admission: NEON=%s FP16=%s BF16=%s\n",
    native::classify_isa(cpu,native::neon).reason(),
    native::classify_isa(cpu,native::neon_fp16).reason(),
    native::classify_isa(cpu,native::neon_bf16).reason());
}
