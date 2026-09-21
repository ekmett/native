#pragma once
#include "native/config.h"
#include "native/simd/common.h"
#include "native/simd/float_constant.h"
#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <cmath>
#include <cstring>
#include <span>
#include <limits>
#if NATIVE_HOST_WASM
#include <wasm_simd128.h>
#endif
#if NATIVE_HOST_X86
#include <immintrin.h>
#endif
#if NATIVE_HOST_NEON
#include <arm_neon.h>
#endif
#define NATIVE_BACKEND_BODY "native/simd/vec_family.h"
#include "native/simd/for_each_backend.h"
#undef NATIVE_BACKEND_BODY

#if NATIVE_HOST_X86 && (!defined(NATIVE_PROFILE) || NATIVE_PROFILE != 0)
#define NATIVE_COMMON_ARCH(...) (::native::abi_lookup<__VA_ARGS__,::native::detail::memory_kernel_policies>::index == 1)
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_9))), apply_to=function)
#include "native/simd/common_body.h"
#pragma clang attribute pop
#undef NATIVE_COMMON_ARCH
#define NATIVE_COMMON_ARCH(...) (::native::abi_lookup<__VA_ARGS__,::native::detail::memory_kernel_policies>::index == 2)
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_13))), apply_to=function)
#include "native/simd/common_body.h"
#pragma clang attribute pop
#undef NATIVE_COMMON_ARCH
#define NATIVE_COMMON_ARCH(...) (::native::abi_lookup<__VA_ARGS__,::native::detail::memory_kernel_policies>::index == 0)
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_17))), apply_to=function)
#include "native/simd/common_body.h"
#pragma clang attribute pop
#undef NATIVE_COMMON_ARCH
#endif

#if NATIVE_HOST_NEON && (!defined(NATIVE_PROFILE) || NATIVE_PROFILE != 0)
#define NATIVE_COMMON_ARCH(...) (::native::abi_lookup<__VA_ARGS__,::native::detail::memory_kernel_policies>::index == 1)
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_21))), apply_to=function)
#include "native/simd/common_body.h"
#pragma clang attribute pop
#undef NATIVE_COMMON_ARCH
#define NATIVE_COMMON_ARCH(...) (::native::abi_lookup<__VA_ARGS__,::native::detail::memory_kernel_policies>::index == 2)
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_22))), apply_to=function)
#include "native/simd/common_body.h"
#pragma clang attribute pop
#undef NATIVE_COMMON_ARCH
#define NATIVE_COMMON_ARCH(...) (::native::abi_lookup<__VA_ARGS__,::native::detail::memory_kernel_policies>::index == 0)
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_23))), apply_to=function)
#include "native/simd/common_body.h"
#pragma clang attribute pop
#undef NATIVE_COMMON_ARCH
#endif
