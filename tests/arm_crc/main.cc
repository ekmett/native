// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <cstdio>
#include <native/attributes.h>
#if NATIVE_TEST_INTERFACE == 0
#include <native/arm/crc.h>
import native.arm.features;
#elif NATIVE_TEST_INTERFACE == 1
import native.arm.crc;
#elif NATIVE_TEST_INTERFACE == 2
import native.arm;
#else
import native;
#endif
constexpr auto requirements=native::target_features("crc");
static_assert(requirements==native::isa(native::arm_feature::crc));
static_assert([] {
  native::arm_capabilities cpu;
  cpu.present.set(native::arm_feature::crc,true);
  cpu.observed.set(native::arm_feature::crc,true);
  // CRC operates on integer registers and needs no FP/Advanced SIMD state.
  return native::classify_isa(cpu,requirements).admitted();
}());
std::uint32_t reference(std::uint32_t crc,std::uint64_t value,unsigned width,std::uint32_t polynomial) {
  for(unsigned bit=0;bit<width;++bit) {
    auto feedback=(crc^std::uint32_t(value))&1u;
    crc=(crc>>1)^(feedback?polynomial:0u);
    value>>=1;
  }
  return crc;
}
std::uint64_t next(std::uint64_t & state) {
  state^=state<<13;state^=state>>7;state^=state<<17;return state;
}
native_target("crc") bool check() {
  std::uint64_t state=0x123456789abcdefull;
  for(unsigned i=0;i<4096;++i) {
    auto acc=std::uint32_t(next(state));auto word=next(state);
#define CHECK(width) \
    if(native::crc32<requirements>(acc,std::uint##width##_t(word))!=reference(acc,word,width,0xedb88320u) || \
       native::crc32c<requirements>(acc,std::uint##width##_t(word))!=reference(acc,word,width,0x82f63b78u)) return false;
    CHECK(8) CHECK(16) CHECK(32) CHECK(64)
#undef CHECK
    auto c=acc,cc=acc;
    for(unsigned byte=0;byte<8;++byte) {
      c=native::crc32<requirements>(c,std::uint8_t(word>>(8*byte)));
      cc=native::crc32c<requirements>(cc,std::uint8_t(word>>(8*byte)));
    }
    if(c!=native::crc32<requirements>(acc,word) || cc!=native::crc32c<requirements>(acc,word)) return false;
  }
  auto crc=~std::uint32_t{},crc_c=crc;
  for(auto c:"123456789") {
    if(!c) break;
    crc=native::crc32<requirements>(crc,std::uint8_t(c));
    crc_c=native::crc32c<requirements>(crc_c,std::uint8_t(c));
  }
  return ~crc==0xcbf43926u && ~crc_c==0xe3069283u;
}
int main() {
  auto cpu=native::observe_arm_capabilities();
  auto admitted=native::classify_isa(cpu,requirements);
  if(!admitted.admitted()) { std::printf("SKIP: %s\n",admitted.reason());return 77; }
  if(!check()) {std::fputs("CRC polynomial or width mismatch\n",stderr);return 1;}
  std::puts("CRC32/CRC32C: all widths, randomized polynomials, byte order and standard check values passed");
}
