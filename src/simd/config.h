#pragma once
// Host architecture controls declaration availability. Function target scopes
// select instruction requirements independently of the producer's minimum ISA.
#ifdef __cplusplus
#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_HOST_X86 1
#else
#define SIMD_HOST_X86 0
#endif
#if defined(__aarch64__) || defined(_M_ARM64)
#define SIMD_HOST_NEON 1
#else
#define SIMD_HOST_NEON 0
#endif
#else
#ifndef SIMD_NAMESPACE
#define SIMD_NAMESPACE simd
#endif
#endif
// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
