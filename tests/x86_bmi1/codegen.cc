// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/x86/bmi1.h>

#if defined(__BMI__) || defined(__BMI2__) || defined(__AVX__) || defined(__AVX2__) || defined(__FMA__)
#error BMI1 must be supplied by the function target, not translation-unit flags
#endif
constexpr native::isa bmi1{native::x86_feature::bmi1};
#define BMI1_PROBE __attribute__((target("bmi"), noinline))
extern "C" {
  BMI1_PROBE std::uint32_t probe_andn32(std::uint32_t a, std::uint32_t b) { return native::andn<bmi1>(a, b); }
  BMI1_PROBE std::uint64_t probe_andn64(std::uint64_t a, std::uint64_t b) { return native::andn<bmi1>(a, b); }
  BMI1_PROBE std::uint32_t probe_bextr32(std::uint32_t a, std::uint32_t c) { return native::bextr<bmi1>(a, c); }
  BMI1_PROBE std::uint64_t probe_bextr64(std::uint64_t a, std::uint32_t c) { return native::bextr<bmi1>(a, c); }
  BMI1_PROBE std::uint32_t probe_bextr_controls32(std::uint32_t a, unsigned s, unsigned n) { return native::bextr<bmi1>(a, s, n); }
  BMI1_PROBE std::uint64_t probe_bextr_controls64(std::uint64_t a, unsigned s, unsigned n) { return native::bextr<bmi1>(a, s, n); }
  BMI1_PROBE std::uint32_t probe_blsi32(std::uint32_t a) { return native::blsi<bmi1>(a); }
  BMI1_PROBE std::uint64_t probe_blsi64(std::uint64_t a) { return native::blsi<bmi1>(a); }
  BMI1_PROBE std::uint32_t probe_blsmsk32(std::uint32_t a) { return native::blsmsk<bmi1>(a); }
  BMI1_PROBE std::uint64_t probe_blsmsk64(std::uint64_t a) { return native::blsmsk<bmi1>(a); }
  BMI1_PROBE std::uint32_t probe_blsr32(std::uint32_t a) { return native::blsr<bmi1>(a); }
  BMI1_PROBE std::uint64_t probe_blsr64(std::uint64_t a) { return native::blsr<bmi1>(a); }
  BMI1_PROBE std::uint16_t probe_tzcnt16(std::uint16_t a) { return native::tzcnt<bmi1>(a); }
  BMI1_PROBE std::uint32_t probe_tzcnt32(std::uint32_t a) { return native::tzcnt<bmi1>(a); }
  BMI1_PROBE std::uint64_t probe_tzcnt64(std::uint64_t a) { return native::tzcnt<bmi1>(a); }
}
#undef BMI1_PROBE
