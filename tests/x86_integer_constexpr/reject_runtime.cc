// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.x86.pclmul;
import native.x86.vpclmul;
import native.x86.gfni;
import native.x86.vpopcntdq;
import native.x86.vnni;
using native::simd;
using native::predicate;
constexpr auto storage128=native::target_features<native::x86>("sse2");
constexpr auto storage256=native::target_features<native::x86>("avx");
constexpr auto storage512=native::target_features<native::x86>("avx512f");

// Each marked call must fail because its operands are runtime values.
// One compiler invocation checks every weak overload without stopping at the first error.
simd<std::uint64_t, 2, storage128> reject_pclmulqdq_128(simd<std::uint64_t, 2, storage128> a, simd<std::uint64_t, 2, storage128> b) {
  return native::pclmulqdq<storage128,0>(a, b); // expected-consteval-rejection
}

simd<std::uint64_t, 2, storage128> reject_vpclmulqdq_128(simd<std::uint64_t, 2, storage128> a, simd<std::uint64_t, 2, storage128> b) {
  return native::vpclmulqdq<storage128,0>(a, b); // expected-consteval-rejection
}

simd<std::uint64_t, 4, storage256> reject_vpclmulqdq_256(simd<std::uint64_t, 4, storage256> a, simd<std::uint64_t, 4, storage256> b) {
  return native::vpclmulqdq<storage256,0>(a, b); // expected-consteval-rejection
}

simd<std::uint64_t, 8, storage512> reject_vpclmulqdq_512(simd<std::uint64_t, 8, storage512> a, simd<std::uint64_t, 8, storage512> b) {
  return native::vpclmulqdq<storage512,0>(a, b); // expected-consteval-rejection
}

simd<std::uint8_t, 16, storage128> reject_gf2p8mulb_128(simd<std::uint8_t, 16, storage128> a, simd<std::uint8_t, 16, storage128> b) {
  return native::gf2p8mulb<storage128>(a, b); // expected-consteval-rejection
}

simd<std::uint8_t, 16, storage128> reject_gf2p8affineqb_128(simd<std::uint8_t, 16, storage128> a, simd<std::uint64_t, 2, storage128> matrix) {
  return native::gf2p8affineqb<storage128,0>(a, matrix); // expected-consteval-rejection
}

simd<std::uint8_t, 16, storage128> reject_gf2p8affineinvqb_128(simd<std::uint8_t, 16, storage128> a, simd<std::uint64_t, 2, storage128> matrix) {
  return native::gf2p8affineinvqb<storage128,0>(a, matrix); // expected-consteval-rejection
}

simd<std::uint8_t, 16, storage128> reject_gf2p8mulb_mask_128(simd<std::uint8_t, 16, storage128> src, predicate<16, storage128> k, simd<std::uint8_t, 16, storage128> a, simd<std::uint8_t, 16, storage128> b) {
  return native::gf2p8mulb_mask<storage128>(src, k, a, b); // expected-consteval-rejection
}

simd<std::uint8_t, 16, storage128> reject_gf2p8mulb_maskz_128(predicate<16, storage128> k, simd<std::uint8_t, 16, storage128> a, simd<std::uint8_t, 16, storage128> b) {
  return native::gf2p8mulb_maskz<storage128>(k, a, b); // expected-consteval-rejection
}

simd<std::uint8_t, 16, storage128> reject_gf2p8affineqb_mask_128(simd<std::uint8_t, 16, storage128> src, predicate<16, storage128> k, simd<std::uint8_t, 16, storage128> a, simd<std::uint64_t, 2, storage128> matrix) {
  return native::gf2p8affineqb_mask<storage128,0>(src, k, a, matrix); // expected-consteval-rejection
}

simd<std::uint8_t, 16, storage128> reject_gf2p8affineqb_maskz_128(predicate<16, storage128> k, simd<std::uint8_t, 16, storage128> a, simd<std::uint64_t, 2, storage128> matrix) {
  return native::gf2p8affineqb_maskz<storage128,0>(k, a, matrix); // expected-consteval-rejection
}

simd<std::uint8_t, 16, storage128> reject_gf2p8affineinvqb_mask_128(simd<std::uint8_t, 16, storage128> src, predicate<16, storage128> k, simd<std::uint8_t, 16, storage128> a, simd<std::uint64_t, 2, storage128> matrix) {
  return native::gf2p8affineinvqb_mask<storage128,0>(src, k, a, matrix); // expected-consteval-rejection
}

