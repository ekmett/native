// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <immintrin.h>
#if !NATIVE_CRYPTO_RAW
import native.x86.sha512;
import native.x86.sm3;
import native.x86.sm4;
#endif
extern "C" [[gnu::target("sha512"), gnu::noinline]]
void native_sha512msg1_4(std::uint64_t const * a, std::uint64_t const * b, std::uint64_t * out) {
#if NATIVE_CRYPTO_RAW
  auto va = _mm256_loadu_si256(reinterpret_cast<__m256i const *>(a));
  auto vb = _mm_loadu_si128(reinterpret_cast<__m128i const *>(b));
  auto result = _mm256_sha512msg1_epi64(va, vb);
  _mm256_storeu_si256(reinterpret_cast<__m256i *>(out), result);
#else
  constexpr auto arch = native::target_features<native::x86>("sha512");
  auto va = native::simd<std::uint64_t, 4, arch>::load(a);
  auto vb = native::simd<std::uint64_t, 2, arch>::load(b);
  native::sha512msg1<arch>(va, vb).store(out);
#endif
}
extern "C" [[gnu::target("sha512"), gnu::noinline]]
void native_sha512msg2_4(std::uint64_t const * a, std::uint64_t const * b, std::uint64_t * out) {
#if NATIVE_CRYPTO_RAW
  auto va = _mm256_loadu_si256(reinterpret_cast<__m256i const *>(a));
  auto vb = _mm256_loadu_si256(reinterpret_cast<__m256i const *>(b));
  auto result = _mm256_sha512msg2_epi64(va, vb);
  _mm256_storeu_si256(reinterpret_cast<__m256i *>(out), result);
#else
  constexpr auto arch = native::target_features<native::x86>("sha512");
  auto va = native::simd<std::uint64_t, 4, arch>::load(a);
  auto vb = native::simd<std::uint64_t, 4, arch>::load(b);
  native::sha512msg2<arch>(va, vb).store(out);
#endif
}
extern "C" [[gnu::target("sha512"), gnu::noinline]]
void native_sha512rnds2_4(std::uint64_t const * a, std::uint64_t const * b, std::uint64_t const * c, std::uint64_t * out) {
#if NATIVE_CRYPTO_RAW
  auto va = _mm256_loadu_si256(reinterpret_cast<__m256i const *>(a));
  auto vb = _mm256_loadu_si256(reinterpret_cast<__m256i const *>(b));
  auto vc = _mm_loadu_si128(reinterpret_cast<__m128i const *>(c));
  auto result = _mm256_sha512rnds2_epi64(va, vb, vc);
  _mm256_storeu_si256(reinterpret_cast<__m256i *>(out), result);
#else
  constexpr auto arch = native::target_features<native::x86>("sha512");
  auto va = native::simd<std::uint64_t, 4, arch>::load(a);
  auto vb = native::simd<std::uint64_t, 4, arch>::load(b);
  auto vc = native::simd<std::uint64_t, 2, arch>::load(c);
  native::sha512rnds2<arch>(va, vb, vc).store(out);
#endif
}
extern "C" [[gnu::target("sm3"), gnu::noinline]]
void native_sm3msg1_4(std::uint32_t const * a, std::uint32_t const * b, std::uint32_t const * c, std::uint32_t * out) {
#if NATIVE_CRYPTO_RAW
  auto va = _mm_loadu_si128(reinterpret_cast<__m128i const *>(a));
  auto vb = _mm_loadu_si128(reinterpret_cast<__m128i const *>(b));
  auto vc = _mm_loadu_si128(reinterpret_cast<__m128i const *>(c));
  auto result = _mm_sm3msg1_epi32(va, vb, vc);
  _mm_storeu_si128(reinterpret_cast<__m128i *>(out), result);
#else
  constexpr auto arch = native::target_features<native::x86>("sm3");
  auto va = native::simd<std::uint32_t, 4, arch>::load(a);
  auto vb = native::simd<std::uint32_t, 4, arch>::load(b);
  auto vc = native::simd<std::uint32_t, 4, arch>::load(c);
  native::sm3msg1<arch>(va, vb, vc).store(out);
#endif
}
extern "C" [[gnu::target("sm3"), gnu::noinline]]
void native_sm3msg2_4(std::uint32_t const * a, std::uint32_t const * b, std::uint32_t const * c, std::uint32_t * out) {
#if NATIVE_CRYPTO_RAW
  auto va = _mm_loadu_si128(reinterpret_cast<__m128i const *>(a));
  auto vb = _mm_loadu_si128(reinterpret_cast<__m128i const *>(b));
  auto vc = _mm_loadu_si128(reinterpret_cast<__m128i const *>(c));
  auto result = _mm_sm3msg2_epi32(va, vb, vc);
  _mm_storeu_si128(reinterpret_cast<__m128i *>(out), result);
#else
  constexpr auto arch = native::target_features<native::x86>("sm3");
  auto va = native::simd<std::uint32_t, 4, arch>::load(a);
  auto vb = native::simd<std::uint32_t, 4, arch>::load(b);
  auto vc = native::simd<std::uint32_t, 4, arch>::load(c);
  native::sm3msg2<arch>(va, vb, vc).store(out);
#endif
}
extern "C" [[gnu::target("sm3"), gnu::noinline]]
void native_sm3rnds2_4_0(std::uint32_t const * a, std::uint32_t const * b, std::uint32_t const * c, std::uint32_t * out) {
#if NATIVE_CRYPTO_RAW
  auto va = _mm_loadu_si128(reinterpret_cast<__m128i const *>(a));
  auto vb = _mm_loadu_si128(reinterpret_cast<__m128i const *>(b));
  auto vc = _mm_loadu_si128(reinterpret_cast<__m128i const *>(c));
  auto result = _mm_sm3rnds2_epi32(va, vb, vc, 0);
  _mm_storeu_si128(reinterpret_cast<__m128i *>(out), result);
#else
  constexpr auto arch = native::target_features<native::x86>("sm3");
  auto va = native::simd<std::uint32_t, 4, arch>::load(a);
  auto vb = native::simd<std::uint32_t, 4, arch>::load(b);
  auto vc = native::simd<std::uint32_t, 4, arch>::load(c);
  native::sm3rnds2<arch, 0>(va, vb, vc).store(out);
#endif
}
extern "C" [[gnu::target("sm3"), gnu::noinline]]
void native_sm3rnds2_4_16(std::uint32_t const * a, std::uint32_t const * b, std::uint32_t const * c, std::uint32_t * out) {
#if NATIVE_CRYPTO_RAW
  auto va = _mm_loadu_si128(reinterpret_cast<__m128i const *>(a));
  auto vb = _mm_loadu_si128(reinterpret_cast<__m128i const *>(b));
  auto vc = _mm_loadu_si128(reinterpret_cast<__m128i const *>(c));
  auto result = _mm_sm3rnds2_epi32(va, vb, vc, 16);
  _mm_storeu_si128(reinterpret_cast<__m128i *>(out), result);
#else
  constexpr auto arch = native::target_features<native::x86>("sm3");
  auto va = native::simd<std::uint32_t, 4, arch>::load(a);
  auto vb = native::simd<std::uint32_t, 4, arch>::load(b);
  auto vc = native::simd<std::uint32_t, 4, arch>::load(c);
  native::sm3rnds2<arch, 16>(va, vb, vc).store(out);
#endif
}
extern "C" [[gnu::target("sm3"), gnu::noinline]]
void native_sm3rnds2_4_62(std::uint32_t const * a, std::uint32_t const * b, std::uint32_t const * c, std::uint32_t * out) {
#if NATIVE_CRYPTO_RAW
  auto va = _mm_loadu_si128(reinterpret_cast<__m128i const *>(a));
  auto vb = _mm_loadu_si128(reinterpret_cast<__m128i const *>(b));
  auto vc = _mm_loadu_si128(reinterpret_cast<__m128i const *>(c));
  auto result = _mm_sm3rnds2_epi32(va, vb, vc, 62);
  _mm_storeu_si128(reinterpret_cast<__m128i *>(out), result);
#else
  constexpr auto arch = native::target_features<native::x86>("sm3");
  auto va = native::simd<std::uint32_t, 4, arch>::load(a);
  auto vb = native::simd<std::uint32_t, 4, arch>::load(b);
  auto vc = native::simd<std::uint32_t, 4, arch>::load(c);
  native::sm3rnds2<arch, 62>(va, vb, vc).store(out);
#endif
}
extern "C" [[gnu::target("sm3"), gnu::noinline]]
void native_sm3rnds2_4_255(std::uint32_t const * a, std::uint32_t const * b, std::uint32_t const * c, std::uint32_t * out) {
#if NATIVE_CRYPTO_RAW
  auto va = _mm_loadu_si128(reinterpret_cast<__m128i const *>(a));
  auto vb = _mm_loadu_si128(reinterpret_cast<__m128i const *>(b));
  auto vc = _mm_loadu_si128(reinterpret_cast<__m128i const *>(c));
  auto result = _mm_sm3rnds2_epi32(va, vb, vc, 255);
  _mm_storeu_si128(reinterpret_cast<__m128i *>(out), result);
#else
  constexpr auto arch = native::target_features<native::x86>("sm3");
  auto va = native::simd<std::uint32_t, 4, arch>::load(a);
  auto vb = native::simd<std::uint32_t, 4, arch>::load(b);
  auto vc = native::simd<std::uint32_t, 4, arch>::load(c);
  native::sm3rnds2<arch, 255>(va, vb, vc).store(out);
#endif
}
extern "C" [[gnu::target("sm4"), gnu::noinline]]
void native_sm4rnds4_4(std::uint32_t const * a, std::uint32_t const * b, std::uint32_t * out) {
#if NATIVE_CRYPTO_RAW
  auto va = _mm_loadu_si128(reinterpret_cast<__m128i const *>(a));
  auto vb = _mm_loadu_si128(reinterpret_cast<__m128i const *>(b));
  auto result = _mm_sm4rnds4_epi32(va, vb);
  _mm_storeu_si128(reinterpret_cast<__m128i *>(out), result);
#else
  constexpr auto arch = native::target_features<native::x86>("sm4");
  auto va = native::simd<std::uint32_t, 4, arch>::load(a);
  auto vb = native::simd<std::uint32_t, 4, arch>::load(b);
  native::sm4rnds4<arch>(va, vb).store(out);
#endif
}
extern "C" [[gnu::target("sm4"), gnu::noinline]]
void native_sm4key4_4(std::uint32_t const * a, std::uint32_t const * b, std::uint32_t * out) {
#if NATIVE_CRYPTO_RAW
  auto va = _mm_loadu_si128(reinterpret_cast<__m128i const *>(a));
  auto vb = _mm_loadu_si128(reinterpret_cast<__m128i const *>(b));
  auto result = _mm_sm4key4_epi32(va, vb);
  _mm_storeu_si128(reinterpret_cast<__m128i *>(out), result);
#else
  constexpr auto arch = native::target_features<native::x86>("sm4");
  auto va = native::simd<std::uint32_t, 4, arch>::load(a);
  auto vb = native::simd<std::uint32_t, 4, arch>::load(b);
  native::sm4key4<arch>(va, vb).store(out);
#endif
}
extern "C" [[gnu::target("sm4"), gnu::noinline]]
void native_sm4rnds4_8(std::uint32_t const * a, std::uint32_t const * b, std::uint32_t * out) {
#if NATIVE_CRYPTO_RAW
  auto va = _mm256_loadu_si256(reinterpret_cast<__m256i const *>(a));
  auto vb = _mm256_loadu_si256(reinterpret_cast<__m256i const *>(b));
  auto result = _mm256_sm4rnds4_epi32(va, vb);
  _mm256_storeu_si256(reinterpret_cast<__m256i *>(out), result);
#else
  constexpr auto arch = native::target_features<native::x86>("sm4");
  auto va = native::simd<std::uint32_t, 8, arch>::load(a);
  auto vb = native::simd<std::uint32_t, 8, arch>::load(b);
  native::sm4rnds4<arch>(va, vb).store(out);
#endif
}
extern "C" [[gnu::target("sm4"), gnu::noinline]]
void native_sm4key4_8(std::uint32_t const * a, std::uint32_t const * b, std::uint32_t * out) {
#if NATIVE_CRYPTO_RAW
  auto va = _mm256_loadu_si256(reinterpret_cast<__m256i const *>(a));
  auto vb = _mm256_loadu_si256(reinterpret_cast<__m256i const *>(b));
  auto result = _mm256_sm4key4_epi32(va, vb);
  _mm256_storeu_si256(reinterpret_cast<__m256i *>(out), result);
#else
  constexpr auto arch = native::target_features<native::x86>("sm4");
  auto va = native::simd<std::uint32_t, 8, arch>::load(a);
  auto vb = native::simd<std::uint32_t, 8, arch>::load(b);
  native::sm4key4<arch>(va, vb).store(out);
#endif
}
