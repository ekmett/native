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
#ifdef NATIVE_TEST_CODEGEN_POLYFILL
constexpr auto byte512_arch = (native::isa<native::x86>{native::x86_feature::avx512f} &
  native::x86_feature::gfni) | native::polyfill;
static_assert(!byte512_arch.has(native::x86_feature::avx512bw));
static_assert(!byte512_arch.has(native::x86_feature::avx));
#else
constexpr auto byte512_arch = native::feature_closure(native::isa<native::x86>{
  native::x86_feature::avx512f} & native::x86_feature::avx512bw & native::x86_feature::gfni);
#endif
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
  // Storage is AVX512F-sized even though byte arithmetic and lower carrier flags
  // are absent from the permitted tag. Available GFNI must remain native.
  [[gnu::target("avx512f,avx512bw,gfni")]] void native_gfni_storage_512(
    std::uint8_t const * a, std::uint8_t const * b, std::uint8_t * out) {
    using V = native::simd<std::uint8_t, 64, byte512_arch>;
    static_assert(sizeof(typename V::native_type) == 64);
    native::gf2p8mulb<byte512_arch>(V::load(a), V::load(b)).store(out);
  }
}
