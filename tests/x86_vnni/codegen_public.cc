// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <immintrin.h>
#include <native/attributes.h>
import native.x86.vnni;

constexpr auto vex = native::feature_closure(native::isa<native::x86>{native::x86_feature::avxvnni});
constexpr auto evex = native::feature_closure(native::x86_feature::avx512f & native::x86_feature::avx512vnni);
constexpr auto evexvl = native::feature_closure(evex & native::x86_feature::avx512vl);
constexpr auto both = native::feature_closure(vex & evexvl);
constexpr auto int8 = native::feature_closure(native::isa<native::x86>{native::x86_feature::avxvnniint8});
constexpr auto int16 = native::feature_closure(native::isa<native::x86>{native::x86_feature::avxvnniint16});


extern "C" native_noinline native_target("avxvnni,no-avx512f")
__m128i native_vnni_vex_dpbusd_128_plain(__m128i acc, __m128i a, __m128i b) noexcept {
  return native::dpbusd<vex>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), vex>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), vex>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), vex>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnni,no-avx512f")
__m128i native_vnni_priority_dpbusd_128_plain(__m128i acc, __m128i a, __m128i b) noexcept {
  return native::dpbusd<both>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), both>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), both>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), both>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnni,no-avx512f")
__m256i native_vnni_vex_dpbusd_256_plain(__m256i acc, __m256i a, __m256i b) noexcept {
  return native::dpbusd<vex>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), vex>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), vex>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), vex>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnni,no-avx512f")
__m256i native_vnni_priority_dpbusd_256_plain(__m256i acc, __m256i a, __m256i b) noexcept {
  return native::dpbusd<both>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), both>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), both>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), both>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,avx512vl,no-avxvnni")
__m128i native_vnni_evex_dpbusd_128_plain(__m128i acc, __m128i a, __m128i b) noexcept {
  return native::dpbusd<evexvl>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evexvl>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), evexvl>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), evexvl>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,avx512vl,no-avxvnni")
__m128i native_vnni_evex_dpbusd_128_merge(__m128i acc, unsigned k, __m128i a, __m128i b) noexcept {
  return native::mask_dpbusd<evexvl>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evexvl>::from_native(acc), native::predicate<sizeof(acc) / sizeof(std::int32_t), evexvl>::from_bitset(k), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), evexvl>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), evexvl>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,avx512vl,no-avxvnni")
__m128i native_vnni_evex_dpbusd_128_zero(unsigned k, __m128i acc, __m128i a, __m128i b) noexcept {
  return native::maskz_dpbusd<evexvl>(native::predicate<sizeof(acc) / sizeof(std::int32_t), evexvl>::from_bitset(k), native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evexvl>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), evexvl>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), evexvl>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,avx512vl,no-avxvnni")
__m256i native_vnni_evex_dpbusd_256_plain(__m256i acc, __m256i a, __m256i b) noexcept {
  return native::dpbusd<evexvl>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evexvl>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), evexvl>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), evexvl>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,avx512vl,no-avxvnni")
__m256i native_vnni_evex_dpbusd_256_merge(__m256i acc, unsigned k, __m256i a, __m256i b) noexcept {
  return native::mask_dpbusd<evexvl>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evexvl>::from_native(acc), native::predicate<sizeof(acc) / sizeof(std::int32_t), evexvl>::from_bitset(k), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), evexvl>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), evexvl>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,avx512vl,no-avxvnni")
__m256i native_vnni_evex_dpbusd_256_zero(unsigned k, __m256i acc, __m256i a, __m256i b) noexcept {
  return native::maskz_dpbusd<evexvl>(native::predicate<sizeof(acc) / sizeof(std::int32_t), evexvl>::from_bitset(k), native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evexvl>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), evexvl>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), evexvl>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,no-avxvnni")
__m512i native_vnni_evex_dpbusd_512_plain(__m512i acc, __m512i a, __m512i b) noexcept {
  return native::dpbusd<evex>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evex>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), evex>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), evex>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,no-avxvnni")
__m512i native_vnni_evex_dpbusd_512_merge(__m512i acc, unsigned k, __m512i a, __m512i b) noexcept {
  return native::mask_dpbusd<evex>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evex>::from_native(acc), native::predicate<sizeof(acc) / sizeof(std::int32_t), evex>::from_bitset(k), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), evex>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), evex>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,no-avxvnni")
