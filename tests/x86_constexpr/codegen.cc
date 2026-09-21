// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <bit>
#include <cstdint>
#include <immintrin.h>
#include <native/attributes.h>
#if !NATIVE_CONSTEXPR_RAW
import native.x86.bmi1;
import native.x86.bmi2;
import native.x86.popcnt;
import native.x86.lzcnt;
import native.x86.crc32c;
#endif

#if NATIVE_CONSTEXPR_RAW
native_inline native_target("bmi2")
std::uint32_t multiply_raw(std::uint32_t a, std::uint32_t b, std::uint32_t* high) {
  unsigned int upper;
  auto low = _mulx_u32(a, b, &upper);
  *high = upper;
  return low;
}
native_inline native_target("bmi2")
std::uint64_t multiply_raw(std::uint64_t a, std::uint64_t b, std::uint64_t* high) {
  unsigned long long upper;
  auto low = _mulx_u64(a, b, &upper);
  *high = upper;
  return low;
}
#define NATIVE_CONSTEXPR_RESULT(raw, api) raw
#else
#define NATIVE_CONSTEXPR_RESULT(raw, api) api
#endif
#define NATIVE_CONSTEXPR_PAIR(name, feature, type, parameters, raw, api) \
  extern "C" native_noinline __attribute__((used)) native_target(feature) \
  type name parameters noexcept { return NATIVE_CONSTEXPR_RESULT(raw, api); }

