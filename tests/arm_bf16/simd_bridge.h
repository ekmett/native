// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
// The oracle uses ACLE storage; every operation crosses the public semantic API.
namespace instruction_fixture {
  template<native::isa<native::arm> Arch>
    requires(Arch.has(native::arm_feature::neon_bf16))
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x2_t bfdot(float32x2_t acc, bfloat16x4_t a, bfloat16x4_t b) noexcept {
    auto result = native::bfdot<Arch>(native::simd<float, 2, Arch>::from_storage(native::simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<float, 4, Arch>::native_type, vcombine_f32(acc, vdup_n_f32(0.f))))), native::simd<native::bf16, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 4, Arch>::native_type, a)), native::simd<native::bf16, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 4, Arch>::native_type, b)));
    static_assert(std::same_as<decltype(result), native::simd<float, 2, Arch>>);
    return vget_low_f32(result.to_storage().to_native());
  }
  template<native::isa<native::arm> Arch, unsigned Lane>
    requires(Arch.has(native::arm_feature::neon_bf16) && Lane < 2)
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x2_t bfdot_lane(float32x2_t acc, bfloat16x4_t a, bfloat16x4_t b) noexcept {
    auto result = native::bfdot_lane<Arch, Lane>(native::simd<float, 2, Arch>::from_storage(native::simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<float, 4, Arch>::native_type, vcombine_f32(acc, vdup_n_f32(0.f))))), native::simd<native::bf16, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 4, Arch>::native_type, a)), native::simd<native::bf16, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 4, Arch>::native_type, b)));
    static_assert(std::same_as<decltype(result), native::simd<float, 2, Arch>>);
    return vget_low_f32(result.to_storage().to_native());
  }
  template<native::isa<native::arm> Arch, unsigned Lane>
    requires(Arch.has(native::arm_feature::neon_bf16) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x2_t bfdot_lane(float32x2_t acc, bfloat16x4_t a, bfloat16x8_t b) noexcept {
    auto result = native::bfdot_lane<Arch, Lane>(native::simd<float, 2, Arch>::from_storage(native::simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<float, 4, Arch>::native_type, vcombine_f32(acc, vdup_n_f32(0.f))))), native::simd<native::bf16, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 4, Arch>::native_type, a)), native::simd<native::bf16, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 8, Arch>::native_type, b)));
    static_assert(std::same_as<decltype(result), native::simd<float, 2, Arch>>);
    return vget_low_f32(result.to_storage().to_native());
  }
  template<native::isa<native::arm> Arch>
    requires(Arch.has(native::arm_feature::neon_bf16))
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x4_t bfdot(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) noexcept {
    auto result = native::bfdot<Arch>(native::simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<float, 4, Arch>::native_type, acc)), native::simd<native::bf16, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 8, Arch>::native_type, a)), native::simd<native::bf16, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 8, Arch>::native_type, b)));
    static_assert(std::same_as<decltype(result), native::simd<float, 4, Arch>>);
    return result.to_native();
  }
  template<native::isa<native::arm> Arch, unsigned Lane>
    requires(Arch.has(native::arm_feature::neon_bf16) && Lane < 2)
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x4_t bfdot_lane(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) noexcept {
    auto result = native::bfdot_lane<Arch, Lane>(native::simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<float, 4, Arch>::native_type, acc)), native::simd<native::bf16, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 8, Arch>::native_type, a)), native::simd<native::bf16, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 4, Arch>::native_type, b)));
    static_assert(std::same_as<decltype(result), native::simd<float, 4, Arch>>);
    return result.to_native();
  }
  template<native::isa<native::arm> Arch, unsigned Lane>
    requires(Arch.has(native::arm_feature::neon_bf16) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x4_t bfdot_lane(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) noexcept {
    auto result = native::bfdot_lane<Arch, Lane>(native::simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<float, 4, Arch>::native_type, acc)), native::simd<native::bf16, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 8, Arch>::native_type, a)), native::simd<native::bf16, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 8, Arch>::native_type, b)));
    static_assert(std::same_as<decltype(result), native::simd<float, 4, Arch>>);
    return result.to_native();
  }
  template<native::isa<native::arm> Arch>
    requires(Arch.has(native::arm_feature::neon_bf16))
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x4_t bfmmla(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) noexcept {
    auto result = native::bfmmla<Arch>(native::simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<float, 4, Arch>::native_type, acc)), native::simd<native::bf16, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 8, Arch>::native_type, a)), native::simd<native::bf16, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 8, Arch>::native_type, b)));
    static_assert(std::same_as<decltype(result), native::simd<float, 4, Arch>>);
    return result.to_native();
  }
  template<native::isa<native::arm> Arch>
    requires(Arch.has(native::arm_feature::neon_bf16))
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x4_t bfmlalb(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) noexcept {
    auto result = native::bfmlalb<Arch>(native::simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<float, 4, Arch>::native_type, acc)), native::simd<native::bf16, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 8, Arch>::native_type, a)), native::simd<native::bf16, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 8, Arch>::native_type, b)));
    static_assert(std::same_as<decltype(result), native::simd<float, 4, Arch>>);
    return result.to_native();
  }
  template<native::isa<native::arm> Arch, unsigned Lane>
    requires(Arch.has(native::arm_feature::neon_bf16) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x4_t bfmlalb_lane(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) noexcept {
    auto result = native::bfmlalb_lane<Arch, Lane>(native::simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<float, 4, Arch>::native_type, acc)), native::simd<native::bf16, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 8, Arch>::native_type, a)), native::simd<native::bf16, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 4, Arch>::native_type, b)));
    static_assert(std::same_as<decltype(result), native::simd<float, 4, Arch>>);
    return result.to_native();
  }
  template<native::isa<native::arm> Arch, unsigned Lane>
    requires(Arch.has(native::arm_feature::neon_bf16) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x4_t bfmlalb_lane(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) noexcept {
    auto result = native::bfmlalb_lane<Arch, Lane>(native::simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<float, 4, Arch>::native_type, acc)), native::simd<native::bf16, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 8, Arch>::native_type, a)), native::simd<native::bf16, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 8, Arch>::native_type, b)));
    static_assert(std::same_as<decltype(result), native::simd<float, 4, Arch>>);
    return result.to_native();
  }
  template<native::isa<native::arm> Arch>
    requires(Arch.has(native::arm_feature::neon_bf16))
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x4_t bfmlalt(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) noexcept {
    auto result = native::bfmlalt<Arch>(native::simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<float, 4, Arch>::native_type, acc)), native::simd<native::bf16, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 8, Arch>::native_type, a)), native::simd<native::bf16, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 8, Arch>::native_type, b)));
    static_assert(std::same_as<decltype(result), native::simd<float, 4, Arch>>);
    return result.to_native();
  }
  template<native::isa<native::arm> Arch, unsigned Lane>
    requires(Arch.has(native::arm_feature::neon_bf16) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x4_t bfmlalt_lane(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) noexcept {
    auto result = native::bfmlalt_lane<Arch, Lane>(native::simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<float, 4, Arch>::native_type, acc)), native::simd<native::bf16, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 8, Arch>::native_type, a)), native::simd<native::bf16, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 4, Arch>::native_type, b)));
    static_assert(std::same_as<decltype(result), native::simd<float, 4, Arch>>);
    return result.to_native();
  }
  template<native::isa<native::arm> Arch, unsigned Lane>
    requires(Arch.has(native::arm_feature::neon_bf16) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("bf16")))
  float32x4_t bfmlalt_lane(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) noexcept {
    auto result = native::bfmlalt_lane<Arch, Lane>(native::simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename native::simd<float, 4, Arch>::native_type, acc)), native::simd<native::bf16, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 8, Arch>::native_type, a)), native::simd<native::bf16, 8, Arch>::from_native(__builtin_bit_cast(typename native::simd<native::bf16, 8, Arch>::native_type, b)));
    static_assert(std::same_as<decltype(result), native::simd<float, 4, Arch>>);
    return result.to_native();
  }
}