__m512i native_vnni_evex_dpbusd_512_zero(unsigned k, __m512i acc, __m512i a, __m512i b) noexcept {
  return native::maskz_dpbusd<evex>(native::predicate<sizeof(acc) / sizeof(std::int32_t), evex>::from_bitset(k), native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evex>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), evex>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), evex>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnni,no-avx512f")
__m128i native_vnni_vex_dpbusds_128_plain(__m128i acc, __m128i a, __m128i b) noexcept {
  return native::dpbusds<vex>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), vex>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), vex>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), vex>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnni,no-avx512f")
__m128i native_vnni_priority_dpbusds_128_plain(__m128i acc, __m128i a, __m128i b) noexcept {
  return native::dpbusds<both>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), both>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), both>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), both>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnni,no-avx512f")
__m256i native_vnni_vex_dpbusds_256_plain(__m256i acc, __m256i a, __m256i b) noexcept {
  return native::dpbusds<vex>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), vex>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), vex>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), vex>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnni,no-avx512f")
__m256i native_vnni_priority_dpbusds_256_plain(__m256i acc, __m256i a, __m256i b) noexcept {
  return native::dpbusds<both>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), both>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), both>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), both>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,avx512vl,no-avxvnni")
__m128i native_vnni_evex_dpbusds_128_plain(__m128i acc, __m128i a, __m128i b) noexcept {
  return native::dpbusds<evexvl>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evexvl>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), evexvl>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), evexvl>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,avx512vl,no-avxvnni")
__m128i native_vnni_evex_dpbusds_128_merge(__m128i acc, unsigned k, __m128i a, __m128i b) noexcept {
  return native::mask_dpbusds<evexvl>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evexvl>::from_native(acc), native::predicate<sizeof(acc) / sizeof(std::int32_t), evexvl>::from_bitset(k), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), evexvl>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), evexvl>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,avx512vl,no-avxvnni")
__m128i native_vnni_evex_dpbusds_128_zero(unsigned k, __m128i acc, __m128i a, __m128i b) noexcept {
  return native::maskz_dpbusds<evexvl>(native::predicate<sizeof(acc) / sizeof(std::int32_t), evexvl>::from_bitset(k), native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evexvl>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), evexvl>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), evexvl>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,avx512vl,no-avxvnni")
__m256i native_vnni_evex_dpbusds_256_plain(__m256i acc, __m256i a, __m256i b) noexcept {
  return native::dpbusds<evexvl>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evexvl>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), evexvl>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), evexvl>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,avx512vl,no-avxvnni")
__m256i native_vnni_evex_dpbusds_256_merge(__m256i acc, unsigned k, __m256i a, __m256i b) noexcept {
  return native::mask_dpbusds<evexvl>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evexvl>::from_native(acc), native::predicate<sizeof(acc) / sizeof(std::int32_t), evexvl>::from_bitset(k), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), evexvl>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), evexvl>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,avx512vl,no-avxvnni")
__m256i native_vnni_evex_dpbusds_256_zero(unsigned k, __m256i acc, __m256i a, __m256i b) noexcept {
  return native::maskz_dpbusds<evexvl>(native::predicate<sizeof(acc) / sizeof(std::int32_t), evexvl>::from_bitset(k), native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evexvl>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), evexvl>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), evexvl>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,no-avxvnni")
__m512i native_vnni_evex_dpbusds_512_plain(__m512i acc, __m512i a, __m512i b) noexcept {
  return native::dpbusds<evex>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evex>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), evex>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), evex>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,no-avxvnni")
__m512i native_vnni_evex_dpbusds_512_merge(__m512i acc, unsigned k, __m512i a, __m512i b) noexcept {
  return native::mask_dpbusds<evex>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evex>::from_native(acc), native::predicate<sizeof(acc) / sizeof(std::int32_t), evex>::from_bitset(k), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), evex>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), evex>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,no-avxvnni")
__m512i native_vnni_evex_dpbusds_512_zero(unsigned k, __m512i acc, __m512i a, __m512i b) noexcept {
  return native::maskz_dpbusds<evex>(native::predicate<sizeof(acc) / sizeof(std::int32_t), evex>::from_bitset(k), native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evex>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), evex>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), evex>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnni,no-avx512f")
