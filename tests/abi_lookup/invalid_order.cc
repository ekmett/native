// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <simd/isa.h>
using namespace simd;
#if INVALID_ORDER == 0
constexpr int chosen=target<avx512,avx2,avx512>;
#elif INVALID_ORDER == 1
constexpr int chosen=target<neon,avx2,avx512>;
#elif INVALID_ORDER == 2
constexpr int chosen=target<avx2,avx2,avx2>;
#elif INVALID_ORDER == 3
constexpr int chosen=target<avx512,avx2,neon,avx512>;
#elif INVALID_ORDER == 4
constexpr int chosen=target<avx2,scalar,avx2>;
#endif
