// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/targets.h>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <limits>
#if NATIVE_TEST_IMPORT
import native;
#else
#include <native/packing.h>
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