__m128i native_vnni_vex_dpwssd_128_plain(__m128i acc, __m128i a, __m128i b) noexcept {
  return native::dpwssd<vex>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), vex>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), vex>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), vex>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnni,no-avx512f")
__m128i native_vnni_priority_dpwssd_128_plain(__m128i acc, __m128i a, __m128i b) noexcept {
  return native::dpwssd<both>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), both>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), both>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), both>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnni,no-avx512f")
__m256i native_vnni_vex_dpwssd_256_plain(__m256i acc, __m256i a, __m256i b) noexcept {
  return native::dpwssd<vex>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), vex>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), vex>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), vex>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnni,no-avx512f")
__m256i native_vnni_priority_dpwssd_256_plain(__m256i acc, __m256i a, __m256i b) noexcept {
  return native::dpwssd<both>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), both>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), both>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), both>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,avx512vl,no-avxvnni")
__m128i native_vnni_evex_dpwssd_128_plain(__m128i acc, __m128i a, __m128i b) noexcept {
  return native::dpwssd<evexvl>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evexvl>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), evexvl>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), evexvl>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,avx512vl,no-avxvnni")
__m128i native_vnni_evex_dpwssd_128_merge(__m128i acc, unsigned k, __m128i a, __m128i b) noexcept {
  return native::mask_dpwssd<evexvl>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evexvl>::from_native(acc), native::predicate<sizeof(acc) / sizeof(std::int32_t), evexvl>::from_bitset(k), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), evexvl>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), evexvl>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,avx512vl,no-avxvnni")
__m128i native_vnni_evex_dpwssd_128_zero(unsigned k, __m128i acc, __m128i a, __m128i b) noexcept {
  return native::maskz_dpwssd<evexvl>(native::predicate<sizeof(acc) / sizeof(std::int32_t), evexvl>::from_bitset(k), native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evexvl>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), evexvl>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), evexvl>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,avx512vl,no-avxvnni")
__m256i native_vnni_evex_dpwssd_256_plain(__m256i acc, __m256i a, __m256i b) noexcept {
  return native::dpwssd<evexvl>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evexvl>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), evexvl>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), evexvl>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,avx512vl,no-avxvnni")
__m256i native_vnni_evex_dpwssd_256_merge(__m256i acc, unsigned k, __m256i a, __m256i b) noexcept {
  return native::mask_dpwssd<evexvl>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evexvl>::from_native(acc), native::predicate<sizeof(acc) / sizeof(std::int32_t), evexvl>::from_bitset(k), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), evexvl>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), evexvl>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,avx512vl,no-avxvnni")
__m256i native_vnni_evex_dpwssd_256_zero(unsigned k, __m256i acc, __m256i a, __m256i b) noexcept {
  return native::maskz_dpwssd<evexvl>(native::predicate<sizeof(acc) / sizeof(std::int32_t), evexvl>::from_bitset(k), native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evexvl>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), evexvl>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), evexvl>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,no-avxvnni")
__m512i native_vnni_evex_dpwssd_512_plain(__m512i acc, __m512i a, __m512i b) noexcept {
  return native::dpwssd<evex>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evex>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), evex>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), evex>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,no-avxvnni")
__m512i native_vnni_evex_dpwssd_512_merge(__m512i acc, unsigned k, __m512i a, __m512i b) noexcept {
  return native::mask_dpwssd<evex>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evex>::from_native(acc), native::predicate<sizeof(acc) / sizeof(std::int32_t), evex>::from_bitset(k), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), evex>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), evex>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,no-avxvnni")
__m512i native_vnni_evex_dpwssd_512_zero(unsigned k, __m512i acc, __m512i a, __m512i b) noexcept {
  return native::maskz_dpwssd<evex>(native::predicate<sizeof(acc) / sizeof(std::int32_t), evex>::from_bitset(k), native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evex>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), evex>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), evex>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnni,no-avx512f")
__m128i native_vnni_vex_dpwssds_128_plain(__m128i acc, __m128i a, __m128i b) noexcept {
  return native::dpwssds<vex>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), vex>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), vex>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), vex>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnni,no-avx512f")
__m128i native_vnni_priority_dpwssds_128_plain(__m128i acc, __m128i a, __m128i b) noexcept {
  return native::dpwssds<both>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), both>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), both>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), both>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnni,no-avx512f")
