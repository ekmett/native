// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <native/isa.h>
#include <native/targets.h>

namespace {
  using native::x86_feature;
  constexpr auto requirements = native::target_features<native::x86>("adx");
  static_assert(requirements == native::isa<native::x86>(x86_feature::adx));
  static_assert(static_cast<unsigned>(x86_feature::avxifma) == 43);
  static_assert(static_cast<unsigned>(x86_feature::adx) == 44);
  static_assert(native::x86_feature_count > 44);
  static_assert(!native::avx2.has(x86_feature::adx) && !native::avx512.has(x86_feature::adx));
  static_assert(native::target_features<native::x86>("no-adx") == native::detail::invalid_features<native::x86>);
#ifdef NATIVE_TEST_FEATURE_MINIMUM
  static_assert(NATIVE_BASELINE.has(x86_feature::adx));
  static_assert(requirements <= NATIVE_TARGET_MINIMUM);
#else
  static_assert(!NATIVE_BASELINE.has(x86_feature::adx));
#endif
  static_assert([] {
    native::isa<native::x86> value{};
    value.adx = true;
    if (!value.adx || value != requirements) {
      return false;
    }
    value.adx = false;
    return value == native::isa<native::x86>{};
  }());

  struct snapshot {
    std::uint32_t max_basic_leaf = 7;
    std::uint32_t leaf1_ecx = 0;
    std::uint32_t leaf1_edx = 0;
    std::uint32_t leaf7_ebx = 1u << 19;
    std::uint32_t leaf7_ecx = 0;
    std::uint32_t leaf7_edx = 0;
    std::uint32_t max_leaf7_subleaf = 0;
    std::uint32_t leaf7_1_eax = 0;
    std::uint64_t xcr0 = 0;
    bool xcr0_observed = false;
  };

  constexpr bool admission() {
    for (unsigned bit = 0; bit < 32; ++bit) {
      snapshot cpu;
      cpu.leaf7_ebx = 1u << bit;
      auto decoded = native::detail::decode_x86_features(cpu);
      if (decoded.present.has(x86_feature::adx) != (bit == 19) ||
          !decoded.observed.has(x86_feature::adx) ||
          native::classify_isa(cpu, requirements).admitted() != (bit == 19)) {
        return false;
      }
    }
    snapshot cpu;
    cpu.max_basic_leaf = 1;
    return !native::detail::decode_x86_features(cpu).observed.has(x86_feature::adx) &&
      !native::classify_isa(cpu, requirements).admitted();
  }
  static_assert(admission());
}

extern "C" bool native_adx_admission(std::uint32_t feature_bits) noexcept {
  snapshot cpu;
  cpu.leaf7_ebx = feature_bits;
  return native::classify_isa(cpu, requirements).admitted();
}

int main() {
  return admission() ? 0 : 1;
}
