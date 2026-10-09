// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
namespace native::detail {
  template<class T,std::size_t N,isa<> A> struct polyfill_feature_chunk;
}
#if NATIVE_HOST_NEON
#define NATIVE_POLYFILL_FEATURE_CHUNK(T,N,A) (polyfill_element_traits<T>::kind==polyfill_element_kind::binary16 && N==8 && neon_fp16<=A)
#pragma clang attribute push(__attribute__((target("neon,fullfp16"))), apply_to=function)
#include "native/simd/polyfill_feature_chunk_body.h"
#pragma clang attribute pop
#undef NATIVE_POLYFILL_FEATURE_CHUNK
#define NATIVE_POLYFILL_FEATURE_CHUNK(T,N,A) (polyfill_element_traits<T>::kind==polyfill_element_kind::bfloat16 && N==8 && neon_bf16<=A)
#pragma clang attribute push(__attribute__((target("neon,bf16"))), apply_to=function)
#include "native/simd/polyfill_feature_chunk_body.h"
#pragma clang attribute pop
#undef NATIVE_POLYFILL_FEATURE_CHUNK
#elif NATIVE_HOST_X86
#define NATIVE_POLYFILL_FEATURE_CHUNK(T,N,A) (polyfill_element_traits<T>::kind==polyfill_element_kind::binary16 && N==32 && avx512_fp16<=A)
#pragma clang attribute push(__attribute__((target("avx2,fma,avx512f,avx512dq,avx512bw,avx512vl,avx512fp16"))), apply_to=function)
#include "native/simd/polyfill_feature_chunk_body.h"
#pragma clang attribute pop
#undef NATIVE_POLYFILL_FEATURE_CHUNK
#define NATIVE_POLYFILL_FEATURE_CHUNK(T,N,A) (polyfill_element_traits<T>::kind==polyfill_element_kind::bfloat16 && (N==8 || N==16 || N==32) && avx512_bf16<=A)
#pragma clang attribute push(__attribute__((target("avx2,fma,avx512f,avx512dq,avx512bw,avx512vl,avx512bf16"))), apply_to=function)
#include "native/simd/polyfill_feature_chunk_body.h"
#pragma clang attribute pop
#undef NATIVE_POLYFILL_FEATURE_CHUNK
#endif
