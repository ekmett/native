// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/targets.h>
#include <array>
#include <bit>
#include <concepts>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>
#if NATIVE_TEST_IMPORT
import native;
#else
#include <native/packing.h>
#if defined(__x86_64__) || defined(_M_X64)
import native.x86.features;
#endif
#endif

#if !defined(NATIVE_TEST_PROFILE)
#if defined(__AVX512F__)
#define NATIVE_TEST_PROFILE 512
#elif defined(__AVX2__)
#define NATIVE_TEST_PROFILE 256
#elif defined(__ARM_NEON)
#define NATIVE_TEST_PROFILE 128
#else
#define NATIVE_TEST_PROFILE 0
#endif
#endif
#if NATIVE_TEST_PROFILE == 512 && NATIVE_TEST_AVX512_FP16
constexpr auto test_arch = native::avx512_fp16;
#elif NATIVE_TEST_PROFILE == 512 && NATIVE_TEST_BF16
constexpr auto test_arch = native::avx512_bf16;
#elif NATIVE_TEST_PROFILE == 512
constexpr auto test_arch = native::avx512;
#elif NATIVE_TEST_PROFILE == 256
constexpr auto test_arch = native::avx2;
#elif NATIVE_TEST_PROFILE == 128 && NATIVE_TEST_BF16
constexpr auto test_arch = native::neon_bf16;
#elif NATIVE_TEST_PROFILE == 128 && NATIVE_TEST_FP16
constexpr auto test_arch = native::neon_fp16;
#elif NATIVE_TEST_PROFILE == 128
constexpr auto test_arch = native::neon;
#else
constexpr auto test_arch = native::scalar;
#endif

namespace {
  void check(bool value) { if (!value) std::abort(); }

#if defined(__x86_64__) || defined(_M_X64)
#define NATIVE_TARGET_bmi2 "bmi2"
  constexpr auto bmi2_arch = NATIVE_TARGET_ISA(bmi2);
  template<native::isa<> Arch, class T> concept can_pdep = requires(T value) {
    { native::pdep<Arch>(value,value) } noexcept -> std::same_as<T>;
  };
  template<native::isa<> Arch, class T> concept can_pext = requires(T value) {
    { native::pext<Arch>(value,value) } noexcept -> std::same_as<T>;
  };
  static_assert(can_pdep<bmi2_arch,std::uint32_t> && can_pdep<bmi2_arch,std::uint64_t>);
  static_assert(can_pdep<native::avx2,std::uint32_t> && can_pdep<native::avx2,std::uint64_t>);
  static_assert(can_pdep<native::avx2 & bmi2_arch,std::uint32_t> &&
                can_pdep<native::avx2 & bmi2_arch,std::uint64_t>);
  static_assert(can_pdep<native::scalar,std::uint32_t> && can_pdep<native::scalar,std::uint64_t>);
  static_assert(can_pdep<native::isa(native::x86_feature::avx2),std::uint32_t> &&
                can_pdep<native::isa(native::x86_feature::avx2),std::uint64_t>);
  static_assert(can_pext<bmi2_arch,std::uint32_t> && can_pext<bmi2_arch,std::uint64_t>);
  static_assert(can_pext<native::avx2,std::uint32_t> && can_pext<native::avx2,std::uint64_t>);
  static_assert(can_pext<native::avx2 & bmi2_arch,std::uint32_t> &&
                can_pext<native::avx2 & bmi2_arch,std::uint64_t>);
  static_assert(can_pext<native::scalar,std::uint32_t> && can_pext<native::scalar,std::uint64_t>);
  static_assert(can_pext<native::isa(native::x86_feature::avx2),std::uint32_t> &&
                can_pext<native::isa(native::x86_feature::avx2),std::uint64_t>);
  static_assert(!bmi2_arch.has(native::x86_feature::avx2));

