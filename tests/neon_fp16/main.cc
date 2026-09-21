// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "reference_cases.h"
#include "sqrt_reference.h"
#if defined(_MSC_VER)
#include <intrin.h>
#endif
import native.arm.features;
import native.scalar;
// FP16 feature macros can remain set via FP16FML after -fullfp16. The
// baseline_features test compiles native intrinsics with this exact command
// and checks availability against the producer's minimum feature probe.
extern "C" bool fp16_storage();
extern "C" void fp16_add(std::uint16_t const *,std::uint16_t const *,std::uint16_t *) noexcept;
extern "C" void fp16_sub(std::uint16_t const *,std::uint16_t const *,std::uint16_t *) noexcept;
extern "C" void fp16_mul(std::uint16_t const *,std::uint16_t const *,std::uint16_t *) noexcept;
extern "C" void fp16_div(std::uint16_t const *,std::uint16_t const *,std::uint16_t *) noexcept;
extern "C" void fp16_sqrt(std::uint16_t const *,std::uint16_t *) noexcept;
extern "C" void fp16_fma(std::uint16_t const *,std::uint16_t const *,std::uint16_t const *,std::uint16_t *) noexcept;
extern "C" void fp16_neg(std::uint16_t const *,std::uint16_t *) noexcept;
extern "C" void fp16_compare(std::uint16_t const *,std::uint16_t const *,std::uint64_t *) noexcept;
extern "C" void fp16_select(std::uint64_t,std::uint16_t const *,std::uint16_t const *,std::uint16_t *) noexcept;
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
    if(native::classify_isa(empty,native::neon).admitted() ||
        native::classify_isa(empty,native::neon_fp16).admitted()) return false;
    native::arm_capabilities::raw_observations all{true,true,true,true,true,true};
    if(!native::classify_isa(all,native::neon).admitted() ||
        !native::classify_isa(all,native::neon_fp16).admitted()) return false;
    if(!native::classify_isa(all,native::isa<native::arm>(static_cast<native::arm_feature>(-1))).invalid_features) return false;
    auto baseline=all; baseline.fp16_observed=baseline.scalar_fp16=baseline.vector_fp16=false;
    if(!native::classify_isa(baseline,native::neon).admitted() ||
        native::classify_isa(baseline,native::neon_fp16).admitted()) return false;
    for(unsigned missing=0;missing!=6;++missing) {
      auto c=all;
      switch(missing) {
        case 0:c.baseline_observed=false;break;
        case 1:c.fp=false;break;
        case 2:c.asimd=false;break;
        case 3:c.fp16_observed=false;break;
        case 4:c.scalar_fp16=false;break;
        case 5:c.vector_fp16=false;break;
      }
      if(native::classify_isa(c,native::neon_fp16).admitted()) return false;
    }
    return true;
  }
  static_assert(classification());
  bool nan(std::uint16_t bits) {return (bits&0x7c00)==0x7c00 && (bits&0x03ff)!=0;}
  bool contract() {
    environment saved;
    std::array<std::uint16_t,8> a,b,c,out[6];
    static_assert(neon_fp16_reference::case_count%8==0);
    for(unsigned config=0;config!=8;++config) for(unsigned dn=0;dn!=2;++dn) for(unsigned fz=0;fz!=2;++fz) {
      // Qualify standard half mode: AH/AHP/FIZ clear, traps masked. FZ is
      // independent of FZ16 and must not change these native half operations.
      auto control=(std::uint64_t(config/2)<<22) | (std::uint64_t(config%2)<<19) | (std::uint64_t(dn)<<25) | (std::uint64_t(fz)<<24);
      set_fpcr(control);
      if(get_fpcr()!=control) {std::puts("FPCR state unavailable");return false;}
      for(std::size_t first=0;first!=neon_fp16_reference::case_count;first+=8) {
        for(unsigned i=0;i!=8;++i) {
          auto const &v=neon_fp16_reference::cases[first+i];a[i]=v.a;b[i]=v.b;c[i]=v.c;
        }
        fp16_add(a.data(),b.data(),out[0].data());fp16_sub(a.data(),b.data(),out[1].data());
        fp16_mul(a.data(),b.data(),out[2].data());fp16_fma(a.data(),b.data(),c.data(),out[3].data());
        fp16_div(a.data(),b.data(),out[4].data());fp16_sqrt(a.data(),out[5].data());
        if(get_fpcr()!=control) {std::puts("Arithmetic changed FPCR");return false;}
        for(unsigned i=0;i!=8;++i) for(unsigned op=0;op!=6;++op) {
          auto expected=neon_fp16_reference::cases[first+i].expected[config][op];
          bool correct=nan(expected) ? nan(out[op][i]) && (out[op][i]&0x0200) && (!dn || out[op][i]==0x7e00) : out[op][i]==expected;
          if(!correct) {
            std::printf("case %zu config %u DN %u op %u: %04x expected %04x\n",first+i,config,dn,op,out[op][i],expected);
            return false;
          }
        }
      }
      for(unsigned first=0;first!=65536;first+=8) {
        for(unsigned i=0;i!=8;++i) a[i]=std::uint16_t(first+i);
        fp16_sqrt(a.data(),out[0].data());
        for(unsigned i=0;i!=8;++i) {
          auto expected=neon_fp16_reference::sqrt_expected(a[i],config);
          bool correct=nan(expected) ? nan(out[0][i]) && (out[0][i]&0x0200) && (!dn || out[0][i]==0x7e00) : out[0][i]==expected;
          if(!correct) {
            std::printf("sqrt %04x config %u DN %u FZ %u: %04x expected %04x\n",a[i],config,dn,fz,out[0][i],expected);
            return false;
          }
        }
      }
      if(get_fpcr()!=control) {std::puts("Square root changed FPCR");return false;}
    }
    set_fpcr(0);set_fpsr(0);
    a.fill(0x7c01);b.fill(0x3c00);
    fp16_add(a.data(),b.data(),out[0].data());
    if(!(get_fpsr()&1)) {std::puts("Signaling NaN did not update FPSR.IOC");return false;}
    a.fill(0x3c00);b.fill(0);set_fpsr(0);
    fp16_div(a.data(),b.data(),out[0].data());
    if(get_fpsr()!=2) {std::puts("Division by zero did not set only FPSR.DZC");return false;}
    a.fill(0);set_fpsr(0);fp16_div(a.data(),b.data(),out[0].data());
    if(get_fpsr()!=1) {std::puts("Zero divided by zero did not set only FPSR.IOC");return false;}
    a.fill(0xbc00);set_fpsr(0);fp16_sqrt(a.data(),out[0].data());
    if(get_fpsr()!=1) {std::puts("Negative square root did not set only FPSR.IOC");return false;}
    a.fill(0x4400);set_fpsr(0);fp16_sqrt(a.data(),out[0].data());
    if(get_fpsr()!=0) {std::puts("Exact square root raised an exception");return false;}
    a.fill(0x4000);set_fpsr(0);fp16_sqrt(a.data(),out[0].data());
    if(get_fpsr()!=16) {std::puts("Inexact square root did not set only FPSR.IXC");return false;}
    a.fill(0x3c00);b.fill(0x4200);set_fpsr(0);fp16_div(a.data(),b.data(),out[0].data());
    if(get_fpsr()!=16) {std::puts("Inexact division did not set only FPSR.IXC");return false;}
    return true;
  }
  bool masks() {
    environment saved;
    std::array<std::uint16_t,8> a{0,0x8000,0x3c00,0xbc00,0x7e01,0x7c01,1,0x8001};
    std::array<std::uint16_t,8> b{0x8000,0,0x4000,0xc000,0x3c00,0x7e01,0,0};
    std::array<std::uint16_t,8> out;
    for(unsigned fz=0;fz!=2;++fz) {
      set_fpcr(std::uint64_t(fz)<<19);
      std::uint64_t comparisons[6];fp16_compare(a.data(),b.data(),comparisons);
      std::uint64_t eq=fz?0xc3:3,lt=4+(fz?0:128),gt=8+(fz?0:64);
      std::array<std::uint64_t,6> expected{eq,eq^255,lt,lt|eq,gt,gt|eq};
      for(unsigned i=0;i!=6;++i) if(comparisons[i]!=expected[i]) {
        std::printf("comparison %u FZ16 %u: %llx expected %llx\n",i,fz,
          static_cast<unsigned long long>(comparisons[i]),static_cast<unsigned long long>(expected[i]));return false;
      }
      for(unsigned bits=0;bits!=256;++bits) {
        set_fpsr(0); fp16_select(bits,a.data(),b.data(),out.data());
        if(get_fpsr()!=0) return false;
        for(unsigned i=0;i!=8;++i) if(out[i]!=((bits>>i)&1?a[i]:b[i])) return false;
      }
      fp16_neg(a.data(),out.data());
      for(unsigned i=0;i!=8;++i) if(out[i]!=(a[i]^0x8000)) return false;
    }
    return true;
  }
}
int main(int argc,char **argv) {
  if(argc!=2)return 2;
  if(!std::strcmp(argv[1],"none")) {std::puts("No optional FP16 profile entered.");return 0;}
  if(!std::strcmp(argv[1],"admission")) {
    auto cpu=native::observe_arm_capabilities();
    std::puts(native::classify_isa(cpu,native::neon_fp16).reason());
    return classification()?0:3;
  }
  if(std::strcmp(argv[1],"native"))return 2;
  auto admission=native::classify_isa(native::observe_arm_capabilities(),native::neon_fp16);
  if(!admission.admitted()) {std::puts(admission.reason());return 77;}
  if(!fp16_storage()) {std::puts("FP16 storage failure");return 4;}
  if(!contract())return 5;
  if(!masks()) {std::puts("FP16 mask/selection failure");return 6;}
  std::printf("FP16: 65536 storage encodings, guarded tails, %zu arithmetic results, 2097152 exhaustive sqrt results across 32 FPCR states, 512 masks\n",neon_fp16_reference::case_count*6*32);
}
