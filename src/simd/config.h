#pragma once

#ifdef __cplusplus
// Host profiles own namespace selection and CPU feature admission.
// A module producer fixes its backend independently of consumer ISA flags.
#ifdef SIMD_PROFILE
#define SIMD_EXPLICIT_PROFILE 1
#else
#define SIMD_EXPLICIT_PROFILE 0
#if defined(__AVX512F__) && defined(__AVX512DQ__)
#define SIMD_PROFILE 512
#elif defined(__AVX2__)
#define SIMD_PROFILE 256
#elif defined(__ARM_NEON)
#define SIMD_PROFILE 128
#else
#define SIMD_PROFILE 0
#endif
#endif

#if SIMD_PROFILE == 512
#define SIMD_ARCH ::simd::avx512
#define SIMD_BACKEND avx512_backend
#elif SIMD_PROFILE == 256
#define SIMD_ARCH ::simd::avx2
#define SIMD_BACKEND avx2_backend
#elif SIMD_PROFILE == 128
#define SIMD_ARCH ::simd::neon
#define SIMD_BACKEND neon_backend
#else
#define SIMD_ARCH ::simd::scalar
#define SIMD_BACKEND scalar_backend
#endif
#define SIMD_BACKEND_NAMESPACE simd::detail::SIMD_BACKEND

#if SIMD_PROFILE != 0 && SIMD_PROFILE != 128 && SIMD_PROFILE != 256 && SIMD_PROFILE != 512
#error SIMD_PROFILE must be 0 (scalar), 128 (NEON), 256 (AVX2), or 512 (AVX512)
#endif
#if SIMD_EXPLICIT_PROFILE && SIMD_PROFILE == 512 && !(defined(__AVX2__) && defined(__AVX512F__) && defined(__AVX512DQ__) && defined(__AVX512BW__) && defined(__AVX512VL__))
#error The AVX512 profile requires compiler AVX2 and AVX512F/DQ/BW/VL flags
#endif
#if SIMD_PROFILE == 256 && !defined(__AVX2__)
#error The AVX2 profile requires compiler AVX2 flags
#endif
#if SIMD_PROFILE == 128 && !defined(__ARM_NEON)
#error The NEON profile requires compiler NEON support
#endif
#if (SIMD_PROFILE == 256 || SIMD_PROFILE == 512) && !defined(_MSC_VER) && !defined(__FMA__)
#error The x86 profiles require compiler FMA support
#endif

// These describe the declared profile, not the importing TU's extra features.
#if SIMD_PROFILE == 256 || SIMD_PROFILE == 512
#define SIMD_HAS_AVX2 1
#else
#define SIMD_HAS_AVX2 0
#endif
#if SIMD_PROFILE == 512
#define SIMD_HAS_AVX512F 1
#define SIMD_HAS_AVX512DQ 1
#else
#define SIMD_HAS_AVX512F 0
#define SIMD_HAS_AVX512DQ 0
#endif
#if SIMD_PROFILE == 512 && defined(__AVX512BW__)
#define SIMD_HAS_AVX512BW 1
#else
#define SIMD_HAS_AVX512BW 0
#endif
#if SIMD_PROFILE == 512 && defined(__AVX512VL__)
#define SIMD_HAS_AVX512VL 1
#else
#define SIMD_HAS_AVX512VL 0
#endif
#if SIMD_PROFILE == 128
#define SIMD_HAS_ARM_NEON 1
#else
#define SIMD_HAS_ARM_NEON 0
#endif

// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#else
// Shader compilation has no host ISA profile or CPU feature requirements.
#ifndef SIMD_NAMESPACE
#define SIMD_NAMESPACE simd
#endif
#endif

// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
