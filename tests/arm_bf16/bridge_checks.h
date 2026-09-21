// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
namespace bf16_fixture {
  native_target("bf16") bool bridges() {
    environment saved; fpcr(0);
    using B=native::simd<native::bf16,8,native::neon_bf16>;
    using F=native::simd<float,4,native::neon_bf16>;
    halves bits; bits.fill(0x3f80);
    auto a=B::load_bits(bits.data());
    auto c=F(0.f);
    std::array<float,4> output;
    F::from_native(native::bfdot<arch>(c.to_native(),a.to_native(),a.to_native())).store(output.data());
    for(auto value:output) if(value!=2.f) return false;
    F::from_native(native::bfmmla<arch>(c.to_native(),a.to_native(),a.to_native())).store(output.data());
    for(auto value:output) if(value!=4.f) return false;
    F::from_native(native::bfmlalt<arch>(c.to_native(),a.to_native(),a.to_native())).store(output.data());
    for(auto value:output) if(value!=1.f) return false;
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
