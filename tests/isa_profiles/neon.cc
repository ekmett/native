// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "contract.h"
#include <array>
#include <cstdio>
#include <type_traits>
import simd.wide;
import simd.scalar;
#include "support/fp_environment.h"
import simd;

static_assert(!std::is_same_v<simd::vec<float,1,simd::scalar>,simd::vec<float,1,simd::neon>>);
static_assert(sizeof(simd::vec<float,4,simd::neon>) == 16);
static_assert(sizeof(simd::vec<std::uint8_t,16,simd::neon>) == 16);
static_assert(std::is_same_v<decltype(simd::vec{simd::neon{},1.f,2.f,3.f,4.f}),simd::vec<float,4,simd::neon>>);
static_assert(std::is_same_v<decltype(simd::vec<float,4,simd::neon>{} < simd::vec<float,4,simd::neon>{}),simd::vec<float,4,simd::neon>::mask>);
static_assert(!simd::vec<float,4,simd::neon>::mask::compact);
template<class Arch> struct family { using value = simd::vec<float,4,Arch>; };
static_assert(std::is_same_v<family<simd::neon>::value,simd::vec<float,4,simd::neon>>);
extern "C" std::size_t profile_neon_header(std::uint32_t *, std::size_t);
extern "C" std::size_t profile_neon_import(std::uint32_t *, std::size_t);

int main(int argc, char ** argv) {
  using capture = std::array<std::uint32_t, profile_test::words>;
  capture header{}, imported{}, baseline{};
  auto before = simd::test::read_fp_state();
  {
    simd::test::fp_scope scope(simd::test::fp_mode::flush);
    if (!scope.controls_match()) return 1;
    if (profile_neon_header(header.data(), header.size()) != header.size() ||
        profile_neon_import(imported.data(), imported.size()) != imported.size()) return 2;
    if (header != imported) return 3;
    if (!scope.controls_match()) return 4;
  }
  if (before != simd::test::read_fp_state()) return 5;
  if (argc != 2 && argc != 3) return 6;
  auto * out = std::fopen(argv[1], "wb");
  if (!out) return 7;
  bool written = std::fwrite(imported.data(), sizeof(imported[0]), imported.size(), out) == imported.size();
  bool closed = std::fclose(out) == 0;
  if (!written || !closed) return 8;
  if (argc == 3) {
    auto * in = std::fopen(argv[2], "rb");
    if (!in) return 9;
    bool read = std::fread(baseline.data(), sizeof(baseline[0]), baseline.size(), in) == baseline.size();
    bool eof = std::fgetc(in) == EOF;
    closed = std::fclose(in) == 0;
    if (!read || !eof || !closed) return 10;
    for (std::size_t i = 0; i < imported.size(); ++i) if (imported[i] != baseline[i]) {
      std::fprintf(stderr, "NEON/PC: column=%zu lane=%zu %08x != %08x\n",
        i / profile_test::count, i % profile_test::count, unsigned(imported[i]), unsigned(baseline[i]));
      return 11;
    }
  }
  std::printf("raw neon header/import words=%zu exact; FPCR restored\n", imported.size());
}
