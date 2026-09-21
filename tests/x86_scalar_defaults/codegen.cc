// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

#include <cstdint>
#include <native/attributes.h>
#include <native/targets.h>
#include "strong_imports.h"

using native::x86_feature;
extern "C" native_noinline __attribute__((used)) native_target("bmi")
std::uint32_t native_scalar_default_0(std::uint32_t a, std::uint32_t b) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::andn<native::x86_feature::bmi1>(a, b);
#else
  return native::andn(a, b);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi")
std::uint32_t native_scalar_default_1(std::uint32_t a, std::uint32_t control) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::bextr<native::x86_feature::bmi1>(a, control);
#else
  return native::bextr(a, control);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi")
std::uint32_t native_scalar_default_2(std::uint32_t a, unsigned start, unsigned length) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::bextr<native::x86_feature::bmi1>(a, start, length);
#else
  return native::bextr(a, start, length);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi")
std::uint32_t native_scalar_default_3(std::uint32_t a) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::blsi<native::x86_feature::bmi1>(a);
#else
  return native::blsi(a);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi")
std::uint32_t native_scalar_default_4(std::uint32_t a) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::blsmsk<native::x86_feature::bmi1>(a);
#else
  return native::blsmsk(a);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi")
std::uint32_t native_scalar_default_5(std::uint32_t a) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::blsr<native::x86_feature::bmi1>(a);
#else
  return native::blsr(a);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi2")
std::uint32_t native_scalar_default_6(std::uint32_t a, unsigned count) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::bzhi<native::x86_feature::bmi2>(a, count);
#else
  return native::bzhi(a, count);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi2")
std::uint32_t native_scalar_default_7(std::uint32_t a, std::uint32_t b, std::uint32_t* high) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::mulx<native::x86_feature::bmi2>(a, b, high);
#else
  return native::mulx(a, b, high);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi2")
std::uint32_t native_scalar_default_8(std::uint32_t a, std::uint32_t mask) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::pdep<native::x86_feature::bmi2>(a, mask);
#else
  return native::pdep(a, mask);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi2")
std::uint32_t native_scalar_default_9(std::uint32_t a, std::uint32_t mask) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::pext<native::x86_feature::bmi2>(a, mask);
#else
  return native::pext(a, mask);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi2")
std::uint32_t native_scalar_default_10(std::uint32_t a, unsigned count) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::shlx<native::x86_feature::bmi2>(a, count);
#else
  return native::shlx(a, count);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi2")
std::uint32_t native_scalar_default_11(std::uint32_t a, unsigned count) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::shrx<native::x86_feature::bmi2>(a, count);
#else
  return native::shrx(a, count);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi2")
std::uint32_t native_scalar_default_12(std::uint32_t a) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::rorx<native::x86_feature::bmi2, 7>(a);
#else
  return native::rorx<7>(a);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi")
std::uint64_t native_scalar_default_13(std::uint64_t a, std::uint64_t b) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::andn<native::x86_feature::bmi1>(a, b);
#else
  return native::andn(a, b);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi")
std::uint64_t native_scalar_default_14(std::uint64_t a, std::uint32_t control) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::bextr<native::x86_feature::bmi1>(a, control);
#else
  return native::bextr(a, control);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi")
std::uint64_t native_scalar_default_15(std::uint64_t a, unsigned start, unsigned length) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::bextr<native::x86_feature::bmi1>(a, start, length);
#else
  return native::bextr(a, start, length);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi")
std::uint64_t native_scalar_default_16(std::uint64_t a) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::blsi<native::x86_feature::bmi1>(a);
#else
  return native::blsi(a);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi")
std::uint64_t native_scalar_default_17(std::uint64_t a) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::blsmsk<native::x86_feature::bmi1>(a);
#else
  return native::blsmsk(a);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi")
std::uint64_t native_scalar_default_18(std::uint64_t a) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::blsr<native::x86_feature::bmi1>(a);
#else
  return native::blsr(a);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi2")
std::uint64_t native_scalar_default_19(std::uint64_t a, unsigned count) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::bzhi<native::x86_feature::bmi2>(a, count);
#else
  return native::bzhi(a, count);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi2")
std::uint64_t native_scalar_default_20(std::uint64_t a, std::uint64_t b, std::uint64_t* high) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::mulx<native::x86_feature::bmi2>(a, b, high);
#else
  return native::mulx(a, b, high);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi2")
std::uint64_t native_scalar_default_21(std::uint64_t a, std::uint64_t mask) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::pdep<native::x86_feature::bmi2>(a, mask);
#else
  return native::pdep(a, mask);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi2")
