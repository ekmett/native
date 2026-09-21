// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "simd_bridge.h"
#include "reference_cases.h"
#include "../neon_bf16/reference_cases.h"
namespace bf16_fixture {
  constexpr auto arch = native::feature_closure(native::isa<native::arm>{native::arm_feature::neon_bf16});
  template<native::isa<native::arm> A, class R, class V, class W>
  concept dot_valid = requires(R r, V v, W w) { native::bfdot<A>(r,v,w); };
  template<native::isa<native::arm> A, unsigned Lane, class R, class V, class W>
  concept dot_lane_valid = requires(R r, V v, W w) { native::bfdot_lane<A,Lane>(r,v,w); };
  template<native::isa<native::arm> A, unsigned Lane, class V>
  concept fma_lane_valid = requires(native::simd<float, 4, arch> r, native::simd<native::bf16, 8, arch> v, V w) { native::bfmlalb_lane<A,Lane>(r,v,w); native::bfmlalt_lane<A,Lane>(r,v,w); };
  static_assert(dot_valid<arch,native::simd<float, 2, arch>,native::simd<native::bf16, 4, arch>,native::simd<native::bf16, 4, arch>>);
  static_assert(dot_valid<arch,native::simd<float, 4, arch>,native::simd<native::bf16, 8, arch>,native::simd<native::bf16, 8, arch>>);
  static_assert(dot_valid<native::neon,native::simd<float, 4, native::neon>,native::simd<native::bf16, 8, native::neon>,native::simd<native::bf16, 8, native::neon>>);
  static_assert(!dot_valid<arch,native::simd<float, 4, arch>,native::simd<native::fp16, 8, arch>,native::simd<native::fp16, 8, arch>>);
  static_assert(!dot_valid<arch,native::simd<std::int32_t, 4, arch>,native::simd<native::bf16, 8, arch>,native::simd<native::bf16, 8, arch>>);
  static_assert(!dot_lane_valid<arch,2,native::simd<float, 2, arch>,native::simd<native::bf16, 4, arch>,native::simd<native::bf16, 4, arch>>);
  static_assert(!dot_lane_valid<arch,4,native::simd<float, 4, arch>,native::simd<native::bf16, 8, arch>,native::simd<native::bf16, 8, arch>>);
  static_assert(!fma_lane_valid<arch,4,native::simd<native::bf16, 4, arch>>);
  static_assert(!fma_lane_valid<arch,8,native::simd<native::bf16, 8, arch>>);
  static_assert(!fma_lane_valid<arch,0,native::simd<native::fp16, 8, arch>>);

  std::uint64_t fpcr() {std::uint64_t x; asm volatile("mrs %0, fpcr" : "=r"(x) :: "memory"); return x;}
  std::uint64_t fpsr() {std::uint64_t x; asm volatile("mrs %0, fpsr" : "=r"(x) :: "memory"); return x;}
  void fpcr(std::uint64_t x) {asm volatile("msr fpcr, %0" :: "r"(x) : "memory");}
  void fpsr(std::uint64_t x) {asm volatile("msr fpsr, %0" :: "r"(x) : "memory");}
  struct environment {
    std::uint64_t control=fpcr(), status=fpsr();
    ~environment() {fpcr(control); fpsr(status);}
  };
  using halves = std::array<std::uint16_t,8>;
  using singles = std::array<std::uint32_t,4>;

