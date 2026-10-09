// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.x86.bmi1;
import native.x86.bmi2;
import native.x86.popcnt;
import native.x86.lzcnt;
import native.x86.crc32c;
import native.x86.adx;

#ifdef NATIVE_CODEGEN_POLYFILL
#define ARCH(feature) (native::isa<native::x86>{native::x86_feature::feature} | native::polyfill)
#else
#define ARCH(feature) native::isa<native::x86>{native::x86_feature::feature}
#endif
extern "C" {
  __attribute__((target("bmi"))) std::uint64_t native_andn(std::uint64_t a, std::uint64_t b) {
    return native::andn<ARCH(bmi1)>(a, b);
  }
  __attribute__((target("bmi2"))) std::uint64_t native_pext(std::uint64_t a, std::uint64_t b) {
    return native::pext<ARCH(bmi2)>(a, b);
  }
  __attribute__((target("bmi2"))) std::uint64_t native_pdep(std::uint64_t a, std::uint64_t b) {
    return native::pdep<ARCH(bmi2)>(a, b);
  }
  __attribute__((target("bmi2"))) std::uint64_t native_mulx(std::uint64_t a, std::uint64_t b, std::uint64_t * high) {
    return native::mulx<ARCH(bmi2)>(a, b, high);
  }
  __attribute__((target("popcnt"))) std::uint64_t native_popcnt(std::uint64_t a) {
    return native::popcnt<ARCH(popcnt)>(a);
  }
  __attribute__((target("lzcnt"))) std::uint64_t native_lzcnt(std::uint64_t a) {
    return native::lzcnt<ARCH(lzcnt)>(a);
  }
  __attribute__((target("crc32"))) std::uint32_t native_crc(std::uint32_t a, std::uint64_t b) {
    return native::crc32c<ARCH(crc32)>(a, b);
  }
  __attribute__((target("adx"))) std::uint8_t native_addcarry(
    std::uint8_t c, std::uint64_t a, std::uint64_t b, std::uint64_t * out) {
    return native::addcarryx<ARCH(adx)>(c, a, b, out);
  }
}
#undef ARCH
