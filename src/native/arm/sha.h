// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
/// \cond NATIVE_DETAIL
#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#include <cstdint>
#if NATIVE_HOST_NEON
#include <arm_neon.h>
#endif

#if NATIVE_HOST_NEON || defined(NATIVE_DOXYGEN)
namespace native::detail::arm_sha {

  template<isa Arch> requires(Arch.has(arm_feature::sha1))
  native_nodiscard native_inline native_const native_target("sha2")
  uint32x4_t sha1c(uint32x4_t abcd, std::uint32_t e, uint32x4_t wk) noexcept {
    return vsha1cq_u32(abcd, e, wk);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha1))
  native_nodiscard native_inline native_const native_target("sha2")
  uint32x4_t sha1p(uint32x4_t abcd, std::uint32_t e, uint32x4_t wk) noexcept {
    return vsha1pq_u32(abcd, e, wk);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha1))
  native_nodiscard native_inline native_const native_target("sha2")
  uint32x4_t sha1m(uint32x4_t abcd, std::uint32_t e, uint32x4_t wk) noexcept {
    return vsha1mq_u32(abcd, e, wk);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha1))
  native_nodiscard native_inline native_const native_target("sha2")
  std::uint32_t sha1h(std::uint32_t a) noexcept {
    return vsha1h_u32(a);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha1))
  native_nodiscard native_inline native_const native_target("sha2")
  uint32x4_t sha1su0(uint32x4_t w0_3, uint32x4_t w4_7, uint32x4_t w8_11) noexcept {
    return vsha1su0q_u32(w0_3, w4_7, w8_11);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha1))
  native_nodiscard native_inline native_const native_target("sha2")
  uint32x4_t sha1su1(uint32x4_t partial, uint32x4_t w12_15) noexcept {
    return vsha1su1q_u32(partial, w12_15);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha2))
  native_nodiscard native_inline native_const native_target("sha2")
  uint32x4_t sha256h(uint32x4_t abcd, uint32x4_t efgh, uint32x4_t wk) noexcept {
    return vsha256hq_u32(abcd, efgh, wk);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha2))
  native_nodiscard native_inline native_const native_target("sha2")
  uint32x4_t sha256h2(uint32x4_t efgh, uint32x4_t abcd, uint32x4_t wk) noexcept {
    return vsha256h2q_u32(efgh, abcd, wk);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha2))
  native_nodiscard native_inline native_const native_target("sha2")
  uint32x4_t sha256su0(uint32x4_t w0_3, uint32x4_t w4_7) noexcept {
    return vsha256su0q_u32(w0_3, w4_7);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha2))
  native_nodiscard native_inline native_const native_target("sha2")
  uint32x4_t sha256su1(uint32x4_t partial, uint32x4_t w8_11, uint32x4_t w12_15) noexcept {
    return vsha256su1q_u32(partial, w8_11, w12_15);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha512))
  native_nodiscard native_inline native_const native_target("sha3")
  uint64x2_t sha512h(uint64x2_t sum, uint64x2_t fg, uint64x2_t de) noexcept {
    return vsha512hq_u64(sum, fg, de);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha512))
  native_nodiscard native_inline native_const native_target("sha3")
  uint64x2_t sha512h2(uint64x2_t sum_ab, uint64x2_t c_, uint64x2_t ab) noexcept {
    return vsha512h2q_u64(sum_ab, c_, ab);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha512))
  native_nodiscard native_inline native_const native_target("sha3")
  uint64x2_t sha512su0(uint64x2_t w0_1, uint64x2_t w2_) noexcept {
    return vsha512su0q_u64(w0_1, w2_);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha512))
  native_nodiscard native_inline native_const native_target("sha3")
  uint64x2_t sha512su1(uint64x2_t partial, uint64x2_t w14_15, uint64x2_t w9_10) noexcept {
    return vsha512su1q_u64(partial, w14_15, w9_10);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  uint8x16_t eor3(uint8x16_t a, uint8x16_t b, uint8x16_t c) noexcept {
    return veor3q_u8(a, b, c);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  uint16x8_t eor3(uint16x8_t a, uint16x8_t b, uint16x8_t c) noexcept {
    return veor3q_u16(a, b, c);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  uint32x4_t eor3(uint32x4_t a, uint32x4_t b, uint32x4_t c) noexcept {
    return veor3q_u32(a, b, c);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  uint64x2_t eor3(uint64x2_t a, uint64x2_t b, uint64x2_t c) noexcept {
    return veor3q_u64(a, b, c);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  int8x16_t eor3(int8x16_t a, int8x16_t b, int8x16_t c) noexcept {
    return veor3q_s8(a, b, c);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  int16x8_t eor3(int16x8_t a, int16x8_t b, int16x8_t c) noexcept {
    return veor3q_s16(a, b, c);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  int32x4_t eor3(int32x4_t a, int32x4_t b, int32x4_t c) noexcept {
    return veor3q_s32(a, b, c);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  int64x2_t eor3(int64x2_t a, int64x2_t b, int64x2_t c) noexcept {
    return veor3q_s64(a, b, c);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  uint8x16_t bcax(uint8x16_t a, uint8x16_t b, uint8x16_t c) noexcept {
    return vbcaxq_u8(a, b, c);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  uint16x8_t bcax(uint16x8_t a, uint16x8_t b, uint16x8_t c) noexcept {
    return vbcaxq_u16(a, b, c);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  uint32x4_t bcax(uint32x4_t a, uint32x4_t b, uint32x4_t c) noexcept {
    return vbcaxq_u32(a, b, c);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  uint64x2_t bcax(uint64x2_t a, uint64x2_t b, uint64x2_t c) noexcept {
    return vbcaxq_u64(a, b, c);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  int8x16_t bcax(int8x16_t a, int8x16_t b, int8x16_t c) noexcept {
    return vbcaxq_s8(a, b, c);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  int16x8_t bcax(int16x8_t a, int16x8_t b, int16x8_t c) noexcept {
    return vbcaxq_s16(a, b, c);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  int32x4_t bcax(int32x4_t a, int32x4_t b, int32x4_t c) noexcept {
    return vbcaxq_s32(a, b, c);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  int64x2_t bcax(int64x2_t a, int64x2_t b, int64x2_t c) noexcept {
    return vbcaxq_s64(a, b, c);
  }

  template<isa Arch> requires(Arch.has(arm_feature::sha3))
  native_nodiscard native_inline native_const native_target("sha3")
  uint64x2_t rax1(uint64x2_t a, uint64x2_t b) noexcept {
    return vrax1q_u64(a, b);
  }

  template<isa Arch, unsigned Rotate> requires(Arch.has(arm_feature::sha3) && Rotate < 64)
  native_nodiscard native_inline native_const native_target("sha3")
  uint64x2_t xar(uint64x2_t a, uint64x2_t b) noexcept {
    return vxarq_u64(a, b, Rotate);
  }

  // Reject Clang's implicit same-size vector conversions and scalar narrowing.
  template<isa Arch, class... T> void sha1c(T...) = delete;

  template<isa Arch, class... T> void sha1p(T...) = delete;

  template<isa Arch, class... T> void sha1m(T...) = delete;

  template<isa Arch, class... T> void sha1h(T...) = delete;

  template<isa Arch, class... T> void sha1su0(T...) = delete;

  template<isa Arch, class... T> void sha1su1(T...) = delete;

  template<isa Arch, class... T> void sha256h(T...) = delete;

  template<isa Arch, class... T> void sha256h2(T...) = delete;

  template<isa Arch, class... T> void sha256su0(T...) = delete;

  template<isa Arch, class... T> void sha256su1(T...) = delete;

  template<isa Arch, class... T> void sha512h(T...) = delete;

  template<isa Arch, class... T> void sha512h2(T...) = delete;

  template<isa Arch, class... T> void sha512su0(T...) = delete;

  template<isa Arch, class... T> void sha512su1(T...) = delete;

  template<isa Arch, class... T> void eor3(T...) = delete;

  template<isa Arch, class... T> void bcax(T...) = delete;

  template<isa Arch, class... T> void rax1(T...) = delete;

  template<isa Arch, unsigned Rotate, class... T> void xar(T...) = delete;

}
#endif
/// \endcond
