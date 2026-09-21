// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/targets.h"

// Include only in this operation module's global module fragment, after its
// public header. Defaults belong to the provider, never to a textual consumer.
#if NATIVE_HOST_X86
namespace native {
  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi2))
  std::uint32_t bzhi(std::uint32_t value, unsigned index) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi2))
  std::uint64_t bzhi(std::uint64_t value, unsigned index) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi2))
  std::uint32_t mulx(std::uint32_t a, std::uint32_t b, std::uint32_t* high) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi2))
  std::uint64_t mulx(std::uint64_t a, std::uint64_t b, std::uint64_t* high) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi2))
  std::uint32_t pdep(std::uint32_t value, std::uint32_t mask) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi2))
  std::uint64_t pdep(std::uint64_t value, std::uint64_t mask) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi2))
  std::uint32_t pext(std::uint32_t value, std::uint32_t mask) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi2))
  std::uint64_t pext(std::uint64_t value, std::uint64_t mask) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi2))
  std::uint32_t shlx(std::uint32_t value, unsigned count) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi2))
  std::uint64_t shlx(std::uint64_t value, unsigned count) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi2))
  std::uint32_t shrx(std::uint32_t value, unsigned count) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi2))
  std::uint64_t shrx(std::uint64_t value, unsigned count) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi2))
  std::int32_t sarx(std::int32_t value, unsigned count) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi2))
  std::int64_t sarx(std::int64_t value, unsigned count) noexcept;

  /// Rotate using the module baseline, or an explicitly supplied ISA.
  /// The legacy rorx<Arch, Imm8> spelling remains available.
  template<unsigned Imm8, isa Arch = NATIVE_BASELINE>
    requires(Arch.has(x86_feature::bmi2) && Imm8 <= 255)
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  std::uint32_t rorx(std::uint32_t value) noexcept {
    return rorx<Arch, Imm8>(value);
  }

  /// Rotate using the module baseline, or an explicitly supplied ISA.
  /// The legacy rorx<Arch, Imm8> spelling remains available.
  template<unsigned Imm8, isa Arch = NATIVE_BASELINE>
    requires(Arch.has(x86_feature::bmi2) && Imm8 <= 255)
  native_nodiscard native_inline native_const __attribute__((target("bmi2")))
  std::uint64_t rorx(std::uint64_t value) noexcept {
    return rorx<Arch, Imm8>(value);
  }

}
#endif
