// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Only public headers and the public hub: no internal policy definitions.
#include <simd/targets.h>
#include <array>
#include <type_traits>
import simd;

SIMD_TARGET_PUSH(avx2)
bool check_import() {
  using V=simd::vec<float,8,simd::avx2>;
  using W=simd::wide<V,2>;
  using F=W (*)(W const &);
  static_assert(static_cast<F>(&simd::exp<false>)==static_cast<F>(&simd::exp<false,8,2,simd::avx2>));
  static_assert(static_cast<F>(&simd::math::exp<false>)==static_cast<F>(&simd::exp<false>));
  static_assert(static_cast<F>(&simd::exp<false>)!=static_cast<F>(&simd::exp<false,V,2>));
  W input{V(0.f),V(0.f)};
  auto output=simd::math::exp(input);
  static_assert(std::same_as<decltype(output),W>);
  std::array<float,8> lanes;
  for(auto const & value:output.registers) {
    value.store(lanes.data());
    for(float lane:lanes) if(lane!=1.f) return false;
  }
  return true;
}
SIMD_TARGET_POP()
int main() {
  auto cpu=simd::observe_x86_capabilities();
  if(!simd::classify_isa(cpu,simd::avx2{},SIMD_TARGET_MINIMUM).admitted()) return 77;
  return check_import()?0:1;
}
