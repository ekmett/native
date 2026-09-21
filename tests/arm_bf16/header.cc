// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
#include <native/arm/bf16.h>
import native.arm.features;
#include "checks.h"
int main(int argc, char**) {
  auto cpu=native::observe_arm_capabilities();
  auto admitted=native::classify_isa(cpu,bf16_fixture::arch,NATIVE_TARGET_MINIMUM);
  if(!admitted.admitted()) {std::puts(admitted.reason()); return 77;}
  bool enhanced=native::classify_isa(cpu,native::arm_feature::ebf16).admitted();
  if(argc>1 && !enhanced) {std::puts("Enhanced BF16 not observed; FPCR.EBF=1 not entered"); return 77;}
  return bf16_fixture::arithmetic(enhanced) && bf16_fixture::effects(enhanced)?0:1;
}
