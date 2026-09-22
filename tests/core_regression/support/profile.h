#pragma once
#include <cstddef>
// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Select a test architecture without changing the public simd primary.
#if !defined(NATIVE_TEST_PROFILE)
#if defined(__AVX512F__)
#define NATIVE_TEST_PROFILE 512
#elif defined(__AVX2__)
#define NATIVE_TEST_PROFILE 256
#elif defined(__ARM_NEON)
#define NATIVE_TEST_PROFILE 128
#else
#define NATIVE_TEST_PROFILE 0
#endif
#endif
#if NATIVE_TEST_IMPORT
#if NATIVE_TEST_PROFILE == 0
import native.scalar;
#else
import native;
#endif
#else
#if NATIVE_TEST_PROFILE == 0
#define NATIVE_PROFILE 0
#endif
#include <native/simd.h>
#include <native/simd/math/exp.h>
#include <native/simd/math/bits.h>
#endif
#if NATIVE_TEST_PROFILE == 512 && NATIVE_TEST_AVX512_FP16
constexpr auto test_arch = native::avx512_fp16;
#elif NATIVE_TEST_PROFILE == 512 && NATIVE_TEST_BF16
constexpr auto test_arch = native::avx512_bf16;
#elif NATIVE_TEST_PROFILE == 512
constexpr auto test_arch = native::avx512;
#elif NATIVE_TEST_PROFILE == 256
constexpr auto test_arch = native::avx2;
#elif NATIVE_TEST_PROFILE == 128 && NATIVE_TEST_BF16
constexpr auto test_arch = native::neon_bf16;
#elif NATIVE_TEST_PROFILE == 128 && NATIVE_TEST_FP16
constexpr auto test_arch = native::neon_fp16;
#elif NATIVE_TEST_PROFILE == 128
constexpr auto test_arch = native::neon;
#else
constexpr auto test_arch = native::scalar;
#endif
namespace test_simd = native;
#if !NATIVE_TEST_IMPORT
#if NATIVE_TEST_PROFILE == 512
namespace test_backend = native::detail::avx512_backend;
#elif NATIVE_TEST_PROFILE == 256
namespace test_backend = native::detail::avx2_backend;
#elif NATIVE_TEST_PROFILE == 128
namespace test_backend = native::detail::neon_backend;
#else
namespace test_backend = native::detail::scalar_backend;
#endif
#endif

template<class T,std::size_t N> using test_vec = native::simd<T,N,test_arch>;
