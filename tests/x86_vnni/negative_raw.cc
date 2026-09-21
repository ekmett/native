// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/x86/vnni.h>


// CMake selects one operation per translation unit: target-mismatch diagnostics
// stop code generation after the first error, so a combined source misses APIs.
constexpr native::isa vex{native::x86_feature::avxvnni};
constexpr auto evex = native::x86_feature::avx512f & native::x86_feature::avx512vnni;
constexpr auto evexvl = evex & native::x86_feature::avx512vl;
constexpr native::isa int8{native::x86_feature::avxvnniint8};
constexpr native::isa int16{native::x86_feature::avxvnniint16};

#if NATIVE_VNNI_WIDTH == 128
using reg = __m128i;
using float_reg = __m128;
using mask = __mmask8;
#elif NATIVE_VNNI_WIDTH == 256
using reg = __m256i;
using float_reg = __m256;
using mask = __mmask8;
#elif NATIVE_VNNI_WIDTH == 512
using reg = __m512i;
using float_reg = __m512;
using mask = __mmask16;
#else
#error Select a VNNI register width
#endif

#if NATIVE_VNNI_REJECT == 1
// The surrounding ISA cannot substitute for the independently required bit.
#if NATIVE_VNNI_FAMILY == 1
constexpr auto requirement = native::x86_feature::avx512f & native::x86_feature::avx512vl;
#else
constexpr native::isa requirement{native::x86_feature::avx2};
#endif
#elif NATIVE_VNNI_REJECT == 2
constexpr auto requirement = native::x86_feature::avx512vnni & native::x86_feature::avx512vl;
#elif NATIVE_VNNI_REJECT == 3
constexpr auto requirement = evex;
#elif NATIVE_VNNI_REJECT == 10
constexpr auto requirement = vex;
#elif NATIVE_VNNI_REJECT == 11
constexpr auto requirement = vex & native::x86_feature::avx512f & native::x86_feature::avx512vl;
#elif NATIVE_VNNI_REJECT == 12
#if NATIVE_VNNI_FAMILY == 2
constexpr auto requirement = int16;
#else
constexpr auto requirement = int8;
#endif
#elif NATIVE_VNNI_FAMILY == 0
constexpr auto requirement = vex;
#elif NATIVE_VNNI_FAMILY == 1
constexpr auto requirement = NATIVE_VNNI_WIDTH == 512 ? evex : evexvl;
#elif NATIVE_VNNI_FAMILY == 2
constexpr auto requirement = int8;
#elif NATIVE_VNNI_FAMILY == 3
constexpr auto requirement = int16;
#else
#error Select a VNNI instruction family
#endif

#if NATIVE_VNNI_REJECT == 4
#define NATIVE_VNNI_CALLER "avx2,no-avxvnni,no-avx512vnni,no-avxvnniint8,no-avxvnniint16"
#elif NATIVE_VNNI_REJECT == 5
#define NATIVE_VNNI_CALLER "avx512vnni,no-avx512f,no-avxvnni"
#elif NATIVE_VNNI_REJECT == 6
#define NATIVE_VNNI_CALLER "avx512f,avx512vnni,no-avx512vl,no-avxvnni"
#elif NATIVE_VNNI_REJECT == 13
#define NATIVE_VNNI_CALLER "avx512f,avxvnniint8,avxvnniint16"
#elif NATIVE_VNNI_FAMILY == 0
#define NATIVE_VNNI_CALLER "avxvnni,no-avx512f"
#elif NATIVE_VNNI_FAMILY == 1
#define NATIVE_VNNI_CALLER "avx512f,avx512vnni,avx512vl,no-avxvnni"
#elif NATIVE_VNNI_FAMILY == 2
#define NATIVE_VNNI_CALLER "avxvnniint8,no-avx512f"
#elif NATIVE_VNNI_FAMILY == 3
#define NATIVE_VNNI_CALLER "avxvnniint16,no-avx512f"
#endif

#define NATIVE_VNNI_JOIN_(a, b) a##b
#define NATIVE_VNNI_JOIN(a, b) NATIVE_VNNI_JOIN_(a, b)
#if NATIVE_VNNI_VARIANT == 1
#define NATIVE_VNNI_FUNCTION NATIVE_VNNI_JOIN(mask_, NATIVE_VNNI_OPERATION)
#define NATIVE_VNNI_CALL(acc, k, a, b) native::detail::x86_vnni::NATIVE_VNNI_FUNCTION<requirement>(acc, k, a, b)
#elif NATIVE_VNNI_VARIANT == 2
#define NATIVE_VNNI_FUNCTION NATIVE_VNNI_JOIN(maskz_, NATIVE_VNNI_OPERATION)
#define NATIVE_VNNI_CALL(acc, k, a, b) native::detail::x86_vnni::NATIVE_VNNI_FUNCTION<requirement>(k, acc, a, b)
#else
#define NATIVE_VNNI_FUNCTION NATIVE_VNNI_OPERATION
#define NATIVE_VNNI_CALL(acc, k, a, b) native::detail::x86_vnni::NATIVE_VNNI_FUNCTION<requirement>(acc, a, b)
#endif

#if NATIVE_VNNI_REJECT == 7
using acc_type = float_reg;
using a_type = float_reg;
using b_type = float_reg;
#elif NATIVE_VNNI_REJECT == 8
using acc_type = reg;
using a_type = float_reg;
using b_type = reg;
#elif NATIVE_VNNI_REJECT == 9
using acc_type = reg;
using a_type = __m128i;
using b_type = reg;
#elif NATIVE_VNNI_REJECT == 14
using acc_type = float_reg;
using a_type = reg;
using b_type = reg;
#elif NATIVE_VNNI_REJECT == 15
using acc_type = reg;
using a_type = reg;
using b_type = float_reg;
#else
using acc_type = reg;
using a_type = reg;
using b_type = reg;
#endif

native_noinline native_target(NATIVE_VNNI_CALLER)
reg rejected(acc_type acc, mask k, a_type a, b_type b) noexcept {
  return NATIVE_VNNI_CALL(acc, k, a, b);
}
