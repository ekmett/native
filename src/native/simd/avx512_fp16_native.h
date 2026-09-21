// Intrinsic calls are owned by the global module fragment.
#pragma clang attribute push(__attribute__((target("avx2,fma,avx512f,avx512dq,avx512bw,avx512vl,avx512fp16"))), apply_to=function)
namespace native::detail::avx512_fp16_backend {
  // Explicit native builtins avoid TU-level excess-precision widening when the
  // provider is compiled below AVX512-FP16. MXCSR still supplies rounding.
  native_inline __m512h add_half(__m512h a, __m512h b) noexcept { return _mm512_add_round_ph(a,b,_MM_FROUND_CUR_DIRECTION); }
  native_inline __m512h sub_half(__m512h a, __m512h b) noexcept { return _mm512_sub_round_ph(a,b,_MM_FROUND_CUR_DIRECTION); }
  native_inline __m512h mul_half(__m512h a, __m512h b) noexcept { return _mm512_mul_round_ph(a,b,_MM_FROUND_CUR_DIRECTION); }
  native_inline __m512h div_half(__m512h a, __m512h b) noexcept { return _mm512_div_round_ph(a,b,_MM_FROUND_CUR_DIRECTION); }
  native_inline __m512h sqrt_half(__m512h a) noexcept { return _mm512_sqrt_ph(a); }
  native_inline __m512h neg_half(__m512h a) noexcept {
    return _mm512_castsi512_ph(_mm512_xor_si512(_mm512_castph_si512(a),_mm512_set1_epi16(short(0x8000))));
  }
  native_inline __m512h fma_half(__m512h a, __m512h b, __m512h c) noexcept { return _mm512_fmadd_ph(a,b,c); }
  native_inline __mmask32 eq_half(__m512h a, __m512h b) noexcept { return _mm512_cmp_ph_mask(a,b,_CMP_EQ_OQ); }
  native_inline __mmask32 lt_half(__m512h a, __m512h b) noexcept { return _mm512_cmp_ph_mask(a,b,_CMP_LT_OQ); }
  native_inline __mmask32 le_half(__m512h a, __m512h b) noexcept { return _mm512_cmp_ph_mask(a,b,_CMP_LE_OQ); }
  native_inline __m512h select_half(__mmask32 m, __m512h a, __m512h b) noexcept {
    return _mm512_mask_blend_ph(m,b,a);
  }
}
#pragma clang attribute pop
