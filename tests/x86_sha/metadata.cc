// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <native/isa.h>
#include <native/targets.h>

namespace {
  using native::x86_feature;
  constexpr auto requirements = native::target_features<native::x86>("sha");
  static_assert(static_cast<unsigned>(x86_feature::sha) == 41);
  static_assert(static_cast<unsigned>(x86_feature::vaes) == 42);
  static_assert(native::x86_feature_count > 42);
  static_assert(requirements == native::feature_closure(x86_feature::sha));
  static_assert(requirements.has(x86_feature::sse2));
  static_assert(!requirements.has(x86_feature::ssse3) && !requirements.has(x86_feature::aes));
  static_assert(!requirements.has(x86_feature::avx));
  static_assert(native::target_features<native::x86>("no-sha") == native::detail::invalid_features<native::x86>);
  static_assert(!native::avx512.has(x86_feature::sha));
#ifdef NATIVE_TEST_FEATURE_MINIMUM
  static_assert(NATIVE_BASELINE.has(x86_feature::sha));
  static_assert(requirements <= NATIVE_TARGET_MINIMUM);
#else
  static_assert(!NATIVE_BASELINE.has(x86_feature::sha));
#endif

  constexpr bool property() {
    native::isa<native::x86> value{};
    value.sha = true;
    if (!value.sha || value != native::isa<native::x86>(x86_feature::sha)) {
      return false;
    }
    value.sha = false;
    return value == native::isa<native::x86>{};
  }
  static_assert(property());

  struct snapshot {
    std::uint32_t max_basic_leaf = 7;
    std::uint32_t leaf1_ecx = 0;
    std::uint32_t leaf1_edx = (1u << 23) | (1u << 25) | (1u << 26);
    std::uint32_t leaf7_ebx = 1u << 29;
    std::uint32_t leaf7_ecx = 0;
    std::uint32_t leaf7_edx = 0;
    std::uint32_t max_leaf7_subleaf = 0;
    std::uint32_t leaf7_1_eax = 0;
    std::uint64_t xcr0 = 0;
    bool xcr0_observed = false;
  };

  constexpr bool admission() {
    if (!native::classify_isa(snapshot{}, requirements).admitted()) {
      return false;
    }
    for (unsigned bit = 0; bit < 32; ++bit) {
      snapshot cpu;
      cpu.leaf7_ebx = 1u << bit;
      auto decoded = native::detail::decode_x86_features(cpu);
      if (decoded.present.has(x86_feature::sha) != (bit == 29) ||
          !decoded.observed.has(x86_feature::sha) ||
          native::classify_isa(cpu, requirements).admitted() != (bit == 29)) {
        return false;
      }
    }
    snapshot cpu;
    cpu.max_basic_leaf = 1;
    if (native::detail::decode_x86_features(cpu).observed.has(x86_feature::sha) ||
        native::classify_isa(cpu, requirements).admitted()) {
      return false;
    }
    cpu = {};
    cpu.leaf1_edx &= ~(1u << 26);
    return !native::classify_isa(cpu, requirements).admitted();
  }
  static_assert(admission());
}

extern "C" bool native_sha_admission(std::uint32_t sha_bits, std::uint32_t storage_bits) noexcept {
  snapshot cpu;
  cpu.leaf7_ebx = sha_bits;
  cpu.leaf1_edx = storage_bits;
  return native::classify_isa(cpu, requirements).admitted();
}

int main() {
  return admission() ? 0 : 1;
}