std::uint64_t native_scalar_default_22(std::uint64_t a, std::uint64_t mask) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::pext<native::x86_feature::bmi2>(a, mask);
#else
  return native::pext(a, mask);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi2")
std::uint64_t native_scalar_default_23(std::uint64_t a, unsigned count) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::shlx<native::x86_feature::bmi2>(a, count);
#else
  return native::shlx(a, count);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi2")
std::uint64_t native_scalar_default_24(std::uint64_t a, unsigned count) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::shrx<native::x86_feature::bmi2>(a, count);
#else
  return native::shrx(a, count);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi2")
std::uint64_t native_scalar_default_25(std::uint64_t a) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::rorx<native::x86_feature::bmi2, 7>(a);
#else
  return native::rorx<7>(a);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi2")
std::int32_t native_scalar_default_26(std::int32_t a, unsigned count) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::sarx<native::x86_feature::bmi2>(a, count);
#else
  return native::sarx(a, count);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi2")
std::int64_t native_scalar_default_27(std::int64_t a, unsigned count) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::sarx<native::x86_feature::bmi2>(a, count);
#else
  return native::sarx(a, count);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi")
std::uint16_t native_scalar_default_28(std::uint16_t a) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::tzcnt<native::x86_feature::bmi1>(a);
#else
  return native::tzcnt(a);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi")
std::uint32_t native_scalar_default_29(std::uint32_t a) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::tzcnt<native::x86_feature::bmi1>(a);
#else
  return native::tzcnt(a);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("bmi")
std::uint64_t native_scalar_default_30(std::uint64_t a) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::tzcnt<native::x86_feature::bmi1>(a);
#else
  return native::tzcnt(a);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("popcnt")
std::uint16_t native_scalar_default_31(std::uint16_t a) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::popcnt<native::x86_feature::popcnt>(a);
#else
  return native::popcnt(a);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("popcnt")
std::uint32_t native_scalar_default_32(std::uint32_t a) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::popcnt<native::x86_feature::popcnt>(a);
#else
  return native::popcnt(a);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("popcnt")
std::uint64_t native_scalar_default_33(std::uint64_t a) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::popcnt<native::x86_feature::popcnt>(a);
#else
  return native::popcnt(a);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("lzcnt")
std::uint16_t native_scalar_default_34(std::uint16_t a) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::lzcnt<native::x86_feature::lzcnt>(a);
#else
  return native::lzcnt(a);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("lzcnt")
std::uint32_t native_scalar_default_35(std::uint32_t a) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::lzcnt<native::x86_feature::lzcnt>(a);
#else
  return native::lzcnt(a);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("lzcnt")
std::uint64_t native_scalar_default_36(std::uint64_t a) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::lzcnt<native::x86_feature::lzcnt>(a);
#else
  return native::lzcnt(a);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("crc32")
std::uint32_t native_scalar_default_37(std::uint32_t a, std::uint8_t b) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::crc32c<native::x86_feature::crc32>(a, b);
#else
  return native::crc32c(a, b);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("crc32")
std::uint32_t native_scalar_default_38(std::uint32_t a, std::uint16_t b) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::crc32c<native::x86_feature::crc32>(a, b);
#else
  return native::crc32c(a, b);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("crc32")
std::uint32_t native_scalar_default_39(std::uint32_t a, std::uint32_t b) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::crc32c<native::x86_feature::crc32>(a, b);
#else
  return native::crc32c(a, b);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("crc32")
std::uint32_t native_scalar_default_40(std::uint32_t a, std::uint64_t b) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::crc32c<native::x86_feature::crc32>(a, b);
#else
  return native::crc32c(a, b);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("f16c")
std::uint16_t native_scalar_default_41(float a) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::cvtss_sh<native::x86_feature::f16c, 7>(a);
#else
  return native::cvtss_sh<7>(a);
#endif
}

extern "C" native_noinline __attribute__((used)) native_target("f16c")
float native_scalar_default_42(std::uint16_t a) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  return native::cvtsh_ss<native::x86_feature::f16c>(a);
#else
  return native::cvtsh_ss(a);
#endif
}

// A discarded result must still update MXCSR or raise an unmasked exception.
extern "C" native_noinline __attribute__((used)) native_target("f16c")
void native_scalar_default_discard(float a) noexcept {
#if NATIVE_SCALAR_EXPLICIT
  (void)native::cvtss_sh<native::x86_feature::f16c, 0>(a);
#else
  (void)native::cvtss_sh<0>(a);
#endif
}
