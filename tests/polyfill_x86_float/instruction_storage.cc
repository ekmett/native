// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.x86.f16c;
#ifdef NATIVE_TEST_CODEGEN_POLYFILL
constexpr auto storage_arch = native::isa<native::x86>{native::x86_feature::f16c} | native::polyfill;
#else
constexpr auto storage_arch = native::feature_closure(native::isa<native::x86>{native::x86_feature::f16c});
#endif
extern "C" {
  [[gnu::target("f16c")]] void native_f16c_storage_narrow4(float const * in, std::uint16_t * out) {
    native::cvtps_ph<storage_arch, 0>(native::simd<float, 4, storage_arch>::load(in)).store_bits(out);
  }
  [[gnu::target("f16c")]] void native_f16c_storage_narrow8(float const * in, std::uint16_t * out) {
    native::cvtps_ph<storage_arch, 0>(native::simd<float, 8, storage_arch>::load(in)).store_bits(out);
  }
  [[gnu::target("f16c")]] void native_f16c_storage_widen4(std::uint16_t const * in, float * out) {
    native::cvtph_ps<storage_arch, 4>(native::simd<native::fp16, 4, storage_arch>::load_bits(in)).store(out);
  }
  [[gnu::target("f16c")]] void native_f16c_storage_widen8(std::uint16_t const * in, float * out) {
    native::cvtph_ps<storage_arch, 8>(native::simd<native::fp16, 8, storage_arch>::load_bits(in)).store(out);
  }
}