simd<std::uint8_t, 16, storage128> reject_gf2p8affineinvqb_maskz_128(predicate<16, storage128> k, simd<std::uint8_t, 16, storage128> a, simd<std::uint64_t, 2, storage128> matrix) {
  return native::gf2p8affineinvqb_maskz<storage128,0>(k, a, matrix); // expected-consteval-rejection
}

simd<std::uint8_t, 32, storage256> reject_gf2p8mulb_256(simd<std::uint8_t, 32, storage256> a, simd<std::uint8_t, 32, storage256> b) {
  return native::gf2p8mulb<storage256>(a, b); // expected-consteval-rejection
}

simd<std::uint8_t, 32, storage256> reject_gf2p8affineqb_256(simd<std::uint8_t, 32, storage256> a, simd<std::uint64_t, 4, storage256> matrix) {
  return native::gf2p8affineqb<storage256,0>(a, matrix); // expected-consteval-rejection
}

simd<std::uint8_t, 32, storage256> reject_gf2p8affineinvqb_256(simd<std::uint8_t, 32, storage256> a, simd<std::uint64_t, 4, storage256> matrix) {
  return native::gf2p8affineinvqb<storage256,0>(a, matrix); // expected-consteval-rejection
}

simd<std::uint8_t, 32, storage256> reject_gf2p8mulb_mask_256(simd<std::uint8_t, 32, storage256> src, predicate<32, storage256> k, simd<std::uint8_t, 32, storage256> a, simd<std::uint8_t, 32, storage256> b) {
  return native::gf2p8mulb_mask<storage256>(src, k, a, b); // expected-consteval-rejection
}

simd<std::uint8_t, 32, storage256> reject_gf2p8mulb_maskz_256(predicate<32, storage256> k, simd<std::uint8_t, 32, storage256> a, simd<std::uint8_t, 32, storage256> b) {
  return native::gf2p8mulb_maskz<storage256>(k, a, b); // expected-consteval-rejection
}

simd<std::uint8_t, 32, storage256> reject_gf2p8affineqb_mask_256(simd<std::uint8_t, 32, storage256> src, predicate<32, storage256> k, simd<std::uint8_t, 32, storage256> a, simd<std::uint64_t, 4, storage256> matrix) {
  return native::gf2p8affineqb_mask<storage256,0>(src, k, a, matrix); // expected-consteval-rejection
}

simd<std::uint8_t, 32, storage256> reject_gf2p8affineqb_maskz_256(predicate<32, storage256> k, simd<std::uint8_t, 32, storage256> a, simd<std::uint64_t, 4, storage256> matrix) {
  return native::gf2p8affineqb_maskz<storage256,0>(k, a, matrix); // expected-consteval-rejection
}

simd<std::uint8_t, 32, storage256> reject_gf2p8affineinvqb_mask_256(simd<std::uint8_t, 32, storage256> src, predicate<32, storage256> k, simd<std::uint8_t, 32, storage256> a, simd<std::uint64_t, 4, storage256> matrix) {
  return native::gf2p8affineinvqb_mask<storage256,0>(src, k, a, matrix); // expected-consteval-rejection
}

simd<std::uint8_t, 32, storage256> reject_gf2p8affineinvqb_maskz_256(predicate<32, storage256> k, simd<std::uint8_t, 32, storage256> a, simd<std::uint64_t, 4, storage256> matrix) {
  return native::gf2p8affineinvqb_maskz<storage256,0>(k, a, matrix); // expected-consteval-rejection
}

simd<std::uint8_t, 64, storage512> reject_gf2p8mulb_512(simd<std::uint8_t, 64, storage512> a, simd<std::uint8_t, 64, storage512> b) {
  return native::gf2p8mulb<storage512>(a, b); // expected-consteval-rejection
}

simd<std::uint8_t, 64, storage512> reject_gf2p8affineqb_512(simd<std::uint8_t, 64, storage512> a, simd<std::uint64_t, 8, storage512> matrix) {
  return native::gf2p8affineqb<storage512,0>(a, matrix); // expected-consteval-rejection
}

simd<std::uint8_t, 64, storage512> reject_gf2p8affineinvqb_512(simd<std::uint8_t, 64, storage512> a, simd<std::uint64_t, 8, storage512> matrix) {
  return native::gf2p8affineinvqb<storage512,0>(a, matrix); // expected-consteval-rejection
}

