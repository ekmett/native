// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <arm_acle.h>
#include <arm_neon.h>
#include <native/arm/rdm.h>
#include <native/arm/pmull.h>
#include <native/arm/sha.h>
#include <cstdint>
import native.arm.crc;
import native.arm.rdm;
import native.arm.pmull;
import native.arm.sha;
constexpr auto crc = native::feature_closure(native::arm_feature::crc);
constexpr auto rdm = native::feature_closure(native::arm_feature::rdm);
constexpr auto pmull = native::feature_closure(native::arm_feature::pmull);
constexpr auto sha1 = native::feature_closure(native::arm_feature::sha1);

extern "C" __attribute__((noinline)) std::uint32_t native_default_crc32_8(std::uint32_t a,std::uint8_t b) {
  return native::crc32(a,b);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_crc32_8_explicit(std::uint32_t a,std::uint8_t b) {
  return native::crc32<crc>(a,b);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_crc32_8_raw(std::uint32_t a,std::uint8_t b) {
  return __crc32b(a,b);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_crc32_16(std::uint32_t a,std::uint16_t b) {
  return native::crc32(a,b);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_crc32_16_explicit(std::uint32_t a,std::uint16_t b) {
  return native::crc32<crc>(a,b);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_crc32_16_raw(std::uint32_t a,std::uint16_t b) {
  return __crc32h(a,b);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_crc32_32(std::uint32_t a,std::uint32_t b) {
  return native::crc32(a,b);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_crc32_32_explicit(std::uint32_t a,std::uint32_t b) {
  return native::crc32<crc>(a,b);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_crc32_32_raw(std::uint32_t a,std::uint32_t b) {
  return __crc32w(a,b);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_crc32_64(std::uint32_t a,std::uint64_t b) {
  return native::crc32(a,b);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_crc32_64_explicit(std::uint32_t a,std::uint64_t b) {
  return native::crc32<crc>(a,b);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_crc32_64_raw(std::uint32_t a,std::uint64_t b) {
  return __crc32d(a,b);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_crc32c_8(std::uint32_t a,std::uint8_t b) {
  return native::crc32c(a,b);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_crc32c_8_explicit(std::uint32_t a,std::uint8_t b) {
  return native::crc32c<crc>(a,b);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_crc32c_8_raw(std::uint32_t a,std::uint8_t b) {
  return __crc32cb(a,b);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_crc32c_16(std::uint32_t a,std::uint16_t b) {
  return native::crc32c(a,b);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_crc32c_16_explicit(std::uint32_t a,std::uint16_t b) {
  return native::crc32c<crc>(a,b);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_crc32c_16_raw(std::uint32_t a,std::uint16_t b) {
  return __crc32ch(a,b);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_crc32c_32(std::uint32_t a,std::uint32_t b) {
  return native::crc32c(a,b);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_crc32c_32_explicit(std::uint32_t a,std::uint32_t b) {
  return native::crc32c<crc>(a,b);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_crc32c_32_raw(std::uint32_t a,std::uint32_t b) {
  return __crc32cw(a,b);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_crc32c_64(std::uint32_t a,std::uint64_t b) {
  return native::crc32c(a,b);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_crc32c_64_explicit(std::uint32_t a,std::uint64_t b) {
  return native::crc32c<crc>(a,b);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_crc32c_64_raw(std::uint32_t a,std::uint64_t b) {
  return __crc32cd(a,b);
}
extern "C" __attribute__((noinline)) std::int16_t native_default_sqrdmlah_16(std::int16_t a,std::int16_t b,std::int16_t c) {
  return native::sqrdmlah(a,b,c);
}
extern "C" __attribute__((noinline)) std::int16_t native_default_sqrdmlah_16_explicit(std::int16_t a,std::int16_t b,std::int16_t c) {
  return native::sqrdmlah<rdm>(a,b,c);
}
extern "C" __attribute__((noinline)) std::int16_t native_default_sqrdmlah_16_raw(std::int16_t a,std::int16_t b,std::int16_t c) {
  return native::detail::sqrdmlah<rdm>(a,b,c);
}
extern "C" __attribute__((noinline)) std::int32_t native_default_sqrdmlah_32(std::int32_t a,std::int32_t b,std::int32_t c) {
  return native::sqrdmlah(a,b,c);
}
extern "C" __attribute__((noinline)) std::int32_t native_default_sqrdmlah_32_explicit(std::int32_t a,std::int32_t b,std::int32_t c) {
  return native::sqrdmlah<rdm>(a,b,c);
}
extern "C" __attribute__((noinline)) std::int32_t native_default_sqrdmlah_32_raw(std::int32_t a,std::int32_t b,std::int32_t c) {
  return native::detail::sqrdmlah<rdm>(a,b,c);
}
extern "C" __attribute__((noinline)) std::int16_t native_default_sqrdmlsh_16(std::int16_t a,std::int16_t b,std::int16_t c) {
  return native::sqrdmlsh(a,b,c);
}
extern "C" __attribute__((noinline)) std::int16_t native_default_sqrdmlsh_16_explicit(std::int16_t a,std::int16_t b,std::int16_t c) {
  return native::sqrdmlsh<rdm>(a,b,c);
}
extern "C" __attribute__((noinline)) std::int16_t native_default_sqrdmlsh_16_raw(std::int16_t a,std::int16_t b,std::int16_t c) {
  return native::detail::sqrdmlsh<rdm>(a,b,c);
}
extern "C" __attribute__((noinline)) std::int32_t native_default_sqrdmlsh_32(std::int32_t a,std::int32_t b,std::int32_t c) {
  return native::sqrdmlsh(a,b,c);
}
extern "C" __attribute__((noinline)) std::int32_t native_default_sqrdmlsh_32_explicit(std::int32_t a,std::int32_t b,std::int32_t c) {
  return native::sqrdmlsh<rdm>(a,b,c);
}
extern "C" __attribute__((noinline)) std::int32_t native_default_sqrdmlsh_32_raw(std::int32_t a,std::int32_t b,std::int32_t c) {
  return native::detail::sqrdmlsh<rdm>(a,b,c);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_sha1h(std::uint32_t a) {
  return native::sha1h(a);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_sha1h_explicit(std::uint32_t a) {
  return native::sha1h<sha1>(a);
}
extern "C" __attribute__((noinline)) std::uint32_t native_default_sha1h_raw(std::uint32_t a) {
  return native::detail::arm_sha::sha1h<sha1>(a);
}
extern "C" __attribute__((noinline)) uint64x2_t native_default_pmull(std::uint64_t a,std::uint64_t b) {
  return __builtin_bit_cast(uint64x2_t,native::pmull(a,b).to_native());
}
extern "C" __attribute__((noinline)) uint64x2_t native_default_pmull_explicit(std::uint64_t a,std::uint64_t b) {
  return __builtin_bit_cast(uint64x2_t,native::pmull<pmull>(a,b).to_native());
}
extern "C" __attribute__((noinline)) uint64x2_t native_default_pmull_raw(std::uint64_t a,std::uint64_t b) {
  return __builtin_bit_cast(uint64x2_t,native::detail::arm_pmull::pmull<pmull>(a,b));
}
