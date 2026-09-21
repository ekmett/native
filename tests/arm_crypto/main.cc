// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native;
#include "checks.h"

__attribute__((target("aes"), noinline)) bool bridge() {
  using bytes = native::simd<std::uint8_t,16,native::neon>;
  bytes state = uint8x16_t{};
  bytes result = native::aese<crypto_fixture::aes>(state.to_native(),state.to_native());
  auto raw = result.to_native();
  for(unsigned i=0; i!=16; ++i) if(raw[i] != 0x63) return false;
  return true;
}
int main(int argc, char**) {
  int result=crypto_fixture::run(argc);
  return result ? result : bridge() ? 0 : 1;
}
