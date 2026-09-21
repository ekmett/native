// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <limits>
import native.arm.crc;
import native.arm.rdm;
import native.arm.pmull;
import native.arm.sha;

static std::uint32_t reference_crc(std::uint32_t c,std::uint64_t x,unsigned bits,std::uint32_t polynomial) {
  for(unsigned i=0;i<bits;++i) {
    auto low = (c ^ x) & 1u;
    c = (c >> 1) ^ (low ? polynomial : 0);
    x >>= 1;
  }
  return c;
}
// Keep optional instructions beyond the admission branch.
__attribute__((noinline)) static bool check(unsigned seed) {
  bool ok = true;
  auto c = std::uint32_t{0x12345678} ^ seed;
  auto x = std::uint64_t{0xfedcba9876543210} ^ seed;
  auto crc_width = [&]<class T>() {
    auto word=static_cast<T>(x);
    return native::crc32(c,word) == reference_crc(c,word,8*sizeof(T),0xedb88320)
      && native::crc32c(c,word) == reference_crc(c,word,8*sizeof(T),0x82f63b78);
  };
  ok &= crc_width.template operator()<std::uint8_t>() && crc_width.template operator()<std::uint16_t>()
    && crc_width.template operator()<std::uint32_t>() && crc_width.template operator()<std::uint64_t>();
  ok &= native::sha1h(c) == std::rotr(c,2);
  auto product = native::pmull(x,std::uint64_t{3});
  std::array<std::uint64_t,2> words{};
  product.store(words.data());
  ok &= words[0] == (x ^ (x << 1)) && words[1] == (x >> 63);
  std::uint64_t saved;
  asm volatile("mrs %0, fpsr" : "=r"(saved));
  auto initial = saved & ~(std::uint64_t{1} << 27);
  asm volatile("msr fpsr, %0" : : "r"(initial) : "memory");
  auto rdm_width = [&]<class T>() {
    auto low = std::numeric_limits<T>::min(), high = std::numeric_limits<T>::max();
    return native::sqrdmlah(high,low,low) == high
      && native::sqrdmlsh(low,low,low) == low;
  };
  ok &= rdm_width.template operator()<std::int16_t>() && rdm_width.template operator()<std::int32_t>();
  std::uint64_t status;
  asm volatile("mrs %0, fpsr" : "=r"(status));
  asm volatile("msr fpsr, %0" : : "r"(saved) : "memory");
  return ok && status == (initial | (std::uint64_t{1} << 27));
}
int main(int argc,char**) {
  auto admitted = native::classify_isa(native::observe_arm_capabilities(),
    native::target_features("neon,crc,aes,sha2,rdm"));
  if(!admitted.admitted()) { std::puts(admitted.reason()); return 77; }
  if(!check(static_cast<unsigned>(argc))) { std::puts("ARM scalar default reference check failed"); return 1; }
  return 0;
}
