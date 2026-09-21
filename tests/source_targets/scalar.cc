// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#ifdef NATIVE_TARGETS_METADATA_ONLY
import native_target_metadata;
#else
import native;
import native.scalar;
#endif
#include <native/targets.h>
#define SCALAR_TARGETS(X,...) X(scalar,__VA_ARGS__)
#define SCALAR_BODY(name,tag) \
  template<native::isa<> A> requires(A == tag) \
  int name(int value) { return value+1; }
NATIVE_TARGET_VARIANTS(source_scalar,SCALAR_TARGETS,SCALAR_BODY)
#undef SCALAR_BODY
#define EMPTY_TARGETS(X,...)
static_assert(sizeof(NATIVE_TARGET_LIST(EMPTY_TARGETS))==1);
int scalar_result(int value) { return source_scalar<native::scalar>(value); }
