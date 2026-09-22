// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <immintrin.h>
import native.x86.sha512;
import native.x86.sm3;
import native.x86.sm4;
using namespace native;
#if NATIVE_CRYPTO_FAMILY == 0
#define NATIVE_CRYPTO_TARGET "sha512"
using word = std::uint64_t;
#else
#if NATIVE_CRYPTO_FAMILY == 1
#define NATIVE_CRYPTO_TARGET "sm3"
#else
#define NATIVE_CRYPTO_TARGET "sm4"
#endif
using word = std::uint32_t;
#endif
#if NATIVE_CRYPTO_NEGATIVE == 1
constexpr auto requirements = target_features<x86>("avx2");
#else
constexpr auto requirements = target_features<x86>(NATIVE_CRYPTO_TARGET);
#endif
#if NATIVE_CRYPTO_NEGATIVE == 4
using V = simd<float,4,requirements>;
#else
using V = simd<word,4,requirements>;
#endif
#if NATIVE_CRYPTO_NEGATIVE != 2
[[gnu::target(NATIVE_CRYPTO_TARGET)]]
#endif
auto probe(V a,V b) {
#if NATIVE_CRYPTO_FAMILY == 0
#if NATIVE_CRYPTO_NEGATIVE == 3
  return sha512msg2<requirements>(a.to_native(),b.to_native());
#else
  return sha512msg2<requirements>(a,b);
#endif
#elif NATIVE_CRYPTO_FAMILY == 1
#if NATIVE_CRYPTO_NEGATIVE == 3
  return sm3msg1<requirements>(a.to_native(),b.to_native(),a.to_native());
#elif NATIVE_CRYPTO_NEGATIVE == 5
  return sm3rnds2<requirements,256>(a,b,a);
#else
  return sm3msg1<requirements>(a,b,a);
#endif
#else
#if NATIVE_CRYPTO_NEGATIVE == 3
  return sm4rnds4<requirements>(a.to_native(),b.to_native());
#else
  return sm4rnds4<requirements>(a,b);
#endif
#endif
}
