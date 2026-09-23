// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Only public headers and the public hub: no internal policy definitions.
#include <native/targets.h>
#include <array>
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
