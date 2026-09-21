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
  /// Instruction-set families; an ISA value belongs to exactly one family.
  enum class architecture { x86, arm, wasm, unknown };
  /// Select x86 feature metadata independently of the compilation target.
  inline constexpr architecture x86=architecture::x86;
  /// Select ARM feature metadata independently of the compilation target.
  inline constexpr architecture arm=architecture::arm;
  /// Select WebAssembly feature metadata independently of the compilation target.
  inline constexpr architecture wasm=architecture::wasm;
  /// The compiler target's family; no optional instruction support is implied.
  inline constexpr architecture target_arch=
#if defined(__arm__) || defined(__aarch64__) || defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC)
    arm;
#elif defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64)
    x86;
#elif defined(__wasm__) || defined(__wasm32__) || defined(__wasm64__)
    wasm;
#else
    architecture::unknown;
#endif
}
#else
#ifndef NATIVE_NAMESPACE
#define NATIVE_NAMESPACE native
#endif
#endif
// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
