#if defined(NATIVE_F16C_IMPORT)
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.x86.f16c;

constexpr auto arch = native::feature_closure(native::isa{native::x86_feature::f16c});
#if NATIVE_F16C_REJECT == 1
native_target("f16c") auto rejected(float x) { return native::cvtss_sh<native::scalar, 0>(x); }
#elif NATIVE_F16C_REJECT == 2
native_target("f16c") auto rejected(native::simd<float,4,native::feature_closure(native::isa(native::x86_feature::avx))> x) { return native::cvtps_ph<native::feature_closure(native::isa(native::x86_feature::avx)), 0>(x); }
#elif NATIVE_F16C_REJECT == 3
native_target("f16c") auto rejected(native::simd<float,8,native::feature_closure(native::isa(native::x86_feature::avx))> x) { return native::cvtps_ph<native::feature_closure(native::isa(native::x86_feature::avx)), 0>(x); }
#elif NATIVE_F16C_REJECT == 4
native_target("f16c") auto rejected(native::simd<native::fp16,4,native::feature_closure(native::isa(native::x86_feature::avx))> x) { return native::cvtph_ps<native::feature_closure(native::isa(native::x86_feature::avx)), 4>(x); }
#elif NATIVE_F16C_REJECT == 5
native_target("f16c") auto rejected(native::simd<native::fp16,8,native::feature_closure(native::isa(native::x86_feature::avx))> x) { return native::cvtph_ps<native::feature_closure(native::isa(native::x86_feature::avx)), 8>(x); }
#elif NATIVE_F16C_REJECT == 6
native_target("avx") auto rejected(float x) { return native::cvtss_sh<arch, 0>(x); }
#elif NATIVE_F16C_REJECT == 7
native_target("avx") auto rejected(native::simd<float,4,arch> x) { return native::cvtps_ph<arch, 0>(x); }
#elif NATIVE_F16C_REJECT == 8
native_target("avx") auto rejected(native::simd<float,8,arch> x) { return native::cvtps_ph<arch, 0>(x); }
#elif NATIVE_F16C_REJECT == 9
native_target("avx") auto rejected(std::uint16_t x) { return native::cvtsh_ss<arch>(x); }
#elif NATIVE_F16C_REJECT == 10
native_target("avx") auto rejected(native::simd<native::fp16,4,arch> x) { return native::cvtph_ps<arch, 4>(x); }
#elif NATIVE_F16C_REJECT == 11
native_target("avx") auto rejected(native::simd<native::fp16,8,arch> x) { return native::cvtph_ps<arch, 8>(x); }
#elif NATIVE_F16C_REJECT == 12
native_target("f16c") auto rejected(float x) { return native::cvtss_sh<arch, 256>(x); }
#elif NATIVE_F16C_REJECT == 13
native_target("f16c") auto rejected(native::simd<float,4,arch> x) { return native::cvtps_ph<arch, unsigned(-1)>(x); }
#elif NATIVE_F16C_REJECT == 14
native_target("f16c") auto rejected(float x, unsigned imm) { return native::cvtss_sh<arch, imm>(x); }
#elif NATIVE_F16C_REJECT == 15
native_target("f16c") auto rejected(native::simd<native::fp16,4,arch> x) { return native::cvtph_ps<arch, 2>(x); }
#elif NATIVE_F16C_REJECT == 16
native_target("f16c") auto rejected(native::simd<native::fp16,4,arch> x) { return native::cvtph_ps<arch>(x); }
#elif NATIVE_F16C_REJECT == 17
native_target("f16c") auto rejected(native::simd<std::uint16_t,8,arch> x) { return native::cvtps_ph<arch, 0>(x); }
#elif NATIVE_F16C_REJECT == 18
native_target("f16c") auto rejected(native::simd<double,2,arch> x) { return native::cvtps_ph<arch, 0>(x); }
#elif NATIVE_F16C_REJECT == 19
native_target("f16c") auto rejected(native::simd<double,4,arch> x) { return native::cvtps_ph<arch, 0>(x); }
#elif NATIVE_F16C_REJECT == 20
native_target("f16c") auto rejected(native::simd<float,4,arch> x) { return native::cvtph_ps<arch, 4>(x); }
#elif NATIVE_F16C_REJECT == 21
native_target("f16c") auto rejected(native::simd<std::uint16_t,16,arch> x) { return native::cvtph_ps<arch, 8>(x); }
#elif NATIVE_F16C_REJECT == 22
native_target("f16c") auto rejected(native::simd<float,16,native::feature_closure(arch & native::x86_feature::avx512f)> x) { return native::cvtps_ph<arch, 0>(x); }
#elif NATIVE_F16C_REJECT == 23
native_target("f16c") auto rejected(std::uint16_t x) { return native::cvtsh_ss<native::scalar>(x); }
#else
#error Select a F16C negative control
#endif

#else
#include "negative_raw.cc"
#endif
