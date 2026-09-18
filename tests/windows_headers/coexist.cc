// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#ifndef NOMINMAX
#define NOMINMAX
#endif
#if SIMD_WINDOWS_HEADER_FIRST
#include <windows.h>
import simd;
#else
import simd;
#include <windows.h>
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
using test_architecture = simd::neon;
__attribute__((target("neon"),noinline))
#else
using test_architecture = simd::avx2;
__attribute__((target("avx2,fma,bmi2"),noinline))
#endif
bool vector_operation(float value) {
  using V = simd::vec<float,4,test_architecture>;
  float input[4]{value,2.f,3.f,4.f}, output[4]{};
  auto a = V::load(input);
  (a+a).store(output);
  for(unsigned i=0;i!=4;++i) if(output[i]!=input[i]+input[i]) return false;
  return true;
}

int main() {
  SYSTEM_INFO info{};
  GetSystemInfo(&info);
  if(!info.dwPageSize) return 1;
#if defined(__aarch64__) || defined(_M_ARM64)
  auto admitted = simd::classify_arm_profile(simd::observe_arm_capabilities(),simd::arm_profile::neon);
#else
  auto admitted = simd::classify_x86_profile(simd::observe_x86_capabilities(),simd::x86_profile::avx2);
#endif
  if(!admitted.admitted()) return 77;
  return vector_operation(static_cast<float>(info.dwPageSize)) ? 0 : 2;
}
