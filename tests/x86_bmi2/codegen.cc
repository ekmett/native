// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Compile at -O2 -S without a global ISA flag to inspect per-function lowering.
#include <native/x86/bmi2.h>

#if defined(__BMI__) || defined(__BMI2__) || defined(__AVX__) || defined(__AVX2__) || defined(__FMA__)
#error BMI2 must be supplied by the function target, not translation-unit flags
#endif

constexpr native::isa<native::x86> bmi2_codegen_arch{native::x86_feature::bmi2};
#define BMI2_CODEGEN __attribute__((target("bmi2"), noinline))

extern "C" {
  BMI2_CODEGEN std::uint32_t native_bzhi32(std::uint32_t x, unsigned index) {
    return native::bzhi<bmi2_codegen_arch>(x, index);
  }
  BMI2_CODEGEN std::uint64_t native_bzhi64(std::uint64_t x, unsigned index) {
    return native::bzhi<bmi2_codegen_arch>(x, index);
  }
  BMI2_CODEGEN std::uint32_t native_mulx32(std::uint32_t x, std::uint32_t y, std::uint32_t* high) {
    return native::mulx<bmi2_codegen_arch>(x, y, high);
  }
  BMI2_CODEGEN std::uint64_t native_mulx64(std::uint64_t x, std::uint64_t y, std::uint64_t* high) {
    return native::mulx<bmi2_codegen_arch>(x, y, high);
  }
  BMI2_CODEGEN std::uint32_t native_shlx32(std::uint32_t x, unsigned count) {
    return native::shlx<bmi2_codegen_arch>(x, count);
  }
  BMI2_CODEGEN std::uint64_t native_shlx64(std::uint64_t x, unsigned count) {
    return native::shlx<bmi2_codegen_arch>(x, count);
  }
  BMI2_CODEGEN std::uint32_t native_shrx32(std::uint32_t x, unsigned count) {
    return native::shrx<bmi2_codegen_arch>(x, count);
  }
  BMI2_CODEGEN std::uint64_t native_shrx64(std::uint64_t x, unsigned count) {
    return native::shrx<bmi2_codegen_arch>(x, count);
  }
  BMI2_CODEGEN std::int32_t native_sarx32(std::int32_t x, unsigned count) {
    return native::sarx<bmi2_codegen_arch>(x, count);
  }
  BMI2_CODEGEN std::int64_t native_sarx64(std::int64_t x, unsigned count) {
    return native::sarx<bmi2_codegen_arch>(x, count);
  }
  BMI2_CODEGEN std::uint32_t native_rorx32(std::uint32_t x) {
    return native::rorx<bmi2_codegen_arch, 7>(x);
  }
  BMI2_CODEGEN std::uint64_t native_rorx64(std::uint64_t x) {
    return native::rorx<bmi2_codegen_arch, 7>(x);
  }
}
