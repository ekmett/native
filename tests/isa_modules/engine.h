// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

#pragma once

#include <cstddef>
#include <array>
#include <concepts>
#include <utility>
#include "../../src/simd/isa.h"

namespace simd {
  template<class T,std::size_t N,isa Arch> struct vec;
}
namespace fixture {
  // One ordinary definition, instantiated in each selected-ISA translation unit.
  // There is no source reinclusion or namespace rewriting in consumer code.
  template<simd::isa Arch, std::size_t Lanes>
  struct engine {
    using value_type = simd::vec<float,Lanes,Arch>;

    static inline __attribute__((always_inline)) void run(
      float const * a, float const * b, float * output) {
      for (std::size_t i = 0; i < 48; i += Lanes) {
        auto x = value_type::load(a + i);
        auto y = value_type::load(b + i);
        // ADL selects the backend's native fused operation.
        fma(x, y, x + y).store(output + i);
      }
    }
  };
  template<class R,std::size_t M> struct wide { std::array<R,M> registers; };
  template<class R,std::size_t M,std::size_t... I>
  inline __attribute__((always_inline)) wide<R,M> add_wide(wide<R,M> const & a,wide<R,M> const & b,std::index_sequence<I...>) {
    return {{(a.registers[I]+b.registers[I])...}};
  }
  template<class R,std::size_t M>
  inline __attribute__((always_inline)) wide<R,M> operator+(wide<R,M> const & a,wide<R,M> const & b) {
    return add_wide(a,b,std::make_index_sequence<M>{});
  }
}
