// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "oracle.h"

namespace f16c_fixture {
  constexpr auto arch = native::feature_closure(native::isa<native::x86>{native::x86_feature::f16c});
  static_assert(!arch.has(native::x86_feature::avx2));
  static_assert(!native::feature_closure(arch).has(native::x86_feature::avx512fp16));
  static_assert(std::same_as<decltype(native::cvtss_sh<arch, 255>(0.f)), std::uint16_t>);
  static_assert(std::same_as<decltype(native::cvtsh_ss<arch>(0)), float>);

#include "api_checks.h"

  struct mxcsr_guard {
    unsigned saved = _mm_getcsr();
    ~mxcsr_guard() { _mm_setcsr(saved); }
  };
  constexpr unsigned status_mask = 0x3f, exception_masks = 0x1f80;
  unsigned controls(unsigned saved, unsigned rc, bool ftz, bool daz) {
    return (saved & ~(status_mask | 0x6000u | 0x8040u)) | exception_masks |
      (rc << 13) | (ftz ? 0x8000u : 0) | (daz ? 0x40u : 0);
  }
  using halfs = std::array<std::uint16_t, 8>;
  using singles = std::array<std::uint32_t, 8>;
  struct widened { singles scalar{}, four{}, eight{}; unsigned scalar_flags{}, four_flags{}, eight_flags{}; };
  struct narrowed { halfs scalar{}, four{}, eight{}; unsigned scalar_flags{}, four_flags{}, eight_flags{}; };

  // Pointer/reference arguments keep vector ABIs behind runtime admission.
  native_target("f16c,no-avx2,no-avx512fp16") native_noinline
  widened evaluate_widen(halfs const& input, unsigned csr) {
    widened out;
    __m128i h;
    __builtin_memcpy(&h, input.data(), 16);
    _mm_setcsr(csr);
    for (unsigned i = 0; i != 8; ++i)
      out.scalar[i] = std::bit_cast<std::uint32_t>(native::cvtsh_ss<arch>(input[i]));
    out.scalar_flags = _mm_getcsr();
    _mm_setcsr(csr);
    __m128 lo = native::cvtph_ps<arch, 4>(native::simd<native::fp16, 4, arch>::from_native(h)).to_native();
    // Upper words must be ignored, including signaling NaNs.
    __m128i upper = _mm_srli_si128(h, 8);
    __m128 hi = native::cvtph_ps<arch, 4>(native::simd<native::fp16, 4, arch>::from_native(upper)).to_native();
    __builtin_memcpy(out.four.data(), &lo, 16);
    __builtin_memcpy(out.four.data() + 4, &hi, 16);
    out.four_flags = _mm_getcsr();
    _mm_setcsr(csr);
    __m256 all = native::cvtph_ps<arch, 8>(native::simd<native::fp16, 8, arch>::from_native(h)).to_native();
    __builtin_memcpy(out.eight.data(), &all, 32);
    out.eight_flags = _mm_getcsr();
    return out;
  }

  template<unsigned Imm8>
  native_target("f16c,no-avx2,no-avx512fp16") native_noinline
  narrowed evaluate_narrow(singles const& input, unsigned csr) {
    narrowed out;
    __m128 lo, hi; __m256 all;
    __builtin_memcpy(&lo, input.data(), 16);
    __builtin_memcpy(&hi, input.data() + 4, 16);
    __builtin_memcpy(&all, input.data(), 32);
    _mm_setcsr(csr);
    for (unsigned i = 0; i != 8; ++i)
      out.scalar[i] = native::cvtss_sh<arch, Imm8>(std::bit_cast<float>(input[i]));
    out.scalar_flags = _mm_getcsr();
    _mm_setcsr(csr);
    auto a = native::cvtps_ph<arch, Imm8>(native::simd<float, sizeof(lo) / sizeof(float), arch>::from_native(lo)).to_native(), b = native::cvtps_ph<arch, Imm8>(native::simd<float, sizeof(hi) / sizeof(float), arch>::from_native(hi)).to_native();
    __builtin_memcpy(out.four.data(), &a, 8);
    __builtin_memcpy(out.four.data() + 4, &b, 8);
    out.four_flags = _mm_getcsr();
    // Observe the cleared high words as well as the converted low words.
    if (_mm_cvtsi128_si64(_mm_srli_si128(a, 8)) || _mm_cvtsi128_si64(_mm_srli_si128(b, 8)))
      out.four[0] ^= 0xffff;
    _mm_setcsr(csr);
    auto c = native::cvtps_ph<arch, Imm8>(native::simd<float, sizeof(all) / sizeof(float), arch>::from_native(all)).to_native();
    __builtin_memcpy(out.eight.data(), &c, 16);
    out.eight_flags = _mm_getcsr();
    return out;
  }

