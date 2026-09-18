// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <simd/isa.h>
#include <simd/targets.h>

static_assert(SIMD_TARGET_ISA(avx2) == simd::avx2);
static_assert(SIMD_TARGET_ISA(neon) == simd::neon);

template<simd::isa A> requires(A == SIMD_TARGET_ISA(avx2))
constexpr int repeated();
template<simd::isa A> requires(A == SIMD_TARGET_ISA(avx2))
constexpr int repeated() { return 7; }
static_assert(repeated<simd::avx2>() == 7);

#define METADATA_TARGETS(X, ...) X(scalar, __VA_ARGS__)
#define METADATA_BODY(name, arch) \
  template<simd::isa A> requires(A == arch) \
  int name(int value) { return value + 1; }
SIMD_TARGET_VARIANTS(increment, METADATA_TARGETS, METADATA_BODY)

int main() { return increment<simd::scalar>(41) == 42 ? 0 : 1; }
