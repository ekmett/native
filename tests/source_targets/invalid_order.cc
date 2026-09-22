// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/targets.h>
import native;

#if defined(NATIVE_TARGET_DUPLICATE)
#define INVALID_TARGETS scalar,scalar
#elif defined(__aarch64__) || defined(_M_ARM64)
#define INVALID_TARGETS scalar,neon
#else
#define INVALID_TARGETS scalar,avx2
#endif

#define INVALID_BODY(name,ISA,...) \
  template<native::isa<> A> \
    requires(native::target<A,__VA_ARGS__> == native::target<ISA,__VA_ARGS__>) \
  int name() { return 0; }
NATIVE_TARGET_VARIANTS(invalid_order,INVALID_BODY,INVALID_TARGETS)

int reject() { return invalid_order<native::scalar>(); }
