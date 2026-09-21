// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "simd_adapter.h"
#include "simd_contract.h"
namespace fp16fml_fixture {
  constexpr native::isa<native::arm> arch{native::arm_feature::fp16fml};
  template<native::isa<native::arm> A, class F, class H> concept available = requires(F f, H h) {
    { fp16fml_api::fmlal<A>(f, h, h) } noexcept -> std::same_as<F>;
    { fp16fml_api::fmlal2<A>(f, h, h) } noexcept -> std::same_as<F>;
    { fp16fml_api::fmlsl<A>(f, h, h) } noexcept -> std::same_as<F>;
    { fp16fml_api::fmlsl2<A>(f, h, h) } noexcept -> std::same_as<F>;
  };
  template<native::isa<native::arm> A, unsigned L, class F, class H, class B>
  concept lane_available = requires(F f, H h, B b) {
    fp16fml_api::fmlal_lane<A, L>(f, h, b);
    fp16fml_api::fmlal2_lane<A, L>(f, h, b);
    fp16fml_api::fmlsl_lane<A, L>(f, h, b);
    fp16fml_api::fmlsl2_lane<A, L>(f, h, b);
  };
  // Weak feature tags participate only through consteval overloads; runtime inputs are rejected.
  static_assert(available<arch, float32x2_t, float16x4_t>);
  static_assert(!available<native::scalar, float32x2_t, float16x4_t>);
  static_assert(available<native::neon, float32x2_t, float16x4_t>);
  static_assert(available<native::neon_fp16, float32x2_t, float16x4_t>);
  static_assert(available<native::isa<native::arm>{native::arm_feature::complxnum}, float32x2_t, float16x4_t>);
  static_assert(available<native::neon_bf16, float32x2_t, float16x4_t>);
  static_assert(lane_available<arch, 3, float32x2_t, float16x4_t, float16x4_t>);
  static_assert(!lane_available<arch, 4, float32x2_t, float16x4_t, float16x4_t>);
  static_assert(lane_available<arch, 7, float32x2_t, float16x4_t, float16x8_t>);
  static_assert(!lane_available<arch, 8, float32x2_t, float16x4_t, float16x8_t>);
  static_assert(available<arch, float32x4_t, float16x8_t>);
  static_assert(!available<native::scalar, float32x4_t, float16x8_t>);
  static_assert(available<native::neon, float32x4_t, float16x8_t>);
  static_assert(available<native::neon_fp16, float32x4_t, float16x8_t>);
  static_assert(available<native::isa<native::arm>{native::arm_feature::complxnum}, float32x4_t, float16x8_t>);
  static_assert(available<native::neon_bf16, float32x4_t, float16x8_t>);
  static_assert(lane_available<arch, 3, float32x4_t, float16x8_t, float16x4_t>);
  static_assert(!lane_available<arch, 4, float32x4_t, float16x8_t, float16x4_t>);
  static_assert(lane_available<arch, 7, float32x4_t, float16x8_t, float16x8_t>);
  static_assert(!lane_available<arch, 8, float32x4_t, float16x8_t, float16x8_t>);

  consteval bool admission_contract() {
    native::arm_capabilities cpu{};
    cpu.present.set(native::arm_feature::neon, true);
    cpu.observed.set(native::arm_feature::neon, true);
    cpu.present.set(native::arm_feature::neon_fp16, true);
    cpu.observed.set(native::arm_feature::neon_fp16, true);
    cpu.present.set(native::arm_feature::fp16fml, true);
    cpu.observed.set(native::arm_feature::fp16fml, true);
    if(!native::classify_isa(cpu, arch).admitted()) return false;
    for(auto feature : {native::arm_feature::neon, native::arm_feature::neon_fp16, native::arm_feature::fp16fml}) {
      auto missing = cpu;
      missing.present.set(feature, false);
      if(native::classify_isa(missing, arch).admitted()) return false;
      missing = cpu;
      missing.observed.set(feature, false);
      if(native::classify_isa(missing, arch).admitted()) return false;
    }
    return true;
  }
  static_assert(admission_contract());