NATIVE_CONSTEXPR_PAIR(native_constexpr_0, "bmi", std::uint32_t,
  (std::uint32_t a, std::uint32_t b),
  (_andn_u32(a, b)),
  (native::andn<native::x86_feature::bmi1>(a, b)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_1, "bmi", std::uint32_t,
  (std::uint32_t a, std::uint32_t control),
  (_bextr2_u32(a, control)),
  (native::bextr<native::x86_feature::bmi1>(a, control)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_2, "bmi", std::uint32_t,
  (std::uint32_t a, unsigned start, unsigned length),
  (_bextr_u32(a, start, length)),
  (native::bextr<native::x86_feature::bmi1>(a, start, length)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_3, "bmi", std::uint32_t,
  (std::uint32_t a),
  (_blsi_u32(a)),
  (native::blsi<native::x86_feature::bmi1>(a)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_4, "bmi", std::uint32_t,
  (std::uint32_t a),
  (_blsmsk_u32(a)),
  (native::blsmsk<native::x86_feature::bmi1>(a)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_5, "bmi", std::uint32_t,
  (std::uint32_t a),
  (_blsr_u32(a)),
  (native::blsr<native::x86_feature::bmi1>(a)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_6, "bmi2", std::uint32_t,
  (std::uint32_t a, unsigned count),
  (_bzhi_u32(a, count)),
  (native::bzhi<native::x86_feature::bmi2>(a, count)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_7, "bmi2", std::uint32_t,
  (std::uint32_t a, std::uint32_t b, std::uint32_t* high),
  (multiply_raw(a, b, high)),
  (native::mulx<native::x86_feature::bmi2>(a, b, high)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_8, "bmi2", std::uint32_t,
  (std::uint32_t a, std::uint32_t mask),
  (_pdep_u32(a, mask)),
  (native::pdep<native::x86_feature::bmi2>(a, mask)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_9, "bmi2", std::uint32_t,
  (std::uint32_t a, std::uint32_t mask),
  (_pext_u32(a, mask)),
  (native::pext<native::x86_feature::bmi2>(a, mask)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_10, "bmi2", std::uint32_t,
  (std::uint32_t a, unsigned count),
  (a << (count & 31u)),
  (native::shlx<native::x86_feature::bmi2>(a, count)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_11, "bmi2", std::uint32_t,
  (std::uint32_t a, unsigned count),
  (a >> (count & 31u)),
  (native::shrx<native::x86_feature::bmi2>(a, count)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_12, "bmi2", std::uint32_t,
  (std::uint32_t a),
  (std::rotr(a, 7)),
  (native::rorx<native::x86_feature::bmi2, 7>(a)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_13, "bmi", std::uint64_t,
  (std::uint64_t a, std::uint64_t b),
  (_andn_u64(a, b)),
  (native::andn<native::x86_feature::bmi1>(a, b)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_14, "bmi", std::uint64_t,
  (std::uint64_t a, std::uint32_t control),
  (_bextr2_u64(a, control)),
  (native::bextr<native::x86_feature::bmi1>(a, control)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_15, "bmi", std::uint64_t,
  (std::uint64_t a, unsigned start, unsigned length),
  (_bextr_u64(a, start, length)),
  (native::bextr<native::x86_feature::bmi1>(a, start, length)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_16, "bmi", std::uint64_t,
  (std::uint64_t a),
  (_blsi_u64(a)),
  (native::blsi<native::x86_feature::bmi1>(a)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_17, "bmi", std::uint64_t,
  (std::uint64_t a),
  (_blsmsk_u64(a)),
  (native::blsmsk<native::x86_feature::bmi1>(a)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_18, "bmi", std::uint64_t,
  (std::uint64_t a),
  (_blsr_u64(a)),
  (native::blsr<native::x86_feature::bmi1>(a)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_19, "bmi2", std::uint64_t,
  (std::uint64_t a, unsigned count),
  (_bzhi_u64(a, count)),
  (native::bzhi<native::x86_feature::bmi2>(a, count)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_20, "bmi2", std::uint64_t,
  (std::uint64_t a, std::uint64_t b, std::uint64_t* high),
  (multiply_raw(a, b, high)),
  (native::mulx<native::x86_feature::bmi2>(a, b, high)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_21, "bmi2", std::uint64_t,
  (std::uint64_t a, std::uint64_t mask),
  (_pdep_u64(a, mask)),
  (native::pdep<native::x86_feature::bmi2>(a, mask)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_22, "bmi2", std::uint64_t,
  (std::uint64_t a, std::uint64_t mask),
  (_pext_u64(a, mask)),
  (native::pext<native::x86_feature::bmi2>(a, mask)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_23, "bmi2", std::uint64_t,
  (std::uint64_t a, unsigned count),
  (a << (count & 63u)),
  (native::shlx<native::x86_feature::bmi2>(a, count)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_24, "bmi2", std::uint64_t,
  (std::uint64_t a, unsigned count),
  (a >> (count & 63u)),
  (native::shrx<native::x86_feature::bmi2>(a, count)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_25, "bmi2", std::uint64_t,
  (std::uint64_t a),
  (std::rotr(a, 7)),
  (native::rorx<native::x86_feature::bmi2, 7>(a)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_26, "bmi2", std::int32_t,
  (std::int32_t a, unsigned count),
  (a >> (count & 31u)),
  (native::sarx<native::x86_feature::bmi2>(a, count)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_27, "bmi2", std::int64_t,
  (std::int64_t a, unsigned count),
  (a >> (count & 63u)),
  (native::sarx<native::x86_feature::bmi2>(a, count)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_28, "bmi", std::uint16_t,
  (std::uint16_t a),
  (_tzcnt_u16(a)),
  (native::tzcnt<native::x86_feature::bmi1>(a)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_29, "bmi", std::uint32_t,
  (std::uint32_t a),
  (_tzcnt_u32(a)),
  (native::tzcnt<native::x86_feature::bmi1>(a)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_30, "bmi", std::uint64_t,
  (std::uint64_t a),
  (_tzcnt_u64(a)),
  (native::tzcnt<native::x86_feature::bmi1>(a)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_31, "popcnt", std::uint16_t,
  (std::uint16_t a),
  (_mm_popcnt_u32(a)),
  (native::popcnt<native::x86_feature::popcnt>(a)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_32, "popcnt", std::uint32_t,
  (std::uint32_t a),
  (_mm_popcnt_u32(a)),
  (native::popcnt<native::x86_feature::popcnt>(a)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_33, "popcnt", std::uint64_t,
  (std::uint64_t a),
  (_mm_popcnt_u64(a)),
  (native::popcnt<native::x86_feature::popcnt>(a)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_34, "lzcnt", std::uint16_t,
  (std::uint16_t a),
  (__builtin_ia32_lzcnt_u16(a)),
  (native::lzcnt<native::x86_feature::lzcnt>(a)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_35, "lzcnt", std::uint32_t,
  (std::uint32_t a),
  (_lzcnt_u32(a)),
  (native::lzcnt<native::x86_feature::lzcnt>(a)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_36, "lzcnt", std::uint64_t,
  (std::uint64_t a),
  (_lzcnt_u64(a)),
  (native::lzcnt<native::x86_feature::lzcnt>(a)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_37, "crc32", std::uint32_t,
  (std::uint32_t a, std::uint8_t b),
  (_mm_crc32_u8(a, b)),
  (native::crc32c<native::x86_feature::crc32>(a, b)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_38, "crc32", std::uint32_t,
  (std::uint32_t a, std::uint16_t b),
  (_mm_crc32_u16(a, b)),
  (native::crc32c<native::x86_feature::crc32>(a, b)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_39, "crc32", std::uint32_t,
  (std::uint32_t a, std::uint32_t b),
  (_mm_crc32_u32(a, b)),
  (native::crc32c<native::x86_feature::crc32>(a, b)))
NATIVE_CONSTEXPR_PAIR(native_constexpr_40, "crc32", std::uint32_t,
  (std::uint32_t a, std::uint64_t b),
  (_mm_crc32_u64(a, b)),
  (native::crc32c<native::x86_feature::crc32>(a, b)))

#undef NATIVE_CONSTEXPR_PAIR
#undef NATIVE_CONSTEXPR_RESULT
