// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/x86/f16c.h>
constexpr native::isa arch{native::x86_feature::f16c};
#define ENTRY extern "C" native_target("f16c,no-avx2,no-avx512fp16") native_noinline
ENTRY std::uint16_t native_f16c_narrow1(float x) { return native::cvtss_sh<arch, 0>(x); }
ENTRY __m128i native_f16c_narrow4(__m128 x) { return native::cvtps_ph<arch, 3>(x); }
ENTRY __m128i native_f16c_narrow8(__m256 x) { return native::cvtps_ph<arch, 4>(x); }
ENTRY float native_f16c_widen1(std::uint16_t x) { return native::cvtsh_ss<arch>(x); }
ENTRY __m128 native_f16c_widen4(__m128i x) { return native::cvtph_ps<arch, 4>(x); }
ENTRY __m256 native_f16c_widen8(__m128i x) { return native::cvtph_ps<arch, 8>(x); }
ENTRY void native_f16c_discard_narrow1(float x) { (void)native::cvtss_sh<arch, 8>(x); }
ENTRY void native_f16c_discard_narrow4(__m128 x) { (void)native::cvtps_ph<arch, 255>(x); }
ENTRY void native_f16c_discard_narrow8(__m256 x) { (void)native::cvtps_ph<arch, 7>(x); }
ENTRY void native_f16c_discard_widen1(std::uint16_t x) { (void)native::cvtsh_ss<arch>(x); }
ENTRY void native_f16c_discard_widen4(__m128i x) { (void)native::cvtph_ps<arch, 4>(x); }
ENTRY void native_f16c_discard_widen8(__m128i x) { (void)native::cvtph_ps<arch, 8>(x); }
#undef ENTRY
