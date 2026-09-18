// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <simd/isa.h>
using invalid=simd::target_entry<simd::avx2,1ull<<63>;
using policies=simd::isa_list<invalid,simd::avx2>;
constexpr auto chosen=simd::target<simd::avx2,policies>;