  // Weak tags support immediate calls. Requires expressions do not distinguish
  // these from runtime calls; x86_constexpr tests reject actual runtime inputs.
  template<native::isa<> Arch, class T> consteval bool constant_bit_permutation() {
    constexpr T high_bit = T{1} << (std::numeric_limits<T>::digits-1);
    return native::pdep<Arch>(T{5},T{22}) == T{18} &&
      native::pext<Arch>(T{18},T{22}) == T{5} &&
      native::pdep<Arch>(T{1},high_bit) == high_bit &&
      native::pext<Arch>(high_bit,high_bit) == T{1};
  }
  static_assert(constant_bit_permutation<native::scalar,std::uint32_t>() &&
                constant_bit_permutation<native::scalar,std::uint64_t>());
  static_assert(constant_bit_permutation<native::avx2,std::uint32_t>() &&
                constant_bit_permutation<native::avx2,std::uint64_t>());
  static_assert(constant_bit_permutation<native::isa(native::x86_feature::avx2),std::uint32_t>() &&
                constant_bit_permutation<native::isa(native::x86_feature::avx2),std::uint64_t>());

  template<class T> T pdep_oracle(T value, T mask) {
    T result = 0;
    unsigned source = 0;
    for (unsigned bit = 0; bit != std::numeric_limits<T>::digits; ++bit)
      if ((mask >> bit) & 1) result |= ((value >> source++) & 1) << bit;
    return result;
  }

  template<class T> T pext_oracle(T value, T mask) {
    T result = 0;
    unsigned destination = 0;
    for (unsigned bit = 0; bit != std::numeric_limits<T>::digits; ++bit)
      if ((mask >> bit) & 1) result |= ((value >> bit) & 1) << destination++;
    return result;
  }

  NATIVE_TARGET_PUSH(bmi2)
  template<class T>
  __attribute__((noinline)) void bit_permutation_test() {
    constexpr auto width = std::numeric_limits<T>::digits;
    constexpr T all_bits = std::numeric_limits<T>::max();
    constexpr T high_bit = T{1} << (width-1);
    constexpr std::array<T,8> edge_masks{
      0, all_bits, 1, high_bit,
      T(0x5555555555555555ull), T(0xaaaaaaaaaaaaaaaaull),
      high_bit | T{1}, T(0x0123456789abcdefull)
    };
    constexpr std::array<T,6> edge_values{
      0, all_bits, 1, high_bit,
      T(0x0123456789abcdefull), T(0xfedcba9876543210ull)
    };
    std::uint64_t state = 0x123456789abcdef0ull;
    for (unsigned round = 0; round != 8192; ++round) {
      state ^= state << 13; state ^= state >> 7; state ^= state << 17;
      T value = T(state);
      state ^= state << 13; state ^= state >> 7; state ^= state << 17;
      T mask = T(state);
      if (round < edge_masks.size() * edge_values.size()) {
        value = edge_values[round / edge_masks.size()];
        mask = edge_masks[round % edge_masks.size()];
      }
      auto deposited = native::pdep<bmi2_arch>(value,mask);
      auto extracted = native::pext<bmi2_arch>(value,mask);
      check(deposited == pdep_oracle(value,mask));
      check(extracted == pext_oracle(value,mask));
      auto count = unsigned(std::popcount(mask));
      T low_bits = count == width ? all_bits : (T{1} << count)-1;
      check(native::pext<bmi2_arch>(deposited,mask) == (value & low_bits));
      check(native::pdep<bmi2_arch>(extracted,mask) == (value & mask));
      for (unsigned j = 0; j != count; ++j) {
        auto bit = native::pdep<bmi2_arch>(T{1} << j,mask);
        check(std::has_single_bit(bit) && (bit & mask) != 0);
        check(unsigned(std::popcount(mask & (bit-1))) == j);
        check(native::pext<bmi2_arch>(bit,mask) == (T{1} << j));
      }
    }
  }
  NATIVE_TARGET_POP()
#endif

#if NATIVE_TEST_PROFILE != 0
  template <class To, class From, unsigned Bytes> void narrow_test() {
    constexpr auto n = Bytes / sizeof(From);
    using V = native::simd<From,n,test_arch>;
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
  auto cpu = native::observe_x86_capabilities();
  if (native::classify_isa(cpu,bmi2_arch,NATIVE_TARGET_MINIMUM).admitted()) {
    bit_permutation_test<std::uint32_t>();
    bit_permutation_test<std::uint64_t>();
    std::puts("PDEP/PEXT BMI2-only: executed");
  } else {
    std::puts("PDEP/PEXT BMI2-only: skipped (not admitted)");
  }
#endif
#if NATIVE_TEST_PROFILE != 0
  register_test<16>();
#endif
#if NATIVE_TEST_PROFILE >= 256
  register_test<32>();
#endif
#if NATIVE_TEST_PROFILE == 512
  register_test<64>();
#endif
}