  template<unsigned N, unsigned M=0, unsigned Lane=0>
  native_inline native_target("bf16") singles dot(singles c, halves a, halves b) {
    singles result{};
    if constexpr(N==2) {
      auto va=vreinterpret_bf16_u16(vld1_u16(a.data()));
      auto vc=vreinterpret_f32_u32(vld1_u32(c.data()));
      if constexpr(M==0) vst1_u32(result.data(),vreinterpret_u32_f32(instruction_fixture::bfdot<arch>(vc,va,vreinterpret_bf16_u16(vld1_u16(b.data())))));
      else if constexpr(M==4) vst1_u32(result.data(),vreinterpret_u32_f32(instruction_fixture::bfdot_lane<arch,Lane>(vc,va,vreinterpret_bf16_u16(vld1_u16(b.data())))));
      else vst1_u32(result.data(),vreinterpret_u32_f32(instruction_fixture::bfdot_lane<arch,Lane>(vc,va,vreinterpretq_bf16_u16(vld1q_u16(b.data())))));
    } else {
      auto va=vreinterpretq_bf16_u16(vld1q_u16(a.data()));
      auto vc=vreinterpretq_f32_u32(vld1q_u32(c.data()));
      if constexpr(M==0) vst1q_u32(result.data(),vreinterpretq_u32_f32(instruction_fixture::bfdot<arch>(vc,va,vreinterpretq_bf16_u16(vld1q_u16(b.data())))));
      else if constexpr(M==4) vst1q_u32(result.data(),vreinterpretq_u32_f32(instruction_fixture::bfdot_lane<arch,Lane>(vc,va,vreinterpret_bf16_u16(vld1_u16(b.data())))));
      else vst1q_u32(result.data(),vreinterpretq_u32_f32(instruction_fixture::bfdot_lane<arch,Lane>(vc,va,vreinterpretq_bf16_u16(vld1q_u16(b.data())))));
    }
    return result;
  }

  template<bool Top, unsigned M=0, unsigned Lane=0>
  native_inline native_target("bf16") singles fused(singles c, halves a, halves b) {
    auto va=vreinterpretq_bf16_u16(vld1q_u16(a.data()));
    auto vc=vreinterpretq_f32_u32(vld1q_u32(c.data()));
    auto vb=[&] {if constexpr(M==4) return vreinterpret_bf16_u16(vld1_u16(b.data())); else return vreinterpretq_bf16_u16(vld1q_u16(b.data()));}();
    float32x4_t value;
    if constexpr(M==0 && Top) value=instruction_fixture::bfmlalt<arch>(vc,va,vb);
    else if constexpr(M==0) value=instruction_fixture::bfmlalb<arch>(vc,va,vb);
    else if constexpr(Top) value=instruction_fixture::bfmlalt_lane<arch,Lane>(vc,va,vb);
    else value=instruction_fixture::bfmlalb_lane<arch,Lane>(vc,va,vb);
    singles result; vst1q_u32(result.data(),vreinterpretq_u32_f32(value)); return result;
  }

  native_inline native_target("bf16") singles matrix(singles c, halves a, halves b) {
    auto value=instruction_fixture::bfmmla<arch>(vreinterpretq_f32_u32(vld1q_u32(c.data())),
      vreinterpretq_bf16_u16(vld1q_u16(a.data())),vreinterpretq_bf16_u16(vld1q_u16(b.data())));
    singles result; vst1q_u32(result.data(),vreinterpretq_u32_f32(value)); return result;
  }

  template<unsigned N, unsigned M=0, unsigned Lane=0>
  native_target("bf16") bool check_dot(bool enhanced, unsigned config, bool dn) {
    std::uint64_t status=dn?0x08000095:0;
    auto control=fpcr();
    for(auto const & sample:neon_bf16_reference::cases) {
      halves a{},b{}; singles c{}; b.fill(0x7f81); c.fill(sample.accumulator);
      for(unsigned i=0;i<N;++i) {
        a[2*i]=sample.a0; a[2*i+1]=sample.a1;
        if constexpr(M==0) {b[2*i]=sample.b0; b[2*i+1]=sample.b1;}
      }
      if constexpr(M!=0) {b[2*Lane]=sample.b0; b[2*Lane+1]=sample.b1;}
      fpsr(status);
      auto result=dot<N,M,Lane>(c,a,b);
      if(fpcr()!=control || fpsr()!=status) {std::puts("BFDOT modified FPCR/FPSR"); return false;}
      auto expected=enhanced?sample.enhanced[config]:sample.baseline;
      for(unsigned i=0;i<N;++i) if(result[i]!=expected) {
        std::printf("BFDOT shape %u/%u lane %u EBF %u config %u: %08x != %08x\n",N,M,Lane,enhanced,config,result[i],expected); return false;
      }
    }
    return true;
  }

