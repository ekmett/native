// Intrinsic calls are owned by the global module fragment.
#pragma clang attribute push(__attribute__((target("neon,bf16"))), apply_to=function)
namespace native::detail::neon_bf16_backend {
  native_inline float32x4_t dot2_native(bfloat16x8_t a, bfloat16x8_t b, float32x4_t accumulator) noexcept {
    return vbfdotq_f32(accumulator, a, b);
  }
}
#pragma clang attribute pop