  bool check_widen(unsigned csr) {
    for (unsigned base = 0; base != 65536; base += 8) {
      halfs h; singles expected;
      unsigned flags = 0;
      for (unsigned i = 0; i != 8; ++i) {
        h[i] = std::uint16_t(base + i);
        expected[i] = widen(h[i]);
        if ((h[i] & 0x7c00) == 0x7c00 && (h[i] & 1023) && !(h[i] & 512)) flags |= 1;
      }
      auto actual = evaluate_widen(h, csr);
      if (actual.scalar != expected || actual.four != expected || actual.eight != expected ||
          actual.scalar_flags != (csr | flags) || actual.four_flags != (csr | flags) ||
          actual.eight_flags != (csr | flags)) {
        std::printf("widen failure at %04x, MXCSR=%08x\n", base, csr); return false;
      }
    }
    return true;
  }

  template<unsigned Imm8> bool check_narrow(singles const& in, unsigned csr) {
    unsigned mode = (Imm8 & 4) ? (csr >> 13) & 3 : Imm8 & 3;
    halfs expected;
    for (unsigned i = 0; i != 8; ++i) expected[i] = narrow(in[i], mode, csr & 0x40);
    auto got = evaluate_narrow<Imm8>(in, csr);
    if (got.scalar != expected || got.four != expected || got.eight != expected ||
        (got.scalar_flags & ~status_mask) != csr || got.scalar_flags != got.four_flags ||
        got.scalar_flags != got.eight_flags) {
      for (unsigned i = 0; i != 8; ++i)
        std::printf("narrow imm=%u csr=%08x lane=%u input=%08x expected=%04x got=%04x/%04x/%04x flags=%x/%x/%x\n",
          Imm8, csr, i, in[i], expected[i], got.scalar[i], got.four[i], got.eight[i],
          got.scalar_flags, got.four_flags, got.eight_flags);
      return false;
    }
    return true;
  }
  bool rounding_modes(singles const& input, unsigned csr) {
    return check_narrow<0>(input, csr) && check_narrow<1>(input, csr) &&
      check_narrow<2>(input, csr) && check_narrow<3>(input, csr) && check_narrow<4>(input, csr);
  }

  template<std::size_t... I> bool immediate_bytes(unsigned csr, std::index_sequence<I...>) {
    singles in{0x3f801000, 0xbf801000, 1, 0x80000001, 0x7f800001, 0x7fc12345, 0x47800000, 0x33000000};
    return (check_narrow<unsigned(I)>(in, csr) && ...);
  }

  // Deliberately discard results. Every instruction must still affect MXCSR.
  template<unsigned Imm8>
  native_target("f16c,no-avx2,no-avx512fp16") native_noinline
  unsigned discarded_narrow(std::uint32_t bits, unsigned csr, unsigned lanes) {
    float a = std::bit_cast<float>(bits);
    _mm_setcsr(csr);
    if (lanes == 1) (void)native::cvtss_sh<arch, Imm8>(a);
    else if (lanes == 4) (void)native::cvtps_ph<arch, Imm8>(native::simd<float, sizeof(_mm_set1_ps(a)) / sizeof(float), arch>::from_native(_mm_set1_ps(a))).to_native();
    else (void)native::cvtps_ph<arch, Imm8>(native::simd<float, sizeof(_mm256_set1_ps(a)) / sizeof(float), arch>::from_native(_mm256_set1_ps(a))).to_native();
    return _mm_getcsr();
  }
  native_target("f16c,no-avx2,no-avx512fp16") native_noinline
  unsigned discarded_widen(std::uint16_t bits, unsigned csr, unsigned lanes) {
    _mm_setcsr(csr);
    if (lanes == 1) (void)native::cvtsh_ss<arch>(bits);
    else if (lanes == 4) (void)native::cvtph_ps<arch, 4>(native::simd<native::fp16, 4, arch>::from_native(_mm_set1_epi16(short(bits)))).to_native();
    else (void)native::cvtph_ps<arch, 8>(native::simd<native::fp16, 8, arch>::from_native(_mm_set1_epi16(short(bits)))).to_native();
    return _mm_getcsr();
  }
  native_target("f16c,no-avx2,no-avx512fp16") native_noinline
  bool ignored_upper(unsigned csr) {
    auto h = _mm_set_epi16(0x7c01, 0x7c01, 0x7c01, 0x7c01, 0x3c00, 0, 1, 0x7c00);
    _mm_setcsr(csr);
    (void)native::cvtph_ps<arch, 4>(native::simd<native::fp16, 4, arch>::from_native(h)).to_native();
    if (_mm_getcsr() != csr) return false;
    (void)native::cvtph_ps<arch, 8>(native::simd<native::fp16, 8, arch>::from_native(h)).to_native();
    return _mm_getcsr() == (csr | 1);
  }
  bool status_checks(unsigned csr) {
    struct example { std::uint32_t bits; unsigned flags; };
    constexpr example cases[] = {
      {0, 0}, {0x80000000, 0}, {0x3f800000, 0}, {0x33800000, 0}, // exact half subnormal
      {0x7f800000, 0}, {0xff800000, 0}, {0x7fc00001, 0}, {0xffd23456, 0},
      {0x7f800001, 1}, {0xff800001, 1}, // signaling NaN, including discarded payload
      {0x3f801000, 0x20}, {0x47800000, 0x28}, // inexact and overflow
      {0x33000000, 0x30}, {0x80000001, 0x32}, {1, 0x32} // tiny and denormal
    };
    for (auto [bits, flags] : cases) for (unsigned lanes : {1u, 4u, 8u}) {
      unsigned want = csr | flags;
      auto got = discarded_narrow<0>(bits, csr, lanes);
      auto no_exc_bit = discarded_narrow<8>(bits, csr, lanes);
      if (got != want || no_exc_bit != want) {
        std::printf("status input=%08x lanes=%u expected=%08x got=%08x imm8=%08x\n", bits, lanes, want, got, no_exc_bit);
        return false;
      }
      singles in; in.fill(bits);
      auto used = evaluate_narrow<0>(in, csr);
      if (used.scalar_flags != want || used.four_flags != want || used.eight_flags != want) return false;
    }
    for (unsigned lanes : {1u, 4u, 8u}) {
      if (discarded_widen(0x7c01, csr, lanes) != (csr | 1) ||
          discarded_widen(1, csr, lanes) != csr ||
          discarded_widen(0x7e01, csr, lanes) != csr ||
          discarded_narrow<2>(1, csr | 0x40, lanes) != (csr | 0x40) ||
          discarded_narrow<0>(0x33800000, csr | 0x8000, lanes) != (csr | 0x8000) ||
          discarded_narrow<0>(0x3f801000, csr | 5, lanes) != (csr | 0x25)) return false;
    }
    return true;
  }

