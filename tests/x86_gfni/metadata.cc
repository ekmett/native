// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/isa.h>
#include <native/targets.h>

namespace {
using native::x86_feature;
constexpr native::isa gfni{x86_feature::gfni};
constexpr auto legacy = native::feature_closure(gfni);
constexpr auto vex = native::target_features("avx,gfni");
constexpr auto evex = native::target_features("avx512f,gfni");

static_assert(static_cast<unsigned>(x86_feature::popcnt) == 7);
static_assert(static_cast<unsigned>(x86_feature::lzcnt) == 25);
static_assert(static_cast<unsigned>(x86_feature::waitpkg) == 29);
static_assert(static_cast<unsigned>(x86_feature::gfni) > static_cast<unsigned>(x86_feature::waitpkg));
static_assert(native::x86_feature_count > static_cast<unsigned>(x86_feature::gfni));
static_assert(gfni == native::isa(native::feature_set{x86_feature::gfni}));
static_assert(!gfni.has(x86_feature::sse2));
static_assert(legacy == (gfni & x86_feature::sse2 & x86_feature::sse & x86_feature::mmx));
static_assert(native::target_features("gfni") == legacy);
static_assert(vex.has(x86_feature::avx));
static_assert(!vex.has(x86_feature::avx2));
static_assert(!vex.has(x86_feature::avx512f));
static_assert(!native::avx2.has(x86_feature::gfni));
static_assert(!native::avx512.has(x86_feature::gfni));
static_assert(NATIVE_TARGET_MINIMUM <= native::detail::known_features);
#ifdef __GFNI__
static_assert(NATIVE_TARGET_MINIMUM.has(x86_feature::gfni));
#else
static_assert(!NATIVE_TARGET_MINIMUM.has(x86_feature::gfni));
#endif

consteval bool property_contract() {
  native::isa value;
  value.gfni = true;
  if (!value.gfni || value != gfni) return false;
  value.gfni = false;
  return value == native::scalar;
}
static_assert(property_contract());

struct normalized_snapshot {
  native::feature_set<x86_feature> present{}, observed{};
  std::uint64_t xcr0 = 0;
  bool xcr0_observed = false;
};

constexpr normalized_snapshot available(native::isa requirements) {
  normalized_snapshot result;
  for (unsigned i = 0; i != native::x86_feature_count; ++i) {
    auto feature = static_cast<x86_feature>(i);
    result.present.set(feature, requirements.has(feature));
    result.observed.set(feature, requirements.has(feature));
  }
  return result;
}

consteval bool normalized_contract() {
  auto cpu = available(legacy);
  // Legacy SSE uses the platform's XMM baseline; it does not require XSAVE.
  if (!native::classify_isa(cpu, gfni).admitted()) return false;
  if (native::classify_isa(cpu, vex).admitted()) return false;
  cpu.present.set(x86_feature::gfni, false);
  if (native::classify_isa(cpu, gfni).admitted()) return false;
  cpu.present.set(x86_feature::gfni, true);
  cpu.observed.set(x86_feature::gfni, false);
  if (native::classify_isa(cpu, gfni).admitted()) return false;

  cpu = available(vex);
  cpu.xcr0 = 0x6;
  if (native::classify_isa(cpu, vex).admitted()) return false;
  cpu.xcr0_observed = true;
  if (!native::classify_isa(cpu, vex).admitted()) return false;
  if (!native::classify_isa(cpu, native::isa(x86_feature::avx), gfni).admitted()) return false;
  for (auto bit : {1u, 2u}) {
    cpu.xcr0 = 0x6 & ~(std::uint64_t{1} << bit);
    auto status = native::classify_isa(cpu, vex);
    if (status.admitted() || status.missing_xcr0 != (std::uint64_t{1} << bit)) return false;
  }

  cpu = available(evex);
  cpu.xcr0_observed = true;
  cpu.xcr0 = 0xe6;
  if (!native::classify_isa(cpu, evex).admitted()) return false;
  for (auto bit : {1u, 2u, 5u, 6u, 7u}) {
    cpu.xcr0 = 0xe6 & ~(std::uint64_t{1} << bit);
    auto status = native::classify_isa(cpu, evex);
    if (status.admitted() || status.missing_xcr0 != (std::uint64_t{1} << bit)) return false;
  }
  return true;
}
static_assert(normalized_contract());

struct raw_snapshot {
  std::uint32_t max_basic_leaf = 7;
  std::uint32_t leaf1_ecx = 0;
  std::uint32_t leaf1_edx = (1u << 23) | (1u << 25) | (1u << 26);
  std::uint32_t leaf7_ebx = 0;
  std::uint32_t max_leaf7_subleaf = 0;
  std::uint32_t leaf7_1_eax = 0;
  std::uint32_t leaf7_edx = 0;
  std::uint32_t leaf7_ecx = 1u << 8;
  std::uint64_t xcr0 = 0;
  bool xcr0_observed = false;
};

consteval bool raw_contract() {
  raw_snapshot cpu;
  if (!native::classify_isa(cpu, gfni).admitted()) return false;
  cpu.max_basic_leaf = 6;
  if (native::classify_isa(cpu, gfni).admitted()) return false;
  cpu.max_basic_leaf = 7;
  cpu.leaf7_ecx = (1u << 5) | (1u << 9);
  cpu.leaf7_ebx = 1u << 8;
  if (native::classify_isa(cpu, gfni).admitted()) return false;
  cpu.leaf7_ecx = 1u << 8;
  cpu.leaf7_ebx = 0;
  // AVX compiler prerequisites and XSAVE/OSXSAVE; AVX2 remains absent.
  cpu.leaf1_ecx = (1u << 0) | (1u << 9) | (1u << 19) | (1u << 20) |
    (1u << 23) | (1u << 26) | (1u << 27) | (1u << 28);
  cpu.xcr0 = 0x6;
  cpu.xcr0_observed = true;
  if (!native::classify_isa(cpu, vex).admitted()) return false;
  for (auto bit : {26u, 27u, 28u}) {
    auto missing = cpu;
    missing.leaf1_ecx &= ~(1u << bit);
    if (native::classify_isa(missing, vex).admitted()) return false;
  }
  cpu.xcr0_observed = false;
  return !native::classify_isa(cpu, vex).admitted();
}
static_assert(raw_contract());
}