__m256i native_vnni_vex_dpwssds_256_plain(__m256i acc, __m256i a, __m256i b) noexcept {
  return native::dpwssds<vex>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), vex>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), vex>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), vex>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnni,no-avx512f")
__m256i native_vnni_priority_dpwssds_256_plain(__m256i acc, __m256i a, __m256i b) noexcept {
  return native::dpwssds<both>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), both>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), both>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), both>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,avx512vl,no-avxvnni")
__m128i native_vnni_evex_dpwssds_128_plain(__m128i acc, __m128i a, __m128i b) noexcept {
  return native::dpwssds<evexvl>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evexvl>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), evexvl>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), evexvl>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,avx512vl,no-avxvnni")
__m128i native_vnni_evex_dpwssds_128_merge(__m128i acc, unsigned k, __m128i a, __m128i b) noexcept {
  return native::mask_dpwssds<evexvl>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evexvl>::from_native(acc), native::predicate<sizeof(acc) / sizeof(std::int32_t), evexvl>::from_bitset(k), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), evexvl>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), evexvl>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,avx512vl,no-avxvnni")
__m128i native_vnni_evex_dpwssds_128_zero(unsigned k, __m128i acc, __m128i a, __m128i b) noexcept {
  return native::maskz_dpwssds<evexvl>(native::predicate<sizeof(acc) / sizeof(std::int32_t), evexvl>::from_bitset(k), native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evexvl>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), evexvl>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), evexvl>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,avx512vl,no-avxvnni")
__m256i native_vnni_evex_dpwssds_256_plain(__m256i acc, __m256i a, __m256i b) noexcept {
  return native::dpwssds<evexvl>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evexvl>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), evexvl>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), evexvl>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,avx512vl,no-avxvnni")
__m256i native_vnni_evex_dpwssds_256_merge(__m256i acc, unsigned k, __m256i a, __m256i b) noexcept {
  return native::mask_dpwssds<evexvl>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evexvl>::from_native(acc), native::predicate<sizeof(acc) / sizeof(std::int32_t), evexvl>::from_bitset(k), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), evexvl>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), evexvl>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,avx512vl,no-avxvnni")
__m256i native_vnni_evex_dpwssds_256_zero(unsigned k, __m256i acc, __m256i a, __m256i b) noexcept {
  return native::maskz_dpwssds<evexvl>(native::predicate<sizeof(acc) / sizeof(std::int32_t), evexvl>::from_bitset(k), native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evexvl>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), evexvl>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), evexvl>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,no-avxvnni")
__m512i native_vnni_evex_dpwssds_512_plain(__m512i acc, __m512i a, __m512i b) noexcept {
  return native::dpwssds<evex>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evex>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), evex>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), evex>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,no-avxvnni")
__m512i native_vnni_evex_dpwssds_512_merge(__m512i acc, unsigned k, __m512i a, __m512i b) noexcept {
  return native::mask_dpwssds<evex>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evex>::from_native(acc), native::predicate<sizeof(acc) / sizeof(std::int32_t), evex>::from_bitset(k), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), evex>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), evex>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avx512f,avx512vnni,no-avxvnni")
__m512i native_vnni_evex_dpwssds_512_zero(unsigned k, __m512i acc, __m512i a, __m512i b) noexcept {
  return native::maskz_dpwssds<evex>(native::predicate<sizeof(acc) / sizeof(std::int32_t), evex>::from_bitset(k), native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), evex>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), evex>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), evex>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnniint8,no-avx512f")
__m128i native_vnni_int8_dpbssd_128_plain(__m128i acc, __m128i a, __m128i b) noexcept {
  return native::dpbssd<int8>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), int8>::from_native(acc), native::simd<std::int8_t, sizeof(a) / sizeof(std::int8_t), int8>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), int8>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnniint8,no-avx512f")
__m256i native_vnni_int8_dpbssd_256_plain(__m256i acc, __m256i a, __m256i b) noexcept {
  return native::dpbssd<int8>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), int8>::from_native(acc), native::simd<std::int8_t, sizeof(a) / sizeof(std::int8_t), int8>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), int8>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnniint8,no-avx512f")
__m128i native_vnni_int8_dpbssds_128_plain(__m128i acc, __m128i a, __m128i b) noexcept {
  return native::dpbssds<int8>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), int8>::from_native(acc), native::simd<std::int8_t, sizeof(a) / sizeof(std::int8_t), int8>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), int8>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnniint8,no-avx512f")
