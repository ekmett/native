// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/targets.h>
#include <array>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <type_traits>
#include <utility>
#if NATIVE_TEST_IMPORT
import native;
import native.math;
#else
#include <native/simd.h>
#include <native/wide.h>
#if defined(__x86_64__) || defined(_M_X64)
import native.x86.features;
#elif defined(__aarch64__) || defined(_M_ARM64)
import native.arm.features;
#endif
#endif
#include "generic.h"

#if defined(__x86_64__) || defined(_M_X64)
#define CASE_TARGET avx2
#define CASE_NAMESPACE case_avx2
#define CASE_WIDTH 8
#define CASE_SCOPE avx2
NATIVE_TARGET_PUSH(avx2)
#include "case_body.h"
NATIVE_TARGET_POP()
#undef CASE_TARGET
#undef CASE_NAMESPACE
#undef CASE_WIDTH
#undef CASE_SCOPE
#define NATIVE_TARGET_kernel_base "avx2,fma,avx512f,avx512dq"
#define CASE_TARGET kernel_base
#define CASE_NAMESPACE case_kernel_base
#define CASE_WIDTH 16
#define CASE_SCOPE kernel_base
NATIVE_TARGET_PUSH(kernel_base)
#include "case_body.h"
NATIVE_TARGET_POP()
#undef CASE_TARGET
#undef CASE_NAMESPACE
#undef CASE_WIDTH
#undef CASE_SCOPE
#define NATIVE_TARGET_kernel_bw "avx2,fma,avx512f,avx512dq,avx512bw"
#define CASE_TARGET kernel_bw
#define CASE_NAMESPACE case_kernel_bw
#define CASE_WIDTH 16
#define CASE_SCOPE kernel_bw
NATIVE_TARGET_PUSH(kernel_bw)
#include "case_body.h"
NATIVE_TARGET_POP()
#undef CASE_TARGET
#undef CASE_NAMESPACE
#undef CASE_WIDTH
#undef CASE_SCOPE
#define NATIVE_TARGET_kernel_vl "avx2,fma,avx512f,avx512dq,avx512vl"
#define CASE_TARGET kernel_vl
#define CASE_NAMESPACE case_kernel_vl
#define CASE_WIDTH 16
#define CASE_SCOPE kernel_vl
NATIVE_TARGET_PUSH(kernel_vl)
#include "case_body.h"
NATIVE_TARGET_POP()
#undef CASE_TARGET
#undef CASE_NAMESPACE
#undef CASE_WIDTH
#undef CASE_SCOPE
#define CASE_TARGET avx512
#define CASE_NAMESPACE case_avx512
#define CASE_WIDTH 16
#define CASE_SCOPE avx512
NATIVE_TARGET_PUSH(avx512)
#include "case_body.h"
NATIVE_TARGET_POP()
#undef CASE_TARGET
#undef CASE_NAMESPACE
#undef CASE_WIDTH
#undef CASE_SCOPE
#define NATIVE_TARGET_kernel_bw_bf16 "avx2,fma,avx512f,avx512dq,avx512bw,avx512bf16"
#define CASE_TARGET kernel_bw_bf16
#define CASE_NAMESPACE case_kernel_bw_bf16
#define CASE_WIDTH 16
#define CASE_SCOPE kernel_bw
NATIVE_TARGET_PUSH(kernel_bw)
#include "case_body.h"
NATIVE_TARGET_POP()
#undef CASE_TARGET
#undef CASE_NAMESPACE
#undef CASE_WIDTH
#undef CASE_SCOPE
#define CASE_TARGET avx512_bf16
#define CASE_NAMESPACE case_avx512_bf16
#define CASE_WIDTH 16
#define CASE_SCOPE avx512
NATIVE_TARGET_PUSH(avx512)
#include "case_body.h"
NATIVE_TARGET_POP()
#undef CASE_TARGET
#undef CASE_NAMESPACE
#undef CASE_WIDTH
#undef CASE_SCOPE
#define NATIVE_TARGET_kernel_bw_fp16 "avx2,fma,avx512f,avx512dq,avx512bw,avx512fp16"
#define CASE_TARGET kernel_bw_fp16
#define CASE_NAMESPACE case_kernel_bw_fp16
#define CASE_WIDTH 16
#define CASE_SCOPE kernel_bw
NATIVE_TARGET_PUSH(kernel_bw)
#include "case_body.h"
NATIVE_TARGET_POP()
#undef CASE_TARGET
#undef CASE_NAMESPACE
#undef CASE_WIDTH
#undef CASE_SCOPE
#define CASE_TARGET avx512_fp16
#define CASE_NAMESPACE case_avx512_fp16
#define CASE_WIDTH 16
#define CASE_SCOPE avx512
NATIVE_TARGET_PUSH(avx512)
#include "case_body.h"
NATIVE_TARGET_POP()
#undef CASE_TARGET
#undef CASE_NAMESPACE
#undef CASE_WIDTH
#undef CASE_SCOPE
#define NATIVE_TARGET_kernel_bw_half "avx2,fma,avx512f,avx512dq,avx512bw,avx512bf16,avx512fp16"
#define CASE_TARGET kernel_bw_half
#define CASE_NAMESPACE case_kernel_bw_half
#define CASE_WIDTH 16
#define CASE_SCOPE kernel_bw
NATIVE_TARGET_PUSH(kernel_bw)
#include "case_body.h"
NATIVE_TARGET_POP()
#undef CASE_TARGET
#undef CASE_NAMESPACE
#undef CASE_WIDTH
#undef CASE_SCOPE
#define NATIVE_TARGET_kernel_half "avx2,fma,avx512f,avx512dq,avx512bw,avx512vl,avx512bf16,avx512fp16"
#define CASE_TARGET kernel_half
#define CASE_NAMESPACE case_kernel_half
#define CASE_WIDTH 16
#define CASE_SCOPE avx512
NATIVE_TARGET_PUSH(avx512)
#include "case_body.h"
NATIVE_TARGET_POP()
#undef CASE_TARGET
#undef CASE_NAMESPACE
#undef CASE_WIDTH
#undef CASE_SCOPE
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define CASE_TARGET neon
#define CASE_NAMESPACE case_neon
#define CASE_WIDTH 4
#define CASE_SCOPE neon
NATIVE_TARGET_PUSH(neon)
#include "case_body.h"
NATIVE_TARGET_POP()
#undef CASE_TARGET
#undef CASE_NAMESPACE
#undef CASE_WIDTH
#undef CASE_SCOPE
#define CASE_TARGET neon_bf16
#define CASE_NAMESPACE case_neon_bf16
#define CASE_WIDTH 4
#define CASE_SCOPE neon
NATIVE_TARGET_PUSH(neon)
#include "case_body.h"
NATIVE_TARGET_POP()
#undef CASE_TARGET
#undef CASE_NAMESPACE
#undef CASE_WIDTH
#undef CASE_SCOPE
#define CASE_TARGET neon_fp16
#define CASE_NAMESPACE case_neon_fp16
#define CASE_WIDTH 4
#define CASE_SCOPE neon
NATIVE_TARGET_PUSH(neon)
#include "case_body.h"
NATIVE_TARGET_POP()
#undef CASE_TARGET
#undef CASE_NAMESPACE
#undef CASE_WIDTH
#undef CASE_SCOPE
#define NATIVE_TARGET_kernel_arm_half "neon,fullfp16,bf16"
#define CASE_TARGET kernel_arm_half
#define CASE_NAMESPACE case_kernel_arm_half
#define CASE_WIDTH 4
#define CASE_SCOPE neon
NATIVE_TARGET_PUSH(neon)
#include "case_body.h"
NATIVE_TARGET_POP()
#undef CASE_TARGET
#undef CASE_NAMESPACE
#undef CASE_WIDTH
#undef CASE_SCOPE
#endif

