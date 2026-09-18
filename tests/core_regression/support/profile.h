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
#if SIMD_TEST_PROFILE == 0
import simd.scalar;
#else
import simd;
#endif
#else
#if SIMD_TEST_PROFILE == 0
#define SIMD_PROFILE 0
#endif
#include <simd/vec.h>
#include <simd/simd/math/exp.h>
#include <simd/simd/math/bits.h>
#endif
#if SIMD_TEST_PROFILE == 512 && SIMD_TEST_AVX512_FP16
using test_arch = simd::avx512_fp16;
#elif SIMD_TEST_PROFILE == 512 && SIMD_TEST_BF16
using test_arch = simd::avx512_bf16;
#elif SIMD_TEST_PROFILE == 512
using test_arch = simd::avx512;
#elif SIMD_TEST_PROFILE == 256
using test_arch = simd::avx2;
#elif SIMD_TEST_PROFILE == 128 && SIMD_TEST_BF16
using test_arch = simd::neon_bf16;
#elif SIMD_TEST_PROFILE == 128 && SIMD_TEST_FP16
using test_arch = simd::neon_fp16;
#elif SIMD_TEST_PROFILE == 128
using test_arch = simd::neon;
#else
using test_arch = simd::scalar;
#endif
namespace test_simd = simd;
#if !SIMD_TEST_IMPORT
#if SIMD_TEST_PROFILE == 512
namespace test_backend = simd::detail::avx512_backend;
#elif SIMD_TEST_PROFILE == 256
namespace test_backend = simd::detail::avx2_backend;
#elif SIMD_TEST_PROFILE == 128
namespace test_backend = simd::detail::neon_backend;
#else
namespace test_backend = simd::detail::scalar_backend;
#endif
#endif

template<class T,std::size_t N> using test_vec = simd::vec<T,N,test_arch>;
