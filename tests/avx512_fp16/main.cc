// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <xmmintrin.h>
// Shared independent rational oracle: use only its gradual-underflow rows.
#include "../neon_fp16/reference_cases.h"
#include "../neon_fp16/sqrt_reference.h"
import native.x86.features;
import native.scalar;
#if !NATIVE_MINIMAL_HAS_AVX512_FP16 && defined(__AVX512FP16__)
#error Optional FP16 flags leaked into the minimum dispatcher
#endif
#if !NATIVE_MINIMAL_HAS_AVX512 && (defined(__AVX512F__) || defined(__AVX512DQ__) || defined(__AVX512BW__) || defined(__AVX512VL__))
#error Optional AVX512 flags leaked into the minimum dispatcher
#endif
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
  struct environment {
    unsigned control=_mm_getcsr();
    ~environment() {_mm_setcsr(control);}
  };
  bool nan(std::uint16_t bits) {return (bits&0x7c00)==0x7c00 && (bits&0x03ff)!=0;}
  bool contract() {
    environment saved;
    std::array<std::uint16_t,32> a,b,c,out[6];
    static_assert(neon_fp16_reference::case_count%32==0);
    // MXCSR order is RNE/RDN/RUP/RTZ; the independent table uses RNE/RUP/RDN/RTZ.
    constexpr unsigned reference_config[4]={0,4,2,6};
    for(unsigned rounding=0;rounding!=4;++rounding) for(unsigned denormal=0;denormal!=4;++denormal) {
      auto control=0x1f80u | (rounding<<13) | ((denormal&1u)<<6) | ((denormal&2u)<<14);
      _mm_setcsr(control);
      for(std::size_t first=0;first!=neon_fp16_reference::case_count;first+=32) {
        for(unsigned i=0;i!=32;++i) {
          auto const &v=neon_fp16_reference::cases[first+i];a[i]=v.a;b[i]=v.b;c[i]=v.c;
        }
        fp16_add(a.data(),b.data(),out[0].data());fp16_sub(a.data(),b.data(),out[1].data());
        fp16_mul(a.data(),b.data(),out[2].data());fp16_fma(a.data(),b.data(),c.data(),out[3].data());
        fp16_div(a.data(),b.data(),out[4].data());fp16_sqrt(a.data(),out[5].data());
        if((_mm_getcsr()&~63u)!=control) {std::puts("Arithmetic changed MXCSR control bits");return false;}
        for(unsigned i=0;i!=32;++i) for(unsigned op=0;op!=6;++op) {
          auto expected=neon_fp16_reference::cases[first+i].expected[reference_config[rounding]][op];
          bool correct=nan(expected) ? nan(out[op][i]) && (out[op][i]&0x0200) : out[op][i]==expected;
          if(!correct) {
            std::printf("case %zu RC %u DAZ/FTZ %u op %u: %04x expected %04x\n",first+i,rounding,denormal,op,out[op][i],expected);
            return false;
          }
        }
      }
      for(unsigned first=0;first!=65536;first+=32) {
        for(unsigned i=0;i!=32;++i) a[i]=std::uint16_t(first+i);
        fp16_sqrt(a.data(),out[0].data());
        for(unsigned i=0;i!=32;++i) {
          auto expected=neon_fp16_reference::sqrt_expected(a[i],reference_config[rounding]);
          bool correct=nan(expected) ? nan(out[0][i]) && (out[0][i]&0x0200) : out[0][i]==expected;
          if(!correct) {
            std::printf("sqrt %04x RC %u DAZ/FTZ %u: %04x expected %04x\n",a[i],rounding,denormal,out[0][i],expected);
            return false;
          }
        }
      }
      if((_mm_getcsr()&~63u)!=control) {std::puts("Square root changed MXCSR control bits");return false;}
    }
    _mm_setcsr(0x1f80);
    a.fill(0x7c01);b.fill(0x3c00);
    fp16_add(a.data(),b.data(),out[0].data());
    if(!(_mm_getcsr()&1)) {std::puts("Signaling NaN did not update MXCSR invalid status");return false;}
    a.fill(0x3c00);b.fill(0);_mm_setcsr(0x1f80);
    fp16_div(a.data(),b.data(),out[0].data());
    if(_mm_getcsr()!=0x1f84) {std::puts("Division by zero did not set only MXCSR divide-by-zero status");return false;}
    a.fill(0);_mm_setcsr(0x1f80);fp16_div(a.data(),b.data(),out[0].data());
    if(_mm_getcsr()!=0x1f81) {std::puts("Zero divided by zero did not set only MXCSR invalid status");return false;}
    a.fill(0xbc00);_mm_setcsr(0x1f80);fp16_sqrt(a.data(),out[0].data());
    if(_mm_getcsr()!=0x1f81) {std::puts("Negative square root did not set only MXCSR invalid status");return false;}
    a.fill(0x4400);_mm_setcsr(0x1f80);fp16_sqrt(a.data(),out[0].data());
    if(_mm_getcsr()!=0x1f80) {std::puts("Exact square root raised an exception");return false;}
    a.fill(0x4000);_mm_setcsr(0x1f80);fp16_sqrt(a.data(),out[0].data());
    if(_mm_getcsr()!=0x1fa0) {std::puts("Inexact square root did not set only MXCSR precision status");return false;}
    a.fill(0x3c00);b.fill(0x4200);_mm_setcsr(0x1f80);fp16_div(a.data(),b.data(),out[0].data());
    if(_mm_getcsr()!=0x1fa0) {std::puts("Inexact division did not set only MXCSR precision status");return false;}
    return true;
  }
  bool masks() {
    environment saved;
    std::array<std::uint16_t,32> a,b,out;
    constexpr std::uint16_t left[8]={0,0x8000,0x3c00,0xbc00,0x7e01,0x7c01,1,0x8001};
    constexpr std::uint16_t right[8]={0x8000,0,0x4000,0xc000,0x3c00,0x7e01,0,0};
    for(unsigned i=0;i!=32;++i) {a[i]=left[i%8];b[i]=right[i%8];}
    for(unsigned denormal=0;denormal!=4;++denormal) {
      auto control=0x1f80u | ((denormal&1u)<<6) | ((denormal&2u)<<14);
      _mm_setcsr(control);
      std::uint64_t comparisons[6];fp16_compare(a.data(),b.data(),comparisons);
      constexpr std::uint64_t eq=0x03030303,lt=0x84848484,gt=0x48484848;
      std::array<std::uint64_t,6> expected{eq,eq^0xffffffff,lt,lt|eq,gt,gt|eq};
      for(unsigned i=0;i!=6;++i) if(comparisons[i]!=expected[i]) {
        std::printf("comparison %u DAZ/FTZ %u: %llx expected %llx\n",i,denormal,
          static_cast<unsigned long long>(comparisons[i]),static_cast<unsigned long long>(expected[i]));return false;
      }
      // Exhaust all 8-bit mask patterns independently in each quarter, plus
      // cross-quarter masks. High bits must be discarded by the 32-lane predicate.
      for(unsigned quarter=0;quarter!=4;++quarter) for(unsigned bits=0;bits!=256;++bits) {
        std::uint64_t mask=(std::uint64_t(bits)<<(quarter*8)) | 0xffffffff00000000ull;
        _mm_setcsr(control);fp16_select(mask,a.data(),b.data(),out.data());
        if(_mm_getcsr()!=control) return false;
        for(unsigned i=0;i!=32;++i) if(out[i]!=((mask>>i)&1?a[i]:b[i])) return false;
      }
      for(auto mask:{0xaaaaaaaaull,0x55555555ull,0xffffffffull}) {
        _mm_setcsr(control);fp16_select(mask,a.data(),b.data(),out.data());
        if(_mm_getcsr()!=control) return false;
        for(unsigned i=0;i!=32;++i) if(out[i]!=((mask>>i)&1?a[i]:b[i])) return false;
      }
      _mm_setcsr(control);fp16_neg(a.data(),out.data());
      if(_mm_getcsr()!=control) return false;
      for(unsigned i=0;i!=32;++i) if(out[i]!=(a[i]^0x8000)) return false;
    }
    return true;
  }
}
int main(int argc,char **argv) {
  if(argc!=2)return 2;
  if(!std::strcmp(argv[1],"none")) {std::puts("No optional FP16 profile entered.");return 0;}
  if(std::strcmp(argv[1],"native"))return 2;
  auto cpu=native::observe_x86_capabilities();
  std::printf("CPUID.7.0.EDX=%08x XCR0=%llx\n",cpu.raw.leaf7_edx,static_cast<unsigned long long>(cpu.xcr0));
  auto admission=native::classify_isa(cpu,native::avx512_fp16);
  if(!admission.admitted()) {std::puts(admission.reason());return 77;}
  if(!fp16_storage()) {std::puts("FP16 storage failure");return 4;}
  if(!contract())return 5;
  if(!masks()) {std::puts("FP16 mask/selection failure");return 6;}
  std::printf("FP16: 65536 storage encodings, guarded tails, %zu arithmetic results, 1048576 exhaustive sqrt results across 16 MXCSR states, 4108 masks\n",neon_fp16_reference::case_count*6*16);
}
