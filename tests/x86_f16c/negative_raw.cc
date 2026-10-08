// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/x86/f16c.h>

constexpr native::isa<native::x86> arch{native::x86_feature::f16c};
#if NATIVE_F16C_REJECT == 1
hint_target("f16c") auto rejected(float x) { return native::detail::x86_f16c::cvtss_sh<native::isa<native::x86>{}, 0>(x); }
#elif NATIVE_F16C_REJECT == 2
hint_target("f16c") auto rejected(__m128 x) { return native::detail::x86_f16c::cvtps_ph<native::isa<native::x86>(native::x86_feature::avx), 0>(x); }
#elif NATIVE_F16C_REJECT == 3
hint_target("f16c") auto rejected(__m256 x) { return native::detail::x86_f16c::cvtps_ph<native::isa<native::x86>{}, 0>(x); }
#elif NATIVE_F16C_REJECT == 4
hint_target("f16c") auto rejected(__m128i x) { return native::detail::x86_f16c::cvtph_ps<native::isa<native::x86>{}, 4>(x); }
#elif NATIVE_F16C_REJECT == 5
hint_target("f16c") auto rejected(__m128i x) { return native::detail::x86_f16c::cvtph_ps<native::isa<native::x86>{}, 8>(x); }
#elif NATIVE_F16C_REJECT == 6
hint_target("avx") auto rejected(float x) { return native::detail::x86_f16c::cvtss_sh<arch, 0>(x); }
#elif NATIVE_F16C_REJECT == 7
hint_target("avx") auto rejected(__m128 x) { return native::detail::x86_f16c::cvtps_ph<arch, 0>(x); }
#elif NATIVE_F16C_REJECT == 8
hint_target("avx") auto rejected(__m256 x) { return native::detail::x86_f16c::cvtps_ph<arch, 0>(x); }
#elif NATIVE_F16C_REJECT == 9
hint_target("avx") auto rejected(std::uint16_t x) { return native::detail::x86_f16c::cvtsh_ss<arch>(x); }
#elif NATIVE_F16C_REJECT == 10
hint_target("avx") auto rejected(__m128i x) { return native::detail::x86_f16c::cvtph_ps<arch, 4>(x); }
#elif NATIVE_F16C_REJECT == 11
hint_target("avx") auto rejected(__m128i x) { return native::detail::x86_f16c::cvtph_ps<arch, 8>(x); }
#elif NATIVE_F16C_REJECT == 12
hint_target("f16c") auto rejected(float x) { return native::detail::x86_f16c::cvtss_sh<arch, 256>(x); }
#elif NATIVE_F16C_REJECT == 13
hint_target("f16c") auto rejected(__m128 x) { return native::detail::x86_f16c::cvtps_ph<arch, unsigned(-1)>(x); }
#elif NATIVE_F16C_REJECT == 14
hint_target("f16c") auto rejected(float x, unsigned imm) { return native::detail::x86_f16c::cvtss_sh<arch, imm>(x); }
#elif NATIVE_F16C_REJECT == 15
hint_target("f16c") auto rejected(__m128i x) { return native::detail::x86_f16c::cvtph_ps<arch, 2>(x); }
#elif NATIVE_F16C_REJECT == 16
hint_target("f16c") auto rejected(__m128i x) { return native::detail::x86_f16c::cvtph_ps<arch>(x); }
#elif NATIVE_F16C_REJECT == 17
hint_target("f16c") auto rejected(__m128i x) { return native::detail::x86_f16c::cvtps_ph<arch, 0>(x); }
#elif NATIVE_F16C_REJECT == 18
hint_target("f16c") auto rejected(__m128d x) { return native::detail::x86_f16c::cvtps_ph<arch, 0>(x); }
#elif NATIVE_F16C_REJECT == 19
hint_target("f16c") auto rejected(__m256d x) { return native::detail::x86_f16c::cvtps_ph<arch, 0>(x); }
#elif NATIVE_F16C_REJECT == 20
hint_target("f16c") auto rejected(__m128 x) { return native::detail::x86_f16c::cvtph_ps<arch, 4>(x); }
#elif NATIVE_F16C_REJECT == 21
hint_target("f16c") auto rejected(__m256i x) { return native::detail::x86_f16c::cvtph_ps<arch, 8>(x); }
#elif NATIVE_F16C_REJECT == 22
hint_target("f16c") auto rejected(__m512 x) { return native::detail::x86_f16c::cvtps_ph<arch, 0>(x); }
#elif NATIVE_F16C_REJECT == 23
hint_target("f16c") auto rejected(std::uint16_t x) { return native::detail::x86_f16c::cvtsh_ss<native::isa<native::x86>{}>(x); }
#else
#error Select a F16C negative control
#endif
