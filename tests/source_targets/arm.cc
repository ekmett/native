// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdio>
#include <string_view>
import simd.arm;

namespace {
  constexpr simd::feature extras[]{
    simd::feature::arm_aes, simd::feature::arm_sha2, simd::feature::arm_sha3,
    simd::feature::arm_crc, simd::feature::arm_lse, simd::feature::arm_rdm,
    simd::feature::arm_fp16fml, simd::feature::arm_dotprod,
    simd::feature::arm_complxnum, simd::feature::arm_jsconv,
    simd::feature::arm_rcpc, simd::feature::arm_pauth
  };
  constexpr auto full=[] {
    simd::arm_capabilities cpu;
    cpu.baseline_observed=cpu.fp=cpu.asimd=true;
    cpu.fp16_observed=cpu.scalar_fp16=cpu.vector_fp16=true;
    cpu.bf16_observed=cpu.bf16=true;
    for(auto f:extras) {
      cpu.extra_observed.set(f,true);
      cpu.extra_features.set(f,true);
    }
    return cpu;
  }();
  constexpr auto combined=simd::neon_fp16&simd::neon_bf16&simd::feature::arm_dotprod;

  constexpr bool synthetic() {
    if(!simd::classify_isa(full,combined).admitted()) return false;
    // BF16 is independent of FP16 and enhanced BF16; neither implies admission.
    auto cpu=full;
    cpu.fp16_observed=cpu.scalar_fp16=cpu.vector_fp16=false;
    if(!simd::classify_isa(cpu,simd::neon_bf16).admitted()) return false;
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
      auto result=simd::classify_isa(cpu,combined);
      auto missing=bit<3 ? simd::feature::neon : bit<6 ? simd::feature::neon_fp16 : simd::feature::neon_bf16;
      if(result.admitted() || result.missing_features!=simd::isa(missing)) return false;
    }
    for(auto f:extras) {
      cpu=full;cpu.extra_observed.set(f,false);
      auto result=simd::classify_isa(cpu,simd::isa(f));
      if(result.admitted() || result.missing_features!=simd::isa(f)) return false;
      cpu=full;cpu.extra_features.set(f,false);
      result=simd::classify_isa(cpu,simd::isa(f));
      if(result.admitted() || result.missing_features!=simd::isa(f)) return false;
    }
    struct old_snapshot {
      bool baseline_observed=true,fp=true,asimd=true;
      bool fp16_observed=true,scalar_fp16=true,vector_fp16=true;
      bool bf16_observed=true,bf16=true;
    };
    if(simd::classify_isa(old_snapshot{},simd::feature::arm_dotprod).admitted()) return false;
    auto invalid=simd::isa(simd::feature::invalid_features);
    if(!simd::classify_isa(full,invalid).invalid_features) return false;
    invalid={};invalid.flags[0]=1ull<<63;
    if(!simd::classify_isa(full,invalid).invalid_features) return false;
    simd::isa_admission unknown;
    unknown.missing_features=invalid;
    if(unknown.admitted() || std::string_view(unknown.reason())=="admitted") return false;
    unknown={};unknown.missing_xcr0=1ull<<63;
    if(unknown.admitted() || std::string_view(unknown.reason())=="admitted") return false;
    if(!simd::classify_isa(full,simd::avx2&simd::neon).invalid_features) return false;
    if(!simd::classify_isa(simd::arm_capabilities{},simd::scalar).admitted()) return false;
    if(simd::classify_isa(simd::arm_capabilities{},simd::scalar,simd::neon).admitted()) return false;

    int calls=0;simd::isa selected{};
    auto callback=[&]<simd::isa A> { ++calls;selected=A; };
    cpu=full;
    if(!simd::with_isa(simd::isa_list<combined,simd::neon>{},cpu,callback) || calls!=1 || selected!=combined) return false;
    calls=0;cpu.bf16_observed=false;
    if(!simd::with_isa(simd::isa_list<combined,simd::neon>{},cpu,callback) || calls!=1 || selected!=simd::neon) return false;
    calls=0;
    if(simd::with_isa(simd::isa_list<combined>{},cpu,callback) || calls) return false;
    if(simd::with_isa(simd::isa_list<>{},cpu,callback) || calls) return false;
    constexpr auto inherited=simd::target_entry{simd::neon,simd::neon_bf16};
    if(simd::with_isa(simd::isa_list<inherited>{},cpu,callback) || calls) return false;
    if(simd::with_isa(simd::isa_list<simd::neon>{},cpu,callback,simd::neon_bf16) || calls) return false;
    cpu=full;
    if(!simd::with_isa(simd::isa_list<inherited>{},cpu,callback) || calls!=1 || selected!=simd::neon) return false;
    calls=0;
    if(!simd::with_isa(simd::isa_list<simd::neon,combined>{},cpu,callback) || calls!=1 || selected!=simd::neon) return false;
    cpu=full;cpu.fp16_observed=false;
    if(std::string_view(simd::classify_isa(cpu,simd::neon_fp16).reason())!="fullfp16") return false;
    return true;
  }
  static_assert(synthetic());
}

int main() {
  if(!synthetic()) return 1;
  auto cpu=simd::observe_arm_capabilities();
  // A real failed observation cannot be repaired by stale feature booleans.
  if(!cpu.baseline_observed && simd::classify_isa(cpu,simd::neon).admitted()) return 2;
  if(!cpu.fp16_observed && simd::classify_isa(cpu,simd::neon_fp16).admitted()) return 3;
  if(!cpu.bf16_observed && simd::classify_isa(cpu,simd::neon_bf16).admitted()) return 4;
  std::printf("ARM admission: NEON=%s FP16=%s BF16=%s\n",
    simd::classify_isa(cpu,simd::neon).reason(),
    simd::classify_isa(cpu,simd::neon_fp16).reason(),
    simd::classify_isa(cpu,simd::neon_bf16).reason());
}