  template<bool Top, unsigned M=0, unsigned Lane=0>
  native_target("bf16") bool check_fused(unsigned config) {
    auto control=fpcr();
    for(auto const & sample:arm_bf16_reference::fmas) {
      halves a{},b{}; singles c{}; a.fill(0x7f81); b.fill(0x7f81); c.fill(sample.acc);
      for(unsigned i=0;i<4;++i) {a[2*i+Top]=sample.a; if constexpr(M==0) b[2*i+Top]=sample.b;}
      if constexpr(M!=0) b[Lane]=sample.b;
      auto result=fused<Top,M,Lane>(c,a,b);
      if(fpcr()!=control) {std::puts("BFMLAL modified FPCR"); return false;}
      for(auto value:result) if(value!=sample.result[config]) {
        std::printf("BFMLAL%s source %u lane %u config %u: %08x != %08x\n",Top?"T":"B",M,Lane,config,value,sample.result[config]); return false;
      }
    }
    return true;
  }

  native_target("bf16") bool check_matrix(bool enhanced,unsigned config,bool dn) {
    auto control=fpcr();
    std::uint64_t status=dn?0x08000095:0;
    for(auto const & sample:arm_bf16_reference::matrices) {
      auto a=std::bit_cast<halves>(sample.a), b=std::bit_cast<halves>(sample.b);
      auto c=std::bit_cast<singles>(sample.acc);
      fpsr(status);
      auto result=matrix(c,a,b);
      if(fpcr()!=control || fpsr()!=status) {std::puts("BFMMLA modified FPCR/FPSR"); return false;}
      for(unsigned i=0;i<4;++i) if(result[i]!=sample.result[enhanced?config+1:0][i]) {
        std::printf("BFMMLA EBF %u config %u lane %u: %08x != %08x\n",enhanced,config,i,result[i],sample.result[enhanced?config+1:0][i]); return false;
      }
    }
    return true;
  }


  template<bool Top, unsigned M=0, unsigned Lane=0>
  native_target("bf16") bool fused_effects() {
    struct example {std::uint16_t a,b; std::uint32_t c,result,flags; bool flush,dn;};
    constexpr example cases[]{
      {0,0x7f80,0,0x7fc00000,1,false,true},
      {0x7f81,0x3f80,0,0x7fc00000,1,false,true},
      {0x7f7f,0x4000,0,0x7f800000,0x14,false,true},
      {0x0080,0x3f00,0,0x00400000,0,false,true},
      {0x0080,0x3f00,0,0,8,true,true},
      {0x3f80,0x3380,0x3f800000,0x3f800000,0x10,false,true},
      {1,0x3f80,0,0x00010000,0,false,true},
      {1,0x3f80,0,0,0x80,true,true},
      {0x7fc5,0x3f80,0,0x7fc50000,0,false,false},
      {0x3f80,0x3f80,0x7f800003,0x7fc00003,1,false,false}
    };
    for(auto const & v:cases) {
      auto control=(std::uint64_t(v.flush)<<24)|(std::uint64_t(v.dn)<<25);
      fpcr(control);
      halves a{},b{}; singles c; c.fill(v.c);
      for(unsigned i=0;i<4;++i) {a[2*i+Top]=v.a; if constexpr(M==0) b[2*i+Top]=v.b;}
      if constexpr(M!=0) b[Lane]=v.b;
      for(unsigned discard=0;discard<2;++discard) {
        constexpr std::uint64_t prior=0x08000002;
        fpsr(prior);
        if(discard) (void)fused<Top,M,Lane>(c,a,b);
        else {
          auto result=fused<Top,M,Lane>(c,a,b);
          for(auto value:result) if(value!=v.result) {
            std::printf("BFMLAL effect value: %08x != %08x\n",value,v.result); return false;
          }
        }
        if(fpcr()!=control || fpsr()!=(prior|v.flags)) {
          std::printf("BFMLAL%s source %u lane %u discarded %u: FPSR %08llx != %08llx\n",
            Top?"T":"B",M,Lane,discard,static_cast<unsigned long long>(fpsr()),
            static_cast<unsigned long long>(prior|v.flags)); return false;
        }
      }
    }
    return true;
  }

