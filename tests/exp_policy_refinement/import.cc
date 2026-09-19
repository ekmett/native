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
// Public literal scopes deliberately omit both half features while the type
// retains them. This catches a stronger attribute hidden behind the export.
#define SIMD_TARGET_import_bw "avx2,fma,bmi2,avx512f,avx512dq,avx512bw"
#define EMIT_HALF_TAG_IMPORT(name) \
  SIMD_TARGET_PUSH(name) \
  bool check_half_tag_##name() { \
    constexpr auto A=simd::feature_closure(SIMD_TARGET_ISA(name)&simd::feature::avx512bf16&simd::feature::avx512fp16); \
    using V=simd::vec<float,8,A>; \
    using W=simd::wide<V,2>; \
    using F=W (*)(W const &); \
    static_assert(static_cast<F>(&simd::exp<false>)==static_cast<F>(&simd::exp<false,8,2,A>)); \
    static_assert(static_cast<F>(&simd::math::exp<false>)==static_cast<F>(&simd::exp<false>)); \
    static_assert(static_cast<F>(&simd::exp<false>)!=static_cast<F>(&simd::exp<false,V,2>)); \
    W input{V(0.f),V(0.f)}; \
    static_assert(noexcept(simd::exp(input))==noexcept(simd::exp<false,V,2>(input))); \
    auto output=simd::math::exp(input); \
    static_assert(std::same_as<decltype(output),W>); \
    std::array<float,8> lanes; \
    for(auto const & value:output.registers) { \
      value.store(lanes.data()); \
      for(float lane:lanes) if(lane!=1.f) return false; \
    } \
    return true; \
  } \
  SIMD_TARGET_POP()
EMIT_HALF_TAG_IMPORT(import_bw)
EMIT_HALF_TAG_IMPORT(avx512)
#undef EMIT_HALF_TAG_IMPORT

int main() {
  auto cpu=simd::observe_x86_capabilities();
  if(!simd::classify_isa(cpu,simd::avx2,SIMD_TARGET_MINIMUM).admitted()) return 77;
  if(!check_import()) return 1;
  if(simd::classify_isa(cpu,SIMD_TARGET_ISA(import_bw),SIMD_TARGET_MINIMUM).admitted() &&
      !check_half_tag_import_bw()) return 2;
  if(simd::classify_isa(cpu,simd::avx512,SIMD_TARGET_MINIMUM).admitted() &&
      !check_half_tag_avx512()) return 3;
  return 0;
}
