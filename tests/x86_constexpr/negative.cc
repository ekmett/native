// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <native/attributes.h>
#if NATIVE_CONSTEXPR_INTERFACE == 0
#include <native/x86/bmi1.h>
#include <native/x86/bmi2.h>
#include <native/x86/popcnt.h>
#include <native/x86/lzcnt.h>
#include <native/x86/crc32c.h>
#else
import native.x86.bmi1;
import native.x86.bmi2;
import native.x86.popcnt;
import native.x86.lzcnt;
import native.x86.crc32c;
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 0
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi")
std::uint32_t rejected(std::uint32_t a, std::uint32_t b) {
  return native::andn<native::isa<native::x86>{}>(a, b);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 1
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi")
std::uint32_t rejected(std::uint32_t a, std::uint32_t control) {
  return native::bextr<native::isa<native::x86>{}>(a, control);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 2
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi")
std::uint32_t rejected(std::uint32_t a, unsigned start, unsigned length) {
  return native::bextr<native::isa<native::x86>{}>(a, start, length);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 3
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi")
std::uint32_t rejected(std::uint32_t a) {
  return native::blsi<native::isa<native::x86>{}>(a);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 4
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi")
std::uint32_t rejected(std::uint32_t a) {
  return native::blsmsk<native::isa<native::x86>{}>(a);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 5
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi")
std::uint32_t rejected(std::uint32_t a) {
  return native::blsr<native::isa<native::x86>{}>(a);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 6
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi2")
std::uint32_t rejected(std::uint32_t a, unsigned count) {
  return native::bzhi<native::isa<native::x86>{}>(a, count);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 7
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi2")
std::uint32_t rejected(std::uint32_t a, std::uint32_t b, std::uint32_t* high) {
  return native::mulx<native::isa<native::x86>{}>(a, b, high);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 8
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi2")
std::uint32_t rejected(std::uint32_t a, std::uint32_t mask) {
  return native::pdep<native::isa<native::x86>{}>(a, mask);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 9
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi2")
std::uint32_t rejected(std::uint32_t a, std::uint32_t mask) {
  return native::pext<native::isa<native::x86>{}>(a, mask);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 10
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi2")
std::uint32_t rejected(std::uint32_t a, unsigned count) {
  return native::shlx<native::isa<native::x86>{}>(a, count);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 11
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi2")
std::uint32_t rejected(std::uint32_t a, unsigned count) {
  return native::shrx<native::isa<native::x86>{}>(a, count);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 12
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi2")
std::uint32_t rejected(std::uint32_t a) {
  return native::rorx<native::isa<native::x86>{}, 7>(a);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 13
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi")
std::uint64_t rejected(std::uint64_t a, std::uint64_t b) {
  return native::andn<native::isa<native::x86>{}>(a, b);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 14
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi")
std::uint64_t rejected(std::uint64_t a, std::uint32_t control) {
  return native::bextr<native::isa<native::x86>{}>(a, control);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 15
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi")
std::uint64_t rejected(std::uint64_t a, unsigned start, unsigned length) {
  return native::bextr<native::isa<native::x86>{}>(a, start, length);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 16
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi")
std::uint64_t rejected(std::uint64_t a) {
  return native::blsi<native::isa<native::x86>{}>(a);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 17
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi")
std::uint64_t rejected(std::uint64_t a) {
  return native::blsmsk<native::isa<native::x86>{}>(a);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 18
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi")
std::uint64_t rejected(std::uint64_t a) {
  return native::blsr<native::isa<native::x86>{}>(a);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 19
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi2")
std::uint64_t rejected(std::uint64_t a, unsigned count) {
  return native::bzhi<native::isa<native::x86>{}>(a, count);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 20
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi2")
std::uint64_t rejected(std::uint64_t a, std::uint64_t b, std::uint64_t* high) {
  return native::mulx<native::isa<native::x86>{}>(a, b, high);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 21
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi2")
std::uint64_t rejected(std::uint64_t a, std::uint64_t mask) {
  return native::pdep<native::isa<native::x86>{}>(a, mask);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 22
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi2")
std::uint64_t rejected(std::uint64_t a, std::uint64_t mask) {
  return native::pext<native::isa<native::x86>{}>(a, mask);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 23
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi2")
std::uint64_t rejected(std::uint64_t a, unsigned count) {
  return native::shlx<native::isa<native::x86>{}>(a, count);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 24
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi2")
std::uint64_t rejected(std::uint64_t a, unsigned count) {
  return native::shrx<native::isa<native::x86>{}>(a, count);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 25
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi2")
std::uint64_t rejected(std::uint64_t a) {
  return native::rorx<native::isa<native::x86>{}, 7>(a);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 26
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi2")
std::int32_t rejected(std::int32_t a, unsigned count) {
  return native::sarx<native::isa<native::x86>{}>(a, count);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 27
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi2")
std::int64_t rejected(std::int64_t a, unsigned count) {
  return native::sarx<native::isa<native::x86>{}>(a, count);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 28
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi")
std::uint16_t rejected(std::uint16_t a) {
  return native::tzcnt<native::isa<native::x86>{}>(a);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 29
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi")
std::uint32_t rejected(std::uint32_t a) {
  return native::tzcnt<native::isa<native::x86>{}>(a);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 30
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("bmi")
std::uint64_t rejected(std::uint64_t a) {
  return native::tzcnt<native::isa<native::x86>{}>(a);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 31
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("popcnt")
std::uint16_t rejected(std::uint16_t a) {
  return native::popcnt<native::isa<native::x86>{}>(a);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 32
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("popcnt")
std::uint32_t rejected(std::uint32_t a) {
  return native::popcnt<native::isa<native::x86>{}>(a);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 33
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("popcnt")
std::uint64_t rejected(std::uint64_t a) {
  return native::popcnt<native::isa<native::x86>{}>(a);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 34
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("lzcnt")
std::uint16_t rejected(std::uint16_t a) {
  return native::lzcnt<native::isa<native::x86>{}>(a);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 35
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("lzcnt")
std::uint32_t rejected(std::uint32_t a) {
  return native::lzcnt<native::isa<native::x86>{}>(a);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 36
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("lzcnt")
std::uint64_t rejected(std::uint64_t a) {
  return native::lzcnt<native::isa<native::x86>{}>(a);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 37
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("crc32")
std::uint32_t rejected(std::uint32_t a, std::uint8_t b) {
  return native::crc32c<native::isa<native::x86>{}>(a, b);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 38
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("crc32")
std::uint32_t rejected(std::uint32_t a, std::uint16_t b) {
  return native::crc32c<native::isa<native::x86>{}>(a, b);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 39
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("crc32")
std::uint32_t rejected(std::uint32_t a, std::uint32_t b) {
  return native::crc32c<native::isa<native::x86>{}>(a, b);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 40
// A matching compiler target cannot grant runtime permission to a weak tag.
native_target("crc32")
std::uint32_t rejected(std::uint32_t a, std::uint64_t b) {
  return native::crc32c<native::isa<native::x86>{}>(a, b);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 41
// AVX2 profile bits cannot grant BMI2 runtime permission, even in a BMI2 target.
native_target("bmi2")
std::uint32_t rejected(std::uint32_t value, std::uint32_t mask) {
  return native::pdep<native::avx2>(value, mask);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 42
// AVX2 profile bits cannot grant BMI2 runtime permission, even in a BMI2 target.
native_target("bmi2")
std::uint64_t rejected(std::uint64_t value, std::uint64_t mask) {
  return native::pdep<native::avx2>(value, mask);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 43
// AVX2 profile bits cannot grant BMI2 runtime permission, even in a BMI2 target.
native_target("bmi2")
std::uint32_t rejected(std::uint32_t value, std::uint32_t mask) {
  return native::pext<native::avx2>(value, mask);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 44
// AVX2 profile bits cannot grant BMI2 runtime permission, even in a BMI2 target.
native_target("bmi2")
std::uint64_t rejected(std::uint64_t value, std::uint64_t mask) {
  return native::pext<native::avx2>(value, mask);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 45
// AVX2 profile bits cannot grant BMI2 runtime permission, even in a BMI2 target.
native_target("bmi2")
std::uint32_t rejected(std::uint32_t value, std::uint32_t mask) {
  return native::pdep<native::isa<native::x86>(native::x86_feature::avx2)>(value, mask);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 46
// AVX2 profile bits cannot grant BMI2 runtime permission, even in a BMI2 target.
native_target("bmi2")
std::uint64_t rejected(std::uint64_t value, std::uint64_t mask) {
  return native::pdep<native::isa<native::x86>(native::x86_feature::avx2)>(value, mask);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 47
// AVX2 profile bits cannot grant BMI2 runtime permission, even in a BMI2 target.
native_target("bmi2")
std::uint32_t rejected(std::uint32_t value, std::uint32_t mask) {
  return native::pext<native::isa<native::x86>(native::x86_feature::avx2)>(value, mask);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 48
// AVX2 profile bits cannot grant BMI2 runtime permission, even in a BMI2 target.
native_target("bmi2")
std::uint64_t rejected(std::uint64_t value, std::uint64_t mask) {
  return native::pext<native::isa<native::x86>(native::x86_feature::avx2)>(value, mask);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 49
// Empty feature sets still belong to their declared architecture family.
std::uint32_t rejected(std::uint32_t value) {
  return native::andn<native::isa<native::arm>{}>(value, value);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 50
// Empty feature sets still belong to their declared architecture family.
std::uint32_t rejected(std::uint32_t value) {
  return native::pdep<native::isa<native::arm>{}>(value, value);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 51
// Empty feature sets still belong to their declared architecture family.
std::uint32_t rejected(std::uint32_t value) {
  return native::popcnt<native::isa<native::arm>{}>(value);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 52
// Empty feature sets still belong to their declared architecture family.
std::uint32_t rejected(std::uint32_t value) {
  return native::lzcnt<native::isa<native::arm>{}>(value);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 53
// Empty feature sets still belong to their declared architecture family.
std::uint32_t rejected(std::uint32_t value) {
  return native::crc32c<native::isa<native::arm>{}>(value, value);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 54
// Empty feature sets still belong to their declared architecture family.
std::uint32_t rejected(std::uint32_t value) {
  return native::andn<native::isa<native::wasm>{}>(value, value);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 55
// Empty feature sets still belong to their declared architecture family.
std::uint32_t rejected(std::uint32_t value) {
  return native::pdep<native::isa<native::wasm>{}>(value, value);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 56
// Empty feature sets still belong to their declared architecture family.
std::uint32_t rejected(std::uint32_t value) {
  return native::popcnt<native::isa<native::wasm>{}>(value);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 57
// Empty feature sets still belong to their declared architecture family.
std::uint32_t rejected(std::uint32_t value) {
  return native::lzcnt<native::isa<native::wasm>{}>(value);
}
#endif

#if NATIVE_CONSTEXPR_NEGATIVE == 58
// Empty feature sets still belong to their declared architecture family.
std::uint32_t rejected(std::uint32_t value) {
  return native::crc32c<native::isa<native::wasm>{}>(value, value);
}
#endif
