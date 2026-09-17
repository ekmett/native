// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "contract.h"
#include "support/fp_environment.h"
#include <array>
#include <cstdio>
#include <cstring>
#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <cpuid.h>
#endif

#if defined(__AVX__) || defined(__AVX2__) || defined(__AVX512F__)
#error "The dispatcher must compile for the baseline ISA."
#endif

namespace {
  void cpuid(unsigned leaf, unsigned subleaf, unsigned (&r)[4]) {
#if defined(_MSC_VER)
    int native[4]; __cpuidex(native, int(leaf), int(subleaf));
    for (unsigned i = 0; i < 4; ++i) r[i] = unsigned(native[i]);
#else
    __cpuid_count(leaf, subleaf, r[0], r[1], r[2], r[3]);
#endif
  }
  unsigned supported() {
    unsigned r[4]; cpuid(0, 0, r); auto max_leaf = r[0];
    if (max_leaf < 7) return 0;
    cpuid(1, 0, r);
    constexpr unsigned vector_state = (1u << 26) | (1u << 27) | (1u << 28) | (1u << 12);
    if ((r[2] & vector_state) != vector_state) return 0;
#if defined(_MSC_VER)
    auto xcr0 = _xgetbv(0);
#else
    unsigned lo, hi;
    __asm__("xgetbv" : "=a"(lo), "=d"(hi) : "c"(0));
    auto xcr0 = std::uint64_t(lo) | (std::uint64_t(hi) << 32);
#endif
    if ((xcr0 & 6u) != 6u) return 0;
    cpuid(7, 0, r);
    // The actual profile flags require AVX2, FMA and BMI2.
    constexpr unsigned avx2 = (1u << 5) | (1u << 8);
    unsigned result = (r[1] & avx2) == avx2 ? 1u : 0u;
    constexpr unsigned avx512 = (1u << 16) | (1u << 17) | (1u << 30) | (1u << 31);
    if (result && (r[1] & avx512) == avx512 && (xcr0 & 0xe6u) == 0xe6u)
      result |= 2u;
    return result;
  }
  using capture = std::array<std::uint32_t, profile_test::words>;
  bool compare(capture const & a, capture const & b, char const * description) {
    for (std::size_t i = 0; i < a.size(); ++i) if (a[i] != b[i]) {
      std::fprintf(stderr, "%s: column=%zu lane=%zu %08x != %08x\n",
        description, i / profile_test::count, i % profile_test::count,
        unsigned(a[i]), unsigned(b[i]));
      return false;
    }
    return true;
  }
  bool run(profile_test::entry entry, capture & output) {
    return entry(output.data(), output.size()) == output.size();
  }
}

int main(int argc, char ** argv) {
  char const * mode = argc > 1 ? argv[1] : "all";
  if (std::strcmp(mode, "none") == 0) {
    std::puts("No profile entered.");
    return 0;
  }
  unsigned request = std::strcmp(mode, "all") == 0 ? 3u :
    std::strcmp(mode, "avx2") == 0 ? 1u : std::strcmp(mode, "avx512") == 0 ? 2u : 0u;
  if (!request) return 2;
  unsigned admitted = supported();
  if ((request & admitted) == 0 || (request != 3 && (request & admitted) != request)) {
    std::puts("Requested profile is unavailable on this CPU/OS.");
    return 77;
  }
  capture avx2{}, avx512{}, header{};
  auto before = simd::test::read_fp_state();
  {
    simd::test::fp_scope environment(simd::test::fp_mode::flush);
    if (!environment.controls_match()) return 3;
    unsigned active = request & admitted;
    if (active & 1u) {
      if (!run(profile_avx2_import, avx2) || !run(profile_avx2_header, header)) return 4;
      if (!compare(avx2, header, "AVX2 import/header")) return 5;
    }
    if (active & 2u) {
      if (!profile_identities() || !run(profile_avx512_import, avx512) ||
          !run(profile_avx512_header, header)) return 6;
      if (!compare(avx512, header, "AVX512 import/header")) return 7;
    }
    if (active == 3 && !compare(avx2, avx512, "AVX2/AVX512")) return 8;
    if (!environment.controls_match()) return 9;
  }
  if (before != simd::test::read_fp_state()) return 10;
  if (argc > 2) {
    auto * file = std::fopen(argv[2], "wb");
    if (!file) return 11;
    auto const & result = request & admitted & 1u ? avx2 : avx512;
    bool written = std::fwrite(result.data(), sizeof(result[0]), result.size(), file) == result.size();
    bool closed = std::fclose(file) == 0;
    if (!written || !closed) return 12;
  }
  if (argc > 3) {
    auto * file = std::fopen(argv[3], "rb");
    if (!file) return 13;
    capture expected{};
    bool read = std::fread(expected.data(), sizeof(expected[0]), expected.size(), file) == expected.size();
    bool eof = std::fgetc(file) == EOF;
    bool closed = std::fclose(file) == 0;
    if (!read || !eof || !closed) return 14;
    auto const & result = request & admitted & 1u ? avx2 : avx512;
    if (!compare(result, expected, "Supplied/current raw packet")) return 15;
  }
  std::printf("raw profiles=%u words=%zu exact\n", request & admitted, profile_test::words);
  return 0;
}
