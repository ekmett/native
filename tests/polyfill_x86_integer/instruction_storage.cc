// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.x86.aes;
import native.x86.pclmul;
import native.x86.gfni;

template<native::x86_feature Feature> constexpr auto instruction_arch=[] {
#ifdef NATIVE_TEST_CODEGEN_POLYFILL
  return native::isa<native::x86>{Feature} | native::polyfill;
#else
  return native::feature_closure(native::isa<native::x86>{Feature});
#endif
}();
extern "C" {
  [[gnu::target("aes")]] void native_aes_storage(
    std::uint8_t const * a,std::uint8_t const * b,std::uint8_t * out) {
    constexpr auto arch=instruction_arch<native::x86_feature::aes>;
    using V=native::simd<std::uint8_t,16,arch>;
    native::aesenc<arch>(V::load(a),V::load(b)).store(out);
  }
  [[gnu::target("pclmul")]] void native_pclmul_storage(
    std::uint64_t const * a,std::uint64_t const * b,std::uint64_t * out) {
    constexpr auto arch=instruction_arch<native::x86_feature::pclmul>;
    using V=native::simd<std::uint64_t,2,arch>;
    native::pclmulqdq<arch,17>(V::load(a),V::load(b)).store(out);
  }
  [[gnu::target("gfni")]] void native_gfni_storage(
    std::uint8_t const * a,std::uint8_t const * b,std::uint8_t * out) {
    constexpr auto arch=instruction_arch<native::x86_feature::gfni>;
    using V=native::simd<std::uint8_t,16,arch>;
    native::gf2p8mulb<arch>(V::load(a),V::load(b)).store(out);
  }
}
