// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/x86/bmi1.h>
#include <native/x86/bmi2.h>
#include <native/x86/popcnt.h>
#include <native/x86/lzcnt.h>
#include <native/x86/crc32c.h>

// Even an enabled textual consumer supplies its architecture explicitly.
template<class T> concept default_bmi1 = requires(T a) { native::andn(a, a); };
template<class T> concept default_bmi2 = requires(T a) { native::bzhi(a, 7); };
template<class T> concept default_popcnt = requires(T a) { native::popcnt(a); };
template<class T> concept default_lzcnt = requires(T a) { native::lzcnt(a); };
template<class T> concept default_crc32 = requires(T a) { native::crc32c(a, a); };
static_assert(!default_bmi1<std::uint32_t> && !default_bmi2<std::uint32_t>);
static_assert(!default_popcnt<std::uint32_t> && !default_lzcnt<std::uint32_t>);
static_assert(!default_crc32<std::uint32_t>);
static_assert(requires(std::uint32_t a) {
  native::andn<native::x86_feature::bmi1>(a, a);
  native::bzhi<native::x86_feature::bmi2>(a, 7);
  native::rorx<native::x86_feature::bmi2, 7>(a);
  native::popcnt<native::x86_feature::popcnt>(a);
  native::lzcnt<native::x86_feature::lzcnt>(a);
  native::crc32c<native::x86_feature::crc32>(a, a);
});