int main() {
  if(!generic_test::check()) return 1;
  unsigned passed=0,skipped=0;
#if defined(__x86_64__) || defined(_M_X64)
  auto cpu=native::observe_x86_capabilities();
#elif defined(__aarch64__) || defined(_M_ARM64)
  auto cpu=native::observe_arm_capabilities();
#endif
#define RUN(name,scope) if(native::classify_isa(cpu,NATIVE_TARGET_ISA(scope),NATIVE_TARGET_MINIMUM).admitted()) { \
  if(!case_##name::check()) {std::printf("FAIL %s\n",#name);return 2;} \
  ++passed;std::printf("PASS %s\n",#name); \
} else {++skipped;std::printf("SKIP %s: CPU/OS admission\n",#name);}
#if defined(__x86_64__) || defined(_M_X64)
  RUN(avx2,avx2)
  RUN(kernel_base,kernel_base)
  RUN(kernel_bw,kernel_bw)
  RUN(kernel_vl,kernel_vl)
  RUN(avx512,avx512)
  RUN(kernel_bw_bf16,kernel_bw)
  RUN(avx512_bf16,avx512)
  RUN(kernel_bw_fp16,kernel_bw)
  RUN(avx512_fp16,avx512)
  RUN(kernel_bw_half,kernel_bw)
  RUN(kernel_half,avx512)
#elif defined(__aarch64__) || defined(_M_ARM64)
  RUN(neon,neon)
  RUN(neon_bf16,neon)
  RUN(neon_fp16,neon)
  RUN(kernel_arm_half,neon)
#endif
#undef RUN
  std::printf("kernel policies: %u executed, %u skipped\n",passed,skipped);
  return passed?0:77;
}
