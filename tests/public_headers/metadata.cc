// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <simd/isa.h>
#include <simd/targets.h>

static_assert(std::same_as<SIMD_TARGET_TYPE(avx2), simd::avx2>);
static_assert(std::same_as<SIMD_TARGET_TYPE(neon), simd::neon>);

#define METADATA_TARGETS(X, ...) X(scalar, __VA_ARGS__)
#define METADATA_BODY(name, tag) int name(tag, int value) { return value + 1; }
SIMD_TARGET_VARIANTS(increment, METADATA_TARGETS, METADATA_BODY)

int main() { return increment(simd::scalar{}, 41) == 42 ? 0 : 1; }
