// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <xmmintrin.h>
import simd.cpuid;
import simd.scalar;
#if !SIMD_MINIMAL_HAS_AVX512_BF16 && defined(__AVX512BF16__)
#error Optional BF16 flags leaked into the minimum dispatcher
#endif
#if !SIMD_MINIMAL_HAS_AVX512 && (defined(__AVX512F__) || defined(__AVX512DQ__) || defined(__AVX512BW__) || defined(__AVX512VL__))
#error Optional AVX512 flags leaked into the minimum dispatcher
#endif
extern "C" bool bf16_storage();
extern "C" void bf16_dot2(std::uint16_t const *,std::uint16_t const *,std::uint32_t const *,std::uint32_t *) noexcept;
namespace {
  struct input { std::array<std::uint16_t,32> a{},b{}; std::array<std::uint32_t,16> c{},expected{}; };
  bool check(input const & data) {
    std::array<std::uint32_t,16> out;
    auto saved=_mm_getcsr();
    bool result=true;
    for (unsigned rounding=0;rounding!=4;++rounding)
      for (unsigned denormal=0;denormal!=4;++denormal)
        for (unsigned flags:{0u,63u}) {
          auto control=0x1f80u | (rounding<<13) | ((denormal&1u)<<6) | ((denormal&2u)<<14) | flags;
          _mm_setcsr(control);
          bf16_dot2(data.a.data(),data.b.data(),data.c.data(),out.data());
          auto after=_mm_getcsr();
          _mm_setcsr(saved);
          if(after!=control) {std::puts("BF16 changed MXCSR");result=false;}
          for(unsigned i=0;i!=16;++i)
            if(out[i]!=data.expected[i]) {
              std::printf("lane %u control %08x: %08x expected %08x\n",i,control,out[i],data.expected[i]);
              return false;
            }
        }
    _mm_setcsr(saved);
    return result;
  }
  bool contract() {
    input data;
    auto lane=[&](unsigned i,std::uint16_t al,std::uint16_t ah,std::uint16_t bl,std::uint16_t bh,
        std::uint32_t c,std::uint32_t expected) {
      data.a[2*i]=al;data.a[2*i+1]=ah;data.b[2*i]=bl;data.b[2*i+1]=bh;
      data.c[i]=c;data.expected[i]=expected;
    };
    // These are representation-level architectural cases, not a reordered sum oracle.
    lane(0,0x3f80,0xc580,0x3f80,0x4580,0x4b800000,0x3f800000); // high first: 2^24 - 2^24 + 1
    lane(1,0,0x3980,0,0x3980,0x3f800000,0x3f800000); // even tie at one
    lane(2,0,0x3980,0,0x3980,0x3f800001,0x3f800002); // odd tie
    lane(3,0x0001,0,0x7f00,0,0,0); // BF16 denormal input is zero
    lane(4,0,0,0,0,0x00000001,0); // FP32 accumulator denormal is zero
    lane(5,0x0080,0,0x3f00,0,0,0); // positive FP32 underflow is flushed
    lane(6,0x8080,0,0x3f00,0,0,0x80000000); // preserve sign of flushed output
    lane(7,0x7fc1,0x7fc3,0x7fc2,0x7fc4,0x7fc50000,0x7fc10000); // low NaN wins
    lane(8,0x7f81,0,0x3f80,0,0,0x7fc10000); // quiet signaling NaN
    lane(9,0x7f80,0,0x3f80,0,0,0x7f800000);
    lane(10,0xff80,0,0x3f80,0,0,0xff800000);
    lane(11,0x8000,0x8000,0x3f80,0x3f80,0x80000000,0x80000000); // all negative zeros
    lane(12,0x7f7f,0,0x4000,0,0,0x7f800000); // overflow
    lane(13,0,0x7fc3,0,0x7fc4,0x7fc50000,0x7fc30000); // high NaN wins over accumulator
    lane(14,0,0,0,0,0x7fc50000,0x7fc50000);
    lane(15,0x3f80,0x4000,0x4040,0x4080,0x40a00000,0x41800000); // 5 + 2*4 + 1*3
    if(!check(data))return false;
    auto saved=_mm_getcsr();
    _mm_setcsr(0x1f80);
    std::uint32_t state=0x43983ab1;
    auto next=[&] {state^=state<<13;state^=state>>17;state^=state<<5;return state;};
    for(unsigned iteration=0;iteration!=256;++iteration) {
      // Normal, bounded dyadics: each BF16 product is exact in FP32; two
      // explicit std::fma calls model the specified high-then-low ordering.
      for(unsigned i=0;i!=32;++i) {
        auto a=next(),b=next();
        data.a[i]=std::uint16_t(((a>>16)&0x8000) | ((123+(a%9))<<7) | (a&127));
        data.b[i]=std::uint16_t(((b>>16)&0x8000) | ((123+(b%9))<<7) | (b&127));
      }
      for(unsigned i=0;i!=16;++i) {
        auto c=next();data.c[i]=(c&0x80000000) | ((123+(c%9))<<23) | (c&0x7fffff);
        auto decode=[](std::uint16_t bits) {return std::bit_cast<float>(std::uint32_t(bits)<<16);};
        auto high=std::fma(decode(data.a[2*i+1]),decode(data.b[2*i+1]),std::bit_cast<float>(data.c[i]));
        data.expected[i]=std::bit_cast<std::uint32_t>(std::fma(decode(data.a[2*i]),decode(data.b[2*i]),high));
      }
      if(!check(data)) {_mm_setcsr(saved);return false;}
    }
    _mm_setcsr(saved);
    return true;
  }
}
int main(int argc,char **argv) {
  if(argc!=2)return 2;
  if(!std::strcmp(argv[1],"none")) {std::puts("No BF16 profile entered.");return 0;}
  if(std::strcmp(argv[1],"native"))return 2;
  auto admission=simd::classify_x86_profile(simd::observe_x86_capabilities(),simd::x86_profile::avx512_bf16);
  if(!admission.admitted()) {std::puts(admission.reason());return 77;}
  if(!bf16_storage()) {std::puts("BF16 storage failure");return 3;}
  if(!contract())return 4;
  std::puts("BF16 storage: 65536 encodings and guarded tails; dot2: 4112 lanes in 32 MXCSR states");
}
