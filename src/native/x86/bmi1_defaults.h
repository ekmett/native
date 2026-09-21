// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/targets.h"

// Include only in this operation module's global module fragment, after its
// public header. Defaults belong to the provider, never to a textual consumer.
#if NATIVE_HOST_X86
namespace native {
  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi1))
  std::uint32_t andn(std::uint32_t first, std::uint32_t second) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi1))
  std::uint64_t andn(std::uint64_t first, std::uint64_t second) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi1))
  std::uint32_t bextr(std::uint32_t value, std::uint32_t control) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi1))
  std::uint64_t bextr(std::uint64_t value, std::uint32_t control) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi1))
  std::uint32_t bextr(std::uint32_t value, unsigned start, unsigned length) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi1))
  std::uint64_t bextr(std::uint64_t value, unsigned start, unsigned length) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi1))
  std::uint32_t blsi(std::uint32_t value) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi1))
  std::uint64_t blsi(std::uint64_t value) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi1))
  std::uint32_t blsmsk(std::uint32_t value) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi1))
  std::uint64_t blsmsk(std::uint64_t value) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi1))
  std::uint32_t blsr(std::uint32_t value) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi1))
  std::uint64_t blsr(std::uint64_t value) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi1))
  std::uint16_t tzcnt(std::uint16_t value) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi1))
  std::uint32_t tzcnt(std::uint32_t value) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::bmi1))
  std::uint64_t tzcnt(std::uint64_t value) noexcept;

}
#endif
