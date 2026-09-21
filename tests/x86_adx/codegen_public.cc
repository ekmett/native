// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <immintrin.h>
#include <native/attributes.h>
import native.x86.adx;
constexpr auto arch = native::target_features<native::x86>("adx");

extern "C" native_noinline native_target("adx")
std::uint8_t native_addcarryx_32(std::uint8_t carry, std::uint32_t a, std::uint32_t b, std::uint32_t * result) noexcept {
  return native::addcarryx<arch>(carry, a, b, result);
}

extern "C" native_noinline native_target("adx")
std::uint8_t native_chain_32(std::uint32_t const * a, std::uint32_t const * b, std::uint32_t * result) noexcept {
  std::uint8_t carry = 0;
  carry = native::addcarryx<arch>(carry, a[0], b[0], result + 0);
  carry = native::addcarryx<arch>(carry, a[1], b[1], result + 1);
  carry = native::addcarryx<arch>(carry, a[2], b[2], result + 2);
  carry = native::addcarryx<arch>(carry, a[3], b[3], result + 3);
  return carry;
}

extern "C" native_noinline native_target("adx")
std::uint8_t native_addcarryx_64(std::uint8_t carry, std::uint64_t a, std::uint64_t b, std::uint64_t * result) noexcept {
  return native::addcarryx<arch>(carry, a, b, result);
}

extern "C" native_noinline native_target("adx")
std::uint8_t native_chain_64(std::uint64_t const * a, std::uint64_t const * b, std::uint64_t * result) noexcept {
  std::uint8_t carry = 0;
  carry = native::addcarryx<arch>(carry, a[0], b[0], result + 0);
  carry = native::addcarryx<arch>(carry, a[1], b[1], result + 1);
  carry = native::addcarryx<arch>(carry, a[2], b[2], result + 2);
  carry = native::addcarryx<arch>(carry, a[3], b[3], result + 3);
  return carry;
}

extern "C" native_noinline unsigned native_adx_baseline(unsigned a, unsigned b) noexcept {
  return (a ^ b) + a;
}
