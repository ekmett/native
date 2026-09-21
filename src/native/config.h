#pragma once
// Host architecture controls declaration availability. Function target scopes
// select instruction requirements independently of the producer's minimum ISA.
#ifdef __cplusplus
#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_HOST_X86 1
#else
#define NATIVE_HOST_X86 0
#endif
#if defined(__aarch64__) || defined(_M_ARM64)
#define NATIVE_HOST_NEON 1
#else
#define NATIVE_HOST_NEON 0
#endif

namespace native {
  /// The compilation target belongs to the ARM family, independently of SIMD support.
  inline constexpr bool is_arm =
#if defined(__arm__) || defined(__aarch64__) || defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC)
    true;
#else
    false;
#endif
  /// The compilation target belongs to the x86 family; no optional ISA is implied.
  inline constexpr bool is_x86 =
#if (defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64)) && !defined(_M_ARM64EC)
    true;
#else
    false;
#endif
  /// The compilation target is WebAssembly, independently of engine capabilities.
  inline constexpr bool is_wasm =
#if defined(__wasm__) || defined(__wasm32__) || defined(__wasm64__)
    true;
#else
    false;
#endif
}
#else
#ifndef NATIVE_NAMESPACE
#define NATIVE_NAMESPACE native
#endif
#endif
// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
