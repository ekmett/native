// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <concepts>
import native;
template<std::size_t N> constexpr bool shape() {
  using B=native::vec<native::bf16,N,native::avx512_bf16>;
  using F=native::vec<float,N/2,native::avx512_bf16>;
  static_assert(sizeof(B)==2*N);
  static_assert(std::same_as<decltype(native::dot2(B{},B{},F{})),F>);
  return true;
}
static_assert(shape<8>() && shape<16>() && shape<32>());