  native_target("bf16") bool environment_controls(bool enhanced) {
    environment saved;
    halves a{},b{}; singles c{};
    a.fill(0x3f80); b.fill(0x3380); c.fill(0x3f800000);
    fpcr(0);
    auto nearest=fused<false>(c,a,b);
    fpcr(std::uint64_t(1)<<22);
    auto upward=fused<false>(c,a,b);
    if(nearest[0]!=0x3f800000 || upward[0]!=0x3f800001) {
      std::puts("BFMLAL calls merged across FPCR change"); return false;
    }
    // AFP controls are entered only if they read back as writable.
    fpcr(3);
    bool alternative=(fpcr()&3)==3;
    if(alternative) {
      a.fill(0x7f81); b.fill(0x3f80); c.fill(0);
      fpcr(0); auto positive=dot<4>(c,a,b); auto positive_matrix=matrix(c,a,b);
      fpcr(2); auto negative=dot<4>(c,a,b); auto negative_matrix=matrix(c,a,b);
      if(positive[0]!=0x7fc00000 || negative[0]!=0xffc00000 ||
          positive_matrix[0]!=0x7fc00000 || negative_matrix[0]!=0xffc00000) {
        std::puts("BF16 dot/matrix calls merged across FPCR.AH change"); return false;
      }
      for(unsigned mode=0;mode<4;++mode) for(unsigned fiz=0;fiz<2;++fiz) {
        auto control=(std::uint64_t(mode)<<22)|2|fiz;
        fpcr(control); fpsr(0x08000002);
        // AH=1 forces BFMLAL's RNE rounding and suppresses its inexact flag.
        a.fill(0x3f80); b.fill(0x3380); c.fill(0x3f800000);
        if(fused<false>(c,a,b)[0]!=0x3f800000 || fused<true,8,7>(c,a,b)[0]!=0x3f800000) return false;
        // FIZ=0/FZ=0 still flush BFMLAL input and output subnormals under AH=1.
        a.fill(1); b.fill(0x3f80); c.fill(0);
        if(fused<true>(c,a,b)[0]!=0 || fused<false,4,3>(c,a,b)[0]!=0) return false;
        a.fill(0x80); b.fill(0x3f00);
        if(fused<false>(c,a,b)[0]!=0) return false;
        // BFMLAL invalid-operation flags are also suppressed under AH=1.
        a.fill(0); b.fill(0x7f80);
        (void)fused<true>(c,a,b);
        if(fpcr()!=control || fpsr()!=0x08000002) {std::puts("AH=1 BFMLAL effects"); return false;}
      }
    }
    // Exercise the EBF change itself only when observed hardware supports it.
    if(enhanced) {
      a.fill(0x7f7f); b.fill(0x4000); c.fill(0);
      for(unsigned i=0;i<4;++i) a[2*i+1]=0xff7f;
      fpcr(0); auto ordinary=dot<4>(c,a,b); auto ordinary_matrix=matrix(c,a,b);
      fpcr(std::uint64_t(1)<<13); auto extended=dot<4>(c,a,b); auto extended_matrix=matrix(c,a,b);
      if(ordinary[0]!=0x7fc00000 || ordinary_matrix[0]!=0x7fc00000 ||
          extended[0]!=0 || extended_matrix[0]!=0) return false;
    }
    // Discarded dot/matrix results must leave cumulative status alone.
    a.fill(0x7f81); b.fill(0x3f80); c.fill(0); fpcr(0); fpsr(0x08000095);
    (void)dot<4>(c,a,b); (void)matrix(c,a,b);
    if(fpcr()!=0 || fpsr()!=0x08000095) return false;
    std::printf("FPCR changes, discarded results, status effects; AFP %s\n",alternative?"executed":"not writable (checks skipped)");
    return true;
  }

  native_target("bf16") bool effects(bool enhanced) {
    environment saved;
    if(!fused_effects<false>()) return false;
    if(!fused_effects<false,4,0>()) return false;
    if(!fused_effects<false,4,1>()) return false;
    if(!fused_effects<false,4,2>()) return false;
    if(!fused_effects<false,4,3>()) return false;
    if(!fused_effects<false,8,0>()) return false;
    if(!fused_effects<false,8,1>()) return false;
    if(!fused_effects<false,8,2>()) return false;
    if(!fused_effects<false,8,3>()) return false;
    if(!fused_effects<false,8,4>()) return false;
    if(!fused_effects<false,8,5>()) return false;
    if(!fused_effects<false,8,6>()) return false;
    if(!fused_effects<false,8,7>()) return false;
    if(!fused_effects<true>()) return false;
    if(!fused_effects<true,4,0>()) return false;
    if(!fused_effects<true,4,1>()) return false;
    if(!fused_effects<true,4,2>()) return false;
    if(!fused_effects<true,4,3>()) return false;
    if(!fused_effects<true,8,0>()) return false;
    if(!fused_effects<true,8,1>()) return false;
    if(!fused_effects<true,8,2>()) return false;
    if(!fused_effects<true,8,3>()) return false;
    if(!fused_effects<true,8,4>()) return false;
    if(!fused_effects<true,8,5>()) return false;
    if(!fused_effects<true,8,6>()) return false;
    if(!fused_effects<true,8,7>()) return false;
    return environment_controls(enhanced);
  }

