// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#ifdef SIMD_TARGETS_METADATA_ONLY
import simd_target_metadata;
#else
import simd;
#endif
#include <simd/targets.h>
#define SCALAR_TARGETS(X,...) X(scalar,__VA_ARGS__)
#define SCALAR_BODY(name,tag) int name(tag,int value) { return value+1; }
SIMD_TARGET_VARIANTS(source_scalar,SCALAR_TARGETS,SCALAR_BODY)
#undef SCALAR_BODY
#define EMPTY_TARGETS(X,...)
static_assert(sizeof(SIMD_TARGET_LIST(EMPTY_TARGETS))==1);
int scalar_result(int value) { return source_scalar(simd::scalar{},value); }
