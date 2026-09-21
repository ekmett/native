// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "reference_cases.h"
#if defined(_MSC_VER)
#include <intrin.h>
#endif
import native.arm.features;
import native.scalar;
extern "C" bool bf16_storage();
extern "C" void bf16_dot2(std::uint16_t const *,std::uint16_t const *,float const *,float *) noexcept;
namespace {
  std::uint64_t get_fpcr() {
#if defined(_MSC_VER)
    return _ReadStatusReg(ARM64_FPCR);
#else
    std::uint64_t value; __asm__ volatile("mrs %0, fpcr":"=r"(value)); return value;
#endif
  }
  std::uint64_t get_fpsr() {
#if defined(_MSC_VER)
    return _ReadStatusReg(ARM64_FPSR);
#else
    std::uint64_t value; __asm__ volatile("mrs %0, fpsr":"=r"(value)); return value;
#endif
  }
  void set_fpcr(std::uint64_t value) {
#if defined(_MSC_VER)
    _WriteStatusReg(ARM64_FPCR,value);
#else
    __asm__ volatile("msr fpcr, %0"::"r"(value):"memory");
#endif
  }
  void set_fpsr(std::uint64_t value) {
#if defined(_MSC_VER)
    _WriteStatusReg(ARM64_FPSR,value);
#else
    __asm__ volatile("msr fpsr, %0"::"r"(value):"memory");
#endif
  }
  struct environment {
    std::uint64_t control=get_fpcr(),status=get_fpsr();
    ~environment() {set_fpcr(control);set_fpsr(status);}
  };
  constexpr bool classification() {
    native::arm_capabilities::raw_observations empty;
    if(native::classify_isa(empty,native::neon_bf16).admitted()) return false;
    native::arm_capabilities::raw_observations all{true,true,true,true,true,true,true,true,true,true};
    if(!native::classify_isa(all,native::neon_bf16).admitted()) return false;
    if(!native::classify_isa(all,native::isa(static_cast<native::x86_feature>(-1))).invalid_features) return false;
    auto baseline=all; baseline.bf16_observed=baseline.bf16=false;
    if(!native::classify_isa(baseline,native::neon).admitted() ||
        !native::classify_isa(baseline,native::neon_fp16).admitted() ||
        native::classify_isa(baseline,native::neon_bf16).admitted()) return false;
    auto independent=all;
    independent.fp16_observed=independent.scalar_fp16=independent.vector_fp16=false;
    independent.ebf16_observed=independent.ebf16=false;
    if(!native::classify_isa(independent,native::neon_bf16).admitted()) return false;
    for(unsigned missing=0;missing!=5;++missing) {
      auto c=all;
      switch(missing) {
        case 0:c.baseline_observed=false;break;
        case 1:c.fp=false;break;
        case 2:c.asimd=false;break;
        case 3:c.bf16_observed=false;break;
        case 4:c.bf16=false;break;
      }
      auto rejected=native::classify_isa(c,native::neon_bf16);
      if(rejected.admitted()) return false;
      if(missing>=3 && !rejected.missing_features.has(native::arm_feature::neon_bf16)) return false;
    }
    return true;
  }
  static_assert(classification());
  bool contract(bool enhanced) {
    environment saved;
    std::array<std::uint16_t,8> a,b;
    std::array<float,4> accumulator,output;
    static_assert(neon_bf16_reference::case_count%4==0);
    std::size_t checked=0;
    // EBF=0 ignores rounding/FZ/DN. Qualify every combination; EBF=1 follows
    // RMode/FZ. AH/FIZ=0 here; their distinct behavior has focused controls below.
    for(unsigned mode=0;mode!=(enhanced?2u:1u);++mode)
      for(unsigned config=0;config!=8;++config) for(unsigned dn=0;dn!=2;++dn) {
        auto control=(std::uint64_t(mode)<<13) | (std::uint64_t(config/2)<<22) |
          (std::uint64_t(config%2)<<24) | (std::uint64_t(dn)<<25);
        set_fpcr(control);
        if(get_fpcr()!=control) {std::puts("FPCR state unavailable");return false;}
        for(std::size_t first=0;first!=neon_bf16_reference::case_count;first+=4) {
          for(unsigned i=0;i!=4;++i) {
            auto const &v=neon_bf16_reference::cases[first+i];
            a[2*i]=v.a0;a[2*i+1]=v.a1;b[2*i]=v.b0;b[2*i+1]=v.b1;
            accumulator[i]=std::bit_cast<float>(v.accumulator);
          }
          // Each case runs once with clear flags and once with a nontrivial sentinel.
          set_fpsr(dn?0x08000095:0);auto status=get_fpsr();
          bf16_dot2(a.data(),b.data(),accumulator.data(),output.data());
          if(get_fpcr()!=control || get_fpsr()!=status) {std::puts("BFDOT changed FPCR/FPSR");return false;}
          for(unsigned i=0;i!=4;++i) {
            auto const &v=neon_bf16_reference::cases[first+i];
            auto expected=mode?v.enhanced[config]:v.baseline;
            auto actual=std::bit_cast<std::uint32_t>(output[i]);
            if(actual!=expected) {
              std::printf("case %zu EBF %u config %u DN %u: %08x expected %08x\n",
                first+i,mode,config,dn,actual,expected);return false;
            }
            ++checked;
          }
        }
      }
    std::printf("%zu exact BFDOT results; EBF=1 %s\n",checked,enhanced?"qualified":"not advertised (not entered)");
    return true;
  }
  bool alternative_controls(bool enhanced) {
    environment saved;
    std::array<std::uint16_t,8> a{},b{};
    std::array<float,4> acc{},out;
    // FEAT_AFP permits AH/FIZ; a readback establishes the writable FPCR controls.
    set_fpcr(3);
    bool alternative=(get_fpcr()&3)==3;
    unsigned checked=0;
    for(unsigned ah=0;ah!=(alternative?2u:1u);++ah) for(unsigned fiz=0;fiz!=(alternative?2u:1u);++fiz) {
      auto control=std::uint64_t((ah<<1)|fiz);
      set_fpcr(control);set_fpsr(0);
      a.fill(0x7f81);b.fill(0x3f80);
      bf16_dot2(a.data(),b.data(),acc.data(),out.data());
      for(float value:out) if(std::bit_cast<std::uint32_t>(value)!=(ah?0xffc00000u:0x7fc00000u)) return false;
      if(get_fpcr()!=control || get_fpsr()!=0) return false;
      // Baseline always flushes BF16 input subnormals, regardless of AH/FIZ/FZ.
      a.fill(1);b.fill(0x3f80);
      bf16_dot2(a.data(),b.data(),acc.data(),out.data());
      for(float value:out) if(std::bit_cast<std::uint32_t>(value)!=0) return false;
      ++checked;
      if(!enhanced) continue;
      // With EBF=1 and FZ=0, FIZ determines input flushing when AFP is present.
      control|=std::uint64_t(1)<<13;set_fpcr(control);set_fpsr(0);
      bf16_dot2(a.data(),b.data(),acc.data(),out.data());
      for(float value:out) if(std::bit_cast<std::uint32_t>(value)!=(fiz?0u:0x00020000u)) return false;
      if(get_fpcr()!=control || get_fpsr()!=0) return false;
      ++checked;
      // AH=1 makes FZ output-only: tiny inputs can produce a normal result.
      control|=std::uint64_t(1)<<24;set_fpcr(control);
      b.fill(0x7f00);
      bf16_dot2(a.data(),b.data(),acc.data(),out.data());
      for(float value:out) if(std::bit_cast<std::uint32_t>(value)!=((ah && !fiz)?0x3d000000u:0u)) return false;
      if(get_fpcr()!=control || get_fpsr()!=0) return false;
      // min_normal - 2^-151 rounds up to min_normal. AH=0 FZ flushes before
      // rounding; AH=1 checks after rounding and retains this normal result.
      for(unsigned i=0;i!=4;++i) {
        a[2*i]=0x0080;a[2*i+1]=0x8080;b[2*i]=0x3f80;b[2*i+1]=0x3300;
      }
      bf16_dot2(a.data(),b.data(),acc.data(),out.data());
      for(float value:out) if(std::bit_cast<std::uint32_t>(value)!=(ah?0x00800000u:0u)) return false;
      if(get_fpcr()!=control || get_fpsr()!=0) return false;
      checked+=2;
    }
    std::printf("%u AH/FIZ controls; AFP %s\n",checked,alternative?"writable":"not writable (AH/FIZ=0 only)");
    return true;
  }
}
int main(int argc,char **argv) {
  if(argc!=2)return 2;
  if(!std::strcmp(argv[1],"none")) {std::puts("No optional BF16 profile entered.");return 0;}
  auto cpu=native::observe_arm_capabilities();
  auto admission=native::classify_isa(cpu,native::neon_bf16);
  if(!std::strcmp(argv[1],"admission")) {
    std::puts(admission.reason());return classification()?0:3;
  }
  if(std::strcmp(argv[1],"native"))return 2;
  if(!admission.admitted()) {std::puts(admission.reason());return 77;}
  environment saved;set_fpcr(0);set_fpsr(0);
  if(!bf16_storage() || get_fpsr()!=0) {std::puts("BF16 storage failure");return 4;}
  bool enhanced=cpu.raw.ebf16_observed && cpu.raw.ebf16;
  if(!contract(enhanced))return 5;
  if(!alternative_controls(enhanced)) {std::puts("AH/FIZ control failure");return 6;}
  std::puts("BF16: 65536 storage encodings and guarded tails passed");
}