  int run() {
    auto cpu = native::observe_x86_capabilities();
    auto admission = native::classify_isa(cpu, arch);
    std::printf("F16C=%d AVX=%d XCR0=%llx observed=%d AVX512FP16=%d\n",
      cpu.present.has(native::x86_feature::f16c), cpu.present.has(native::x86_feature::avx),
      static_cast<unsigned long long>(cpu.xcr0), cpu.xcr0_observed,
      cpu.present.has(native::x86_feature::avx512fp16));
    if (!admission.admitted()) { std::puts("SKIP: F16C CPU/OS state unavailable"); return 77; }
    unsigned saved = _mm_getcsr();
    {
      mxcsr_guard guard;
      unsigned csr = controls(saved, 0, false, false);
      if (!status_checks(csr) || !ignored_upper(csr)) return 1;
      for (unsigned rc = 0; rc != 4; ++rc) {
        for (unsigned environment = 0; environment != 4; ++environment) {
          csr = controls(saved, rc, environment & 1, environment & 2);
          if (!check_widen(csr)) return 1;
          if (!immediate_bytes(csr, std::make_index_sequence<256>{})) return 1;
          // Neighbors of every positive half value and both sides of every tie;
          // mirror the sign.
          for (unsigned h = 0; h < 0x7c00; ++h) {
            unsigned a = widen(std::uint16_t(h));
            unsigned b = h == 0x7bff ? 0x47800000 : widen(std::uint16_t(h + 1));
            unsigned midpoint;
            // Decode to double only to construct exact binary32 midpoints. These
            // additions are exact and all values are normal in binary32/binary64.
            double lo = std::bit_cast<float>(a), hi = std::bit_cast<float>(b);
            midpoint = std::bit_cast<std::uint32_t>(float((lo + hi) * 0.5));
            singles input{a, a ? a - 1 : 0, a + 1, midpoint - 1, midpoint, midpoint + 1, b, b + 1};
            if (!rounding_modes(input, csr)) return 1;
            for (auto& x : input) x |= 0x80000000u;
            if (!rounding_modes(input, csr)) return 1;
          }
          std::uint32_t state = 0x31415927;
          for (unsigned trial = 0; trial != 4096; ++trial) {
            singles input;
            for (auto& x : input) { state ^= state << 13; state ^= state >> 17; state ^= state << 5; x = state; }
            if (!rounding_modes(input, csr)) return 1;
          }
        }
      }
    }
    if (_mm_getcsr() != saved) { std::puts("MXCSR restoration failed"); return 1; }
    std::puts("All 65536 half patterns, all rounding modes/immediate bytes, half boundaries/ties, random bits and MXCSR checks passed");
    return 0;
  }
}