simd<std::uint8_t, 64, storage512> reject_gf2p8mulb_mask_512(simd<std::uint8_t, 64, storage512> src, predicate<64, storage512> k, simd<std::uint8_t, 64, storage512> a, simd<std::uint8_t, 64, storage512> b) {
  return native::gf2p8mulb_mask<storage512>(src, k, a, b); // expected-consteval-rejection
}

simd<std::uint8_t, 64, storage512> reject_gf2p8mulb_maskz_512(predicate<64, storage512> k, simd<std::uint8_t, 64, storage512> a, simd<std::uint8_t, 64, storage512> b) {
  return native::gf2p8mulb_maskz<storage512>(k, a, b); // expected-consteval-rejection
}

simd<std::uint8_t, 64, storage512> reject_gf2p8affineqb_mask_512(simd<std::uint8_t, 64, storage512> src, predicate<64, storage512> k, simd<std::uint8_t, 64, storage512> a, simd<std::uint64_t, 8, storage512> matrix) {
  return native::gf2p8affineqb_mask<storage512,0>(src, k, a, matrix); // expected-consteval-rejection
}

simd<std::uint8_t, 64, storage512> reject_gf2p8affineqb_maskz_512(predicate<64, storage512> k, simd<std::uint8_t, 64, storage512> a, simd<std::uint64_t, 8, storage512> matrix) {
  return native::gf2p8affineqb_maskz<storage512,0>(k, a, matrix); // expected-consteval-rejection
}

simd<std::uint8_t, 64, storage512> reject_gf2p8affineinvqb_mask_512(simd<std::uint8_t, 64, storage512> src, predicate<64, storage512> k, simd<std::uint8_t, 64, storage512> a, simd<std::uint64_t, 8, storage512> matrix) {
  return native::gf2p8affineinvqb_mask<storage512,0>(src, k, a, matrix); // expected-consteval-rejection
}

simd<std::uint8_t, 64, storage512> reject_gf2p8affineinvqb_maskz_512(predicate<64, storage512> k, simd<std::uint8_t, 64, storage512> a, simd<std::uint64_t, 8, storage512> matrix) {
  return native::gf2p8affineinvqb_maskz<storage512,0>(k, a, matrix); // expected-consteval-rejection
}

simd<std::uint32_t, 16, storage512> reject_vpopcntd_512(simd<std::uint32_t, 16, storage512> value) {
  return native::vpopcntd<storage512>(value); // expected-consteval-rejection
}

simd<std::uint32_t, 16, storage512> reject_mask_vpopcntd_512(simd<std::uint32_t, 16, storage512> source, predicate<16, storage512> mask, simd<std::uint32_t, 16, storage512> value) {
  return native::mask_vpopcntd<storage512>(source, mask, value); // expected-consteval-rejection
}

simd<std::uint32_t, 16, storage512> reject_maskz_vpopcntd_512(predicate<16, storage512> mask, simd<std::uint32_t, 16, storage512> value) {
  return native::maskz_vpopcntd<storage512>(mask, value); // expected-consteval-rejection
}

simd<std::uint64_t, 8, storage512> reject_vpopcntq_512(simd<std::uint64_t, 8, storage512> value) {
  return native::vpopcntq<storage512>(value); // expected-consteval-rejection
}

simd<std::uint64_t, 8, storage512> reject_mask_vpopcntq_512(simd<std::uint64_t, 8, storage512> source, predicate<8, storage512> mask, simd<std::uint64_t, 8, storage512> value) {
  return native::mask_vpopcntq<storage512>(source, mask, value); // expected-consteval-rejection
}

simd<std::uint64_t, 8, storage512> reject_maskz_vpopcntq_512(predicate<8, storage512> mask, simd<std::uint64_t, 8, storage512> value) {
  return native::maskz_vpopcntq<storage512>(mask, value); // expected-consteval-rejection
}

simd<std::uint32_t, 4, storage128> reject_vpopcntd_128(simd<std::uint32_t, 4, storage128> value) {
  return native::vpopcntd<storage128>(value); // expected-consteval-rejection
}

simd<std::uint32_t, 4, storage128> reject_mask_vpopcntd_128(simd<std::uint32_t, 4, storage128> source, predicate<4, storage128> mask, simd<std::uint32_t, 4, storage128> value) {
  return native::mask_vpopcntd<storage128>(source, mask, value); // expected-consteval-rejection
}

