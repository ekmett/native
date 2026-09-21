// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "contract.h"
#include <array>
#include <cstdio>
#include <type_traits>
import native.wide;
import native.scalar;
#include "support/fp_environment.h"
import native;

static_assert(!std::is_same_v<native::vec<float,1,native::scalar>,native::vec<float,1,native::neon>>);
static_assert(sizeof(native::vec<float,4,native::neon>) == 16);
static_assert(sizeof(native::vec<std::uint8_t,16,native::neon>) == 16);
static_assert(std::is_same_v<decltype(native::vec<float,4,native::neon>{1.f,2.f,3.f,4.f}),native::vec<float,4,native::neon>>);
static_assert(std::is_same_v<decltype(native::vec<float,4,native::neon>{} < native::vec<float,4,native::neon>{}),native::vec<float,4,native::neon>::mask>);
static_assert(!native::vec<float,4,native::neon>::mask::compact);
template<native::isa Arch> struct family { using value = native::vec<float,4,Arch>; };
static_assert(std::is_same_v<family<native::neon>::value,native::vec<float,4,native::neon>>);
extern "C" std::size_t profile_neon_header(std::uint32_t *, std::size_t);
extern "C" std::size_t profile_neon_import(std::uint32_t *, std::size_t);

int main(int argc, char ** argv) {
  using capture = std::array<std::uint32_t, profile_test::words>;
  capture header{}, imported{}, baseline{};
  auto before = native::test::read_fp_state();
  {
    native::test::fp_scope scope(native::test::fp_mode::flush);
    if (!scope.controls_match()) return 1;
    if (profile_neon_header(header.data(), header.size()) != header.size() ||
        profile_neon_import(imported.data(), imported.size()) != imported.size()) return 2;
    if (header != imported) return 3;
    if (!scope.controls_match()) return 4;
  }
  if (before != native::test::read_fp_state()) return 5;
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
