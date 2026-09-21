// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <native/isa.h>
#include <native/targets.h>

namespace {
  using native::x86_feature;
  constexpr auto vex128 = native::target_features<native::x86>("avx,aes");
  constexpr auto hardware = native::feature_closure(x86_feature::vaes);
  constexpr auto vex256 = native::target_features<native::x86>("avx,vaes");
  constexpr auto evex512 = native::target_features<native::x86>("avx512f,vaes");
  static_assert(static_cast<unsigned>(x86_feature::vaes) == 42);
  static_assert(hardware.has(x86_feature::avx) && !hardware.has(x86_feature::avx2));
  static_assert(!hardware.has(x86_feature::aes));
  static_assert(vex256 == native::feature_closure(hardware & x86_feature::avx2 & x86_feature::aes));
  static_assert(!vex128.has(x86_feature::vaes));
  static_assert(!evex512.has(x86_feature::avx512vl) && !evex512.has(x86_feature::avx512bw));
  static_assert(!native::avx512.has(x86_feature::vaes));
#ifdef NATIVE_TEST_FEATURE_MINIMUM
  static_assert(NATIVE_BASELINE.has(x86_feature::vaes));
  static_assert(vex256 <= NATIVE_TARGET_MINIMUM);
#else
  static_assert(!NATIVE_BASELINE.has(x86_feature::vaes));
#endif

  constexpr bool property() {
    native::isa<native::x86> value{};
    value.vaes = true;
    if (!value.vaes || value != native::isa<native::x86>(x86_feature::vaes)) {
      return false;
    }
    value.vaes = false;
    return value == native::isa<native::x86>{};
  }
  static_assert(property());

  struct snapshot {
    std::uint32_t max_basic_leaf = 7;
    std::uint32_t leaf1_ecx = (1u << 0) | (1u << 9) | (1u << 12) | (1u << 19) |
      (1u << 20) | (1u << 23) | (1u << 25) | (1u << 26) | (1u << 27) | (1u << 28) | (1u << 29);
    std::uint32_t leaf1_edx = (1u << 23) | (1u << 25) | (1u << 26);
    std::uint32_t leaf7_ebx = (1u << 5) | (1u << 16);
    std::uint32_t leaf7_ecx = 1u << 9;
    std::uint32_t leaf7_edx = 0;
    std::uint32_t max_leaf7_subleaf = 0;
    std::uint32_t leaf7_1_eax = 0;
    std::uint64_t xcr0 = 0xe6;
    bool xcr0_observed = true;
  };

  constexpr bool state_admission(native::isa<native::x86> requirements) {
    if (!native::classify_isa(snapshot{}, requirements).admitted()) {
      return false;
    }
    auto state = requirements.has(x86_feature::avx512f) ? 0xe6u : 6u;
    for (unsigned bit = 0; bit < 64; ++bit) {
      snapshot cpu;
      cpu.xcr0 &= ~(std::uint64_t{1} << bit);
      auto result = native::classify_isa(cpu, requirements);
      if (result.admitted() != ((state & (std::uint64_t{1} << bit)) == 0)) {
        return false;
      }
    }
    snapshot cpu;
    cpu.xcr0_observed = false;
    return !native::classify_isa(cpu, requirements).admitted();
  }
  // Keep each scenario within Clang's default constant-evaluation budget.
  static_assert(state_admission(hardware));
  static_assert(state_admission(vex128));
  static_assert(state_admission(vex256));
  static_assert(state_admission(evex512));

  constexpr bool feature_admission() {
    for (unsigned bit = 0; bit < 32; ++bit) {
      snapshot cpu;
      cpu.leaf7_ecx = 1u << bit;
      auto decoded = native::detail::decode_x86_features(cpu);
      if (decoded.present.has(x86_feature::vaes) != (bit == 9) ||
          !decoded.observed.has(x86_feature::vaes) ||
          native::classify_isa(cpu, vex256).admitted() != (bit == 9)) {
        return false;
      }
    }
    snapshot cpu;
    cpu.leaf1_ecx &= ~(1u << 25);
    cpu.leaf7_ebx &= ~(1u << 5);
    if (!native::classify_isa(cpu, hardware).admitted() || native::classify_isa(cpu, vex256).admitted()) {
      return false;
    }
    cpu = {};
    cpu.max_basic_leaf = 1;
    return !native::detail::decode_x86_features(cpu).observed.has(x86_feature::vaes) &&
      !native::classify_isa(cpu, vex256).admitted();
  }
  static_assert(feature_admission());
}

extern "C" bool native_vaes_admission(std::uint32_t feature_bits, std::uint64_t state) noexcept {
  snapshot cpu;
  cpu.leaf7_ecx = feature_bits;
  cpu.xcr0 = state;
  return native::classify_isa(cpu, evex512).admitted();
}

int main() {
  return state_admission(hardware) && state_admission(vex128) &&
    state_admission(vex256) && state_admission(evex512) && feature_admission() ? 0 : 1;
}