simd<std::uint32_t, 4, storage128> reject_maskz_vpopcntd_128(predicate<4, storage128> mask, simd<std::uint32_t, 4, storage128> value) {
  return native::maskz_vpopcntd<storage128>(mask, value); // expected-consteval-rejection
}

simd<std::uint64_t, 2, storage128> reject_vpopcntq_128(simd<std::uint64_t, 2, storage128> value) {
  return native::vpopcntq<storage128>(value); // expected-consteval-rejection
}

simd<std::uint64_t, 2, storage128> reject_mask_vpopcntq_128(simd<std::uint64_t, 2, storage128> source, predicate<2, storage128> mask, simd<std::uint64_t, 2, storage128> value) {
  return native::mask_vpopcntq<storage128>(source, mask, value); // expected-consteval-rejection
}

simd<std::uint64_t, 2, storage128> reject_maskz_vpopcntq_128(predicate<2, storage128> mask, simd<std::uint64_t, 2, storage128> value) {
  return native::maskz_vpopcntq<storage128>(mask, value); // expected-consteval-rejection
}

simd<std::uint32_t, 8, storage256> reject_vpopcntd_256(simd<std::uint32_t, 8, storage256> value) {
  return native::vpopcntd<storage256>(value); // expected-consteval-rejection
}

simd<std::uint32_t, 8, storage256> reject_mask_vpopcntd_256(simd<std::uint32_t, 8, storage256> source, predicate<8, storage256> mask, simd<std::uint32_t, 8, storage256> value) {
  return native::mask_vpopcntd<storage256>(source, mask, value); // expected-consteval-rejection
}

simd<std::uint32_t, 8, storage256> reject_maskz_vpopcntd_256(predicate<8, storage256> mask, simd<std::uint32_t, 8, storage256> value) {
  return native::maskz_vpopcntd<storage256>(mask, value); // expected-consteval-rejection
}

simd<std::uint64_t, 4, storage256> reject_vpopcntq_256(simd<std::uint64_t, 4, storage256> value) {
  return native::vpopcntq<storage256>(value); // expected-consteval-rejection
}

simd<std::uint64_t, 4, storage256> reject_mask_vpopcntq_256(simd<std::uint64_t, 4, storage256> source, predicate<4, storage256> mask, simd<std::uint64_t, 4, storage256> value) {
  return native::mask_vpopcntq<storage256>(source, mask, value); // expected-consteval-rejection
}

simd<std::uint64_t, 4, storage256> reject_maskz_vpopcntq_256(predicate<4, storage256> mask, simd<std::uint64_t, 4, storage256> value) {
  return native::maskz_vpopcntq<storage256>(mask, value); // expected-consteval-rejection
}

