// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
import native.isa;
#include <native/targets.h>
#if defined(__aarch64__) || defined(_M_ARM64)
#define NATIVE_TARGET_foreign "avx2"
#else
#define NATIVE_TARGET_foreign "neon"
#endif
constexpr auto rejected=NATIVE_TARGET_ISA(foreign);
