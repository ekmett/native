// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cstdint>
#include <cstring>
import native.x86.sha512;
import native.x86.sm3;
import native.x86.sm4;
#include "checks.h"
using namespace native;
int main(int argc,char **argv) {
  if(argc!=2) return 1;
  auto cpu=observe_x86_capabilities();
  if(!std::strcmp(argv[1],"sha512")) {
    constexpr auto arch=target_features<x86>("sha512");
    if(!classify_isa(cpu,arch).admitted()) return 77;
    return !checks::sha512_abc<arch>();
  }
  if(!std::strcmp(argv[1],"sm3")) {
    constexpr auto arch=target_features<x86>("sm3");
    if(!classify_isa(cpu,arch).admitted()) return 77;
    return !checks::sm3_abc<arch>();
  }
  if(!std::strcmp(argv[1],"sm4")) {
    constexpr auto arch=target_features<x86>("sm4");
    if(!classify_isa(cpu,arch).admitted()) return 77;
    return !(checks::sm4_example<arch,4>() && checks::sm4_example<arch,8>());
  }
  return 1;
}
