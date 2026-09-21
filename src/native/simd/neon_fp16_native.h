// Intrinsic calls are owned by the global module fragment.
#pragma clang attribute push(__attribute__((target("neon,fullfp16"))), apply_to=function)
namespace native::detail::neon_fp16_backend {
  native_inline float16x8_t add_half(float16x8_t a, float16x8_t b) noexcept { return vaddq_f16(a,b); }
  native_inline float16x8_t sub_half(float16x8_t a, float16x8_t b) noexcept { return vsubq_f16(a,b); }
  native_inline float16x8_t mul_half(float16x8_t a, float16x8_t b) noexcept { return vmulq_f16(a,b); }
  native_inline float16x8_t div_half(float16x8_t a, float16x8_t b) noexcept { return vdivq_f16(a,b); }
  native_inline float16x8_t sqrt_half(float16x8_t a) noexcept { return vsqrtq_f16(a); }
  native_inline float16x8_t neg_half(float16x8_t a) noexcept { return vnegq_f16(a); }
  native_inline float16x8_t fma_half(float16x8_t a, float16x8_t b, float16x8_t c) noexcept { return vfmaq_f16(c,a,b); }
  native_inline uint8x16_t eq_half(float16x8_t a, float16x8_t b) noexcept { return vreinterpretq_u8_u16(vceqq_f16(a,b)); }
  native_inline uint8x16_t lt_half(float16x8_t a, float16x8_t b) noexcept { return vreinterpretq_u8_u16(vcltq_f16(a,b)); }
  native_inline uint8x16_t le_half(float16x8_t a, float16x8_t b) noexcept { return vreinterpretq_u8_u16(vcleq_f16(a,b)); }
  native_inline float16x8_t select_half(uint8x16_t m, float16x8_t a, float16x8_t b) noexcept {
    return vbslq_f16(vreinterpretq_u16_u8(m),a,b);
  }
}
#pragma clang attribute pop