__m256i native_vnni_int8_dpbssds_256_plain(__m256i acc, __m256i a, __m256i b) noexcept {
  return native::dpbssds<int8>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), int8>::from_native(acc), native::simd<std::int8_t, sizeof(a) / sizeof(std::int8_t), int8>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), int8>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnniint8,no-avx512f")
__m128i native_vnni_int8_dpbsud_128_plain(__m128i acc, __m128i a, __m128i b) noexcept {
  return native::dpbsud<int8>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), int8>::from_native(acc), native::simd<std::int8_t, sizeof(a) / sizeof(std::int8_t), int8>::from_native(a), native::simd<std::uint8_t, sizeof(b) / sizeof(std::uint8_t), int8>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnniint8,no-avx512f")
__m256i native_vnni_int8_dpbsud_256_plain(__m256i acc, __m256i a, __m256i b) noexcept {
  return native::dpbsud<int8>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), int8>::from_native(acc), native::simd<std::int8_t, sizeof(a) / sizeof(std::int8_t), int8>::from_native(a), native::simd<std::uint8_t, sizeof(b) / sizeof(std::uint8_t), int8>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnniint8,no-avx512f")
__m128i native_vnni_int8_dpbsuds_128_plain(__m128i acc, __m128i a, __m128i b) noexcept {
  return native::dpbsuds<int8>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), int8>::from_native(acc), native::simd<std::int8_t, sizeof(a) / sizeof(std::int8_t), int8>::from_native(a), native::simd<std::uint8_t, sizeof(b) / sizeof(std::uint8_t), int8>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnniint8,no-avx512f")
__m256i native_vnni_int8_dpbsuds_256_plain(__m256i acc, __m256i a, __m256i b) noexcept {
  return native::dpbsuds<int8>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), int8>::from_native(acc), native::simd<std::int8_t, sizeof(a) / sizeof(std::int8_t), int8>::from_native(a), native::simd<std::uint8_t, sizeof(b) / sizeof(std::uint8_t), int8>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnniint8,no-avx512f")
__m128i native_vnni_int8_dpbuud_128_plain(__m128i acc, __m128i a, __m128i b) noexcept {
  return native::dpbuud<int8>(native::simd<std::uint32_t, sizeof(acc) / sizeof(std::uint32_t), int8>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), int8>::from_native(a), native::simd<std::uint8_t, sizeof(b) / sizeof(std::uint8_t), int8>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnniint8,no-avx512f")
__m256i native_vnni_int8_dpbuud_256_plain(__m256i acc, __m256i a, __m256i b) noexcept {
  return native::dpbuud<int8>(native::simd<std::uint32_t, sizeof(acc) / sizeof(std::uint32_t), int8>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), int8>::from_native(a), native::simd<std::uint8_t, sizeof(b) / sizeof(std::uint8_t), int8>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnniint8,no-avx512f")
__m128i native_vnni_int8_dpbuuds_128_plain(__m128i acc, __m128i a, __m128i b) noexcept {
  return native::dpbuuds<int8>(native::simd<std::uint32_t, sizeof(acc) / sizeof(std::uint32_t), int8>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), int8>::from_native(a), native::simd<std::uint8_t, sizeof(b) / sizeof(std::uint8_t), int8>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnniint8,no-avx512f")
__m256i native_vnni_int8_dpbuuds_256_plain(__m256i acc, __m256i a, __m256i b) noexcept {
  return native::dpbuuds<int8>(native::simd<std::uint32_t, sizeof(acc) / sizeof(std::uint32_t), int8>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), int8>::from_native(a), native::simd<std::uint8_t, sizeof(b) / sizeof(std::uint8_t), int8>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnniint16,no-avx512f")
__m128i native_vnni_int16_dpwsud_128_plain(__m128i acc, __m128i a, __m128i b) noexcept {
  return native::dpwsud<int16>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), int16>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), int16>::from_native(a), native::simd<std::uint16_t, sizeof(b) / sizeof(std::uint16_t), int16>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnniint16,no-avx512f")
__m256i native_vnni_int16_dpwsud_256_plain(__m256i acc, __m256i a, __m256i b) noexcept {
  return native::dpwsud<int16>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), int16>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), int16>::from_native(a), native::simd<std::uint16_t, sizeof(b) / sizeof(std::uint16_t), int16>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnniint16,no-avx512f")
__m128i native_vnni_int16_dpwsuds_128_plain(__m128i acc, __m128i a, __m128i b) noexcept {
  return native::dpwsuds<int16>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), int16>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), int16>::from_native(a), native::simd<std::uint16_t, sizeof(b) / sizeof(std::uint16_t), int16>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnniint16,no-avx512f")
__m256i native_vnni_int16_dpwsuds_256_plain(__m256i acc, __m256i a, __m256i b) noexcept {
  return native::dpwsuds<int16>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), int16>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), int16>::from_native(a), native::simd<std::uint16_t, sizeof(b) / sizeof(std::uint16_t), int16>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnniint16,no-avx512f")
