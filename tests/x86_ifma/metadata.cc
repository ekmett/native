// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <initializer_list>
#include <native/isa.h>
#include <native/targets.h>

namespace {
  using native::x86_feature;
  constexpr auto hardware = native::feature_closure(x86_feature::avxifma);
  constexpr auto vex = native::target_features<native::x86>("avxifma");
  constexpr auto evex = native::target_features<native::x86>("avx512ifma");
  constexpr auto evex_vl = native::target_features<native::x86>("avx512ifma,avx512vl");
  static_assert(hardware.has(x86_feature::avx) && !hardware.has(x86_feature::avx2));
  static_assert(vex == native::feature_closure(hardware & x86_feature::avx2));
  static_assert(!vex.has(x86_feature::avx512f) && !vex.has(x86_feature::fma));
  static_assert(evex.has(x86_feature::avx512f) && !evex.has(x86_feature::avx512vl));
  static_assert(!evex.has(x86_feature::avx512dq) && !evex.has(x86_feature::avx512bw));
  static_assert(!native::avx512.has(x86_feature::avx512ifma));
  static_assert(static_cast<unsigned>(x86_feature::avx512ifma) == 24);
  static_assert(static_cast<unsigned>(x86_feature::avxifma) == 43);
  static_assert(native::target_features<native::x86>("no-avxifma") == native::detail::invalid_features<native::x86>);
#if NATIVE_TEST_FEATURE_MINIMUM == 1
  static_assert(vex <= NATIVE_TARGET_MINIMUM);
#elif NATIVE_TEST_FEATURE_MINIMUM == 2
  static_assert(evex <= NATIVE_TARGET_MINIMUM);
#elif NATIVE_TEST_FEATURE_MINIMUM == 3
  static_assert(evex_vl <= NATIVE_TARGET_MINIMUM);
#else
  static_assert(!NATIVE_BASELINE.has(x86_feature::avxifma));
  static_assert(!NATIVE_BASELINE.has(x86_feature::avx512ifma));
#endif
  static_assert([] {
    native::isa<native::x86> value{};
    value.avxifma = true;
    if (!value.avxifma || value != native::isa<native::x86>(x86_feature::avxifma)) {
      return false;
    }
    value.avxifma = false;
    return value == native::isa<native::x86>{};
  }());

  struct snapshot {
    std::uint32_t max_basic_leaf = 7;
    std::uint32_t leaf1_ecx = (1u << 0) | (1u << 9) | (1u << 12) | (1u << 19) |
      (1u << 20) | (1u << 23) | (1u << 26) | (1u << 27) | (1u << 28) | (1u << 29);
    std::uint32_t leaf1_edx = (1u << 23) | (1u << 25) | (1u << 26);
    std::uint32_t leaf7_ebx = (1u << 5) | (1u << 16) | (1u << 21) | (1u << 31);
    std::uint32_t leaf7_ecx = 0;
    std::uint32_t leaf7_edx = 0;
    std::uint32_t max_leaf7_subleaf = 1;
    std::uint32_t leaf7_1_eax = 1u << 23;
    std::uint64_t xcr0 = 0xe6;
    bool xcr0_observed = true;
  };

  constexpr bool admission() {
    for (auto requirements : {hardware, vex, evex, evex_vl}) {
      if (!native::classify_isa(snapshot{}, requirements).admitted()) {
        return false;
      }
      auto state = requirements.has(x86_feature::avx512f) ? 0xe6u : 6u;
      for (unsigned bit = 0; bit < 64; ++bit) {
        snapshot cpu;
        cpu.xcr0 &= ~(std::uint64_t{1} << bit);
        if (native::classify_isa(cpu, requirements).admitted() !=
            ((state & (std::uint64_t{1} << bit)) == 0)) {
          return false;
        }
      }
      snapshot cpu;
      cpu.xcr0_observed = false;
      if (native::classify_isa(cpu, requirements).admitted()) {
        return false;
      }
    }
    for (unsigned bit = 0; bit < 32; ++bit) {
      snapshot cpu;
      cpu.leaf7_1_eax = 1u << bit;
      auto decoded = native::detail::decode_x86_features(cpu);
      if (decoded.present.has(x86_feature::avxifma) != (bit == 23) ||
          !decoded.observed.has(x86_feature::avxifma) ||
          native::classify_isa(cpu, vex).admitted() != (bit == 23)) {
        return false;
      }
    }
    snapshot cpu;
    cpu.max_leaf7_subleaf = 0;
    if (native::detail::decode_x86_features(cpu).observed.has(x86_feature::avxifma) ||
        native::classify_isa(cpu, vex).admitted()) {
      return false;
    }
    cpu = {};
    cpu.leaf7_ebx &= ~(1u << 5);
    return native::classify_isa(cpu, hardware).admitted() &&
      !native::classify_isa(cpu, vex).admitted();
  }
  static_assert(admission());
}

extern "C" bool native_ifma_admission(std::uint32_t subleaf, std::uint32_t bits, std::uint64_t state) noexcept {
  snapshot cpu;
  cpu.max_leaf7_subleaf = subleaf;
  cpu.leaf7_1_eax = bits;
  cpu.xcr0 = state;
  return native::classify_isa(cpu, vex).admitted();
}

int main() {
  return admission() ? 0 : 1;
}
