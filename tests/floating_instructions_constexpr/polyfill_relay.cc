// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <cstdint>
import native_test.polyfill_float_relay;
int main(int argc,char **) {
  auto input=argc>1?0x40000000u:0x3f800000u;
  auto expected=argc>1?0x41100000u:0x40a00000u;
  return native_test::polyfill_float_relay(input)==std::array{expected,expected}?0:1;
}
