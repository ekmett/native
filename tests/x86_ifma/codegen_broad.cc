// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#define NATIVE_TEST_TARGET_0 "avxifma,avx512f,avx512bw,avx512dq,avx512vl,avx512cd,fma"
#define NATIVE_TEST_TARGET_1 "avxifma,avx512f,avx512bw,avx512dq,avx512vl,avx512cd,fma"
#define NATIVE_TEST_TARGET_2 "avx512ifma,avx512vl,avx512f,avx512bw,avx512dq,avx512vl,avx512cd,fma"
#define NATIVE_TEST_TARGET_3 "avx512ifma,avx512vl,avx512f,avx512bw,avx512dq,avx512vl,avx512cd,fma"
#define NATIVE_TEST_TARGET_4 "avx512ifma,avx512f,avx512bw,avx512dq,avx512vl,avx512cd,fma"
#include "codegen.cc"
