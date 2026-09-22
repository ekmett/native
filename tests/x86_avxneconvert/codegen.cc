// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <immintrin.h>
#if NATIVE_NE_PROFILE
#define NATIVE_NE_TARGET "avxneconvert,avx512bf16,avx512vl,avx512bw,avx512dq,fma"
#else
#define NATIVE_NE_TARGET "avxneconvert"
#endif
#if !NATIVE_NE_RAW
import native.x86.avxneconvert;
constexpr auto arch = native::target_features<native::x86>(NATIVE_NE_TARGET);
#endif
extern "C" __attribute__((target(NATIVE_NE_TARGET), noinline)) void native_bcstnebf16_ps_4(
#if NATIVE_NE_RAW
    std::uint16_t const * source,
#else
    native::bf16 const * source,
#endif
    float * output) {
  // Offset a naturally aligned pointer, explicitly exercising a non-vector-aligned address.
  auto input = source + 1;
#if NATIVE_NE_RAW
  _mm_storeu_ps(output, _mm_bcstnebf16_ps(input));
#else
  native::bcstnebf16_ps<arch, 4>(input).store(output);
#endif
}
extern "C" __attribute__((target(NATIVE_NE_TARGET), noinline)) void native_bcstnebf16_ps_8(
#if NATIVE_NE_RAW
    std::uint16_t const * source,
#else
    native::bf16 const * source,
#endif
    float * output) {
  // Offset a naturally aligned pointer, explicitly exercising a non-vector-aligned address.
  auto input = source + 1;
#if NATIVE_NE_RAW
  _mm256_storeu_ps(output, _mm256_bcstnebf16_ps(input));
#else
  native::bcstnebf16_ps<arch, 8>(input).store(output);
#endif
}
extern "C" __attribute__((target(NATIVE_NE_TARGET), noinline)) void native_bcstnesh_ps_4(
#if NATIVE_NE_RAW
    std::uint16_t const * source,
#else
    native::fp16 const * source,
#endif
    float * output) {
  // Offset a naturally aligned pointer, explicitly exercising a non-vector-aligned address.
  auto input = source + 1;
#if NATIVE_NE_RAW
  _mm_storeu_ps(output, _mm_bcstnesh_ps(input));
#else
  native::bcstnesh_ps<arch, 4>(input).store(output);
#endif
}
extern "C" __attribute__((target(NATIVE_NE_TARGET), noinline)) void native_bcstnesh_ps_8(
#if NATIVE_NE_RAW
    std::uint16_t const * source,
#else
    native::fp16 const * source,
#endif
    float * output) {
  // Offset a naturally aligned pointer, explicitly exercising a non-vector-aligned address.
  auto input = source + 1;
#if NATIVE_NE_RAW
  _mm256_storeu_ps(output, _mm256_bcstnesh_ps(input));
#else
  native::bcstnesh_ps<arch, 8>(input).store(output);
#endif
}
extern "C" __attribute__((target(NATIVE_NE_TARGET), noinline)) void native_cvtneebf16_ps_4(
#if NATIVE_NE_RAW
    std::uint16_t const * source,
#else
    native::bf16 const * source,
#endif
    float * output) {
  // Offset a naturally aligned pointer, explicitly exercising a non-vector-aligned address.
  auto input = source + 1;
#if NATIVE_NE_RAW
  _mm_storeu_ps(output, _mm_cvtneebf16_ps(reinterpret_cast<__m128bh const *>(input)));
#else
  native::cvtneebf16_ps<arch, 4>(input).store(output);
#endif
}
extern "C" __attribute__((target(NATIVE_NE_TARGET), noinline)) void native_cvtneebf16_ps_8(
#if NATIVE_NE_RAW
    std::uint16_t const * source,
#else
    native::bf16 const * source,
#endif
    float * output) {
  // Offset a naturally aligned pointer, explicitly exercising a non-vector-aligned address.
  auto input = source + 1;
#if NATIVE_NE_RAW
  _mm256_storeu_ps(output, _mm256_cvtneebf16_ps(reinterpret_cast<__m256bh const *>(input)));
#else
  native::cvtneebf16_ps<arch, 8>(input).store(output);
#endif
}
extern "C" __attribute__((target(NATIVE_NE_TARGET), noinline)) void native_cvtneeph_ps_4(
#if NATIVE_NE_RAW
    std::uint16_t const * source,
#else
    native::fp16 const * source,
#endif
    float * output) {
  // Offset a naturally aligned pointer, explicitly exercising a non-vector-aligned address.
  auto input = source + 1;
#if NATIVE_NE_RAW
  _mm_storeu_ps(output, _mm_cvtneeph_ps(reinterpret_cast<__m128h const *>(input)));
#else
  native::cvtneeph_ps<arch, 4>(input).store(output);
#endif
}
extern "C" __attribute__((target(NATIVE_NE_TARGET), noinline)) void native_cvtneeph_ps_8(
#if NATIVE_NE_RAW
    std::uint16_t const * source,
#else
    native::fp16 const * source,
#endif
    float * output) {
  // Offset a naturally aligned pointer, explicitly exercising a non-vector-aligned address.
  auto input = source + 1;
#if NATIVE_NE_RAW
  _mm256_storeu_ps(output, _mm256_cvtneeph_ps(reinterpret_cast<__m256h const *>(input)));
#else
  native::cvtneeph_ps<arch, 8>(input).store(output);
#endif
}
extern "C" __attribute__((target(NATIVE_NE_TARGET), noinline)) void native_cvtneobf16_ps_4(
#if NATIVE_NE_RAW
    std::uint16_t const * source,
#else
    native::bf16 const * source,
#endif
    float * output) {
  // Offset a naturally aligned pointer, explicitly exercising a non-vector-aligned address.
  auto input = source + 1;
#if NATIVE_NE_RAW
  _mm_storeu_ps(output, _mm_cvtneobf16_ps(reinterpret_cast<__m128bh const *>(input)));
#else
  native::cvtneobf16_ps<arch, 4>(input).store(output);
#endif
}
extern "C" __attribute__((target(NATIVE_NE_TARGET), noinline)) void native_cvtneobf16_ps_8(
#if NATIVE_NE_RAW
    std::uint16_t const * source,
#else
    native::bf16 const * source,
#endif
    float * output) {
  // Offset a naturally aligned pointer, explicitly exercising a non-vector-aligned address.
  auto input = source + 1;
#if NATIVE_NE_RAW
  _mm256_storeu_ps(output, _mm256_cvtneobf16_ps(reinterpret_cast<__m256bh const *>(input)));
#else
  native::cvtneobf16_ps<arch, 8>(input).store(output);
#endif
}
extern "C" __attribute__((target(NATIVE_NE_TARGET), noinline)) void native_cvtneoph_ps_4(
#if NATIVE_NE_RAW
    std::uint16_t const * source,
#else
    native::fp16 const * source,
#endif
    float * output) {
  // Offset a naturally aligned pointer, explicitly exercising a non-vector-aligned address.
  auto input = source + 1;
#if NATIVE_NE_RAW
  _mm_storeu_ps(output, _mm_cvtneoph_ps(reinterpret_cast<__m128h const *>(input)));
#else
  native::cvtneoph_ps<arch, 4>(input).store(output);
#endif
}
extern "C" __attribute__((target(NATIVE_NE_TARGET), noinline)) void native_cvtneoph_ps_8(
#if NATIVE_NE_RAW
    std::uint16_t const * source,
#else
    native::fp16 const * source,
#endif
    float * output) {
  // Offset a naturally aligned pointer, explicitly exercising a non-vector-aligned address.
  auto input = source + 1;
#if NATIVE_NE_RAW
  _mm256_storeu_ps(output, _mm256_cvtneoph_ps(reinterpret_cast<__m256h const *>(input)));
#else
  native::cvtneoph_ps<arch, 8>(input).store(output);
#endif
}
extern "C" __attribute__((target(NATIVE_NE_TARGET), noinline)) void
native_cvtneps_bf16_4(float const * input, std::uint16_t * output) {
#if NATIVE_NE_RAW
  auto r = _mm_cvtneps_avx_pbh(_mm_loadu_ps(input));
  _mm_storel_epi64(reinterpret_cast<__m128i *>(output), (__m128i)r);
#else
  native::cvtneps_bf16(native::simd<float, 4, arch>::load(input)).store_bits(output);
#endif
}
extern "C" __attribute__((target(NATIVE_NE_TARGET), noinline)) void
native_cvtneps_bf16_8(float const * input, std::uint16_t * output) {
#if NATIVE_NE_RAW
  auto r = _mm256_cvtneps_avx_pbh(_mm256_loadu_ps(input));
  _mm_storeu_si128(reinterpret_cast<__m128i *>(output), (__m128i)r);
#else
  native::cvtneps_bf16(native::simd<float, 8, arch>::load(input)).store_bits(output);
#endif
}
