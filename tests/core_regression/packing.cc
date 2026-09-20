// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <simd/targets.h>
#include <array>
#include <bit>
#include <concepts>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>
#if SIMD_TEST_IMPORT
import simd;
#else
#include <simd/packing.h>
#if defined(__x86_64__) || defined(_M_X64)
import simd.cpu.x86;
#endif
#endif

#if !defined(SIMD_TEST_PROFILE)
#if defined(__AVX512F__)
#define SIMD_TEST_PROFILE 512
#elif defined(__AVX2__)
#define SIMD_TEST_PROFILE 256
#elif defined(__ARM_NEON)
#define SIMD_TEST_PROFILE 128
#else
#define SIMD_TEST_PROFILE 0
#endif
#endif
#if SIMD_TEST_PROFILE == 512 && SIMD_TEST_AVX512_FP16
constexpr auto test_arch = simd::avx512_fp16;
#elif SIMD_TEST_PROFILE == 512 && SIMD_TEST_BF16
constexpr auto test_arch = simd::avx512_bf16;
#elif SIMD_TEST_PROFILE == 512
constexpr auto test_arch = simd::avx512;
#elif SIMD_TEST_PROFILE == 256
constexpr auto test_arch = simd::avx2;
#elif SIMD_TEST_PROFILE == 128 && SIMD_TEST_BF16
constexpr auto test_arch = simd::neon_bf16;
#elif SIMD_TEST_PROFILE == 128 && SIMD_TEST_FP16
constexpr auto test_arch = simd::neon_fp16;
#elif SIMD_TEST_PROFILE == 128
constexpr auto test_arch = simd::neon;
#else
constexpr auto test_arch = simd::scalar;
#endif

namespace {
  void check(bool value) { if (!value) std::abort(); }

#if defined(__x86_64__) || defined(_M_X64)
#define SIMD_TARGET_bmi2 "bmi2"
  constexpr auto bmi2_arch = SIMD_TARGET_ISA(bmi2);
  template<simd::isa Arch> concept can_pdep = requires(std::uint64_t value) {
    { simd::pdep<Arch>(value,value) } noexcept -> std::same_as<std::uint64_t>;
  };
  static_assert(can_pdep<bmi2_arch>);
  static_assert(can_pdep<simd::avx2>);
  static_assert(!can_pdep<simd::scalar>);
  static_assert(!can_pdep<simd::isa(simd::x86_feature::avx2)>);
  static_assert(!bmi2_arch.has(simd::x86_feature::avx2));

  std::uint64_t pdep_oracle(std::uint64_t value, std::uint64_t mask) {
    std::uint64_t result = 0;
    unsigned source = 0;
    for (unsigned bit = 0; bit != 64; ++bit)
      if ((mask >> bit) & 1) result |= ((value >> source++) & 1) << bit;
    return result;
  }

  SIMD_TARGET_PUSH(bmi2)
  __attribute__((noinline)) void pdep_test() {
    std::uint64_t state = 0x123456789abcdef0ull;
    for (unsigned round = 0; round != 8192; ++round) {
      state ^= state << 13; state ^= state >> 7; state ^= state << 17;
      auto value = state;
      state ^= state << 13; state ^= state >> 7; state ^= state << 17;
      auto mask = round == 0 ? 0ull : round == 1 ? ~0ull : state;
      check(simd::pdep<bmi2_arch>(value,mask) == pdep_oracle(value,mask));
      for (unsigned j = 0; j != unsigned(std::popcount(mask)); ++j) {
        auto bit = simd::pdep<bmi2_arch>(std::uint64_t{1} << j,mask);
        check(std::has_single_bit(bit) && (bit & mask) != 0);
        check(unsigned(std::popcount(mask & (bit-1))) == j);
      }
    }
  }
  SIMD_TARGET_POP()
#endif

#if SIMD_TEST_PROFILE != 0
  template <class To, class From, unsigned Bytes> void narrow_test() {
    constexpr auto n = Bytes / sizeof(From);
    using V = simd::vec<From,n,test_arch>;
    std::array<From,n> a{}, b{};
    std::array<To,2*n> actual{};
    for (unsigned round = 0; round != 257; ++round) {
      for (unsigned i = 0; i != n; ++i) {
        a[i] = round == 0 ? std::numeric_limits<From>::max() :
          From(std::uint64_t(round) * 0x9e3779b97f4a7c15ull + i * 0xd1b54a32d192ed03ull);
        b[i] = From(~a[i] + i);
      }
      narrow_concat<To>(V::loadu(a.data()), V::loadu(b.data())).storeu(actual.data());
      for (unsigned i = 0; i != n; ++i) {
        check(actual[i] == To(a[i]));
        check(actual[n+i] == To(b[i]));
      }
    }
  }
  template <unsigned Bytes> void register_test() {
    narrow_test<std::uint8_t,std::uint16_t,Bytes>();
    narrow_test<std::uint16_t,std::uint32_t,Bytes>();
    narrow_test<std::uint32_t,std::uint64_t,Bytes>();
  }
#endif
}

int main() {
#if defined(__x86_64__) || defined(_M_X64)
  auto cpu = simd::observe_x86_capabilities();
  if (simd::classify_isa(cpu,bmi2_arch,SIMD_TARGET_MINIMUM).admitted()) {
    pdep_test();
    std::puts("PDEP BMI2-only: executed");
  } else {
    std::puts("PDEP BMI2-only: skipped (not admitted)");
  }
#endif
#if SIMD_TEST_PROFILE != 0
  register_test<16>();
#endif
#if SIMD_TEST_PROFILE >= 256
  register_test<32>();
#endif
#if SIMD_TEST_PROFILE == 512
  register_test<64>();
#endif
}
