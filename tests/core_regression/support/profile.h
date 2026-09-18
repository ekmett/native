#pragma once
#include <cstddef>
// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Select a test architecture without changing the public vec primary.
#if !defined(SIMD_TEST_PROFILE)
#if defined(__AVX512F__)
#define SIMD_TEST_PROFILE 512
#elif defined(__AVX2__)
#define SIMD_TEST_PROFILE 256
#elif defined(__ARM_NEON)
#define SIMD_TEST_PROFILE 128
#else
#define SIMD_TEST_PROFILE 0
#endif
#endif
#if SIMD_TEST_IMPORT
#if SIMD_TEST_PROFILE == 512 && SIMD_TEST_AVX512_FP16
import simd.avx512_fp16;
using test_arch = simd::avx512_fp16;
#elif SIMD_TEST_PROFILE == 512 && SIMD_TEST_BF16
import simd.avx512_bf16;
using test_arch = simd::avx512_bf16;
#elif SIMD_TEST_PROFILE == 512
import simd.avx512;
using test_arch = simd::avx512;
#elif SIMD_TEST_PROFILE == 256
import simd.avx2;
using test_arch = simd::avx2;
#elif SIMD_TEST_PROFILE == 128 && SIMD_TEST_FP16
import simd.neon_fp16;
using test_arch = simd::neon_fp16;
#elif SIMD_TEST_PROFILE == 128
import simd.neon;
using test_arch = simd::neon;
#else
import simd.scalar;
using test_arch = simd::scalar;
#endif
namespace test_simd = simd;
#else
#define SIMD_PROFILE SIMD_TEST_PROFILE
#if SIMD_TEST_AVX512_FP16
#define SIMD_PROFILE_AVX512_FP16 1
#endif
#if SIMD_TEST_BF16
#define SIMD_PROFILE_BF16 1
#endif
#if SIMD_TEST_FP16
#define SIMD_PROFILE_FP16 1
#endif
#include <simd/vec.h>
#include <simd/simd/math/exp.h>
#include <simd/simd/math/bits.h>
using test_arch = SIMD_ARCH;
namespace test_simd = simd;
#endif

template<class T,std::size_t N> using test_vec = simd::vec<T,N,test_arch>;
