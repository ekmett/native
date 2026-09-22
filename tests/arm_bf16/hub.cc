// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native;
#include "checks.h"
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
namespace bf16_fixture {
  native_target("bf16") bool bridges() {
    environment saved; fpcr(0);
    using B=native::simd<native::bf16,8,native::neon_bf16>;
    using F=native::simd<float,4,native::neon_bf16>;
    halves bits; bits.fill(0x3f80);
    auto a=B::load_bits(bits.data());
    auto c=F(0.f);
    std::array<float,4> output;
    native::bfdot<arch>(c,a,a).store(output.data());
    for(auto value:output) if(value!=2.f) return false;
    native::bfmmla<arch>(c,a,a).store(output.data());
    for(auto value:output) if(value!=4.f) return false;
    native::bfmlalt<arch>(c,a,a).store(output.data());
    for(auto value:output) if(value!=1.f) return false;
    using B4=native::simd<native::bf16,4,arch>;
    using F2=native::simd<float,2,arch>;
    auto short_input=B4::load_bits(bits.data());
    auto short_result=native::bfdot<arch>(F2(0.f),short_input,short_input);
    auto storage=short_result.to_storage().to_native();
    if(storage[0]!=2.f || storage[1]!=2.f || storage[2]!=0.f || storage[3]!=0.f) return false;
    return true;
  }

  native_target("bf16") bool dot2_controls(bool enhanced) {
    environment saved;
    using B=native::simd<native::bf16,8,native::neon_bf16>;
    using F=native::simd<float,4,native::neon_bf16>;
    halves a_bits, b_bits;
    a_bits.fill(0x7f81); b_bits.fill(0x3f80);
    auto a=B::load_bits(a_bits.data()), b=B::load_bits(b_bits.data());
    auto c=F(0.f);
    fpcr(2);
    bool alternative=(fpcr()&2)!=0;
    if(alternative) {
      fpsr(0x08000095);
      fpcr(0); auto positive=native::dot2(a,b,c);
      fpcr(2); auto negative=native::dot2(a,b,c);
      auto p=std::bit_cast<singles>(positive.to_native());
      auto n=std::bit_cast<singles>(negative.to_native());
      for(unsigned i=0;i<4;++i) if(p[i]!=0x7fc00000 || n[i]!=0xffc00000) {
        std::puts("High-level dot2 reused a result across FPCR.AH change"); return false;
      }
      if(fpcr()!=2 || fpsr()!=0x08000095) return false;
    }
    if(enhanced) {
      a_bits.fill(0x7f7f); b_bits.fill(0x4000);
      for(unsigned i=0;i<4;++i) a_bits[2*i+1]=0xff7f;
      a=B::load_bits(a_bits.data()); b=B::load_bits(b_bits.data());
      fpsr(0x08000095);
      fpcr(0); auto ordinary=native::dot2(a,b,c);
      fpcr(std::uint64_t(1)<<13); auto extended=native::dot2(a,b,c);
      auto o=std::bit_cast<singles>(ordinary.to_native());
      auto e=std::bit_cast<singles>(extended.to_native());
      for(unsigned i=0;i<4;++i) if(o[i]!=0x7fc00000 || e[i]!=0) {
        std::puts("High-level dot2 reused a result across FPCR.EBF change"); return false;
      }
      if(fpcr()!=(std::uint64_t(1)<<13) || fpsr()!=0x08000095) return false;
    }
    std::printf("High-level dot2 control changes: AFP %s, EBF %s\n",
      alternative?"executed":"not writable",enhanced?"executed":"unavailable");
    return true;
  }
}
int main(int argc, char**) {
  auto cpu=native::observe_arm_capabilities();
  auto admitted=native::classify_isa(cpu,bf16_fixture::arch,NATIVE_TARGET_MINIMUM);
  if(!admitted.admitted()) {std::puts(admitted.reason()); return 77;}
  bool enhanced=native::classify_isa(cpu,native::arm_feature::ebf16).admitted();
  if(argc>1 && !enhanced) {std::puts("Enhanced BF16 not observed; FPCR.EBF=1 not entered"); return 77;}
  return bf16_fixture::arithmetic(enhanced) && bf16_fixture::effects(enhanced) &&
    bf16_fixture::bridges() && bf16_fixture::dot2_controls(enhanced)?0:1;
}
