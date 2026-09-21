// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

extern "C" __attribute__((noinline, target("aes"))) uint8x16_t native_crypto_aes_0_raw(uint8x16_t state, uint8x16_t round_key) {
  return native::detail::arm_aes::aese<native::feature_closure(native::isa{native::arm_feature::aes})>(state, round_key);
}
extern "C" __attribute__((noinline, target("aes"))) uint8x16_t native_crypto_aes_1_raw(uint8x16_t state, uint8x16_t round_key) {
  return native::detail::arm_aes::aesd<native::feature_closure(native::isa{native::arm_feature::aes})>(state, round_key);
}
extern "C" __attribute__((noinline, target("aes"))) uint8x16_t native_crypto_aes_2_raw(uint8x16_t state) {
  return native::detail::arm_aes::aesmc<native::feature_closure(native::isa{native::arm_feature::aes})>(state);
}
extern "C" __attribute__((noinline, target("aes"))) uint8x16_t native_crypto_aes_3_raw(uint8x16_t state) {
  return native::detail::arm_aes::aesimc<native::feature_closure(native::isa{native::arm_feature::aes})>(state);
}
extern "C" __attribute__((noinline, target("aes"))) poly128_t native_crypto_pmull_0_raw(poly64_t a, poly64_t b) {
  return native::detail::arm_pmull::pmull<native::feature_closure(native::isa{native::arm_feature::pmull})>(a, b);
}
extern "C" __attribute__((noinline, target("aes"))) poly128_t native_crypto_pmull_1_raw(poly64x2_t a, poly64x2_t b) {
  return native::detail::arm_pmull::pmull2<native::feature_closure(native::isa{native::arm_feature::pmull})>(a, b);
}
extern "C" __attribute__((noinline)) poly16x8_t native_crypto_pmull_2_raw(poly8x8_t a, poly8x8_t b) {
  return native::detail::arm_pmull::pmull<native::feature_closure(native::isa{native::arm_feature::neon})>(a, b);
}
extern "C" __attribute__((noinline)) poly16x8_t native_crypto_pmull_3_raw(poly8x16_t a, poly8x16_t b) {
  return native::detail::arm_pmull::pmull2<native::feature_closure(native::isa{native::arm_feature::neon})>(a, b);
}
extern "C" __attribute__((noinline, target("sha2"))) uint32x4_t native_crypto_sha_0_raw(uint32x4_t abcd, std::uint32_t e, uint32x4_t wk) {
  return native::detail::arm_sha::sha1c<native::feature_closure(native::isa{native::arm_feature::sha1})>(abcd, e, wk);
}
extern "C" __attribute__((noinline, target("sha2"))) uint32x4_t native_crypto_sha_1_raw(uint32x4_t abcd, std::uint32_t e, uint32x4_t wk) {
  return native::detail::arm_sha::sha1p<native::feature_closure(native::isa{native::arm_feature::sha1})>(abcd, e, wk);
}
extern "C" __attribute__((noinline, target("sha2"))) uint32x4_t native_crypto_sha_2_raw(uint32x4_t abcd, std::uint32_t e, uint32x4_t wk) {
  return native::detail::arm_sha::sha1m<native::feature_closure(native::isa{native::arm_feature::sha1})>(abcd, e, wk);
}
extern "C" __attribute__((noinline, target("sha2"))) std::uint32_t native_crypto_sha_3_raw(std::uint32_t a) {
  return native::detail::arm_sha::sha1h<native::feature_closure(native::isa{native::arm_feature::sha1})>(a);
}
extern "C" __attribute__((noinline, target("sha2"))) uint32x4_t native_crypto_sha_4_raw(uint32x4_t w0_3, uint32x4_t w4_7, uint32x4_t w8_11) {
  return native::detail::arm_sha::sha1su0<native::feature_closure(native::isa{native::arm_feature::sha1})>(w0_3, w4_7, w8_11);
}
extern "C" __attribute__((noinline, target("sha2"))) uint32x4_t native_crypto_sha_5_raw(uint32x4_t partial, uint32x4_t w12_15) {
  return native::detail::arm_sha::sha1su1<native::feature_closure(native::isa{native::arm_feature::sha1})>(partial, w12_15);
}
extern "C" __attribute__((noinline, target("sha2"))) uint32x4_t native_crypto_sha_6_raw(uint32x4_t abcd, uint32x4_t efgh, uint32x4_t wk) {
  return native::detail::arm_sha::sha256h<native::feature_closure(native::isa{native::arm_feature::sha2})>(abcd, efgh, wk);
}
extern "C" __attribute__((noinline, target("sha2"))) uint32x4_t native_crypto_sha_7_raw(uint32x4_t efgh, uint32x4_t abcd, uint32x4_t wk) {
  return native::detail::arm_sha::sha256h2<native::feature_closure(native::isa{native::arm_feature::sha2})>(efgh, abcd, wk);
}
extern "C" __attribute__((noinline, target("sha2"))) uint32x4_t native_crypto_sha_8_raw(uint32x4_t w0_3, uint32x4_t w4_7) {
  return native::detail::arm_sha::sha256su0<native::feature_closure(native::isa{native::arm_feature::sha2})>(w0_3, w4_7);
}
extern "C" __attribute__((noinline, target("sha2"))) uint32x4_t native_crypto_sha_9_raw(uint32x4_t partial, uint32x4_t w8_11, uint32x4_t w12_15) {
  return native::detail::arm_sha::sha256su1<native::feature_closure(native::isa{native::arm_feature::sha2})>(partial, w8_11, w12_15);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_10_raw(uint64x2_t ed, uint64x2_t gf, uint64x2_t kwh_kwh2) {
  return native::detail::arm_sha::sha512h<native::feature_closure(native::isa{native::arm_feature::sha512})>(ed, gf, kwh_kwh2);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_11_raw(uint64x2_t sum_ab, uint64x2_t c_, uint64x2_t ab) {
  return native::detail::arm_sha::sha512h2<native::feature_closure(native::isa{native::arm_feature::sha512})>(sum_ab, c_, ab);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_12_raw(uint64x2_t w0_1, uint64x2_t w2_) {
  return native::detail::arm_sha::sha512su0<native::feature_closure(native::isa{native::arm_feature::sha512})>(w0_1, w2_);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_13_raw(uint64x2_t partial, uint64x2_t w14_15, uint64x2_t w9_10) {
  return native::detail::arm_sha::sha512su1<native::feature_closure(native::isa{native::arm_feature::sha512})>(partial, w14_15, w9_10);
}
extern "C" __attribute__((noinline, target("sha3"))) uint8x16_t native_crypto_sha_14_raw(uint8x16_t a, uint8x16_t b, uint8x16_t c) {
  return native::detail::arm_sha::eor3<native::feature_closure(native::isa{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) uint16x8_t native_crypto_sha_15_raw(uint16x8_t a, uint16x8_t b, uint16x8_t c) {
  return native::detail::arm_sha::eor3<native::feature_closure(native::isa{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) uint32x4_t native_crypto_sha_16_raw(uint32x4_t a, uint32x4_t b, uint32x4_t c) {
  return native::detail::arm_sha::eor3<native::feature_closure(native::isa{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_17_raw(uint64x2_t a, uint64x2_t b, uint64x2_t c) {
  return native::detail::arm_sha::eor3<native::feature_closure(native::isa{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) int8x16_t native_crypto_sha_18_raw(int8x16_t a, int8x16_t b, int8x16_t c) {
  return native::detail::arm_sha::eor3<native::feature_closure(native::isa{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) int16x8_t native_crypto_sha_19_raw(int16x8_t a, int16x8_t b, int16x8_t c) {
  return native::detail::arm_sha::eor3<native::feature_closure(native::isa{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) int32x4_t native_crypto_sha_20_raw(int32x4_t a, int32x4_t b, int32x4_t c) {
  return native::detail::arm_sha::eor3<native::feature_closure(native::isa{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) int64x2_t native_crypto_sha_21_raw(int64x2_t a, int64x2_t b, int64x2_t c) {
  return native::detail::arm_sha::eor3<native::feature_closure(native::isa{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) uint8x16_t native_crypto_sha_22_raw(uint8x16_t a, uint8x16_t b, uint8x16_t c) {
  return native::detail::arm_sha::bcax<native::feature_closure(native::isa{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) uint16x8_t native_crypto_sha_23_raw(uint16x8_t a, uint16x8_t b, uint16x8_t c) {
  return native::detail::arm_sha::bcax<native::feature_closure(native::isa{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) uint32x4_t native_crypto_sha_24_raw(uint32x4_t a, uint32x4_t b, uint32x4_t c) {
  return native::detail::arm_sha::bcax<native::feature_closure(native::isa{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_25_raw(uint64x2_t a, uint64x2_t b, uint64x2_t c) {
  return native::detail::arm_sha::bcax<native::feature_closure(native::isa{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) int8x16_t native_crypto_sha_26_raw(int8x16_t a, int8x16_t b, int8x16_t c) {
  return native::detail::arm_sha::bcax<native::feature_closure(native::isa{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) int16x8_t native_crypto_sha_27_raw(int16x8_t a, int16x8_t b, int16x8_t c) {
  return native::detail::arm_sha::bcax<native::feature_closure(native::isa{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) int32x4_t native_crypto_sha_28_raw(int32x4_t a, int32x4_t b, int32x4_t c) {
  return native::detail::arm_sha::bcax<native::feature_closure(native::isa{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) int64x2_t native_crypto_sha_29_raw(int64x2_t a, int64x2_t b, int64x2_t c) {
  return native::detail::arm_sha::bcax<native::feature_closure(native::isa{native::arm_feature::sha3})>(a, b, c);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_30_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::rax1<native::feature_closure(native::isa{native::arm_feature::sha3})>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_0_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 0>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_1_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 1>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_2_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 2>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_3_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 3>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_4_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 4>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_5_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 5>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_6_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 6>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_7_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 7>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_8_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 8>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_9_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 9>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_10_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 10>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_11_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 11>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_12_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 12>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_13_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 13>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_14_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 14>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_15_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 15>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_16_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 16>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_17_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 17>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_18_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 18>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_19_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 19>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_20_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 20>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_21_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 21>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_22_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 22>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_23_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 23>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_24_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 24>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_25_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 25>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_26_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 26>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_27_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 27>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_28_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 28>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_29_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 29>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_30_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 30>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_31_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 31>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_32_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 32>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_33_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 33>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_34_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 34>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_35_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 35>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_36_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 36>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_37_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 37>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_38_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 38>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_39_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 39>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_40_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 40>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_41_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 41>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_42_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 42>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_43_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 43>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_44_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 44>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_45_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 45>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_46_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 46>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_47_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 47>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_48_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 48>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_49_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 49>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_50_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 50>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_51_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 51>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_52_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 52>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_53_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 53>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_54_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 54>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_55_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 55>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_56_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 56>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_57_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 57>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_58_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 58>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_59_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 59>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_60_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 60>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_61_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 61>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_62_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 62>(a, b);
}
extern "C" __attribute__((noinline, target("sha3"))) uint64x2_t native_crypto_sha_31_63_raw(uint64x2_t a, uint64x2_t b) {
  return native::detail::arm_sha::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 63>(a, b);
}
extern "C" __attribute__((noinline)) unsigned native_crypto_baseline_raw(unsigned x) { return x * 17 + 3; }
