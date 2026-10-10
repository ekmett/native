// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iterator>
#if defined(__aarch64__)
#include "../neon_bf16/reference_cases.h"
#elif defined(__x86_64__)
#include <xmmintrin.h>
#endif
import native;

namespace {
  using namespace native;
  template<std::size_t N,isa<> A> constexpr bool simple(unsigned seed=0) {
    using H=simd<bf16,N,A>; using V=simd<float,N/2,A>;
    std::array<std::uint16_t,N> a{},b{}; std::array<float,N/2> c{},out{};
    for(std::size_t i=0;i<N/2;++i) {
      a[2*i]=0x4000; a[2*i+1]=0x4040; b[2*i]=0x4040; b[2*i+1]=0x4080; c[i]=float(i+seed);
    }
    dot2(H::load_bits(a.data()),H::load_bits(b.data()),V::load(c.data())).store(out.data());
    for(std::size_t i=0;i<N/2;++i) if(out[i]!=18.f+float(i+seed)) { std::printf("dot2 shape N=%zu lane=%zu got=%g expected=%g\n",N,i,double(out[i]),double(18.f+float(i+seed))); return false; }
    return true;
  }
  template<isa<> A> constexpr bool raw_storage(std::uint16_t word) {
    using H=simd<bf16,64,A>; std::array<std::uint16_t,64> out{};
    H(bf16::from_bits(word)).store_bits(out.data());
    for(auto bits:out) if(bits!=word) { std::printf("broadcast bits=%04x expected=%04x\n",unsigned(bits),unsigned(word)); return false; }
    std::array<bf16,64> elements{}; H::load_bits(out.data()).store(elements.data());
    H::load_partial(elements.data(),63,bf16::from_bits(std::uint16_t(word^1))).store_bits(out.data());
    for(std::size_t i=0;i<63;++i) if(out[i]!=word) { std::printf("partial lane=%zu bits=%04x expected=%04x\n",i,unsigned(out[i]),unsigned(word)); return false; }
    if(out[63]!=std::uint16_t(word^1)) { std::puts("partial fill mismatch"); return false; }
    auto first=H::load_bits(out.data()).template set<31>(bf16::from_bits(std::uint16_t(word^2)));
    if(first.template get<31>().to_bits()!=std::uint16_t(word^2)) { std::puts("get/set mismatch"); return false; }
    auto second=H(bf16::from_bits(std::uint16_t(word^4)));
    auto chosen=select(H::mask_type::from_bitset(0x5555555555555555ull),first,second);
    chosen.store_bits(out.data());
    for(std::size_t i=0;i<64;++i) if(out[i]!=((i&1)?std::uint16_t(word^4):word)) { std::printf("select lane=%zu bits=%04x word=%04x\n",i,unsigned(out[i]),unsigned(word)); return false; }
    return true;
  }
  static_assert(raw_storage<isa<>(polyfill)>(0x7f81));
  static_assert(simple<2,isa<>(polyfill)>() && simple<18,isa<>(polyfill)>() && simple<64,isa<>(polyfill)>());
#if defined(__aarch64__)
  constexpr auto feature=arm_feature::neon_bf16|polyfill;
  constexpr auto profile=neon_bf16|polyfill;
#elif defined(__x86_64__)
  constexpr auto feature=x86_feature::avx512bf16|polyfill;
  constexpr auto profile=avx512_bf16|polyfill;
#endif
  static_assert(simple<2,feature>() && simple<18,feature>() && simple<64,feature>());
  static_assert(raw_storage<profile>(0x7f81) && raw_storage<feature>(0xffc5));
  static_assert(simple<8,profile>() && simple<18,profile>() && simple<64,profile>());
  template<class H,class V> concept accepts_dot2=requires(H h,V v) { dot2(h,h,v); };
  static_assert(!accepts_dot2<simd<bf16,17,polyfill>,simd<float,8,polyfill>>);

