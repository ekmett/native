// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
#include <native/arm/aes.h>
#include <native/arm/pmull.h>
#include <native/arm/sha.h>
import native.arm.aes;
import native.arm.pmull;
import native.arm.sha;
#include "simd_bridge.h"

extern "C" __attribute__((noinline, target("aes"))) uint8x16_t native_crypto_aes_0(uint8x16_t state, uint8x16_t round_key) {
  return instruction_fixture::aese<native::feature_closure(native::isa<native::arm>{native::arm_feature::aes})>(state, round_key);
}
extern "C" __attribute__((noinline, target("aes"))) uint8x16_t native_crypto_aes_1(uint8x16_t state, uint8x16_t round_key) {
  return instruction_fixture::aesd<native::feature_closure(native::isa<native::arm>{native::arm_feature::aes})>(state, round_key);
}
extern "C" __attribute__((noinline, target("aes"))) uint8x16_t native_crypto_aes_2(uint8x16_t state) {
  return instruction_fixture::aesmc<native::feature_closure(native::isa<native::arm>{native::arm_feature::aes})>(state);
}
extern "C" __attribute__((noinline, target("aes"))) uint8x16_t native_crypto_aes_3(uint8x16_t state) {
  return instruction_fixture::aesimc<native::feature_closure(native::isa<native::arm>{native::arm_feature::aes})>(state);
}
extern "C" __attribute__((noinline, target("aes"))) poly128_t native_crypto_pmull_0(poly64_t a, poly64_t b) {
  return instruction_fixture::pmull<native::feature_closure(native::isa<native::arm>{native::arm_feature::pmull})>(a, b);
}
extern "C" __attribute__((noinline, target("aes"))) poly128_t native_crypto_pmull_1(poly64x2_t a, poly64x2_t b) {
  return instruction_fixture::pmull2<native::feature_closure(native::isa<native::arm>{native::arm_feature::pmull})>(a, b);
}
extern "C" __attribute__((noinline)) poly16x8_t native_crypto_pmull_2(poly8x8_t a, poly8x8_t b) {
  return instruction_fixture::pmull<native::feature_closure(native::isa<native::arm>{native::arm_feature::neon})>(a, b);
}
extern "C" __attribute__((noinline)) poly16x8_t native_crypto_pmull_3(poly8x16_t a, poly8x16_t b) {
  return instruction_fixture::pmull2<native::feature_closure(native::isa<native::arm>{native::arm_feature::neon})>(a, b);
}
extern "C" __attribute__((noinline, target("sha2"))) uint32x4_t native_crypto_sha_0(uint32x4_t abcd, std::uint32_t e, uint32x4_t wk) {
  return instruction_fixture::sha1c<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1})>(abcd, e, wk);
}
extern "C" __attribute__((noinline, target("sha2"))) uint32x4_t native_crypto_sha_1(uint32x4_t abcd, std::uint32_t e, uint32x4_t wk) {
  return instruction_fixture::sha1p<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1})>(abcd, e, wk);
}
extern "C" __attribute__((noinline, target("sha2"))) uint32x4_t native_crypto_sha_2(uint32x4_t abcd, std::uint32_t e, uint32x4_t wk) {
  return instruction_fixture::sha1m<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1})>(abcd, e, wk);
}
extern "C" __attribute__((noinline, target("sha2"))) std::uint32_t native_crypto_sha_3(std::uint32_t a) {
  return instruction_fixture::sha1h<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1})>(a);
}
extern "C" __attribute__((noinline, target("sha2"))) uint32x4_t native_crypto_sha_4(uint32x4_t w0_3, uint32x4_t w4_7, uint32x4_t w8_11) {
  return instruction_fixture::sha1su0<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1})>(w0_3, w4_7, w8_11);
}
extern "C" __attribute__((noinline, target("sha2"))) uint32x4_t native_crypto_sha_5(uint32x4_t partial, uint32x4_t w12_15) {
  return instruction_fixture::sha1su1<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha1})>(partial, w12_15);
}
extern "C" __attribute__((noinline, target("sha2"))) uint32x4_t native_crypto_sha_6(uint32x4_t abcd, uint32x4_t efgh, uint32x4_t wk) {
  return instruction_fixture::sha256h<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2})>(abcd, efgh, wk);
}
extern "C" __attribute__((noinline, target("sha2"))) uint32x4_t native_crypto_sha_7(uint32x4_t efgh, uint32x4_t abcd, uint32x4_t wk) {
  return instruction_fixture::sha256h2<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2})>(efgh, abcd, wk);
}
extern "C" __attribute__((noinline, target("sha2"))) uint32x4_t native_crypto_sha_8(uint32x4_t w0_3, uint32x4_t w4_7) {
  return instruction_fixture::sha256su0<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2})>(w0_3, w4_7);
}
extern "C" __attribute__((noinline, target("sha2"))) uint32x4_t native_crypto_sha_9(uint32x4_t partial, uint32x4_t w8_11, uint32x4_t w12_15) {
  return instruction_fixture::sha256su1<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2})>(partial, w8_11, w12_15);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_10(uint64x2_t ed, uint64x2_t gf, uint64x2_t kwh_kwh2) {
  return instruction_fixture::sha512h<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512})>(ed, gf, kwh_kwh2);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_11(uint64x2_t sum_ab, uint64x2_t c_, uint64x2_t ab) {
  return instruction_fixture::sha512h2<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512})>(sum_ab, c_, ab);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_12(uint64x2_t w0_1, uint64x2_t w2_) {
  return instruction_fixture::sha512su0<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512})>(w0_1, w2_);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_13(uint64x2_t partial, uint64x2_t w14_15, uint64x2_t w9_10) {
  return instruction_fixture::sha512su1<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha512})>(partial, w14_15, w9_10);
}
extern "C" __attribute__((noinline, target("sha3"))) uint8x16_t native_crypto_sha_14(uint8x16_t a, uint8x16_t b, uint8x16_t c) {
  return instruction_fixture::eor3<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) uint16x8_t native_crypto_sha_15(uint16x8_t a, uint16x8_t b, uint16x8_t c) {
  return instruction_fixture::eor3<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) uint32x4_t native_crypto_sha_16(uint32x4_t a, uint32x4_t b, uint32x4_t c) {
  return instruction_fixture::eor3<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_17(uint64x2_t a, uint64x2_t b, uint64x2_t c) {
  return instruction_fixture::eor3<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) int8x16_t native_crypto_sha_18(int8x16_t a, int8x16_t b, int8x16_t c) {
  return instruction_fixture::eor3<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) int16x8_t native_crypto_sha_19(int16x8_t a, int16x8_t b, int16x8_t c) {
  return instruction_fixture::eor3<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) int32x4_t native_crypto_sha_20(int32x4_t a, int32x4_t b, int32x4_t c) {
  return instruction_fixture::eor3<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) int64x2_t native_crypto_sha_21(int64x2_t a, int64x2_t b, int64x2_t c) {
  return instruction_fixture::eor3<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) uint8x16_t native_crypto_sha_22(uint8x16_t a, uint8x16_t b, uint8x16_t c) {
  return instruction_fixture::bcax<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) uint16x8_t native_crypto_sha_23(uint16x8_t a, uint16x8_t b, uint16x8_t c) {
  return instruction_fixture::bcax<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) uint32x4_t native_crypto_sha_24(uint32x4_t a, uint32x4_t b, uint32x4_t c) {
  return instruction_fixture::bcax<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_25(uint64x2_t a, uint64x2_t b, uint64x2_t c) {
  return instruction_fixture::bcax<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) int8x16_t native_crypto_sha_26(int8x16_t a, int8x16_t b, int8x16_t c) {
  return instruction_fixture::bcax<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) int16x8_t native_crypto_sha_27(int16x8_t a, int16x8_t b, int16x8_t c) {
  return instruction_fixture::bcax<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) int32x4_t native_crypto_sha_28(int32x4_t a, int32x4_t b, int32x4_t c) {
  return instruction_fixture::bcax<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) int64x2_t native_crypto_sha_29(int64x2_t a, int64x2_t b, int64x2_t c) {
  return instruction_fixture::bcax<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_30(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::rax1<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_0(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 0>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_1(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 1>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_2(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 2>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_3(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 3>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_4(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 4>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_5(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 5>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_6(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 6>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_7(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 7>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_8(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 8>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_9(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 9>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_10(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 10>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_11(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 11>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_12(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 12>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_13(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 13>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_14(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 14>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_15(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 15>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_16(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 16>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_17(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 17>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_18(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 18>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_19(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 19>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_20(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 20>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_21(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 21>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_22(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 22>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_23(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 23>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_24(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 24>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_25(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 25>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_26(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 26>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_27(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 27>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_28(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 28>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_29(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 29>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_30(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 30>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_31(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 31>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_32(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 32>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_33(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 33>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_34(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 34>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_35(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 35>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_36(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 36>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_37(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 37>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_38(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 38>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_39(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 39>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_40(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 40>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_41(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 41>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_42(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 42>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_43(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 43>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_44(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 44>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_45(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 45>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_46(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 46>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_47(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 47>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_48(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 48>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_49(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 49>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_50(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 50>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_51(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 51>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_52(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 52>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_53(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 53>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_54(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 54>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_55(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 55>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_56(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 56>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_57(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 57>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_58(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 58>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_59(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 59>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_60(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 60>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_61(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 61>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_62(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 62>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_63(uint64x2_t a, uint64x2_t b) {
  return instruction_fixture::xar<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3}), 63>(a, b);
}
extern "C" __attribute__((noinline)) unsigned native_crypto_baseline(unsigned x) { return x * 17 + 3; }

// Same signatures and targets isolate instruction-wrapper overhead.
#include "raw_codegen.h"

constexpr auto polynomial_arch = native::feature_closure(native::arm_feature::pmull);
using polynomial_vector = native::simd<std::uint64_t,2,polynomial_arch>;
extern "C" __attribute__((noinline, target("aes"))) uint64x2_t native_crypto_pmull_vector(std::uint64_t a,std::uint64_t b) {
  return __builtin_bit_cast(uint64x2_t,native::pmull<polynomial_arch>(a,b).to_native());
}
extern "C" __attribute__((noinline, target("aes"))) uint64x2_t native_crypto_pmull_vector_raw(std::uint64_t a,std::uint64_t b) {
  auto result=native::detail::arm_pmull::pmull<polynomial_arch>(a,b);
  return uint64x2_t{static_cast<std::uint64_t>(result),static_cast<std::uint64_t>(result>>64)};
}
extern "C" __attribute__((noinline, target("aes"))) uint64x2_t native_crypto_pmull2_vector(uint64x2_t a,uint64x2_t b) {
  auto left=polynomial_vector::from_native(__builtin_bit_cast(polynomial_vector::native_type,a));
  auto right=polynomial_vector::from_native(__builtin_bit_cast(polynomial_vector::native_type,b));
  return __builtin_bit_cast(uint64x2_t,native::pmull2<polynomial_arch>(left,right).to_native());
}
extern "C" __attribute__((noinline, target("aes"))) uint64x2_t native_crypto_pmull2_vector_raw(uint64x2_t a,uint64x2_t b) {
  auto result=native::detail::arm_pmull::pmull2<polynomial_arch>(__builtin_bit_cast(poly64x2_t,a),__builtin_bit_cast(poly64x2_t,b));
  return uint64x2_t{static_cast<std::uint64_t>(result),static_cast<std::uint64_t>(result>>64)};
}
