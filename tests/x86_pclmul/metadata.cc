// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/isa.h>
#include <native/targets.h>

namespace {
using native::x86_feature;
constexpr native::isa<native::x86> vpclmulqdq{x86_feature::vpclmulqdq};
constexpr native::isa<native::x86> pclmul{x86_feature::pclmul};
constexpr auto legacy = native::feature_closure(pclmul);
constexpr auto vex = native::target_features<native::x86>("avx,vpclmulqdq");
constexpr auto evex = native::target_features<native::x86>("avx512f,vpclmulqdq");

static_assert(static_cast<unsigned>(x86_feature::popcnt) == 7);
static_assert(static_cast<unsigned>(x86_feature::lzcnt) == 25);
static_assert(static_cast<unsigned>(x86_feature::waitpkg) == 29);
static_assert(static_cast<unsigned>(x86_feature::vpclmulqdq) > static_cast<unsigned>(x86_feature::waitpkg));
static_assert(native::x86_feature_count > static_cast<unsigned>(x86_feature::vpclmulqdq));
static_assert(std::same_as<std::remove_cv_t<decltype(vpclmulqdq)>,native::isa<native::x86>>);
static_assert(!vpclmulqdq.has(x86_feature::sse2));
static_assert(legacy == (pclmul & x86_feature::sse2 & x86_feature::sse & x86_feature::mmx));
static_assert(native::target_features<native::x86>("pclmul") == legacy);
static_assert(native::target_features<native::x86>("vpclmulqdq") == vex);
static_assert(vex.has(x86_feature::pclmul));
static_assert(vex.has(x86_feature::avx));
static_assert(!vex.has(x86_feature::avx2));
static_assert(!vex.has(x86_feature::avx512f));
static_assert(!native::avx2.has(x86_feature::vpclmulqdq));
static_assert(!native::avx512.has(x86_feature::vpclmulqdq));
#ifdef __AMX_TILE__
static_assert(!(NATIVE_TARGET_MINIMUM <= native::detail::known_features<native::x86>));
#else
static_assert(NATIVE_TARGET_MINIMUM <= native::detail::known_features<native::x86>);
#endif
static_assert(static_cast<unsigned>(x86_feature::crc32) == 30);
static_assert(static_cast<unsigned>(x86_feature::gfni) == 31);
static_assert(static_cast<unsigned>(x86_feature::avx512vpopcntdq) == 32);
static_assert(static_cast<unsigned>(x86_feature::vpclmulqdq) == 33);
static_assert(static_cast<unsigned>(native::arm_feature::i8mm) == 15);
#ifdef __PCLMUL__
static_assert(NATIVE_TARGET_MINIMUM.has(x86_feature::pclmul));
#else
static_assert(!NATIVE_TARGET_MINIMUM.has(x86_feature::pclmul));
#endif
#ifdef __VPCLMULQDQ__
static_assert(NATIVE_TARGET_MINIMUM.has(x86_feature::vpclmulqdq));
#else
static_assert(!NATIVE_TARGET_MINIMUM.has(x86_feature::vpclmulqdq));
#endif

consteval bool property_contract() {
  native::isa<native::x86> value;
  value.vpclmulqdq = true;
  if (!value.vpclmulqdq || value != vpclmulqdq) return false;
  value.vpclmulqdq = false;
  return value == native::isa<native::x86>{};
}
static_assert(property_contract());

struct normalized_snapshot {
  native::isa<native::x86> present{}, observed{};
  std::uint64_t xcr0 = 0;
  bool xcr0_observed = false;
};

constexpr normalized_snapshot available(native::isa<native::x86> requirements) {
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
  if (!native::classify_isa(cpu, pclmul).admitted()) return false;
  if (native::classify_isa(cpu, vex).admitted()) return false;
  cpu.present.set(x86_feature::pclmul, false);
  if (native::classify_isa(cpu, pclmul).admitted()) return false;
  cpu.present.set(x86_feature::pclmul, true);
  cpu.observed.set(x86_feature::pclmul, false);
  if (native::classify_isa(cpu, pclmul).admitted()) return false;

  cpu = available(vex);
  cpu.xcr0 = 0x6;
  if (native::classify_isa(cpu, vex).admitted()) return false;
  cpu.xcr0_observed = true;
  if (!native::classify_isa(cpu, vex).admitted()) return false;
  if (!native::classify_isa(cpu, native::isa<native::x86>(x86_feature::avx), vpclmulqdq).admitted()) return false;
  for (auto bit : {1u, 2u}) {
    cpu.xcr0 = 0x6 & ~(std::uint64_t{1} << bit);
    auto status = native::classify_isa(cpu, vex);
    if (status.admitted() || status.missing_xcr0 != (std::uint64_t{1} << bit)) return false;
  }

  cpu.xcr0 = 0x6;
  cpu.present.set(x86_feature::vpclmulqdq,false);
  if (native::classify_isa(cpu,vex).admitted()) return false;
  cpu.present.set(x86_feature::vpclmulqdq,true);
  cpu.observed.set(x86_feature::vpclmulqdq,false);
  if (native::classify_isa(cpu,vex).admitted()) return false;
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
  std::uint32_t leaf7_ecx = 1u << 10;
  std::uint64_t xcr0 = 0;
  bool xcr0_observed = false;
};

consteval bool raw_contract() {
  raw_snapshot cpu;
  // Presence is independent of missing AVX/XSAVE/OS state and of PCLMUL.
  auto features=native::detail::decode_x86_features(cpu);
  if (!features.present.has(x86_feature::vpclmulqdq) ||
      !features.observed.has(x86_feature::vpclmulqdq) ||
      features.present.has(x86_feature::pclmul)) return false;
  if (native::classify_isa(cpu,vpclmulqdq).admitted()) return false;
  cpu.max_basic_leaf=6;
  features=native::detail::decode_x86_features(cpu);
  if (features.present.has(x86_feature::vpclmulqdq) || features.observed.has(x86_feature::vpclmulqdq)) return false;
  cpu.max_basic_leaf=7;
  cpu.leaf7_ecx=(1u<<9)|(1u<<11);
  cpu.leaf7_ebx=1u<<10;
  if (native::detail::decode_x86_features(cpu).present.has(x86_feature::vpclmulqdq)) return false;
  cpu.leaf7_ecx=1u<<10;
  cpu.leaf7_ebx=0;
  cpu.leaf1_ecx=(1u<<0)|(1u<<1)|(1u<<9)|(1u<<19)|(1u<<20)|
    (1u<<23)|(1u<<26)|(1u<<27)|(1u<<28);
  cpu.xcr0=0x6;
  cpu.xcr0_observed=true;
  if (!native::classify_isa(cpu,vex).admitted()) return false;
  for (auto bit : {1u,26u,27u,28u}) {
    auto missing=cpu;
    missing.leaf1_ecx &= ~(1u<<bit);
    if (native::classify_isa(missing,vex).admitted()) return false;
  }
  cpu.xcr0_observed=false;
  return !native::classify_isa(cpu,vex).admitted();
}
static_assert(raw_contract());
}