  // A small aggregate return must keep all BF16 bits, independently of the
  // compiler's native BF16 scalar ABI and the active floating environment.
  __attribute__((noinline)) simd<bf16,4,polyfill> polyfill_bf16_transport(std::uint16_t const * words) noexcept {
    auto result=simd<bf16,4,polyfill>::load_bits(words);
    asm volatile("" : "+m"(result) : : "memory");
    return result;
  }
  extern "C" __attribute__((noinline)) void polyfill_dot2_scalar(std::uint16_t const * a,std::uint16_t const * b,
      std::uint32_t const * c,std::uint32_t * out) noexcept {
    using H=simd<bf16,18,polyfill>; using V=simd<float,9,polyfill>;
    dot2(H::load_bits(a),H::load_bits(b),V::load_bits(c)).store_bits(out);
  }
#if defined(__aarch64__)
#define DOT_TARGET "neon,bf16"
#define DOT_PROFILE_TARGET DOT_TARGET
#else
#define DOT_TARGET "avx512bf16"
#define DOT_PROFILE_TARGET "avx2,fma,avx512f,avx512dq,avx512bw,avx512vl,avx512bf16"
#endif
  extern "C" __attribute__((noinline,target(DOT_TARGET))) void polyfill_dot2_hardware(std::uint16_t const * a,
      std::uint16_t const * b,std::uint32_t const * c,std::uint32_t * out) noexcept {
    using H=simd<bf16,18,feature>; using V=simd<float,9,feature>;
    dot2(H::load_bits(a),H::load_bits(b),V::load_bits(c)).store_bits(out);
  }
  template<std::size_t N,isa<> A> __attribute__((target(DOT_PROFILE_TARGET))) bool native_simple(unsigned seed) noexcept {
    using H=simd<bf16,N,A>; using V=simd<float,N/2,A>;
    std::array<std::uint16_t,N> a{},b{}; std::array<float,N/2> c{},out{};
    for(std::size_t i=0;i<N/2;++i) {
      a[2*i]=0x4000; a[2*i+1]=0x4040; b[2*i]=0x4040; b[2*i+1]=0x4080; c[i]=float(i+seed);
    }
    dot2(H::load_bits(a.data()),H::load_bits(b.data()),V::load(c.data())).store(out.data());
    for(std::size_t i=0;i<N/2;++i) if(out[i]!=18.f+float(i+seed)) { std::printf("dot2 shape N=%zu lane=%zu got=%g expected=%g\n",N,i,double(out[i]),double(18.f+float(i+seed))); return false; }
    return true;
  }
  __attribute__((noinline,target(DOT_PROFILE_TARGET))) bool hardware_shapes(unsigned seed) noexcept {
    for(unsigned word=0;word<65536;++word) if(!raw_storage<profile>(std::uint16_t(word))) { std::printf("native BF16 storage word=%04x\n",word); return false; }
    return native_simple<2,feature>(seed) && native_simple<18,feature>(seed) && native_simple<64,feature>(seed) &&
      native_simple<8,profile>(seed) && native_simple<18,profile>(seed) && native_simple<64,profile>(seed);
  }
#undef DOT_TARGET
#undef DOT_PROFILE_TARGET
#if defined(__aarch64__)
  bool corpus(bool hardware,bool enhanced) noexcept {
    std::uint64_t saved,status;
    asm volatile("mrs %0, fpcr":"=r"(saved)); asm volatile("mrs %0, fpsr":"=r"(status));
    bool valid=true;
    for(unsigned ebf=0;ebf<=(enhanced?1u:0u);++ebf) for(unsigned mode=0;mode<4;++mode) for(unsigned flush=0;flush<2;++flush) {
      std::uint64_t control=(std::uint64_t(ebf)<<13)|(std::uint64_t(mode)<<22)|(std::uint64_t(flush)<<24);
      asm volatile("msr fpcr, %0"::"r"(control):"memory"); asm volatile("msr fpsr, xzr":::"memory");
      for(std::size_t begin=0;begin<std::size(neon_bf16_reference::cases);begin+=9) {
        std::array<std::uint16_t,18> a{},b{}; std::array<std::uint32_t,9> c{},expected{},out{},native_out{};
        auto const count=std::min(std::size_t{9},std::size(neon_bf16_reference::cases)-begin);
        for(std::size_t i=0;i<count;++i) {
          auto const & s=neon_bf16_reference::cases[begin+i];
          a[2*i]=s.a0;a[2*i+1]=s.a1;b[2*i]=s.b0;b[2*i+1]=s.b1;c[i]=s.accumulator;
          expected[i]=ebf?s.enhanced[mode*2+flush]:s.baseline;
        }
        polyfill_dot2_scalar(a.data(),b.data(),c.data(),out.data());
        if(hardware) polyfill_dot2_hardware(a.data(),b.data(),c.data(),native_out.data());
        for(std::size_t i=0;i<count;++i) if(out[i]!=expected[i] || (hardware && native_out[i]!=expected[i])) valid=false;
      }
      std::uint64_t observed,flags; asm volatile("mrs %0, fpcr":"=r"(observed)); asm volatile("mrs %0, fpsr":"=r"(flags));
      if(observed!=control || flags!=0) valid=false;
    }
    asm volatile("msr fpcr, %0"::"r"(saved):"memory"); asm volatile("msr fpsr, %0"::"r"(status):"memory");
    return valid;
  }
#else
  bool corpus(bool hardware,bool) noexcept {
    auto const saved=_mm_getcsr(); bool valid=true;
    std::uint32_t state=0x71a45e19;
    auto next=[&] { state^=state<<13;state^=state>>17;state^=state<<5;return state; };
    for(unsigned mode=0;mode<4;++mode) for(unsigned flush=0;flush<4;++flush) {
      auto const control=(saved&~(0x6000u|0x8040u|0x3fu))|(mode<<13)|((flush&1)?0x8000u:0u)|((flush&2)?0x40u:0u);
      _mm_setcsr(control);
      for(unsigned packet=0;packet<128;++packet) {
        std::array<std::uint16_t,18> a{},b{}; std::array<std::uint32_t,9> c{},out{},native_out{};
        for(auto & x:a) x=std::uint16_t(next()); for(auto & x:b) x=std::uint16_t(next()); for(auto & x:c) x=next();
        polyfill_dot2_scalar(a.data(),b.data(),c.data(),out.data());
        if(hardware) {
          polyfill_dot2_hardware(a.data(),b.data(),c.data(),native_out.data());
          for(std::size_t i=0;i<out.size();++i) if(out[i]!=native_out[i]) {
            std::printf("dot2 mode=%u flush=%u packet=%u lane=%zu: a=%04x,%04x b=%04x,%04x c=%08x software=%08x hardware=%08x\n",
              mode,flush,packet,i,unsigned(a[2*i]),unsigned(a[2*i+1]),
              unsigned(b[2*i]),unsigned(b[2*i+1]),c[i],out[i],native_out[i]);
            _mm_setcsr(saved); return false;
          }
        }
        if(_mm_getcsr()!=control) {
          std::printf("dot2 changed MXCSR: expected=%08x actual=%08x\n",control,_mm_getcsr());
          _mm_setcsr(saved); return false;
        }
      }
    }
    _mm_setcsr(saved); return valid;
  }
#endif
}
int main(int argc,char **) {
  auto cpu=native::observe_cpu();
#if defined(__aarch64__)
  bool hardware=native::classify_isa(cpu,native::neon_bf16).admitted();
  bool enhanced=cpu.raw.ebf16_observed && cpu.raw.ebf16;
#else
  bool hardware=native::classify_isa(cpu,native::avx512_bf16).admitted();
  bool enhanced=false;
#endif
  for(unsigned word=0;word<65536;++word) {
    if(!raw_storage<native::isa<>(native::polyfill)>(std::uint16_t(word))) { std::printf("BF16 software storage failed: %04x\n",word); return 4; }
    std::array<std::uint16_t,4> input{std::uint16_t(word),0x8000,0x8001,0x7f81},output{};
    polyfill_bf16_transport(input.data()).store_bits(output.data());
    if(input!=output) { std::printf("BF16 transport failed: %04x\n",word); return 5; }
  }
  if(!simple<2,native::isa<>(native::polyfill)>(unsigned(argc-1)) ||
     !simple<18,native::isa<>(native::polyfill)>(unsigned(argc-1)) ||
     !simple<64,native::isa<>(native::polyfill)>(unsigned(argc-1))) return 1;
  if(hardware && !hardware_shapes(unsigned(argc-1))) { std::puts("BF16 hardware storage or shape failed"); return 2; }
  if(!corpus(hardware,enhanced)) return 3;
  std::puts(hardware?"BF16 logical dot2: software, native groups and caller controls passed":"BF16 logical dot2: software and caller controls passed; native feature unavailable");
}
