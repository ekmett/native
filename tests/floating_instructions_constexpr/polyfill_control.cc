// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/bf16_constexpr.h>
#include "../neon_bf16/reference_cases.h"
#include <cstdio>
namespace cf=native::detail::constexpr_float;
constexpr cf::policy before{.flush_outputs=true};
constexpr cf::policy after{.flush_outputs_after_rounding=true};
// Subnormal packing can round to minimum normal while full-precision tininess
// still flushes it. Only a full-precision carry crosses the AH boundary.
static_assert(cf::mul_bits<cf::binary32>(0x00800000,0x3f7fffff,cf::rounding::nearest_even,before)==0);
static_assert(cf::mul_bits<cf::binary32>(0x00800000,0x3f7fffff,cf::rounding::nearest_even,after)==0);
static_assert(cf::fma_bits<cf::binary32>(0x80800000,0x32800000,0x00800000,cf::rounding::nearest_even,after)==0x00800000);
static_assert(cf::fma_bits<cf::binary32>(0x80800000,0x32800000,0x00800000,cf::rounding::nearest_even,before)==0);
static_assert(cf::dot2_bits<cf::binary32>(0x7f7f0000,0xff7f0000,0x40000000,0x40000000)==0);
int main() {
  using namespace native::detail;
  for(auto const & c:neon_bf16_reference::cases) {
    auto actual=arm_bfdot_bits(c.accumulator,c.a0,c.a1,c.b0,c.b1);
    if(actual!=c.baseline) {std::printf("legacy %08x != %08x\n",actual,c.baseline);return 1;}
    for(unsigned i=0;i<8;++i) {
      arm_float_control control{(1ull<<13)|(std::uint64_t(i/2)<<22)|(std::uint64_t(i%2)<<24)};
      actual=arm_bfdot_bits(c.accumulator,c.a0,c.a1,c.b0,c.b1,control);
      if(actual!=c.enhanced[i]) {std::printf("enhanced %u %08x != %08x\n",i,actual,c.enhanced[i]);return 2;}
    }
  }
}
