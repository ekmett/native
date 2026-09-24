// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <immintrin.h>
#include <native/attributes.h>
import native.x86.memory;

#if NATIVE_MEMORY_BROAD
inline constexpr auto profile_0 = native::target_features<native::x86>("avx512f,avx512dq,avx512bw,avx512vl");
inline constexpr auto profile_1 = profile_0;
inline constexpr auto profile_2 = profile_0;
#define NATIVE_MEMORY_TARGET(features) native_target("avx512f,avx512dq,avx512bw,avx512vl")
#else
inline constexpr auto profile_0 = native::target_features<native::x86>("avx2");
inline constexpr auto profile_1 = native::target_features<native::x86>("avx512f");
inline constexpr auto profile_2 = native::target_features<native::x86>("avx512f,avx512vl");
#define NATIVE_MEMORY_TARGET(features) native_target(features)
#endif
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 public_0_s_1(float const * base, __m128i indices) noexcept {
  return native::vgatherdps<1,4>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 raw_0_s_1(float const * base, __m128i indices) noexcept {
  return _mm_i32gather_ps(base, indices, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 public_0_s_2(float const * base, __m128i indices) noexcept {
  return native::vgatherdps<2,4>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 raw_0_s_2(float const * base, __m128i indices) noexcept {
  return _mm_i32gather_ps(base, indices, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 public_0_s_4(float const * base, __m128i indices) noexcept {
  return native::vgatherdps<4,4>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 raw_0_s_4(float const * base, __m128i indices) noexcept {
  return _mm_i32gather_ps(base, indices, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 public_0_s_8(float const * base, __m128i indices) noexcept {
  return native::vgatherdps<8,4>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 raw_0_s_8(float const * base, __m128i indices) noexcept {
  return _mm_i32gather_ps(base, indices, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 public_1_s_1(float const * base, __m128i indices, __m128 source, __m128i mask) noexcept {
  return native::mask_vgatherdps<1>(native::simd<float,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 raw_1_s_1(float const * base, __m128i indices, __m128 source, __m128i mask) noexcept {
  return _mm_mask_i32gather_ps(source, base, indices, __builtin_bit_cast(__m128, mask), 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 public_1_s_2(float const * base, __m128i indices, __m128 source, __m128i mask) noexcept {
  return native::mask_vgatherdps<2>(native::simd<float,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 raw_1_s_2(float const * base, __m128i indices, __m128 source, __m128i mask) noexcept {
  return _mm_mask_i32gather_ps(source, base, indices, __builtin_bit_cast(__m128, mask), 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 public_1_s_4(float const * base, __m128i indices, __m128 source, __m128i mask) noexcept {
  return native::mask_vgatherdps<4>(native::simd<float,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 raw_1_s_4(float const * base, __m128i indices, __m128 source, __m128i mask) noexcept {
  return _mm_mask_i32gather_ps(source, base, indices, __builtin_bit_cast(__m128, mask), 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 public_1_s_8(float const * base, __m128i indices, __m128 source, __m128i mask) noexcept {
  return native::mask_vgatherdps<8>(native::simd<float,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 raw_1_s_8(float const * base, __m128i indices, __m128 source, __m128i mask) noexcept {
  return _mm_mask_i32gather_ps(source, base, indices, __builtin_bit_cast(__m128, mask), 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128 public_2_s_1(float const * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  return native::mask_vgatherdps<1>(native::simd<float,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128 raw_2_s_1(float const * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  return _mm_mmask_i32gather_ps(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128 public_2_s_2(float const * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  return native::mask_vgatherdps<2>(native::simd<float,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128 raw_2_s_2(float const * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  return _mm_mmask_i32gather_ps(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128 public_2_s_4(float const * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  return native::mask_vgatherdps<4>(native::simd<float,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128 raw_2_s_4(float const * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  return _mm_mmask_i32gather_ps(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128 public_2_s_8(float const * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  return native::mask_vgatherdps<8>(native::simd<float,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128 raw_2_s_8(float const * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  return _mm_mmask_i32gather_ps(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_3_s_1(float * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  native::mask_vscatterdps<1>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<float,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_3_s_1(float * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  _mm_mask_i32scatter_ps(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_3_s_2(float * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  native::mask_vscatterdps<2>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<float,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_3_s_2(float * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  _mm_mask_i32scatter_ps(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_3_s_4(float * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  native::mask_vscatterdps<4>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<float,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_3_s_4(float * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  _mm_mask_i32scatter_ps(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_3_s_8(float * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  native::mask_vscatterdps<8>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<float,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_3_s_8(float * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  _mm_mask_i32scatter_ps(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256 public_4_s_1(float const * base, __m256i indices) noexcept {
  return native::vgatherdps<1,8>(base, native::simd<std::int32_t,8,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256 raw_4_s_1(float const * base, __m256i indices) noexcept {
  return _mm256_i32gather_ps(base, indices, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256 public_4_s_2(float const * base, __m256i indices) noexcept {
  return native::vgatherdps<2,8>(base, native::simd<std::int32_t,8,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256 raw_4_s_2(float const * base, __m256i indices) noexcept {
  return _mm256_i32gather_ps(base, indices, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256 public_4_s_4(float const * base, __m256i indices) noexcept {
  return native::vgatherdps<4,8>(base, native::simd<std::int32_t,8,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256 raw_4_s_4(float const * base, __m256i indices) noexcept {
  return _mm256_i32gather_ps(base, indices, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256 public_4_s_8(float const * base, __m256i indices) noexcept {
  return native::vgatherdps<8,8>(base, native::simd<std::int32_t,8,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256 raw_4_s_8(float const * base, __m256i indices) noexcept {
  return _mm256_i32gather_ps(base, indices, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256 public_5_s_1(float const * base, __m256i indices, __m256 source, __m256i mask) noexcept {
  return native::mask_vgatherdps<1>(native::simd<float,8,profile_0>::from_native(source), native::simd<std::int32_t,8,profile_0>::from_native(mask), base, native::simd<std::int32_t,8,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256 raw_5_s_1(float const * base, __m256i indices, __m256 source, __m256i mask) noexcept {
  return _mm256_mask_i32gather_ps(source, base, indices, __builtin_bit_cast(__m256, mask), 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256 public_5_s_2(float const * base, __m256i indices, __m256 source, __m256i mask) noexcept {
  return native::mask_vgatherdps<2>(native::simd<float,8,profile_0>::from_native(source), native::simd<std::int32_t,8,profile_0>::from_native(mask), base, native::simd<std::int32_t,8,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256 raw_5_s_2(float const * base, __m256i indices, __m256 source, __m256i mask) noexcept {
  return _mm256_mask_i32gather_ps(source, base, indices, __builtin_bit_cast(__m256, mask), 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256 public_5_s_4(float const * base, __m256i indices, __m256 source, __m256i mask) noexcept {
  return native::mask_vgatherdps<4>(native::simd<float,8,profile_0>::from_native(source), native::simd<std::int32_t,8,profile_0>::from_native(mask), base, native::simd<std::int32_t,8,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256 raw_5_s_4(float const * base, __m256i indices, __m256 source, __m256i mask) noexcept {
  return _mm256_mask_i32gather_ps(source, base, indices, __builtin_bit_cast(__m256, mask), 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256 public_5_s_8(float const * base, __m256i indices, __m256 source, __m256i mask) noexcept {
  return native::mask_vgatherdps<8>(native::simd<float,8,profile_0>::from_native(source), native::simd<std::int32_t,8,profile_0>::from_native(mask), base, native::simd<std::int32_t,8,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256 raw_5_s_8(float const * base, __m256i indices, __m256 source, __m256i mask) noexcept {
  return _mm256_mask_i32gather_ps(source, base, indices, __builtin_bit_cast(__m256, mask), 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256 public_6_s_1(float const * base, __m256i indices, __m256 source, __mmask8 mask) noexcept {
  return native::mask_vgatherdps<1>(native::simd<float,8,profile_2>::from_native(source), native::predicate<8,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,8,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256 raw_6_s_1(float const * base, __m256i indices, __m256 source, __mmask8 mask) noexcept {
  return _mm256_mmask_i32gather_ps(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256 public_6_s_2(float const * base, __m256i indices, __m256 source, __mmask8 mask) noexcept {
  return native::mask_vgatherdps<2>(native::simd<float,8,profile_2>::from_native(source), native::predicate<8,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,8,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256 raw_6_s_2(float const * base, __m256i indices, __m256 source, __mmask8 mask) noexcept {
  return _mm256_mmask_i32gather_ps(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256 public_6_s_4(float const * base, __m256i indices, __m256 source, __mmask8 mask) noexcept {
  return native::mask_vgatherdps<4>(native::simd<float,8,profile_2>::from_native(source), native::predicate<8,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,8,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256 raw_6_s_4(float const * base, __m256i indices, __m256 source, __mmask8 mask) noexcept {
  return _mm256_mmask_i32gather_ps(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256 public_6_s_8(float const * base, __m256i indices, __m256 source, __mmask8 mask) noexcept {
  return native::mask_vgatherdps<8>(native::simd<float,8,profile_2>::from_native(source), native::predicate<8,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,8,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256 raw_6_s_8(float const * base, __m256i indices, __m256 source, __mmask8 mask) noexcept {
  return _mm256_mmask_i32gather_ps(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_7_s_1(float * base, __m256i indices, __m256 source, __mmask8 mask) noexcept {
  native::mask_vscatterdps<1>(base, native::predicate<8,profile_2>::from_bitset(mask), native::simd<std::int32_t,8,profile_2>::from_native(indices), native::simd<float,8,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_7_s_1(float * base, __m256i indices, __m256 source, __mmask8 mask) noexcept {
  _mm256_mask_i32scatter_ps(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_7_s_2(float * base, __m256i indices, __m256 source, __mmask8 mask) noexcept {
  native::mask_vscatterdps<2>(base, native::predicate<8,profile_2>::from_bitset(mask), native::simd<std::int32_t,8,profile_2>::from_native(indices), native::simd<float,8,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_7_s_2(float * base, __m256i indices, __m256 source, __mmask8 mask) noexcept {
  _mm256_mask_i32scatter_ps(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_7_s_4(float * base, __m256i indices, __m256 source, __mmask8 mask) noexcept {
  native::mask_vscatterdps<4>(base, native::predicate<8,profile_2>::from_bitset(mask), native::simd<std::int32_t,8,profile_2>::from_native(indices), native::simd<float,8,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_7_s_4(float * base, __m256i indices, __m256 source, __mmask8 mask) noexcept {
  _mm256_mask_i32scatter_ps(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_7_s_8(float * base, __m256i indices, __m256 source, __mmask8 mask) noexcept {
  native::mask_vscatterdps<8>(base, native::predicate<8,profile_2>::from_bitset(mask), native::simd<std::int32_t,8,profile_2>::from_native(indices), native::simd<float,8,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_7_s_8(float * base, __m256i indices, __m256 source, __mmask8 mask) noexcept {
  _mm256_mask_i32scatter_ps(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512 public_8_s_1(float const * base, __m512i indices, __m512 source, __mmask16 mask) noexcept {
  return native::mask_vgatherdps<1>(native::simd<float,16,profile_1>::from_native(source), native::predicate<16,profile_1>::from_bitset(mask), base, native::simd<std::int32_t,16,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512 raw_8_s_1(float const * base, __m512i indices, __m512 source, __mmask16 mask) noexcept {
  return _mm512_mask_i32gather_ps(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512 public_8_s_2(float const * base, __m512i indices, __m512 source, __mmask16 mask) noexcept {
  return native::mask_vgatherdps<2>(native::simd<float,16,profile_1>::from_native(source), native::predicate<16,profile_1>::from_bitset(mask), base, native::simd<std::int32_t,16,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512 raw_8_s_2(float const * base, __m512i indices, __m512 source, __mmask16 mask) noexcept {
  return _mm512_mask_i32gather_ps(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512 public_8_s_4(float const * base, __m512i indices, __m512 source, __mmask16 mask) noexcept {
  return native::mask_vgatherdps<4>(native::simd<float,16,profile_1>::from_native(source), native::predicate<16,profile_1>::from_bitset(mask), base, native::simd<std::int32_t,16,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512 raw_8_s_4(float const * base, __m512i indices, __m512 source, __mmask16 mask) noexcept {
  return _mm512_mask_i32gather_ps(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512 public_8_s_8(float const * base, __m512i indices, __m512 source, __mmask16 mask) noexcept {
  return native::mask_vgatherdps<8>(native::simd<float,16,profile_1>::from_native(source), native::predicate<16,profile_1>::from_bitset(mask), base, native::simd<std::int32_t,16,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512 raw_8_s_8(float const * base, __m512i indices, __m512 source, __mmask16 mask) noexcept {
  return _mm512_mask_i32gather_ps(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_9_s_1(float * base, __m512i indices, __m512 source, __mmask16 mask) noexcept {
  native::mask_vscatterdps<1>(base, native::predicate<16,profile_1>::from_bitset(mask), native::simd<std::int32_t,16,profile_1>::from_native(indices), native::simd<float,16,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_9_s_1(float * base, __m512i indices, __m512 source, __mmask16 mask) noexcept {
  _mm512_mask_i32scatter_ps(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_9_s_2(float * base, __m512i indices, __m512 source, __mmask16 mask) noexcept {
  native::mask_vscatterdps<2>(base, native::predicate<16,profile_1>::from_bitset(mask), native::simd<std::int32_t,16,profile_1>::from_native(indices), native::simd<float,16,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_9_s_2(float * base, __m512i indices, __m512 source, __mmask16 mask) noexcept {
  _mm512_mask_i32scatter_ps(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_9_s_4(float * base, __m512i indices, __m512 source, __mmask16 mask) noexcept {
  native::mask_vscatterdps<4>(base, native::predicate<16,profile_1>::from_bitset(mask), native::simd<std::int32_t,16,profile_1>::from_native(indices), native::simd<float,16,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_9_s_4(float * base, __m512i indices, __m512 source, __mmask16 mask) noexcept {
  _mm512_mask_i32scatter_ps(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_9_s_8(float * base, __m512i indices, __m512 source, __mmask16 mask) noexcept {
  native::mask_vscatterdps<8>(base, native::predicate<16,profile_1>::from_bitset(mask), native::simd<std::int32_t,16,profile_1>::from_native(indices), native::simd<float,16,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_9_s_8(float * base, __m512i indices, __m512 source, __mmask16 mask) noexcept {
  _mm512_mask_i32scatter_ps(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_10_s_1(std::int32_t const * base, __m128i indices) noexcept {
  return native::vpgatherdd<1,4>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_10_s_1(std::int32_t const * base, __m128i indices) noexcept {
  return _mm_i32gather_epi32(reinterpret_cast<int const *>(base), indices, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_10_s_2(std::int32_t const * base, __m128i indices) noexcept {
  return native::vpgatherdd<2,4>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_10_s_2(std::int32_t const * base, __m128i indices) noexcept {
  return _mm_i32gather_epi32(reinterpret_cast<int const *>(base), indices, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_10_s_4(std::int32_t const * base, __m128i indices) noexcept {
  return native::vpgatherdd<4,4>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_10_s_4(std::int32_t const * base, __m128i indices) noexcept {
  return _mm_i32gather_epi32(reinterpret_cast<int const *>(base), indices, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_10_s_8(std::int32_t const * base, __m128i indices) noexcept {
  return native::vpgatherdd<8,4>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_10_s_8(std::int32_t const * base, __m128i indices) noexcept {
  return _mm_i32gather_epi32(reinterpret_cast<int const *>(base), indices, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_10_u_1(std::uint32_t const * base, __m128i indices) noexcept {
  return native::vpgatherdd<1,4>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_10_u_1(std::uint32_t const * base, __m128i indices) noexcept {
  return _mm_i32gather_epi32(reinterpret_cast<int const *>(base), indices, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_10_u_2(std::uint32_t const * base, __m128i indices) noexcept {
  return native::vpgatherdd<2,4>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_10_u_2(std::uint32_t const * base, __m128i indices) noexcept {
  return _mm_i32gather_epi32(reinterpret_cast<int const *>(base), indices, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_10_u_4(std::uint32_t const * base, __m128i indices) noexcept {
  return native::vpgatherdd<4,4>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_10_u_4(std::uint32_t const * base, __m128i indices) noexcept {
  return _mm_i32gather_epi32(reinterpret_cast<int const *>(base), indices, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_10_u_8(std::uint32_t const * base, __m128i indices) noexcept {
  return native::vpgatherdd<8,4>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_10_u_8(std::uint32_t const * base, __m128i indices) noexcept {
  return _mm_i32gather_epi32(reinterpret_cast<int const *>(base), indices, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_11_s_1(std::int32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherdd<1>(native::simd<std::int32_t,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_11_s_1(std::int32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i32gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_11_s_2(std::int32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherdd<2>(native::simd<std::int32_t,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_11_s_2(std::int32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i32gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_11_s_4(std::int32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherdd<4>(native::simd<std::int32_t,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_11_s_4(std::int32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i32gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_11_s_8(std::int32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherdd<8>(native::simd<std::int32_t,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_11_s_8(std::int32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i32gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_11_u_1(std::uint32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherdd<1>(native::simd<std::uint32_t,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_11_u_1(std::uint32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i32gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_11_u_2(std::uint32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherdd<2>(native::simd<std::uint32_t,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_11_u_2(std::uint32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i32gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_11_u_4(std::uint32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherdd<4>(native::simd<std::uint32_t,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_11_u_4(std::uint32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i32gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_11_u_8(std::uint32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherdd<8>(native::simd<std::uint32_t,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_11_u_8(std::uint32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i32gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_12_s_1(std::int32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdd<1>(native::simd<std::int32_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_12_s_1(std::int32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i32gather_epi32(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_12_s_2(std::int32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdd<2>(native::simd<std::int32_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_12_s_2(std::int32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i32gather_epi32(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_12_s_4(std::int32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdd<4>(native::simd<std::int32_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_12_s_4(std::int32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i32gather_epi32(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_12_s_8(std::int32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdd<8>(native::simd<std::int32_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_12_s_8(std::int32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i32gather_epi32(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_12_u_1(std::uint32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdd<1>(native::simd<std::uint32_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_12_u_1(std::uint32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i32gather_epi32(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_12_u_2(std::uint32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdd<2>(native::simd<std::uint32_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_12_u_2(std::uint32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i32gather_epi32(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_12_u_4(std::uint32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdd<4>(native::simd<std::uint32_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_12_u_4(std::uint32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i32gather_epi32(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_12_u_8(std::uint32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdd<8>(native::simd<std::uint32_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_12_u_8(std::uint32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i32gather_epi32(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_13_s_1(std::int32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdd<1>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<std::int32_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_13_s_1(std::int32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i32scatter_epi32(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_13_s_2(std::int32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdd<2>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<std::int32_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_13_s_2(std::int32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i32scatter_epi32(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_13_s_4(std::int32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdd<4>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<std::int32_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_13_s_4(std::int32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i32scatter_epi32(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_13_s_8(std::int32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdd<8>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<std::int32_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_13_s_8(std::int32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i32scatter_epi32(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_13_u_1(std::uint32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdd<1>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<std::uint32_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_13_u_1(std::uint32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i32scatter_epi32(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_13_u_2(std::uint32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdd<2>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<std::uint32_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_13_u_2(std::uint32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i32scatter_epi32(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_13_u_4(std::uint32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdd<4>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<std::uint32_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_13_u_4(std::uint32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i32scatter_epi32(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_13_u_8(std::uint32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdd<8>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<std::uint32_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_13_u_8(std::uint32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i32scatter_epi32(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_14_s_1(std::int32_t const * base, __m256i indices) noexcept {
  return native::vpgatherdd<1,8>(base, native::simd<std::int32_t,8,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_14_s_1(std::int32_t const * base, __m256i indices) noexcept {
  return _mm256_i32gather_epi32(reinterpret_cast<int const *>(base), indices, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_14_s_2(std::int32_t const * base, __m256i indices) noexcept {
  return native::vpgatherdd<2,8>(base, native::simd<std::int32_t,8,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_14_s_2(std::int32_t const * base, __m256i indices) noexcept {
  return _mm256_i32gather_epi32(reinterpret_cast<int const *>(base), indices, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_14_s_4(std::int32_t const * base, __m256i indices) noexcept {
  return native::vpgatherdd<4,8>(base, native::simd<std::int32_t,8,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_14_s_4(std::int32_t const * base, __m256i indices) noexcept {
  return _mm256_i32gather_epi32(reinterpret_cast<int const *>(base), indices, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_14_s_8(std::int32_t const * base, __m256i indices) noexcept {
  return native::vpgatherdd<8,8>(base, native::simd<std::int32_t,8,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_14_s_8(std::int32_t const * base, __m256i indices) noexcept {
  return _mm256_i32gather_epi32(reinterpret_cast<int const *>(base), indices, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_14_u_1(std::uint32_t const * base, __m256i indices) noexcept {
  return native::vpgatherdd<1,8>(base, native::simd<std::int32_t,8,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_14_u_1(std::uint32_t const * base, __m256i indices) noexcept {
  return _mm256_i32gather_epi32(reinterpret_cast<int const *>(base), indices, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_14_u_2(std::uint32_t const * base, __m256i indices) noexcept {
  return native::vpgatherdd<2,8>(base, native::simd<std::int32_t,8,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_14_u_2(std::uint32_t const * base, __m256i indices) noexcept {
  return _mm256_i32gather_epi32(reinterpret_cast<int const *>(base), indices, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_14_u_4(std::uint32_t const * base, __m256i indices) noexcept {
  return native::vpgatherdd<4,8>(base, native::simd<std::int32_t,8,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_14_u_4(std::uint32_t const * base, __m256i indices) noexcept {
  return _mm256_i32gather_epi32(reinterpret_cast<int const *>(base), indices, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_14_u_8(std::uint32_t const * base, __m256i indices) noexcept {
  return native::vpgatherdd<8,8>(base, native::simd<std::int32_t,8,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_14_u_8(std::uint32_t const * base, __m256i indices) noexcept {
  return _mm256_i32gather_epi32(reinterpret_cast<int const *>(base), indices, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_15_s_1(std::int32_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return native::mask_vpgatherdd<1>(native::simd<std::int32_t,8,profile_0>::from_native(source), native::simd<std::int32_t,8,profile_0>::from_native(mask), base, native::simd<std::int32_t,8,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_15_s_1(std::int32_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return _mm256_mask_i32gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_15_s_2(std::int32_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return native::mask_vpgatherdd<2>(native::simd<std::int32_t,8,profile_0>::from_native(source), native::simd<std::int32_t,8,profile_0>::from_native(mask), base, native::simd<std::int32_t,8,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_15_s_2(std::int32_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return _mm256_mask_i32gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_15_s_4(std::int32_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return native::mask_vpgatherdd<4>(native::simd<std::int32_t,8,profile_0>::from_native(source), native::simd<std::int32_t,8,profile_0>::from_native(mask), base, native::simd<std::int32_t,8,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_15_s_4(std::int32_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return _mm256_mask_i32gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_15_s_8(std::int32_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return native::mask_vpgatherdd<8>(native::simd<std::int32_t,8,profile_0>::from_native(source), native::simd<std::int32_t,8,profile_0>::from_native(mask), base, native::simd<std::int32_t,8,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_15_s_8(std::int32_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return _mm256_mask_i32gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_15_u_1(std::uint32_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return native::mask_vpgatherdd<1>(native::simd<std::uint32_t,8,profile_0>::from_native(source), native::simd<std::int32_t,8,profile_0>::from_native(mask), base, native::simd<std::int32_t,8,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_15_u_1(std::uint32_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return _mm256_mask_i32gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_15_u_2(std::uint32_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return native::mask_vpgatherdd<2>(native::simd<std::uint32_t,8,profile_0>::from_native(source), native::simd<std::int32_t,8,profile_0>::from_native(mask), base, native::simd<std::int32_t,8,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_15_u_2(std::uint32_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return _mm256_mask_i32gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_15_u_4(std::uint32_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return native::mask_vpgatherdd<4>(native::simd<std::uint32_t,8,profile_0>::from_native(source), native::simd<std::int32_t,8,profile_0>::from_native(mask), base, native::simd<std::int32_t,8,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_15_u_4(std::uint32_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return _mm256_mask_i32gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_15_u_8(std::uint32_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return native::mask_vpgatherdd<8>(native::simd<std::uint32_t,8,profile_0>::from_native(source), native::simd<std::int32_t,8,profile_0>::from_native(mask), base, native::simd<std::int32_t,8,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_15_u_8(std::uint32_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return _mm256_mask_i32gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i public_16_s_1(std::int32_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdd<1>(native::simd<std::int32_t,8,profile_2>::from_native(source), native::predicate<8,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,8,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i raw_16_s_1(std::int32_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i32gather_epi32(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i public_16_s_2(std::int32_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdd<2>(native::simd<std::int32_t,8,profile_2>::from_native(source), native::predicate<8,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,8,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i raw_16_s_2(std::int32_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i32gather_epi32(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i public_16_s_4(std::int32_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdd<4>(native::simd<std::int32_t,8,profile_2>::from_native(source), native::predicate<8,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,8,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i raw_16_s_4(std::int32_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i32gather_epi32(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i public_16_s_8(std::int32_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdd<8>(native::simd<std::int32_t,8,profile_2>::from_native(source), native::predicate<8,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,8,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i raw_16_s_8(std::int32_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i32gather_epi32(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i public_16_u_1(std::uint32_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdd<1>(native::simd<std::uint32_t,8,profile_2>::from_native(source), native::predicate<8,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,8,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i raw_16_u_1(std::uint32_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i32gather_epi32(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i public_16_u_2(std::uint32_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdd<2>(native::simd<std::uint32_t,8,profile_2>::from_native(source), native::predicate<8,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,8,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i raw_16_u_2(std::uint32_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i32gather_epi32(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i public_16_u_4(std::uint32_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdd<4>(native::simd<std::uint32_t,8,profile_2>::from_native(source), native::predicate<8,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,8,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i raw_16_u_4(std::uint32_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i32gather_epi32(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i public_16_u_8(std::uint32_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdd<8>(native::simd<std::uint32_t,8,profile_2>::from_native(source), native::predicate<8,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,8,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i raw_16_u_8(std::uint32_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i32gather_epi32(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_17_s_1(std::int32_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdd<1>(base, native::predicate<8,profile_2>::from_bitset(mask), native::simd<std::int32_t,8,profile_2>::from_native(indices), native::simd<std::int32_t,8,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_17_s_1(std::int32_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  _mm256_mask_i32scatter_epi32(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_17_s_2(std::int32_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdd<2>(base, native::predicate<8,profile_2>::from_bitset(mask), native::simd<std::int32_t,8,profile_2>::from_native(indices), native::simd<std::int32_t,8,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_17_s_2(std::int32_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  _mm256_mask_i32scatter_epi32(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_17_s_4(std::int32_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdd<4>(base, native::predicate<8,profile_2>::from_bitset(mask), native::simd<std::int32_t,8,profile_2>::from_native(indices), native::simd<std::int32_t,8,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_17_s_4(std::int32_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  _mm256_mask_i32scatter_epi32(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_17_s_8(std::int32_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdd<8>(base, native::predicate<8,profile_2>::from_bitset(mask), native::simd<std::int32_t,8,profile_2>::from_native(indices), native::simd<std::int32_t,8,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_17_s_8(std::int32_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  _mm256_mask_i32scatter_epi32(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_17_u_1(std::uint32_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdd<1>(base, native::predicate<8,profile_2>::from_bitset(mask), native::simd<std::int32_t,8,profile_2>::from_native(indices), native::simd<std::uint32_t,8,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_17_u_1(std::uint32_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  _mm256_mask_i32scatter_epi32(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_17_u_2(std::uint32_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdd<2>(base, native::predicate<8,profile_2>::from_bitset(mask), native::simd<std::int32_t,8,profile_2>::from_native(indices), native::simd<std::uint32_t,8,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_17_u_2(std::uint32_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  _mm256_mask_i32scatter_epi32(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_17_u_4(std::uint32_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdd<4>(base, native::predicate<8,profile_2>::from_bitset(mask), native::simd<std::int32_t,8,profile_2>::from_native(indices), native::simd<std::uint32_t,8,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_17_u_4(std::uint32_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  _mm256_mask_i32scatter_epi32(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_17_u_8(std::uint32_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdd<8>(base, native::predicate<8,profile_2>::from_bitset(mask), native::simd<std::int32_t,8,profile_2>::from_native(indices), native::simd<std::uint32_t,8,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_17_u_8(std::uint32_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  _mm256_mask_i32scatter_epi32(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i public_18_s_1(std::int32_t const * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  return native::mask_vpgatherdd<1>(native::simd<std::int32_t,16,profile_1>::from_native(source), native::predicate<16,profile_1>::from_bitset(mask), base, native::simd<std::int32_t,16,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i raw_18_s_1(std::int32_t const * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  return _mm512_mask_i32gather_epi32(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i public_18_s_2(std::int32_t const * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  return native::mask_vpgatherdd<2>(native::simd<std::int32_t,16,profile_1>::from_native(source), native::predicate<16,profile_1>::from_bitset(mask), base, native::simd<std::int32_t,16,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i raw_18_s_2(std::int32_t const * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  return _mm512_mask_i32gather_epi32(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i public_18_s_4(std::int32_t const * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  return native::mask_vpgatherdd<4>(native::simd<std::int32_t,16,profile_1>::from_native(source), native::predicate<16,profile_1>::from_bitset(mask), base, native::simd<std::int32_t,16,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i raw_18_s_4(std::int32_t const * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  return _mm512_mask_i32gather_epi32(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i public_18_s_8(std::int32_t const * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  return native::mask_vpgatherdd<8>(native::simd<std::int32_t,16,profile_1>::from_native(source), native::predicate<16,profile_1>::from_bitset(mask), base, native::simd<std::int32_t,16,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i raw_18_s_8(std::int32_t const * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  return _mm512_mask_i32gather_epi32(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i public_18_u_1(std::uint32_t const * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  return native::mask_vpgatherdd<1>(native::simd<std::uint32_t,16,profile_1>::from_native(source), native::predicate<16,profile_1>::from_bitset(mask), base, native::simd<std::int32_t,16,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i raw_18_u_1(std::uint32_t const * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  return _mm512_mask_i32gather_epi32(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i public_18_u_2(std::uint32_t const * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  return native::mask_vpgatherdd<2>(native::simd<std::uint32_t,16,profile_1>::from_native(source), native::predicate<16,profile_1>::from_bitset(mask), base, native::simd<std::int32_t,16,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i raw_18_u_2(std::uint32_t const * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  return _mm512_mask_i32gather_epi32(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i public_18_u_4(std::uint32_t const * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  return native::mask_vpgatherdd<4>(native::simd<std::uint32_t,16,profile_1>::from_native(source), native::predicate<16,profile_1>::from_bitset(mask), base, native::simd<std::int32_t,16,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i raw_18_u_4(std::uint32_t const * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  return _mm512_mask_i32gather_epi32(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i public_18_u_8(std::uint32_t const * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  return native::mask_vpgatherdd<8>(native::simd<std::uint32_t,16,profile_1>::from_native(source), native::predicate<16,profile_1>::from_bitset(mask), base, native::simd<std::int32_t,16,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i raw_18_u_8(std::uint32_t const * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  return _mm512_mask_i32gather_epi32(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_19_s_1(std::int32_t * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  native::mask_vpscatterdd<1>(base, native::predicate<16,profile_1>::from_bitset(mask), native::simd<std::int32_t,16,profile_1>::from_native(indices), native::simd<std::int32_t,16,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_19_s_1(std::int32_t * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  _mm512_mask_i32scatter_epi32(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_19_s_2(std::int32_t * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  native::mask_vpscatterdd<2>(base, native::predicate<16,profile_1>::from_bitset(mask), native::simd<std::int32_t,16,profile_1>::from_native(indices), native::simd<std::int32_t,16,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_19_s_2(std::int32_t * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  _mm512_mask_i32scatter_epi32(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_19_s_4(std::int32_t * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  native::mask_vpscatterdd<4>(base, native::predicate<16,profile_1>::from_bitset(mask), native::simd<std::int32_t,16,profile_1>::from_native(indices), native::simd<std::int32_t,16,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_19_s_4(std::int32_t * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  _mm512_mask_i32scatter_epi32(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_19_s_8(std::int32_t * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  native::mask_vpscatterdd<8>(base, native::predicate<16,profile_1>::from_bitset(mask), native::simd<std::int32_t,16,profile_1>::from_native(indices), native::simd<std::int32_t,16,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_19_s_8(std::int32_t * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  _mm512_mask_i32scatter_epi32(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_19_u_1(std::uint32_t * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  native::mask_vpscatterdd<1>(base, native::predicate<16,profile_1>::from_bitset(mask), native::simd<std::int32_t,16,profile_1>::from_native(indices), native::simd<std::uint32_t,16,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_19_u_1(std::uint32_t * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  _mm512_mask_i32scatter_epi32(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_19_u_2(std::uint32_t * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  native::mask_vpscatterdd<2>(base, native::predicate<16,profile_1>::from_bitset(mask), native::simd<std::int32_t,16,profile_1>::from_native(indices), native::simd<std::uint32_t,16,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_19_u_2(std::uint32_t * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  _mm512_mask_i32scatter_epi32(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_19_u_4(std::uint32_t * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  native::mask_vpscatterdd<4>(base, native::predicate<16,profile_1>::from_bitset(mask), native::simd<std::int32_t,16,profile_1>::from_native(indices), native::simd<std::uint32_t,16,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_19_u_4(std::uint32_t * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  _mm512_mask_i32scatter_epi32(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_19_u_8(std::uint32_t * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  native::mask_vpscatterdd<8>(base, native::predicate<16,profile_1>::from_bitset(mask), native::simd<std::int32_t,16,profile_1>::from_native(indices), native::simd<std::uint32_t,16,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_19_u_8(std::uint32_t * base, __m512i indices, __m512i source, __mmask16 mask) noexcept {
  _mm512_mask_i32scatter_epi32(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d public_20_s_1(double const * base, __m128i indices) noexcept {
  return native::vgatherdpd<1,2>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d raw_20_s_1(double const * base, __m128i indices) noexcept {
  return _mm_i32gather_pd(base, indices, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d public_20_s_2(double const * base, __m128i indices) noexcept {
  return native::vgatherdpd<2,2>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d raw_20_s_2(double const * base, __m128i indices) noexcept {
  return _mm_i32gather_pd(base, indices, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d public_20_s_4(double const * base, __m128i indices) noexcept {
  return native::vgatherdpd<4,2>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d raw_20_s_4(double const * base, __m128i indices) noexcept {
  return _mm_i32gather_pd(base, indices, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d public_20_s_8(double const * base, __m128i indices) noexcept {
  return native::vgatherdpd<8,2>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d raw_20_s_8(double const * base, __m128i indices) noexcept {
  return _mm_i32gather_pd(base, indices, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d public_21_s_1(double const * base, __m128i indices, __m128d source, __m128i mask) noexcept {
  return native::mask_vgatherdpd<1>(native::simd<double,2,profile_0>::from_native(source), native::simd<std::int64_t,2,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d raw_21_s_1(double const * base, __m128i indices, __m128d source, __m128i mask) noexcept {
  return _mm_mask_i32gather_pd(source, base, indices, __builtin_bit_cast(__m128d, mask), 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d public_21_s_2(double const * base, __m128i indices, __m128d source, __m128i mask) noexcept {
  return native::mask_vgatherdpd<2>(native::simd<double,2,profile_0>::from_native(source), native::simd<std::int64_t,2,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d raw_21_s_2(double const * base, __m128i indices, __m128d source, __m128i mask) noexcept {
  return _mm_mask_i32gather_pd(source, base, indices, __builtin_bit_cast(__m128d, mask), 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d public_21_s_4(double const * base, __m128i indices, __m128d source, __m128i mask) noexcept {
  return native::mask_vgatherdpd<4>(native::simd<double,2,profile_0>::from_native(source), native::simd<std::int64_t,2,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d raw_21_s_4(double const * base, __m128i indices, __m128d source, __m128i mask) noexcept {
  return _mm_mask_i32gather_pd(source, base, indices, __builtin_bit_cast(__m128d, mask), 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d public_21_s_8(double const * base, __m128i indices, __m128d source, __m128i mask) noexcept {
  return native::mask_vgatherdpd<8>(native::simd<double,2,profile_0>::from_native(source), native::simd<std::int64_t,2,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d raw_21_s_8(double const * base, __m128i indices, __m128d source, __m128i mask) noexcept {
  return _mm_mask_i32gather_pd(source, base, indices, __builtin_bit_cast(__m128d, mask), 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128d public_22_s_1(double const * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  return native::mask_vgatherdpd<1>(native::simd<double,2,profile_2>::from_native(source), native::predicate<2,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128d raw_22_s_1(double const * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  return _mm_mmask_i32gather_pd(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128d public_22_s_2(double const * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  return native::mask_vgatherdpd<2>(native::simd<double,2,profile_2>::from_native(source), native::predicate<2,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128d raw_22_s_2(double const * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  return _mm_mmask_i32gather_pd(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128d public_22_s_4(double const * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  return native::mask_vgatherdpd<4>(native::simd<double,2,profile_2>::from_native(source), native::predicate<2,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128d raw_22_s_4(double const * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  return _mm_mmask_i32gather_pd(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128d public_22_s_8(double const * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  return native::mask_vgatherdpd<8>(native::simd<double,2,profile_2>::from_native(source), native::predicate<2,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128d raw_22_s_8(double const * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  return _mm_mmask_i32gather_pd(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_23_s_1(double * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  native::mask_vscatterdpd<1>(base, native::predicate<2,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<double,2,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_23_s_1(double * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  _mm_mask_i32scatter_pd(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_23_s_2(double * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  native::mask_vscatterdpd<2>(base, native::predicate<2,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<double,2,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_23_s_2(double * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  _mm_mask_i32scatter_pd(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_23_s_4(double * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  native::mask_vscatterdpd<4>(base, native::predicate<2,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<double,2,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_23_s_4(double * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  _mm_mask_i32scatter_pd(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_23_s_8(double * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  native::mask_vscatterdpd<8>(base, native::predicate<2,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<double,2,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_23_s_8(double * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  _mm_mask_i32scatter_pd(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d public_24_s_1(double const * base, __m128i indices) noexcept {
  return native::vgatherdpd<1,4>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d raw_24_s_1(double const * base, __m128i indices) noexcept {
  return _mm256_i32gather_pd(base, indices, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d public_24_s_2(double const * base, __m128i indices) noexcept {
  return native::vgatherdpd<2,4>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d raw_24_s_2(double const * base, __m128i indices) noexcept {
  return _mm256_i32gather_pd(base, indices, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d public_24_s_4(double const * base, __m128i indices) noexcept {
  return native::vgatherdpd<4,4>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d raw_24_s_4(double const * base, __m128i indices) noexcept {
  return _mm256_i32gather_pd(base, indices, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d public_24_s_8(double const * base, __m128i indices) noexcept {
  return native::vgatherdpd<8,4>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d raw_24_s_8(double const * base, __m128i indices) noexcept {
  return _mm256_i32gather_pd(base, indices, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d public_25_s_1(double const * base, __m128i indices, __m256d source, __m256i mask) noexcept {
  return native::mask_vgatherdpd<1>(native::simd<double,4,profile_0>::from_native(source), native::simd<std::int64_t,4,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d raw_25_s_1(double const * base, __m128i indices, __m256d source, __m256i mask) noexcept {
  return _mm256_mask_i32gather_pd(source, base, indices, __builtin_bit_cast(__m256d, mask), 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d public_25_s_2(double const * base, __m128i indices, __m256d source, __m256i mask) noexcept {
  return native::mask_vgatherdpd<2>(native::simd<double,4,profile_0>::from_native(source), native::simd<std::int64_t,4,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d raw_25_s_2(double const * base, __m128i indices, __m256d source, __m256i mask) noexcept {
  return _mm256_mask_i32gather_pd(source, base, indices, __builtin_bit_cast(__m256d, mask), 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d public_25_s_4(double const * base, __m128i indices, __m256d source, __m256i mask) noexcept {
  return native::mask_vgatherdpd<4>(native::simd<double,4,profile_0>::from_native(source), native::simd<std::int64_t,4,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d raw_25_s_4(double const * base, __m128i indices, __m256d source, __m256i mask) noexcept {
  return _mm256_mask_i32gather_pd(source, base, indices, __builtin_bit_cast(__m256d, mask), 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d public_25_s_8(double const * base, __m128i indices, __m256d source, __m256i mask) noexcept {
  return native::mask_vgatherdpd<8>(native::simd<double,4,profile_0>::from_native(source), native::simd<std::int64_t,4,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d raw_25_s_8(double const * base, __m128i indices, __m256d source, __m256i mask) noexcept {
  return _mm256_mask_i32gather_pd(source, base, indices, __builtin_bit_cast(__m256d, mask), 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256d public_26_s_1(double const * base, __m128i indices, __m256d source, __mmask8 mask) noexcept {
  return native::mask_vgatherdpd<1>(native::simd<double,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256d raw_26_s_1(double const * base, __m128i indices, __m256d source, __mmask8 mask) noexcept {
  return _mm256_mmask_i32gather_pd(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256d public_26_s_2(double const * base, __m128i indices, __m256d source, __mmask8 mask) noexcept {
  return native::mask_vgatherdpd<2>(native::simd<double,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256d raw_26_s_2(double const * base, __m128i indices, __m256d source, __mmask8 mask) noexcept {
  return _mm256_mmask_i32gather_pd(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256d public_26_s_4(double const * base, __m128i indices, __m256d source, __mmask8 mask) noexcept {
  return native::mask_vgatherdpd<4>(native::simd<double,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256d raw_26_s_4(double const * base, __m128i indices, __m256d source, __mmask8 mask) noexcept {
  return _mm256_mmask_i32gather_pd(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256d public_26_s_8(double const * base, __m128i indices, __m256d source, __mmask8 mask) noexcept {
  return native::mask_vgatherdpd<8>(native::simd<double,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256d raw_26_s_8(double const * base, __m128i indices, __m256d source, __mmask8 mask) noexcept {
  return _mm256_mmask_i32gather_pd(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_27_s_1(double * base, __m128i indices, __m256d source, __mmask8 mask) noexcept {
  native::mask_vscatterdpd<1>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<double,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_27_s_1(double * base, __m128i indices, __m256d source, __mmask8 mask) noexcept {
  _mm256_mask_i32scatter_pd(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_27_s_2(double * base, __m128i indices, __m256d source, __mmask8 mask) noexcept {
  native::mask_vscatterdpd<2>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<double,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_27_s_2(double * base, __m128i indices, __m256d source, __mmask8 mask) noexcept {
  _mm256_mask_i32scatter_pd(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_27_s_4(double * base, __m128i indices, __m256d source, __mmask8 mask) noexcept {
  native::mask_vscatterdpd<4>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<double,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_27_s_4(double * base, __m128i indices, __m256d source, __mmask8 mask) noexcept {
  _mm256_mask_i32scatter_pd(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_27_s_8(double * base, __m128i indices, __m256d source, __mmask8 mask) noexcept {
  native::mask_vscatterdpd<8>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<double,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_27_s_8(double * base, __m128i indices, __m256d source, __mmask8 mask) noexcept {
  _mm256_mask_i32scatter_pd(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512d public_28_s_1(double const * base, __m256i indices, __m512d source, __mmask8 mask) noexcept {
  return native::mask_vgatherdpd<1>(native::simd<double,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int32_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512d raw_28_s_1(double const * base, __m256i indices, __m512d source, __mmask8 mask) noexcept {
  return _mm512_mask_i32gather_pd(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512d public_28_s_2(double const * base, __m256i indices, __m512d source, __mmask8 mask) noexcept {
  return native::mask_vgatherdpd<2>(native::simd<double,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int32_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512d raw_28_s_2(double const * base, __m256i indices, __m512d source, __mmask8 mask) noexcept {
  return _mm512_mask_i32gather_pd(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512d public_28_s_4(double const * base, __m256i indices, __m512d source, __mmask8 mask) noexcept {
  return native::mask_vgatherdpd<4>(native::simd<double,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int32_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512d raw_28_s_4(double const * base, __m256i indices, __m512d source, __mmask8 mask) noexcept {
  return _mm512_mask_i32gather_pd(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512d public_28_s_8(double const * base, __m256i indices, __m512d source, __mmask8 mask) noexcept {
  return native::mask_vgatherdpd<8>(native::simd<double,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int32_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512d raw_28_s_8(double const * base, __m256i indices, __m512d source, __mmask8 mask) noexcept {
  return _mm512_mask_i32gather_pd(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_29_s_1(double * base, __m256i indices, __m512d source, __mmask8 mask) noexcept {
  native::mask_vscatterdpd<1>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int32_t,8,profile_1>::from_native(indices), native::simd<double,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_29_s_1(double * base, __m256i indices, __m512d source, __mmask8 mask) noexcept {
  _mm512_mask_i32scatter_pd(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_29_s_2(double * base, __m256i indices, __m512d source, __mmask8 mask) noexcept {
  native::mask_vscatterdpd<2>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int32_t,8,profile_1>::from_native(indices), native::simd<double,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_29_s_2(double * base, __m256i indices, __m512d source, __mmask8 mask) noexcept {
  _mm512_mask_i32scatter_pd(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_29_s_4(double * base, __m256i indices, __m512d source, __mmask8 mask) noexcept {
  native::mask_vscatterdpd<4>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int32_t,8,profile_1>::from_native(indices), native::simd<double,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_29_s_4(double * base, __m256i indices, __m512d source, __mmask8 mask) noexcept {
  _mm512_mask_i32scatter_pd(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_29_s_8(double * base, __m256i indices, __m512d source, __mmask8 mask) noexcept {
  native::mask_vscatterdpd<8>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int32_t,8,profile_1>::from_native(indices), native::simd<double,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_29_s_8(double * base, __m256i indices, __m512d source, __mmask8 mask) noexcept {
  _mm512_mask_i32scatter_pd(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_30_s_1(std::int64_t const * base, __m128i indices) noexcept {
  return native::vpgatherdq<1,2>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_30_s_1(std::int64_t const * base, __m128i indices) noexcept {
  return _mm_i32gather_epi64(reinterpret_cast<long long const *>(base), indices, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_30_s_2(std::int64_t const * base, __m128i indices) noexcept {
  return native::vpgatherdq<2,2>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_30_s_2(std::int64_t const * base, __m128i indices) noexcept {
  return _mm_i32gather_epi64(reinterpret_cast<long long const *>(base), indices, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_30_s_4(std::int64_t const * base, __m128i indices) noexcept {
  return native::vpgatherdq<4,2>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_30_s_4(std::int64_t const * base, __m128i indices) noexcept {
  return _mm_i32gather_epi64(reinterpret_cast<long long const *>(base), indices, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_30_s_8(std::int64_t const * base, __m128i indices) noexcept {
  return native::vpgatherdq<8,2>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_30_s_8(std::int64_t const * base, __m128i indices) noexcept {
  return _mm_i32gather_epi64(reinterpret_cast<long long const *>(base), indices, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_30_u_1(std::uint64_t const * base, __m128i indices) noexcept {
  return native::vpgatherdq<1,2>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_30_u_1(std::uint64_t const * base, __m128i indices) noexcept {
  return _mm_i32gather_epi64(reinterpret_cast<long long const *>(base), indices, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_30_u_2(std::uint64_t const * base, __m128i indices) noexcept {
  return native::vpgatherdq<2,2>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_30_u_2(std::uint64_t const * base, __m128i indices) noexcept {
  return _mm_i32gather_epi64(reinterpret_cast<long long const *>(base), indices, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_30_u_4(std::uint64_t const * base, __m128i indices) noexcept {
  return native::vpgatherdq<4,2>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_30_u_4(std::uint64_t const * base, __m128i indices) noexcept {
  return _mm_i32gather_epi64(reinterpret_cast<long long const *>(base), indices, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_30_u_8(std::uint64_t const * base, __m128i indices) noexcept {
  return native::vpgatherdq<8,2>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_30_u_8(std::uint64_t const * base, __m128i indices) noexcept {
  return _mm_i32gather_epi64(reinterpret_cast<long long const *>(base), indices, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_31_s_1(std::int64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherdq<1>(native::simd<std::int64_t,2,profile_0>::from_native(source), native::simd<std::int64_t,2,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_31_s_1(std::int64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i32gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_31_s_2(std::int64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherdq<2>(native::simd<std::int64_t,2,profile_0>::from_native(source), native::simd<std::int64_t,2,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_31_s_2(std::int64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i32gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_31_s_4(std::int64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherdq<4>(native::simd<std::int64_t,2,profile_0>::from_native(source), native::simd<std::int64_t,2,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_31_s_4(std::int64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i32gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_31_s_8(std::int64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherdq<8>(native::simd<std::int64_t,2,profile_0>::from_native(source), native::simd<std::int64_t,2,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_31_s_8(std::int64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i32gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_31_u_1(std::uint64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherdq<1>(native::simd<std::uint64_t,2,profile_0>::from_native(source), native::simd<std::int64_t,2,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_31_u_1(std::uint64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i32gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_31_u_2(std::uint64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherdq<2>(native::simd<std::uint64_t,2,profile_0>::from_native(source), native::simd<std::int64_t,2,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_31_u_2(std::uint64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i32gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_31_u_4(std::uint64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherdq<4>(native::simd<std::uint64_t,2,profile_0>::from_native(source), native::simd<std::int64_t,2,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_31_u_4(std::uint64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i32gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_31_u_8(std::uint64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherdq<8>(native::simd<std::uint64_t,2,profile_0>::from_native(source), native::simd<std::int64_t,2,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_31_u_8(std::uint64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i32gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_32_s_1(std::int64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdq<1>(native::simd<std::int64_t,2,profile_2>::from_native(source), native::predicate<2,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_32_s_1(std::int64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i32gather_epi64(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_32_s_2(std::int64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdq<2>(native::simd<std::int64_t,2,profile_2>::from_native(source), native::predicate<2,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_32_s_2(std::int64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i32gather_epi64(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_32_s_4(std::int64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdq<4>(native::simd<std::int64_t,2,profile_2>::from_native(source), native::predicate<2,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_32_s_4(std::int64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i32gather_epi64(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_32_s_8(std::int64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdq<8>(native::simd<std::int64_t,2,profile_2>::from_native(source), native::predicate<2,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_32_s_8(std::int64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i32gather_epi64(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_32_u_1(std::uint64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdq<1>(native::simd<std::uint64_t,2,profile_2>::from_native(source), native::predicate<2,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_32_u_1(std::uint64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i32gather_epi64(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_32_u_2(std::uint64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdq<2>(native::simd<std::uint64_t,2,profile_2>::from_native(source), native::predicate<2,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_32_u_2(std::uint64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i32gather_epi64(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_32_u_4(std::uint64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdq<4>(native::simd<std::uint64_t,2,profile_2>::from_native(source), native::predicate<2,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_32_u_4(std::uint64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i32gather_epi64(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_32_u_8(std::uint64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdq<8>(native::simd<std::uint64_t,2,profile_2>::from_native(source), native::predicate<2,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_32_u_8(std::uint64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i32gather_epi64(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_33_s_1(std::int64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdq<1>(base, native::predicate<2,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<std::int64_t,2,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_33_s_1(std::int64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i32scatter_epi64(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_33_s_2(std::int64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdq<2>(base, native::predicate<2,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<std::int64_t,2,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_33_s_2(std::int64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i32scatter_epi64(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_33_s_4(std::int64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdq<4>(base, native::predicate<2,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<std::int64_t,2,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_33_s_4(std::int64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i32scatter_epi64(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_33_s_8(std::int64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdq<8>(base, native::predicate<2,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<std::int64_t,2,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_33_s_8(std::int64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i32scatter_epi64(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_33_u_1(std::uint64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdq<1>(base, native::predicate<2,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<std::uint64_t,2,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_33_u_1(std::uint64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i32scatter_epi64(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_33_u_2(std::uint64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdq<2>(base, native::predicate<2,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<std::uint64_t,2,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_33_u_2(std::uint64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i32scatter_epi64(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_33_u_4(std::uint64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdq<4>(base, native::predicate<2,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<std::uint64_t,2,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_33_u_4(std::uint64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i32scatter_epi64(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_33_u_8(std::uint64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdq<8>(base, native::predicate<2,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<std::uint64_t,2,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_33_u_8(std::uint64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i32scatter_epi64(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_34_s_1(std::int64_t const * base, __m128i indices) noexcept {
  return native::vpgatherdq<1,4>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_34_s_1(std::int64_t const * base, __m128i indices) noexcept {
  return _mm256_i32gather_epi64(reinterpret_cast<long long const *>(base), indices, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_34_s_2(std::int64_t const * base, __m128i indices) noexcept {
  return native::vpgatherdq<2,4>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_34_s_2(std::int64_t const * base, __m128i indices) noexcept {
  return _mm256_i32gather_epi64(reinterpret_cast<long long const *>(base), indices, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_34_s_4(std::int64_t const * base, __m128i indices) noexcept {
  return native::vpgatherdq<4,4>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_34_s_4(std::int64_t const * base, __m128i indices) noexcept {
  return _mm256_i32gather_epi64(reinterpret_cast<long long const *>(base), indices, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_34_s_8(std::int64_t const * base, __m128i indices) noexcept {
  return native::vpgatherdq<8,4>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_34_s_8(std::int64_t const * base, __m128i indices) noexcept {
  return _mm256_i32gather_epi64(reinterpret_cast<long long const *>(base), indices, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_34_u_1(std::uint64_t const * base, __m128i indices) noexcept {
  return native::vpgatherdq<1,4>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_34_u_1(std::uint64_t const * base, __m128i indices) noexcept {
  return _mm256_i32gather_epi64(reinterpret_cast<long long const *>(base), indices, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_34_u_2(std::uint64_t const * base, __m128i indices) noexcept {
  return native::vpgatherdq<2,4>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_34_u_2(std::uint64_t const * base, __m128i indices) noexcept {
  return _mm256_i32gather_epi64(reinterpret_cast<long long const *>(base), indices, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_34_u_4(std::uint64_t const * base, __m128i indices) noexcept {
  return native::vpgatherdq<4,4>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_34_u_4(std::uint64_t const * base, __m128i indices) noexcept {
  return _mm256_i32gather_epi64(reinterpret_cast<long long const *>(base), indices, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_34_u_8(std::uint64_t const * base, __m128i indices) noexcept {
  return native::vpgatherdq<8,4>(base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_34_u_8(std::uint64_t const * base, __m128i indices) noexcept {
  return _mm256_i32gather_epi64(reinterpret_cast<long long const *>(base), indices, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_35_s_1(std::int64_t const * base, __m128i indices, __m256i source, __m256i mask) noexcept {
  return native::mask_vpgatherdq<1>(native::simd<std::int64_t,4,profile_0>::from_native(source), native::simd<std::int64_t,4,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_35_s_1(std::int64_t const * base, __m128i indices, __m256i source, __m256i mask) noexcept {
  return _mm256_mask_i32gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_35_s_2(std::int64_t const * base, __m128i indices, __m256i source, __m256i mask) noexcept {
  return native::mask_vpgatherdq<2>(native::simd<std::int64_t,4,profile_0>::from_native(source), native::simd<std::int64_t,4,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_35_s_2(std::int64_t const * base, __m128i indices, __m256i source, __m256i mask) noexcept {
  return _mm256_mask_i32gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_35_s_4(std::int64_t const * base, __m128i indices, __m256i source, __m256i mask) noexcept {
  return native::mask_vpgatherdq<4>(native::simd<std::int64_t,4,profile_0>::from_native(source), native::simd<std::int64_t,4,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_35_s_4(std::int64_t const * base, __m128i indices, __m256i source, __m256i mask) noexcept {
  return _mm256_mask_i32gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_35_s_8(std::int64_t const * base, __m128i indices, __m256i source, __m256i mask) noexcept {
  return native::mask_vpgatherdq<8>(native::simd<std::int64_t,4,profile_0>::from_native(source), native::simd<std::int64_t,4,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_35_s_8(std::int64_t const * base, __m128i indices, __m256i source, __m256i mask) noexcept {
  return _mm256_mask_i32gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_35_u_1(std::uint64_t const * base, __m128i indices, __m256i source, __m256i mask) noexcept {
  return native::mask_vpgatherdq<1>(native::simd<std::uint64_t,4,profile_0>::from_native(source), native::simd<std::int64_t,4,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_35_u_1(std::uint64_t const * base, __m128i indices, __m256i source, __m256i mask) noexcept {
  return _mm256_mask_i32gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_35_u_2(std::uint64_t const * base, __m128i indices, __m256i source, __m256i mask) noexcept {
  return native::mask_vpgatherdq<2>(native::simd<std::uint64_t,4,profile_0>::from_native(source), native::simd<std::int64_t,4,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_35_u_2(std::uint64_t const * base, __m128i indices, __m256i source, __m256i mask) noexcept {
  return _mm256_mask_i32gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_35_u_4(std::uint64_t const * base, __m128i indices, __m256i source, __m256i mask) noexcept {
  return native::mask_vpgatherdq<4>(native::simd<std::uint64_t,4,profile_0>::from_native(source), native::simd<std::int64_t,4,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_35_u_4(std::uint64_t const * base, __m128i indices, __m256i source, __m256i mask) noexcept {
  return _mm256_mask_i32gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_35_u_8(std::uint64_t const * base, __m128i indices, __m256i source, __m256i mask) noexcept {
  return native::mask_vpgatherdq<8>(native::simd<std::uint64_t,4,profile_0>::from_native(source), native::simd<std::int64_t,4,profile_0>::from_native(mask), base, native::simd<std::int32_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_35_u_8(std::uint64_t const * base, __m128i indices, __m256i source, __m256i mask) noexcept {
  return _mm256_mask_i32gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i public_36_s_1(std::int64_t const * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdq<1>(native::simd<std::int64_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i raw_36_s_1(std::int64_t const * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i32gather_epi64(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i public_36_s_2(std::int64_t const * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdq<2>(native::simd<std::int64_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i raw_36_s_2(std::int64_t const * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i32gather_epi64(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i public_36_s_4(std::int64_t const * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdq<4>(native::simd<std::int64_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i raw_36_s_4(std::int64_t const * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i32gather_epi64(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i public_36_s_8(std::int64_t const * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdq<8>(native::simd<std::int64_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i raw_36_s_8(std::int64_t const * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i32gather_epi64(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i public_36_u_1(std::uint64_t const * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdq<1>(native::simd<std::uint64_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i raw_36_u_1(std::uint64_t const * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i32gather_epi64(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i public_36_u_2(std::uint64_t const * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdq<2>(native::simd<std::uint64_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i raw_36_u_2(std::uint64_t const * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i32gather_epi64(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i public_36_u_4(std::uint64_t const * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdq<4>(native::simd<std::uint64_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i raw_36_u_4(std::uint64_t const * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i32gather_epi64(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i public_36_u_8(std::uint64_t const * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdq<8>(native::simd<std::uint64_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int32_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i raw_36_u_8(std::uint64_t const * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i32gather_epi64(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_37_s_1(std::int64_t * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdq<1>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<std::int64_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_37_s_1(std::int64_t * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  _mm256_mask_i32scatter_epi64(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_37_s_2(std::int64_t * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdq<2>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<std::int64_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_37_s_2(std::int64_t * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  _mm256_mask_i32scatter_epi64(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_37_s_4(std::int64_t * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdq<4>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<std::int64_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_37_s_4(std::int64_t * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  _mm256_mask_i32scatter_epi64(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_37_s_8(std::int64_t * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdq<8>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<std::int64_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_37_s_8(std::int64_t * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  _mm256_mask_i32scatter_epi64(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_37_u_1(std::uint64_t * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdq<1>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<std::uint64_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_37_u_1(std::uint64_t * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  _mm256_mask_i32scatter_epi64(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_37_u_2(std::uint64_t * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdq<2>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<std::uint64_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_37_u_2(std::uint64_t * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  _mm256_mask_i32scatter_epi64(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_37_u_4(std::uint64_t * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdq<4>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<std::uint64_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_37_u_4(std::uint64_t * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  _mm256_mask_i32scatter_epi64(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_37_u_8(std::uint64_t * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdq<8>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int32_t,4,profile_2>::from_native(indices), native::simd<std::uint64_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_37_u_8(std::uint64_t * base, __m128i indices, __m256i source, __mmask8 mask) noexcept {
  _mm256_mask_i32scatter_epi64(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i public_38_s_1(std::int64_t const * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdq<1>(native::simd<std::int64_t,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int32_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i raw_38_s_1(std::int64_t const * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  return _mm512_mask_i32gather_epi64(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i public_38_s_2(std::int64_t const * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdq<2>(native::simd<std::int64_t,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int32_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i raw_38_s_2(std::int64_t const * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  return _mm512_mask_i32gather_epi64(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i public_38_s_4(std::int64_t const * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdq<4>(native::simd<std::int64_t,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int32_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i raw_38_s_4(std::int64_t const * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  return _mm512_mask_i32gather_epi64(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i public_38_s_8(std::int64_t const * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdq<8>(native::simd<std::int64_t,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int32_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i raw_38_s_8(std::int64_t const * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  return _mm512_mask_i32gather_epi64(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i public_38_u_1(std::uint64_t const * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdq<1>(native::simd<std::uint64_t,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int32_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i raw_38_u_1(std::uint64_t const * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  return _mm512_mask_i32gather_epi64(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i public_38_u_2(std::uint64_t const * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdq<2>(native::simd<std::uint64_t,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int32_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i raw_38_u_2(std::uint64_t const * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  return _mm512_mask_i32gather_epi64(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i public_38_u_4(std::uint64_t const * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdq<4>(native::simd<std::uint64_t,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int32_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i raw_38_u_4(std::uint64_t const * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  return _mm512_mask_i32gather_epi64(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i public_38_u_8(std::uint64_t const * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherdq<8>(native::simd<std::uint64_t,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int32_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i raw_38_u_8(std::uint64_t const * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  return _mm512_mask_i32gather_epi64(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_39_s_1(std::int64_t * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdq<1>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int32_t,8,profile_1>::from_native(indices), native::simd<std::int64_t,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_39_s_1(std::int64_t * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  _mm512_mask_i32scatter_epi64(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_39_s_2(std::int64_t * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdq<2>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int32_t,8,profile_1>::from_native(indices), native::simd<std::int64_t,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_39_s_2(std::int64_t * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  _mm512_mask_i32scatter_epi64(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_39_s_4(std::int64_t * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdq<4>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int32_t,8,profile_1>::from_native(indices), native::simd<std::int64_t,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_39_s_4(std::int64_t * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  _mm512_mask_i32scatter_epi64(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_39_s_8(std::int64_t * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdq<8>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int32_t,8,profile_1>::from_native(indices), native::simd<std::int64_t,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_39_s_8(std::int64_t * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  _mm512_mask_i32scatter_epi64(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_39_u_1(std::uint64_t * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdq<1>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int32_t,8,profile_1>::from_native(indices), native::simd<std::uint64_t,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_39_u_1(std::uint64_t * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  _mm512_mask_i32scatter_epi64(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_39_u_2(std::uint64_t * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdq<2>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int32_t,8,profile_1>::from_native(indices), native::simd<std::uint64_t,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_39_u_2(std::uint64_t * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  _mm512_mask_i32scatter_epi64(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_39_u_4(std::uint64_t * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdq<4>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int32_t,8,profile_1>::from_native(indices), native::simd<std::uint64_t,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_39_u_4(std::uint64_t * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  _mm512_mask_i32scatter_epi64(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_39_u_8(std::uint64_t * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  native::mask_vpscatterdq<8>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int32_t,8,profile_1>::from_native(indices), native::simd<std::uint64_t,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_39_u_8(std::uint64_t * base, __m256i indices, __m512i source, __mmask8 mask) noexcept {
  _mm512_mask_i32scatter_epi64(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 public_40_s_1(float const * base, __m128i indices) noexcept {
  return native::vgatherqps<1,4>(base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 raw_40_s_1(float const * base, __m128i indices) noexcept {
  return _mm_i64gather_ps(base, indices, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 public_40_s_2(float const * base, __m128i indices) noexcept {
  return native::vgatherqps<2,4>(base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 raw_40_s_2(float const * base, __m128i indices) noexcept {
  return _mm_i64gather_ps(base, indices, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 public_40_s_4(float const * base, __m128i indices) noexcept {
  return native::vgatherqps<4,4>(base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 raw_40_s_4(float const * base, __m128i indices) noexcept {
  return _mm_i64gather_ps(base, indices, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 public_40_s_8(float const * base, __m128i indices) noexcept {
  return native::vgatherqps<8,4>(base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 raw_40_s_8(float const * base, __m128i indices) noexcept {
  return _mm_i64gather_ps(base, indices, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 public_41_s_1(float const * base, __m128i indices, __m128 source, __m128i mask) noexcept {
  return native::mask_vgatherqps<1>(native::simd<float,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 raw_41_s_1(float const * base, __m128i indices, __m128 source, __m128i mask) noexcept {
  return _mm_mask_i64gather_ps(source, base, indices, __builtin_bit_cast(__m128, mask), 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 public_41_s_2(float const * base, __m128i indices, __m128 source, __m128i mask) noexcept {
  return native::mask_vgatherqps<2>(native::simd<float,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 raw_41_s_2(float const * base, __m128i indices, __m128 source, __m128i mask) noexcept {
  return _mm_mask_i64gather_ps(source, base, indices, __builtin_bit_cast(__m128, mask), 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 public_41_s_4(float const * base, __m128i indices, __m128 source, __m128i mask) noexcept {
  return native::mask_vgatherqps<4>(native::simd<float,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 raw_41_s_4(float const * base, __m128i indices, __m128 source, __m128i mask) noexcept {
  return _mm_mask_i64gather_ps(source, base, indices, __builtin_bit_cast(__m128, mask), 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 public_41_s_8(float const * base, __m128i indices, __m128 source, __m128i mask) noexcept {
  return native::mask_vgatherqps<8>(native::simd<float,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 raw_41_s_8(float const * base, __m128i indices, __m128 source, __m128i mask) noexcept {
  return _mm_mask_i64gather_ps(source, base, indices, __builtin_bit_cast(__m128, mask), 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128 public_42_s_1(float const * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  return native::mask_vgatherqps<1>(native::simd<float,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,2,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128 raw_42_s_1(float const * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  return _mm_mmask_i64gather_ps(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128 public_42_s_2(float const * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  return native::mask_vgatherqps<2>(native::simd<float,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,2,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128 raw_42_s_2(float const * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  return _mm_mmask_i64gather_ps(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128 public_42_s_4(float const * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  return native::mask_vgatherqps<4>(native::simd<float,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,2,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128 raw_42_s_4(float const * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  return _mm_mmask_i64gather_ps(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128 public_42_s_8(float const * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  return native::mask_vgatherqps<8>(native::simd<float,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,2,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128 raw_42_s_8(float const * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  return _mm_mmask_i64gather_ps(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_43_s_1(float * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  native::mask_vscatterqps<1>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,2,profile_2>::from_native(indices), native::simd<float,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_43_s_1(float * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  _mm_mask_i64scatter_ps(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_43_s_2(float * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  native::mask_vscatterqps<2>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,2,profile_2>::from_native(indices), native::simd<float,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_43_s_2(float * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  _mm_mask_i64scatter_ps(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_43_s_4(float * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  native::mask_vscatterqps<4>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,2,profile_2>::from_native(indices), native::simd<float,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_43_s_4(float * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  _mm_mask_i64scatter_ps(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_43_s_8(float * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  native::mask_vscatterqps<8>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,2,profile_2>::from_native(indices), native::simd<float,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_43_s_8(float * base, __m128i indices, __m128 source, __mmask8 mask) noexcept {
  _mm_mask_i64scatter_ps(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 public_44_s_1(float const * base, __m256i indices) noexcept {
  return native::vgatherqps<1,4>(base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 raw_44_s_1(float const * base, __m256i indices) noexcept {
  return _mm256_i64gather_ps(base, indices, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 public_44_s_2(float const * base, __m256i indices) noexcept {
  return native::vgatherqps<2,4>(base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 raw_44_s_2(float const * base, __m256i indices) noexcept {
  return _mm256_i64gather_ps(base, indices, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 public_44_s_4(float const * base, __m256i indices) noexcept {
  return native::vgatherqps<4,4>(base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 raw_44_s_4(float const * base, __m256i indices) noexcept {
  return _mm256_i64gather_ps(base, indices, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 public_44_s_8(float const * base, __m256i indices) noexcept {
  return native::vgatherqps<8,4>(base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 raw_44_s_8(float const * base, __m256i indices) noexcept {
  return _mm256_i64gather_ps(base, indices, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 public_45_s_1(float const * base, __m256i indices, __m128 source, __m128i mask) noexcept {
  return native::mask_vgatherqps<1>(native::simd<float,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 raw_45_s_1(float const * base, __m256i indices, __m128 source, __m128i mask) noexcept {
  return _mm256_mask_i64gather_ps(source, base, indices, __builtin_bit_cast(__m128, mask), 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 public_45_s_2(float const * base, __m256i indices, __m128 source, __m128i mask) noexcept {
  return native::mask_vgatherqps<2>(native::simd<float,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 raw_45_s_2(float const * base, __m256i indices, __m128 source, __m128i mask) noexcept {
  return _mm256_mask_i64gather_ps(source, base, indices, __builtin_bit_cast(__m128, mask), 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 public_45_s_4(float const * base, __m256i indices, __m128 source, __m128i mask) noexcept {
  return native::mask_vgatherqps<4>(native::simd<float,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 raw_45_s_4(float const * base, __m256i indices, __m128 source, __m128i mask) noexcept {
  return _mm256_mask_i64gather_ps(source, base, indices, __builtin_bit_cast(__m128, mask), 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 public_45_s_8(float const * base, __m256i indices, __m128 source, __m128i mask) noexcept {
  return native::mask_vgatherqps<8>(native::simd<float,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128 raw_45_s_8(float const * base, __m256i indices, __m128 source, __m128i mask) noexcept {
  return _mm256_mask_i64gather_ps(source, base, indices, __builtin_bit_cast(__m128, mask), 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128 public_46_s_1(float const * base, __m256i indices, __m128 source, __mmask8 mask) noexcept {
  return native::mask_vgatherqps<1>(native::simd<float,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128 raw_46_s_1(float const * base, __m256i indices, __m128 source, __mmask8 mask) noexcept {
  return _mm256_mmask_i64gather_ps(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128 public_46_s_2(float const * base, __m256i indices, __m128 source, __mmask8 mask) noexcept {
  return native::mask_vgatherqps<2>(native::simd<float,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128 raw_46_s_2(float const * base, __m256i indices, __m128 source, __mmask8 mask) noexcept {
  return _mm256_mmask_i64gather_ps(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128 public_46_s_4(float const * base, __m256i indices, __m128 source, __mmask8 mask) noexcept {
  return native::mask_vgatherqps<4>(native::simd<float,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128 raw_46_s_4(float const * base, __m256i indices, __m128 source, __mmask8 mask) noexcept {
  return _mm256_mmask_i64gather_ps(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128 public_46_s_8(float const * base, __m256i indices, __m128 source, __mmask8 mask) noexcept {
  return native::mask_vgatherqps<8>(native::simd<float,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128 raw_46_s_8(float const * base, __m256i indices, __m128 source, __mmask8 mask) noexcept {
  return _mm256_mmask_i64gather_ps(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_47_s_1(float * base, __m256i indices, __m128 source, __mmask8 mask) noexcept {
  native::mask_vscatterqps<1>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,4,profile_2>::from_native(indices), native::simd<float,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_47_s_1(float * base, __m256i indices, __m128 source, __mmask8 mask) noexcept {
  _mm256_mask_i64scatter_ps(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_47_s_2(float * base, __m256i indices, __m128 source, __mmask8 mask) noexcept {
  native::mask_vscatterqps<2>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,4,profile_2>::from_native(indices), native::simd<float,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_47_s_2(float * base, __m256i indices, __m128 source, __mmask8 mask) noexcept {
  _mm256_mask_i64scatter_ps(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_47_s_4(float * base, __m256i indices, __m128 source, __mmask8 mask) noexcept {
  native::mask_vscatterqps<4>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,4,profile_2>::from_native(indices), native::simd<float,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_47_s_4(float * base, __m256i indices, __m128 source, __mmask8 mask) noexcept {
  _mm256_mask_i64scatter_ps(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_47_s_8(float * base, __m256i indices, __m128 source, __mmask8 mask) noexcept {
  native::mask_vscatterqps<8>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,4,profile_2>::from_native(indices), native::simd<float,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_47_s_8(float * base, __m256i indices, __m128 source, __mmask8 mask) noexcept {
  _mm256_mask_i64scatter_ps(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m256 public_48_s_1(float const * base, __m512i indices, __m256 source, __mmask8 mask) noexcept {
  return native::mask_vgatherqps<1>(native::simd<float,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int64_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m256 raw_48_s_1(float const * base, __m512i indices, __m256 source, __mmask8 mask) noexcept {
  return _mm512_mask_i64gather_ps(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m256 public_48_s_2(float const * base, __m512i indices, __m256 source, __mmask8 mask) noexcept {
  return native::mask_vgatherqps<2>(native::simd<float,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int64_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m256 raw_48_s_2(float const * base, __m512i indices, __m256 source, __mmask8 mask) noexcept {
  return _mm512_mask_i64gather_ps(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m256 public_48_s_4(float const * base, __m512i indices, __m256 source, __mmask8 mask) noexcept {
  return native::mask_vgatherqps<4>(native::simd<float,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int64_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m256 raw_48_s_4(float const * base, __m512i indices, __m256 source, __mmask8 mask) noexcept {
  return _mm512_mask_i64gather_ps(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m256 public_48_s_8(float const * base, __m512i indices, __m256 source, __mmask8 mask) noexcept {
  return native::mask_vgatherqps<8>(native::simd<float,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int64_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m256 raw_48_s_8(float const * base, __m512i indices, __m256 source, __mmask8 mask) noexcept {
  return _mm512_mask_i64gather_ps(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_49_s_1(float * base, __m512i indices, __m256 source, __mmask8 mask) noexcept {
  native::mask_vscatterqps<1>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int64_t,8,profile_1>::from_native(indices), native::simd<float,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_49_s_1(float * base, __m512i indices, __m256 source, __mmask8 mask) noexcept {
  _mm512_mask_i64scatter_ps(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_49_s_2(float * base, __m512i indices, __m256 source, __mmask8 mask) noexcept {
  native::mask_vscatterqps<2>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int64_t,8,profile_1>::from_native(indices), native::simd<float,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_49_s_2(float * base, __m512i indices, __m256 source, __mmask8 mask) noexcept {
  _mm512_mask_i64scatter_ps(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_49_s_4(float * base, __m512i indices, __m256 source, __mmask8 mask) noexcept {
  native::mask_vscatterqps<4>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int64_t,8,profile_1>::from_native(indices), native::simd<float,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_49_s_4(float * base, __m512i indices, __m256 source, __mmask8 mask) noexcept {
  _mm512_mask_i64scatter_ps(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_49_s_8(float * base, __m512i indices, __m256 source, __mmask8 mask) noexcept {
  native::mask_vscatterqps<8>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int64_t,8,profile_1>::from_native(indices), native::simd<float,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_49_s_8(float * base, __m512i indices, __m256 source, __mmask8 mask) noexcept {
  _mm512_mask_i64scatter_ps(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_50_s_1(std::int32_t const * base, __m128i indices) noexcept {
  return native::vpgatherqd<1,4>(base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_50_s_1(std::int32_t const * base, __m128i indices) noexcept {
  return _mm_i64gather_epi32(reinterpret_cast<int const *>(base), indices, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_50_s_2(std::int32_t const * base, __m128i indices) noexcept {
  return native::vpgatherqd<2,4>(base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_50_s_2(std::int32_t const * base, __m128i indices) noexcept {
  return _mm_i64gather_epi32(reinterpret_cast<int const *>(base), indices, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_50_s_4(std::int32_t const * base, __m128i indices) noexcept {
  return native::vpgatherqd<4,4>(base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_50_s_4(std::int32_t const * base, __m128i indices) noexcept {
  return _mm_i64gather_epi32(reinterpret_cast<int const *>(base), indices, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_50_s_8(std::int32_t const * base, __m128i indices) noexcept {
  return native::vpgatherqd<8,4>(base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_50_s_8(std::int32_t const * base, __m128i indices) noexcept {
  return _mm_i64gather_epi32(reinterpret_cast<int const *>(base), indices, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_50_u_1(std::uint32_t const * base, __m128i indices) noexcept {
  return native::vpgatherqd<1,4>(base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_50_u_1(std::uint32_t const * base, __m128i indices) noexcept {
  return _mm_i64gather_epi32(reinterpret_cast<int const *>(base), indices, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_50_u_2(std::uint32_t const * base, __m128i indices) noexcept {
  return native::vpgatherqd<2,4>(base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_50_u_2(std::uint32_t const * base, __m128i indices) noexcept {
  return _mm_i64gather_epi32(reinterpret_cast<int const *>(base), indices, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_50_u_4(std::uint32_t const * base, __m128i indices) noexcept {
  return native::vpgatherqd<4,4>(base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_50_u_4(std::uint32_t const * base, __m128i indices) noexcept {
  return _mm_i64gather_epi32(reinterpret_cast<int const *>(base), indices, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_50_u_8(std::uint32_t const * base, __m128i indices) noexcept {
  return native::vpgatherqd<8,4>(base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_50_u_8(std::uint32_t const * base, __m128i indices) noexcept {
  return _mm_i64gather_epi32(reinterpret_cast<int const *>(base), indices, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_51_s_1(std::int32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherqd<1>(native::simd<std::int32_t,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_51_s_1(std::int32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i64gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_51_s_2(std::int32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherqd<2>(native::simd<std::int32_t,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_51_s_2(std::int32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i64gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_51_s_4(std::int32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherqd<4>(native::simd<std::int32_t,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_51_s_4(std::int32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i64gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_51_s_8(std::int32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherqd<8>(native::simd<std::int32_t,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_51_s_8(std::int32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i64gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_51_u_1(std::uint32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherqd<1>(native::simd<std::uint32_t,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_51_u_1(std::uint32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i64gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_51_u_2(std::uint32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherqd<2>(native::simd<std::uint32_t,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_51_u_2(std::uint32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i64gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_51_u_4(std::uint32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherqd<4>(native::simd<std::uint32_t,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_51_u_4(std::uint32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i64gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_51_u_8(std::uint32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherqd<8>(native::simd<std::uint32_t,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_51_u_8(std::uint32_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i64gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_52_s_1(std::int32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqd<1>(native::simd<std::int32_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,2,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_52_s_1(std::int32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i64gather_epi32(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_52_s_2(std::int32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqd<2>(native::simd<std::int32_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,2,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_52_s_2(std::int32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i64gather_epi32(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_52_s_4(std::int32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqd<4>(native::simd<std::int32_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,2,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_52_s_4(std::int32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i64gather_epi32(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_52_s_8(std::int32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqd<8>(native::simd<std::int32_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,2,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_52_s_8(std::int32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i64gather_epi32(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_52_u_1(std::uint32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqd<1>(native::simd<std::uint32_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,2,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_52_u_1(std::uint32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i64gather_epi32(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_52_u_2(std::uint32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqd<2>(native::simd<std::uint32_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,2,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_52_u_2(std::uint32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i64gather_epi32(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_52_u_4(std::uint32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqd<4>(native::simd<std::uint32_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,2,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_52_u_4(std::uint32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i64gather_epi32(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_52_u_8(std::uint32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqd<8>(native::simd<std::uint32_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,2,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_52_u_8(std::uint32_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i64gather_epi32(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_53_s_1(std::int32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqd<1>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,2,profile_2>::from_native(indices), native::simd<std::int32_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_53_s_1(std::int32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i64scatter_epi32(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_53_s_2(std::int32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqd<2>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,2,profile_2>::from_native(indices), native::simd<std::int32_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_53_s_2(std::int32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i64scatter_epi32(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_53_s_4(std::int32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqd<4>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,2,profile_2>::from_native(indices), native::simd<std::int32_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_53_s_4(std::int32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i64scatter_epi32(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_53_s_8(std::int32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqd<8>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,2,profile_2>::from_native(indices), native::simd<std::int32_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_53_s_8(std::int32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i64scatter_epi32(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_53_u_1(std::uint32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqd<1>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,2,profile_2>::from_native(indices), native::simd<std::uint32_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_53_u_1(std::uint32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i64scatter_epi32(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_53_u_2(std::uint32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqd<2>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,2,profile_2>::from_native(indices), native::simd<std::uint32_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_53_u_2(std::uint32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i64scatter_epi32(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_53_u_4(std::uint32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqd<4>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,2,profile_2>::from_native(indices), native::simd<std::uint32_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_53_u_4(std::uint32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i64scatter_epi32(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_53_u_8(std::uint32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqd<8>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,2,profile_2>::from_native(indices), native::simd<std::uint32_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_53_u_8(std::uint32_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i64scatter_epi32(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_54_s_1(std::int32_t const * base, __m256i indices) noexcept {
  return native::vpgatherqd<1,4>(base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_54_s_1(std::int32_t const * base, __m256i indices) noexcept {
  return _mm256_i64gather_epi32(reinterpret_cast<int const *>(base), indices, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_54_s_2(std::int32_t const * base, __m256i indices) noexcept {
  return native::vpgatherqd<2,4>(base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_54_s_2(std::int32_t const * base, __m256i indices) noexcept {
  return _mm256_i64gather_epi32(reinterpret_cast<int const *>(base), indices, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_54_s_4(std::int32_t const * base, __m256i indices) noexcept {
  return native::vpgatherqd<4,4>(base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_54_s_4(std::int32_t const * base, __m256i indices) noexcept {
  return _mm256_i64gather_epi32(reinterpret_cast<int const *>(base), indices, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_54_s_8(std::int32_t const * base, __m256i indices) noexcept {
  return native::vpgatherqd<8,4>(base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_54_s_8(std::int32_t const * base, __m256i indices) noexcept {
  return _mm256_i64gather_epi32(reinterpret_cast<int const *>(base), indices, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_54_u_1(std::uint32_t const * base, __m256i indices) noexcept {
  return native::vpgatherqd<1,4>(base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_54_u_1(std::uint32_t const * base, __m256i indices) noexcept {
  return _mm256_i64gather_epi32(reinterpret_cast<int const *>(base), indices, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_54_u_2(std::uint32_t const * base, __m256i indices) noexcept {
  return native::vpgatherqd<2,4>(base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_54_u_2(std::uint32_t const * base, __m256i indices) noexcept {
  return _mm256_i64gather_epi32(reinterpret_cast<int const *>(base), indices, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_54_u_4(std::uint32_t const * base, __m256i indices) noexcept {
  return native::vpgatherqd<4,4>(base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_54_u_4(std::uint32_t const * base, __m256i indices) noexcept {
  return _mm256_i64gather_epi32(reinterpret_cast<int const *>(base), indices, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_54_u_8(std::uint32_t const * base, __m256i indices) noexcept {
  return native::vpgatherqd<8,4>(base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_54_u_8(std::uint32_t const * base, __m256i indices) noexcept {
  return _mm256_i64gather_epi32(reinterpret_cast<int const *>(base), indices, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_55_s_1(std::int32_t const * base, __m256i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherqd<1>(native::simd<std::int32_t,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_55_s_1(std::int32_t const * base, __m256i indices, __m128i source, __m128i mask) noexcept {
  return _mm256_mask_i64gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_55_s_2(std::int32_t const * base, __m256i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherqd<2>(native::simd<std::int32_t,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_55_s_2(std::int32_t const * base, __m256i indices, __m128i source, __m128i mask) noexcept {
  return _mm256_mask_i64gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_55_s_4(std::int32_t const * base, __m256i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherqd<4>(native::simd<std::int32_t,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_55_s_4(std::int32_t const * base, __m256i indices, __m128i source, __m128i mask) noexcept {
  return _mm256_mask_i64gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_55_s_8(std::int32_t const * base, __m256i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherqd<8>(native::simd<std::int32_t,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_55_s_8(std::int32_t const * base, __m256i indices, __m128i source, __m128i mask) noexcept {
  return _mm256_mask_i64gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_55_u_1(std::uint32_t const * base, __m256i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherqd<1>(native::simd<std::uint32_t,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_55_u_1(std::uint32_t const * base, __m256i indices, __m128i source, __m128i mask) noexcept {
  return _mm256_mask_i64gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_55_u_2(std::uint32_t const * base, __m256i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherqd<2>(native::simd<std::uint32_t,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_55_u_2(std::uint32_t const * base, __m256i indices, __m128i source, __m128i mask) noexcept {
  return _mm256_mask_i64gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_55_u_4(std::uint32_t const * base, __m256i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherqd<4>(native::simd<std::uint32_t,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_55_u_4(std::uint32_t const * base, __m256i indices, __m128i source, __m128i mask) noexcept {
  return _mm256_mask_i64gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_55_u_8(std::uint32_t const * base, __m256i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherqd<8>(native::simd<std::uint32_t,4,profile_0>::from_native(source), native::simd<std::int32_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_55_u_8(std::uint32_t const * base, __m256i indices, __m128i source, __m128i mask) noexcept {
  return _mm256_mask_i64gather_epi32(source, reinterpret_cast<int const *>(base), indices, mask, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_56_s_1(std::int32_t const * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqd<1>(native::simd<std::int32_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_56_s_1(std::int32_t const * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i64gather_epi32(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_56_s_2(std::int32_t const * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqd<2>(native::simd<std::int32_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_56_s_2(std::int32_t const * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i64gather_epi32(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_56_s_4(std::int32_t const * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqd<4>(native::simd<std::int32_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_56_s_4(std::int32_t const * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i64gather_epi32(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_56_s_8(std::int32_t const * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqd<8>(native::simd<std::int32_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_56_s_8(std::int32_t const * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i64gather_epi32(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_56_u_1(std::uint32_t const * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqd<1>(native::simd<std::uint32_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_56_u_1(std::uint32_t const * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i64gather_epi32(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_56_u_2(std::uint32_t const * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqd<2>(native::simd<std::uint32_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_56_u_2(std::uint32_t const * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i64gather_epi32(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_56_u_4(std::uint32_t const * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqd<4>(native::simd<std::uint32_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_56_u_4(std::uint32_t const * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i64gather_epi32(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_56_u_8(std::uint32_t const * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqd<8>(native::simd<std::uint32_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_56_u_8(std::uint32_t const * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i64gather_epi32(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_57_s_1(std::int32_t * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqd<1>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,4,profile_2>::from_native(indices), native::simd<std::int32_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_57_s_1(std::int32_t * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  _mm256_mask_i64scatter_epi32(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_57_s_2(std::int32_t * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqd<2>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,4,profile_2>::from_native(indices), native::simd<std::int32_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_57_s_2(std::int32_t * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  _mm256_mask_i64scatter_epi32(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_57_s_4(std::int32_t * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqd<4>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,4,profile_2>::from_native(indices), native::simd<std::int32_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_57_s_4(std::int32_t * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  _mm256_mask_i64scatter_epi32(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_57_s_8(std::int32_t * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqd<8>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,4,profile_2>::from_native(indices), native::simd<std::int32_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_57_s_8(std::int32_t * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  _mm256_mask_i64scatter_epi32(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_57_u_1(std::uint32_t * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqd<1>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,4,profile_2>::from_native(indices), native::simd<std::uint32_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_57_u_1(std::uint32_t * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  _mm256_mask_i64scatter_epi32(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_57_u_2(std::uint32_t * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqd<2>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,4,profile_2>::from_native(indices), native::simd<std::uint32_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_57_u_2(std::uint32_t * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  _mm256_mask_i64scatter_epi32(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_57_u_4(std::uint32_t * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqd<4>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,4,profile_2>::from_native(indices), native::simd<std::uint32_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_57_u_4(std::uint32_t * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  _mm256_mask_i64scatter_epi32(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_57_u_8(std::uint32_t * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqd<8>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,4,profile_2>::from_native(indices), native::simd<std::uint32_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_57_u_8(std::uint32_t * base, __m256i indices, __m128i source, __mmask8 mask) noexcept {
  _mm256_mask_i64scatter_epi32(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m256i public_58_s_1(std::int32_t const * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqd<1>(native::simd<std::int32_t,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int64_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m256i raw_58_s_1(std::int32_t const * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm512_mask_i64gather_epi32(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m256i public_58_s_2(std::int32_t const * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqd<2>(native::simd<std::int32_t,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int64_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m256i raw_58_s_2(std::int32_t const * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm512_mask_i64gather_epi32(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m256i public_58_s_4(std::int32_t const * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqd<4>(native::simd<std::int32_t,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int64_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m256i raw_58_s_4(std::int32_t const * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm512_mask_i64gather_epi32(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m256i public_58_s_8(std::int32_t const * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqd<8>(native::simd<std::int32_t,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int64_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m256i raw_58_s_8(std::int32_t const * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm512_mask_i64gather_epi32(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m256i public_58_u_1(std::uint32_t const * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqd<1>(native::simd<std::uint32_t,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int64_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m256i raw_58_u_1(std::uint32_t const * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm512_mask_i64gather_epi32(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m256i public_58_u_2(std::uint32_t const * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqd<2>(native::simd<std::uint32_t,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int64_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m256i raw_58_u_2(std::uint32_t const * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm512_mask_i64gather_epi32(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m256i public_58_u_4(std::uint32_t const * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqd<4>(native::simd<std::uint32_t,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int64_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m256i raw_58_u_4(std::uint32_t const * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm512_mask_i64gather_epi32(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m256i public_58_u_8(std::uint32_t const * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqd<8>(native::simd<std::uint32_t,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int64_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m256i raw_58_u_8(std::uint32_t const * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm512_mask_i64gather_epi32(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_59_s_1(std::int32_t * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqd<1>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int64_t,8,profile_1>::from_native(indices), native::simd<std::int32_t,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_59_s_1(std::int32_t * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  _mm512_mask_i64scatter_epi32(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_59_s_2(std::int32_t * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqd<2>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int64_t,8,profile_1>::from_native(indices), native::simd<std::int32_t,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_59_s_2(std::int32_t * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  _mm512_mask_i64scatter_epi32(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_59_s_4(std::int32_t * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqd<4>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int64_t,8,profile_1>::from_native(indices), native::simd<std::int32_t,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_59_s_4(std::int32_t * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  _mm512_mask_i64scatter_epi32(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_59_s_8(std::int32_t * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqd<8>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int64_t,8,profile_1>::from_native(indices), native::simd<std::int32_t,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_59_s_8(std::int32_t * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  _mm512_mask_i64scatter_epi32(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_59_u_1(std::uint32_t * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqd<1>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int64_t,8,profile_1>::from_native(indices), native::simd<std::uint32_t,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_59_u_1(std::uint32_t * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  _mm512_mask_i64scatter_epi32(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_59_u_2(std::uint32_t * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqd<2>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int64_t,8,profile_1>::from_native(indices), native::simd<std::uint32_t,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_59_u_2(std::uint32_t * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  _mm512_mask_i64scatter_epi32(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_59_u_4(std::uint32_t * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqd<4>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int64_t,8,profile_1>::from_native(indices), native::simd<std::uint32_t,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_59_u_4(std::uint32_t * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  _mm512_mask_i64scatter_epi32(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_59_u_8(std::uint32_t * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqd<8>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int64_t,8,profile_1>::from_native(indices), native::simd<std::uint32_t,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_59_u_8(std::uint32_t * base, __m512i indices, __m256i source, __mmask8 mask) noexcept {
  _mm512_mask_i64scatter_epi32(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d public_60_s_1(double const * base, __m128i indices) noexcept {
  return native::vgatherqpd<1,2>(base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d raw_60_s_1(double const * base, __m128i indices) noexcept {
  return _mm_i64gather_pd(base, indices, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d public_60_s_2(double const * base, __m128i indices) noexcept {
  return native::vgatherqpd<2,2>(base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d raw_60_s_2(double const * base, __m128i indices) noexcept {
  return _mm_i64gather_pd(base, indices, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d public_60_s_4(double const * base, __m128i indices) noexcept {
  return native::vgatherqpd<4,2>(base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d raw_60_s_4(double const * base, __m128i indices) noexcept {
  return _mm_i64gather_pd(base, indices, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d public_60_s_8(double const * base, __m128i indices) noexcept {
  return native::vgatherqpd<8,2>(base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d raw_60_s_8(double const * base, __m128i indices) noexcept {
  return _mm_i64gather_pd(base, indices, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d public_61_s_1(double const * base, __m128i indices, __m128d source, __m128i mask) noexcept {
  return native::mask_vgatherqpd<1>(native::simd<double,2,profile_0>::from_native(source), native::simd<std::int64_t,2,profile_0>::from_native(mask), base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d raw_61_s_1(double const * base, __m128i indices, __m128d source, __m128i mask) noexcept {
  return _mm_mask_i64gather_pd(source, base, indices, __builtin_bit_cast(__m128d, mask), 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d public_61_s_2(double const * base, __m128i indices, __m128d source, __m128i mask) noexcept {
  return native::mask_vgatherqpd<2>(native::simd<double,2,profile_0>::from_native(source), native::simd<std::int64_t,2,profile_0>::from_native(mask), base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d raw_61_s_2(double const * base, __m128i indices, __m128d source, __m128i mask) noexcept {
  return _mm_mask_i64gather_pd(source, base, indices, __builtin_bit_cast(__m128d, mask), 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d public_61_s_4(double const * base, __m128i indices, __m128d source, __m128i mask) noexcept {
  return native::mask_vgatherqpd<4>(native::simd<double,2,profile_0>::from_native(source), native::simd<std::int64_t,2,profile_0>::from_native(mask), base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d raw_61_s_4(double const * base, __m128i indices, __m128d source, __m128i mask) noexcept {
  return _mm_mask_i64gather_pd(source, base, indices, __builtin_bit_cast(__m128d, mask), 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d public_61_s_8(double const * base, __m128i indices, __m128d source, __m128i mask) noexcept {
  return native::mask_vgatherqpd<8>(native::simd<double,2,profile_0>::from_native(source), native::simd<std::int64_t,2,profile_0>::from_native(mask), base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128d raw_61_s_8(double const * base, __m128i indices, __m128d source, __m128i mask) noexcept {
  return _mm_mask_i64gather_pd(source, base, indices, __builtin_bit_cast(__m128d, mask), 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128d public_62_s_1(double const * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  return native::mask_vgatherqpd<1>(native::simd<double,2,profile_2>::from_native(source), native::predicate<2,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,2,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128d raw_62_s_1(double const * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  return _mm_mmask_i64gather_pd(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128d public_62_s_2(double const * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  return native::mask_vgatherqpd<2>(native::simd<double,2,profile_2>::from_native(source), native::predicate<2,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,2,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128d raw_62_s_2(double const * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  return _mm_mmask_i64gather_pd(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128d public_62_s_4(double const * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  return native::mask_vgatherqpd<4>(native::simd<double,2,profile_2>::from_native(source), native::predicate<2,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,2,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128d raw_62_s_4(double const * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  return _mm_mmask_i64gather_pd(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128d public_62_s_8(double const * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  return native::mask_vgatherqpd<8>(native::simd<double,2,profile_2>::from_native(source), native::predicate<2,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,2,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128d raw_62_s_8(double const * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  return _mm_mmask_i64gather_pd(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_63_s_1(double * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  native::mask_vscatterqpd<1>(base, native::predicate<2,profile_2>::from_bitset(mask), native::simd<std::int64_t,2,profile_2>::from_native(indices), native::simd<double,2,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_63_s_1(double * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  _mm_mask_i64scatter_pd(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_63_s_2(double * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  native::mask_vscatterqpd<2>(base, native::predicate<2,profile_2>::from_bitset(mask), native::simd<std::int64_t,2,profile_2>::from_native(indices), native::simd<double,2,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_63_s_2(double * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  _mm_mask_i64scatter_pd(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_63_s_4(double * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  native::mask_vscatterqpd<4>(base, native::predicate<2,profile_2>::from_bitset(mask), native::simd<std::int64_t,2,profile_2>::from_native(indices), native::simd<double,2,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_63_s_4(double * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  _mm_mask_i64scatter_pd(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_63_s_8(double * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  native::mask_vscatterqpd<8>(base, native::predicate<2,profile_2>::from_bitset(mask), native::simd<std::int64_t,2,profile_2>::from_native(indices), native::simd<double,2,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_63_s_8(double * base, __m128i indices, __m128d source, __mmask8 mask) noexcept {
  _mm_mask_i64scatter_pd(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d public_64_s_1(double const * base, __m256i indices) noexcept {
  return native::vgatherqpd<1,4>(base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d raw_64_s_1(double const * base, __m256i indices) noexcept {
  return _mm256_i64gather_pd(base, indices, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d public_64_s_2(double const * base, __m256i indices) noexcept {
  return native::vgatherqpd<2,4>(base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d raw_64_s_2(double const * base, __m256i indices) noexcept {
  return _mm256_i64gather_pd(base, indices, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d public_64_s_4(double const * base, __m256i indices) noexcept {
  return native::vgatherqpd<4,4>(base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d raw_64_s_4(double const * base, __m256i indices) noexcept {
  return _mm256_i64gather_pd(base, indices, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d public_64_s_8(double const * base, __m256i indices) noexcept {
  return native::vgatherqpd<8,4>(base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d raw_64_s_8(double const * base, __m256i indices) noexcept {
  return _mm256_i64gather_pd(base, indices, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d public_65_s_1(double const * base, __m256i indices, __m256d source, __m256i mask) noexcept {
  return native::mask_vgatherqpd<1>(native::simd<double,4,profile_0>::from_native(source), native::simd<std::int64_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d raw_65_s_1(double const * base, __m256i indices, __m256d source, __m256i mask) noexcept {
  return _mm256_mask_i64gather_pd(source, base, indices, __builtin_bit_cast(__m256d, mask), 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d public_65_s_2(double const * base, __m256i indices, __m256d source, __m256i mask) noexcept {
  return native::mask_vgatherqpd<2>(native::simd<double,4,profile_0>::from_native(source), native::simd<std::int64_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d raw_65_s_2(double const * base, __m256i indices, __m256d source, __m256i mask) noexcept {
  return _mm256_mask_i64gather_pd(source, base, indices, __builtin_bit_cast(__m256d, mask), 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d public_65_s_4(double const * base, __m256i indices, __m256d source, __m256i mask) noexcept {
  return native::mask_vgatherqpd<4>(native::simd<double,4,profile_0>::from_native(source), native::simd<std::int64_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d raw_65_s_4(double const * base, __m256i indices, __m256d source, __m256i mask) noexcept {
  return _mm256_mask_i64gather_pd(source, base, indices, __builtin_bit_cast(__m256d, mask), 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d public_65_s_8(double const * base, __m256i indices, __m256d source, __m256i mask) noexcept {
  return native::mask_vgatherqpd<8>(native::simd<double,4,profile_0>::from_native(source), native::simd<std::int64_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256d raw_65_s_8(double const * base, __m256i indices, __m256d source, __m256i mask) noexcept {
  return _mm256_mask_i64gather_pd(source, base, indices, __builtin_bit_cast(__m256d, mask), 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256d public_66_s_1(double const * base, __m256i indices, __m256d source, __mmask8 mask) noexcept {
  return native::mask_vgatherqpd<1>(native::simd<double,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256d raw_66_s_1(double const * base, __m256i indices, __m256d source, __mmask8 mask) noexcept {
  return _mm256_mmask_i64gather_pd(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256d public_66_s_2(double const * base, __m256i indices, __m256d source, __mmask8 mask) noexcept {
  return native::mask_vgatherqpd<2>(native::simd<double,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256d raw_66_s_2(double const * base, __m256i indices, __m256d source, __mmask8 mask) noexcept {
  return _mm256_mmask_i64gather_pd(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256d public_66_s_4(double const * base, __m256i indices, __m256d source, __mmask8 mask) noexcept {
  return native::mask_vgatherqpd<4>(native::simd<double,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256d raw_66_s_4(double const * base, __m256i indices, __m256d source, __mmask8 mask) noexcept {
  return _mm256_mmask_i64gather_pd(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256d public_66_s_8(double const * base, __m256i indices, __m256d source, __mmask8 mask) noexcept {
  return native::mask_vgatherqpd<8>(native::simd<double,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256d raw_66_s_8(double const * base, __m256i indices, __m256d source, __mmask8 mask) noexcept {
  return _mm256_mmask_i64gather_pd(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_67_s_1(double * base, __m256i indices, __m256d source, __mmask8 mask) noexcept {
  native::mask_vscatterqpd<1>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,4,profile_2>::from_native(indices), native::simd<double,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_67_s_1(double * base, __m256i indices, __m256d source, __mmask8 mask) noexcept {
  _mm256_mask_i64scatter_pd(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_67_s_2(double * base, __m256i indices, __m256d source, __mmask8 mask) noexcept {
  native::mask_vscatterqpd<2>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,4,profile_2>::from_native(indices), native::simd<double,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_67_s_2(double * base, __m256i indices, __m256d source, __mmask8 mask) noexcept {
  _mm256_mask_i64scatter_pd(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_67_s_4(double * base, __m256i indices, __m256d source, __mmask8 mask) noexcept {
  native::mask_vscatterqpd<4>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,4,profile_2>::from_native(indices), native::simd<double,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_67_s_4(double * base, __m256i indices, __m256d source, __mmask8 mask) noexcept {
  _mm256_mask_i64scatter_pd(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_67_s_8(double * base, __m256i indices, __m256d source, __mmask8 mask) noexcept {
  native::mask_vscatterqpd<8>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,4,profile_2>::from_native(indices), native::simd<double,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_67_s_8(double * base, __m256i indices, __m256d source, __mmask8 mask) noexcept {
  _mm256_mask_i64scatter_pd(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512d public_68_s_1(double const * base, __m512i indices, __m512d source, __mmask8 mask) noexcept {
  return native::mask_vgatherqpd<1>(native::simd<double,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int64_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512d raw_68_s_1(double const * base, __m512i indices, __m512d source, __mmask8 mask) noexcept {
  return _mm512_mask_i64gather_pd(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512d public_68_s_2(double const * base, __m512i indices, __m512d source, __mmask8 mask) noexcept {
  return native::mask_vgatherqpd<2>(native::simd<double,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int64_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512d raw_68_s_2(double const * base, __m512i indices, __m512d source, __mmask8 mask) noexcept {
  return _mm512_mask_i64gather_pd(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512d public_68_s_4(double const * base, __m512i indices, __m512d source, __mmask8 mask) noexcept {
  return native::mask_vgatherqpd<4>(native::simd<double,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int64_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512d raw_68_s_4(double const * base, __m512i indices, __m512d source, __mmask8 mask) noexcept {
  return _mm512_mask_i64gather_pd(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512d public_68_s_8(double const * base, __m512i indices, __m512d source, __mmask8 mask) noexcept {
  return native::mask_vgatherqpd<8>(native::simd<double,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int64_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512d raw_68_s_8(double const * base, __m512i indices, __m512d source, __mmask8 mask) noexcept {
  return _mm512_mask_i64gather_pd(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_69_s_1(double * base, __m512i indices, __m512d source, __mmask8 mask) noexcept {
  native::mask_vscatterqpd<1>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int64_t,8,profile_1>::from_native(indices), native::simd<double,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_69_s_1(double * base, __m512i indices, __m512d source, __mmask8 mask) noexcept {
  _mm512_mask_i64scatter_pd(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_69_s_2(double * base, __m512i indices, __m512d source, __mmask8 mask) noexcept {
  native::mask_vscatterqpd<2>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int64_t,8,profile_1>::from_native(indices), native::simd<double,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_69_s_2(double * base, __m512i indices, __m512d source, __mmask8 mask) noexcept {
  _mm512_mask_i64scatter_pd(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_69_s_4(double * base, __m512i indices, __m512d source, __mmask8 mask) noexcept {
  native::mask_vscatterqpd<4>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int64_t,8,profile_1>::from_native(indices), native::simd<double,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_69_s_4(double * base, __m512i indices, __m512d source, __mmask8 mask) noexcept {
  _mm512_mask_i64scatter_pd(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_69_s_8(double * base, __m512i indices, __m512d source, __mmask8 mask) noexcept {
  native::mask_vscatterqpd<8>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int64_t,8,profile_1>::from_native(indices), native::simd<double,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_69_s_8(double * base, __m512i indices, __m512d source, __mmask8 mask) noexcept {
  _mm512_mask_i64scatter_pd(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_70_s_1(std::int64_t const * base, __m128i indices) noexcept {
  return native::vpgatherqq<1,2>(base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_70_s_1(std::int64_t const * base, __m128i indices) noexcept {
  return _mm_i64gather_epi64(reinterpret_cast<long long const *>(base), indices, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_70_s_2(std::int64_t const * base, __m128i indices) noexcept {
  return native::vpgatherqq<2,2>(base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_70_s_2(std::int64_t const * base, __m128i indices) noexcept {
  return _mm_i64gather_epi64(reinterpret_cast<long long const *>(base), indices, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_70_s_4(std::int64_t const * base, __m128i indices) noexcept {
  return native::vpgatherqq<4,2>(base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_70_s_4(std::int64_t const * base, __m128i indices) noexcept {
  return _mm_i64gather_epi64(reinterpret_cast<long long const *>(base), indices, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_70_s_8(std::int64_t const * base, __m128i indices) noexcept {
  return native::vpgatherqq<8,2>(base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_70_s_8(std::int64_t const * base, __m128i indices) noexcept {
  return _mm_i64gather_epi64(reinterpret_cast<long long const *>(base), indices, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_70_u_1(std::uint64_t const * base, __m128i indices) noexcept {
  return native::vpgatherqq<1,2>(base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_70_u_1(std::uint64_t const * base, __m128i indices) noexcept {
  return _mm_i64gather_epi64(reinterpret_cast<long long const *>(base), indices, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_70_u_2(std::uint64_t const * base, __m128i indices) noexcept {
  return native::vpgatherqq<2,2>(base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_70_u_2(std::uint64_t const * base, __m128i indices) noexcept {
  return _mm_i64gather_epi64(reinterpret_cast<long long const *>(base), indices, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_70_u_4(std::uint64_t const * base, __m128i indices) noexcept {
  return native::vpgatherqq<4,2>(base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_70_u_4(std::uint64_t const * base, __m128i indices) noexcept {
  return _mm_i64gather_epi64(reinterpret_cast<long long const *>(base), indices, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_70_u_8(std::uint64_t const * base, __m128i indices) noexcept {
  return native::vpgatherqq<8,2>(base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_70_u_8(std::uint64_t const * base, __m128i indices) noexcept {
  return _mm_i64gather_epi64(reinterpret_cast<long long const *>(base), indices, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_71_s_1(std::int64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherqq<1>(native::simd<std::int64_t,2,profile_0>::from_native(source), native::simd<std::int64_t,2,profile_0>::from_native(mask), base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_71_s_1(std::int64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i64gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_71_s_2(std::int64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherqq<2>(native::simd<std::int64_t,2,profile_0>::from_native(source), native::simd<std::int64_t,2,profile_0>::from_native(mask), base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_71_s_2(std::int64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i64gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_71_s_4(std::int64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherqq<4>(native::simd<std::int64_t,2,profile_0>::from_native(source), native::simd<std::int64_t,2,profile_0>::from_native(mask), base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_71_s_4(std::int64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i64gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_71_s_8(std::int64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherqq<8>(native::simd<std::int64_t,2,profile_0>::from_native(source), native::simd<std::int64_t,2,profile_0>::from_native(mask), base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_71_s_8(std::int64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i64gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_71_u_1(std::uint64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherqq<1>(native::simd<std::uint64_t,2,profile_0>::from_native(source), native::simd<std::int64_t,2,profile_0>::from_native(mask), base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_71_u_1(std::uint64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i64gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_71_u_2(std::uint64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherqq<2>(native::simd<std::uint64_t,2,profile_0>::from_native(source), native::simd<std::int64_t,2,profile_0>::from_native(mask), base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_71_u_2(std::uint64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i64gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_71_u_4(std::uint64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherqq<4>(native::simd<std::uint64_t,2,profile_0>::from_native(source), native::simd<std::int64_t,2,profile_0>::from_native(mask), base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_71_u_4(std::uint64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i64gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i public_71_u_8(std::uint64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return native::mask_vpgatherqq<8>(native::simd<std::uint64_t,2,profile_0>::from_native(source), native::simd<std::int64_t,2,profile_0>::from_native(mask), base, native::simd<std::int64_t,2,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m128i raw_71_u_8(std::uint64_t const * base, __m128i indices, __m128i source, __m128i mask) noexcept {
  return _mm_mask_i64gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_72_s_1(std::int64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqq<1>(native::simd<std::int64_t,2,profile_2>::from_native(source), native::predicate<2,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,2,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_72_s_1(std::int64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i64gather_epi64(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_72_s_2(std::int64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqq<2>(native::simd<std::int64_t,2,profile_2>::from_native(source), native::predicate<2,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,2,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_72_s_2(std::int64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i64gather_epi64(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_72_s_4(std::int64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqq<4>(native::simd<std::int64_t,2,profile_2>::from_native(source), native::predicate<2,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,2,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_72_s_4(std::int64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i64gather_epi64(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_72_s_8(std::int64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqq<8>(native::simd<std::int64_t,2,profile_2>::from_native(source), native::predicate<2,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,2,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_72_s_8(std::int64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i64gather_epi64(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_72_u_1(std::uint64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqq<1>(native::simd<std::uint64_t,2,profile_2>::from_native(source), native::predicate<2,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,2,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_72_u_1(std::uint64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i64gather_epi64(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_72_u_2(std::uint64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqq<2>(native::simd<std::uint64_t,2,profile_2>::from_native(source), native::predicate<2,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,2,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_72_u_2(std::uint64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i64gather_epi64(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_72_u_4(std::uint64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqq<4>(native::simd<std::uint64_t,2,profile_2>::from_native(source), native::predicate<2,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,2,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_72_u_4(std::uint64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i64gather_epi64(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i public_72_u_8(std::uint64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqq<8>(native::simd<std::uint64_t,2,profile_2>::from_native(source), native::predicate<2,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,2,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m128i raw_72_u_8(std::uint64_t const * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  return _mm_mmask_i64gather_epi64(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_73_s_1(std::int64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqq<1>(base, native::predicate<2,profile_2>::from_bitset(mask), native::simd<std::int64_t,2,profile_2>::from_native(indices), native::simd<std::int64_t,2,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_73_s_1(std::int64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i64scatter_epi64(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_73_s_2(std::int64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqq<2>(base, native::predicate<2,profile_2>::from_bitset(mask), native::simd<std::int64_t,2,profile_2>::from_native(indices), native::simd<std::int64_t,2,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_73_s_2(std::int64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i64scatter_epi64(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_73_s_4(std::int64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqq<4>(base, native::predicate<2,profile_2>::from_bitset(mask), native::simd<std::int64_t,2,profile_2>::from_native(indices), native::simd<std::int64_t,2,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_73_s_4(std::int64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i64scatter_epi64(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_73_s_8(std::int64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqq<8>(base, native::predicate<2,profile_2>::from_bitset(mask), native::simd<std::int64_t,2,profile_2>::from_native(indices), native::simd<std::int64_t,2,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_73_s_8(std::int64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i64scatter_epi64(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_73_u_1(std::uint64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqq<1>(base, native::predicate<2,profile_2>::from_bitset(mask), native::simd<std::int64_t,2,profile_2>::from_native(indices), native::simd<std::uint64_t,2,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_73_u_1(std::uint64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i64scatter_epi64(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_73_u_2(std::uint64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqq<2>(base, native::predicate<2,profile_2>::from_bitset(mask), native::simd<std::int64_t,2,profile_2>::from_native(indices), native::simd<std::uint64_t,2,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_73_u_2(std::uint64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i64scatter_epi64(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_73_u_4(std::uint64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqq<4>(base, native::predicate<2,profile_2>::from_bitset(mask), native::simd<std::int64_t,2,profile_2>::from_native(indices), native::simd<std::uint64_t,2,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_73_u_4(std::uint64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i64scatter_epi64(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_73_u_8(std::uint64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqq<8>(base, native::predicate<2,profile_2>::from_bitset(mask), native::simd<std::int64_t,2,profile_2>::from_native(indices), native::simd<std::uint64_t,2,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_73_u_8(std::uint64_t * base, __m128i indices, __m128i source, __mmask8 mask) noexcept {
  _mm_mask_i64scatter_epi64(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_74_s_1(std::int64_t const * base, __m256i indices) noexcept {
  return native::vpgatherqq<1,4>(base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_74_s_1(std::int64_t const * base, __m256i indices) noexcept {
  return _mm256_i64gather_epi64(reinterpret_cast<long long const *>(base), indices, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_74_s_2(std::int64_t const * base, __m256i indices) noexcept {
  return native::vpgatherqq<2,4>(base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_74_s_2(std::int64_t const * base, __m256i indices) noexcept {
  return _mm256_i64gather_epi64(reinterpret_cast<long long const *>(base), indices, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_74_s_4(std::int64_t const * base, __m256i indices) noexcept {
  return native::vpgatherqq<4,4>(base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_74_s_4(std::int64_t const * base, __m256i indices) noexcept {
  return _mm256_i64gather_epi64(reinterpret_cast<long long const *>(base), indices, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_74_s_8(std::int64_t const * base, __m256i indices) noexcept {
  return native::vpgatherqq<8,4>(base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_74_s_8(std::int64_t const * base, __m256i indices) noexcept {
  return _mm256_i64gather_epi64(reinterpret_cast<long long const *>(base), indices, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_74_u_1(std::uint64_t const * base, __m256i indices) noexcept {
  return native::vpgatherqq<1,4>(base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_74_u_1(std::uint64_t const * base, __m256i indices) noexcept {
  return _mm256_i64gather_epi64(reinterpret_cast<long long const *>(base), indices, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_74_u_2(std::uint64_t const * base, __m256i indices) noexcept {
  return native::vpgatherqq<2,4>(base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_74_u_2(std::uint64_t const * base, __m256i indices) noexcept {
  return _mm256_i64gather_epi64(reinterpret_cast<long long const *>(base), indices, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_74_u_4(std::uint64_t const * base, __m256i indices) noexcept {
  return native::vpgatherqq<4,4>(base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_74_u_4(std::uint64_t const * base, __m256i indices) noexcept {
  return _mm256_i64gather_epi64(reinterpret_cast<long long const *>(base), indices, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_74_u_8(std::uint64_t const * base, __m256i indices) noexcept {
  return native::vpgatherqq<8,4>(base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_74_u_8(std::uint64_t const * base, __m256i indices) noexcept {
  return _mm256_i64gather_epi64(reinterpret_cast<long long const *>(base), indices, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_75_s_1(std::int64_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return native::mask_vpgatherqq<1>(native::simd<std::int64_t,4,profile_0>::from_native(source), native::simd<std::int64_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_75_s_1(std::int64_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return _mm256_mask_i64gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_75_s_2(std::int64_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return native::mask_vpgatherqq<2>(native::simd<std::int64_t,4,profile_0>::from_native(source), native::simd<std::int64_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_75_s_2(std::int64_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return _mm256_mask_i64gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_75_s_4(std::int64_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return native::mask_vpgatherqq<4>(native::simd<std::int64_t,4,profile_0>::from_native(source), native::simd<std::int64_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_75_s_4(std::int64_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return _mm256_mask_i64gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_75_s_8(std::int64_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return native::mask_vpgatherqq<8>(native::simd<std::int64_t,4,profile_0>::from_native(source), native::simd<std::int64_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_75_s_8(std::int64_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return _mm256_mask_i64gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_75_u_1(std::uint64_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return native::mask_vpgatherqq<1>(native::simd<std::uint64_t,4,profile_0>::from_native(source), native::simd<std::int64_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_75_u_1(std::uint64_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return _mm256_mask_i64gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_75_u_2(std::uint64_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return native::mask_vpgatherqq<2>(native::simd<std::uint64_t,4,profile_0>::from_native(source), native::simd<std::int64_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_75_u_2(std::uint64_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return _mm256_mask_i64gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_75_u_4(std::uint64_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return native::mask_vpgatherqq<4>(native::simd<std::uint64_t,4,profile_0>::from_native(source), native::simd<std::int64_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_75_u_4(std::uint64_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return _mm256_mask_i64gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i public_75_u_8(std::uint64_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return native::mask_vpgatherqq<8>(native::simd<std::uint64_t,4,profile_0>::from_native(source), native::simd<std::int64_t,4,profile_0>::from_native(mask), base, native::simd<std::int64_t,4,profile_0>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx2")
__m256i raw_75_u_8(std::uint64_t const * base, __m256i indices, __m256i source, __m256i mask) noexcept {
  return _mm256_mask_i64gather_epi64(source, reinterpret_cast<long long const *>(base), indices, mask, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i public_76_s_1(std::int64_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqq<1>(native::simd<std::int64_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i raw_76_s_1(std::int64_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i64gather_epi64(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i public_76_s_2(std::int64_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqq<2>(native::simd<std::int64_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i raw_76_s_2(std::int64_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i64gather_epi64(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i public_76_s_4(std::int64_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqq<4>(native::simd<std::int64_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i raw_76_s_4(std::int64_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i64gather_epi64(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i public_76_s_8(std::int64_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqq<8>(native::simd<std::int64_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i raw_76_s_8(std::int64_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i64gather_epi64(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i public_76_u_1(std::uint64_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqq<1>(native::simd<std::uint64_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i raw_76_u_1(std::uint64_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i64gather_epi64(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i public_76_u_2(std::uint64_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqq<2>(native::simd<std::uint64_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i raw_76_u_2(std::uint64_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i64gather_epi64(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i public_76_u_4(std::uint64_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqq<4>(native::simd<std::uint64_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i raw_76_u_4(std::uint64_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i64gather_epi64(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i public_76_u_8(std::uint64_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqq<8>(native::simd<std::uint64_t,4,profile_2>::from_native(source), native::predicate<4,profile_2>::from_bitset(mask), base, native::simd<std::int64_t,4,profile_2>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
__m256i raw_76_u_8(std::uint64_t const * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  return _mm256_mmask_i64gather_epi64(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_77_s_1(std::int64_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqq<1>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,4,profile_2>::from_native(indices), native::simd<std::int64_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_77_s_1(std::int64_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  _mm256_mask_i64scatter_epi64(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_77_s_2(std::int64_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqq<2>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,4,profile_2>::from_native(indices), native::simd<std::int64_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_77_s_2(std::int64_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  _mm256_mask_i64scatter_epi64(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_77_s_4(std::int64_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqq<4>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,4,profile_2>::from_native(indices), native::simd<std::int64_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_77_s_4(std::int64_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  _mm256_mask_i64scatter_epi64(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_77_s_8(std::int64_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqq<8>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,4,profile_2>::from_native(indices), native::simd<std::int64_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_77_s_8(std::int64_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  _mm256_mask_i64scatter_epi64(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_77_u_1(std::uint64_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqq<1>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,4,profile_2>::from_native(indices), native::simd<std::uint64_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_77_u_1(std::uint64_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  _mm256_mask_i64scatter_epi64(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_77_u_2(std::uint64_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqq<2>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,4,profile_2>::from_native(indices), native::simd<std::uint64_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_77_u_2(std::uint64_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  _mm256_mask_i64scatter_epi64(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_77_u_4(std::uint64_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqq<4>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,4,profile_2>::from_native(indices), native::simd<std::uint64_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_77_u_4(std::uint64_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  _mm256_mask_i64scatter_epi64(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void public_77_u_8(std::uint64_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqq<8>(base, native::predicate<4,profile_2>::from_bitset(mask), native::simd<std::int64_t,4,profile_2>::from_native(indices), native::simd<std::uint64_t,4,profile_2>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f,avx512vl")
void raw_77_u_8(std::uint64_t * base, __m256i indices, __m256i source, __mmask8 mask) noexcept {
  _mm256_mask_i64scatter_epi64(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i public_78_s_1(std::int64_t const * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqq<1>(native::simd<std::int64_t,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int64_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i raw_78_s_1(std::int64_t const * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  return _mm512_mask_i64gather_epi64(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i public_78_s_2(std::int64_t const * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqq<2>(native::simd<std::int64_t,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int64_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i raw_78_s_2(std::int64_t const * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  return _mm512_mask_i64gather_epi64(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i public_78_s_4(std::int64_t const * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqq<4>(native::simd<std::int64_t,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int64_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i raw_78_s_4(std::int64_t const * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  return _mm512_mask_i64gather_epi64(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i public_78_s_8(std::int64_t const * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqq<8>(native::simd<std::int64_t,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int64_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i raw_78_s_8(std::int64_t const * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  return _mm512_mask_i64gather_epi64(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i public_78_u_1(std::uint64_t const * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqq<1>(native::simd<std::uint64_t,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int64_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i raw_78_u_1(std::uint64_t const * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  return _mm512_mask_i64gather_epi64(source, mask, indices, base, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i public_78_u_2(std::uint64_t const * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqq<2>(native::simd<std::uint64_t,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int64_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i raw_78_u_2(std::uint64_t const * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  return _mm512_mask_i64gather_epi64(source, mask, indices, base, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i public_78_u_4(std::uint64_t const * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqq<4>(native::simd<std::uint64_t,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int64_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i raw_78_u_4(std::uint64_t const * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  return _mm512_mask_i64gather_epi64(source, mask, indices, base, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i public_78_u_8(std::uint64_t const * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  return native::mask_vpgatherqq<8>(native::simd<std::uint64_t,8,profile_1>::from_native(source), native::predicate<8,profile_1>::from_bitset(mask), base, native::simd<std::int64_t,8,profile_1>::from_native(indices)).to_native();
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
__m512i raw_78_u_8(std::uint64_t const * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  return _mm512_mask_i64gather_epi64(source, mask, indices, base, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_79_s_1(std::int64_t * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqq<1>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int64_t,8,profile_1>::from_native(indices), native::simd<std::int64_t,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_79_s_1(std::int64_t * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  _mm512_mask_i64scatter_epi64(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_79_s_2(std::int64_t * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqq<2>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int64_t,8,profile_1>::from_native(indices), native::simd<std::int64_t,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_79_s_2(std::int64_t * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  _mm512_mask_i64scatter_epi64(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_79_s_4(std::int64_t * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqq<4>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int64_t,8,profile_1>::from_native(indices), native::simd<std::int64_t,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_79_s_4(std::int64_t * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  _mm512_mask_i64scatter_epi64(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_79_s_8(std::int64_t * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqq<8>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int64_t,8,profile_1>::from_native(indices), native::simd<std::int64_t,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_79_s_8(std::int64_t * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  _mm512_mask_i64scatter_epi64(base, mask, indices, source, 8);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_79_u_1(std::uint64_t * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqq<1>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int64_t,8,profile_1>::from_native(indices), native::simd<std::uint64_t,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_79_u_1(std::uint64_t * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  _mm512_mask_i64scatter_epi64(base, mask, indices, source, 1);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_79_u_2(std::uint64_t * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqq<2>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int64_t,8,profile_1>::from_native(indices), native::simd<std::uint64_t,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_79_u_2(std::uint64_t * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  _mm512_mask_i64scatter_epi64(base, mask, indices, source, 2);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_79_u_4(std::uint64_t * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqq<4>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int64_t,8,profile_1>::from_native(indices), native::simd<std::uint64_t,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_79_u_4(std::uint64_t * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  _mm512_mask_i64scatter_epi64(base, mask, indices, source, 4);
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void public_79_u_8(std::uint64_t * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  native::mask_vpscatterqq<8>(base, native::predicate<8,profile_1>::from_bitset(mask), native::simd<std::int64_t,8,profile_1>::from_native(indices), native::simd<std::uint64_t,8,profile_1>::from_native(source));
}
extern "C" native_noinline NATIVE_MEMORY_TARGET("avx512f")
void raw_79_u_8(std::uint64_t * base, __m512i indices, __m512i source, __mmask8 mask) noexcept {
  _mm512_mask_i64scatter_epi64(base, mask, indices, source, 8);
}

#undef NATIVE_MEMORY_TARGET
