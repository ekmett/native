// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <cstdio>
#include <hint.h>
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
#if defined(NATIVE_CRC_POLYFILL_TEST)
constexpr native::isa<native::arm> requirements=native::polyfill;
static_assert(requirements.has(native::polyfill));
static_assert(!requirements.has(native::arm_feature::crc));
static_assert([] {
  auto crc=~std::uint32_t{},crc_c=crc;
  for(auto c:"123456789") {
    if(!c) break;
    crc=native::crc32<requirements>(crc,std::uint8_t(c));
    crc_c=native::crc32c<requirements>(crc_c,std::uint8_t(c));
  }
  return ~crc==0xcbf43926u && ~crc_c==0xe3069283u;
}());
template<class T> concept crc_operand = requires(std::uint32_t accumulator, T value) {
  native::crc32<requirements>(accumulator, value);
  native::crc32c<requirements>(accumulator, value);
};
static_assert(crc_operand<std::uint8_t> && crc_operand<std::uint16_t> &&
  crc_operand<std::uint32_t> && crc_operand<std::uint64_t>);
static_assert(!crc_operand<float> && !crc_operand<std::int32_t>);
static_assert(native::crc32<native::polyfill>(0u, std::uint8_t(1)) == 0x77073096u);
static_assert(native::crc32c<native::polyfill>(0u, std::uint8_t(1)) == 0xf26b8303u);
#define NATIVE_CRC_TEST_TARGET
#else
constexpr auto requirements=native::target_features<native::arm>("crc");
#define NATIVE_CRC_TEST_TARGET hint_target("crc")
static_assert(requirements==native::isa<native::arm>(native::arm_feature::crc));
static_assert([] {
  native::arm_capabilities cpu;
  cpu.present.set(native::arm_feature::crc,true);
  cpu.observed.set(native::arm_feature::crc,true);
  // CRC operates on integer registers and needs no FP/Advanced SIMD state.
  return native::classify_isa(cpu,requirements).admitted();
}());
#endif
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
NATIVE_CRC_TEST_TARGET bool check(std::uint64_t seed=0x123456789abcdefull) {
  std::uint64_t state=seed;
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
int main(int argc,char **) {
#if !defined(NATIVE_CRC_POLYFILL_TEST)
  auto cpu=native::observe_arm_capabilities();
  auto admitted=native::classify_isa(cpu,requirements);
  if(!admitted.admitted()) { std::printf("SKIP: %s\n",admitted.reason());return 77; }
#endif
#if defined(NATIVE_CRC_POLYFILL_TEST)
  auto passed=check(0x123456789abcdefull ^ std::uint64_t(argc));
#else
  (void)argc;
  auto passed=check();
#endif
  if(!passed) {std::fputs("CRC polynomial or width mismatch\n",stderr);return 1;}
  std::puts("CRC32/CRC32C: all widths, randomized polynomials, byte order and standard check values passed");
}