  inline std::uint64_t fpcr() {
    std::uint64_t value; asm volatile("mrs %0, fpcr" : "=r"(value)); return value;
  }
  inline bool same(float a, float b) {
    return std::bit_cast<std::uint32_t>(a) == std::bit_cast<std::uint32_t>(b)
      || (std::isnan(a) && std::isnan(b));
  }
  template<class F, class H, class B, unsigned Lane>
  __attribute__((target("fp16fml"), noinline))
  bool check(F acc, H a, B b) {
    constexpr unsigned n = sizeof(F) / sizeof(float);
    auto l = fp16fml_api::fmlal_lane<arch, Lane>(acc, a, b);
    auto h = fp16fml_api::fmlal2_lane<arch, Lane>(acc, a, b);
    auto sl = fp16fml_api::fmlsl_lane<arch, Lane>(acc, a, b);
    auto sh = fp16fml_api::fmlsl2_lane<arch, Lane>(acc, a, b);
    for(unsigned i = 0; i != n; ++i) {
      float bv = float(b[Lane]);
      if(!same(l[i], std::fma(float(a[i]), bv, acc[i])) ||
         !same(h[i], std::fma(float(a[i+n]), bv, acc[i])) ||
         !same(sl[i], std::fma(-float(a[i]), bv, acc[i])) ||
         !same(sh[i], std::fma(-float(a[i+n]), bv, acc[i]))) return false;
    }
    return true;
  }
  template<class F, class H>
  __attribute__((target("fp16fml"), noinline))
  bool check(F acc, H a, H b) {
    constexpr unsigned n = sizeof(F) / sizeof(float);
    auto l = fp16fml_api::fmlal<arch>(acc, a, b);
    auto h = fp16fml_api::fmlal2<arch>(acc, a, b);
    auto sl = fp16fml_api::fmlsl<arch>(acc, a, b);
    auto sh = fp16fml_api::fmlsl2<arch>(acc, a, b);
    for(unsigned i = 0; i != n; ++i) {
      if(!same(l[i], std::fma(float(a[i]), float(b[i]), acc[i])) ||
         !same(h[i], std::fma(float(a[i+n]), float(b[i+n]), acc[i])) ||
         !same(sl[i], std::fma(-float(a[i]), float(b[i]), acc[i])) ||
         !same(sh[i], std::fma(-float(a[i+n]), float(b[i+n]), acc[i]))) return false;
    }
    return true;
  }
  __attribute__((target("fp16fml"), noinline))
  bool exercise(unsigned seed) {
    auto control = fpcr();
    for(unsigned k = 0; k != 256; ++k) {
      float16x8_t a{}, b{}; float32x4_t acc{};
      for(unsigned i = 0; i != 8; ++i) {
        seed = seed * 1664525u + 1013904223u;
        a[i] = __builtin_bit_cast(__fp16, std::uint16_t((seed & 0x83ffu) | 0x3800u));
        seed = seed * 1664525u + 1013904223u;
        b[i] = __builtin_bit_cast(__fp16, std::uint16_t((seed & 0x83ffu) | 0x4000u));
        if(i < 4) acc[i] = std::bit_cast<float>((seed & 0x807fffffu) | 0x3f000000u);
      }
      auto a2 = vget_low_f16(a); auto b2 = vget_low_f16(b); auto acc2 = vget_low_f32(acc);
      if(!check(acc, a, b) || !check(acc2, a2, b2)) return false;
      if(!check<float32x2_t, float16x4_t, float16x4_t, 0>(acc2, a2, b2)) return false;
      if(!check<float32x2_t, float16x4_t, float16x4_t, 1>(acc2, a2, b2)) return false;
      if(!check<float32x2_t, float16x4_t, float16x4_t, 2>(acc2, a2, b2)) return false;
      if(!check<float32x2_t, float16x4_t, float16x4_t, 3>(acc2, a2, b2)) return false;
      if(!check<float32x2_t, float16x4_t, float16x8_t, 0>(acc2, a2, b)) return false;
      if(!check<float32x2_t, float16x4_t, float16x8_t, 1>(acc2, a2, b)) return false;
      if(!check<float32x2_t, float16x4_t, float16x8_t, 2>(acc2, a2, b)) return false;
      if(!check<float32x2_t, float16x4_t, float16x8_t, 3>(acc2, a2, b)) return false;
      if(!check<float32x2_t, float16x4_t, float16x8_t, 4>(acc2, a2, b)) return false;
      if(!check<float32x2_t, float16x4_t, float16x8_t, 5>(acc2, a2, b)) return false;
      if(!check<float32x2_t, float16x4_t, float16x8_t, 6>(acc2, a2, b)) return false;
      if(!check<float32x2_t, float16x4_t, float16x8_t, 7>(acc2, a2, b)) return false;
      if(!check<float32x4_t, float16x8_t, float16x4_t, 0>(acc, a, b2)) return false;
      if(!check<float32x4_t, float16x8_t, float16x4_t, 1>(acc, a, b2)) return false;
      if(!check<float32x4_t, float16x8_t, float16x4_t, 2>(acc, a, b2)) return false;
      if(!check<float32x4_t, float16x8_t, float16x4_t, 3>(acc, a, b2)) return false;
      if(!check<float32x4_t, float16x8_t, float16x8_t, 0>(acc, a, b)) return false;
      if(!check<float32x4_t, float16x8_t, float16x8_t, 1>(acc, a, b)) return false;
      if(!check<float32x4_t, float16x8_t, float16x8_t, 2>(acc, a, b)) return false;
      if(!check<float32x4_t, float16x8_t, float16x8_t, 3>(acc, a, b)) return false;
      if(!check<float32x4_t, float16x8_t, float16x8_t, 4>(acc, a, b)) return false;
      if(!check<float32x4_t, float16x8_t, float16x8_t, 5>(acc, a, b)) return false;
      if(!check<float32x4_t, float16x8_t, float16x8_t, 6>(acc, a, b)) return false;
      if(!check<float32x4_t, float16x8_t, float16x8_t, 7>(acc, a, b)) return false;
    }
    // Signed zero, subnormal inputs, and invalid 0*infinity are observed under
    // the caller's default FPCR; NaN payload identity is deliberately not assumed.
    float16x8_t a{0, -0.0, 1, -1, 2, -2, 0, 0};
    float16x8_t b{1, 1, 1, 1, 1, 1, 1, 1};
    float32x4_t acc{0, -0.0f, 0, 0};
    if(!check(acc, a, b)) return false;
    a[0] = __builtin_bit_cast(__fp16, std::uint16_t(1));
    if(!check(acc, a, b)) return false;
    a[0] = 0; b[0] = __builtin_bit_cast(__fp16, std::uint16_t(0x7c00));
    std::feclearexcept(FE_ALL_EXCEPT);
    volatile auto invalid = fp16fml_api::fmlal<arch>(acc, a, b);
    (void)invalid;
    if(!(std::fetestexcept(FE_INVALID) & FE_INVALID)) return false;
    std::feclearexcept(FE_ALL_EXCEPT);
    (void)fp16fml_api::fmlal<arch>(acc, a, b);
    if(!(std::fetestexcept(FE_INVALID) & FE_INVALID)) return false;
    if(std::feraiseexcept(FE_DIVBYZERO) != 0) return false;
    b[0] = 1;
    (void)fp16fml_api::fmlal<arch>(acc, a, b);
    if(!(std::fetestexcept(FE_DIVBYZERO) & FE_DIVBYZERO)) return false;
    return fpcr() == control;
  }
  inline int run(unsigned seed) {
    auto admission = native::classify_isa(native::observe_arm_capabilities(), arch);
    if(!admission.admitted()) {
      std::printf("Skipped: %s\n", admission.reason());
      return 77;
    }
    std::fenv_t saved;
    if(std::fegetenv(&saved) != 0) return 1;
    bool ok = true;
    for(int mode : {FE_TONEAREST, FE_DOWNWARD, FE_UPWARD, FE_TOWARDZERO}) {
      if(std::fesetround(mode) != 0 || !exercise(seed)) { ok = false; break; }
    }
    if(std::fesetenv(&saved) != 0) ok = false;
    if(!ok) std::puts("FP16FML operation graph or FP environment check failed");
    return ok ? 0 : 1;
  }
}
