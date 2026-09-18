#pragma once
#include "simd/config.h"
#include "simd/simd/common.h"
#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <cmath>
#include <cstring>
#include <span>
#include <limits>
#if SIMD_HOST_X86
#include <immintrin.h>
#endif
#if SIMD_HOST_NEON
#include <arm_neon.h>
#endif
#define SIMD_BACKEND_BODY "simd/simd/vec_family.h"
#include "simd/simd/for_each_backend.h"
#undef SIMD_BACKEND_BODY

#if SIMD_HOST_X86 && (!defined(SIMD_PROFILE) || SIMD_PROFILE != 0)
#define SIMD_COMMON_ARCH_CONCEPT ::simd::detail::memory_avx512_bf16_architecture
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_9))), apply_to=function)
#include "simd/simd/common_body.h"
#pragma clang attribute pop
#undef SIMD_COMMON_ARCH_CONCEPT
#define SIMD_COMMON_ARCH_CONCEPT ::simd::detail::memory_avx512_fp16_architecture
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_13))), apply_to=function)
#include "simd/simd/common_body.h"
#pragma clang attribute pop
#undef SIMD_COMMON_ARCH_CONCEPT
#define SIMD_COMMON_ARCH_CONCEPT ::simd::detail::memory_avx512_half_architecture
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_17))), apply_to=function)
#include "simd/simd/common_body.h"
#pragma clang attribute pop
#undef SIMD_COMMON_ARCH_CONCEPT
#endif

#if SIMD_HOST_NEON && (!defined(SIMD_PROFILE) || SIMD_PROFILE != 0)
#define SIMD_COMMON_ARCH_CONCEPT ::simd::detail::memory_neon_bf16_architecture
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_21))), apply_to=function)
#include "simd/simd/common_body.h"
#pragma clang attribute pop
#undef SIMD_COMMON_ARCH_CONCEPT
#define SIMD_COMMON_ARCH_CONCEPT ::simd::detail::memory_neon_fp16_architecture
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_22))), apply_to=function)
#include "simd/simd/common_body.h"
#pragma clang attribute pop
#undef SIMD_COMMON_ARCH_CONCEPT
#define SIMD_COMMON_ARCH_CONCEPT ::simd::detail::memory_neon_half_architecture
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_23))), apply_to=function)
#include "simd/simd/common_body.h"
#pragma clang attribute pop
#undef SIMD_COMMON_ARCH_CONCEPT
#endif
