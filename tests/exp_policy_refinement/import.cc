// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Only public headers and the public hub: no internal policy definitions.
#include <native/targets.h>
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <type_traits>
import native;
import native.math;

NATIVE_TARGET_PUSH(avx2)
bool check_import() {
  using V=native::simd<float,8,native::avx2>;
  using W=native::wide<V,2>;
  using F=W (*)(W const &);
  static_assert(static_cast<F>(&native::exp<false>)==static_cast<F>(&native::exp<false,6,8,2,native::avx2>));
  static_assert(static_cast<F>(&native::math::exp<false>)==static_cast<F>(&native::exp<false>));
  static_assert(static_cast<F>(&native::exp<false>)!=static_cast<F>(&native::exp<false,6,V,2>));
  W input{V(0.f),V(0.f)};
  auto output=native::math::exp(input);
  static_assert(std::same_as<decltype(output),W>);
  std::array<float,8> lanes;
  for(auto const & value:output.registers) {
    value.store(lanes.data());
    for(float lane:lanes) if(lane!=1.f) return false;
  }
  constexpr std::array<std::uint32_t,8> edge{
    0x42b0c0a5,0x42b0c0a6,0x42b17214,0x42b17215,0x42b17216,0x42b17217,0x42b17218,0x42b17219};
  constexpr std::array<std::uint32_t,4> historical{0x7f7ffe04,0x7f7ffe84,0x7f7fff04,0x7f7fff84};
  auto input_edge=V::load_bits(edge.data());
  for(auto candidate:{native::exp<false,6>(input_edge),native::exp<true,6>(input_edge),
                      native::exp<false,7>(input_edge),native::exp<true,7>(input_edge)}) {
    std::array<std::uint32_t,8> words;candidate.store_bits(words.data());
    for(unsigned lane=0;lane<8;++lane) {
      double exact_sample=std::exp(double(std::bit_cast<float>(edge[lane])));
      // The two adjacent binary32 inputs straddle the overflow midpoint with
      // much more margin than binary64 libm error; no tolerance is used for bits.
      bool finite=exact_sample < 0x1.ffffffp127;
      if(finite!=(words[lane]<0x7f800000u))return false;
      if(lane>=2&&lane<6&&words[lane]!=historical[lane-2])return false;
    }
  }
  return true;
}
NATIVE_TARGET_POP()
// Public literal scopes deliberately omit both half features while the type
// retains them. This catches a stronger attribute hidden behind the export.
#define NATIVE_TARGET_import_bw "avx2,fma,avx512f,avx512dq,avx512bw"
#define EMIT_HALF_TAG_IMPORT(name) \
  NATIVE_TARGET_PUSH(name) \
  bool check_half_tag_##name() { \
    constexpr auto A=native::feature_closure(NATIVE_TARGET_ISA(name)&native::x86_feature::avx512bf16&native::x86_feature::avx512fp16); \
    using V=native::simd<float,8,A>; \
    using W=native::wide<V,2>; \
    using F=W (*)(W const &); \
    static_assert(static_cast<F>(&native::exp<false>)==static_cast<F>(&native::exp<false,6,8,2,A>)); \
    static_assert(static_cast<F>(&native::math::exp<false>)==static_cast<F>(&native::exp<false>)); \
    static_assert(static_cast<F>(&native::exp<false>)!=static_cast<F>(&native::exp<false,6,V,2>)); \
    W input{V(0.f),V(0.f)}; \
    static_assert(noexcept(native::exp(input))==noexcept(native::exp<false,6,V,2>(input))); \
    auto output=native::math::exp(input); \
    static_assert(std::same_as<decltype(output),W>); \
    std::array<float,8> lanes; \
    for(auto const & value:output.registers) { \
      value.store(lanes.data()); \
      for(float lane:lanes) if(lane!=1.f) return false; \
    } \
    return true; \
  } \
  NATIVE_TARGET_POP()
EMIT_HALF_TAG_IMPORT(import_bw)
EMIT_HALF_TAG_IMPORT(avx512)
#undef EMIT_HALF_TAG_IMPORT

int main() {
  auto cpu=native::observe_x86_capabilities();
  if(!native::classify_isa(cpu,native::avx2,NATIVE_TARGET_MINIMUM).admitted()) return 77;
  if(!check_import()) return 1;
  if(native::classify_isa(cpu,NATIVE_TARGET_ISA(import_bw),NATIVE_TARGET_MINIMUM).admitted() &&
      !check_half_tag_import_bw()) return 2;
  if(native::classify_isa(cpu,native::avx512,NATIVE_TARGET_MINIMUM).admitted() &&
      !check_half_tag_avx512()) return 3;
  return 0;
}
