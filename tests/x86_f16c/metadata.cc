// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/isa.h>
#include <native/targets.h>

namespace {
using native::x86_feature;
constexpr native::isa f16c{x86_feature::f16c};
constexpr auto closed = native::feature_closure(f16c);
static_assert(unsigned(x86_feature::f16c) == 11);
static_assert(unsigned(x86_feature::crc32) == 30);
static_assert(unsigned(x86_feature::gfni) == 31);
static_assert(unsigned(x86_feature::avx512vpopcntdq) == 32);
static_assert(f16c == native::isa(native::feature_set{x86_feature::f16c}));
static_assert(!f16c.has(x86_feature::avx));
static_assert(closed == (native::feature_closure(native::isa(x86_feature::avx)) & f16c));
static_assert(native::target_features("f16c") == closed);
static_assert(!closed.has(x86_feature::avx2) && !closed.has(x86_feature::avx512fp16));
static_assert(NATIVE_TARGET_MINIMUM <= native::detail::known_features);
#ifdef __F16C__
static_assert(NATIVE_TARGET_MINIMUM.has(x86_feature::f16c));
static_assert(NATIVE_TARGET_MINIMUM.has(x86_feature::avx));
#else
static_assert(!NATIVE_TARGET_MINIMUM.has(x86_feature::f16c));
#endif
static_assert([] {
  native::isa value;
  value.f16c = true;
  if (!value.f16c || value != f16c) return false;
  value.f16c = false;
  return value == native::scalar;
}());
struct normalized_snapshot {
  native::feature_set<x86_feature> present{}, observed{};
  std::uint64_t xcr0 = 6;
  bool xcr0_observed = true;
};
static_assert([] {
  normalized_snapshot cpu;
  for (unsigned i = 0; i != native::x86_feature_count; ++i) {
    auto f = static_cast<x86_feature>(i);
    cpu.present.set(f, closed.has(f)); cpu.observed.set(f, closed.has(f));
  }
  if (!native::classify_isa(cpu, f16c).admitted()) return false;
  for (auto feature : {x86_feature::f16c, x86_feature::avx}) {
    auto absent = cpu; absent.present.set(feature, false);
    if (native::classify_isa(absent, f16c).admitted()) return false;
    auto unobserved = cpu; unobserved.observed.set(feature, false);
    if (native::classify_isa(unobserved, f16c).admitted()) return false;
  }
  for (unsigned bit : {1u, 2u}) {
    auto missing = cpu; missing.xcr0 &= ~(1ull << bit);
    auto status = native::classify_isa(missing, f16c);
    if (status.admitted() || status.missing_xcr0 != (1ull << bit)) return false;
    // CPU presence survives missing OS state.
    if (!missing.present.has(x86_feature::f16c)) return false;
  }
  auto unknown = f16c; unknown.flags.back() |= 1ull << 63;
  if (native::classify_isa(cpu, unknown).admitted()) return false;
  cpu.xcr0_observed = false;
  return !native::classify_isa(cpu, f16c).admitted();
}());
struct raw_snapshot {
  std::uint32_t max_basic_leaf = 1;
  std::uint32_t leaf1_ecx = (1u << 0) | (1u << 9) | (1u << 19) | (1u << 20) |
    (1u << 23) | (1u << 26) | (1u << 27) | (1u << 28) | (1u << 29);
  std::uint32_t leaf1_edx = (1u << 23) | (1u << 25) | (1u << 26);
  std::uint32_t leaf7_ebx = 0, max_leaf7_subleaf = 0, leaf7_1_eax = 0, leaf7_edx = 0;
  std::uint64_t xcr0 = 6;
  bool xcr0_observed = true;
};
static_assert([] {
  raw_snapshot cpu;
  if (!native::classify_isa(cpu, f16c).admitted()) return false;
  for (unsigned bit : {26u, 27u, 28u, 29u}) {
    auto missing = cpu; missing.leaf1_ecx &= ~(1u << bit);
    if (native::classify_isa(missing, f16c).admitted()) return false;
  }
  cpu.max_basic_leaf = 0;
  return !native::classify_isa(cpu, f16c).admitted();
}());
}
