#if defined(NATIVE_VNNI_IMPORT)
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <type_traits>
#include <native/attributes.h>
import native.x86.vnni;

// CMake selects one operation per translation unit: target-mismatch diagnostics
// stop code generation after the first error, so a combined source misses APIs.
constexpr auto vex = native::feature_closure(native::isa<native::x86>{native::x86_feature::avxvnni});
constexpr auto evex = native::feature_closure(native::x86_feature::avx512f & native::x86_feature::avx512vnni);
constexpr auto evexvl = native::feature_closure(evex & native::x86_feature::avx512vl);
constexpr auto int8 = native::feature_closure(native::isa<native::x86>{native::x86_feature::avxvnniint8});
constexpr auto int16 = native::feature_closure(native::isa<native::x86>{native::x86_feature::avxvnniint16});



#if NATIVE_VNNI_REJECT == 1
// The surrounding ISA cannot substitute for the independently required bit.
#if NATIVE_VNNI_FAMILY == 1
constexpr auto requirement = native::feature_closure(native::x86_feature::avx512f & native::x86_feature::avx512vl);
#else
constexpr auto requirement = native::feature_closure(native::isa<native::x86>{native::x86_feature::avx2});
#endif
#elif NATIVE_VNNI_REJECT == 2
constexpr auto requirement = native::feature_closure(native::x86_feature::avx512vnni & native::x86_feature::avx512vl);
#elif NATIVE_VNNI_REJECT == 3
constexpr auto requirement = native::feature_closure(evex);
#elif NATIVE_VNNI_REJECT == 10
constexpr auto requirement = native::feature_closure(vex);
#elif NATIVE_VNNI_REJECT == 11
constexpr auto requirement = native::feature_closure(vex & native::x86_feature::avx512f & native::x86_feature::avx512vl);
#elif NATIVE_VNNI_REJECT == 12
#if NATIVE_VNNI_FAMILY == 2
constexpr auto requirement = native::feature_closure(int16);
#else
constexpr auto requirement = native::feature_closure(int8);
#endif
#elif NATIVE_VNNI_FAMILY == 0
constexpr auto requirement = native::feature_closure(vex);
#elif NATIVE_VNNI_FAMILY == 1
constexpr auto requirement = native::feature_closure(NATIVE_VNNI_WIDTH == 512 ? evex : evexvl);
#elif NATIVE_VNNI_FAMILY == 2
constexpr auto requirement = native::feature_closure(int8);
#elif NATIVE_VNNI_FAMILY == 3
constexpr auto requirement = native::feature_closure(int16);
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
#define NATIVE_VNNI_CALL(acc, k, a, b) native::NATIVE_VNNI_FUNCTION<requirement>(acc, k, a, b)
#elif NATIVE_VNNI_VARIANT == 2
#define NATIVE_VNNI_FUNCTION NATIVE_VNNI_JOIN(maskz_, NATIVE_VNNI_OPERATION)
#define NATIVE_VNNI_CALL(acc, k, a, b) native::NATIVE_VNNI_FUNCTION<requirement>(k, acc, a, b)
#else
#define NATIVE_VNNI_FUNCTION NATIVE_VNNI_OPERATION
#define NATIVE_VNNI_CALL(acc, k, a, b) native::NATIVE_VNNI_FUNCTION<requirement>(acc, a, b)
#endif

#define NATIVE_VNNI_NAME_(x) #x
#define NATIVE_VNNI_NAME(x) NATIVE_VNNI_NAME_(x)
constexpr char operation_name[]=NATIVE_VNNI_NAME(NATIVE_VNNI_OPERATION);
constexpr bool word=operation_name[2]=='w';
constexpr bool unsigned_a=operation_name[3]=='u', unsigned_b=operation_name[4]=='u';
using scalar_a=std::conditional_t<word,
  std::conditional_t<unsigned_a,std::uint16_t,std::int16_t>,
  std::conditional_t<unsigned_a,std::uint8_t,std::int8_t>>;
using scalar_b=std::conditional_t<word,
  std::conditional_t<unsigned_b,std::uint16_t,std::int16_t>,
  std::conditional_t<unsigned_b,std::uint8_t,std::int8_t>>;
using accumulator=native::simd<std::conditional_t<unsigned_a && unsigned_b,std::uint32_t,std::int32_t>,NATIVE_VNNI_WIDTH/32,requirement>;
using a_vector=native::simd<scalar_a,NATIVE_VNNI_WIDTH/(8*sizeof(scalar_a)),requirement>;
using b_vector=native::simd<scalar_b,NATIVE_VNNI_WIDTH/(8*sizeof(scalar_b)),requirement>;
using mask=native::predicate<NATIVE_VNNI_WIDTH/32,requirement>;
#if NATIVE_VNNI_REJECT == 8
using a_type=native::simd<float,NATIVE_VNNI_WIDTH/32,requirement>;
#else
using a_type=a_vector;
#endif
native_noinline native_target(NATIVE_VNNI_CALLER)
auto rejected(accumulator acc, mask k, a_type a, b_vector b) noexcept {
  return NATIVE_VNNI_CALL(acc, k, a, b);
}

#else
#include "negative_raw.cc"
#endif