simd<std::int32_t, 4, storage128> reject_dpbusd_128(simd<std::int32_t, 4, storage128> accumulator, simd<std::uint8_t, 16, storage128> a, simd<std::int8_t, 16, storage128> b) {
  return native::dpbusd<storage128>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 4, storage128> reject_mask_dpbusd_128(simd<std::int32_t, 4, storage128> accumulator, predicate<4, storage128> mask, simd<std::uint8_t, 16, storage128> a, simd<std::int8_t, 16, storage128> b) {
  return native::mask_dpbusd<storage128>(accumulator, mask, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 4, storage128> reject_maskz_dpbusd_128(predicate<4, storage128> mask, simd<std::int32_t, 4, storage128> accumulator, simd<std::uint8_t, 16, storage128> a, simd<std::int8_t, 16, storage128> b) {
  return native::maskz_dpbusd<storage128>(mask, accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 4, storage128> reject_dpbusds_128(simd<std::int32_t, 4, storage128> accumulator, simd<std::uint8_t, 16, storage128> a, simd<std::int8_t, 16, storage128> b) {
  return native::dpbusds<storage128>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 4, storage128> reject_mask_dpbusds_128(simd<std::int32_t, 4, storage128> accumulator, predicate<4, storage128> mask, simd<std::uint8_t, 16, storage128> a, simd<std::int8_t, 16, storage128> b) {
  return native::mask_dpbusds<storage128>(accumulator, mask, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 4, storage128> reject_maskz_dpbusds_128(predicate<4, storage128> mask, simd<std::int32_t, 4, storage128> accumulator, simd<std::uint8_t, 16, storage128> a, simd<std::int8_t, 16, storage128> b) {
  return native::maskz_dpbusds<storage128>(mask, accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 4, storage128> reject_dpwssd_128(simd<std::int32_t, 4, storage128> accumulator, simd<std::int16_t, 8, storage128> a, simd<std::int16_t, 8, storage128> b) {
  return native::dpwssd<storage128>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 4, storage128> reject_mask_dpwssd_128(simd<std::int32_t, 4, storage128> accumulator, predicate<4, storage128> mask, simd<std::int16_t, 8, storage128> a, simd<std::int16_t, 8, storage128> b) {
  return native::mask_dpwssd<storage128>(accumulator, mask, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 4, storage128> reject_maskz_dpwssd_128(predicate<4, storage128> mask, simd<std::int32_t, 4, storage128> accumulator, simd<std::int16_t, 8, storage128> a, simd<std::int16_t, 8, storage128> b) {
  return native::maskz_dpwssd<storage128>(mask, accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 4, storage128> reject_dpwssds_128(simd<std::int32_t, 4, storage128> accumulator, simd<std::int16_t, 8, storage128> a, simd<std::int16_t, 8, storage128> b) {
  return native::dpwssds<storage128>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 4, storage128> reject_mask_dpwssds_128(simd<std::int32_t, 4, storage128> accumulator, predicate<4, storage128> mask, simd<std::int16_t, 8, storage128> a, simd<std::int16_t, 8, storage128> b) {
  return native::mask_dpwssds<storage128>(accumulator, mask, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 4, storage128> reject_maskz_dpwssds_128(predicate<4, storage128> mask, simd<std::int32_t, 4, storage128> accumulator, simd<std::int16_t, 8, storage128> a, simd<std::int16_t, 8, storage128> b) {
  return native::maskz_dpwssds<storage128>(mask, accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 8, storage256> reject_dpbusd_256(simd<std::int32_t, 8, storage256> accumulator, simd<std::uint8_t, 32, storage256> a, simd<std::int8_t, 32, storage256> b) {
  return native::dpbusd<storage256>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 8, storage256> reject_mask_dpbusd_256(simd<std::int32_t, 8, storage256> accumulator, predicate<8, storage256> mask, simd<std::uint8_t, 32, storage256> a, simd<std::int8_t, 32, storage256> b) {
  return native::mask_dpbusd<storage256>(accumulator, mask, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 8, storage256> reject_maskz_dpbusd_256(predicate<8, storage256> mask, simd<std::int32_t, 8, storage256> accumulator, simd<std::uint8_t, 32, storage256> a, simd<std::int8_t, 32, storage256> b) {
  return native::maskz_dpbusd<storage256>(mask, accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 8, storage256> reject_dpbusds_256(simd<std::int32_t, 8, storage256> accumulator, simd<std::uint8_t, 32, storage256> a, simd<std::int8_t, 32, storage256> b) {
  return native::dpbusds<storage256>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 8, storage256> reject_mask_dpbusds_256(simd<std::int32_t, 8, storage256> accumulator, predicate<8, storage256> mask, simd<std::uint8_t, 32, storage256> a, simd<std::int8_t, 32, storage256> b) {
  return native::mask_dpbusds<storage256>(accumulator, mask, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 8, storage256> reject_maskz_dpbusds_256(predicate<8, storage256> mask, simd<std::int32_t, 8, storage256> accumulator, simd<std::uint8_t, 32, storage256> a, simd<std::int8_t, 32, storage256> b) {
  return native::maskz_dpbusds<storage256>(mask, accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 8, storage256> reject_dpwssd_256(simd<std::int32_t, 8, storage256> accumulator, simd<std::int16_t, 16, storage256> a, simd<std::int16_t, 16, storage256> b) {
  return native::dpwssd<storage256>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 8, storage256> reject_mask_dpwssd_256(simd<std::int32_t, 8, storage256> accumulator, predicate<8, storage256> mask, simd<std::int16_t, 16, storage256> a, simd<std::int16_t, 16, storage256> b) {
  return native::mask_dpwssd<storage256>(accumulator, mask, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 8, storage256> reject_maskz_dpwssd_256(predicate<8, storage256> mask, simd<std::int32_t, 8, storage256> accumulator, simd<std::int16_t, 16, storage256> a, simd<std::int16_t, 16, storage256> b) {
  return native::maskz_dpwssd<storage256>(mask, accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 8, storage256> reject_dpwssds_256(simd<std::int32_t, 8, storage256> accumulator, simd<std::int16_t, 16, storage256> a, simd<std::int16_t, 16, storage256> b) {
  return native::dpwssds<storage256>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 8, storage256> reject_mask_dpwssds_256(simd<std::int32_t, 8, storage256> accumulator, predicate<8, storage256> mask, simd<std::int16_t, 16, storage256> a, simd<std::int16_t, 16, storage256> b) {
  return native::mask_dpwssds<storage256>(accumulator, mask, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 8, storage256> reject_maskz_dpwssds_256(predicate<8, storage256> mask, simd<std::int32_t, 8, storage256> accumulator, simd<std::int16_t, 16, storage256> a, simd<std::int16_t, 16, storage256> b) {
  return native::maskz_dpwssds<storage256>(mask, accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 16, storage512> reject_dpbusd_512(simd<std::int32_t, 16, storage512> accumulator, simd<std::uint8_t, 64, storage512> a, simd<std::int8_t, 64, storage512> b) {
  return native::dpbusd<storage512>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 16, storage512> reject_mask_dpbusd_512(simd<std::int32_t, 16, storage512> accumulator, predicate<16, storage512> mask, simd<std::uint8_t, 64, storage512> a, simd<std::int8_t, 64, storage512> b) {
  return native::mask_dpbusd<storage512>(accumulator, mask, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 16, storage512> reject_maskz_dpbusd_512(predicate<16, storage512> mask, simd<std::int32_t, 16, storage512> accumulator, simd<std::uint8_t, 64, storage512> a, simd<std::int8_t, 64, storage512> b) {
  return native::maskz_dpbusd<storage512>(mask, accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 16, storage512> reject_dpbusds_512(simd<std::int32_t, 16, storage512> accumulator, simd<std::uint8_t, 64, storage512> a, simd<std::int8_t, 64, storage512> b) {
  return native::dpbusds<storage512>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 16, storage512> reject_mask_dpbusds_512(simd<std::int32_t, 16, storage512> accumulator, predicate<16, storage512> mask, simd<std::uint8_t, 64, storage512> a, simd<std::int8_t, 64, storage512> b) {
  return native::mask_dpbusds<storage512>(accumulator, mask, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 16, storage512> reject_maskz_dpbusds_512(predicate<16, storage512> mask, simd<std::int32_t, 16, storage512> accumulator, simd<std::uint8_t, 64, storage512> a, simd<std::int8_t, 64, storage512> b) {
  return native::maskz_dpbusds<storage512>(mask, accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 16, storage512> reject_dpwssd_512(simd<std::int32_t, 16, storage512> accumulator, simd<std::int16_t, 32, storage512> a, simd<std::int16_t, 32, storage512> b) {
  return native::dpwssd<storage512>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 16, storage512> reject_mask_dpwssd_512(simd<std::int32_t, 16, storage512> accumulator, predicate<16, storage512> mask, simd<std::int16_t, 32, storage512> a, simd<std::int16_t, 32, storage512> b) {
  return native::mask_dpwssd<storage512>(accumulator, mask, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 16, storage512> reject_maskz_dpwssd_512(predicate<16, storage512> mask, simd<std::int32_t, 16, storage512> accumulator, simd<std::int16_t, 32, storage512> a, simd<std::int16_t, 32, storage512> b) {
  return native::maskz_dpwssd<storage512>(mask, accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 16, storage512> reject_dpwssds_512(simd<std::int32_t, 16, storage512> accumulator, simd<std::int16_t, 32, storage512> a, simd<std::int16_t, 32, storage512> b) {
  return native::dpwssds<storage512>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 16, storage512> reject_mask_dpwssds_512(simd<std::int32_t, 16, storage512> accumulator, predicate<16, storage512> mask, simd<std::int16_t, 32, storage512> a, simd<std::int16_t, 32, storage512> b) {
  return native::mask_dpwssds<storage512>(accumulator, mask, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 16, storage512> reject_maskz_dpwssds_512(predicate<16, storage512> mask, simd<std::int32_t, 16, storage512> accumulator, simd<std::int16_t, 32, storage512> a, simd<std::int16_t, 32, storage512> b) {
  return native::maskz_dpwssds<storage512>(mask, accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 4, storage128> reject_dpbssd_128(simd<std::int32_t, 4, storage128> accumulator, simd<std::int8_t, 16, storage128> a, simd<std::int8_t, 16, storage128> b) {
  return native::dpbssd<storage128>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 8, storage256> reject_dpbssd_256(simd<std::int32_t, 8, storage256> accumulator, simd<std::int8_t, 32, storage256> a, simd<std::int8_t, 32, storage256> b) {
  return native::dpbssd<storage256>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 4, storage128> reject_dpbssds_128(simd<std::int32_t, 4, storage128> accumulator, simd<std::int8_t, 16, storage128> a, simd<std::int8_t, 16, storage128> b) {
  return native::dpbssds<storage128>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 8, storage256> reject_dpbssds_256(simd<std::int32_t, 8, storage256> accumulator, simd<std::int8_t, 32, storage256> a, simd<std::int8_t, 32, storage256> b) {
  return native::dpbssds<storage256>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 4, storage128> reject_dpbsud_128(simd<std::int32_t, 4, storage128> accumulator, simd<std::int8_t, 16, storage128> a, simd<std::uint8_t, 16, storage128> b) {
  return native::dpbsud<storage128>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 8, storage256> reject_dpbsud_256(simd<std::int32_t, 8, storage256> accumulator, simd<std::int8_t, 32, storage256> a, simd<std::uint8_t, 32, storage256> b) {
  return native::dpbsud<storage256>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 4, storage128> reject_dpbsuds_128(simd<std::int32_t, 4, storage128> accumulator, simd<std::int8_t, 16, storage128> a, simd<std::uint8_t, 16, storage128> b) {
  return native::dpbsuds<storage128>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 8, storage256> reject_dpbsuds_256(simd<std::int32_t, 8, storage256> accumulator, simd<std::int8_t, 32, storage256> a, simd<std::uint8_t, 32, storage256> b) {
  return native::dpbsuds<storage256>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::uint32_t, 4, storage128> reject_dpbuud_128(simd<std::uint32_t, 4, storage128> accumulator, simd<std::uint8_t, 16, storage128> a, simd<std::uint8_t, 16, storage128> b) {
  return native::dpbuud<storage128>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::uint32_t, 8, storage256> reject_dpbuud_256(simd<std::uint32_t, 8, storage256> accumulator, simd<std::uint8_t, 32, storage256> a, simd<std::uint8_t, 32, storage256> b) {
  return native::dpbuud<storage256>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::uint32_t, 4, storage128> reject_dpbuuds_128(simd<std::uint32_t, 4, storage128> accumulator, simd<std::uint8_t, 16, storage128> a, simd<std::uint8_t, 16, storage128> b) {
  return native::dpbuuds<storage128>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::uint32_t, 8, storage256> reject_dpbuuds_256(simd<std::uint32_t, 8, storage256> accumulator, simd<std::uint8_t, 32, storage256> a, simd<std::uint8_t, 32, storage256> b) {
  return native::dpbuuds<storage256>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 4, storage128> reject_dpwsud_128(simd<std::int32_t, 4, storage128> accumulator, simd<std::int16_t, 8, storage128> a, simd<std::uint16_t, 8, storage128> b) {
  return native::dpwsud<storage128>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 8, storage256> reject_dpwsud_256(simd<std::int32_t, 8, storage256> accumulator, simd<std::int16_t, 16, storage256> a, simd<std::uint16_t, 16, storage256> b) {
  return native::dpwsud<storage256>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 4, storage128> reject_dpwsuds_128(simd<std::int32_t, 4, storage128> accumulator, simd<std::int16_t, 8, storage128> a, simd<std::uint16_t, 8, storage128> b) {
  return native::dpwsuds<storage128>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 8, storage256> reject_dpwsuds_256(simd<std::int32_t, 8, storage256> accumulator, simd<std::int16_t, 16, storage256> a, simd<std::uint16_t, 16, storage256> b) {
  return native::dpwsuds<storage256>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 4, storage128> reject_dpwusd_128(simd<std::int32_t, 4, storage128> accumulator, simd<std::uint16_t, 8, storage128> a, simd<std::int16_t, 8, storage128> b) {
  return native::dpwusd<storage128>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 8, storage256> reject_dpwusd_256(simd<std::int32_t, 8, storage256> accumulator, simd<std::uint16_t, 16, storage256> a, simd<std::int16_t, 16, storage256> b) {
  return native::dpwusd<storage256>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 4, storage128> reject_dpwusds_128(simd<std::int32_t, 4, storage128> accumulator, simd<std::uint16_t, 8, storage128> a, simd<std::int16_t, 8, storage128> b) {
  return native::dpwusds<storage128>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::int32_t, 8, storage256> reject_dpwusds_256(simd<std::int32_t, 8, storage256> accumulator, simd<std::uint16_t, 16, storage256> a, simd<std::int16_t, 16, storage256> b) {
  return native::dpwusds<storage256>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::uint32_t, 4, storage128> reject_dpwuud_128(simd<std::uint32_t, 4, storage128> accumulator, simd<std::uint16_t, 8, storage128> a, simd<std::uint16_t, 8, storage128> b) {
  return native::dpwuud<storage128>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::uint32_t, 8, storage256> reject_dpwuud_256(simd<std::uint32_t, 8, storage256> accumulator, simd<std::uint16_t, 16, storage256> a, simd<std::uint16_t, 16, storage256> b) {
  return native::dpwuud<storage256>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::uint32_t, 4, storage128> reject_dpwuuds_128(simd<std::uint32_t, 4, storage128> accumulator, simd<std::uint16_t, 8, storage128> a, simd<std::uint16_t, 8, storage128> b) {
  return native::dpwuuds<storage128>(accumulator, a, b); // expected-consteval-rejection
}

simd<std::uint32_t, 8, storage256> reject_dpwuuds_256(simd<std::uint32_t, 8, storage256> accumulator, simd<std::uint16_t, 16, storage256> a, simd<std::uint16_t, 16, storage256> b) {
  return native::dpwuuds<storage256>(accumulator, a, b); // expected-consteval-rejection
}

// Weak overloads retain the architectural immediate range.
simd<std::uint64_t,2,storage128> reject_immediate_pclmulqdq_128(simd<std::uint64_t,2,storage128> a,simd<std::uint64_t,2,storage128> b) {
  return native::pclmulqdq<storage128,256>(a,b); // expected-immediate-rejection
}
simd<std::uint64_t,2,storage128> reject_immediate_vpclmulqdq_128(simd<std::uint64_t,2,storage128> a,simd<std::uint64_t,2,storage128> b) {
  return native::vpclmulqdq<storage128,256>(a,b); // expected-immediate-rejection
}
simd<std::uint64_t,4,storage256> reject_immediate_vpclmulqdq_256(simd<std::uint64_t,4,storage256> a,simd<std::uint64_t,4,storage256> b) {
  return native::vpclmulqdq<storage256,256>(a,b); // expected-immediate-rejection
}
simd<std::uint64_t,8,storage512> reject_immediate_vpclmulqdq_512(simd<std::uint64_t,8,storage512> a,simd<std::uint64_t,8,storage512> b) {
  return native::vpclmulqdq<storage512,256>(a,b); // expected-immediate-rejection
}
simd<std::uint8_t,16,storage128> reject_immediate_gf2p8affineqb_128(simd<std::uint8_t,16,storage128> a,simd<std::uint64_t,2,storage128> matrix) {
  return native::gf2p8affineqb<storage128,256>(a,matrix); // expected-immediate-rejection
}
simd<std::uint8_t,16,storage128> reject_immediate_gf2p8affineinvqb_128(simd<std::uint8_t,16,storage128> a,simd<std::uint64_t,2,storage128> matrix) {
  return native::gf2p8affineinvqb<storage128,256>(a,matrix); // expected-immediate-rejection
}
simd<std::uint8_t,32,storage256> reject_immediate_gf2p8affineqb_256(simd<std::uint8_t,32,storage256> a,simd<std::uint64_t,4,storage256> matrix) {
  return native::gf2p8affineqb<storage256,256>(a,matrix); // expected-immediate-rejection
}
simd<std::uint8_t,32,storage256> reject_immediate_gf2p8affineinvqb_256(simd<std::uint8_t,32,storage256> a,simd<std::uint64_t,4,storage256> matrix) {
  return native::gf2p8affineinvqb<storage256,256>(a,matrix); // expected-immediate-rejection
}
simd<std::uint8_t,64,storage512> reject_immediate_gf2p8affineqb_512(simd<std::uint8_t,64,storage512> a,simd<std::uint64_t,8,storage512> matrix) {
  return native::gf2p8affineqb<storage512,256>(a,matrix); // expected-immediate-rejection
}
simd<std::uint8_t,64,storage512> reject_immediate_gf2p8affineinvqb_512(simd<std::uint8_t,64,storage512> a,simd<std::uint64_t,8,storage512> matrix) {
  return native::gf2p8affineinvqb<storage512,256>(a,matrix); // expected-immediate-rejection
}
