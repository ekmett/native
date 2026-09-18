// Intrinsic calls are owned by the global module fragment.
#pragma clang attribute push(__attribute__((target("avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512vl,avx512bf16"))), apply_to=function)
namespace simd::detail::avx512_bf16_backend {
  simd_inline __m128 dot2_native(__m128bh a, __m128bh b, __m128 accumulator) noexcept {
    return _mm_dpbf16_ps(accumulator, a, b);
  }
  simd_inline __m256 dot2_native(__m256bh a, __m256bh b, __m256 accumulator) noexcept {
    return _mm256_dpbf16_ps(accumulator, a, b);
  }
  simd_inline __m512 dot2_native(__m512bh a, __m512bh b, __m512 accumulator) noexcept {
    return _mm512_dpbf16_ps(accumulator, a, b);
  }
}
#pragma clang attribute pop