  native_target("bf16") bool arithmetic(bool enhanced) {
    environment saved;
    for(unsigned ebf=0;ebf<(enhanced?2u:1u);++ebf)
      for(unsigned config=0;config<8;++config) for(unsigned dn=0;dn<2;++dn) {
        auto control=(std::uint64_t(ebf)<<13)|(std::uint64_t(config/2)<<22)|
          (std::uint64_t(config%2)<<24)|(std::uint64_t(dn)<<25);
        fpcr(control);
        if(fpcr()!=control) {std::puts("Requested FPCR state unavailable"); return false;}
        if(!check_dot<2>(ebf,config,dn)) return false;
        if(!check_dot<2,4,0>(ebf,config,dn)) return false;
        if(!check_dot<2,4,1>(ebf,config,dn)) return false;
        if(!check_dot<2,8,0>(ebf,config,dn)) return false;
        if(!check_dot<2,8,1>(ebf,config,dn)) return false;
        if(!check_dot<2,8,2>(ebf,config,dn)) return false;
        if(!check_dot<2,8,3>(ebf,config,dn)) return false;
        if(!check_dot<4>(ebf,config,dn)) return false;
        if(!check_dot<4,4,0>(ebf,config,dn)) return false;
        if(!check_dot<4,4,1>(ebf,config,dn)) return false;
        if(!check_dot<4,8,0>(ebf,config,dn)) return false;
        if(!check_dot<4,8,1>(ebf,config,dn)) return false;
        if(!check_dot<4,8,2>(ebf,config,dn)) return false;
        if(!check_dot<4,8,3>(ebf,config,dn)) return false;
        if(!check_matrix(ebf,config,dn)) return false;
        // Fused reference uses DN=1; EBF must not affect these instructions.
        if(dn) {
          if(!check_fused<false>(config)) return false;
          if(!check_fused<false,4,0>(config)) return false;
          if(!check_fused<false,4,1>(config)) return false;
          if(!check_fused<false,4,2>(config)) return false;
          if(!check_fused<false,4,3>(config)) return false;
          if(!check_fused<false,8,0>(config)) return false;
          if(!check_fused<false,8,1>(config)) return false;
          if(!check_fused<false,8,2>(config)) return false;
          if(!check_fused<false,8,3>(config)) return false;
          if(!check_fused<false,8,4>(config)) return false;
          if(!check_fused<false,8,5>(config)) return false;
          if(!check_fused<false,8,6>(config)) return false;
          if(!check_fused<false,8,7>(config)) return false;
          if(!check_fused<true>(config)) return false;
          if(!check_fused<true,4,0>(config)) return false;
          if(!check_fused<true,4,1>(config)) return false;
          if(!check_fused<true,4,2>(config)) return false;
          if(!check_fused<true,4,3>(config)) return false;
          if(!check_fused<true,8,0>(config)) return false;
          if(!check_fused<true,8,1>(config)) return false;
          if(!check_fused<true,8,2>(config)) return false;
          if(!check_fused<true,8,3>(config)) return false;
          if(!check_fused<true,8,4>(config)) return false;
          if(!check_fused<true,8,5>(config)) return false;
          if(!check_fused<true,8,6>(config)) return false;
          if(!check_fused<true,8,7>(config)) return false;
        }
      }
    std::printf("All 41 BF16 forms: 2048 dot cases, 512 matrices, 512 fused cases; EBF %s\n",
      enhanced?"executed":"unavailable (enhanced checks skipped)");
    return true;
  }
}
