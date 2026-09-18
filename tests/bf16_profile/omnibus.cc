// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <concepts>
import simd;
template<std::size_t N> constexpr bool shape() {
  using B=simd::vec<simd::bf16,N,simd::avx512_bf16>;
  using F=simd::vec<float,N/2,simd::avx512_bf16>;
  static_assert(sizeof(B)==2*N);
  static_assert(std::same_as<decltype(simd::dot2(B{},B{},F{})),F>);
  return true;
}
static_assert(shape<8>() && shape<16>() && shape<32>());
