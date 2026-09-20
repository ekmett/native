// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#ifndef NOMINMAX
#define NOMINMAX
#endif
#if NATIVE_WINDOWS_HEADER_FIRST
#include <windows.h>
import native;
#else
import native;
#include <windows.h>
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
constexpr auto test_architecture = native::neon;
__attribute__((target("neon"),noinline))
#else
constexpr auto test_architecture = native::avx2;
__attribute__((target("avx2,fma"),noinline))
#endif
bool vector_operation(float value) {
  using V = native::vec<float,4,test_architecture>;
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
  auto admitted = native::classify_isa(native::observe_arm_capabilities(),native::neon);
#else
  auto admitted = native::classify_isa(native::observe_x86_capabilities(),native::avx2);
#endif
  if(!admitted.admitted()) return 77;
  return vector_operation(static_cast<float>(info.dwPageSize)) ? 0 : 2;
}