__m128i native_vnni_int16_dpwusd_128_plain(__m128i acc, __m128i a, __m128i b) noexcept {
  return native::dpwusd<int16>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), int16>::from_native(acc), native::simd<std::uint16_t, sizeof(a) / sizeof(std::uint16_t), int16>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), int16>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnniint16,no-avx512f")
__m256i native_vnni_int16_dpwusd_256_plain(__m256i acc, __m256i a, __m256i b) noexcept {
  return native::dpwusd<int16>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), int16>::from_native(acc), native::simd<std::uint16_t, sizeof(a) / sizeof(std::uint16_t), int16>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), int16>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnniint16,no-avx512f")
__m128i native_vnni_int16_dpwusds_128_plain(__m128i acc, __m128i a, __m128i b) noexcept {
  return native::dpwusds<int16>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), int16>::from_native(acc), native::simd<std::uint16_t, sizeof(a) / sizeof(std::uint16_t), int16>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), int16>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnniint16,no-avx512f")
__m256i native_vnni_int16_dpwusds_256_plain(__m256i acc, __m256i a, __m256i b) noexcept {
  return native::dpwusds<int16>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), int16>::from_native(acc), native::simd<std::uint16_t, sizeof(a) / sizeof(std::uint16_t), int16>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), int16>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnniint16,no-avx512f")
__m128i native_vnni_int16_dpwuud_128_plain(__m128i acc, __m128i a, __m128i b) noexcept {
  return native::dpwuud<int16>(native::simd<std::uint32_t, sizeof(acc) / sizeof(std::uint32_t), int16>::from_native(acc), native::simd<std::uint16_t, sizeof(a) / sizeof(std::uint16_t), int16>::from_native(a), native::simd<std::uint16_t, sizeof(b) / sizeof(std::uint16_t), int16>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnniint16,no-avx512f")
__m256i native_vnni_int16_dpwuud_256_plain(__m256i acc, __m256i a, __m256i b) noexcept {
  return native::dpwuud<int16>(native::simd<std::uint32_t, sizeof(acc) / sizeof(std::uint32_t), int16>::from_native(acc), native::simd<std::uint16_t, sizeof(a) / sizeof(std::uint16_t), int16>::from_native(a), native::simd<std::uint16_t, sizeof(b) / sizeof(std::uint16_t), int16>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnniint16,no-avx512f")
__m128i native_vnni_int16_dpwuuds_128_plain(__m128i acc, __m128i a, __m128i b) noexcept {
  return native::dpwuuds<int16>(native::simd<std::uint32_t, sizeof(acc) / sizeof(std::uint32_t), int16>::from_native(acc), native::simd<std::uint16_t, sizeof(a) / sizeof(std::uint16_t), int16>::from_native(a), native::simd<std::uint16_t, sizeof(b) / sizeof(std::uint16_t), int16>::from_native(b)).to_native();
}

extern "C" native_noinline native_target("avxvnniint16,no-avx512f")
__m256i native_vnni_int16_dpwuuds_256_plain(__m256i acc, __m256i a, __m256i b) noexcept {
  return native::dpwuuds<int16>(native::simd<std::uint32_t, sizeof(acc) / sizeof(std::uint32_t), int16>::from_native(acc), native::simd<std::uint16_t, sizeof(a) / sizeof(std::uint16_t), int16>::from_native(a), native::simd<std::uint16_t, sizeof(b) / sizeof(std::uint16_t), int16>::from_native(b)).to_native();
}

extern "C" native_noinline unsigned long long native_vnni_baseline(unsigned long long value) noexcept {
  return (value >> 3) ^ (value + 17);
}
